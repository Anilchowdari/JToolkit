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
 * HISTORY
 * $Log: put_types.h,v $
 * 2009/09/21  2:51:25PM PDT  Jane Wang
 * T1280H01 - evolve from the spt_types.h in T1248 with following changes:
 *    rename PUT include file name from "spt" to "put",
 *    move the compliant pthread types to the appropriate standard header
 *    files as per POSIX standard.
 * $EndLog$
 */

#ifndef _PUT_TYPES_H /* { */
#define _PUT_TYPES_H

#ifdef __cplusplus
  extern "C" {
#endif

/*
 *  Miscellaneous types 
 */
typedef void   *pthread_address_t;

typedef void (*pthread_void_fn_ptr_t)(void);

typedef int pthread_integer_t;

/*
 * HP SPT Pthreads implementation data types
 */

typedef void                  *pthread_addr_t;
typedef pthread_address_t (* pthread_startroutine_t) (pthread_address_t);
typedef void (*pthread_initroutine_t)  (void);
typedef void (*pthread_destructor_t) (pthread_address_t);

/*
 * For compatibility with OSF/1 pthreads
 */
typedef pthread_addr_t        any_t;
typedef void (*pthread_cleanup_t)  (pthread_addr_t);

#ifdef __cplusplus /* { */
  }
#endif   /* } end of C++ wrapper */

#endif  /* } _PUT_TYPES_H */

