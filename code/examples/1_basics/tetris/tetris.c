#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"
#include "tetris.h"

#define POINTS_PER_LINE     5       // points scored for every line cleared

#define GRID_OFFSET_X       (7*16)  // pixel offset
#define GRID_OFFSET_Y       0       // pixel offset
#define GRID_WIDTH          10      // number of blocks
#define GRID_HEIGHT         19      // number of blocks

uint8_t grid_x = 0;
uint8_t grid_y = 0;

int8_t piece_x = 3;
int8_t piece_y = 0;

uint8_t block_x = 0;
uint8_t block_y = 0;

uint8_t grid[GRID_WIDTH][GRID_HEIGHT];

#define BUTTON_DEBOUNCE     3       // how many game loops until button is registered as pressed
uint8_t left_debounce = 0;
uint8_t right_debounce = 0;

// keep track of last button state so we can only allow one button press at a time
uint8_t last_up = 0;
uint8_t last_a = 0;
uint8_t last_start = 0;

#define DROP_TIME           6      // program loops until the piece drops
uint16_t drop_clock = 0;

#define DROP_NORM           0
#define DROP_SOFT           1
#define DROP_HARD           2
uint16_t drop_type = DROP_NORM;

uint8_t piece_hit = 0;

uint8_t shape = 5;
uint8_t rotation = 0;
uint8_t color = 0;

uint8_t next_shape = 6;
uint8_t next_rotation = 1;
uint8_t next_color = 2;

uint8_t hold_shape = 0xFF;
uint8_t hold_rotation = 0;
uint8_t hold_color = 0;
uint8_t hold_lock = 0;

uint16_t score = 0;
uint16_t high_score = 0;
uint16_t lines_cleared = 0;
uint8_t num_sprites = 0;

uint8_t wall_sprite = 0;        // what sprite to draw the walls with

uint8_t paused = 1;


void clear_grid(void);
void move_down(void);
void move_left(void);
void move_right(void);
void rotate(void);
void new_piece(void);


int main(void)
{
    // initialize the gamepack
    GP_begin();

    // setup ascii
    GP_ascii();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(10, 10, 15));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE16A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    clear_grid();

    while (1) 
    {
        GP_waitvblank();

        GP_nes_read();

        // ********************************************************************
        //                          TEXT
        // ********************************************************************

        uint8_t txt = 39;
        GP_putstr(txt+0, 1, "Hi-Score");
        GP_putnum(txt+0, 2, (high_score / 10000) % 10); // ten-thousands
        GP_putnum(txt+1, 2, (high_score / 1000) % 10);  // thousands
        GP_putnum(txt+2, 2, (high_score / 100) % 10);   // hundred
        GP_putnum(txt+3, 2, (high_score / 10) % 10);    // ten
        GP_putnum(txt+4, 2, (high_score / 1) % 10);     // one

        GP_putstr(txt+0, 4, "Score");
        GP_putnum(txt+0, 5, (score / 10000) % 10); // ten-thousands
        GP_putnum(txt+1, 5, (score / 1000) % 10);  // thousands
        GP_putnum(txt+2, 5, (score / 100) % 10);   // hundred
        GP_putnum(txt+3, 5, (score / 10) % 10);    // ten
        GP_putnum(txt+4, 5, (score / 1) % 10);     // one

        GP_putstr(txt+0, 7, "Lines");
        GP_putnum(txt+0, 8, (lines_cleared / 100) % 10);   // hundred
        GP_putnum(txt+1, 8, (lines_cleared / 10) % 10);    // ten
        GP_putnum(txt+2, 8, (lines_cleared / 1) % 10);     // one

        GP_putstr(txt+0, 35, "Sprites");
        GP_putnum(txt+0, 36, (num_sprites / 100) % 10);   // hundred
        GP_putnum(txt+1, 36, (num_sprites / 10) % 10);    // ten
        GP_putnum(txt+2, 36, (num_sprites / 1) % 10);     // one

        if (paused) 
            GP_putstr(txt+0, 20, "Paused");
        else
            GP_putstr(txt+0, 20, "      ");

        GP_putstr(4, 1, "Next");
        GP_putstr(4, 14, "Hold");

        // ********************************************************************
        //                          GRAPHICS
        // ********************************************************************

        // reset the sprite counter to 0
        __wstartspr(0); 

        //
        // DRAW WALLS
        //

        for (grid_y = 0; grid_y < GRID_HEIGHT; grid_y++)
        {
            draw_sprite(GRID_OFFSET_X + GRID_WIDTH*16, grid_y*16, wall_sprite, 0, 0);
            draw_sprite(GRID_OFFSET_X-16, grid_y*16, wall_sprite, 0, 0);
        }

        //
        // DRAW GRID
        //

        for (grid_x = 0; grid_x < GRID_WIDTH; grid_x++)
        {
            for (grid_y = 0; grid_y < GRID_HEIGHT; grid_y++)
            {
                if (grid[grid_x][grid_y] < 0xFF)
                {
                    draw_sprite(GRID_OFFSET_X + 16*grid_x, 
                                GRID_OFFSET_Y + 16*grid_y,
                                grid[grid_x][grid_y], 0, 0);
                }
            }
        }

        //
        // DRAW PIECE
        //

        for (block_x = 0; block_x < 4; block_x++)
        {
            for (block_y = 0; block_y < 4; block_y++)
            {
                if (pieces[shape][rotation][block_x][block_y])
                    draw_sprite(GRID_OFFSET_X + 16*piece_x + 16*block_x, 
                                GRID_OFFSET_Y + 16*piece_y + 16*block_y,
                                color, 0, 0);
            }
        }

        //
        // DRAW NEXT PIECE
        //

        for (block_x = 0; block_x < 4; block_x++)
        {
            for (block_y = 0; block_y < 4; block_y++)
            {
                if (pieces[next_shape][next_rotation][block_x][block_y])
                    draw_sprite(16*1 + 16*block_x,
                                16*2 + 16*block_y, 
                                next_color, 0, 0);
            }
        }

        //
        // DRAW HOLD PIECE
        //

        for (block_x = 0; block_x < 4; block_x++)
        {
            for (block_y = 0; block_y < 4; block_y++)
            {

                // if we're not holding anything skip drawing
                if (hold_shape == 0xFF)
                {
                    break;
                }

                if (pieces[hold_shape][hold_rotation][block_x][block_y])
                    draw_sprite(16*1 + 16*block_x,
                                16*9 + 16*block_y, 
                                hold_color, 0, 0);
            }
        }

        //
        // HIDE THE REST
        //

        num_sprites = gp_cur_spr;
        while (gp_cur_spr < 255) { GP_xhide(); }

        // finished drawing sprites
        __end();

        // ********************************************************************
        //                          PAUSE
        // ********************************************************************

        if (GP_player_1(NES_START))
        {
            if (last_start == 0)
            {
                paused ^= 1;    
                last_start = 1;
            }
        }
        else
        {
            last_start = 0;
        }

        if (paused) continue;

        // ********************************************************************
        //                          MOVEMENT
        // ********************************************************************

        //
        // MOVE LEFT
        //
        if (GP_player_1(NES_LEFT) && (++left_debounce) == BUTTON_DEBOUNCE)
        {
            left_debounce = 0;
            move_left();
        }
        //
        // MOVE RIGHT
        //
        else if (GP_player_1(NES_RIGHT) && (++right_debounce)==BUTTON_DEBOUNCE)
        {
            right_debounce = 0;
            move_right();
        }
        //
        // UP (ROTATE)
        //
        else if (GP_player_1(NES_UP) && last_up == 0)
        {
            // player was last pressing up
            last_up = 1;
            rotate();
        }

        //
        // DOWN (SOFT DROP)
        //
        if (GP_player_1(NES_DOWN))
            drop_type = DROP_SOFT;
        else
            drop_type = DROP_NORM;

        //
        // A (HARD DROP)
        //
        if (GP_player_1(NES_A) && last_a == 0)
        {
            drop_type = DROP_HARD;
            last_a = 1;
        }

        // we can only rotate if we release the Up button then press it again
        if (GP_player_1(NES_UP) == 0) last_up = 0;

        // we can only hard drop if we release the A button then press it again
        if (GP_player_1(NES_A) == 0) last_a = 0;


        // ********************************************************************
        //                          ACTIONS
        // ********************************************************************

        //
        // B (HOLD)
        //
        if (GP_player_1(NES_B))
        {
            if (hold_lock == 0)
            {
                if (hold_shape == 0xFF)
                {
                    hold_shape = shape;
                    hold_rotation = rotation;
                    hold_color = color;

                    // generate a new piece
                    new_piece();
                }
                else
                {
                    // swap the held shape with the current one
                    uint8_t tmp_shape = shape;
                    uint8_t tmp_rotation = rotation;
                    uint8_t tmp_color = color;

                    // restore held shape
                    shape = hold_shape;
                    rotation = hold_rotation;
                    color = hold_color;

                    // store old shape
                    hold_shape = tmp_shape;
                    hold_rotation = tmp_rotation;
                    hold_color = tmp_color;

                    // reset its position
                    piece_x = 3;
                    piece_y = 0;
                }

                // lock future holds since we just used it
                hold_lock = 1;
            }
        }



        // ********************************************************************
        //                          DROP
        // ********************************************************************

        drop_clock++;

        switch (drop_type)
        {
            case DROP_NORM:

                if (drop_clock > DROP_TIME)
                {
                    drop_clock = 0;
                    move_down();
                }
                break;

            case DROP_SOFT:

                if (drop_clock > DROP_TIME / 3)
                {
                    drop_clock = 0;
                    move_down();
                }
                break;

            case DROP_HARD:

                piece_hit = 0;
                while (piece_hit == 0) { move_down(); }

                drop_type = DROP_NORM;

                break;
        }


        // check for game over
        for (grid_x = 0; grid_x < GRID_WIDTH; grid_x++)
        {
            if (grid[grid_x][0] < 0xFF)
            {
                clear_grid();
                lines_cleared = 0;
                score = 0;
                paused = 1;
                hold_shape = 0xFF;
            }
        }


        // check for new high score
        if (score > high_score) high_score = score;

        delay(60000);
        delay(60000);
        delay(60000);
        delay(60000);
    }

    return 0;
}

void clear_grid(void)
{
    for (grid_x = 0; grid_x < GRID_WIDTH; grid_x++)
        for (grid_y = 0; grid_y < GRID_HEIGHT; grid_y++)
            grid[grid_x][grid_y] = 0xFF;
}

void move_down(void)
{
    piece_hit = 0;

    // check each row from bottom to top
    for (block_y = 4; block_y > 0; block_y--)
    {
        for (block_x = 0; block_x < 4; block_x++)
        {
            if (piece_hit) continue;

            if ((pieces[shape][rotation][block_x][block_y-1]) &&     // block in the piece
                    (piece_y+block_y-1+1 == GRID_HEIGHT))                   // bottom of grid
            {
                piece_hit = 1;
                break;
            }

            if ((pieces[shape][rotation][block_x][block_y-1]) &&     // block in the piece
                    (grid[piece_x+block_x][piece_y+block_y-1+1] < 0xFF))    // block in the grid
            {
                piece_hit = 1;
                break;
            }
        }
    }

    if (piece_hit)
    {
        // if we were locking hold it's now free
        hold_lock = 0;

        // store this piece in the grid
        for (block_x = 0; block_x < 4; block_x++)
        {
            for (block_y = 0; block_y < 4; block_y++)
            {
                if (pieces[shape][rotation][block_x][block_y])
                    grid[piece_x+block_x][piece_y+block_y] = color+1;
            }
        }

        // score bonus for clearing multiple lines at once
        uint8_t multi_clear = 0;

        // check for a complete row
        // loop through each row
        for (grid_y = 0; grid_y < GRID_HEIGHT; grid_y++)
        {
            // add up each block in the row
            uint8_t num_blocks = 0;
            for (grid_x = 0; grid_x < GRID_WIDTH; grid_x++)
            {
                if (grid[grid_x][grid_y] < 0xFF) num_blocks++;
            }

            if (num_blocks == GRID_WIDTH)
            {
                lines_cleared++;
                multi_clear++;

                score += POINTS_PER_LINE;

                wall_sprite = (wall_sprite+1)%20;

                // shift the grid down
                for (block_y = grid_y; block_y > 0; block_y--)
                {
                    for (block_x = 0; block_x < GRID_WIDTH; block_x++)
                        grid[block_x][block_y] = grid[block_x][block_y-1];
                }
                // top row has to be open now
                for (block_x = 0; block_x < GRID_WIDTH; block_x++)
                    grid[block_x][0] = 0xFF;
            }
        }

        // double points for clearing multiple lines
        if (multi_clear > 1)
            score += (multi_clear*POINTS_PER_LINE);

        new_piece();

    }
    else
    {
        piece_y++;
    }

}

void move_left(void)
{
    piece_hit = 0;

    // check each column from left to right
    for (block_x = 0; block_x < 4; block_x++)
    {
        for (block_y = 0; block_y < 4; block_y++)
        {
            if (piece_hit) continue;

            if ((pieces[shape][rotation][block_x][block_y]) &&      // block in the piece
                (piece_x+block_x == 0))                             // left side of grid
            {
                piece_hit = 1;
                break;
            }

            if ((pieces[shape][rotation][block_x][block_y]) &&      // block in the piece
                (grid[piece_x+block_x-1][piece_y+block_y] < 0xFF))  // block in the grid
            {
                piece_hit = 1;
                break;
            }
        }
    }

    // if there was no hit move it left
    if (!piece_hit) piece_x -= 1;
}

void move_right(void)
{
    piece_hit = 0;

    // check each column from right to left
    for (block_x = 4; block_x > 0; block_x--)
    {
        for (block_y = 0; block_y < 4; block_y++)
        {
            if (piece_hit) continue;

            if ((pieces[shape][rotation][block_x-1][block_y]) &&     // block in the piece
                (piece_x+block_x-1 == GRID_WIDTH-1))                    // right side of grid
            {
                piece_hit = 1;
                break;
            }


            if ((pieces[shape][rotation][block_x-1][block_y]) &&     // block in the piece
                (grid[piece_x+block_x-1+1][piece_y+block_y] < 0xFF))    // block in the grid
            {
                piece_hit = 1;
                break;
            }

        }
    }

    // if there was no hit move it right
    if (!piece_hit) piece_x += 1;
}

void rotate(void)
{
    piece_hit = 0;

    // check each block of the next rotation
    for (block_x = 0; block_x < 4; block_x++)
    {
        for (block_y = 0; block_y < 4; block_y++)
        {
            if (piece_hit) continue;

            // block on block
            if ((pieces[shape][(rotation+1)%4][block_x][block_y]) &&     // block in piece
                (grid[piece_x+block_x][piece_y+block_y] < 0xFF))         // block in grid
            {
                piece_hit = 1;
                break;
            }

            // block on right wall
            if ((pieces[shape][(rotation+1)%4][block_x][block_y]) &&     // block in piece
                (piece_x+block_x >= GRID_WIDTH))                         // right wall
            {
                piece_hit = 1;
                break;
            }

            // block on left wall
            if ((pieces[shape][(rotation+1)%4][block_x][block_y]) &&     // block in piece
                (piece_x+block_x < 0))                                   // left wall
            {
                piece_hit = 1;
                break;
            }

            // block on bottom wall
            if ((pieces[shape][(rotation+1)%4][block_x][block_y]) &&     // block in piece
                (piece_y+block_y == GRID_HEIGHT))                        // bottom wall
            {
                piece_hit = 1;
                break;
            }
        }
    }

    // if no collision rotate the piece
    if (!piece_hit) rotation = (rotation+1)%4;

}

void new_piece(void)
{
    // next piece
    shape = next_shape;
    next_shape = (next_shape + 1) % 7;

    // next rotation
    rotation = next_rotation;
    next_rotation = (next_rotation + 1) % 4;

    // next color
    color = next_color;
    next_color = (next_color + 2) % 20;

    piece_x = 3;
    piece_y = 0;

}
