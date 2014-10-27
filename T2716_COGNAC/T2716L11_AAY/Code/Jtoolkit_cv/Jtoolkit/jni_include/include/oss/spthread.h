/* 
 * @ @ @ START COPYRIGHT @ @ @
 *
 * Copyright 2003
 * Hewlett Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and 
 * documentation herein are the property of Compaq Computer 
 * Corporation and successor entities such as Hewlett Packard 
 * Development Company, L.P., or a third party supplier and 
 * shall not be reproduced, copied, disclosed, or used in whole 
 * or in part for any reason without the prior express written 
 * permission of Hewlett Packard Development Company, L.P.
 *
 * @ @ @ END COPYRIGHT @ @ @
 */

/*
 * @OSF_COPYRIGHT@
 * COPYRIGHT NOTICE
 * Copyright (c) 1990, 1991, 1992, 1993, 1994 Open Software Foundation, Inc.
 * ALL RIGHTS RESERVED (DCE).  See the file named COPYRIGHT.DCE for
 * the full copyright text.
 */
/*
 *      %W%     (DEC OSF/1)    %G%
 */
/*
 *  Copyright (c) 1990, 1992 by
 *  Digital Equipment Corporation, Maynard Massachusetts.
 *  All rights reserved.
 *
 *  This software is furnished under a license and may be used and  copied
 *  only  in  accordance  with  the  terms  of  such  license and with the
 *  inclusion of the above copyright notice.  This software or  any  other
 *  copies  thereof may not be provided or otherwise made available to any
 *  other person.  No title to and ownership of  the  software  is  hereby
 *  transferred.
 *
 *  The information in this software is subject to change  without  notice
 *  and  should  not  be  construed  as  a commitment by DIGITAL Equipment
 *  Corporation.
 *
 *  DIGITAL assumes no responsibility for the use or  reliability  of  its
 *  software on equipment which is not supplied by DIGITAL.
 */


#ifndef _PTHREAD_H /* { */
#define _PTHREAD_H

#ifdef __cplusplus
  extern "C" {
#endif

/*
 * Include the Guardian C system header files now!! This ensures that
 * they are included before any functions are redefined to call cma wrappers.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <spt_types.h>
#include <spt/exc_handling.h>


/*
 * -------------------------------------------------------------
 *
 * POSIX 1003.1 Compile Time Constants
 * ====================================
 *
 * _POSIX_THREADS (defined)
 *                      If defined, the implementation supports 
 *                      the Threads option.
 *
 * _POSIX_THREAD_ATTR_STACKSIZE (defined)
 *                      If defined, the implementation supports 
 *                      the Thread Stack Size Attribute option.
 */

#ifndef _POSIX_THREADS
# define _POSIX_THREADS
#endif
#ifndef _POSIX_THREAD_ATTR_STACKSIZE
# define _POSIX_THREAD_ATTR_STACKSIZE
#endif

/* 
 * POSIX 1003.1 Limits
 * ===========================
 *
 * PTHREAD_DESTRUCTOR_ITERATIONS
 *                      Maximum number of attempts to destroy
 *                      a thread's thread-specific data on
 *                      termination (must be at least 4)
 *
 * PTHREAD_KEYS_MAX
 *                      Maximum number of thread-specific data keys
 *                      available per process (must be at least 128)
 *
 * PTHREAD_THREADS_MAX
 *                      Maximum number of threads supported per
 *                      process (must be at least 64).
 *
 * -------------------------------------------------------------
 */

#define PTHREAD_DESTRUCTOR_ITERATIONS	4
#define PTHREAD_KEYS_MAX                1024
#define PTHREAD_THREADS_MAX		2048


#define PTHREAD_STACK_MIN		(4 * 1024) /* 4KB */
#define PTHREAD_STACK_MAX_NP	(32 * 1024 * 1024) /* 32MB */


/*
 * Non portable mutex constant definitions.
 */

#define MUTEX_FAST_NP             0
#define MUTEX_RECURSIVE_NP        1
#define MUTEX_NONRECURSIVE_NP     2

/*
 * Usage:  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 */

#define PTHREAD_COND_INITIALIZER {(void *)-1, -1, -1}

/*
 * Usage:  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 */

#define PTHREAD_MUTEX_INITIALIZER {(void *)-1, -1, -1}

/*
 * Usage:  pthread_once_t once = PTHREAD_ONCE_INIT;
 */

#define PTHREAD_ONCE_INIT         {0, 0, 0}

/*
 * pthread_attr_{get,set}detachstate
 */
#define PTHREAD_CREATE_JOINABLE   0
#define PTHREAD_CREATE_DETACHED   1

/*
 * inheritsched attribute values - pthread_attr{get,set}inheritsched
 */
#define PTHREAD_INHERIT_SCHED     0
#define PTHREAD_EXPLICIT_SCHED    1

/*
 * pthread_setcancelstate parameters
 */
#define PTHREAD_CANCEL_ENABLE     0
#define PTHREAD_CANCEL_DISABLE    1

/*
 * pthread_setcanceltype parameters
 */
#define PTHREAD_CANCEL_ASYNCHRONOUS  0
#define PTHREAD_CANCEL_DEFERRED      1

/*
 * Literal to indicate to a joiner that the thread terminated due to it being
 * cancelled.
 */
#define PTHREAD_CANCELED		((void *)-1)


#ifndef _SCHED_H
#include <sched.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <spt_extensions.h>

/*=============================================================
 * POSIX Threads function prototypes
 *=============================================================
 */

/* pthread_atfork - register fork handlers */

extern int pthread_atfork( pthread_void_fn_ptr_t, pthread_void_fn_ptr_t,
                     pthread_void_fn_ptr_t );

/* pthread_attr_destroy  - destroy a threads attribute object*/

extern int pthread_attr_destroy( pthread_attr_t * );

/* pthread_attr_getdetachstate - get attribute detach state */

extern int pthread_attr_getdetachstate( const pthread_attr_t *, int * );

/* pthread_attr_getinheritsched - get the inheritsched attribute */

extern int pthread_attr_getinheritsched( const pthread_attr_t *, int * );

/* pthread_attr_getschedparam  - get default thread priority.  */

extern int pthread_attr_getschedparam( const pthread_attr_t *,
                                 struct sched_param * );

/* pthread_attr_getschedpolicy - get scheduling policy  */

extern int pthread_attr_getschedpolicy( const pthread_attr_t *, int *);

/* pthread_attr_getstackaddr - get thread stack address  */

extern int pthread_attr_getstackaddr( const pthread_attr_t *, void ** );

/* pthread_attr_getstacksize - get the default stack size
 * Be aware that the stack size will be rounded up by 2048 bytes 
 */

extern int pthread_attr_getstacksize( const pthread_attr_t *, size_t *);

/* pthread_attr_init */

extern int pthread_attr_init( pthread_attr_t *);

/* pthread_attr_setdetachstate - set attribute detach state */

extern int pthread_attr_setdetachstate( pthread_attr_t *, int);

/* pthread_attr_setinheritsched - set the inheritsched attribute */

extern int pthread_attr_setinheritsched( pthread_attr_t *, int );

/* pthread_attr_setschedparam - get the default thread priority */

extern int pthread_attr_setschedparam( pthread_attr_t *,
                                 const struct sched_param *);

/* pthread_attr_setschedpolicy - set the default scheduling algorithm */

extern int pthread_attr_setschedpolicy( pthread_attr_t *, int);

/* pthread_attr_setstacksize - set the default stack size */

extern int pthread_attr_setstacksize( pthread_attr_t *, size_t); 

/* pthread_cancel request that a thread terminate its execution */

extern int pthread_cancel( pthread_t );

/* pthread_cond_broadcast - broadcast a condition variable */

extern int pthread_cond_broadcast( pthread_cond_t * );

/* pthread_cond_destroy - */

extern int pthread_cond_destroy( pthread_cond_t * );

/* pthread_cond_init - initialize a condition variable */

extern int pthread_cond_init( pthread_cond_t *, const pthread_condattr_t *);

/* pthread_cond_signal - signal a condition variable */

extern int pthread_cond_signal( pthread_cond_t *);

/* pthread_cond_timedwait - perform a timed wait on a condition variable.  */

extern int pthread_cond_timedwait( pthread_cond_t *, pthread_mutex_t *,
                             const struct timespec *);

/* pthread_cond_wait - perform a wait on a condition variable */

extern int pthread_cond_wait( pthread_cond_t *, pthread_mutex_t *);

/* pthread_condattr_destroy */

extern int pthread_condattr_destroy( pthread_condattr_t * );

/* pthread_condattr_init */

extern int pthread_condattr_init( pthread_condattr_t *);

/* pthread_create - create a new thread object */

extern int pthread_create( pthread_t *, const pthread_attr_t *,
                     pthread_startroutine_t, pthread_addr_t);

/* pthread_detach */

extern int pthread_detach( pthread_t);

/* pthread_equal - determine if two threads are equal */

#define  pthread_equal(thread1,thread2) \
    (((thread1).field1 == (thread2).field1) \
    && ((thread1).field2 == (thread2).field2) \
    && ((thread1).field3 == (thread2).field3))

/* pthread_exit - terminate a thread */

void  pthread_exit( pthread_addr_t );

/* pthread_getschedparam */

extern int pthread_getschedparam( pthread_t, int *, struct sched_param *);

/* pthread_getspecific - */

void * pthread_getspecific( pthread_key_t);

/* pthread_join - wait termination of another thread */

extern int pthread_join( pthread_t, pthread_addr_t *);

/* pthread_key_create - create a per thread context key */

extern int pthread_key_create( pthread_key_t *, pthread_destructor_t);

/* pthread_key_delete - delete thread-specific data key */

extern int pthread_key_delete( pthread_key_t );

/* pthread_kill - send a signal to a thread */

extern int pthread_kill(pthread_t, int);

/* pthread_kill_np - send a signal to a thread from a signal handler*/

extern int pthread_kill_np(pthread_t, int);

/* pthread_cond_timedwait_np - perform a timed wait on a condition variable.  */

extern int pthread_cond_timedwait_np( pthread_cond_t *, pthread_mutex_t *,
                                      long long);

/* pthread_mutexattr_destroy */

extern int pthread_mutexattr_destroy( pthread_mutexattr_t *);

/* pthread_mutexattr_init */

extern int pthread_mutexattr_init( pthread_mutexattr_t * );

/* pthread_mutex_destroy */

extern int pthread_mutex_destroy( pthread_mutex_t *);

/* pthread_mutex_init */

extern int pthread_mutex_init( pthread_mutex_t *, 
                               const pthread_mutexattr_t *);

/* pthread_mutex_lock */

extern int pthread_mutex_lock( pthread_mutex_t * );

/* pthread_mutex_trylock */

extern int pthread_mutex_trylock( pthread_mutex_t * );

/* pthread_mutex_unlock */

extern int pthread_mutex_unlock( pthread_mutex_t * );

/* pthread_once */

extern int pthread_once( pthread_once_t *, pthread_initroutine_t);

/* pthread_self - get a copy of own thread handle. */

pthread_t  pthread_self(void);

/* pthread_setcancelstate */

extern int pthread_setcancelstate( int, int *);

/* pthread_setcanceltype */

extern int pthread_setcanceltype( int, int * );

/* pthread_setschedparam */

extern int pthread_setschedparam( pthread_t, int, 
                                  const struct sched_param *);

/* pthread_setspecific - */

extern int pthread_setspecific( pthread_key_t, pthread_addr_t);

/* pthread_sigmask - set and examine thread's sigmask */

extern int pthread_sigmask( int, const sigset_t *, sigset_t *);

/* pthread_testcancel - */

void  pthread_testcancel(void);

/*======================================
 * signal related functions
 *======================================
 */

# if !defined(_CMA_NOWRAPPERS_)
#define sigaction(a,b,c) spt_sigaction(a,b,c)
#endif

/* thread aware sigaction */

int spt_sigaction (int, const struct sigaction *, struct sigaction *);


# if !defined(_CMA_NOWRAPPERS_)
#define sigpending(a) spt_sigpending(a)
#endif

/* thread aware sigpending */

extern int spt_sigpending( sigset_t *);

# if !defined(_CMA_NOWRAPPERS_)
#define sigsuspend(a) spt_sigsuspend(a)
#endif

/* thread aware sigsuspend */

extern int spt_sigsuspend( const sigset_t *);

#if !defined(_CMA_NOWRAPPERS_)
#define sigwait(a,b) spt_sigwait(a,b)
# endif

/* thread aware sigwait */

extern pthread_integer_t spt_sigwait(sigset_t *, int *);

# if !defined(_CMA_NOWRAPPERS_)
#define pause() spt_pause()
#endif

/* thread aware pause */

extern int spt_pause();

/*
 * thread aware fork() related functions
 */

# if !defined(_CMA_NOWRAPPERS_)
#define fork()      spt_fork()
#define atfork(a,b,c)    pthread_atfork(a,b,c)
#endif

extern pid_t spt_fork(void);

/*===================================================================
 * pthreads functions which are not currently implemented
 * for this implementation.
 *===================================================================
 */

#if 0

extern int pthread_attr_setscope( const pthread_attr_t *, int *);

extern int pthread_attr_getscope( const pthread_attr_t *, int *);

extern int pthread_condattr_getpshared( pthread_condattr_t *, int *);

extern int pthread_condattr_setpshared( pthread_condattr_t *, int);

extern int pthread_mutexattr_getprotocol( const pthread_mutexattr_t *, int *);

extern int pthread_mutexattr_getpshared( pthread_mutexattr_t *, int *);

extern int pthread_mutexattr_setprotocol( pthread_mutexattr_t *, int);

extern int pthread_mutexattr_setpshared( pthread_mutexattr_t *, int);

extern int pthread_mutexattr_setprioceiling( pthread_mutexattr_t *, int);

extern int pthread_mutexattr_getprioceiling( const pthread_mutexattr_t *, int *);

extern int pthread_mutex_setprioceiling( pthread_mutex_t *, int, int *);

extern int pthread_mutex_getprioceiling( const pthread_mutex_t *, int *);

extern int pthread_attr_setstackaddr( const pthread_attr_t *, void *);

#endif


#define  pthread_cleanup_push(routine,arg)	\
    { \
    pthread_cleanup_t _XXX_proc = (pthread_cleanup_t)(routine); \
    pthread_addr_t _XXX_arg = (arg); \
    int _XXX_completed = 0; \
    TRY {


#define  pthread_cleanup_pop(execute)	\
    _XXX_completed = 1;} \
    FINALLY { \
	int _XXX_execute = execute; \
	if ((! _XXX_completed) || (_XXX_execute)) (*_XXX_proc) (_XXX_arg);} \
    ENDTRY}

/*
 * Operations for alerts.
 */

extern pthread_attr_t		pthread_attr_default;
extern pthread_mutexattr_t	pthread_mutexattr_default;
extern pthread_condattr_t	pthread_condattr_default;

/*
 * pthread_getconcurrency, pthread_setconcurrency - get or set level of
 * concurrency.
 */
extern int pthread_getconcurrency(void);
extern int pthread_setconcurrency(int new_level);

#ifdef __cplusplus /* { */
  }
#endif   /* } end of C++ wrapper */

#endif	/* } _PTHREAD_H */

