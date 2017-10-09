/**
 * @file error.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 8 October 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license
 * This project is released under the GNU Public License (GPLv3).
 *
 * @brief OS-wide error handling functionality
 */

#include "include/error.h"

void ERR_TRAP(char *file, uint32_t line, err_t error)
{
    while(1);
}
