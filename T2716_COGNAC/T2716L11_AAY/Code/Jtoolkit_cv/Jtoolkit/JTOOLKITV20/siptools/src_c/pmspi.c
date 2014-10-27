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
/* End Exported Data */

/* Implementation dependencies */
#include <assert.h>   nolist
#include <string.h>   nolist
#include <stdio.h>    nolist
#include <tal.h>      nolist
#include <cextdecs.h> nolist
#include "error.h"    nolist
#include <errno.h>    nolist
#include <unistd.h>   nolist
#include "pmspi.h"    nolist
#include "cgi_sip.h"  nolist
#include <tdmext.h>   nolist
/* Begin Exported Data */
#ifndef _pmspi
#define _pmspi

/* Interface dependencies */

/* Exported type and data declarations */
/* End Exported Data */
#endif

/* Private type and data declarations */


/*---------------------------------------------
 * This declaration probably won't be useful now
 *---------------------------------------------*/
#define MAXPATHMONS	16

zpwy_ddl_msg_buffer_def	spibuf;
#define SPIBUFLEN	sizeof(struct __zpwy_ddl_msg_buffer)

#pragma nowarn (74)
zpwy_val_ssid_def	zpwy_val_ssid = { ZSPI_VAL_TANDEM,
   					  ZSPI_SSN_ZPWY,
					  ZPWY_VAL_VERSION };
#pragma warn (74)

struct bytestring_t {
   zspi_ddl_int_def	len;		/* # of bytes that follow */
   zspi_ddl_byte_def	data[1];	/* data */
};

struct cwd_t {
   zspi_ddl_int_def	len;			/* # of bytes that follow */
   zspi_ddl_byte_def	data[MAXSTRING+1];	/* data */
};

struct programoss_t {
   zspi_ddl_int_def	len;			/* # of bytes that follow */
   zspi_ddl_byte_def	data[MAXSTRING+1];	/* data */
};

struct scstderr_t {
	zspi_ddl_int_def 	len;
	zspi_ddl_byte_def	data[1024];
};

struct scstdin_t {
	zspi_ddl_int_def	len;
	zspi_ddl_byte_def 	data[1024];
};

struct scstdout_t {
	zspi_ddl_int_def	len;
	zspi_ddl_byte_def	data[1024];
};


/*----------------------------------------------
 * The following are used to START the PATHWAY
 * system on each node.
 *---------------------------------------------*/
#define ZMAXASSIGNS			0
#define ZMAXPARAMS			0
#define MASTERSYSPROCS		18
#define NONMASTERSYSPROCS 	17
#define ZMAXSERVERPROCESSES	4095
#define ZMAXSTARTUPS		0
#define ZMAXTCPS			0
#define ZMAXTERMS			0
#define ZMAXEXTERNALTCPS	0
#define ZMAXLINKMONS		150
#define ZMAXPATHCOMS		5
#define ZMAXPROGRAMS		0
#define ZMAXSPI                         5
#define ZMAXTELLQUEUE		0
#define ZMAXTELLS			0
#define ZMAXTMFRESTARTS		0
#define ZMAXDEFINES			4095

/*----------------------------------------------
 * The following are used when adding a
 * distributor server.
 *---------------------------------------------*/
#define	DIST_AUTORESTART	3
#define	DIST_HIGHPIN		ZPWY_VAL_ON
#define DIST_CREATEDELAY 5
#define DIST_MAXLINKS  16
#define	DIST_MAXSERVERS		1
#define	DIST_NUMSTATIC		1
#define	DIST_TMF		ZPWY_VAL_ON

/*----------------------------------------------
 * The following are used when adding the
 * SIP servers.
 *---------------------------------------------*/
#define	SIP_AUTORESTART	3
#define SIP_CREATEDELAY	0
#define	SIP_HIGHPIN		ZPWY_VAL_ON
#define SIP_MAXLINKS	16       /* unless MAXSERVERS = 1 -> unlimited */
#define U_W_PERM		0200	/* write permission bit for "user" */
#define G_W_PERM		0020	/* write permission bit for "group" */
#define O_W_PERM		0002	/* write permission bit for "other" */
#define	SIP_TMF		ZPWY_VAL_ON

#pragma page "pmspi_setpriority()"
/*---------------------------------------------------------------------------
*
* pmspi_setpriority
*   Retrieves the priority of the process and sets it in the appropriate
*   PATHMON structure.
*
* Results:
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
void pmspi_setpriority()
{
   short pmnamelen;
   PHandle_t	phandle;
   short	pri;
   short	error;

   pmnamelen = (short)strlen(pathmon_T.pathmonname);
   error = FILENAME_TO_PROCESSHANDLE_(pathmon_T.pathmonname, pmnamelen,
   		phandle.data);
   if (!error) {
      error = PROCESS_GETINFO_(phandle.data,,,, &pri);
      if (!error) {
	 	pathmon_T.priority = pri;
      }
   }
}

#pragma page "pmspi_pm()"
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
   char		*pmName,	/* IN: pathmon name to be set */
   char		*nodename)	/* IN: Node name where PATHMON resides */
{
   assert(pathmon_T.fnum == -1);
   sprintf(pathmon_T.pathmonname,"%s.%s.#ZSPI",nodename,pmName);
   strcpy(pathmon_T.node, nodename);
   pathmon_T.fnum = -1;
   pathmon_T.error = 0;
   pathmon_T.version = 0;
   pathmon_T.priority = -1;
}

#pragma page "pmspi_available()"
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
{
   short	error;
   short	info;
   short	detail;

   if (pathmon_T.fnum > -1)
      return TRUE;
   if ((pathmon_T.fnum == -1) &&
       (pathmon_T.error != 0))
      return FALSE;
   (void) pmspi_getversion(&error, &info, &detail);
   return (error == 0);
}

#pragma page "pmspi_open()"
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
{
   const short	nowait = 1;
   const short	sync_depth = 1;
   const short	nowait_open = 0x4000;
   const long	timeout = 6000l;
   short	error;
   short	fierr;

   short	fnum;

   error = FILE_OPEN_(pathmon_T.pathmonname,
   			(short)strlen(pathmon_T.pathmonname), &fnum,,,
		      nowait, sync_depth, nowait_open);
   if (!error) {
      /*
       * In D30.02 interrupts started breaking processes out
       * of AWAITIOX.  The following is the simplest change
       * to avoid this behavior.  I realize the following
       * doesn't account for any time already spent waiting
       * on AWAITIOX if we reissue the call, but it's better
       * than waiting indefinitely, and can be enhanced in
       * a future release.
       */
      while (_status_ne(AWAITIOX(&fnum,,,, timeout))) {
	 	if ((fierr = FILE_GETINFO_(fnum, &error)) != FEOK)
	    	error = fierr;
	 	if (error != EINTR) {
	    	(void) FILE_CLOSE_(fnum);
	    	fnum = -1;
	    	break;
	 	}
      }
   }
   pathmon_T.fnum = fnum;
   pathmon_T.error = error;
   return (int)error;
}

#pragma page "pmspi_close()"
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

{
   if (pathmon_T.fnum != -1) {
      pathmon_T.error = FILE_CLOSE_(pathmon_T.fnum);
      pathmon_T.fnum = -1;
      return (int)pathmon_T.error;
   }
   return 0;
}

#pragma page "pmspi_sendbuf()"
/*----------------------------------------------------------------------------
*
* pmspi_sendbuf
*   Sends the (valid) SPI buffer to the appropriate PATHMON.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
static short pmspi_sendbuf(
   short		*spibuf,	/* IN/OUT: SPI buffer to send/receive */
   short		maxspibuflen,	/* IN: max size of spibuf */
   short		*ssn,		/* OUT: ZSPI_SSN of the subsystem if */
   					/*      an error is returned.        */
   short		*info,		/* OUT: optional error info, or 0 */
   short		*detail)	/* OUT: optional error detail, or 0 */

{
   long			timeout = -1l;  /* current thinking is wait forever */
   short		error;
   short		fierr;
   short		usedlen;
   short		retcode;
   short		awaitfnum;
   zpwy_ddl_errinfo2_def errinfo2;

   *ssn = ZSPI_SSN_NULL;
   *info = *detail = 0;
   if (pathmon_T.fnum == -1) {
      pathmon_T.version = 0;
      if (error = (short)pmspi_open()) {
	 	*ssn = ZSPI_SSN_ZFIL;
	 	return error;
      }
   }
   error = SSGETTKN(spibuf, ZSPI_TKN_USEDLEN, (char *)&usedlen);
   assert(error == 0);
   WRITEREADX(pathmon_T.fnum, (char *)spibuf, usedlen,
	      ZPWY_VAL_BUFLEN);
   /*
    * In D30.02 interrupts started breaking processes out
    * of AWAITIOX.  The following is the simplest change
    * to avoid this behavior.  In a future release this
    * can be enhanced to allow the user to "break out"
    * of waiting on PATHMON, but for now the rest of the
    * code just isn't ready for the change.
    */
   do {
      error = FEOK;
      awaitfnum = pathmon_T.fnum;
      if (_status_ne(AWAITIOX(&awaitfnum,,,, timeout))) {
	 	if ((fierr = FILE_GETINFO_(awaitfnum, &error)) != FEOK)
	    	error = fierr;
	 	if (error != EINTR) {
	    	/*
	     	 * If a "hard" error occurs close this session.
	     	*/
	    	if (error == FEPATHDOWN)
	       		(void) pmspi_close();
	    	*ssn = ZSPI_SSN_ZFIL;
	    	return error;
	 	}
      }
   } while (error != FEOK);
   error = SSPUTTKN(spibuf, (long)ZSPI_TKN_RESET_BUFFER,
		    (char *)&maxspibuflen);
   if (error) {
      *ssn = ZSPI_SSN_ZSPI;

/*      // Begin Changes for Solution Number 10-040512-5972

        // Closing the appropriate PATHMON for SPI access.

     (void) pmspi_close();

     // End Changes for Solution Number 10-040512-5972 */


      return error;


   }
   error = SSGETTKN(spibuf, ZSPI_TKN_RETCODE, (char *)&retcode, 1);
   if (error)
      *ssn = ZSPI_SSN_ZSPI;
   else if (retcode > 0) {
	 	*ssn = ZSPI_SSN_ZPWY;
	 	error = SSGETTKN(spibuf, ZSPI_TKN_ERRLIST,, 1);
	 	/*-------------------------------------------------------
	  	* There appears to be only two tokens that are returned
	  	* which will give additional error information.  Instead
	  	* of trying to special case based on error number, let's
	  	* just try to get the most useful error token first, and
	  	* if it isn't present, try for the less useful token.
	  	* If neither is present don't treat it as an error
	  	* because the command may not return additional info.
	  	*------------------------------------------------------*/
	 	if (error == 0) {
	    	error = SSGETTKN(spibuf, ZPWY_TKN_ERRINFO2, (char *)&errinfo2, 1);
	    	if (error == 0) {
	       		*info = errinfo2.zinfo2;
	       		*detail = errinfo2.zdetail2;
	    	} else if (error == ZSPI_ERR_MISTKN) {
		  		*detail = 0;
		  		error = SSGETTKN(spibuf, ZPWY_TKN_ERRINFO, (char *)info, 1);
		  		if (error != 0)
		     		*info = 0;
	       	}
	 	}
	 	error = retcode;
   }

   // Begin Changes for Solution Number 10-040512-5972 

   // Closing the appropriate PATHMON for SPI access.

   (void) pmspi_close();

   // End Changes for Solution Number 10-040512-5972

   return error;
}

#pragma page "pmspi_errormsg()"
/*EXPORT----------------------------------------------------------------------
*
* pmspi_errormsg
*   This routine takes the error number(s) returned from
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
*
*---------------------------------------------------------------------------*/
char * pmspi_errormsg(
   int  	pmspi_error,	/* IN: prev pmspi error # <> 0 */
   short	error1,		/* IN: additional error number */
   short	error2)		/* IN: additional error number */

{
#define		MAXMSG	132
   static char	emsg[MAXMSG+1];
   int		emsglen;
   /*----------------------------------------------------------
    * There are three types of errors that can be returned from
    * a pmspi routine: SPI, FS, or TS/MP.  Luckily the three
    * error ranges don't overlap, so we can just use the error
    * number.  However, if this should change, the pmspi
    * routines do keep an internal SSN (subsystem number) that
    * could be used to solve the overlap problem.
    *---------------------------------------------------------*/
   assert(pmspi_error != 0);
   emsg[0] = '\0';
   emsglen = MAXMSG+1;
   if (pmspi_error < 0) {
      (void) sprintf(emsg,"SPI error %hd occurred", pmspi_error);
      return emsg;
   }
   if ((pmspi_error > 0) && (pmspi_error <= FESCLASTERROR)) {
      switch (pmspi_error) {
	 	case FENOSUCHDEV:
			sprintf(emsg,"%d: PATHMON is not running",FENOSUCHDEV);
			return emsg;
	 	case FETIMEDOUT:
	    	sprintf(emsg,"%d: operation timed out",FETIMEDOUT);
			return emsg;
	 	case FEPATHDOWN:
	    	sprintf(emsg,"%d: PATHMON has stopped",FEPATHDOWN);
			return emsg;
	 	case FENETFAIL:
	 	case FENETDOWN:
	    	sprintf (emsg, "network failure");
	 	default:
	    	(void) sprintf(emsg, "FS error %d occurred", pmspi_error);
	    	return emsg;
      }
   }
   /*--------------------------------------
    * To get here it must be a TS/MP error.
    *-------------------------------------*/
   switch (pmspi_error) {
      case ZPWY_ERR_PM_ALREADYSTARTED:
	  	sprintf(emsg,"%d: pathway already started",ZPWY_ERR_PM_ALREADYSTARTED);
	 	return emsg;
      case ZPWY_ERR_PM_ALREADYSTOPPED:
	  	sprintf(emsg,"%d: pathway already stopped",ZPWY_ERR_PM_ALREADYSTOPPED);
		return emsg;
	  case ZPWY_ERR_PM_DUPNAME:
	  	sprintf(emsg,"%d: Either a PATHMON or a serverclass with this name already exists",
			ZPWY_ERR_PM_DUPNAME);
	 	return emsg;
      case ZPWY_ERR_PM_PATHCTLIO:
	 	(void) sprintf(emsg,"%d: PATHMON cannot access its control file, FS error %hd",
			ZPWY_ERR_PM_PATHCTLIO,error1);
	 	return emsg;
      case ZPWY_ERR_PM_NODISKSPACE:
	  	(void) sprintf(emsg, "%d: Pathway control file is full",ZPWY_ERR_PM_NODISKSPACE);
		return emsg;
      case ZPWY_ERR_PM_NOFREESPACE:
	  	sprintf(emsg,"%d: PATHMON freespace table is full",ZPWY_ERR_PM_NOFREESPACE);
		return emsg;
      case ZPWY_ERR_PM_NAMENOTDEFINED:
	    sprintf(emsg,"%d: Requested object does not exist",ZPWY_ERR_PM_NAMENOTDEFINED);
		return emsg;
      case ZPWY_ERR_PM_STATEMISMATCH:
	 	sprintf(emsg,"%d: object not in the correct state",ZPWY_ERR_PM_STATEMISMATCH);
		return emsg;
      case ZPWY_ERR_PM_FSERROR:
	 	(void) sprintf(emsg, "%d: PATHMON reported FS error %d",ZPWY_ERR_PM_FSERROR, error1);
	 	return emsg;
      case ZPWY_ERR_PM_ALREADYFROZEN:
	 	sprintf(emsg,"%d: Serverclass already frozen",ZPWY_ERR_PM_ALREADYFROZEN);
		return emsg;
      case ZPWY_ERR_PM_FREEZEINPROGRESS:
	  	sprintf(emsg,"%d: A serverclass freeze is already in progress",ZPWY_ERR_PM_FREEZEINPROGRESS);
		return emsg;
      case ZPWY_ERR_PM_FREEZEPENDING:
	 	sprintf(emsg,"%d: A serverclass freeze request is pending",ZPWY_ERR_PM_FREEZEPENDING);
		return emsg;
      case ZPWY_ERR_PM_ALREADYTHAWED:
	 	sprintf(emsg,"%d:Serverclass is already thawed",ZPWY_ERR_PM_ALREADYTHAWED);
		return emsg;
      case ZPWY_ERR_PM_SCFROZEN:
	 	sprintf(emsg,"%d: Serverclass is frozen",ZPWY_ERR_PM_SCFROZEN);
		return emsg;
      case ZPWY_ERR_PM_MUSTBEFROZEN:
	 	sprintf(emsg,"Serverclass must be frozen");
		return emsg;
      case ZPWY_ERR_PM_MUSTBETHAWED:
	 	sprintf(emsg,"Serverclass must be thawed");
      case ZPWY_ERR_PM_SECURITY:
	 	sprintf(emsg,"%d: PATHMON security violation",ZPWY_ERR_PM_SECURITY);
		return emsg;
      case ZPWY_ERR_PM_INVALIDHOMETERM:
	 	sprintf(emsg,"%d: Invalid HOMETERM",ZPWY_ERR_PM_INVALIDHOMETERM);
		return emsg;
      case ZPWY_ERR_PM_TOOMANYTCP:
	 	sprintf(emsg,"Too many LINKMON's");
		return emsg;
      case ZPWY_ERR_PM_TOOMANYSC:
	 	sprintf(emsg,"%d: Too many SERVERCLASSES",ZPWY_ERR_PM_TOOMANYSC);
		return emsg;
      case ZPWY_ERR_PM_TOOMANYSCPROCESS:
	 	sprintf(emsg,"too many server processes");
		return emsg;
      case ZPWY_ERR_PM_SPIDISABLED:
	 	sprintf(emsg,"PATHMON's SPI interface is disabled");
		return emsg;
      case ZPWY_ERR_PM_IN_SHUTDOWN:
	 	sprintf(emsg,"TS/MP shutdown in progress");
		return emsg;
      case ZPWY_ERR_PM_SHUTDOWNFAILED:
	 	sprintf(emsg,"TS/MP shutdown failed");
		return emsg;
      case ZPWY_ERR_PM_OSSPROCESSFAILURE:
	 	switch (error1) {
	    	case _TPC_UC_FERROR:
	       		sprintf(emsg,"error accessing a.out");
	       		break;
	    	case _TPC_UL_FERROR:
	       		sprintf(emsg,"error accessing library");
	       		break;
	    	case _TPC_SWAP_FERROR:
	       		sprintf(emsg,"swap file error");
	       		break;
	    	case _TPC_EXTSWAP_FERROR:
	       		sprintf(emsg, "extended swap file error");
	       		break;
	    	case _TPC_BAD_HOMETERM_FERROR:
	       		sprintf(emsg, "illegal HOMETERM");
	       		break;
	    	case _TPC_HOMETERM_FERROR:
	       		sprintf(emsg, "I/O error to HOMETERM");
	       		break;
	    	case _TPC_SYSMON_FERROR:
	       		sprintf(emsg, "can't talk to sysmon");
	       		break;
	    	case _TPC_NO_PCB:
	       		sprintf(emsg, "no PCBS available");
	       		break;
	    	case _TPC_NO_VMEM:
	       		sprintf(emsg, "no virtual memory available");
	       		break;
	    	case _TPC_NO_LICENSE:
	       		sprintf(emsg, "unlicensed priv program");
	       		break;
	    	case _TPC_UCUL_CONFLICT:
	       		sprintf(emsg, "UC/UL conflicts");
	       		break;
	    	default:
	       		sprintf(emsg,"TPC error %d", error1);
	       	    break;
	 	}
	 if (error2 != 0) {
	    switch (error2) {
	       case FENOTFOUND:
		  		sprintf(emsg, "File(server object file) not found");
		  		break;
	       case FETIMEDOUT:
		  		sprintf(emsg, "operation timed out");
		  		break;
	       case FESECVIOL:
		  		sprintf(emsg, "security violation");
		  		break;
	       default:
		  		if (error2 >= EPERM) {
		     		(void) strcpy(emsg, strerror(error2));
		  		} else {
		     		sprintf(emsg,"TPC detail %d",error2);
		        }
				break;
	    }
	 }
	 return emsg;
      case ZPWY_ERR_SPI_INVALIDPATHNAME:
	 	sprintf(emsg, "Invalid value specified for JAVA_HOME");
		return emsg;
      case ZPWY_ERR_SPI_TOOMANYDEFSERVERS:
	 	sprintf(emsg,"too many defined servers");
		return emsg;
      case ZPWY_ERR_SPI_RESERVEDWORD:
	 	sprintf(emsg,"Serverclass name is a TS/MP reserved word");
		return emsg;
      default:
	 /*---------------------------------------------------
	  * Only display error numbers that are non-zero with
	  * the assumption being that error1 is never non-zero
	  * if error1 is zero (though it won't hurt).
	  *--------------------------------------------------*/
	 if (error2)
	    (void) sprintf(emsg, "TS/MP error %d (%hd, %hd)",
			   pmspi_error, error1, error2);
	 else
	    if (error1)
	       (void) sprintf(emsg, "TS/MP error %d (%hd)",
			      pmspi_error, error1);
	    else
	       (void) sprintf(emsg, "TS/MP error %d",
			      pmspi_error);

	 return emsg;
   }
}

#pragma page "pmspi_getversion()"
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

{
   short	ssn;

   *error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZSPI_CMD_GETVERSION, ZPWY_OBJ_PM);
   assert(*error == 0);
   *error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			  detail);
   if (*error)
      return 0;
   *error = SSGETTKN((short *)&spibuf, ZSPI_TKN_SERVER_VERSION,
		    (char *)&pathmon_T.version);
   return pathmon_T.version;
}
#pragma page "pmspi_writePMInfo()"
/*EXPORT----------------------------------------------------------------------
*
* pmspi_writePMInfo
*   writes the Pathmon and Pathway information to the backup configuration file.
*
* Results:
*
*
* Side Effect:
*
*---------------------------------------------------------------------------*/
int pmspi_writePMInfo(
   char 		*cPath,		/* IN: the configuration directory */
   short		backupcpu,	/* IN: if 0..15, PATHMON's backup cpu */
   short		maxscs)	/* IN: # of (user) SC's for this node */

{
   	FILE *pfp;

   	pfp = fopen(cPath,"w");
   	if(pfp == NULL) {
		return -1;
	}
	fprintf(pfp,"SET PATHMON BACKUPCPU %d\n",backupcpu);
	fprintf(pfp,"SET PATHMON DUMP OFF\n");
	fprintf(pfp,"SET PATHWAY MAXASSIGNS %d\n",ZMAXASSIGNS);
	fprintf(pfp,"SET PATHWAY MAXDEFINES %d\n",ZMAXDEFINES);
	fprintf(pfp,"SET PATHWAY MAXEXTERNALTCPS %d\n",ZMAXEXTERNALTCPS);
	fprintf(pfp,"SET PATHWAY MAXLINKMONS %d\n",ZMAXLINKMONS);
	fprintf(pfp,"SET PATHWAY MAXPATHCOMS %d\n",ZMAXPATHCOMS);
	fprintf(pfp,"SET PATHWAY MAXPARAMS %d\n",ZMAXPARAMS);
	fprintf(pfp,"SET PATHWAY MAXPROGRAMS %d\n", ZMAXPROGRAMS);
	fprintf(pfp,"SET PATHWAY MAXSERVERCLASSES %d\n",MAXSCS);
	fprintf(pfp,"SET PATHWAY MAXSERVERPROCESSES %d\n",ZMAXSERVERPROCESSES);
	fprintf(pfp,"SET PATHWAY MAXSPI %d\n", ZMAXSPI);
	fprintf(pfp,"SET PATHWAY MAXSTARTUPS %d\n",ZMAXSTARTUPS);
	fprintf(pfp,"SET PATHWAY MAXTCPS %d\n", ZMAXTCPS);
	fprintf(pfp,"SET PATHWAY MAXTELLQUEUE %d\n", ZMAXTELLQUEUE);
	fprintf(pfp,"SET PATHWAY MAXTELLS %d\n", ZMAXTELLS);
	fprintf(pfp,"SET PATHWAY MAXTERMS %d\n", ZMAXTERMS);
	fprintf(pfp,"SET PATHWAY MAXTMFRESTARTS %d\n", ZMAXTMFRESTARTS);
	fprintf(pfp,"SET PATHWAY SECURITY \"N\"\n");
	fprintf(pfp,"START PATHWAY COLD!\n");
	fclose(pfp);
	return 1;
}
#pragma page "pmspi_coldstart()"
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

{
   short		error;
   short		ssn;
   long			nodenumber;
   short		*start_map = zpwy_map_par_start_pathway;
   zpwy_ddl_par_start_pathway_def	start_parm;
   short		*pathway_map = zpwy_map_def_pathway;
   zpwy_ddl_def_pathway_def		pathway_attrs;

   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_START, ZPWY_OBJ_PATHWAY);
   error = SSNULL(start_map, (char *)&start_parm);
   start_parm.zignore = ZPWY_VAL_YES;
   error = SSPUT((short *)&spibuf, start_map, (char *)&start_parm);
   error = SSNULL(pathway_map, (char *)&pathway_attrs);
   if (backupcpu >= 0 && backupcpu <= 15)
      pathway_attrs.zbackupcpu = backupcpu;
   pathway_attrs.zmaxassigns = ZMAXASSIGNS;
   pathway_attrs.zmaxparams = ZMAXPARAMS;
   pathway_attrs.zmaxserverclasses = maxscs;
   pathway_attrs.zmaxserverprocesses = ZMAXSERVERPROCESSES;
   pathway_attrs.zmaxstartups = ZMAXSTARTUPS;
   pathway_attrs.zmaxtcps = ZMAXTCPS;
   pathway_attrs.zmaxterms = ZMAXTERMS;
   pathway_attrs.zmaxdefines = ZMAXDEFINES;
   pathway_attrs.zmaxexternaltcps = ZMAXEXTERNALTCPS;
   pathway_attrs.zmaxlinkmons = ZMAXLINKMONS;
   pathway_attrs.zmaxpathcoms = ZMAXPATHCOMS;
   pathway_attrs.zmaxprograms = ZMAXPROGRAMS;
   pathway_attrs.zmaxspi = ZMAXSPI;
   pathway_attrs.zmaxtellqueue = ZMAXTELLQUEUE;
   pathway_attrs.zmaxtells = ZMAXTELLS;
   pathway_attrs.zmaxtmfrestarts = ZMAXTMFRESTARTS;
   /*---------------------------------------------------
    * PATHMON requires a system number, so give it ours.
    *--------------------------------------------------*/
   error = NODENAME_TO_NODENUMBER_(,, &nodenumber);
   assert(error == 0);
   /* FIX */
   /* pathway_attrs.zowner.zsystemnumber = (short)nodenumber;
      pathway_attrs.zowner.zgroupnumber = ((uid & 0xff00) >> 8);
   	  pathway_attrs.zowner.zusernumber = (uid & 0xff); */
   pathway_attrs.zsecurity = ZPWY_VAL_N;
   error = SSPUT((short *)&spibuf, pathway_map, (char *)&pathway_attrs);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   return (int)error;
}

#pragma page "pmspi_shutdown()"
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

{
   short		error;
   short		ssn;
   int  		escalate = 0;
   short		*shutdown_map = zpwy_map_par_shutdown;
   zpwy_ddl_par_shutdown_def	shutdown_parm;

retry:
   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_SHUTDOWN2, ZPWY_OBJ_PATHWAY);
   assert(error == 0);
   error = SSNULL(shutdown_map, (char *)&shutdown_parm);
   shutdown_parm.zmode = (escalate) ? ZPWY_VAL_IMMEDIATE : ZPWY_VAL_ORDERLY;
   error = SSPUT((short *)&spibuf, shutdown_map, (char *)&shutdown_parm);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   if ((error == ZPWY_ERR_PM_IN_SHUTDOWN) && (!escalate++)) {
      goto retry;
   }
   /*------------------------------------------
    * If we don't receive an error then PATHMON
    * is about to stop so go ahead and close
    * the file so that subsequent calls to
    * pmspi routines won't attempt to talk
    * to the same PATHMON process.
    *-----------------------------------------*/
   if (!error)
      (void) pmspi_close();
   return (int)error;
}
#pragma page "pmspi_writeServerConfig()"
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
   char *cPath,					 /* IN: path to the configuration file */
   struct serverClassInfo_t *sc, /* IN: general information about the serverclass */
   struct arglist_t	*argv,		 /* IN: arglist to be passed to the process */
   struct env_t     *envlist)    /* IN: environmental variables to be passed to the process */
{
  	FILE *pfp;
	char *ptr, *ptr1;
	char pArglist[4095];
	char Argline1[115];
	char ArglineRest[133];
	char envline1[120];
	char envlineRest[133];
	int count;
	int len, totalLen = 0;
	#define MAXARGLIST_LINE1 100
	#define MAXARGLIST_NEXT 131
	#define MAXENV_LINE1 110
	#define MAXENV_NEXT 131

   	pfp = fopen(cPath,"a");
   	if(pfp == NULL) {
		return -1;
	}
	fprintf(pfp,"RESET SERVER\n");
	fprintf(pfp,"SET SERVER PROCESSTYPE OSS\n");
    assert(argv->len > 0);
	ptr = (char *)&argv->data[0];
	len = strlen(ptr) + 1;
	totalLen += len;
	strcpy(pArglist,ptr);
	ptr += len;
	while(totalLen < argv->len) {
		strcat(pArglist,",");
		len = strlen(ptr) + 1;
		strcat(pArglist,ptr);
		ptr += len;
		totalLen += len;
	}
	/* Length of pArglist plus SET SERVER ARGLIST can't exceed 132 */
	count = strlen(pArglist);
	if(count > MAXARGLIST_LINE1) {
		strncpy(Argline1,pArglist,MAXARGLIST_LINE1);
		fprintf(pfp,"SET SERVER ARGLIST %s&\n",Argline1);
		count = count - MAXARGLIST_LINE1;
		ptr = (char *)&pArglist[MAXARGLIST_LINE1];
		while(count > 132) {
			strncpy(ArglineRest,ptr,MAXARGLIST_NEXT);
			fprintf(pfp,"%s&\n",ArglineRest);
			count = count - MAXARGLIST_NEXT;
			ptr += MAXARGLIST_NEXT;
		}
		if(count > 0) {
			fprintf(pfp,"%s\n",ptr);
		}
	} else {
		fprintf(pfp,"SET SERVER ARGLIST %s\n",pArglist);
	}


	if(envlist->len > 0) {
		totalLen = 0;
		ptr = (char *)&envlist->data[0];
		len = strlen(ptr) + 1;
		totalLen += len;
		count = strlen(ptr);
		if(count > MAXENV_LINE1) {
			strncpy(envline1,ptr,MAXENV_LINE1);
			fprintf(pfp,"SET SERVER ENV %s&\n",envline1);
			count = count - MAXENV_LINE1;
			ptr1 = ptr + MAXENV_LINE1;
			while(count > 132) {
				strncpy(envlineRest,ptr1,MAXENV_NEXT);
				fprintf(pfp,"%s&\n",ptr1);
				count = count - MAXENV_NEXT;
				ptr1 += 132;
			}
			if(count > 0) {
				fprintf(pfp,"%s\n",ptr1);
			}
		} else {
			fprintf(pfp,"SET SERVER ENV %s\n",ptr);
		}
		ptr += len;
		while(totalLen < envlist->len) {
			len = strlen(ptr) + 1;
			count = strlen(ptr);
			if(count > MAXENV_LINE1) {
				strncpy(envline1,ptr,MAXENV_LINE1);
				fprintf(pfp,"SET SERVER ENV %s&\n",envline1);
				count = count - MAXENV_LINE1;
				ptr1 = ptr + MAXENV_LINE1;
				while(count > 132) {
					strncpy(envlineRest,ptr1,MAXENV_NEXT);
					fprintf(pfp,"%s&\n",ptr1);
					count = count - MAXENV_NEXT;
					ptr1 += MAXENV_NEXT;
				}
				if(count > 0) {
					fprintf(pfp,"%s\n",ptr1);
				}
			} else {
				fprintf(pfp,"SET SERVER ENV %s\n",ptr);
			}
			ptr += len;
			totalLen += len;
		}
	}
		fprintf(pfp,"SET SERVER AUTORESTART %d\n", SIP_AUTORESTART);
	 	fprintf(pfp,"SET SERVER CREATEDELAY %d SECS\n",SIP_CREATEDELAY);
		fprintf(pfp,"SET SERVER HIGHPIN ON\n");
		fprintf(pfp,"SET SERVER NUMSTATIC 1\n");
		fprintf(pfp,"[DO NOT CHANGE THE FOLLOWING COMMAND]\n");
		fprintf(pfp,"SET SERVER MAXSERVERS 1\n");
		fprintf(pfp,"[DO NOT CHANGE THE FOLLOWING COMMAND]\n");
		fprintf(pfp,"SET SERVER MAXLINKS %d\n", SIP_MAXLINKS);
		ptr1 = (char *)&initialSipInfo.pathmonName[1];
		fprintf(pfp,"SET SERVER STDOUT %s/%s_%s.log\n",initialSipInfo.logPath, ptr1, sc->sc_name);
		fprintf(pfp,"SET SERVER STDERR %s/%s_%s.errlog\n",initialSipInfo.logPath,ptr1, sc->sc_name);
		fprintf(pfp,"SET SERVER TMF ON\n");
		fprintf(pfp,"SET SERVER DEFINE =TCPIP^PROCESS^NAME,CLASS MAP, FILE %s\n",serverInfo.tcpipProcessName);
		/* change made for solution 10-070910-7331 : adding define for ptcpip filter key */
		fprintf(pfp,"SET SERVER DEFINE =PTCPIP^FILTER^KEY,CLASS MAP, FILE %s\n",serverInfo.ptcpipkey);
	        fprintf(pfp,"SET SERVER DEBUG OFF\n");
		fprintf(pfp,"SET SERVER PRI 170\n");
		fprintf(pfp,"SET SERVER SECURITY \"N\"\n");
		fprintf(pfp,"SET SERVER VOLUME %s\n",sc->gsubvol);
		fprintf(pfp,"SET SERVER CWD %s\n",sc->cwddir);
		fprintf(pfp,"SET SERVER PROGRAM %s\n",sc->progname);
       	        fprintf(pfp,"SET SERVER CPUS (%d)\n",sc->cpulist);
		fprintf(pfp,"ADD SERVER %s\n",sc->sc_name);

		fclose(pfp);
	return 0;
}
#pragma page "pmspi_addsc()"
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
   struct serverClassInfo_t *sc, /* IN: general information about the serverclass */
   struct arglist_t	*argv,		 /* IN: arglist to be passed to the process */
   struct env_t     *envlist,    /* IN: environmental variables to be passed to the process */
   short		*info,			 /* OUT: error information */
   short		*detail)		 /* OUT: error detail */

{
#define	SECSPERMICROSECS 1000000
   short		error;
   short		ssn;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;
   short		*def_sc_map = zpwy_map_def_sc;
   zpwy_ddl_def_sc_def	def_sc;
   char			tmpfname[sizeof(zspi_ddl_fname_def)];
   struct cwd_t		cwd;
   struct programoss_t	programoss;
   struct env_t		env;
   struct arglist_t	arglist;
   struct scstderr_t	stdErr;
   struct scstdin_t		stdIn;
   struct scstdout_t    stdOut;
   char			*ptr1;


   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_ADD, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc->sc_name, strlen(sc->sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   /*--------------------------------------------------------
    * This is where the attributes are set.  For now let's do
    * all of the process types in this routine, with the
    * thought being that there's more similarities than
    * differences.
    *-------------------------------------------------------*/
   error = SSNULL(def_sc_map, (char *)&def_sc);
   def_sc.zprocesstype = ZPWY_VAL_OSS;
   error = FILENAME_TO_OLDFILENAME_(sc->gsubvol,
				    (short)strlen(sc->gsubvol),
				    (short *)&tmpfname);
   assert(error == 0);
   (void) memcpy((char *)&def_sc.zdefaultvolume, tmpfname,
		 sizeof(zspi_ddl_subvol_def));
   def_sc.zdebug = ZPWY_VAL_OFF;
   strcpy(cwd.data, sc->cwddir);
   cwd.len = strlen(cwd.data);
   strcpy(programoss.data, sc->progname);
   programoss.len = strlen(programoss.data);
   /*-------------------------------------------------
	* See if the user specifed CPUS by seeing if any
	* CPU's aren't selected.
	*------------------------------------------------*/
            def_sc.zcpuinfo.zcpupaircount = 0;
               if (sc->cpulist!=-1) {
                        def_sc.zcpuinfo.zcpupair[def_sc.zcpuinfo.zcpupaircount].
zprimary = sc->cpulist;
                        def_sc.zcpuinfo.zcpupaircount++;
               }

	 		def_sc.zautorestart = SIP_AUTORESTART;
	 		def_sc.zcreatedelay = SIP_CREATEDELAY * SECSPERMICROSECS;
	 		def_sc.zhighpin = SIP_HIGHPIN;
	 		def_sc.znumstatic = (short)1;

// Begin Changes for Solution Number 10-040512-5972 

/* the max servers for the server class is always 1 */

	 		def_sc.zmaxservers = (short)1;

// End Changes for Solution Number 10-040512-5972

	    	def_sc.zmaxlinks = SIP_MAXLINKS;
		    def_sc.ztmf = SIP_TMF;
			/* FIX */
			/* Set up stdin, stdout, and stderr for later use */
			strcpy(stdIn.data,"/dev/null");
			stdIn.len = strlen(stdIn.data);
			ptr1 = (char *)&initialSipInfo.pathmonName[1];
			sprintf(stdOut.data,"%s/%s_%s.log",sc->logpath,ptr1, sc->sc_name);
			stdOut.len = strlen(stdOut.data);
			sprintf(stdErr.data,"%s/%s_%s.errlog",sc->logpath, ptr1, sc->sc_name);
			stdErr.len = strlen(stdErr.data);
	def_sc.zsecurity = ZPWY_VAL_N;
    error = SSPUT((short *)&spibuf, def_sc_map, (char *)&def_sc);
    assert(error == 0);
    error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCCWD,
		    (char *)&cwd);
    assert(error == 0);
    error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCPROGRAMOSS,
		    (char *)&programoss);
    assert(error == 0);
	error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCSTDERR,
			(char *)&stdErr);
	assert(error == 0);
	error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCSTDIN,
			(char *)&stdIn);
	assert(error == 0);
	error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCSTDOUT,
			(char *)&stdOut);
	assert(error == 0);
	env.len = envlist->len;
    memcpy(env.data,envlist->data,env.len);
    error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCENV, (char *)&env);
    assert(error == 0);
    arglist.len = argv->len;
    memcpy(arglist.data,argv->data,arglist.len);
   	error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCARGLIST,
		    (char *)&arglist);
   	assert(error == 0);
   	error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   	return (int)error;
}

#pragma page "pmspi_deletesc()"
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

{
   short		error;
   short		ssn;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;


   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_DELETE, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   return (int)error;
}

#pragma page "pmspi_freezesc()"
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

{
   short		error;
   short		ssn;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;

   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_FREEZE, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   if (error == ZPWY_ERR_PM_ALREADYFROZEN)
      error = 0;
   return (int)error;
}

#pragma page "pmspi_thawsc()"
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

{
   short		error;
   short		ssn;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;

   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_THAW, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   return (int)error;
}

#pragma page "pmspi_stopsc()"
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

{
   short		error;
   short		ssn;
   short		tmp_info;
   short		tmp_detail;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;
   int  		retried_freeze = 0;
   int  		retried_stop = 0;

retry:
   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_STOP, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
   switch (error) {
      case ZPWY_ERR_PM_ALREADYSTOPPED:
	 	error = 0;
	 	break;
      case ZPWY_ERR_PM_MUSTBEFROZEN:
	 	/*-------------------------------------------------
	  	 * The caller should have done a FREEZE SC before
	  	 * calling us, but it may still be pending so try
	  	 * the FREEZE SC again.  If we get a freeze pending
	  	 * error then sleep for a second and try the STOP
	  	 * again.
	  	 *------------------------------------------------*/
	 	if (retried_freeze++)
	    	return error;
		error = pmspi_freezesc(sc_name, &tmp_info, &tmp_detail);
	 	if (error == ZPWY_ERR_PM_FREEZEINPROGRESS) {
	    	(void) sleep(1);
	 	}
	 	goto retry;
     case ZPWY_ERR_TCP_REQUESTPENDING:
	 	/*-----------------------------------------------
	  	 * Only retry the STOP one time, and then let the
	  	 * caller decide if and when it should retry the
	  	 * STOP SC.
	  	*----------------------------------------------*/
	 	if (!retried_stop++) {
	    	(void) sleep(1);
	    	goto retry;
	 	}
	 	break;
     default:
	 	break;
   }
   return (int)error;
}

#pragma page "pmspi_startsc()"
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

{
   short		error;
   short		starterr;
   short		ssn;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;

   error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_START, ZPWY_OBJ_SC);
   assert(error == 0);
   error = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   error = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(error == 0);
   starterr = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, err1,
			    err2);
   return (int)starterr;
}

#pragma page "pmspi_statussc()"
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

{
   short		spierror;
   short		statuserr;
   short		ssn;
   short		reqinfo;
   short		reqdetail;
   short		*sel_sc_map = zpwy_map_sel_sc;
   zpwy_ddl_sel_sc_def	sel_sc;
   short		*status_sc_map = zpwy_map_status_sc;
   zpwy_ddl_status_sc_def status_sc_def;

   spierror = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
		  ZSPI_VAL_CMDHDR, ZPWY_CMD_STATUS, ZPWY_OBJ_SC);
   assert(spierror == 0);
   spierror = SSNULL(sel_sc_map, (char *)&sel_sc);
   memcpy(sel_sc.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
   sel_sc.zname.zversion = ZPWY_VAL_SERVERCLASS2;
   spierror = SSPUT((short *)&spibuf, sel_sc_map, (char *)&sel_sc);
   assert(spierror == 0);
   statuserr = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn,
			     &reqinfo, &reqdetail);
   if (!statuserr) {
      spierror = SSGET((short *)&spibuf, status_sc_map,
		       (char *)&status_sc_def, 1);
      assert(spierror == 0);
      *freezestate = status_sc_def.zfreezestate;
      *error = status_sc_def.zerrorinfo2.zerror2;
      *info = status_sc_def.zerrorinfo2.zinfo2;
      *detail = status_sc_def.zerrorinfo2.zdetail2;
      *running = status_sc_def.zrunning;
   }
   return (int)statuserr;
}
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
	short *info,		/* OUT */
	short *detail)

{
	short 		error;
   	short		ssn;
	struct defineItem_t defineItem;
	short		*sel_scdefine_map = zpwy_map_sel_scdefine;
	zpwy_ddl_sel_scdefine_def	sel_scdefine;

	error = SSINIT((short *)&spibuf, ZPWY_VAL_BUFLEN, (short *)&zpwy_val_ssid,
			  ZSPI_VAL_CMDHDR, ZPWY_CMD_ADD, ZPWY_OBJ_SCDEFINE);
    assert(error == 0);
    error = SSNULL(sel_scdefine_map, (char *)&sel_scdefine);

	memcpy(sel_scdefine.zname.zserverclass.u_z_c.z_c, sc_name, strlen(sc_name));
	memcpy(sel_scdefine.zname.zdefine.u_z_c.z_c, defineName, strlen(defineName));
	error = SSPUT((short *)&spibuf, sel_scdefine_map, (char *)&sel_scdefine);
   	assert(error == 0);

	memcpy(defineItem.data,defineInfo->data,defineInfo->len);
	defineItem.len = defineInfo->len;

	error = SSPUTTKN((short *)&spibuf, (long)ZPWY_TKN_DEF_SCDEFINE,
			    (char *)&defineItem);
    assert(error == 0);

	error = pmspi_sendbuf((short *)&spibuf, SPIBUFLEN, &ssn, info,
			 detail);
	return (int)error;
}







