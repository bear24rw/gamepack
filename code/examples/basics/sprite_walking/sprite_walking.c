#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"

#define LINK_DOWN_1         0
#define LINK_DOWN_2         1
#define LINK_UP_1           2
#define LINK_UP_2           3
#define LINK_LEFT_1         4
#define LINK_LEFT_2         5
#define LINK_RIGHT_1        6
#define LINK_RIGHT_2        7
#define LINK_ATTACK_RIGHT   8
#define LINK_SWORD_RIGHT    9
#define LINK_SWORD_LEFT     10
#define LINK_ATTACK_LEFT    11
#define LINK_ATTACK_DOWN    12
#define LINK_SWORD_UP       13
#define LINK_SWORD_DOWN     14
#define LINK_ATTACK_UP      15

#define UP      0
#define RIGHT   1
#define DOWN    2
#define LEFT    3

#define SPEED               5   // how many pixels link moves each game loop
#define FOOT_COUNT_MAX      3   // how many pixels before switching walking sprite

int main(void)
{

    uint8_t dir = DOWN;         // direction link is facing
    uint8_t foot = 0;           // which walking sprite to use
    uint8_t foot_count = 0;     // keep track of how many pixels since last walkiing switch
    uint8_t anim = 0;           // what sprite animation of link to display
    uint16_t x = 200;           // x pixel location of link
    uint16_t y = 200;           // y pixel location of link

    // initialize the gamepack
    GP_begin();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE16A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    while (1) 
    {
        // wait for vblank so we don't draw in the middle of a scan
        GP_waitvblank(); 

        // reset the sprite counter to 0
        __wstartspr(0); 

        GP_nes_read();

        // are we attacking?
        if (GP_player_1(NES_A))
        {
            // change to attack animation and display the sword sprite
            switch (dir)
            {
                case UP:
                    anim = LINK_ATTACK_UP;
                    draw_sprite(x, y-16, LINK_SWORD_UP, 0, 0);
                    break;
                case DOWN:
                    anim = LINK_ATTACK_DOWN;
                    draw_sprite(x, y+16, LINK_SWORD_DOWN, 0, 0);
                    break;
                case LEFT:
                    anim = LINK_ATTACK_LEFT;
                    draw_sprite(x-16, y, LINK_SWORD_LEFT, 0, 0);
                    break;
                case RIGHT:
                    anim = LINK_ATTACK_RIGHT;
                    draw_sprite(x+16, y, LINK_SWORD_RIGHT, 0, 0);
                    break;
            }
        }
        else
        {
            // we're not attacking so hide the sword sprite
            GP_xhide();
            
            if (GP_player_1(NES_UP))
            {
                dir = UP;
                y -= SPEED;
                foot_count++;
            }
            else if (GP_player_1(NES_DOWN))
            {
                dir = DOWN;
                y += SPEED;
                foot_count++;
            }
            else if (GP_player_1(NES_LEFT))
            {
                dir = LEFT;
                x -= SPEED;
                foot_count++;
            }
            else if (GP_player_1(NES_RIGHT))
            {
                dir = RIGHT;
                x += SPEED;
                foot_count++;
            }

            // is it time to switch walking animation?
            if (foot_count > FOOT_COUNT_MAX)
            {
                foot ^= 1;
                foot_count = 0;
            } 
        }

        switch(dir)
        {
            case UP:
                anim = foot ? LINK_UP_1 : LINK_UP_2;
                break;
            case DOWN:
                anim = foot ? LINK_DOWN_1 : LINK_DOWN_2;
                break;
            case LEFT:
                anim = foot ? LINK_LEFT_1 : LINK_LEFT_2;
                break;
            case RIGHT:
                anim = foot ? LINK_RIGHT_1 : LINK_RIGHT_2;
                break;
        }

        // draw link
        draw_sprite(x, y, anim, 0, 0);

        // finished drawing sprites
        __end();

        delay(30000);
    }

    return 0;
}
