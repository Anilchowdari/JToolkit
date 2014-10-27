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

#ifndef _SPT_UCONTEXT_H 
#define _SPT_UCONTEXT_H

#ifdef __cplusplus /* C++ Wrapper */
	extern "C" {
#endif


#define GREG_S0	3
#define GREG_S1	4
#define GREG_S2	5
#define GREG_S3	6
#define GREG_S4	7
#define GREG_S5	8
#define GREG_S6	9
#define GREG_S7	10
#define GREG_S8	11
#define GREG_SP	12
#define GREG_GP	13
#define GREG_RA	14

typedef int	greg_t;
typedef greg_t	gregset_t[38];

typedef struct	mcontext
{
	gregset_t	gpregs;
} mcontext_t;

typedef struct	ucontext
{
	mcontext_t	uc_mcontext;
} ucontext_t;


#ifdef __cplusplus
	}
#endif /* C ++ wrapper */

#endif	/* _SPT_UCONTEXT_H */

