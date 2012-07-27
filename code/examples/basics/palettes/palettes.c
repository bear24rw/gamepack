#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

void spr(uint16_t x, uint16_t y, uint8_t pal)
{
    static uint8_t sprnum = 0;
    GP_sprite(sprnum++, x, y, 0, pal, 0, 0);
}

uint16_t random_color(void)
{
  return RGB(64 + random(0, 192), 64 + random(0, 192), 64 + random(0, 192));
}

int main(void)
{
    uint16_t i;

    GP_begin();
    GP_ascii();
    GP_putstr(0, 0, "Sprite palettes");

    for (i = 0; i < 256; i++)
        GP_wr(RAM_SPRIMG + i, i);


    // Fill all the palettes with random colors
    for (i = 0; i < (4 * 256); i++)
        GP_wr16(RAM_SPRPAL + (i << 1), random_color());
    for (i = 0; i < 16; i++) {
        GP_wr16(PALETTE16A + (i << 1), random_color());
        GP_wr16(PALETTE16B + (i << 1), random_color());
    }
    for (i = 0; i < 4; i++) {
        GP_wr16(PALETTE4A + (i << 1), random_color());
        GP_wr16(PALETTE4B + (i << 1), random_color());
    }

    GP_putstr(0, 8, "Four 256-color palettes");
    for (i = 0; i < 4; i++)
        spr(200 + 20 * i, (8 * 8), i);

    GP_putstr(0, 12, "Two 16-color palettes");
    for (i = 0; i < 2; i++) {
        spr(200 + 20 * i, (8 * 12),      0x4 | i);
        spr(200 + 20 * i, (8 * 12) + 20, 0x6 | i);
    }

    GP_putstr(0, 18, "Two 4-color palettes");
    for (i = 0; i < 2; i++) {
        spr(200 + 20 * i, (8 * 18),      0x8 | i);
        spr(200 + 20 * i, (8 * 18) + 20, 0xa | i);
        spr(200 + 20 * i, (8 * 18) + 40, 0xc | i);
        spr(200 + 20 * i, (8 * 18) + 60, 0xe | i);
    }

    while (1) {}

    return 0;
}
