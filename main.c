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

#include "include/error.h"
#include "architecture/platform.h"
#include "scheduler/scheduler.h"

int main(void)
{
    platform_proc_init();

    ASSERT(platform_set_sysclk(24000000));

    schInit();

    // Execute all available tasks, then put the processor to sleep
    // until its awoken by an interrupt and repeats
    while(1)
    {
        schServiceTasks();
        platform_proc_sleep();
    }
	
    return 0;
}

void ERR_TRAP(char *file, uint32_t line, err_t error)
{
    while(1);
}
