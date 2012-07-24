#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

int atxy(int x, int y)
{
  return (y << 6) + x;
}

int main(void)
{

    uint16_t i;

    GP_begin();

    GP_ascii();
    GP_fill(0, ' ', 4096);
    GP_putstr(0, 0,"<------------------- TOP LINE ------------------->");
    GP_putstr(0,36,"<----------------- BOTTOM LINE ------------------>");


    for (i = 0; i < 32; i++) {
        GP_wr16(RAM_PAL + (0x80 + i) * 8, RGB(8 * i, 0, 0));
        GP_wr16(RAM_PAL + (0xa0 + i) * 8, RGB(0, 8 * i, 0));
        GP_wr16(RAM_PAL + (0xc0 + i) * 8, RGB(0, 0, 8 * i));
        GP_wr(atxy(i, 24), 0x80 + i);
        GP_wr(atxy(i, 25), 0xa0 + i);
        GP_wr(atxy(i, 26), 0xc0 + i);
    }
    GP_putstr(0, 28, "R");
    GP_putstr(0, 29, "G");
    GP_putstr(0, 30, "B");
    GP_putstr(4, 31, "0");
    GP_putstr(8, 31, "1");
    GP_putstr(16, 31, "2");

    GP_wr(atxy(4, 28), 0x80 + 4);
    GP_wr(atxy(8, 28), 0x80 + 8);
    GP_wr(atxy(16, 28), 0x80 + 16);

    GP_wr(atxy(4, 29), 0xa0 + 4);
    GP_wr(atxy(8, 29), 0xa0 + 8);
    GP_wr(atxy(16, 29), 0xa0 + 16);

    GP_wr(atxy(4, 30), 0xc0 + 4);
    GP_wr(atxy(8, 30), 0xc0 + 8);
    GP_wr(atxy(16, 30), 0xc0 + 16);


    while (1) {}

    return 0;
}
