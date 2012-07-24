#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"
#include "r.h"

int main(void)
{
    uint8_t i;

    GP_begin();
    GP_ascii();
    GP_putstr(0, 0, "Sprite rotation");

    GP_copy(RAM_SPRIMG, r_img, sizeof(r_img));
    GP_copy(RAM_SPRPAL, r_pal, sizeof(r_pal));

    for (i=0; i<8; i++)
    {
        char msg[] = "ROT=.";
        uint8_t y = 3 + 4 * i;
        msg[4] = '0' + i;
        GP_putstr(18, y, msg);
        GP_sprite(i, 200, 8 * y, 0, 0, i, 0);
    }

    while (1) {}

    return 0;
}
