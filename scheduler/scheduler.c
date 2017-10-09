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
TASK_IDX SCH_QUEUE_HEAD = 0;
TASK_IDX SCH_NUM_JOBS = 0;
bool INITIALIZED = false;

err_t schAddTask(TCB_PRIORITY priority, void (*callback)(uint32_t), uint32_t args, uint32_t period, uint16_t exec)
{
    // Find an empty task slot
    TASK_IDX empty;
    for(empty = 0; empty < MAX_TASKS; empty++)
    {
        if(SCH_TASK_QUEUE[empty].status == EMPTY) break;    // We found one!
    }
    if(empty == MAX_TASKS) return ENOSPACE;     // No blocks are open

    // Populate the new field
    SCH_TASK_QUEUE[empty].priority = priority;
    SCH_TASK_QUEUE[empty].callback = callback;
    SCH_TASK_QUEUE[empty].args = args;
    SCH_TASK_QUEUE[empty].period = period;
    SCH_TASK_QUEUE[empty].exec = exec;
    SCH_TASK_QUEUE[empty].index = empty;
    SCH_TASK_QUEUE[empty].status = WAITING;

    // "Add" the new element to the linked list structure
    if(SCH_NUM_JOBS == 0) // Only job in list
    {
        SCH_TASK_QUEUE[empty].next = empty;
        SCH_TASK_QUEUE[empty].prev = empty;
    } else
    {
        TASK_IDX priorityslot = SCH_QUEUE_HEAD;
        while(SCH_TASK_QUEUE[priorityslot].priority > priority)
        {
            priorityslot = SCH_TASK_QUEUE[priorityslot].next;
        }
        if(SCH_TASK_QUEUE[priorityslot].prev == SCH_TASK_QUEUE[priorityslot].index) // If priorityslot is front of list
        {
            SCH_TASK_QUEUE[empty].prev = empty;
            SCH_TASK_QUEUE[empty].next = priorityslot;
            SCH_TASK_QUEUE[priorityslot].prev = empty;
        } else
        {
            SCH_TASK_QUEUE[empty].prev = SCH_TASK_QUEUE[priorityslot].prev;
            SCH_TASK_QUEUE[SCH_TASK_QUEUE[priorityslot].prev].next = empty;
            SCH_TASK_QUEUE[empty].next = priorityslot;
        }
    }

    //TODO: We can use total utilization as a rejection test by summing exec/period for each active job

    SCH_NUM_JOBS++;

	return SUCCESS;
}

void schServiceTasks()
{
    TASK_IDX current = SCH_QUEUE_HEAD;
    // Only run through active jobs
    for(TASK_IDX i = 0; i < SCH_NUM_JOBS; i++)
    {
        // If the current job is executable, do so
        if(SCH_TASK_QUEUE[current].status == EXECUTABLE)
        {
            SCH_TASK_QUEUE[current].callback(SCH_TASK_QUEUE[current].args);
        }

        // If it was a one-off job it's now done, otherwise it's waiting to release again
        if(SCH_TASK_QUEUE[current].period == 0)
        {
            schRemoveTask(current);
        } else
        {
            SCH_TASK_QUEUE[current].status = WAITING;
        }

        // Move to the next element in the list
        current = SCH_TASK_QUEUE[current].next;
    }
}

void schInterrupt()
{
    if(!INITIALIZED) return;

	SCH_TICKS++;

	TASK_IDX current = SCH_QUEUE_HEAD;
	for(TASK_IDX i = 0; i < SCH_NUM_JOBS; i++)
	{
        // Check if a job is up for release - note that this is dependent on the job periods being a
        // multiple of tick size, otherwise this may never release a job. At creation everything is
        // aimed at periods being millisecond resolution for a 1ms tick size.
        // TODO: This check could definitely be more robust
        if(SCH_TASK_QUEUE[current].status == WAITING && SCH_TICKS % SCH_TASK_QUEUE[current].period == 0)
        {
            SCH_TASK_QUEUE[current].status = EXECUTABLE;
        }
	    current = SCH_TASK_QUEUE[current].next;
	}
}

void schInit()
{
    // Make sure all tasks are empty
    for(TASK_IDX i = 0; i < MAX_TASKS; i++)
    {
        SCH_TASK_QUEUE[i].status = EMPTY;
    }

    INITIALIZED = true;
}

void schRemoveTask(TASK_IDX task)
{
    // We may want to return an error type here to account for an already empty job being removed.
    // It isn't technically an invalid operation, but if you're removing an empty task something is
    // probably wrong somewhere. For now just return, since it's already "gone"
    if(SCH_TASK_QUEUE[task].status == EMPTY) return;

    // Mark the task empty
    SCH_TASK_QUEUE[task].status = EMPTY;

    // Update the linked list
    // Account for this being the first or last element in the list
    if(SCH_TASK_QUEUE[task].next == SCH_TASK_QUEUE[task].index) // Last
    {
        SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].prev].next = SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].prev].index;
    } else if(SCH_TASK_QUEUE[task].next == SCH_TASK_QUEUE[task].index) // First
    {
        SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].next].prev = SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].next].index;
    } else // Middle
    {
        SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].prev].next = SCH_TASK_QUEUE[task].next;
        SCH_TASK_QUEUE[SCH_TASK_QUEUE[task].next].prev = SCH_TASK_QUEUE[task].prev;
    }

    SCH_NUM_JOBS--;
}
