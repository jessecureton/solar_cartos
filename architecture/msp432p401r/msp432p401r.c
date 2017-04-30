/**
 * @file msp432p401r.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 17 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief Platform implementation for MSP432P401R
 */

#if defined(__MSP432P401R__)

#include <stdint.h>

#include "include/error.h"
#include "architecture/platform.h"


err_t platform_proc_init()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer

    __enable_interrupts();      // Globally enable interrupts

    return SUCCESS;
}

err_t platform_set_sysclk(uint32_t hertz)
{

    return SUCCESS;
}

#endif  /* defined(__MSP432P401R__) */
