#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

int main(void)
{
    // setup LED
    P1DIR |= BIT0;

    // initialize the GamePack
    GP_begin();

    while (1)
    {
        // read new values from the controllers
        GP_nes_read();

        // if LEFT was pressed on controller 1 turn on the LED
        if (GP_player_1(LEFT))
            P1OUT |= BIT0;
        else
            P1OUT &= ~BIT0;
    }

    return 0;
}
