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
   pmspi

    This module contains functions concerning talking SPI to PATHMON.
*****************************************************************************/
#ifndef _pmspi
#define _pmspi
#include "zspic"
#include "zpwyc"      nolist

#define SIP_SERVER 1000
#define MAXSTRING 78
#define MAXDIR 1024
#define CPUSPERNODE 16
#define MAXNODENAMELEN 8

typedef struct serverClassInfo_t {
	char progname[MAXSTRING+1];
	char sc_name[16];
	char gsubvol[36];
	char hometerm[36];
	char logpath[1023];
	char cwddir[MAXDIR+1];
	char arglist[24000];
	char envlist[24000];
// Begin Changes for Solution Number 10-040512-5972 

/*  Each server class is configured in only one CPU */
	short cpulist;

// End Changes for Solution Number 10-040512-5972
}serverClassInfo_t;

struct pathmon_t {
   short		fnum;
   short		error;
   zspi_ddl_int_def	version;
   char			node[9];
   char	pathmonname[26];
   short		priority;	/* only obtained if we need it */
}pathmon_T;

struct arglist_t {
   zspi_ddl_int_def	len;			/* # of bytes that follow */
   zspi_ddl_byte_def	data[24000];		/* data */
};
struct defineItem_t {
	zspi_ddl_int_def len;
	zspi_ddl_byte_def   data[475];
};

struct env_t {
   zspi_ddl_int_def	len;			/* # of bytes that follow */
   zspi_ddl_byte_def	data[24000];	/* data */
};

/* Interface dependencies */

/* Exported type and data declarations */
extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_pm
*   Sets the structure to the values passed in.
*
* Results:
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
void pmspi_pm(
   char		*pmName,	/* IN: name of pathmon */
   char		*nodename	/* IN: Node name where PATHMON resides */
)
;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_available
*   Returns TRUE if the PATHMON is already open for access, or can be
*   successfully opened for access.
*
* Results:
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_available()
;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_open
*   Opens the appropriate PATHMON for SPI access.
*
* Results:
* <>0 -- PATHMON could not be opened; this is the FS error received
*   0 -- PATHMON is now open for access
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_open()
;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_close
*   Closes File System access to the appropriate PATHMON.
*
* Results:
* <>0 -- FILE_CLOSE_ returned an error; however, the file should still be
*        considered closed.  The error is purely FYI.
*   0 -- FILE_CLOSE_ was successful.
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_close()
;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_errormsg
*   This routine takes a msg buffer, and the error number(s) returned from
*   a previous call to a pmspi routine and formats a message fragment.  The
*   calling routine can then insert the message into a message of its own.
*
* Results:
*
*   A pointer to the formatted message which is a static buffer in this
*   routine.  The caller must use the message before calling this routine
*   again.
*
* Side Effect:
*
*   In all cases this routine returns the formatted error in its own
*   static buffer.
*
*---------------------------------------------------------------------------*/
char * pmspi_errormsg(
   int  	pmspi_error,	/* IN: prev pmspi error # <> 0 */
   short	error1,		/* IN: additional error number */
   short	error2)		/* IN: additional error number */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_getversion
*   Obtains the version of the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
short pmspi_getversion(
   short	*error,		/* OUT: error encountered if any */
   short	*info,		/* OUT: error information */
   short	*detail)	/* OUT: error detail */

;
extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_writePMInfo
*   writes the Pathmon and Pathway info to the backup configuration file.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_writePMInfo(
   char			*cPath,		/* Configuration directory */
   short		backupcpu,	/* IN: if 0..15, PATHMON's backup cpu */
   short		maxscs)		/* IN: # of (user) SC's for this node */
;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_coldstart
*   Performs a coldstart of the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_coldstart(
   short		backupcpu,	/* IN: if 0..15, PATHMON's backup cpu */
   short		maxscs,		/* IN: # of (user) SC's for this node */
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_shutdown
*   Performs a shutdown of the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_shutdown(
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_addsc
*   Adds the indicated SERVERCLASS to the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_addsc(
   struct serverClassInfo_t *sc,	/* general information about the serverclass */
   struct arglist_t *arglist,		/* IN: arglist to be passed to the process */
   struct env_t		*envlist,		/* IN: environmental variables to be passed
   										   to the process */
   short		*info,				/* OUT: error information */
   short		*detail)			/* OUT: error detail */

;
extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_writeServerConfig
*   Writes the server configuration
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_writeServerConfig(
   char *cPath,						/* the configuration directory */
   struct serverClassInfo_t *sc,	/* general information about the serverclass */
   struct arglist_t *arglist,		/* IN: arglist to be passed to the process */
   struct env_t		*envlist)		/* IN: environmental variables to be passed
   										   to the process */
;
extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_deletesc
*   Deletes the indicated SERVERCLASS from the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_deletesc(
   char			*sc_name,	/* IN: valid TS/MP server class name */
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_freezesc
*   Freezes the indicated SERVERCLASS for the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_freezesc(
   char			*sc_name,	/* IN: valid TS/MP server class name */
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_thawsc
*   Thaws the indicated SERVERCLASS for the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_thawsc(
   char			*sc_name,	/* IN: valid TS/MP server class name */
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_stopsc
*   Stops the indicated SERVERCLASS for the appropriate PATHMON.  It would be
*   nice to have this function do it all, but the caller has more information
*   about the SERVERCLASS (e.g., how many processes are executing, etc), and
*   its time requirements.  So, if the STOP doesn't work the first time, this
*   routine will AT MOST try a FREEZE SC, wait 1 second, and rety the STOP SC,
*   wait another second, and retry the STOP SC one last time.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_stopsc(
   char			*sc_name,	/* IN: valid TS/MP server class name */
   short		*info,		/* OUT: error information */
   short		*detail)	/* OUT: error detail */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_startsc
*   Starts the SERVERCLASS under the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_startsc(
   char			*sc_name,	/* IN: valid TS/MP server class name */
   short		*err1,		/* OUT: 0 or error encountered */
   short		*err2)		/* OUT: 0 or additional error code */

;

extern
/*EXPORT----------------------------------------------------------------------
*
* pmspi_statussc
*   Performs a STATUS SERVER sc_name on the appropriate PATHMON.
*
* Results:
*
*   <> 0 -- Error encountered trying to get the information, no output
*           parameter should be considered valid.
*      0 -- Status was obtained
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_statussc(
   char 		*sc_name,	/* IN: valid TS/MP server class name */
   short		*freezestate,   /* OUT: FREEZESTATE of the server */
   short		*error,		/* OUT: 0 or error encountered */
   short		*info,		/* OUT: 0 or additional error code */
   short		*detail,	/* OUT: 0 or additional error code */
   short		*running)	/* OUT: # of servers running */

;
extern
/*EXPORT**************************************************************************
 * pmspi_addDefine
 *
 * Adds a define to the appropriate serverclass.
 *
 * Results:
 * <> 0 -- Error occurred adding define.
 *    0 -- Define was added.
 *
 *------------------------------------------------------------------------------*/
 int pmspi_addDefine(
 	char *sc_name,		/* IN: valid TS/MP server class name */
	char *defineName, 	/* IN: name of the define */
	struct defineItem_t *defineInfo,	/* IN: the full define */
	short *info,
	short *detail)
;

#endif
