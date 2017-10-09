/**
 * @file scheduler.h
 * @author Jesse Cureton (jlcvm6@mst.edu)
 * @date 1 May 2017
 * @copyright Copyright 2017 - Jesse Cureton. All rights reserved.
 * @license This project is released under the GNU Public License (GPLv3).
 *
 * @brief A cyclic scheduler implementation for generic tasks in a platform-independent way
 *
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

/*====================================
 * Constant definitions
 *====================================*/
#define TASK_IDX uint8_t
#define MAX_TASKS 128

/*====================================
 * Data Types
 *====================================*/

/**
 * Priority enumeration for all tasks
 */
typedef enum {
	LOWEST,
	MEDIUM,
	HIGHEST
} TCB_PRIORITY;

/**
 * Status enumeration for all tasks
 */
typedef enum {
    EMPTY,      ///< An empty block in the {@link SCH_TASK_QUEUE} array
    WAITING,    ///< A valid job in the execution queue, but not yet released
    EXECUTABLE  ///< A valid job that is released and ready to execute
} TCB_STATUS;

/**
 * @brief A task control block for the scheduler to manage a job
 *
 * Needs:
 * 	- function pointer explanation
 * 	- Argument details: data contract must be worked out between thread adding job and job being added, since args is just a reference
 */
typedef struct {
    // Requirements for this task
    TCB_STATUS status;              ///< Status of this task
	TCB_PRIORITY priority;			///< Priority for this task
	void (*callback)(uint32_t);		///< Function pointer to this task's entry point, with argument parameter
	uint32_t args;					///< Reference to the start location of the arguments
	uint32_t period;				///< Period of this task in milliseconds. 0 if task is a one-off
	uint16_t exec;					///< Job execution time in milliseconds. 1ms if < 1ms to execute

	// Doubly-linked list support
	TASK_IDX index;                 ///< The index of this job
	TASK_IDX prev;                  ///< Previous, higher priority job. If job is highest priority this is its own index
	TASK_IDX next;                  ///< Next, lower priority job. If job is lowest priority this is its own id
} SCH_TCB;

/*====================================
 * Variables
 *====================================*/
extern uint64_t SCH_TICKS;					///< Current system time measured as number of ticks, which is definable per platform but should be 1ms
extern SCH_TCB SCH_TASK_QUEUE[MAX_TASKS];	///< All tasks currently being managed by the scheduler
extern TASK_IDX SCH_QUEUE_HEAD;              ///< The index of the highest priority job in the {@link SCH_TASK_QUEUE}
extern TASK_IDX SCH_NUM_JOBS;                ///< The number of jobs currently active in the system

/*====================================/
 * Functions
 *====================================*/

/**
 * @brief Add a new task to be considered for execution
 *
 * This task will manage the actual scheduling of jobs when they are added.
 *
 * See {@link SCH_TCB} for descriptions of these parameters.
 *
 * @param priority
 * @param callback
 * @param args
 * @param period
 * @param exec
 * @return A {@link err_t system error code}
 */
extern err_t schAddTask(TCB_PRIORITY priority, void (*callback)(uint32_t), uint32_t args, uint32_t period, uint16_t exec);

/**
 * @brief Service the task queues and run any jobs currently up for execution
 *
 * Prioritization happens here at execution time based on assigned priorities.
 */
extern void schServiceTasks();

/**
 * @brief The scheduler interrupt handler
 *
 * This must be called by the platform specific scheduler interrupt, however that is implemented on a platform.
 */
extern void schInterrupt();

/**
 * @brief Initialize the scheduler
 *
 * Must be called before the schedulers execution state can be guaranteed. It does the following:
 *  - Initializes the {@link SCH_TASK_QUEUE} array to be all EMPTY jobs
 */
extern void schInit();

/**
 * @brief Remove a task from the execution queue by editing the linked-list pointers
 *
 * @param task The task ID to remove
 */
extern void schRemoveTask(TASK_IDX task);




#endif /* SCHEDULER_SCHEDULER_H_ */
