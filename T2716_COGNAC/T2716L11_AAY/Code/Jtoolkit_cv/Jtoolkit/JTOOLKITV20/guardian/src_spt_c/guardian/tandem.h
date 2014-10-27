/* @@@ START COPYRIGHT @@@
**     COMPAQ CONFIDENTIAL: NEED TO KNOW ONLY
**
**  Copyright 1995-2001
**  Compaq Computer Corporation
**     Protected as an unpublished work.
**        All rights reserved.
**
**  The computer program listings, specifications, and documentation
**  herein are the property of Compaq Computer Corporation or a
**  third party supplier and shall not be reproduced, copied,
**  disclosed, or used in whole or in part for any reason without
**  the prior express written permission of Compaq Computer
**  Corporation.
**
** @@@ END COPYRIGHT @@@ */
#ifndef _TANDEMH
#define _TANDEMH

#define MAXFILENAMELEN		35	/* External Tandem file name maximum
					   length */
#define MAXDIRNAMELEN		26	/* External Tandem directory name
					   maximum length */
#define MAXDEVNAMELEN		26	/* External Tandem device name maximum
					   length */
#define MAXPROCNAMELEN		35	/* process name maximum length */
#define MAXNODENAMELEN		8	/* node (system) name maximum length */
#define MAXLOCALVOLNAMELEN	8	/* volume name max (no system name) */
#define MAXPMNAMELEN		15	/* PATHMON named process maximum
					   length */
#define MAXSCNAMELEN		15	/* Pathway server class name maximum
					   length */
#define CPUSPERNODE		16	/* each node can have 16 CPU's */

#define EXTENDEDSEGMENTADDR	02000000l	/* Extended memory location
						   and length */
#define MAXEXTENDEDSEGMENTLEN	(128l*1024l*1024l - 512l*1024l)  /* 127.5 mb */

/* Boolean defines */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 * Definition for permanent assertions.  "assert" is a no-op
 * if NDEBUG is defined.  "permAssert" will always work.
 */

extern short _assert(char *, char *, long);

#define permAssert(exp) (void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )

/*
 * Use the following for embedded omitted parameters.
 */
#define OMITSHORT
#define OMITUNSIGNEDSHORT
#define OMITINT
#define OMITFAT_16
#define OMITUNSIGNEDINT
#define OMITLONG
#define OMITFAT_32
#define OMITUNSIGNEDLONG
#define OMIT__INT64
#define OMITREF

typedef long            TM32I;
typedef unsigned long   TM32U;
typedef long long       TM64I;

/*
 * struct PHandle_t -	Process Handle, references a single process.
 *			Implementation is private.
 */
typedef struct PHandle_t {
    short data[10];
} PHandle_t;

/*
 * Simple routine for checking for a NULL PHandle.
 * Use Guardian PROCESSHANDLE_NULLIT_() to make a NULL handle.
 */

#define PHandle_isNULL( PH ) \
     ((PH).data[0] == -1 && \
      0 == memcmp((char*) &(PH).data[1], (char*) &(PH).data[0], \
		  sizeof(PH) - sizeof((PH).data[0])))

/*
 * Data structure for analyzing the results from PROCESSORSTATUS() or
 * REMOTEPROCESSORSTATUS().
 * Let 'whole' get return value of either of these functions, and use
 * breakdown.cpuMask to interpret the values.  The bits can be indexed
 * using the define cpuBit().  For example, to see if CPU 7 is up,
 * use (foo.breakdown.cpuMask & cpuBit(7) ).
 */
#pragma fieldalign shared2 RemoteProcStatus_Union

union RemoteProcStatus_Union {
    long	whole;
    struct {
	short	highestCpu;
	unsigned short cpuMask;
    } breakdown;
};
#define cpuBit( cpuNum ) (1 << (15 - (cpuNum)))

#endif /* _TANDEMH */

