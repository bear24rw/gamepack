#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    P1DIR |= BIT4;
    P1SEL |= BIT4;

    P1DIR |= BIT0;
    while (1) { P1OUT ^= BIT0; }

    return 0;
}

