/**
 * @file platform.h
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 16 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief Nested-import configurations for supported microcontrollers
 *
 * This header merges TIs platform include files for register definitions
 * with our platform implementation API for each microcontroller. This ensures
 * that as a new microcontroller or features is added, all required features are
 * properly implemented across all supported microcontrollers.
 *
 * For more details on how this works see {@link PLATFORM_CONFIGURATION Platform Configuration}
 */

#ifndef INCLUDE_PLATFORMS_H_
#define INCLUDE_PLATFORMS_H_

/*
 * Platform include definitions
 */
#if defined (__MSP432P401R__)
#include <msp432p401r.h>
#include "architecture/msp432p401r/msp432p401r.h"

#elif defined (__MSP430F5529__)
#include <msp430f5529.h>

#else
#error "Attempting to build for unsupported microcontroller!"
#endif

/*
 * API Implementation
 */

/**
 * Current system clock speed
 */
extern uint32_t PLATFORM_SYSCLK;

/**
 * @brief Complete any processor-specific initialization
 *
 * Outcomes:
 * - Watchdog timer disabled
 * - Global interrupts enabled
 * - {@link platform_sysclk} updated to default speed for processor at RESET
 *
 * @return A {@link err_t system error code}
 */
extern err_t platform_proc_init();

/**
 * @brief Set main system clock (typically MCLK) as close as possible to the given value in hertz.
 *
 * Once the frequency has been set, {@link platform_sysclk} will be set to the new clock speed
 *
 * @param hertz Desired processor speed in Hz
 * @return A {@link err_t system error code}
 */
extern err_t platform_set_sysclk(uint32_t hertz);

#endif /* INCLUDE_PLATFORMS_H_ */
