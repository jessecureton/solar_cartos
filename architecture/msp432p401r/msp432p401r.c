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
#include "scheduler/scheduler.h"

uint32_t PLATFORM_SYSCLK;

err_t platform_proc_init()
{
	PLATFORM_SYSCLK = 3000000;	// DCO at startup is 3MHz

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	// Unlock the watchdog timer and issue a hold to shut it off

    __enable_irq();      // Globally enable interrupts

    /* TODO: Configure to use external switching supply if present. Is there a way to check if present? */

    // Set the proper clock domains for standalone operation of all clocks
    // MCLK, SMCLK, and HSMCLK are all DCO by default
    CS->KEY = CS_KEY_VAL;
    CS->CLKEN |= CS_CLKEN_REFO_EN;  // Enable REF0CLK
    CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_SELB;    //ACLK and BCLK both to REF0CLK source
    CS->KEY = 0;

    // Set up the timer interrupt for the scheduler tick on Timer A0
    // We use ACLK as the source because it won't change wrt SYSCLK like MCLK/HSMCLK/SMCLK
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 32;  // 32768Hz works out to ~32 timer ticks for 1ms. Experimentally 32 was closer than 33.
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;

    return SUCCESS;
}

/* TODO: Need a way to logically set up, calculate, and store the (H)SMCLK frequencies here
 * Also configure the DCO to try and use the external resistor unless it is faulted
 */
err_t platform_set_sysclk(uint32_t hertz)
{
	// Error out if we are out of bounds
	if(hertz > CLK_FREQ_MAX || hertz < CLK_FREQ_MIN) return EPARAMINVAL;

	// Unlock the clock registers to set the new speed
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

	// Update the power domain to match the new clock speed
	// In theory this should happen before the clock registers are actually set, but practice seems okay
	// and it's easier to do this after we calculate final SYSCLK
	proc_set_powermode(ACTIVEMODE);

    return SUCCESS;
}

void platform_proc_sleep()
{
    // Enter LPM0 power domain - note that we could modify to enter LPM3, 3.5, 4, or 5 if desired
    // but currently we continue using LPM0 for ~30% power consumption of active, but still quick wakeup
    proc_set_powermode(LPM0);
    // Sleep
    __sleep();  // Just issues an ARM wfi instruction (wait for interrupt) and goes to sleep
}

void proc_set_powermode(proc_power_mode mode)
{
    uint32_t modebits = 0;
    switch(mode)
    {
        // Intentional fall-through. This sets the power state based on SYSCLK, but we actually
        // enter LPM0 with a __sleep() call or ARM "wfi" instruction.
        case ACTIVEMODE:
        case LPM0:
            if(PLATFORM_SYSCLK > 24000000) modebits = PCM_CTL0_AMR__AM_LDO_VCORE1;
            else modebits = PCM_CTL0_AMR__AM_LDO_VCORE0;

            while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            PCM->CTL0 = PCM_CTL0_KEY_VAL | modebits;
            while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            break;
    }
}

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    schInterrupt();
}

#endif  /* defined(__MSP432P401R__) */
