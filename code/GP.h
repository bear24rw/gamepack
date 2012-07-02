#ifndef _GP_H_
#define _GP_H_

#include <stdint.h>

// ---------------------------------------------------------------------------
// PIN MAPPINGS
// ---------------------------------------------------------------------------

#define P_FLASH_CS_OUT  P2OUT
#define P_FLASH_CS_DIR  P2DIR
#define FLASH_CS_PIN    BIT5

#define P_FPGA_CS_OUT   P2OUT
#define P_FPGA_CS_DIR   P2DIR
#define FPGA_CS_PIN     BIT0

#define P_GPIO_IN       P2IN
#define P_GPIO_OUT      P2OUT
#define P_GPIO_DIR      P2DIR

#define GPIO_1_PIN      BIT3
#define GPIO_2_PIN      BIT4
#define GPIO_3_PIN      BIT1
#define GPIO_4_PIN      BIT2

// ---------------------------------------------------------------------------
// NES CONTROLLERS
// ---------------------------------------------------------------------------

#define NES_LATCH_PIN   GPIO_1_PIN
#define NES_CLK_PIN     GPIO_2_PIN
#define NES_DATA_1_PIN  GPIO_3_PIN
#define NES_DATA_2_PIN  GPIO_4_PIN

#define P_NES_IN        P_GPIO_IN
#define P_NES_OUT       P_GPIO_OUT
#define P_NES_DIR       P_GPIO_DIR

extern uint8_t nes_data_1;
extern uint8_t nes_data_2;

#define A               (1 << 7)
#define B               (1 << 6)
#define SELECT          (1 << 5)
#define START           (1 << 4)
#define UP              (1 << 3)
#define DOWN            (1 << 2)
#define LEFT            (1 << 1)
#define RIGHT           (1 << 0)

#define GP_player_1(x)  (~x & nes_data_1)
#define GP_player_2(x)  (~x & nes_data_2)

// ---------------------------------------------------------------------------
// GAMEPACK MEMORY MAP
// ---------------------------------------------------------------------------

#define RAM_PIC         0x0000    // Screen Picture, 64 x 64 = 4096 bytes
#define RAM_CHR         0x1000    // Screen Characters, 256 x 16 = 4096 bytes
#define RAM_PAL         0x2000    // Screen Character Palette, 256 x 8 = 2048 bytes

#define IDENT           0x2800
#define REV             0x2801
#define FRAME           0x2802
#define VBLANK          0x2803
#define SCROLL_X        0x2804
#define SCROLL_Y        0x2806
#define JK_MODE         0x2808
#define J1_RESET        0x2809
#define SPR_DISABLE     0x280a
#define SPR_PAGE        0x280b
#define IOMODE          0x280c

#define BG_COLOR        0x280e
#define SAMPLE_L        0x2810
#define SAMPLE_R        0x2812

#define MODULATOR       0x2814

#define SCREENSHOT_Y    0x281e

#define PALETTE16A      0x2840   // 16-color palette RAM A, 32 bytes
#define PALETTE16B      0x2860   // 16-color palette RAM B, 32 bytes
#define PALETTE4A       0x2880   // 4-color palette RAM A, 8 bytes
#define PALETTE4B       0x2888   // 4-color palette RAM A, 8 bytes
#define COMM            0x2890   // Communication buffer
#define COLLISION       0x2900   // Collision detection RAM, 256 bytes
#define VOICES          0x2a00   // Voice controls
#define J1_CODE         0x2b00   // J1 coprocessor microcode RAM
#define SCREENSHOT      0x2c00   // screenshot line RAM

#define RAM_SPR         0x3000    // Sprite Control, 512 x 4 = 2048 bytes
#define RAM_SPRPAL      0x3800    // Sprite Palettes, 4 x 256 = 2048 bytes
#define RAM_SPRIMG      0x4000    // Sprite Image, 64 x 256 = 16384 bytes

// ---------------------------------------------------------------------------
// HELPER MACROS
// ---------------------------------------------------------------------------

#define LOW_BYTE(x)     (x & 0xFF)
#define HIGH_BYTE(x)    (x >> 8)

#define RGB(r,g,b)      ((((r) >> 3) << 10) | (((g) >> 3) << 5) | ((b) >> 3))
#define TRANSPARENT     (1 << 15) // transparent for chars and sprites

// ---------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

void GP_begin(void);

void GP_sprite(uint16_t spr, uint16_t x, uint16_t y, uint8_t image, uint8_t palette, uint8_t rot, uint8_t jk);
void GP_fill(uint16_t addr, uint8_t v, uint16_t count);
void GP_xhide(void);
void GP_ascii(void);
void GP_setpal(uint16_t pal, uint16_t rgb);
void GP_putstr(uint16_t x, uint16_t y, const char *s);

void GP_wr(uint16_t addr, uint8_t v);
void GP_wr16(uint16_t addr, uint16_t v);

void GP_nes_read(void);

void __start(uint16_t addr);
void __wstart(uint16_t addr);
void __end(void);

void spi_transfer(uint8_t byte);

void delay(uint16_t ms);

#endif
