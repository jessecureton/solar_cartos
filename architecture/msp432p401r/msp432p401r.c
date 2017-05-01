/**
 * @file msp432p401r.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 17 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief Platform implementation for MSP432P401R
 *
 * Needs info on:
 * 		- Bitbanding - see https://community.arm.com/processors/f/discussions/2646/what-is-the-main-reason-or-purpose-of-bit-band-in-cortex-m3
 */

#if defined(__MSP432P401R__)

#include <stdint.h>
#include <math.h>

#include "include/error.h"
#include "architecture/platform.h"

uint32_t PLATFORM_SYSCLK;

err_t platform_proc_init()
{
	PLATFORM_SYSCLK = 3000000;	// DCO at startup is 3MHz

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	// Unlock the watchdog timer and issue a hold to shut it off

    __enable_interrupts();      // Globally enable interrupts

    return SUCCESS;
}

err_t platform_set_sysclk(uint32_t hertz)
{
	// Error out if we are out of bounds
	if(hertz > CLK_FREQ_MAX || hertz < CLK_FREQ_MIN) return EPARAMINVAL;

	// Unlock the clock registers
	CS->KEY = CS_KEY_VAL;

	// Find closest DCO center frequency per TI SLAU356F doc Table 5-4
	// These are 1.5, 3, 6, 12, 24, 48MHz for DCORSEL = 0, 1, etc
	uint8_t dcorsel;				// DCORSEL index
	for(dcorsel = 0; dcorsel < 6; dcorsel++)
	{
		// Check if desired is less than midpoint between each center frequency
		if(hertz <= 1.5 * (1<<dcorsel) * 1500000)
		{
			// If it is, break the loop, this is the closest center freq
			break;
		}
	}

	// Using hertz (desired nominal freq) and the calculated DCO range, calculate needed offset
	// These equations come from TI SLAA658A
	float kdcoconst; uint32_t fcal;
	if(dcorsel == 5)
	{
		kdcoconst = *((float*) &(TLV->DCOIR_CONSTK_RSEL5));
		fcal = TLV->DCOIR_FCAL_RSEL5;
	} else
	{
		kdcoconst = *((float*) &(TLV->DCOIR_CONSTK_RSEL04));
		fcal = TLV->DCOIR_FCAL_RSEL04;
	}
	float dcotune = (((int32_t)hertz-(1500000*(1<<dcorsel))) * (1+(kdcoconst * (768-fcal)))) / (hertz * kdcoconst);
	dcotune = roundf(dcotune);
	// We have to convert this to 10-bit 2's complement
	uint16_t dcotune_bin = (BIT9 & (uint16_t)dcotune) | (0x1FF & (uint16_t)dcotune);

	// Set the CS registers to match this frequency
	BITBAND_PERI(CS->CTL0, CS_CTL0_DCORSEL_OFS) = dcorsel;
	BITBAND_PERI(CS->CTL0, CS_CTL0_DCOTUNE_OFS) = dcotune_bin;

	// Relock the clock registers
	CS->KEY = 0;

	// Calculate actual clock frequency we just set and store that, since it may not be exact
	PLATFORM_SYSCLK = (1500000*(1<<dcorsel))/( 1 - ((kdcoconst*dcotune)/(1+kdcoconst*(768-fcal))));

    return SUCCESS;
}

#endif  /* defined(__MSP432P401R__) */
