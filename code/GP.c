#include <msp430.h>
#include <stdint.h>
#include <GP.h>

uint8_t gp_cur_spr = 0;    // current sprite, incremented by xsprite/xhide

void GP_begin(void)
{

    // TODO:
    // put FPGA_CS pin in high Z state immediately
    // it's used for FPGA init

    delay(250);

    // TODO:
    // setup CS pin as output
    // configure SPI
    // set CS pin high

    
    GP_wr(J1_RESET, 1);           // HALT coprocessor
    __wstart(RAM_SPR);            // Hide all sprites
    for (uint16_t i = 0; i < 512; i++)
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


void GP_sprite(uint16_t spr, uint16_t x, uint16_t y, uint8_t image, uint8_t palette, uint8_t rot, uint8_t jk)
{
  __wstart(RAM_SPR + (gp_cur_spr << 2));
  spi_transfer(LOW_BYTE(x));
  spi_transfer((palette << 4) | (rot << 1) | (HIGH_BYTE(x) & 1));
  spi_transfer(LOW_BYTE(y));
  spi_transfer((jk << 7) | (image << 1) | (HIGH_BYTE(y) & 1));
  __end();
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


void GP_xhide(void)
{
    spi_transfer(LOW_BYTE(400));
    spi_transfer(HIGH_BYTE(400));
    spi_transfer(LOW_BYTE(400));
    spi_transfer(HIGH_BYTE(400));
    gp_cur_spr++;
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

void __wstart(uint16_t addr)
{
    __start(0x8000 | addr);
}

void __start(uint16_t addr)
{
    //TODO: CS pin LOW
    spi_transfer(LOW_BYTE(addr));
    spi_transfer(HIGH_BYTE(addr));
}

void __end(void)
{
    //TODO CS pin high
}

void spi_transfer(uint8_t byte)
{
    // TODO implement spi transfer
}

void delay(uint16_t ms)
{
    //TODO implement ms delay
}

