/**
 * @file error.h
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 16 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
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
#define ENOSPACE            2   ///< A needed element was full
#define EUNIMPLEMENTED      3   ///< Function isn't implemented on this platform
/** @} */

/**
 * @brief Halt OS execution on an error
 *
 * This should only be called by the {@link ASSERT} operator.
 *
 * @param file The __FILE__ ID where the error occurred
 * @param line The __LINE__ ID where the error occurred
 * @param error An {@link err_t} describing the error
 */
extern void ERR_TRAP(char *file, uint32_t line, err_t error);

/**
 * @brief Assertion operator - Will trap if expr doesn't evaluate to the {@link err_t} type {@link SUCCESS}
 *
 * expr should be an expression that returns an {@link err_t}
 *
 * Unlike most ASSERT implementations, this is active regardless of if it's a debug or
 * release build. Will result in stopping execution in a way that the debugger can observe
 * the location and type of the error.
 */
#define ASSERT(expr) err_t val = expr; if(val) ERR_TRAP(__FILE__, __LINE__, val)


#endif /* INCLUDE_ERROR_H_ */
