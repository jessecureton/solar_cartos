#include "msp.h"

int main(void)
{
	
    WDTCTL = WDTPW | WDTHOLD;                    /* Stop watchdog timer */
	
    return 0;
}
