#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"

#define ALIEN_A     0
#define ALIEN_B     1
#define ALIEN_C     2

// x y values are top left of alien
void draw_alien(uint8_t alien, uint16_t x, uint16_t y, uint8_t anim)
{
    switch (alien)
    {
        case ALIEN_A:
            draw_sprite(x+8, y, 0 + (anim * 1) , 0 , 0);
            break;
        case ALIEN_B:
            draw_sprite(x   , y, 3 + (anim * 3) , 0 , 0);
            draw_sprite(x+16, y, 4 + (anim * 3) , 0 , 0);
            break;
        case ALIEN_C:
            draw_sprite(x   , y, 9 + (anim * 3)  , 0 , 0);
            draw_sprite(x+16, y, 10 + (anim * 3) , 0 , 0);
            break;
    }
}

// x y values are top left of brick
void draw_brick(uint16_t x, uint16_t y, uint8_t anim)
{
    uint8_t base = 15 + 6*anim;

    draw_sprite(x    , y    , base+0 , 0 , 0);
    draw_sprite(x+16 , y    , base+1 , 0 , 0);
    draw_sprite(x+32 , y    , base+2 , 0 , 0);

    draw_sprite(x    , y+16 , base+3 , 0 , 0);
    draw_sprite(x+16 , y+16 , base+4 , 0 , 0);
    draw_sprite(x+32 , y+16 , base+5 , 0 , 0);
}

// x value is center of the ship
void draw_ship(uint16_t x, uint16_t y)
{
    draw_sprite(x-16, y, 2, 0, 0);
    draw_sprite(x, y, 5, 0, 0);
}


int main(void)
{
    // initialize the gamepack
    GP_begin();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE4A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    // loop through all the sprites and draw them spaced out
    __wstartspr(0);

    draw_alien(ALIEN_A, 0, 00, 0);
    draw_alien(ALIEN_B, 0, 32, 0);
    draw_alien(ALIEN_C, 0, 64, 0);

    draw_alien(ALIEN_A, 32, 00, 1);
    draw_alien(ALIEN_B, 32, 32, 1);
    draw_alien(ALIEN_C, 32, 64, 1);

    draw_brick(64+(48*0), 0, 0);
    draw_brick(64+(48*1), 0, 1);
    draw_brick(64+(48*2), 0, 2);
    draw_brick(64+(48*3), 0, 3);
    draw_brick(64+(48*4), 0, 4);
    
    // hide the rest
    while (gp_cur_spr < 255) { GP_xhide(); }

    __end();
    
    while (1) {}

    return 0;
}
