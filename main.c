/**
 * @file main.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 13 April 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license
 * This project is released under the GNU Public License (GPLv3).
 *
 * @brief Primary OS entry point, initialization, and loop
 */
#include <stdint.h>

#include "include/error.h"
#include "architecture/platform.h"

int main(void)
{
    platform_proc_init();

    platform_set_sysclk(20000000);
	
    return 0;
}
