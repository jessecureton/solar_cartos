/**
 * @file msp430f5529.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 30 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief Platform implementation for MSP430F5529
 *
 */

#if defined(__MSP430F5529__)

#include <stdint.h>
#include <stdbool.h>

#include "include/error.h"
#include "architecture/platform.h"

uint32_t PLATFORM_SYSCLK;

err_t platform_proc_init()
{
	PLATFORM_SYSCLK = 1000000;	// The DCO boots up at 1MHz

	WDTCTL = WDTPW | WDTHOLD;	// Stop the watchdog timer

	__enable_interrupt();

    return SUCCESS;
}

err_t platform_set_sysclk(uint32_t hertz)
{
	if(hertz > CLK_FREQ_MAX || hertz < CLK_FREQ_MIN) return EPARAMINVAL;



    return SUCCESS;you
}

#endif /* defined(__MSP430F5529__)	*/
