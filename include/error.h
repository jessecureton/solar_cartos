/**
 * @file error.h
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 16 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license
 *
 * @brief Global error description system
 */

#ifndef INCLUDE_ERROR_H_
#define INCLUDE_ERROR_H_

/**
 * Error type - one of the global {@link ERR_CODES error codes}
 */
typedef uint8_t err_t;

/**
 * @defgroup ERR_CODES Error Codes
 *
 * Global error codes for representing types of errors
 *
 * @{
 */
#define SUCCESS             0   ///< Function evaluated without error
#define EPARAMINVAL         1   ///< A given function parameter was invalid
/** @} */

#endif /* INCLUDE_ERROR_H_ */
