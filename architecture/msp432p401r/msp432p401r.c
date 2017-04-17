/**
 * @file msp432p401r.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 17 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license
 *
 */

#if defined(__MSP432P401R__)

#include <stdint.h>

#include "include/error.h"
#include "architecture/platforms.h"


err_t platform_proc_init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Disable the watchdog timer



    return SUCCESS;
}

err_t platform_set_sysclk(uint32_t hertz)
{
    return SUCCESS;
}

#endif  /* defined(__MSP432P401R__) */
