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
#include <stdbool.h>

#include "application.h"
#include "include/error.h"
#include "architecture/platform.h"
#include "scheduler/scheduler.h"

int main(void)
{
    platform_proc_init();

    // If the application defines a clock frequency run at that speed.'
    // If not, the platform is expected to be running at some default after init.
    #ifdef APP_CLOCK_FREQ
        ASSERT(platform_set_sysclk(APP_SYSCLK_FREQ));
    #endif

    schInit();

    appInit();

    // Execute all available tasks, then put the processor to sleep
    // until its awoken by an interrupt and repeats
    while(1)
    {
        schServiceTasks();
        platform_proc_sleep();
    }
	
    return 0;
}
