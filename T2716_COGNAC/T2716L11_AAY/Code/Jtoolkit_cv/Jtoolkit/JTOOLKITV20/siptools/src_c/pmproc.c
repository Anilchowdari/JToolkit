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
/* End Exported Data */

/* Implementation dependencies */
#include <assert.h> nolist
#include <stdio.h>  nolist
#include <string.h>
#include <tal.h>    nolist
#include "error.h"  nolist
#ifndef _GUARDIAN_HOST
#include <cextdecs.h>
#else
#include <cextdecs> nolist
#endif  /* _GUARDIAN_HOST */
#include "pmproc.h" nolist

/* Begin Exported Data */
#ifndef _pmproc
#define _pmproc

/* Interface dependencies */

/* Exported type and data declarations */

/* End Exported Data */
#endif

/* Private type and data declarations */
#pragma fieldalign shared2 cimsg_t
   struct	cimsg_t {
      short		msgcode;
      short		def[8];
      char		in[24];
      char		out[24];
   };
#pragma fieldalign shared2 assignmsg_t
   struct	assignmsg_t {
      short		msgcode;
      struct	{
	 	char		prognamelen;
	 	char		progname[31];
	 	char		filenamelen;
	 	char		filename[31];
      }			logicalunit;
      int		fieldmask;
      short		physicalsubvolname[8];
      char		physicalfname[8];
      short		primaryextent;
      short		secondaryextent;
      short		filecode;
      short		exclusionspec;
      short		accessspec;
      short		recordsize;
      short		blocksize;
   };


#pragma page "pmproc_getphandle()"
/*EXPORT----------------------------------------------------------------------
*
* pmproc_getphandle
*   Returns the phandle of the process (presumably a PATHMON) process on the
*   node specified with the given IPCKEY.
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

{
   short	pmnamelen;
   short	error;

   pmnamelen = strlen(initialSipInfo.pathmonName);
   error = FILENAME_TO_PROCESSHANDLE_(initialSipInfo.pathmonName, pmnamelen, phandle->data);
   return (error == 0);
}

#pragma page "pmproc_getpriority()"
/*EXPORT----------------------------------------------------------------------
*
* pmproc_getpriority
*   Returns the priority of the process (presumably a PATHMON) process on the
*   node specified with the given IPCKEY.
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

{
   PHandle_t	phandle;
   short	pri;
   short	error;

   *priority = -1;
   if (pmproc_getphandle(&phandle)) {
      error = PROCESS_GETINFO_(phandle.data,,,, &pri);
      if (!error) {
	 	*priority = pri;
	 	return TRUE;
      }
   }
   return FALSE;
}

#pragma page "pmproc_create()"
/*EXPORT----------------------------------------------------------------------
*
* pmproc_create
*   Starts a PATHMON using the information contained in initialSipInfo.
*   The GSUBVOL is first checked for an executable PATHMON, and if not
*   found, the routine just trys to create a process using
*   $SYSTEM.SYSTEM.PATHMON.  It also sends the startup message, and
*   the ASSIGN message mapping the PATHCTL file to <pathmonNameCTL> in the
*   GSUBVOL.  The pathmon name used is the simple name with the $ and any
*   node information removed.
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
   char			*pathmonfname,  	/* OUT: name used in the start attempt */
   short		*error,			/* OUT: error encountered if problems */
   short		*error_detail)	/* OUT: additional info is some cases */

{
#define	OBJECTFILE	100	/* file code of a Guardian object file */
#define PDESCMAXLEN	49
   char			pathmonswap[36] = "";
   char			pathmonextswap[36] = "";
   short		guardianSubVolLen;
   short		pri = -1;
   short		processor = -1;
   PHandle_t	phandle;
   const short	runnamed = 1;
   char 		name[6];
   int			namelen;
   char			pdesc[PDESCMAXLEN+1];
   short		pdesclen;
   char			pathmonhometerm[26] = "";
   short		create_opts = 0;
   short		debug_opts = 0;
   short		pfs_size = 0;
   short		typeinfo[5];
   short		fnum;
   short		countwritten;
   int			i;
   struct cimsg_t	cimsg;
   struct assignmsg_t	assignmsg;
   getSimplePathmonName(name);
   namelen = strlen(initialSipInfo.pathmonName);
   processor = (short)initialSipInfo.pmCpu;
   if(initialSipInfo.hometerm[0] != '\0') {
   		if(strcmp(initialSipInfo.hometerm,"none") != 0) {
   			strcpy(pathmonhometerm,initialSipInfo.hometerm);
		} else {
			pathmonhometerm[0] = '\0';
		}
   } else {
   		pathmonhometerm[0] = '\0';
   }

   /*-----------------------------------------------
    * We first look for PATHMON in the GSUBVOL to
    * allow multiple versions of PATHMON to be used.
    * However, if the file isn't there, or not an
    * object file, then we use what's in
    * $SYSTEM.SYSTEM on the appropriate node.
    *----------------------------------------------*/
   strcpy(pathmonfname, initialSipInfo.pmonSubVol);
   strcat(pathmonfname, ".pathmon");
   *error = FILE_GETINFOBYNAME_(pathmonfname, (short)strlen(pathmonfname),
				typeinfo);
   if ((*error) || (typeinfo[4] != OBJECTFILE)) {
      strcpy(pathmonfname, "$system.system.pathmon");
   }  /* if info not available or file isn't an object file */
   *error = PROCESS_CREATE_(pathmonfname, (short)strlen(pathmonfname),,,
			    pathmonswap, (short)strlen(pathmonswap),
			    pathmonextswap, (short)strlen(pathmonextswap),
			    pri, processor, phandle.data, error_detail,
			    runnamed, initialSipInfo.pathmonName, (short)namelen,
			    (char *)&pdesc, PDESCMAXLEN, &pdesclen,,
			    pathmonhometerm,(short)strlen(pathmonhometerm),,,
				create_opts,,, debug_opts, pfs_size);
   if (*error) {
      return FALSE;
   }
   *error = FILE_OPEN_(pdesc, pdesclen, &fnum);
   if (fnum == -1) {
      PROCESS_STOP_(phandle.data);
      *error_detail = 0;
      return FALSE;
   }
   else {
      cimsg.msgcode = -1;

      /*-------------------------------------------------
       * The following call technically outputs 24 chars
       * while we only have room for 16, but there is
       * space to hold the data.  Just don't fill in the
       * cimsg.in until after this call.
       *------------------------------------------------*/
	   guardianSubVolLen = (short)strlen(initialSipInfo.pmonSubVol);
	  *error = FILENAME_TO_OLDFILENAME_(initialSipInfo.pmonSubVol,
	  		guardianSubVolLen,  cimsg.def);
      assert (*error == 0);
      /*--------------------------------------------------
       * PATHMON doesn't use the IN file, and we want to
       * set the OUT file to be <simplePathmonName>LOG in the
       * appropriate GSUBVOL.
       *-------------------------------------------------*/
      for (i = 0; i < 24; i++) {
	 	cimsg.in[i] = ' ';
      }
      /*-----------------------------------------
       * We already created the <simplePathmonName> prefix
       * for the process name (but it contains a
       * leading '$'), so just use it.
       *----------------------------------------*/
	  namelen = strlen(name);
	  if(namelen > 5)
	  	namelen = 5;
	  memset(cimsg.out,' ',sizeof(cimsg.out));
      memcpy(cimsg.out, cimsg.def, 16);
      memcpy((char *)&cimsg.out[16], (char *)&name[0], namelen);
      memcpy((char *)&cimsg.out[16 + namelen], "LOG", 3);
      *error = WRITEX(fnum, (char *)&cimsg, sizeof(cimsg), &countwritten);
      (void) FILE_GETINFO_(fnum, error);
      /*--------------------------------------------------
       * This maybe overkill, but PATHMON should reply
       * with an error 70 indicating that we can send any
       * ASSIGN and PARAM messages if we want.  If we
       * don't get the error, then assume we aren't
       * talking with a real PATHMON and blow it away.
       *-------------------------------------------------*/
      if (*error != FECONTINUE) {
	 	PROCESS_STOP_(phandle.data);
	 	*error = -1;
	 	*error_detail = 0;
	  return FALSE;
      }
      /*--------------------------------
       * Now pass the ASSIGN messages...
       *-------------------------------*/
      assignmsg.msgcode = -2;
      assignmsg.logicalunit.prognamelen = 0;
      for (i = 0; i < 31; i++) {
	 	assignmsg.logicalunit.progname[i] = ' ';
      }
      assignmsg.logicalunit.filenamelen = 7;
      strcpy(assignmsg.logicalunit.filename, "PATHCTL");
      assignmsg.fieldmask = 0x80000000;
      *error = FILENAME_TO_OLDFILENAME_(initialSipInfo.pmonSubVol,
	  		guardianSubVolLen, assignmsg.physicalsubvolname);
	  if(*error != 0)
	  	return FALSE;
	  memset(assignmsg.physicalfname,' ',sizeof(assignmsg.physicalfname));
      memcpy(assignmsg.physicalfname, (char *)&name[0], namelen);
      memcpy((char *)&assignmsg.physicalfname[namelen], "CTL", 3);
      *error = WRITEX(fnum, (char *)&assignmsg, sizeof(assignmsg), &countwritten);
	  (void)FILE_GETINFO_(fnum, error);
      /*------------------------------------------
       * It may be okay if PATHMON doesn't get our
       * ASSIGN message, but for now assume that's
       * a fatal flaw.
       *-----------------------------------------*/
      if ((*error != FEOK) && (*error != FECONTINUE)) {
	 	PROCESS_STOP_(phandle.data);
		*error = 1017;
	 	*error_detail = 0;
	 	return FALSE;
      }
   }
   FILE_CLOSE_(fnum);
   return TRUE;
}

#pragma page "pmproc_running()"
/*EXPORT----------------------------------------------------------------------
*
* pmproc_running
*   Determines if a process (presumably a PATHMON) is already running on the
*   node specified with the given name.
*
* Results:
*   TRUE  -- A process with the name is present on the node
*   FALSE -- A process with the name isn't present
*
* Side Effect:
*
*---------------------------------------------------------------------------*/

int pmproc_running(
)

{
   PHandle_t	phandle;

   return(pmproc_getphandle(&phandle));
}

#pragma page "pmproc_stop()"
/*EXPORT----------------------------------------------------------------------
*
* pmproc_stop
*   Determines if a process (presumably a PATHMON) is already running on the
*   node specified with the given name, and then trys to stop the process.
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

{
#define		BOTH_MEMBERS	1
   PHandle_t	phandle;

   if (pmproc_getphandle(&phandle))
      return((int)PROCESS_STOP_(phandle.data, BOTH_MEMBERS));
   return FENOTFOUND;
}
/**--------------------------------------------------------------------------
* Returns a simple version of the pathmon name removing the $ and any node
* information.
*--------------------------------------------------------------------------*/
void getSimplePathmonName(char *name)
{
	char pathmonName[26];
	char *ptr;

	strcpy(pathmonName,initialSipInfo.pathmonName);
	ptr = strtok(pathmonName,"$");
	strcpy(name,ptr);
	return;
}
/**---------------------------------------------------------------------------------
* char * pmproc_errormsg(
*	short error,			IN: error number
*	short error_detail)		IN: additional information
*
*	This routine takes the error numbers(s) returned from a previous call and formats
*	an error message.
*------------------------------------------------------------------------------------*/
char * pmproc_errormsg(short error, short error_detail)
{
	static char emsg[133];
	int emsglen;
	emsg[0] = '\0';
	emsglen = 133;

	switch(error) {
	case 1:
		sprintf(emsg,"%d: File system error %d on program file",error,error_detail);
		return emsg;
	case 2:
		sprintf(emsg,"%d: Parameter error - first param in error = %d",error,error_detail);
		return emsg;
	case 3:
		sprintf(emsg,"%d: Bounds error - parameter in error %d", error, error_detail);
		return emsg;
	case 4:
		sprintf(emsg,"%d: File system error %d on library file",error, error_detail);
		return emsg;
	case 5:
		sprintf(emsg,"%d: File system error %d on swap file",error, error_detail);
		return emsg;
	case 6:
		sprintf(emsg,"%d: File system error %d on extended swap file", error,error_detail);
		return emsg;
	case 7:
		sprintf(emsg,"%d: Error %d occurred while creating PFS", error, error_detail);
		return emsg;
	case 8:
		sprintf(emsg,"%d: Invalid home terminal specified", error);
		return emsg;
	case 9:
		sprintf(emsg,"%d: I/O error  %d to the home terminal",error, error_detail);
		return emsg;
	case 10:
		sprintf(emsg,"%d: Unable to communicate with system monitor, %d", error,error_detail);
		return emsg;
	case 11:
		sprintf(emsg,"%d: Invalid process name error %d", error, error_detail);
		return emsg;
	case 12:
		sprintf(emsg,"%d: Invalid program file format",error);
		return emsg;
	case 13:
		sprintf(emsg,"%d: Invalid library file format", error);
		return emsg;
	case 14:
		sprintf(emsg,"%d: Process has undefined externals but was started",error);
		return emsg;
	case 15:
		sprintf(emsg,"%d: Either no process control block or no PIN less than 255 available", error);
		return emsg;
	case 16:
		sprintf(emsg,"%d: Unable to allocate virtual address space", error);
		return emsg;
	case 17:
		sprintf(emsg,"%d: Unlicensed privileged program", error);
		return emsg;
	case 18:
		sprintf(emsg,"%d: Library conflict",error);
		return emsg;
	case 19:
		sprintf(emsg,"%d: Program file and library file are the same", error);
		return emsg;
	case 20:
		sprintf(emsg,"%d: Program file has an illegal process device subtype",error);
		return emsg;
	case 21:
		sprintf(emsg,"%d: Process device subtype in backup does not match primary",error);
		return emsg;
	case 22:
		sprintf(emsg,"%d: Backup creation was specified but caller is unnamed",error);
		return emsg;
	case 24:
		sprintf(emsg,"%d: DEFINE error %d", error, error_detail);
		return emsg;
	case 27:
		sprintf(emsg,"%d: PFS size is invalid",error);
		return emsg;
	case 29:
		sprintf(emsg,"%d: Unable to allocate priv stack for the process",error);
		return emsg;
	case 30:
		sprintf(emsg,"%d: Unable to lock the priv stack for the process",error);
		return emsg;
	case 31:
		sprintf(emsg,"%d: Unable to allocate a main stack for the process",error);
		return emsg;
	case 33:
		sprintf(emsg,"%d: Security inheritance failure",error);
		return emsg;
	case 35:
		sprintf(emsg,"%d: Internal process creation failure", error);
		return emsg;
	case 36:
		sprintf(emsg,"%d: Child's PFS error %d", error, error_detail);
		return emsg;
	case 37:
		sprintf(emsg,"%d: Unable to allocate global data for process",error);
		return emsg;
	case 40:
		sprintf(emsg,"%d: Main stack value is too large",error);
		return emsg;
	case 41:
		sprintf(emsg,"%d: The heap maximum value is too large",error);
		return emsg;
	case 42:
		sprintf(emsg,"%d: The space guarantee value is too large",error);
		return emsg;
	case 44:
		sprintf(emsg,"%d: Unable to find an SRL specified by the program file, error %d",
			error, error_detail);
		return emsg;
	case 45:
		sprintf(emsg,"%d: Unable to find SRL specified by another SRL %d", error, error_detail);
		return emsg;
	case 47:
		sprintf(emsg,"%d: Program file requires fixups to SRL %d", error, error_detail);
		return emsg;
	case 48:
		sprintf(emsg,"%d: An SRl requires fixups to another SRL %d", error, error_detail);
		return emsg;
	case 49:
		sprintf(emsg,"%d: Security violation. An SRL used by the program is licensed", error);
		return emsg;
	case 50:
		sprintf(emsg,"%d: Security violation. An SRL used by the program is not licensed", error);
		return emsg;
	case 56:
		sprintf(emsg,"%d: Internal error", error);
		return emsg;
	case 57:
		sprintf(emsg,"%d: An SRL has undefined externals %d is the SRL number",error, error_detail);
		return emsg;
	case 58:
		sprintf(emsg,"%d: Internal error", error);
		return emsg;
	case 59:
		sprintf(emsg,"%d: Internal error", error);
		return emsg;
	case 60:
		sprintf(emsg,"%d: Security violation. An SRL contain callable procedures must be licensced",error);
		return emsg;
	case 1017:
		sprintf(emsg,"%d: Error occurred opening the pathctl file; may be security error or space may not be available on the volume",error);
		return emsg;
	default:
		sprintf(emsg,"%d: File Creation error occurred; detail %d", error, error_detail);
		return emsg;
	}
}



