/*
 *  Copyright (C) Tandem Computers Incorporated  1996
 *
 *     ALL RIGHTS RESERVED
 */

/*
 * sched.h
 */

#ifndef _SCHED_H /* { */
#define _SCHED_H

#ifdef __cplusplus
  extern "C" {
#endif

/*
 * Scheduling policies:
 * (1c) 13.2
 */
#define SCHED_FIFO	0	/*	((int)cma_c_sched_fifo)		*/
#define SCHED_RR	1	/*	((int)cma_c_sched_rr)		*/
#define SCHED_OTHER	2	/*	((int)cma_c_sched_throughput)	*/

/*
 * Warning: Be aware there is overlap with the standard ones above!
 */
#define SCHED_FG_NP	2	/*	((int)cma_c_sched_throughput)	*/
#define SCHED_BG_NP	3	/*	((int)cma_c_sched_background)	*/

/*
 * (1c) 13.2 Call sched_get_priority_max/min to obtain scheduling priority
 * limits. These defines shouldn't be used directly.
 */
#define PRI_FIFO_MIN		((int)cma_c_prio_fifo_min)
#define PRI_FIFO_MAX		((int)cma_c_prio_fifo_max)

#define PRI_RR_MIN		((int)cma_c_prio_rr_min)
#define PRI_RR_MAX		((int)cma_c_prio_rr_max)

#define PRI_OTHER_MIN		((int)cma_c_prio_through_min)
#define PRI_OTHER_MAX		((int)cma_c_prio_through_max)

/*
 * FG_ values must be same as OTHER since the value SCHED_FG_NP is the same as
 * SCHED_OTHER
 */
#define PRI_FG_MIN_NP		((int)cma_c_prio_through_min)
#define PRI_FG_MAX_NP		((int)cma_c_prio_through_max)

#define PRI_BG_MIN_NP		((int)cma_c_prio_back_min)
#define PRI_BG_MAX_NP		((int)cma_c_prio_back_max)

/*
 * 13.1. Scheduling Parameters
 * ----------------------------
 */
/*
 * This structure contains the scheduling parameters required for
 * implementation of each scheduling policy supported.
 */
struct sched_param {
    int sched_priority;
};

/*
 * 13.3.6 Scheduling Parameter Limits
 * ----------------------------------
 */
int sched_get_priority_max(int);                      /* policy */
int sched_get_priority_min(int);                      /* policy */

/*
 * sched_yield()
 * 13.3.5 (1c). Replaces pthread_yield() (4a)
 */
int sched_yield(void);

#ifdef __cplusplus /* { */
  }
#endif   /* } end of C++ wrapper */

#endif	/* } _SCHED_H */
