/**
 * @file platforms.h
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 16 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license
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

/**
 * Platform include definitions
 */
#if defined (__MSP432P401R__)
#include <msp432p401r.h>

#elif defined (__MSP430F5529__)
#include <msp430f5529.h>

#else
#error "Attempting to build for unsupported microcontroller!"
#endif

/**
 * API Implementation
 */
extern err_t platform_proc_init();
extern err_t platform_set_systclk(uint32_t hertz);




#endif /* INCLUDE_PLATFORMS_H_ */
