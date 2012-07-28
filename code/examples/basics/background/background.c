#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "background.h"

int main(void)
{

    GP_begin();

    uint8_t y;
    for (y = 0; y < 14; y++)
        GP_copy(RAM_PIC + y * 64 + 5*64 + 4, titlescreen_pic + y * 41, 41);
    GP_copy(RAM_CHR, titlescreen_chr, sizeof(titlescreen_chr));
    GP_copy(RAM_PAL, titlescreen_pal, sizeof(titlescreen_pal));

    while (1) {}

    return 0;
}
