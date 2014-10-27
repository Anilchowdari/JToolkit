/*---------------------------------------------------------------------------
 * Copyright 2003
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
#ifndef TMRQST_H
#define TMRQST_H

/* Interface Dependencies */
#include "error.h"
#include "filesys.h"
#include <stdio.h>
#include <dlfcn.h>
#include <jni.h>

/* Exported type and data declarations */


/******************************************************************************
 *
 * System Message Selection Flags
 *
 * These values correspond to types of system messages that a server might want
 * to process.  They can be supplied to jpwy_open() and jpwy_setsmm() to
 * specify which system messages should be returned to the server by 
 * jpwy_comprecv().
 *
 * The values are grouped into words: the "*_SMM1" values belong in the first
 * word, and the "*_SMM2" values belong in the second word.  A selection flag
 * word can specify multiple selection flags by OR'ing the individual flag
 * values together.  jpwy_open() and jpwy_setsmm() manipulate whole
 * selection flag words.
 *
 ******************************************************************************/

#define LOCALCPUDOWN_SMM1           0x40000000    /* CpuDown_SysMsg */
#define LOCALCPUUP_SMM1             0x20000000    /* CpuUp_SysMsg */
#define SETTIME_SMM1                0x02000000    /* SetTime_SysMsg */
#define LOCALCPUON_SMM1             0x01000000    /* PowerOn_SysMsg */
#define NEWPROCESSDONE_SMM1         0x00800000    /* NewProcess_SysMsg */
#define LOSTSTATUS_SMM1             0x00400000    /* LostStatusMsgs_SysMsg */
#define ELAPSEDTIMEOUT_SMM1         0x00100000    /* TimeOut_SysMsg */
#define LOCKMEMORYDONE_SMM1         0x00080000    /* LockComplete_SysMsg */
#define LOCKMEMORYFAILED_SMM1       0x00040000    /* LockFailed_SysMsg */
#define PROCESSTIMEOUT_SMM1         0x00010000    /* ProcessTimeOut_SysMsg */
#define CONTROL_SMM1                0x00002000    /* Control_SysMsg */
#define SETMODE_SMM1                0x00001000    /* SetMode_SysMsg */
#define RESETSYNC_SMM1              0x00000800    /* ResetSync_SysMsg */
#define CONTROLBUF_SMM1             0x00000400    /* ControlBuf_SysMsg */
#define SETPARAM_SMM1               0x00000100    /* SetParam_SysMsg */
#define CANCEL_SMM1                 0x00000080    /* Cancel_SysMsg */
#define DEVICEINFODONE_SMM1         0x00000010    /* DeviceInfoNowait_SysMsg */

#define REMOTECPUDOWN_SMM2          0x80000000    /* RemoteCpuDown_SysMsg */
#define PROCESSSTOPPED_SMM2         0x40000000    /* StopD00_SysMsg */
#define PROCESSCREATEDONE_SMM2      0x20000000    /* ProcessCreate_SysMsg */
#define OPEN_SMM2                   0x10000000    /* OpenD00_SysMsg */
#define CLOSE_SMM2                  0x08000000    /* CloseD00_SysMsg */
#define BREAK_SMM2                  0x04000000    /* BreakD00_SysMsg */
#define DEVICEINQUIRY_SMM2          0x02000000    /* DeviceInfoD00_SysMsg */
#define NAMEINQUIRY_SMM2            0x01000000    /* NameInquiry_SysMsg */
#define FILEGETINFODONE_SMM2        0x00800000    /* FileGetInfoByName_SysMsg */
#define FILEFINDNEXTDONE_SMM2       0x00400000    /* FileFindNext_SysMsg */
#define NODEDOWN_SMM2               0x00200000    /* NodeDown_SysMsg */
#define NODEUP_SMM2                 0x00100000    /* NodeUp_SysMsg */
#define JOBCHILDSTARTED_SMM2        0x00080000    /* StartD00_SysMsg */
#define REMOTECPUUP_SMM2            0x00040000    /* RemoteCpuUp_SysMsg */
#define ENDDIALOG_SMM2              0x00000800    /* EndDialog_SysMsg */
#define ABORTDIALOG_SMM2            0x00000400    /* AbortDialog_SysMsg */

/******************************************************************************
 *
 * Selection Flag Update Modes
 *
 * These values can be supplied to jpwy_setsmm() to specify which system
 * message selection flags word is to be updated and how it is to be updated.
 * See jpwy_setsmm() for more information.
 *
 ******************************************************************************/

#define FETCH_SMM1           0      /* fetch selection flags word 1 */
#define ENABLE_SMM1          1      /* enable messages in flags word 1 */
#define DISABLE_SMM1         2      /* disable messages in flags word 1 */
#define RESET_SMM1           3      /* replace selection flags word 1 */
#define FETCH_SMM2           4      /* fetch selection flags word 2 */
#define ENABLE_SMM2          5      /* enable messages in flags word 2 */
#define DISABLE_SMM2         6      /* disable messages in flags word 2 */
#define RESET_SMM2           7      /* replace selection flags word 2 */


/******************************************************************************
 *
 * Process Request Queue ($RECEIVE) File Number
 *
 * This symbol denotes the Guardian physical file number of a server's request
 * queue.  It should be supplied to AWAITIOX() when waiting for a previously
 * initiated request queue receive operation to complete.  It is returned by
 * AWAITIOX(-1) or FILE_COMPLETE_() when a request queue receive operation has
 * completed.
 *
 ******************************************************************************/

#define JPWY_FNUM          0      /* server request queue file number */


/******************************************************************************
 *
 * Received Message System Information
 *
 * This structure contains system-supplied information associated with a
 * message received and passed into and returned by jpwy_comprecv().
 * The message can be either a request message from a connection to the
 * server, or a system message from a connection to the server or from the
 * operating system.
 *
 * The following fields might typically be used by a server:
 *
 * rcvinfo.MaxReplyLen       the maximum size of the sender's reply buffer.  Any
 *                    reply data longer than this limit is truncated, without
 *                    warning to the server or the sender.
 *
 * rcvinfo.OpenLabel  the index of the sending connection table entry, or -1
 *                    if the message is a status message from the operating
 *                    system.  This value might be useful if the server
 *                    maintains its own information about each connection.
 *
 * rcvinfo.MessageTag the index of the message's outstanding request table
 *                    entry.  This value must be supplied to jpwy_reply()
 *                    when replying to a message.  It ranges between 0 and
 *                    the receive depth specified to jpwy_open().
 *
 * dlgflags           the Pathsend dialog flags word returned by FILE_GET-
 *                    RECEIVEINFO_().  See the JPWY_DLG* #defines for the
 *                    specific value of each flag.
 *
 * See the FILE_GETRECEIVEINFO_() system procedure for more information.
 *
 ******************************************************************************/

typedef struct JPWY_MSGINFO_T JPWY_MSGINFO;

struct JPWY_MSGINFO_T
{
	ReceiveInfo    	rcvinfo;
	short           dlgflags;
};


/*changes begin for TSMP 2.4*/






#define GET_RECEIVEINFO_FIELD(RECEIVEINFOPTR, FIELD, TSMPVER)\
	((TSMPVER == 0)? (RECEIVEINFOPTR##->rcvInfo.##FIELD ): (RECEIVEINFOPTR##->rcvInfo2.##FIELD))

#define SET_RECEIVEINFO_FIELD(RECEIVEINFOPTR, FIELD, VALUE, TSMPVER)\
	((TSMPVER == 0)? (RECEIVEINFOPTR##->rcvInfo.##FIELD = VALUE ): \
			(RECEIVEINFOPTR##->rcvInfo2.##FIELD = VALUE))


/*changes end for TSMP 2.4*/

#define JPWY_DLGCHGTRAN    0x0002 /* trans id can vary during dialog. */
#define JPWY_DLGMSGTYPE    0x000C /* dialog message type field mask. */
#define JPWY_NODIALOG      0x0000 /* request is not part of a dialog. */
#define JPWY_DLGBEGIN      0x0004 /* first request in a dialog. */
#define JPWY_DLGSEND       0x0008 /* subsequent request in a dialog. */
#define JPWY_DLGABORT      0x000C /* "dialog abort" system message. */


/******************************************************************************
 *
 * Module Status Information
 *
 * This structure contains status information returned by jpwy_getinfo().
 *
 ******************************************************************************/

typedef struct JPWY_GETINFO_T JPWY_GETINFO;
struct JPWY_GETINFO_T
{
	short         maxopens;     /* maximum (initial or high-water) number of
                                       concurrent connections to the server. */
	short         maxrqsts;     /* maximum (receive depth) number of unreplied-
                                       to messages outstanding in the server. */
	short         numopens;     /* current number of connections to the server. */
	short         numrqsts;     /* current number of unreplied-to messages. */
};

/* See the C source file for descriptions of these external procedures */

extern int jpwy_open (
		short          initopens,
		short          recvdepth,
		short          syncdepth,
		int            maxrplylen,
		short          options,
		unsigned int  smm1,
		unsigned int  smm2);

extern int jpwy_close (int pathSendMode);

extern unsigned long jpwy_setsmm (
		short          mode,
		unsigned int   smm);

extern int jpwy_reply (
		char           *msgbuf,
		int            msglen,
		short          error,
		short          msgtag,
		int           *countReplied,
		int 		  pathSendMode);

extern int jpwy_initrecv (
		char           *msgbuf,
		int            buflen,
		int           *countRead,
		JPWY_MSGINFO   *msginfo,
		int pathSendMode);

extern int jpwy_comprecv (
		char           *msgbuf,
		int            *msglen,
		JPWY_MSGINFO *msginfo,
		int pathSendMode);

extern int jpwy_cancelled (
		short          msgtag);

extern short jpwy_activatetransid (
		short          msgtag);

extern void jpwy_getinfo (
		JPWY_GETINFO *info);

extern void jpwy_msginfo (
		short          msgtag,
		JPWY_MSGINFO *msginfo,
		int pathSendMode);

extern void jpwy_openinfo (
		short          opentag,
		short          *phandle,
		short          *file);

extern void jpwy_dropopen (
		short          opentag);

extern int jpwy_print (FILE *stream);

extern void initialise_functionptr();
#endif
