/*---------------------------------------------------------------------------
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/

/*****************************************************************************
   pmproc

    This module contains functions concerning the PATHMON process control.
    For now that means starting a PATHMON, and checking if a PATHMON is
    running on a given node.
*****************************************************************************/
#ifndef _pmproc
#define _pmproc

/* Interface dependencies */
#include "cgi_sip.h"

/* Exported type and data declarations */



extern
/*EXPORT----------------------------------------------------------------------
*
* pmproc_getphandle
*   Returns the phandle of the process (presumably a PATHMON) process
*
* Results:
*   TRUE  -- A process exists, and it's phandle is returned
*   FALSE -- An error occurred, the phandle isn't valid
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_getphandle(
    PHandle_t	*phandle)/* OUT: phandle of the requested process */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmproc_getpriority
*   Returns the priority of the process (presumably a PATHMON) process
*
* Results:
*   TRUE  -- A process exists, and it's priority is returned
*   FALSE -- An error occurred, the priority isn't valid
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_getpriority(
    short	*priority)	/* OUT: priority of the requested process */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmproc_create
*   Starts a PATHMON using the information contained in the initialSipInfo entry.
*   The pmonSubVol is first checked for an executable PATHMON, and if not
*   found, the routine just trys to create a process using
*   $SYSTEM.SYSTEM.PATHMON.  It also sends the startup message, and
*   the ASSIGN message mapping the PATHCTL file to <simplePathmonname>CTL in the
*   pmonSubVol.
*
* Results:
*   TRUE  -- The PATHMON was successfully started
*   FALSE -- The PATHMON could not be created, check error, and error_detail
*            for the reason why.
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_create(
   char			*pathmonfname,  /* OUT: name used in the start attempt */
   short		*error,		/* OUT: error encountered if problems */
   short		*error_detail)	/* OUT: additional info is some cases */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmproc_running
*   Determines if a process (presumably a PATHMON) is already running
*
* Results:
*   TRUE  -- A process with the name is present on the node
*   FALSE -- A process with the derived name isn't present
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_running(
)

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmproc_stop
*   Determines if a process (presumably a PATHMON) is already running on the
*   and then trys to stop the process.
*   Clearly you should only call this function if you're sure you're the one
*   that created the process, and it can be stopped externally (versus
*   sending it a SPI shutdown message).
*
* Results:
*   <>0 -- There was no process to stop, or it couldn't be stopped.
*     0 -- The process has been stopped
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_stop(
)

;
/**--------------------------------------------------------------------------
* Returns a simple version of the pathmon name removing the $ and any node
* information.
*--------------------------------------------------------------------------*/
void getSimplePathmonName(char *name);
/**---------------------------------------------------------------------------------
* char * pmproc_errormsg(
*	short error,			 IN: error number
*	short error_detail)		 IN: additional information
*
*	This routine takes the error numbers(s) returned from a previous call and formats
*	an error message.
*------------------------------------------------------------------------------------*/
char * pmproc_errormsg(short error, short error_detail);
#endif
