#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"

int main(void)
{
    uint16_t i;

    // initialize the gamepack
    GP_begin();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE16A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    // loop through all the sprites and draw them spaced out
    __wstartspr(0);
    for (i = 0; i<SPRITE_FRAMES; i++)
    {
        draw_sprite(i*32+20, 10, i, 0, 0);
    }
    __end();
    
    while (1) {}

    return 0;
}
