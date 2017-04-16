/**
 *  @file main.c
 *  @author Jesse Cureton (jlcvm6@mst.edu)
 *  @date 13 April 2017
 *
 *  @brief Primary OS entry point, initialization, and loop
 *
 *  @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 *  @license
 */

#include "msp.h"

int main(void)
{
	
    WDTCTL = WDTPW | WDTHOLD;                    /* Stop watchdog timer */
	
    return 0;
}
