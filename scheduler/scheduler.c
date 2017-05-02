/**
 * @file scheduler.c
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 1 May 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief A cyclic scheduler implementation for generic tasks in a platform-independent way
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "include/error.h"
#include "scheduler/scheduler.h"

uint64_t SCH_TICKS = 0;
SCH_TCB SCH_TASK_QUEUE[MAX_TASKS];

err_t schAddTask(TCB_PRIORITY priority, void (*callback)(uint32_t), uint32_t args, uint32_t period, uint16_t exec)
{
	return SUCCESS;
}

void schServiceTasks()
{

}

void schInterrupt()
{
	SCH_TICKS++;
	for(TASK_IDX i = 0; i < MAX_TASKS; i++)
	{
		// Check if a job is up for release - note that this is dependent on the job periods being a
		// multiple of tick size, otherwise this may never release a job. At creation everything is
		// aimed at periods being millisecond resolution for a 1ms tick size.
	    // TODO: This check could definitely be more robust
		if(SCH_TASK_QUEUE[i].period % SCH_TICKS == 0)
		{
			SCH_TASK_QUEUE[i].need_exec = true;
		}
	}
}
