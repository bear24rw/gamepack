#include <msp430.h>
#include <stdint.h>
#include "GP.h"

uint8_t gp_cur_spr = 0;    // current sprite, incremented by xsprite/xhide

uint8_t nes_data_1 = 0;
uint8_t nes_data_2 = 0;

void GP_begin(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    // put FPGA_CS pin in high Z state immediately
    // it's used for FPGA init
    P_FPGA_CS_DIR &= ~FPGA_CS_PIN;

    // let the fpga initialize
    delay(250);

    // we can now drive FPGA_CS
    P_FPGA_CS_DIR |= FPGA_CS_PIN;

    // set it high initially (unselected)
    P1DIR |= FPGA_CS_PIN;


    // configure SPI
    P1DIR |= BIT7 | BIT5;           // UCB0CLK | UCB0SIMO
    P1DIR &= ~BIT6;                 // UCB0SOMI

    P1SEL |= BIT5 | BIT6 | BIT7;    // select SPI mode
    P1SEL2 |= BIT5 | BIT6 | BIT7;   // select SPI mode

    UCB0CTL1 = UCSWRST;             // reset the USCI to configure SPI
    UCB0CTL0 |= UCMST;              // master mode
    UCB0CTL0 |= UCMSB;              // MSB first
    UCB0CTL0 |= UCSYNC;             // syncronous
    UCB0CTL0 |= UCCKPH;             // mode 0
    UCB0CTL1 |= UCSSEL_2;           // SMCLK
    UCB0BR0 = 4;                    // divide clock by 4 (to get a 4MHz clock)
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;           // clear UCSWRST


    // setup NES controller pins
    P_NES_DIR |= (NES_LATCH_PIN | NES_CLK_PIN);
    P_NES_DIR &= ~(NES_DATA_1_PIN | NES_DATA_2_PIN);

    GP_wr(J1_RESET, 1);           // HALT coprocessor
    __wstart(RAM_SPR);            // Hide all sprites
    uint16_t i = 0;
    for (i = 0; i < 512; i++)
        GP_xhide();
    __end();
    GP_fill(RAM_PIC, 0, 1024 * 10);  // Zero all character RAM
    GP_fill(RAM_SPRPAL, 0, 2048);    // Sprite palletes black
    GP_fill(RAM_SPRIMG, 0, 64 * 256);   // Clear all sprite data
    GP_fill(VOICES, 0, 256);         // Silence
    GP_fill(PALETTE16A, 0, 128);     // Black 16-, 4-palletes and COMM

    GP_wr16(SCROLL_X, 0);
    GP_wr16(SCROLL_Y, 0);
    GP_wr(JK_MODE, 0);
    GP_wr(SPR_DISABLE, 0);
    GP_wr(SPR_PAGE, 0);
    GP_wr(IOMODE, 0);
    GP_wr16(BG_COLOR, 0);
    GP_wr16(SAMPLE_L, 0);
    GP_wr16(SAMPLE_R, 0);
    GP_wr16(SCREENSHOT_Y, 0);
    GP_wr(MODULATOR, 64);

}


void GP_sprite(uint8_t spr, uint16_t x, uint16_t y, uint8_t image, uint8_t palette, uint8_t rot, uint8_t jk)
{
    __wstart(RAM_SPR + (spr << 2));
    spi_transfer(LOW_BYTE(x));
    spi_transfer((palette << 4) | (rot << 1) | (HIGH_BYTE(x) & 1));
    spi_transfer(LOW_BYTE(y));
    spi_transfer((jk << 7) | (image << 1) | (HIGH_BYTE(y) & 1));
    __end();
}

void GP_xsprite(int16_t ox, int16_t oy, int8_t x, int8_t y, uint8_t image, uint8_t palette, uint8_t rot, uint8_t jk)
{
    if (rot & 2)
        x = -16-x;
    if (rot & 4)
        y = -16-y;
    if (rot & 1) {
        int s;
        s = x; x = y; y = s;
    }
    ox += x;
    oy += y;
    spi_transfer(LOW_BYTE(ox));
    spi_transfer((palette << 4) | (rot << 1) | (HIGH_BYTE(ox) & 1));
    spi_transfer(LOW_BYTE(oy));
    spi_transfer((jk << 7) | (image << 1) | (HIGH_BYTE(oy) & 1));
    gp_cur_spr++;
}


#include "font8x8.h"
static uint8_t stretch[16] = {
    0x00, 0x03, 0x0c, 0x0f,
    0x30, 0x33, 0x3c, 0x3f,
    0xc0, 0xc3, 0xcc, 0xcf,
    0xf0, 0xf3, 0xfc, 0xff
};

void GP_ascii(void)
{
    uint16_t i;

    for (i = 0; i < 768; i++) {
        uint8_t b = font8x8[i];

        uint8_t h = stretch[b >> 4];
        uint8_t l = stretch[b & 0xf];
        GP_wr(0x1000 + (16 * ' ') + (2 * i), h);
        GP_wr(0x1000 + (16 * ' ') + (2 * i) + 1, l);
    }
    for (i = 0x20; i < 0x80; i++) {
        GP_setpal(4 * i + 0, TRANSPARENT);
        GP_setpal(4 * i + 3, RGB(255,255,255));
    }
    GP_fill(RAM_PIC, ' ', 4096);

}

void GP_putstr(uint16_t x, uint16_t y, const char *s)
{
    __wstart((y << 6) + x);
    while (*s)
        spi_transfer(*s++);
    __end();
}

void GP_putnum(uint16_t x, uint16_t y, uint8_t s)
{
    __wstart((y << 6) + x);
    spi_transfer('0'+s);
    __end();
}

void GP_setpal(uint16_t pal, uint16_t rgb)
{
    GP_wr16(RAM_PAL + (pal << 1), rgb);
}


void GP_fill(uint16_t addr, uint8_t v, uint16_t count)
{
    __wstart(addr);
    while (count--)
        spi_transfer(v);
    __end();
}

void GP_copy(uint16_t addr, const uint8_t *src, uint16_t count)
{
    __wstart(addr);
    while (count--)
    {
        spi_transfer(*(const uint8_t*)src);
        src++;
    }
    __end();
}

void GP_xhide(void)
{
    spi_transfer(LOW_BYTE(400));
    spi_transfer(HIGH_BYTE(400));
    spi_transfer(LOW_BYTE(400));
    spi_transfer(HIGH_BYTE(400));
    gp_cur_spr++;
}

void GP_waitvblank(void)
{
    while (GP_rd(VBLANK) == 1);
    while (GP_rd(VBLANK) == 0);
}

void GP_wr(uint16_t addr, uint8_t v)
{
    __wstart(addr);
    spi_transfer(v);
    __end();
}

void GP_wr16(uint16_t addr, uint16_t v)
{
  __wstart(addr);
  spi_transfer(LOW_BYTE(v));
  spi_transfer(HIGH_BYTE(v));
  __end();
}

uint8_t GP_rd(uint16_t addr)
{
    __start(addr);
    uint8_t r = spi_transfer(0x00);
    __end();
    return r;
}

void __wstart(uint16_t addr)
{
    __start(0x8000 | addr);
}

void __wstartspr(uint8_t sprnum)
{
    __start((0x8000 | RAM_SPR) + (sprnum << 2));
    gp_cur_spr = 0;
}

void __start(uint16_t addr)
{
    P_FPGA_CS_OUT &= ~FPGA_CS_PIN;
    spi_transfer(HIGH_BYTE(addr));
    spi_transfer(LOW_BYTE(addr));
}

void __end(void)
{
    P_FPGA_CS_OUT |= FPGA_CS_PIN;
}

uint8_t spi_transfer(uint8_t byte)
{
    UCB0TXBUF = byte;
    while(UCB0STAT & UCBUSY);     // wait for tx to finish
    return UCB0RXBUF;
}

void delay(uint16_t ms)
{
    //TODO implement ms delay. Use a timer I suppose. I'll do this tomorrow, I need to go to sleep
    volatile uint16_t i;
    while (i) { i--; };
}

void GP_nes_read(void)
{
    nes_data_1 = 0;
    nes_data_2 = 0;

    P_NES_OUT |= NES_LATCH_PIN;
    P_NES_OUT &= ~NES_LATCH_PIN;

    if ((~P_NES_IN) & NES_DATA_1_PIN) nes_data_1 = 1;
    if ((~P_NES_IN) & NES_DATA_2_PIN) nes_data_2 = 1;

    uint8_t i = 8;
    while (--i)
    {
        P_NES_OUT |= NES_CLK_PIN;
        P_NES_OUT &= ~NES_CLK_PIN;

        nes_data_1 <<= 1;
        nes_data_2 <<= 1;

        if ((~P_NES_IN) & NES_DATA_1_PIN) nes_data_1 |= 1;
        if ((~P_NES_IN) & NES_DATA_2_PIN) nes_data_2 |= 1;
    }
}

