#include <msp430.h>
#include <stdint.h>
#include "GP.h"

#include "header.h"

#define BULLET_SPRITE   8
#define BULLET_SPEED    5
#define ALIEN_BULLET_SPEED  2
#define NUMBER_OF_BULLETS   5
#define EXPLOSION_DURATION  20

#define BOULDER_Y       32*7 + 8
#define BOULDER_WIDTH   38
#define BOULDER_HEIGHT  16

#define ALIEN_A         0
#define ALIEN_B         1
#define ALIEN_C         2
#define DANCETIME       30
#define TIMEALIENSHOOT  75

#define SHIP_Y        32*8+16 + 8 // pixel location of top of ship
#define SHIP_SPEED    5       // pixels per gameloop

#define KILLED      BIT1
#define ANIM        BIT0

struct {
    int x;
    uint8_t damage;
} boulders[4];

struct {
    int alien_x;
    int alien_y;
    uint8_t explode;
    uint8_t KilledAnim;
} grid[5][9];

typedef struct {
    int bullet_x;
    int bullet_y;
    uint8_t draw;
} bullet;
bullet ship_bullet[NUMBER_OF_BULLETS];
bullet alien_bullet[NUMBER_OF_BULLETS];


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

// x is left side, y is the center of explosion
void draw_explosion(uint16_t x, uint16_t y)
{
    draw_sprite(x, y, 14, 0, 0);
    draw_sprite(x+16, y, 11, 0, 0);
}

void reset_aliens(void)
{
    uint8_t row = 0;
    uint8_t col = 0;
    for (row = 0; row < 5; row ++)
    {
        for (col = 0; col < 9; col++)
        {
            grid[row][col].alien_x = col*32;
            grid[row][col].alien_y = 20 + 32*row;
            grid[row][col].explode = 0;
            grid[row][col].KilledAnim = 0;
            grid[3][col].KilledAnim |= ANIM;
            grid[4][col].KilledAnim |= ANIM;
        }
    }
}

void reset_bullets(void)
{
    uint8_t k = 0;
    for (k = 0; k < NUMBER_OF_BULLETS; k++)
    {
        ship_bullet[k].bullet_x = 0;
        ship_bullet[k].bullet_y = 0;
        ship_bullet[k].draw = 0;

        alien_bullet[k].bullet_x = 0;
        alien_bullet[k].bullet_y = 0;
        alien_bullet[k].draw = 0;
    }
}

void reset_boulders(void)
{
    uint8_t k = 0;
    for (k = 0; k < 4; k++)
    {
        boulders[k].x = 0;
        boulders[k].damage = 0;
        boulders[k].x = 26+96*k;
    }
}


int main(void)
{
    uint16_t i = 0;

    uint16_t ship_x = 26;
    uint8_t paused = 1;

    int refresh = 0;

    uint8_t alien_row_lookup[] = {ALIEN_A, ALIEN_B, ALIEN_C, ALIEN_B, ALIEN_C};

    unsigned char col;
    unsigned char row;
    unsigned char toRight = 1;
    unsigned int mvt = 0;
    unsigned int down = 0;
    unsigned int speed = 0;
    unsigned int bulletnum = 0;
    unsigned char pressA = 0;
    //unsigned int bulletstart = 0;
    //unsigned int bulletend = 0;
    unsigned int win = 0;
    unsigned int end = 0;
    unsigned int k;
    unsigned int high_score = 0;
    unsigned int score = 0;
    unsigned char lives = 3;
    unsigned char ShootColumn;
    unsigned char breakflag = 0;


    // initialize the gamepack
    GP_begin();

    // setup ascii
    GP_ascii();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE4A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    // hide the rest
    while (gp_cur_spr < 255) { GP_xhide(); }

    __end();


    reset_aliens();
    reset_bullets();
    reset_boulders();


    while (1) {

        GP_waitvblank();

        GP_nes_read();



        // ********************************************************************
        //                         DRAWING
        // ********************************************************************

        GP_putstr(1, 1, "HI-SCORE:");
        GP_putnum(11, 1, (high_score / 100) % 10);   // hundred
        GP_putnum(12, 1, (high_score / 10) % 10);    // ten
        GP_putnum(13, 1, (high_score / 1) % 10);     // one

        GP_putstr(17, 1, "SCORE:");
        GP_putnum(24, 1, (score / 100) % 10);   // hundred
        GP_putnum(25, 1, (score / 10) % 10);    // ten
        GP_putnum(26, 1, (score / 1) % 10);     // one

        GP_putstr(30, 1, "LIVES:");


        // reset the sprite counter to 0
        __wstartspr(0);


        //
        // LIVES
        //

        if (lives > 0)
        {
            draw_ship(310,1);
        }
        if (lives > 1)
        {
            draw_ship(340,1);
        }
        if (lives > 2)
        {
            draw_ship(370,1);
        }

        //
        // GRID OF ALIENS
        //

        for (row = 0; row<5; row++)
        {
            for (col = 0; col<9; col++)
            {
                if ((grid[row][col].KilledAnim & KILLED) == 0) //if it has not been hit
                {
                    draw_alien(alien_row_lookup[row], grid[row][col].alien_x, grid[row][col].alien_y, (grid[row][col].KilledAnim & ANIM));
                }
                else if (grid[row][col].explode != 0)
                {
                    draw_explosion(grid[row][col].alien_x,grid[row][col].alien_y);
                    grid[row][col].explode--;
                }
            }
        }

        //
        // BOULDERS
        //

        for (i = 0; i < 4; i++)
        {
            draw_brick(boulders[i].x, BOULDER_Y, boulders[i].damage);
        }

        //
        // SHIP
        //

        draw_ship(ship_x, SHIP_Y);

        //
        // SHIP BULLETS
        //

        for (k = 0; k < NUMBER_OF_BULLETS; k++)
        {
            if (ship_bullet[k].draw == 0) continue;

            draw_sprite(ship_bullet[k].bullet_x, ship_bullet[k].bullet_y,8,0,0);
        }

        //
        // ALIEN BULLETS
        //

        for (k = 0; k < NUMBER_OF_BULLETS; k++)
        {
            if (alien_bullet[k].draw == 0) continue;

            draw_sprite(alien_bullet[k].bullet_x, alien_bullet[k].bullet_y,8,0,0);
        }

        // hide the rest
        while (gp_cur_spr < 255) { GP_xhide(); }

        __end();


        // ********************************************************************
        //                       PAUSED
        // ********************************************************************

        if (paused)
        {
            if (GP_player_1(NES_START))
            {
                paused = 0;
                GP_putstr(20, 7, "         ");
            }

            continue;
        }


        // ********************************************************************
        //                       ALIEN MOVEMENT
        // ********************************************************************

        refresh++;
        if (refresh % DANCETIME== 0)
        {
            for (row = 0; row < 5; row++)
                for (col = 0; col < 9; col++)
                    grid[row][col].KilledAnim ^= 1;
        }

        for (row = 0; row<5; row++)
        {
            for (col = 0; col<9; col++)
            {
                grid[row][col].alien_x = col*32 + mvt;
                grid[row][col].alien_y = 20 + 32*row + down*16;
            }
        }

        speed++;

        if (toRight == 1 && speed%2 == 0)
        {
            mvt++;
            if (mvt == 110)
            {
                mvt--;
                toRight = 0;
                //down++;
            }
        }

        if (toRight == 0 && speed%2 == 0)
        {
            mvt--;
            if (mvt == 0)
            {
                mvt++;
                toRight = 1;
                down++;
            }
        }


        // ********************************************************************
        //                      SHIP BULLET MOVEMENT
        // ********************************************************************

        for (k = 0; k < NUMBER_OF_BULLETS; k++)
        {
            if (ship_bullet[k].draw == 0) continue;

            ship_bullet[k].bullet_y -= BULLET_SPEED;

            //
            // BOULDERS
            //
            for (col = 0; col < 4; col++)
            {
                // if boulder is already fully damaged skip it
                if (boulders[col].damage == 5) continue;

                // if the bullet is within x y bounds of bounder
                if (ship_bullet[k].bullet_x > boulders[col].x &&
                    ship_bullet[k].bullet_x < boulders[col].x + BOULDER_WIDTH &&
                    ship_bullet[k].bullet_y <= BOULDER_Y + BOULDER_HEIGHT)
                {
                    boulders[col].damage++;
                    ship_bullet[k].draw = 0; //don't draw anymore
                }
            }

            //
            // TOP OF SCREEN
            //
            if (ship_bullet[k].bullet_y < 2) //maybe 0
            {
                ship_bullet[k].draw = 0; //don't draw anymore
            }

            //
            // ALIENS
            //
            for (col = 0; col<9; col++)
            {
                for (row = 0; row < 5; row++)
                {
                    // if alien isn't alive, skip
                    if(grid[row][col].KilledAnim && KILLED) continue;

                    // if bullet already hit something, skip
                    if (ship_bullet[k].draw == 0) continue;

                    // if bullet is within x y bounds of alien
                    if (ship_bullet[k].bullet_x + 5 + 6 > grid[row][col].alien_x + 4 &&        // right side of bullet with left size of alien
                        ship_bullet[k].bullet_x + 5 + 0 < grid[row][col].alien_x + 32 - 4  &&  // left side of bullet with right side of alien
                        ship_bullet[k].bullet_y + 4 <= grid[row][col].alien_y + 16)
                    {
                        grid[row][col].KilledAnim |= KILLED;
                        grid[row][col].explode = EXPLOSION_DURATION;
                        ship_bullet[k].draw = 0; //don't draw anymore
                        score += 10;
                    }
                }
            }

        }


        // ********************************************************************
        //                      ALIEN BULLETS MOVEMENT
        // ********************************************************************
        breakflag = 0;
        if (refresh % TIMEALIENSHOOT == 0) //make time random maybe
        {
            ShootColumn = random(0,8);  //pick a random alien column
            for (i = 4; i >= 0; i--) //find the lowest alien that is still alive
            {
                if ((grid[i][ShootColumn].KilledAnim & KILLED) == 0)  //if it is still alive
                {
                    for (bulletnum = 0; bulletnum < NUMBER_OF_BULLETS; bulletnum++)
                    {
                        if (alien_bullet[bulletnum].draw == 0) //not being drawn
                        {
                            alien_bullet[bulletnum].bullet_x = grid[i][ShootColumn].alien_x + 8;  //probably need to fix offsets here
                            alien_bullet[bulletnum].bullet_y = grid[i][ShootColumn].alien_y + 16;
                            alien_bullet[bulletnum].draw = 1;  //draw the bullet
                            breakflag = 1;
                            break;
                        }
                    }
                }
                if (breakflag == 1) break;
            }
        }

        for (k = 0; k < NUMBER_OF_BULLETS; k++)
        {
            if (alien_bullet[k].draw == 0) continue;

            alien_bullet[k].bullet_y += ALIEN_BULLET_SPEED;

            for (col = 0; col < 4; col++)
            {
                if (alien_bullet[k].bullet_x > boulders[col].x &&                   // bullet is to the right of the boulder
                    alien_bullet[k].bullet_x < boulders[col].x + BOULDER_WIDTH &&   // bullet is to the left of the boulder
                    alien_bullet[k].bullet_y + 16 >= BOULDER_Y &&                   // bullet is  inside boulder from the top
                    boulders[col].damage < 5)                                       // can the boulder take more damage?
                {
                    boulders[col].damage++;     // increase boulder damage
                    alien_bullet[k].draw = 0;   // dont draw bullet anymore since it just his the boulder
                }
            }

            // if the bullet reaches the bottom of the screen
            if (alien_bullet[k].bullet_y > 280)
            {
                alien_bullet[k].draw = 0; //don't draw anymore
            }

            // if the bullet hits a boulder or kills the guy, do stuff
            if ((alien_bullet[k].bullet_x > ship_x - 16) && 
                (alien_bullet[k].bullet_x < ship_x + 16) &&
                (alien_bullet[k].bullet_y + 16 > SHIP_Y))
            {
                lives--;
                alien_bullet[k].draw = 0; //don't draw anymore
            }
        }




        // ********************************************************************
        //                        SHIP MOVEMENT
        // ********************************************************************

        if (GP_player_1(NES_LEFT)) ship_x -= SHIP_SPEED;
        if (GP_player_1(NES_RIGHT)) ship_x += SHIP_SPEED;

        if (ship_x >= 380) ship_x = 380;
        if (ship_x <= 16) ship_x = 16;


        // ********************************************************************
        //                          NEW BULLETS
        // ********************************************************************

        if (GP_player_1(NES_A))
        {
            pressA = 1;
        }

        if (pressA == 1 && !(GP_player_1(NES_A))) // the player released the button
        {
            pressA = 0;
            for (bulletnum = 0; bulletnum < NUMBER_OF_BULLETS; bulletnum++)
            {
                if (ship_bullet[bulletnum].draw == 0) //not being drawn
                {
                    ship_bullet[bulletnum].bullet_x = ship_x-8;
                    ship_bullet[bulletnum].bullet_y = SHIP_Y-16;
                    ship_bullet[bulletnum].draw = 1;
                    bulletnum++;
                    //bulletend++;
                    break;
                }
            }
        }

        // ********************************************************************
        //                          GAME OVER
        // ********************************************************************

        for (col = 0; col<9; col++)
        {
            for (row = 0; row < 5; row++)
            {
                if ((grid[row][col].alien_y + 16 > BOULDER_Y && (grid[row][col].KilledAnim & KILLED) == 0) || lives == 0)
                {
                    //paused = 1;
                    GP_putstr(20, 7, "GAME OVER");
                    end = 1;
                }
            }
        }

        // ********************************************************************
        //                          VICTORY
        // ********************************************************************
        win = 0;
        for (col = 0; col<9; col++)
        {
            for (row = 0; row < 5; row++)
            {
                win = win + (grid[row][col].KilledAnim & KILLED);  //wrong
            }
        }
        if (win == 90)
        {
            //paused = 1;
            GP_putstr(20, 7, "YOU WIN!");
            end = 1;
        }

        // ********************************************************************
        //                          RESTART
        // ********************************************************************

        if (end == 1)
        {
            paused = 1;

            if (score > high_score)
            {
                high_score = score;
            }

            reset_aliens();

            reset_bullets();

            reset_boulders();

            refresh = 0;
            down = 0;
            toRight = 1;
            mvt = 0;
            end = 0;
            score = 0;
            lives = 3;
        }

    }

    return 0;
}

