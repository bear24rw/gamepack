#include <msp430.h>
#include <stdint.h>
#include "GP.h"
#include "header.h"     //using his sprites for now.
#include <ctype.h>

#define GRID_WIDTH      9       // number of bricks
#define GRID_HEIGHT     8       // number of bricks

#define PADDLE1_X       16 //this will change- needs to be tested on screen
#define PADDLE2_X       380 //this will change- needs to be tested on screen
#define PADDLE_SPEED    5       // pixels per gameloop

#define BALL_R          6       // ball radius
#define BALL_Y_V_MAX    5       // max x velocity
#define BALL_Y_V_MIN    (-5)    // min x velocity

int8_t absolute(int8_t x)
{
    if (x < 0)
        return (x * -1);
    else
        return x;
}

int main(void)
{
    uint8_t score1 = 0;
    uint8_t score2 = 0;

    uint16_t paddle1_y = 80;
    uint16_t paddle2_y = 200;

    uint16_t ball_x = 16*1 + BALL_R;   
    uint16_t ball_y = paddle1_y;
    int16_t ball_x_v = 3;
    int16_t ball_y_v = 2;

    uint8_t paused = 1;
    uint8_t winner = 1;

    // initialize the gamepack
    GP_begin();

    // setup ascii
    GP_ascii();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(255,20,147));    //10,10,15 gray. 255 20 147 pink

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

        //
        // DRAW SCORES
        //
		GP_putnum(8, 2, (score1 / 10000) % 10); // ten-thousand
		GP_putnum(9, 2, (score1 / 1000) % 10);  // thousand
		GP_putnum(10, 2, (score1 / 100) % 10);   // hundred
		GP_putnum(11, 2, (score1 / 10) % 10);    // ten
		GP_putnum(12, 2, (score1 / 1) % 10);     // one

		GP_putnum(35, 2, (score2 / 10000) % 10); // ten-thousand
		GP_putnum(36, 2, (score2 / 1000) % 10);  // thousand
		GP_putnum(37, 2, (score2 / 100) % 10);   // hundred
		GP_putnum(38, 2, (score2 / 10) % 10);    // ten
		GP_putnum(39, 2, (score2 / 1) % 10);     // one

        // reset the sprite counter to 0
        __wstartspr(0);

        //
        // DRAW PADDLE 1
        //
        //ROTATE 90DEGREES
        draw_sprite(PADDLE1_X, paddle1_y - 8 - 16, 36, 5, 0);
        draw_sprite(PADDLE1_X, paddle1_y - 8 - 00, 37, 5, 0);
        draw_sprite(PADDLE1_X, paddle1_y + 8 + 00, 38, 5, 0);

        //
        // DRAW PADDLE 2
        //
        draw_sprite(PADDLE2_X, paddle2_y - 8 - 16, 38, 3, 0);
        draw_sprite(PADDLE2_X, paddle2_y - 8 - 00, 37, 3, 0);
        draw_sprite(PADDLE2_X, paddle2_y + 8 + 00, 36, 3, 0);

        //
        // DRAW BALL
        //

        draw_sprite(ball_x - 8, ball_y - 8, 2, 0, 0);

        // finished drawing sprites
        __end();

        // ********************************************************************
        //                           PADDLE 1
        // ********************************************************************

        if (GP_player_1(NES_UP)) paddle1_y -= PADDLE_SPEED;
        if (GP_player_1(NES_DOWN)) paddle1_y += PADDLE_SPEED;

        if (paddle1_y >= 280) paddle1_y = 280;
        if (paddle1_y <= 16 + 8) paddle1_y = 16 + 8;

        // ********************************************************************
        //                           PADDLE 2
        // ********************************************************************

        if (GP_player_2(NES_UP)) paddle2_y -= PADDLE_SPEED;
        if (GP_player_2(NES_DOWN)) paddle2_y += PADDLE_SPEED;

        if (paddle2_y >= 280) paddle2_y = 280;
        if (paddle2_y <= 16 + 8) paddle2_y = 16 + 8;

        if (paused)
        {
        	if (winner == 1)
        	{
        		ball_y = paddle1_y - 2;
        		if (GP_player_1(NES_START))
        			paused = 0;
        		continue;
        	}
        	else if (winner ==2)
        	{
        		ball_y = paddle2_y - 12;
        		if  (GP_player_2(NES_START))
        			paused = 0;
        		continue;
        	}
        }


        // ********************************************************************
        //                         BALL PHYSICS
        // ********************************************************************

        //
        // WALLS
        //

        // hit top wall traveling up
        if (ball_y <= 3  && ball_y_v <= 0)
        	{
        	ball_y_v *= -1;
        	ball_y = 3;
        	}

        // hit bottom wall traveling down
        if (ball_y >= 285  && ball_y_v >= 0)
        	{
        	ball_y_v *= -1;
        	ball_y = 283;
        	}

        //
        // PADDLE 1
        //

        // if the ball is traveling left and is to the left of the player 1 paddle
        if (ball_x_v < 0 && ball_x - BALL_R < PADDLE1_X + BALL_R) 
        {
        	// hit the center of paddle
        	if ((ball_y + BALL_R >= paddle1_y - 8 && ball_y + BALL_R <= paddle1_y + 8) ||
        	    (ball_y - BALL_R >= paddle1_y - 8 && ball_y - BALL_R <= paddle1_y + 8))
            {
        	    ball_x_v *= -1;
            }
        	// hit the top part of the paddle.
        	else if ((ball_y + BALL_R >= paddle1_y - 8 - 16 && ball_y + BALL_R <= paddle1_y - 8) ||
        	         (ball_y - BALL_R >= paddle1_y - 8 - 16 && ball_y - BALL_R <= paddle1_y - 8))
        	{
        	    ball_x_v *= -1;

        	    // ball is coming from the top
        	    if (ball_y_v > 0)
                    // reverse the direction
        	        ball_y_v *= -1;
        	    // ball is coming from the bottom
        	    else
        	        // increase the speed
        	        ball_y_v -= 1;

        	    if (ball_y_v < BALL_Y_V_MIN) ball_y_v = BALL_Y_V_MIN;
        	}

            // hit the bottom part of the paddle
            else if ((ball_y + BALL_R >= paddle1_y + 8 && ball_y + BALL_R <= paddle1_y + 8 + 16) ||
                     (ball_y - BALL_R >= paddle1_y + 8 && ball_y - BALL_R <= paddle1_y + 8 + 16))
            {
                ball_x_v *= -1;

                // ball is coming from the bottom
                if (ball_y_v < 0)
                    // reverse the direction
                    ball_y_v *= -1;
                // ball is coming from the top
                else
                    // increase the speed
                    ball_y_v += 1;

                if (ball_y_v > BALL_Y_V_MAX) ball_y_v = BALL_Y_V_MAX;
            }
        	// player1 missed the ball
        	else
        	{
        		paused = 1;

        	    paddle1_y = 80;
        	    paddle2_y = 200;

        	    ball_x = PADDLE2_X - 2;   
        	    ball_y = paddle2_y - 12;
        	    ball_x_v = -3;
        	    ball_y_v = -2;

        	    score2++;
        	    winner = 2;
        	}
        }



       //
	   // PADDLE 2
	   //

	   // if the ball is traveling right and is to the right of the player 2 paddle
	   if (ball_x_v > 0 && ball_x + BALL_R > PADDLE2_X - BALL_R) 
	   {
		// hit the center of paddle
		if ((ball_y + BALL_R >= paddle2_y - 8 && ball_y + BALL_R <= paddle2_y + 8) ||
			(ball_y - BALL_R >= paddle2_y - 8 && ball_y - BALL_R <= paddle2_y + 8))
		   {
			ball_x_v *= -1;
		   }
		// hit the top part of the paddle.
		else if ((ball_y + BALL_R >= paddle2_y - 8 - 16 && ball_y + BALL_R <= paddle2_y - 8) ||
				 (ball_y - BALL_R >= paddle2_y - 8 - 16 && ball_y - BALL_R <= paddle2_y - 8))
		{
			ball_x_v *= -1;

			// ball is coming from the top
			if (ball_y_v > 0)
				   // reverse the direction
				ball_y_v *= -1;
			// ball is coming from the bottom
			else
				// increase the speed
				ball_y_v -= 1;

			if (ball_y_v < BALL_Y_V_MIN) ball_y_v = BALL_Y_V_MIN;
		}

		   // hit the bottom part of the paddle
		   else if ((ball_y + BALL_R >= paddle2_y + 8 && ball_y + BALL_R <= paddle2_y + 8 + 16) ||
					(ball_y - BALL_R >= paddle2_y + 8 && ball_y - BALL_R <= paddle2_y + 8 + 16))
		   {
			   ball_x_v *= -1;

			   // ball is coming from the bottom
			   if (ball_y_v < 0)
				   // reverse the direction
				   ball_y_v *= -1;
			   // ball is coming from the top
			   else
				   // increase the speed
				   ball_y_v += 1;

			   if (ball_y_v > BALL_Y_V_MAX) ball_y_v = BALL_Y_V_MAX;
		   }
		// player2 missed the ball
		else
		{
			paused = 1;

			paddle1_y = 80;
			paddle2_y = 200;

			ball_x = PADDLE1_X + BALL_R - 2;  
			ball_y = paddle1_y;
			ball_x_v = 3;
			ball_y_v = 2;

			score1++;

			winner = 1;
		}
	   }

	   // update the ball location
	   ball_x += ball_x_v;
	   ball_y += ball_y_v;

    }
    return 0;
}
