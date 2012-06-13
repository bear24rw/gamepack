#include <msp430.h>

int main(void)
{
    P2DIR |= BIT2;
    while (1) { P2OUT ^= BIT2; }

    return 0;
}

