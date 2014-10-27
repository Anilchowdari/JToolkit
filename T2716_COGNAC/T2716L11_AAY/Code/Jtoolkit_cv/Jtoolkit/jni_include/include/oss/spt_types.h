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

#ifndef _SPT_TYPES_H /* { */
#define _SPT_TYPES_H

#ifdef __cplusplus
  extern "C" {
#endif

/*
 *  Miscellaneous types 
 */
#ifdef _SPT_MODEL_
# ifndef __timespec_DEFINED
# define __timespec_DEFINED
typedef struct timespec {
    unsigned long       tv_sec;         /* seconds */
    long                tv_nsec;        /* and nanoseconds */
    } timespec_t;
#endif
#endif /* _SPT_MODEL_ */

typedef void   *pthread_address_t;

typedef struct PTHREAD_HANDLE_T {
   void                 *field1;
   short int             field2;
   short int             field3;
   } pthread_handle_t;

typedef void (*pthread_void_fn_ptr_t)(void);

typedef int pthread_integer_t;

/*
 * Standard Pthreads primitive data types
 */
typedef pthread_handle_t      pthread_t;
typedef pthread_handle_t      pthread_attr_t;
typedef pthread_handle_t      pthread_mutex_t;
typedef pthread_handle_t      pthread_mutexattr_t;
typedef pthread_handle_t      pthread_cond_t;
typedef pthread_handle_t      pthread_condattr_t;
typedef int                   pthread_key_t;
/* typedef cma_t_once            pthread_once_t; */
typedef struct PTHREAD_ONCE_T {
    int    field1;
    int    field2;
    int    field3;
    } pthread_once_t;

/*
 * HP Pthreads implementation data types
 */

typedef void                  *pthread_addr_t;
/* typedef cma_t_start_routine   pthread_startroutine_t; */
typedef pthread_address_t (*pthread_startroutine_t) (pthread_address_t);
typedef void (*pthread_initroutine_t)  (void);
/* typedef cma_t_destructor      pthread_destructor_t;  */
typedef void (*pthread_destructor_t) (pthread_address_t);

/*
 * For compatibility with OSF/1 pthreads
 */
typedef pthread_addr_t        any_t;
typedef void (*pthread_cleanup_t)  (pthread_addr_t);

#ifdef __cplusplus /* { */
  }
#endif   /* } end of C++ wrapper */

#endif  /* } _SPT_TYPES_H */

