#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

int main(void)
{
    // initialize the GamePack
    GP_begin();
    GP_ascii();
    GP_fill(0, ' ', 4096);

    while (1)
    {
        // read new values from the controllers
        GP_nes_read();

        GP_putstr(0, 0, "Controller 1");
        GP_putstr(0, 1, "------------");
        GP_putstr(0, 2, "A: ");
        GP_putstr(0, 3, "B: ");
        GP_putstr(0, 4, "START: ");
        GP_putstr(0, 5, "SELECT: ");
        GP_putstr(0, 6, "LEFT: ");
        GP_putstr(0, 7, "RIGHT: ");
        GP_putstr(0, 8, "UP: ");
        GP_putstr(0, 9, "DOWN: ");

        if (GP_player_1(NES_A)) GP_putstr(10, 2, "1"); else GP_putstr(10, 2, "0");
        if (GP_player_1(NES_B)) GP_putstr(10, 3, "1"); else GP_putstr(10, 3, "0");
        if (GP_player_1(NES_START)) GP_putstr(10, 4, "1"); else GP_putstr(10, 4, "0");
        if (GP_player_1(NES_SELECT)) GP_putstr(10, 5, "1"); else GP_putstr(10, 5, "0");
        if (GP_player_1(NES_LEFT)) GP_putstr(10, 6, "1"); else GP_putstr(10, 6, "0");
        if (GP_player_1(NES_RIGHT)) GP_putstr(10, 7, "1"); else GP_putstr(10, 7, "0");
        if (GP_player_1(NES_UP)) GP_putstr(10, 8, "1"); else GP_putstr(10, 8, "0");
        if (GP_player_1(NES_DOWN)) GP_putstr(10, 9, "1"); else GP_putstr(10, 9, "0");

        GP_putstr(20, 0, "Controller 2");
        GP_putstr(20, 1, "------------");
        GP_putstr(20, 2, "A: ");
        GP_putstr(20, 3, "B: ");
        GP_putstr(20, 4, "START: ");
        GP_putstr(20, 5, "SELECT: ");
        GP_putstr(20, 6, "LEFT: ");
        GP_putstr(20, 7, "RIGHT: ");
        GP_putstr(20, 8, "UP: ");
        GP_putstr(20, 9, "DOWN: ");

        if (GP_player_2(NES_A)) GP_putstr(30, 2, "1"); else GP_putstr(30, 2, "0");
        if (GP_player_2(NES_B)) GP_putstr(30, 3, "1"); else GP_putstr(30, 3, "0");
        if (GP_player_2(NES_START)) GP_putstr(30, 4, "1"); else GP_putstr(30, 4, "0");
        if (GP_player_2(NES_SELECT)) GP_putstr(30, 5, "1"); else GP_putstr(30, 5, "0");
        if (GP_player_2(NES_LEFT)) GP_putstr(30, 6, "1"); else GP_putstr(30, 6, "0");
        if (GP_player_2(NES_RIGHT)) GP_putstr(30, 7, "1"); else GP_putstr(30, 7, "0");
        if (GP_player_2(NES_UP)) GP_putstr(30, 8, "1"); else GP_putstr(30, 8, "0");
        if (GP_player_2(NES_DOWN)) GP_putstr(30, 9, "1"); else GP_putstr(30, 9, "0");
    }

    return 0;
}
