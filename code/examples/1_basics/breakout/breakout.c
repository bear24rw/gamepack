#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"

#define GRID_X          16      // pixel offset
#define GRID_Y          16      // pixel offset
#define GRID_WIDTH      9       // number of bricks
#define GRID_HEIGHT     8       // number of bricks

#define PADDLE_Y        260     // pixel location of top of paddle
#define PADDLE_SPEED    5       // pixels per gameloop

#define BALL_R          6       // ball radius
#define BALL_X_V_MAX    5       // max x velocity
#define BALL_X_V_MIN    (-5)    // min x velocity

int8_t abs(int8_t x)
{
    if (x < 0)
        return (x * -1);
    else
        return x;
}

int main(void)
{

    // sprite lookup table
    uint8_t colors[] = {0, 4, 8, 12, 16, 20, 24, 28, 32};

    // 0's are on top of screen, it's sideways
    uint8_t grid[GRID_WIDTH][GRID_HEIGHT] = {
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
    };

    uint8_t round = 1;
    uint16_t score = 0;
    uint16_t high_score = 0;

    uint16_t bricks_left = GRID_WIDTH * GRID_HEIGHT;

    uint8_t lives = 3;

    uint16_t paddle_x = 100;

    uint16_t ball_x = 50;
    uint16_t ball_y = 180;
    int16_t ball_x_v = 3;
    int16_t ball_y_v = 2;

    uint8_t paused = 1;


    // initialize the gamepack
    GP_begin();

    // setup ascii
    GP_ascii();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(10, 10, 15));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(RAM_SPRPAL, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    while (1) 
    {
        GP_waitvblank();

        GP_nes_read();


        // ********************************************************************
        //                         DRAWING
        // ********************************************************************
      
        GP_putstr(41, 1, "HI-SCORE");
        GP_putnum(41, 2, (high_score / 10000) % 10); // ten-thousand
        GP_putnum(42, 2, (high_score / 1000) % 10);  // thousand
        GP_putnum(43, 2, (high_score / 100) % 10);   // hundred
        GP_putnum(44, 2, (high_score / 10) % 10);    // ten
        GP_putnum(45, 2, (high_score / 1) % 10);     // one

        GP_putstr(41, 4, "SCORE");
        GP_putnum(41, 5, (score / 10000) % 10); // ten-thousand
        GP_putnum(42, 5, (score / 1000) % 10);  // thousand
        GP_putnum(43, 5, (score / 100) % 10);   // hundred
        GP_putnum(44, 5, (score / 10) % 10);    // ten
        GP_putnum(45, 5, (score / 1) % 10);     // one
 
        GP_putstr(41, 7, "ROUND");
        GP_putnum(41, 8, round);

        GP_putstr(41, 10, "LIVES");

        GP_putstr(41, 14, "BRICKS");
        GP_putnum(41, 15, (bricks_left / 10) % 10);    // ten
        GP_putnum(42, 15, (bricks_left / 1) % 10);     // one

        GP_putnum(41, 17, abs(ball_x_v));
        GP_putnum(41, 18, abs(ball_y_v));

        // reset the sprite counter to 0
        __wstartspr(0); 

        //
        // NUMBER OF LIVES
        //

        if (lives > 0) draw_sprite(20*16+1*16-8, 90, 2, 0, 0); else GP_xhide();
        if (lives > 1) draw_sprite(20*16+2*16-8, 90, 2, 0, 0); else GP_xhide();
        if (lives > 2) draw_sprite(20*16+3*16-8, 90, 2, 0, 0); else GP_xhide();

        //
        // DRAW BOARDER (should probably be a background image)
        //

        // corners
        draw_sprite(0, 0, 3, 0, 0);
        draw_sprite(16*20, 0, 3, 2, 0);

        // top
        draw_sprite(2*16, 0, 7, 5, 0);
        draw_sprite(3*16, 0, 7, 5, 0);
        draw_sprite(4*16, 0, 7, 5, 0);
        draw_sprite(5*16, 0, 7, 5, 0);

        draw_sprite(6*16, 0, 11, 5, 0);
        draw_sprite(7*16, 0, 15, 5, 0);
        draw_sprite(8*16, 0, 19, 5, 0);

        draw_sprite(9*16, 0, 7, 5, 0);
        draw_sprite(10*16, 0, 7, 5, 0);
        draw_sprite(11*16, 0, 7, 5, 0);
        draw_sprite(12*16, 0, 7, 5, 0);

        draw_sprite(13*16, 0, 11, 5, 0);
        draw_sprite(14*16, 0, 15, 5, 0);
        draw_sprite(15*16, 0, 19, 5, 0);

        draw_sprite(16*16, 0, 7, 5, 0);
        draw_sprite(17*16, 0, 7, 5, 0);
        draw_sprite(18*16, 0, 7, 5, 0);
        draw_sprite(19*16, 0, 7, 5, 0);

        // left
        draw_sprite(0, 16*1, 7, 0, 0);
        draw_sprite(0, 16*2, 7, 0, 0);
        draw_sprite(0, 16*3, 7, 0, 0);
        draw_sprite(0, 16*4, 7, 0, 0);

        draw_sprite(0, 16*5, 11, 0, 0);
        draw_sprite(0, 16*6, 15, 0, 0);
        draw_sprite(0, 16*7, 19, 0, 0);

        draw_sprite(0, 16*8, 7, 0, 0);
        draw_sprite(0, 16*9, 7, 0, 0);
        draw_sprite(0, 16*10, 7, 0, 0);
        draw_sprite(0, 16*11, 7, 0, 0);

        draw_sprite(0, 16*12, 11, 0, 0);
        draw_sprite(0, 16*13, 15, 0, 0);
        draw_sprite(0, 16*14, 19, 0, 0);

        draw_sprite(0, 16*15, 7, 0, 0);
        draw_sprite(0, 16*16, 7, 0, 0);
        draw_sprite(0, 16*17, 7, 0, 0);
        draw_sprite(0, 16*18, 7, 0, 0);

        // right
        draw_sprite(20*16, 16*1, 7, 2, 0);
        draw_sprite(20*16, 16*2, 7, 2, 0);
        draw_sprite(20*16, 16*3, 7, 2, 0);
        draw_sprite(20*16, 16*4, 7, 2, 0);

        draw_sprite(20*16, 16*5, 11, 2, 0);
        draw_sprite(20*16, 16*6, 15, 2, 0);
        draw_sprite(20*16, 16*7, 19, 2, 0);

        draw_sprite(20*16, 16*8, 7, 2, 0);
        draw_sprite(20*16, 16*9, 7, 2, 0);
        draw_sprite(20*16, 16*10, 7, 2, 0);
        draw_sprite(20*16, 16*11, 7, 2, 0);

        draw_sprite(20*16, 16*12, 11, 2, 0);
        draw_sprite(20*16, 16*13, 15, 2, 0);
        draw_sprite(20*16, 16*14, 19, 2, 0);

        draw_sprite(20*16, 16*15, 7, 2, 0);
        draw_sprite(20*16, 16*16, 7, 2, 0);
        draw_sprite(20*16, 16*17, 7, 2, 0);
        draw_sprite(20*16, 16*18, 7, 2, 0);

        //
        // DRAW BRICKS
        //

        uint8_t x = 0;
        uint8_t y = 0;
        for (y = 0; y < GRID_HEIGHT; y++)
        {
            for (x = 0; x < GRID_WIDTH; x++)
            {
                if (grid[x][y] < 0xFF)
                {
                    draw_sprite(GRID_X + x*32, GRID_Y +  y*16, colors[grid[x][y]], 0, 0);
                    draw_sprite(GRID_X + x*32+16, GRID_Y + y*16, colors[grid[x][y]]+1, 0, 0);
                }
                else
                {
                    GP_xhide();
                    GP_xhide();
                }
            }
        }

        //
        // DRAW PADDLE
        //

        draw_sprite(GRID_X + paddle_x - 8 - 16, GRID_Y + PADDLE_Y, 36, 0, 0);
        draw_sprite(GRID_X + paddle_x - 8 - 00, GRID_Y + PADDLE_Y, 37, 0, 0);
        draw_sprite(GRID_X + paddle_x + 8 + 00, GRID_Y + PADDLE_Y, 38, 0, 0);

        //
        // DRAW BALL
        //

        draw_sprite(GRID_X + ball_x - 8, GRID_Y + ball_y - 8, 2, 0, 0);

        // finished drawing sprites
        __end();

        if (paused)
        {
            if (GP_player_1(NES_START))
                paused = 0;

            continue;
        }

        // ********************************************************************
        //                           PADDLE
        // ********************************************************************

        if (GP_player_1(NES_LEFT)) paddle_x -= PADDLE_SPEED;
        if (GP_player_1(NES_RIGHT)) paddle_x += PADDLE_SPEED;

        if (paddle_x >= GRID_WIDTH*32 - 16 - 8) paddle_x = GRID_WIDTH*32 - 16 - 8;
        if (paddle_x <= 0 + 16 + 8) paddle_x = 0 + 16 + 8;


        // ********************************************************************
        //                         BALL PHYSICS
        // ********************************************************************

        //
        // WALLS
        //

        // hit left wall traveling left
        if (ball_x <= 0 + BALL_R && ball_x_v <= 0) ball_x_v *= -1;

        // hit right wall traveling right
        if (ball_x >= GRID_WIDTH*32 - BALL_R && ball_x_v >= 0) ball_x_v *= -1;

        // hit top wall traveling up
        if (ball_y <= 0 + BALL_R && ball_y_v <= 0) ball_y_v *= -1;

        // hit bottom wall traveling down
        if (ball_y >= 300 - BALL_R && ball_y_v >= 0) ball_y_v *= -1;

        //
        // PADDLE
        //

        // if the ball is traveling down and is below the paddle
        if (ball_y_v > 0 && ball_y + BALL_R > PADDLE_Y)
        {
            // hit the center of the paddle
            if ((ball_x + BALL_R >= paddle_x - 8 && ball_x + BALL_R <= paddle_x + 8) ||
                (ball_x - BALL_R >= paddle_x - 8 && ball_x - BALL_R <= paddle_x + 8))
            {
                ball_y_v *= -1;
            }
            // hit the left side of the paddle
            else if ((ball_x + BALL_R >= paddle_x - 8 - 16 && ball_x + BALL_R <= paddle_x - 8) ||
                     (ball_x - BALL_R >= paddle_x - 8 - 16 && ball_x - BALL_R <= paddle_x - 8))
            {
                ball_y_v *= -1;
                
                // ball is coming from the left
                if (ball_x_v > 0)
                    // reverse the direction
                    ball_x_v *= -1;
                // ball is coming from the right
                else
                    // increase the speed
                    ball_x_v -= 1;

                if (ball_x_v < BALL_X_V_MIN) ball_x_v = BALL_X_V_MIN;
            }
            // hit the right side of the paddle
            else if ((ball_x + BALL_R >= paddle_x + 8 && ball_x + BALL_R <= paddle_x + 8 + 16) ||
                     (ball_x - BALL_R >= paddle_x + 8 && ball_x - BALL_R <= paddle_x + 8 + 16))
            {
                ball_y_v *= -1;

                // ball is coming from the right
                if (ball_x_v < 0)
                    // reverse the direction
                    ball_x_v *= -1;
                // ball is coming from the left
                else
                    // increase the speed
                    ball_x_v -= 1;

                if (ball_x_v > BALL_X_V_MAX) ball_x_v = BALL_X_V_MAX;
            }
            // player missed the ball
            else
            {
                paused = 1;


                paddle_x = 100;

                ball_x = 50;
                ball_y = 180;
                ball_x_v = 3;
                ball_y_v = 2;

                // is it game-over?
                if (lives == 0)
                {
                    // do we have a new high score?
                    if (score > high_score) high_score = score;

                    // reset everything
                    bricks_left = GRID_WIDTH * GRID_HEIGHT;
                    lives = 3;
                    round = 1;
                    score = 0;

                    // reset the grid
                    uint8_t i = 0;
                    for (y = 0; y < GRID_HEIGHT; y++)
                    {
                        for(x = 0; x < GRID_WIDTH; x++)
                        {
                            grid[x][y] = (i + round) % 8; // different order each round
                        }

                        i++; // increment color
                    }
                }
                else
                {
                    lives--;
                }

            }

        }

        //
        // BRICKS
        //

        uint8_t hit_x = 0;
        uint8_t hit_y = 0;

        // check if the ball hit a brick
        for (y = 0; y < GRID_HEIGHT; y++)
        {
            for(x = 0; x < GRID_WIDTH; x++)
            {
                // skip this break if it's already gone
                if (grid[x][y] == 0xFF) continue;

                // step the ball in the x direction 
                ball_x += ball_x_v; 
                // does it now intersect?
                if (((ball_x + BALL_R >= x*32 && ball_x + BALL_R <= x*32+32) ||
                     (ball_x - BALL_R >= x*32 && ball_x - BALL_R <= x*32+32)) &&
                    ((ball_y + BALL_R >= y*16 && ball_y + BALL_R <= y*16+16) ||
                     (ball_y - BALL_R >= y*16 && ball_y - BALL_R <= y*16+16))
                   )
                {
                    // yes, remove this brick
                    grid[x][y] = 0xFF;
                    // mark it as a hit
                    hit_x = 1;
                }
                // put ball back where it was
                ball_x -= ball_x_v;

                // step the ball in the y direction
                ball_y += ball_y_v; 
                // does it now intersect?
                if (((ball_x + BALL_R >= x*32 && ball_x + BALL_R <= x*32+32) ||
                     (ball_x - BALL_R >= x*32 && ball_x - BALL_R <= x*32+32)) &&
                    ((ball_y + BALL_R >= y*16 && ball_y + BALL_R <= y*16+16) ||
                     (ball_y - BALL_R >= y*16 && ball_y - BALL_R <= y*16+16))
                   )
                {
                    // yes, remove this brick
                    grid[x][y] = 0xFF;
                    // mark it as a hit
                    hit_y = 1;
                }
                // put ball back where it was
                ball_y -= ball_y_v;

                // if this brick is now gone we just scored
                if (grid[x][y] == 0xFF)
                {
                    score += 5;
                    bricks_left--;
                }

            }
        }

        // if there was a hit reverse the direction
        if (hit_x) ball_x_v *= -1;
        if (hit_y) ball_y_v *= -1;


        // update the ball location
        ball_x += ball_x_v;
        ball_y += ball_y_v;

        // is the round over?
        if (bricks_left == 0)
        {
            // the game is now paused
            paused = 1;

            // go to next round
            round++;
            if (round > 3) round = 3;

            // score bonus
            score += 1000;

            // reset everything
            bricks_left = GRID_WIDTH * GRID_HEIGHT;

            lives = 3;

            paddle_x = 100;

            ball_x = 50;
            ball_y = 180;
            ball_x_v = 3;
            ball_y_v = 2;

            // reset the grid
            uint8_t i = 0;
            for (y = 0; y < GRID_HEIGHT; y++)
            {
                for(x = 0; x < GRID_WIDTH; x++)
                {
                    grid[x][y] = (i + round) % 7; // different order each round
                }

                i++; // increment color
            }

        }

        //delay(3000);
    }

    return 0;
}
