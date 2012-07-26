#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

void draw_number(uint16_t x, uint16_t y, uint16_t number)
{
    GP_putnum(x+0, y, (number / 10000) % 10); // ten-thousands
    GP_putnum(x+1, y, (number / 1000) % 10);  // thousands
    GP_putnum(x+2, y, (number / 100) % 10);   // hundred
    GP_putnum(x+3, y, (number / 10) % 10);    // ten
    GP_putnum(x+4, y, (number / 1) % 10);     // one
}

int main(void)
{
    GP_begin();

    GP_ascii();

    uint8_t y = 0;
    uint8_t x = 0;

    while (1)
    {
        GP_waitvblank();

        for (x = 0; x < 50; x += 6)
            for (y = 0; y < 36; y++)
                draw_number(x, y, random(0, 65000));

        delay(60000);
    }

    return 0;
}
