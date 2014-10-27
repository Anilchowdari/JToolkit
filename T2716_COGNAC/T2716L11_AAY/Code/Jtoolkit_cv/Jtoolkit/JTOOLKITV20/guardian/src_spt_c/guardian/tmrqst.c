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
/******************************************************************************
 *
 * Server Process Request Queue ($RECEIVE) Manager
 *
 * This module manages the request queue ($RECEIVE file) of a server process.
 * It maintains a table of the current connections to the server, and updates
 * this table automatically in response to process and system status messages.
 * It returns selected system messages to the server for processing, and
 * processes system messages that have not been selected internally.  It also
 * indicates when the server has lost all of its connections so that it can
 * shut down (a Tandem NonStop system convention).
 *
 * The module supports multi-threaded servers:
 *
 *  *   A multi-threaded server is driven by operation completions.  It init-
 *      iates a nowait receive operation using jpwy_initrecv(), and waits
 *      for any operation to complete using AWAITIOX(-1) or FILE_COMPLETE_().
 *      When the receive operation completes, the server calls
 *      FILE_GETRECEIVEINFO_ and then notifies the module
 *      that a message has been received using jpwy_comprecv(), processes
 *      the message, and (eventually) replies to it using jpwy_reply().  A
 *      multi-threaded server may have several, unreplied-to requests outstand-
 *      ing at a time.  When it replies to a particular request, it supplies a
 *      message tag assigned to the request when it was received.
 
 *
 * In a multi-threaded server, a receive operation may complete with a system
 * message that has not been selected for processing by the server.  jpwy_-
 * comprecv() processes the message internally, and indicates that the server
 * should not process the message.  In this case, the server initiates another
 * receive operation using jpwy_initrecv().
 *
 * The module synthesizes CANCEL, dialog abort, or CLOSE system messages if it
 * receives CLOSE, cpu down, node down or lost status system messages, that indicate
 * terminated connections with the server, and if the server has selected these
 * system messages for processing.  If more than one selected, synthesizable
 * system message applies to a connection, the module synthesizes only the most
 * subsuming message (CLOSE subsumes dialog abort, which subsumes CANCEL).
 *
 ******************************************************************************/

/* Implementation Dependencies */
#include <assert.h>
#include <tal.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <cextdecs.h>
#include <spthread.h>
#include "error.h"
#include "tandem.h"
#include "tmrqst.h"
#include "filesys.h"
#include "com_tandem_ext_util_PathsendModeType.h"
#include "functionpointers.h"

/* Private type and data declarations */

/******************************************************************************
 *
 * An OLDREPLY describes a previously sent reply being saved for a connection.
 * Replies are saved in case the connector's backup process sends a duplicate
 * request.  Reply descriptors are allocated when the connection table entry
 * is created, and are freed when the entry is destroyed.
 *
 * The reply message data is stored following the OLDREPLY structure (i.e.
 * the OLDREPLY structure is the header of a reply descriptor).  Each descrip-
 * tor is large enough to hold the maximum reply size specified by the server.
 *
 * A connection's OLDREPLY descriptors form a circular list.  The connection
 * points to the oldest descriptor, which is reused when a new reply is saved.
 *
 ******************************************************************************/

typedef struct oldreply_t OLDREPLY;
struct oldreply_t {
	OLDREPLY *next; /* pointer to the next saved reply. */
	/* syncid is unsigned so can do '<' comparison */
	unsigned long syncid; /* connection's request sync id if the reply
	 is saved, or 0 if the descriptor is free. */
	int msglen; /* length of the reply message data. */
	short error; /* file error code to send with the reply. */
};

/******************************************************************************
 *
 * An OPENER describes a process (or process pair) that can send requests to the
 * server.  An OPENFILE (aka an open aka a sender) describes a logical connection
 * between the server and an opener.  An OPENER can have many OPENFILEs,
 * an OPENFILE has only one OPENER.  The module updates a table of process
 * and connection descriptors in response to system status messages that
 * describe process terminations and requests to open the server.
 *
 * The primary, or the backup, or both, can have a particular connection to the
 * server.  The primary's connection will be missing if the primary has closed
 * its connection, but the backup has not.
 *
 ******************************************************************************/

typedef struct opener_t OPENER;
struct opener_t {
	short priproc [10]; /* opener's primary process handle, or nil. */
	short bakproc [10]; /* opener's backup process handle, or nil. */
	short pricpu, bakcpu; /* opener's cpu numbers, or -1 if no process. */
	long node; /* node number of system running the opener,
	 or -1 if the opener is on the local node. */
	short frstopnx; /* open table INDEX of a list of opens by this
	 opener, or nil (-1) if entry is unused. */
	short nextopnrx; /* INDEX to next opener in goner list (see myinfo). */
};

typedef struct openfile_t OPENFILE;
struct openfile_t {
	short prifile; /* server's file number in opener's primary, or
	 -1 if primary does not have server open. */
	short bakfile; /* server's file number in opener's backup, or
	 -1 if backup does not have server open. */
	OLDREPLY *oldrply; /* oldest reply saved for the open; advanced
	 through the reply list as replies are sent. */
	/* syncid is unsigned so can do '<' comparison */
	short openerx; /* opener table INDEX of the opener process(es),
	 or nil (-1) if the entry is unused. */
	short nextopnx; /* open table INDEX of next open by the opener. */
   	unsigned short openflags; /* open parameters primary and backup openers
	 must agree on. */
	short flags; /* status flags (see *_OPEN #defines below). */
	short firstrqstx; /* Index to the first request*/
	short lastrqstx;  /* Index to the last request*/
};

#define RCVTYPE_OPEN         0x000F /* current request type (a *_ReceiveType value). */
#define DLGFLAGS_OPEN        0x00F0 /* current dialog operation type and flags. */
#define OPENING_OPEN         0x0800 /* primary or backup process added to open entry
	and open not yet replied to. */
#define DLGABORT_OPEN        0x1000 /* current dialog on open entry was aborted. */
#define CLOSING_OPEN         0x2000 /* logical close received for open entry and
	close not yet replied to */
// Java doesn't currently use the "dropped open" feature
#define DROPPED_OPEN         0x4000 /* no primary or backup opener for open entry,
	or open entry dropped by jpwy_dropopen(). */

typedef struct rqstinfo_t RQSTINFO;
struct rqstinfo_t {
	short gmsgtag; /* Guardian message tag of the 
	open's current request -1 if the open has no request, or -2 if the request is 
	synthesized. */
	unsigned long syncid;  /*connection's request 
	sync id if the reply is saved, or 0 if the descriptor is free.*/
	int maxrplylen;	/* (in)  maximum size of each 
	reply message.*/  
 	short rqstflags; /*  status flags, this is not 
	completely removed from OPENFILE structure as few flags remain specific to a open.*/
	short openx; /*  stores the open index of the OPEN table for the particular request.
	if >= 0, open index of the request sender;
	if < -1, negative of a system message tag - 2
	also is -tag-2 if it's a cancel sysmsg for a request.
	if = -1, the entry is available.*/
	short nextrqstx; /* index to the next request of a given connection. */
};

typedef struct myinfo_t MYINFO;
struct myinfo_t {
	short recvdepth; /* maximum concurrent, unreplied-to requests. */
	short syncdepth; /* maximum replies saved for each connection. */
	int maxrplylen; /* maximum size of each saved reply buffer.
	 INT_MAX if replies are not being saved. */
	unsigned long smm [2]; /* server's system message selection masks. */
	short maxopeners; /* number of entries allocated in openers table. */
	short maxopens; /* number of entries allocated in opens table. */
	OPENER *openers; /* requester descriptors (maxopeners entries). */
	OPENFILE *opens; /* connection descriptors (maxopens entries). */
	
	short goners; /* opener table index of a list of openers with
	 recently deceased primary or backup processes,
	 or -1 if none. */
	short numopens; /* current number of active opens, or -1 if the
	 server has not yet been opened. */
	RQSTINFO *rqstinfo; /* Pointer to the new requestinfo structure*/
};

static MYINFO *myinfo = NULL;



#define RQSTFREE  -1  /* Indicates Request table entry which is free*/
#define ENDOFREQ  -1  /* Indicates the end of request in the list of outstanding requests*/
#define NORQST    -1  /* Indicates no outstanding requests associated with the OPEN

/* This instance of RQSTINFO is used for resetting the request entries 
 * when the request is successfully replied */						 
static RQSTINFO RQSTINIT={-1,0,-1,0,RQSTFREE,ENDOFREQ};


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_open (short initopens, short recvdepth, short syncdepth,
 *                   int maxrplylen, unsigned long smm1, unsigned long smm2)
 *
 * Description:
 *      Open the calling server's $RECEIVE file so that the server can receive
 *      request messages and selected system messages.  Allocate data structures
 *      to monitor connections to the server.
 *
 *      The recvdepth parameter specifies how many unreplied-to requests the
 *      server can have outstanding at one time.  Recvdepth determines whether
 *      the module regards the server as single-threaded or multi-threaded,
 *      and influences how the server replies to messages returned by jpwy_-
 *      receive() and jpwy_comprecv().  If recvdepth = 0, the server does
 *      not reply to any messages: The module replies to all messages automati-
 *      cally when they are received.  If recvdepth >= 1, the server must
 *      reply to all messages returned by the module.
 *
 *      The syncdepth and maxrplylen parameters determine whether the module
 *      saves previously issued replies for NonStop process pair requesters.
 *      The syncdepth parameter specifies the maximum number of replies that the
 *      module saves for each connection.  The maxrplylen parameter specifies
 *      the maximum size of each reply.  The saved reply buffers are allocated
 *      when a requester opens the server.  Only the most recent replies to
 *      each connection are saved.  If syncdepth = 0, the module does not save
 *      previously issued replies and the maxrplylen parameter is ignored.  The
 *      syncdepth and maxreplylen parameters are ignored if recvdepth = 0.
 *
 * Results:
 *      The function returns FEOK if it initialized the module successfully,
 *      or a Guardian error code if a file system error occurred while opening
 *      $RECEIVE, or an OSS error code if an error occurred while allocating
 *      the open connections data structures.
 *
 ******************************************************************************/

int jpwy_open(short initopens, /* (in)  initial number of concurrent connec-
 tions that the server will support. */
		short recvdepth, /* (in)  maximum number of unreplied-to requests
 that the server can hold at a time. */
		short syncdepth, /* (in)  maximum number of previous replies to
 be saved for a connection. */
		int maxrplylen, /* (in)  maximum size of each reply message. */
		short inOptions, /* (in)  open options parameter, bits 12 and 13
 let one turn off transactions and turn on
 locale support, respectively. All other bits ignored. */
		unsigned long smm1, unsigned long smm2) /* (in)  system messages that the server wants
 to process.  See the *_SMM1 & *_SMM2
 #defines. */
{
	short initopeners;
	OPENER *openers = NULL;
	OPENFILE *opens = NULL;
	RQSTINFO *rqstinfo = NULL;
	short rcvfile = -1;
	int opnx;
	short fserr, fierr;
	short options = (short)(inOptions & 0xC); /* the only options user can
	 turn on is bit 12 and 13 */

	assert(initopens >= 0 && recvdepth >= 0 && syncdepth >= 0 && maxrplylen
			>= 0);

	if (myinfo != NULL)
		return FEINUSE;

	initopeners = (short)_min(initopens, 256);
  /*Allocating an array of RQSTINFO structure with size equal to Receivedepth value*/
	if ((myinfo = (MYINFO *) malloc(sizeof *myinfo)) == NULL || (openers
			= (OPENER *) malloc(initopeners * sizeof *openers)) == NULL
			&& initopeners > 0 || (opens = (OPENFILE *) malloc(initopens
					* sizeof *opens)) == NULL && initopens > 0 || (rqstinfo
					  = (RQSTINFO *)malloc(recvdepth * sizeof *rqstinfo)) == NULL
							&& recvdepth > 0) {
		fserr = (short) errno;
		goto errexit;
	}

	myinfo->recvdepth = recvdepth;
	myinfo->syncdepth = (short)((recvdepth > 0 ? syncdepth : 0));
	myinfo->maxrplylen = (myinfo->syncdepth > 0 ? maxrplylen : INT_MAX);
	myinfo->maxopeners = initopeners;
	myinfo->maxopens = initopens;
	myinfo->openers = openers;
	myinfo->opens = opens;
	myinfo->rqstinfo = rqstinfo;
	myinfo->smm [0] = smm1;
	myinfo->smm [1] = smm2;
	myinfo->goners = -1;
	myinfo->numopens = -1;

	if (myinfo->maxopeners > 0) {
		PROCESSHANDLE_NULLIT_(openers [0].priproc);
		PROCESSHANDLE_NULLIT_(openers [0].bakproc);
		openers [0].pricpu = openers [0].bakcpu = -1;
		openers [0].node = -1;
		openers [0].frstopnx = -1;
		for (opnx = 1; opnx < myinfo->maxopeners; opnx++)
			openers [opnx] = openers [0];
	}

	if (myinfo->maxopens > 0) {
		opens [0].prifile = opens [0].bakfile = -1;
		opens [0].oldrply = NULL;
		opens [0].openerx = -1;
		opens [0].nextopnx = -1;
		opens [0].openflags = 0;
		opens [0].flags = 0;
		for (opnx = 1; opnx < myinfo->maxopens; opnx++)
			opens [opnx] = opens [0];
	}
	/*Initializing the Rqstinfo structure entries */
	if (recvdepth > 0) {
		short rqstx;
		for ( rqstx = 0; rqstx < recvdepth; rqstx++)
			myinfo->rqstinfo[rqstx] = RQSTINIT;

	}
		


	fserr = FILE_OPEN_("$RECEIVE", 8, &rcvfile, /*access*/, /*exclusion*/,
			1/*nowait depth*/, (short) _max (recvdepth, 1),
			options);
	if (rcvfile != JPWY_FNUM)
		goto errexit;

	if (smm1 & (CANCEL_SMM1 | SETPARAM_SMM1 | SETMODE_SMM1)) {
		if (_status_lt(SETMODE(JPWY_FNUM, ReceiveOptions_SetMode,
				(short) ((smm1 & CANCEL_SMM1 ? 4 : 0)
						| (smm1 & SETPARAM_SMM1 ? 2 : 0)
						| (smm1 & SETMODE_SMM1 ? 1 : 0)), OMITFAT_16, OMITREF))) {
			fierr = FILE_GETINFO_(JPWY_FNUM, &fserr, OMITREF, OMITSHORT,
					OMITREF, OMITREF, OMITREF);
			if (fierr != FEOK)
				fserr = fierr;
			goto errexit;
		}
	}

	MONITORCPUS((short) (-1));

	MONITORNET(1);

	if (smm1 & (LOCALCPUON_SMM1 | SETTIME_SMM1))
		MONITORNEW(1);

	return (FEOK);
	
	errexit: free(rqstinfo);
	free(opens);
	free(openers);
	free(myinfo), myinfo = NULL;

	return ((int) fserr);
} // jpwy_open


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_close ()
 *
 * Description:
 *      Close the calling server's $RECEIVE file.  Free memory allocated by
 *      the module.
 *
 * Results:
 *      The function returns FEOK if it closed $RECEIVE successfully, or a
 *      Guardian error code if a file system error occurred.
 *
 ******************************************************************************/

int jpwy_close(int pathSendMode) {
	short opnx;
	OPENFILE *open;
	short fserr;
	short rqstx;

	if (myinfo == NULL)
		return (FENOTOPEN);

   /*loop through all the request entries and reply to all pending request*/
	for (rqstx = 0; rqstx < myinfo->recvdepth; rqstx++) {
		if (myinfo->rqstinfo[rqstx].openx == RQSTFREE)
			continue;
		(void) jpwy_reply(NULL, 0, FEPATHDOWN, rqstx, NULL, pathSendMode);
	}
	/* free the memory allocated for the rqstinfo structure*/
	free(myinfo->rqstinfo);

	for (opnx = 0; opnx < myinfo->maxopens; opnx++) {
		OLDREPLY *oldrply, *nxtrply;

		open = &myinfo->opens [opnx];

		if (open->oldrply == NULL)
			continue;
		
        //Changes start for Soln. 10-121012-5050
        /*		
		oldrply = open->oldrply;
		do {
			nxtrply = oldrply->next;
			free(oldrply);
			oldrply = nxtrply;
		} while (oldrply != open->oldrply);
		*/
		//Changes end for Soln. 10-121012-5050
        		
	}
	free(myinfo->opens);

	free(myinfo->openers);

	MONITORCPUS(0);
	MONITORNET(0);
	MONITORNEW(0);

	free(myinfo), myinfo = NULL;

	fserr = FILE_CLOSE_(JPWY_FNUM, OMITSHORT);

	return ((int) fserr);
} // jpwy_close


/******************************************************************************
 *
 * Syntax:
 *      unsigned long jpwy_setsmm (short mode, unsigned long smm)
 *
 * Description:
 *      Enable or disable server processing of selected system messages after
 *      the server's $RECEIVE file has been opened via jpwy_open().
 *
 *      The smm parameter specifies the system message selections to be com-
 *      bined with one of the current system message selection words.  The
 *      mode parameter specifies which selection word is to be updated, and
 *      how the selection words are to be combined, as follows:
 *
 *      FETCH_SMM<n>  message selection word <n> is returned without being
 *                    updated.  The supplied selection word is ignored.
 *
 *      ENABLE_SMM<n> the supplied selection word is added to message
 *                    selection word <n> by OR'ing the two words.
 *
 *      DISABLE_SMM<n>       the supplied selections are removed from message
 *                    selection word <n> by AND'ing the selection word
 *                    with the complement of the supplied selection word.
 *
 *      RESET_SMM<n>  all of message selection word <n> is replaced by the
 *                    supplied selection word.
 *
 * Results:
 *      The function returns the previous value of the updated system message
 *      selection word, before it was combined with the supplied selections.
 *
 ******************************************************************************/

unsigned long jpwy_setsmm(short mode, /* (in)  indicates how the current selections
 are to be updated.  See above. */
		unsigned long smm) /* (in)  selected system messages to be enabled
 or disabled.  See the *_SMM1 & *_SMM2
 #defines. */
{
	unsigned long oldsmm, newsmm;

	assert(myinfo != NULL);

	switch (mode) {
	case FETCH_SMM1:
	case FETCH_SMM2:
		return (myinfo->smm [mode / 4]);

	case ENABLE_SMM1:
	case ENABLE_SMM2:
		oldsmm = myinfo->smm [mode / 4];
		newsmm = (myinfo->smm [mode / 4] |= smm);
		break;

	case DISABLE_SMM1:
	case DISABLE_SMM2:
		oldsmm = myinfo->smm [mode / 4];
		newsmm = (myinfo->smm [mode / 4] &= ~smm);
		break;

	case RESET_SMM1:
	case RESET_SMM2:
		oldsmm = myinfo->smm [mode / 4];
		newsmm = (myinfo->smm [mode / 4] = smm);
		break;

	default:
		assert(mode >= FETCH_SMM1 && mode <= RESET_SMM2);
	}

	switch (mode / 4) {
	case 0:
		if (~oldsmm & newsmm & (LOCALCPUON_SMM1 | SETTIME_SMM1))
			MONITORNEW(1);

		if (~oldsmm & newsmm & (CANCEL_SMM1 | SETPARAM_SMM1 | SETMODE_SMM1))
			(void) SETMODE(JPWY_FNUM, ReceiveOptions_SetMode, (short) ((newsmm
					& CANCEL_SMM1 ? 4 : 0) | (newsmm & SETPARAM_SMM1 ? 2 : 0)
					| (newsmm & SETMODE_SMM1 ? 1 : 0)), OMITFAT_16, OMITREF);
		break;
	}

	return (oldsmm);
} //jpwy_setsmm


/******************************************************************************
 *
 * Syntax:
 *      OPENFILE *open_add (SysMsg_Type *sysmsg, ReceiveInfo_Type *rcvinfo)
 *
 * Description:
 *      Allocate and initialize an open file table entry to describe a new
 *      open.  If the opener is a backup process, only the backup phandle and
 *      file number of an existing entry are set.  Otherwise, an entire entry
 *      is allocated and initialized.
 *
 * Results:
 *      The function returns a pointer to the allocated entry if it was
 *      successful, or NULL if it failed.
 *
 ******************************************************************************/

static OPENFILE *open_add(SysMsg_Type *sysmsg, /* (in)  the D00 OPEN message from the opener. */
		ReceiveInfo *rcvInfo, /* (i/o) additional information.  On output,
 OpenLabel is set to the index of the
 opens table entry. */
		int pathSendMode) {
	OPENER *opener;
	OPENFILE *open;
	short opnrx, opnx, srchx, tmpOpnrx, tmpOpnx;
	long node;
	short cpu;
	unsigned short openflags;
	void *tmpptr;
	int growth; // amount to grow tables by if tables are full

	assert(sysmsg->MsgType == OpenD00_SysMsg);

	(void) PROCESSHANDLE_DECOMPOSE_(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
			pathSendMode), &cpu, OMITREF, &node, OMITREF, OMITSHORT, OMITREF,
			OMITREF, OMITSHORT, OMITREF, OMITREF);
	/*
	 * Use a constant (-1) to designate our own node, to avoid needing
	 * to look up our node number when we get local system messages.
	 */
	if (!sysmsg->variant.OpenD00.RemoteProcess)
		node = -1;

	// Put into openflags those elements of the open that the primary and
	// backup open must agree upon.  Sync depth takes of 6 bits, access 4,
	// exclusion 2 and nowait depth 4.  Access really only needs 2 so could
	// give 2 more bits to sync depth.
	openflags = (sysmsg->variant.OpenD00.SyncDepth & 077) << 10
	| (sysmsg->variant.OpenD00.Access & 017) << 6
	| (sysmsg->variant.OpenD00.Exclusion & 003) << 4
	| (sysmsg->variant.OpenD00.NowaitDepth & 017);

	if (sysmsg->variant.OpenD00.BackupOpen) {
		/*
		 * This is a backup open.  Look for the corresponding primary
		 * 'opens' slot.  Return an error if there is no corresponding
		 * primary open, or if the primary's open is not yet complete,
		 * or if the primary and backup opens are not compatible.
		 */
		for (opnx = 0, open = myinfo->opens;; opnx++, open++) {
			if (opnx >= myinfo->maxopens)
				return (NULL); // we reached end of list without finding open

			/*
			 * Check the file number first because, if the open
			 * entry is unused, its prifile will not match and
			 * we will avoid dereferencing its openerx, which is
			 * invalid in unused entries.
			 */
			if (sysmsg->variant.OpenD00.PrimaryFileNum == open->prifile
					&& memcmp(sysmsg->variant.OpenD00.PrimaryPHandle,
							myinfo->openers [open->openerx].priproc,
							sizeof myinfo->openers->priproc) == 0)
				break; // found the primary open
		}

		// the open has an index to its opener so now we know the opener
		opener = &myinfo->openers [open->openerx];

		// make sure backup open agrees with primary open and that we've
		// already replied to the primary open.
		if (node != opener->node || openflags != open->openflags
				|| (open->flags & OPENING_OPEN) && open->bakfile == -1)
			return (NULL);

		memcpy(opener->bakproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode), sizeof opener->bakproc);
		opener->bakcpu = cpu;
		open->bakfile
		= GET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, pathSendMode);
		open->flags |= OPENING_OPEN;
	} else {
		/*
		 * This is a primary open.  Look for an empty 'opens' slot,
		 * and a matching or empty 'openers' slot.  Reallocate either
		 * table to get more entries if the table is full.
		 */

		opnrx = -1;
		for (srchx = 0, opener = myinfo->openers;; srchx++, opener++) {
			if (srchx >= myinfo->maxopeners) {
				// got to end of list without finding matching opener
				if (opnrx != -1) {
					// but we did find an unused opener slot we can
					// use for this new opener.
					/* recycle an unused opener entry. */
					opener = &myinfo->openers [opnrx];
					break;
				}

				/* no unused opener entries; grow the table by at least
				 20% or 20 entries minimum. */
				growth = _max((int)(myinfo->maxopeners * 0.2), 20);
				tmpptr = realloc(myinfo->openers, (myinfo->maxopeners + growth)
						* sizeof(OPENER));
				if (tmpptr == NULL)
					return (NULL);

				myinfo->openers = (OPENER *) tmpptr;
				opnrx = (short)(myinfo->maxopeners + 1);
				myinfo->maxopeners += growth;
				opener = &myinfo->openers [opnrx];

				// Initialize all the new growth
				PROCESSHANDLE_NULLIT_(opener->priproc);
				PROCESSHANDLE_NULLIT_(opener->bakproc);
				opener->pricpu = opener->bakcpu = -1;
				opener->node = -1;
				opener->frstopnx = -1;
				for (tmpOpnrx = (short)(opnrx+1); tmpOpnrx < myinfo->maxopeners; tmpOpnrx++)
					myinfo->openers[tmpOpnrx] = myinfo->openers[opnrx];

				break; // we're done searching opener list
			}

			// we're not at end of opener list yet so look for matching opener
			if (memcmp(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
					opener->priproc, sizeof opener->priproc) == 0) {
				opnrx = srchx;
				break; // match found, we're done searching
			}

			if (opener->frstopnx == -1 && opnrx == -1) {
				// we found first empty opener slot, still need to keep searching
				// for matching opener, we'll use this empty slot if we get to
				// end of list without find a match
				opnrx = srchx;
			}
		} // end 'opener' for loop

		// go thru list of opens looking for unused one
		for (opnx = 0, open = myinfo->opens;; opnx++, open++) {
			if (opnx >= myinfo->maxopens) {
				/* no unused open entries; grow the table by at least
				 20% or 20 entries minimum. */
				growth = _max((int)(myinfo->maxopens * 0.2), 20);
				tmpptr = realloc(myinfo->opens, (myinfo->maxopens + growth)
						* sizeof(OPENFILE));
				if (tmpptr == NULL)
					return (NULL);

				myinfo->opens = (OPENFILE *) tmpptr;
				opnx = (short)(myinfo->maxopens + 1);
				myinfo->maxopens += growth;
				open = &myinfo->opens [opnx];

				// Initialize all the new growth
				open->prifile = open->bakfile = -1;
				open->oldrply = NULL;
				open->openerx = -1;
				open->nextopnx = -1;
				open->openflags = 0;
				open->flags = 0;
				open->firstrqstx = NORQST;
	 			open->lastrqstx = NORQST;
				
				for (tmpOpnx =(short)(opnx+1); tmpOpnx < myinfo->maxopens; tmpOpnx++)
					myinfo->opens[tmpOpnx] = myinfo->opens[opnx];

				break; // we're done searching open list
			}
			if (open->openerx == -1)
				break; // found an unused open entry
		} // end 'open' for loop

		if (myinfo->syncdepth == 0 || sysmsg->variant.OpenD00.SyncDepth == 0) {
			// requester doesn't require us to save replies
			open->oldrply = NULL;
		} else {
			/*
			 * allocate space for maximum saved replies, list of replies
			 * is a circular list
			 */
			OLDREPLY *oldrply, *newrply;
			short nrply;

			oldrply = malloc(sizeof(OLDREPLY) + myinfo->maxrplylen);
			if (oldrply == NULL)
				return (NULL);

			oldrply->next = oldrply;
			oldrply->syncid = 0;
			oldrply->msglen = 0;
			oldrply->error = 0;

			open->oldrply = oldrply;

			for (nrply = 1; nrply < sysmsg->variant.OpenD00.SyncDepth; nrply++) {
				newrply = malloc(sizeof(OLDREPLY) + myinfo->maxrplylen);
				if (newrply == NULL) {
					do {
						newrply = oldrply->next;
						free(oldrply);
						oldrply = newrply;
					} while (oldrply != open->oldrply);
					open->oldrply = NULL;
					return (NULL);
				}

				newrply->next = oldrply->next;
				newrply->syncid = 0;
				newrply->msglen = 0;
				newrply->error = 0;

				oldrply->next = newrply;
			}
		}

		/*
		 * Nothing more can go wrong with the open and opener entries,
		 * so fill them in.  Initialize the opener entry if this is its
		 * first open.
		 */
		if (opener->frstopnx == -1) {
			memcpy(opener->priproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
					pathSendMode), sizeof opener->priproc);
			opener->pricpu = cpu;
			opener->node = node;
		}

		open->prifile
		= GET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, pathSendMode);
		open->bakfile = -1;
		open->openerx = opnrx;
		open->openflags = openflags;
		open->flags = OPENING_OPEN;
		open->firstrqstx = NORQST;
	 	open->lastrqstx = NORQST;


		// open is added to beginning of the list of opener's opens
		open->nextopnx = opener->frstopnx;
		opener->frstopnx = opnx;
	}

	// This is how an open is found when future requests come in.
	// The opnx value is the index into the list of opens in myinfo.
	SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, opnx, pathSendMode);
	return (open);
} // open_add


/******************************************************************************
 *
 * Syntax:
 *      OPENFILE *open_find (ReceiveInfo_Type *rcvinfo)
 *
 * Description:
 *      Find a current open of the server.  The open is identified by the
 *      message sender information return by FILE_GETRECEIVEINFO_().
 *
 *      If the sender is a backup process, swap it with the primary process
 *      fields in the opens table.  Thus, the caller can always assume that
 *      the sender information identifies the primary process fields in the
 *      returned entry.
 *
 * Results:
 *      The function returns the address of the open's table entry if the open
 *      was found, or NULL if the open does not exist in the server's opens
 *      table.
 *
 ******************************************************************************/

static OPENFILE *open_find(ReceiveInfo *rcvInfo, /* (i/o)  designates the open.  On output,
 OpenLabel is set to the open's table
 entry index if not already set. */
		int pathSendMode) {
	OPENFILE *open;
	OPENER *opener;
	short opnx, srchx;
	short swap;
	assert(myinfo != NULL);

	if (GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode) != -1) {
		/*
		 * The sender is maintaining his open table index for us.
		 * We only need to verify that he is who he claims to be.
		 */
		// The open label is the index into the list of opens in myinfo
		// so we can quickly find an open.
		if ((unsigned) GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode)
				>= myinfo->maxopens)
			return (NULL);
		open = &myinfo->opens [GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode)];

		if (open->openerx == -1)
			return (NULL);

		assert((unsigned) open->openerx < myinfo->maxopeners);
		// The open has an index to its opener.
		opener = &myinfo->openers [open->openerx];

		if (memcmp(opener->priproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
						pathSendMode)) == 0 && open->prifile == GET_RECEIVEINFO_FIELD(
								rcvInfo, FileNumber, pathSendMode)) {
			return (open);
		}

		if (memcmp(opener->bakproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
						pathSendMode)) == 0 && open->bakfile == GET_RECEIVEINFO_FIELD(
								rcvInfo, FileNumber, pathSendMode)) {
			/*
			 * The sender is a backup process.  Swap the backup
			 * and primary fields so that the caller can always
			 * assume that the primary process is the sender.
			 */
			memcpy(opener->bakproc, opener->priproc, sizeof opener->priproc);
			memcpy(opener->priproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
					pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo,
							PHandle, pathSendMode));
			// swap primary and backup for opener
			swap = opener->bakcpu;
			opener->bakcpu = opener->pricpu;
			opener->pricpu = swap;

			for (opnx = opener->frstopnx; opnx != -1; opnx
			= myinfo->opens [opnx].nextopnx) {
				assert((unsigned) opnx < myinfo->maxopens);
				// Swap primary and backup for all opens associated
				// with this opener.
				swap = myinfo->opens [opnx].bakfile;
				myinfo->opens [opnx].bakfile = myinfo->opens [opnx].prifile;
				myinfo->opens [opnx].prifile = swap;
			}

			open->flags &= ~CLOSING_OPEN;

			return (open);
		}

		return (NULL); // phandle didn't match either primary or backup
	}

	/*
	 * The sender is not maintaining an open table index for us.
	 * Search the open and opener tables for his entry.
	 */
	for (srchx = 0, open = myinfo->opens;; srchx++, open++) {
		if (srchx >= myinfo->maxopens)
			return (NULL);

		if (open->openerx == -1)
			continue;

		assert((unsigned) open->openerx < myinfo->maxopeners);
		opener = &myinfo->openers [open->openerx];

		if (memcmp(opener->priproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
						pathSendMode)) == 0 && open->prifile == GET_RECEIVEINFO_FIELD(
								rcvInfo, FileNumber, pathSendMode)) {
			break;
		}

		if (memcmp(opener->bakproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
						pathSendMode)) == 0 && open->bakfile == GET_RECEIVEINFO_FIELD(
								rcvInfo, FileNumber, pathSendMode)) {
			/*
			 * The sender is a backup process.  Swap the backup
			 * and primary fields so that the caller can always
			 * assume that the primary process is the sender.
			 */
			memcpy(opener->bakproc, opener->priproc, sizeof opener->priproc);
			memcpy(opener->priproc, GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
					pathSendMode), sizeof GET_RECEIVEINFO_FIELD(rcvInfo,
							PHandle, pathSendMode));
			swap = opener->bakcpu;
			opener->bakcpu = opener->pricpu;
			opener->pricpu = swap;

			for (opnx = opener->frstopnx; opnx != -1; opnx
			= myinfo->opens [opnx].nextopnx) {
				assert((unsigned) opnx < myinfo->maxopens);
				swap = myinfo->opens [opnx].bakfile;
				myinfo->opens [opnx].bakfile = myinfo->opens [opnx].prifile;
				myinfo->opens [opnx].prifile = swap;
			}

			open->flags &= ~CLOSING_OPEN;

			break;
		}
	}

	SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, srchx, pathSendMode);
	return (open);
} // open_find


/******************************************************************************
 *
 * Syntax:
 *      open_free (OPENFILE *open)
 *
 * Description:
 *      Remove an open entry from the server's opens table.  Free any dynamic
 *      memory allocated to the open, and reset its table entry.
 *
 * Side Effects:
 *      The function removes the open's parent opener table entry, if the open
 *      was the last open of that opener.  The caller should not reference any
 *      of the open or opener entry fields (e.g. nextopnx or nextopnrx) after
 *      calling this function.  Outstanding request info is modified
 *      so it doesn't index its (former) open.
 *
 ******************************************************************************/

static void open_free(OPENFILE *open) /* (i/o) the open table entry to clear. */
{
	short opnx, opnrx, *prevx;
	short rqstx=0,lastrqstx=0;

	opnx = (short) (open - myinfo->opens); // index of open in myinfo list
	opnrx = open->openerx; // index of opener in myinfo list
	lastrqstx= open->lastrqstx; //Index to the  last request into the rqstinfo table for an open
	
    /*If there is any outstanding request for this open
	  then change the openx to ( -gmsgtag - 2) so that
	  it wont be refering to its former open anymore */
    if(open->firstrqstx != NORQST)
		for(rqstx=open->firstrqstx; rqstx!=ENDOFREQ; rqstx=myinfo->rqstinfo[rqstx].nextrqstx) 
			myinfo->rqstinfo[rqstx].openx = (short)(-myinfo->rqstinfo[rqstx].gmsgtag - 2);

	/*indicates that no more outstanding requests for this OPEN*/
	open->lastrqstx=open->firstrqstx=NORQST;
	

	/*
	 * Decrement the current number of opens if this open passed its
	 * provisional primary open phase successfully.
	 */
	if (!((open->flags & OPENING_OPEN) && open->bakfile == -1))
		if (myinfo->numopens > 0)
			myinfo->numopens--;

	/*
	 * Free any saved reply message buffers allocated to the open.
	 */
	if (open->oldrply != NULL) {
		OLDREPLY *oldrply, *nxtrply;

		oldrply = open->oldrply;
		do {
			nxtrply = oldrply->next;
			free(oldrply);
			oldrply = nxtrply;
		} while (oldrply != open->oldrply);
	}

	/*
	 * Unlink the open entry from the opener's list of open entries.
	 */
	for (prevx = &myinfo->openers [opnrx].frstopnx; *prevx != opnx; prevx
	= &myinfo->opens [*prevx].nextopnx)
		assert((unsigned) *prevx < myinfo->maxopens);
	*prevx = open->nextopnx;

	/*
	 * Reset the open's table entry; clear all of its status flags.
	 */
	open->prifile = open->bakfile = -1;
	open->oldrply = NULL;
	open->openerx = -1;
	open->nextopnx = -1;
	open->openflags = 0;
	open->flags = 0;
	
	

	/*
	 * Reset the opener's table entry if no other open is using it.
	 */
	if (myinfo->openers [opnrx].frstopnx == -1) {
		OPENER *opener = &myinfo->openers [opnrx];

		PROCESSHANDLE_NULLIT_(opener->priproc);
		PROCESSHANDLE_NULLIT_(opener->bakproc);
		opener->pricpu = opener->bakcpu = -1;
		opener->node = -1;
	}
} // open_free


/******************************************************************************
 *
 * Syntax:
 *      OLDREPLY *reply_save (OPENFILE *open, unsigned long syncid,
 *                          char *msgbuf, int msglen, short error)
 *
 * Description:
 *      Create (actually, recycle) and initialize a new reply descriptor for
 *      an open.
 *
 *      The sync id of the new reply must not be null (0), and the message
 *      length must not exceed the maximum reply size for the server.
 *
 * Results:
 *      The function returns a pointer to the saved reply descriptor, or NULL
 *      if the open does not have any saved reply descriptors allocated.
 *
 ******************************************************************************/

static OLDREPLY *reply_save(OPENFILE *open, /* (in)  the open's table entry. */
		unsigned long syncid, /* (in)  the open's request sync id. */
		char *msgbuf, /* (in)  reply message data buffer. */
		int msglen, /* (in)  reply message data length. */
		short error) /* (in)  reply file system error code. */
{
	OLDREPLY *newrply;

	if (open->oldrply == NULL)
		return (NULL);

	assert(myinfo != NULL);

	assert(syncid != 0);
	assert(msgbuf != NULL && msglen >= 0 || msglen == 0);
	assert(msglen <= myinfo->maxrplylen);

	newrply = open->oldrply;
	open->oldrply = newrply->next;

	newrply->syncid = syncid;
	newrply->msglen = msglen;
	newrply->error = error;

	if (msgbuf != NULL)
		memcpy((char *) (newrply + 1), msgbuf, msglen);

	return (newrply);
} // reply_save


/******************************************************************************
 *
 * Syntax:
 *      OLDREPLY *reply_find (OPENFILE *open, unsigned long syncid)
 *
 * Description:
 *      Look up a reply previously sent to an open and saved, or a reply
 *      previously queued for an open by the server.  The sync id identifies
 *      the reply is if it is a saved reply.  The sync id is -1 if a queued
 *      reply is desired.
 *
 *      If a queued reply is returned, the function removes it from the list
 *      of replies queued for the open.  The caller is responsible for freeing
 *      the reply descriptor.
 *
 * Results:
 *      The function returns a pointer to the reply descriptor, or NULL if it
 *      did not find a saved or queued reply.
 *
 ******************************************************************************/

static OLDREPLY *reply_find(OPENFILE *open, /* (in)  the open's table entry. */
		unsigned long syncid) /* (in)  the open's request sync id. */
{
	OLDREPLY *reply;

	/*
	 * There is no reply if the open has no saved replies, or if
	 * the sync id is older than the open's oldest saved reply.
	 */
	if (open->oldrply == NULL || syncid < open->oldrply->syncid)
		return (NULL);

	reply = open->oldrply;
	do {
		if (reply->syncid == syncid)
			return (reply);
		reply = reply->next;
	} while (reply != open->oldrply);

	return (NULL);
} // reply_find


/******************************************************************************
 *
 * Syntax:
 *      int sysmsg_selected (short msgtype)
 *
 * Description:
 *      Determine whether a Guardian system message type is currently selected
 *      for processing by the server or not.
 *
 * Results:
 *      The function returns non-zero if the system message is selected, or
 *      zero if it is not selected.
 *
 ******************************************************************************/

static int sysmsg_selected(short msgtype) /* (in)  the system message type code. */
{
	assert(myinfo != NULL);

	if (msgtype <= -100) {
		if ((0x80000000u >> (-100 - msgtype)) & myinfo->smm [1])
			return (1);
	} else if (msgtype <= -1) {
		if (msgtype <= -32)
			msgtype += 13;
		else if (msgtype <= -21)
			msgtype += 10;
		else if (msgtype <= -7)
			msgtype += 3;

		if ((0x80000000u >> (-1 - msgtype)) & myinfo->smm [0])
			return (1);
	}

	return (0);
} // sysmsg_selected


/******************************************************************************
 *
 * Syntax:
 *      short sysmsg_receive (SysMsg_Type *sysmsg, int *msglen,
 *                          ReceiveInfo_Type *rcvinfo)
 *
 * Description:
 *      Perform internal processing of system messages, including updating
 *      the opens table.
 *
 * Results:
 *      The function returns a file system error code that can be sent when
 *      replying to the system message.  Values less than FEBADERR indicate
 *      that the message is acceptable and can be returned to the server if
 *      it is selected.  Other values indicate that the message cannot be
 *      honored and should be replied to immediately.
 *
 ******************************************************************************/

static short sysmsg_receive(SysMsg_Type *sysmsg, /* (i/o) the system message to process.  Some
 fields may be updated on output. */
		int *msglen, /* (i/o) the length of the system message. */
		ReceiveInfo *rcvInfo, /* (i/o) additional information.  On output,
 OpenLabel is set to the opens table
 index of the sending connection, or to
 -1 if the message is from the system. */
		int pathSendMode) {
	short opnrx, opnx, rqstx,prqstx;
	OPENER *opener;
	OPENFILE *open;
	long downnode;
	short downcpu;
	assert(myinfo != NULL && myinfo->goners == -1);

	switch (sysmsg->MsgType) {
	case OpenD00_SysMsg:
		/*
		 * Verify that this open won't send us more than one request at
		 * a time (if we're multi-threaded), and that the opener doesn't
		 * expect us to save too many replies (if we support the sync-
		 * depth mechanism).
		 */
		
		if (myinfo->syncdepth > 0 && sysmsg->variant.OpenD00.SyncDepth
				> myinfo->syncdepth)
			return (FETOOMANY);

		/*
		 * Add the connection to the opens table.  If the sender is a
		 * backup process, this can fail if the primary has not opened
		 * the server, or if the open flags differ.  If the sender is
		 * a primary process, this can fail for lack of resources.
		 */
		if ((open = open_add(sysmsg, rcvInfo, pathSendMode)) == NULL)
			return ((short) (sysmsg->variant.OpenD00.BackupOpen ? FENOPRIMARY
					: FEOPENSTOP));

		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode); /* don't save any reply to this message */
		return (FEOK);

	case CloseD00_SysMsg:
		if ((open = open_find(rcvInfo, pathSendMode)) == NULL)
			return (FEWRONGID);

		if (open->bakfile == -1 && !sysmsg_selected(CloseD00_SysMsg)) {
			// if there is no backup open and user didn't select close msgs
			// then see if they selected dialog abort or cancel and if
			// these particular messages apply.  If they do apply then
			// we need to synthesize a message.

			/*
			 * This code was copied from sysmsg_synthesize(), which
			 * we would have arranged to be called if an opener, and
			 * not merely an open, had gone away.
			 */

			rqstx = open->firstrqstx;
			if ((open->flags & DLGFLAGS_OPEN)
					&& sysmsg_selected(AbortDialog_SysMsg)) {
				// we're in middle of dialog and user has selected dialog aborts
				sysmsg->MsgType = AbortDialog_SysMsg;
				*msglen = sizeof sysmsg->MsgType;

				open->flags |= DLGABORT_OPEN;
			} else{
				
					if (myinfo->rqstinfo[rqstx].gmsgtag >= 0 && sysmsg_selected(Cancel_SysMsg)) {
						// this open has outstanding request(s) and user
						// has selected to receive cancel messages
					
						sysmsg->MsgType = Cancel_SysMsg;
						sysmsg->variant.Cancel.Tag = rqstx;
						*msglen = sizeof sysmsg->MsgType + sizeof sysmsg->variant.Cancel;
					}
			}
	
			/* rcvinfo is already set up by the CLOSE message. */
		}

		open->flags |= CLOSING_OPEN; // open is going away

		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode); /* don't save any reply to this message */
		return (FEOK);

	case ResetSync_SysMsg: {
		OLDREPLY *reply;

		if ((open = open_find(rcvInfo, pathSendMode)) == NULL)
			return (FEWRONGID);

		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode); /* don't save any reply to this message */
		if (open->oldrply == NULL)
			return (FEINVALOP);

		// throw away all saved replies so can start saving new ones
		reply = open->oldrply;
		do {
			reply->syncid = 0;
			reply->msglen = 0;
			reply->error = 0;
			reply = reply->next;
		} while (reply != open->oldrply);
		/* Resetting the syncid of all pending requests,so that they never get saved*/
		for(rqstx= open->firstrqstx; rqstx!=ENDOFREQ; rqstx=myinfo->rqstinfo[rqstx].nextrqstx){
			myinfo->rqstinfo[rqstx].syncid = 0;
		}
		return (FEOK);
	}

	case Cancel_SysMsg:

		for (rqstx =0;rqstx < myinfo->recvdepth; rqstx++) {
			if(myinfo->rqstinfo[rqstx].gmsgtag==sysmsg->variant.Cancel.Tag)	
				break;
		}
		if(rqstx >= myinfo->recvdepth)
			return (FEWRONGID);

		opnx= myinfo->rqstinfo[rqstx].openx;
		open= &myinfo->opens[opnx];
		
		sysmsg->variant.Cancel.Tag = rqstx;

		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, opnx, pathSendMode);
		if (open->prifile != -1) {
			SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, open->prifile,
					pathSendMode);
			memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
					myinfo->openers [open->openerx].priproc,
					sizeof myinfo->openers->priproc);
		} else {
			SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, open->bakfile,
					pathSendMode);
			memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
					myinfo->openers [open->openerx].bakproc,
					sizeof myinfo->openers->bakproc);
		}

		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode); /* don't save any reply to this message */
		return (FEOK);

	case Control_SysMsg:
	case ControlBuf_SysMsg:
	case SetMode_SysMsg:
	case SetParam_SysMsg:
		if ((open = open_find(rcvInfo, pathSendMode)) == NULL)
			return (FEWRONGID);

		/*
		 * If the system message has not been selected by the server,
		 * this error code will be returned to the requester.
		 */
		return (FEINVALOP);

	case AbortDialog_SysMsg:
		if ((open = open_find(rcvInfo, pathSendMode)) == NULL)
			return (FEWRONGID);

		open->flags |= DLGABORT_OPEN;

		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode); /* don't save any reply to this message */
		return (FEOK);

	case CpuDown_SysMsg:
	case RemoteCpuDown_SysMsg:
		if (sysmsg->MsgType == CpuDown_SysMsg) {
			downnode = -1; /* our local node; see open_add() */
			downcpu = sysmsg->variant.CpuDown.cpudown1.Cpu [0];
		} else {
			downnode = sysmsg->variant.RemoteCpuUpDown.System;
			downcpu = sysmsg->variant.RemoteCpuUpDown.Cpu;
		}

		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);

		for (opnrx = 0, opener = myinfo->openers; opnrx < myinfo->maxopeners;
		opnrx++, opener++) {
			if (opener->node != downnode)
				continue;

			// just because primary or backup went down doesn't mean
			// we lost an opener (we may have just lost an open)
			// but we let sysmsg_synthesize sort that out and so
			// add entry to goners list.  sysmsg_synthesize will
			// take care of generating any necessary closes, dialog
			// aborts or cancel msgs.
			if (opener->bakcpu == downcpu) {
				opener->bakcpu = -1;
				opener->nextopnrx = myinfo->goners;
				myinfo->goners = opnrx;
			} else if (opener->pricpu == downcpu) {
				opener->pricpu = -1;
				opener->nextopnrx = myinfo->goners;
				myinfo->goners = opnrx;
			}
		}

		return (FEOK);

	case NodeDown_SysMsg:
		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);

		downnode = sysmsg->variant.NodeUpDown.System;

		for (opnrx = 0, opener = myinfo->openers; opnrx < myinfo->maxopeners;
		opnrx++, opener++) {
			if (opener->node != downnode)
				continue;

			opener->pricpu = opener->bakcpu = -1;
			opener->nextopnrx = myinfo->goners;
			myinfo->goners = opnrx;
		}

		return (FEOK);

	case CpuUp_SysMsg:
	case SetTime_SysMsg:
	case PowerOn_SysMsg:
	case RemoteCpuUp_SysMsg:
	case NodeUp_SysMsg:
		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);
		return (FEOK);

	case LostStatusMsgs_SysMsg: {
		// broadcast system messages such as CPU/NET UP/DOWN were lost
		// so we check if we lost an opener and didn't get notified.
		short status, detail;

		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);

		for (opnrx = 0, opener = myinfo->openers; opnrx < myinfo->maxopeners;
		opnrx++, opener++) {
			if (opener->bakcpu != -1 && (status = PROCESS_GETINFO_(
					opener->bakproc,
					/* char *proc-fname */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *proc-fname-len */OMITREF,
					/* shor *priority */OMITREF,
					/* short *mom39s-processhandle */OMITREF,
					/* char *hometerm */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *hometerm-len */OMITREF,
					/* TMI64 *process-time */OMITREF,
					/* short *creator-access-id */OMITREF,
					/* short *process-access-id */OMITREF,
					/* short *gmom39s-processhandle */OMITREF,
					/* short *jobid */OMITREF,
					/* char *program-file */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *program-len */OMITREF,
					/* char *swap-file */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *swap-len */OMITREF, &detail,
					/* short *proc-type */OMITREF,
					/* long *oss-pid */OMITREF)) != 0) {

				assert(status >= 4 && status <= 6);

				opener->bakcpu = -1;
				opener->nextopnrx = myinfo->goners;
				myinfo->goners = opnrx;
			} // end if have backup opener

			if (opener->pricpu != -1 && (status = PROCESS_GETINFO_(
					opener->priproc,
					/* char *proc-fname */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *proc-fname-len */OMITREF,
					/* shor *priority */OMITREF,
					/* short *mom39s-processhandle */OMITREF,
					/* char *hometerm */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *hometerm-len */OMITREF,
					/* TMI64 *process-time */OMITREF,
					/* short *creator-access-id */OMITREF,
					/* short *process-access-id */OMITREF,
					/* short *gmom39s-processhandle */OMITREF,
					/* short *jobid */OMITREF,
					/* char *program-file */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *program-len */OMITREF,
					/* char *swap-file */OMITREF,
					/* short maxlen */OMITSHORT,
					/* short *swap-len */OMITREF, &detail,
					/* short *proc-type */OMITREF,
					/* long *oss-pid */OMITREF)) != 0) {

				assert(status >= 4 && status <= 6);

				opener->pricpu = -1;
				if (myinfo->goners != opnrx) {
					opener->nextopnrx = myinfo->goners;
					myinfo->goners = opnrx;
				}
			} // end if have primary opener
		} // end for each opener

		return (FEOK);
	}

	default:
		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);

		/*
		 * If the system message has not been selected by the server,
		 * this error code will be returned to the requester.
		 */
		return (FEINVALOP);
	}
} // sysmsg_receive


/******************************************************************************
 *
 * Syntax:
 *      int sysmsg_synthesize (SysMsg_Type *sysmsg, int *msglen,
 *                           ReceiveInfo_Type *rcvinfo)
 *
 * Description:
 *      Process status changes to openers and their opens.  Free opens (and
 *      their deceased openers) that do not require selected system messages.
 *      Synthesize system messages for opens that require them.  The deceased
 *      openers are assumed to be on the myinfo->goners list.
 *
 *      This function can be called in two distinct modes.  If a message buffer
 *      is supplied, it searches for an open that requires a message, synthe-
 *      sizes the message, and positions myinfo->goners past the open.  If no
 *      message buffer is supplied, it searches for an open that requires a
 *      message, and positions myinfo->goners to that open.  This latter mode
 *      is used by jpwy_initrecv() to determine whether the myinfo->goners
 *      list actually has an open that requires a message to be synthesized.
 *
 * Results:
 *      The function returns 1 if it found an open that requires a synthesized
 *      message, or 0 if it exhausted the myinfo->goners list.
 *
 * Side Effects:
 *      myinfo->goners is updated as openers are removed from the list.  After
 *      each call that synthesizes a message, myinfo->goners points to the next
 *      opener whose first (remaining) open requires a synthesized message.
 *      It is set to nil (-1) when all openers have been processed.
 *
 ******************************************************************************/

static int sysmsg_synthesize(SysMsg_Type *sysmsg, /* (out) the synthesized system message. */
		int *msglen, /* (out) the length of the system message. */
		ReceiveInfo *rcvInfo, /* (out) additional information about the
 message and its sending connection. */
		int pathSendMode) {
	short opnx;
	short nextopnrx, nextopnx;
	OPENER *opener;
	OPENFILE *open;
	short rqstx=0,prqstx=0;

	assert(sysmsg != NULL && msglen != NULL && rcvInfo != NULL || sysmsg
			== NULL && msglen == NULL && rcvInfo == NULL);

	

	for (; myinfo->goners != -1; myinfo->goners = nextopnrx) {
		opener = &myinfo->openers [myinfo->goners];
		nextopnrx = opener->nextopnrx;

		for (opnx = opener->frstopnx; opnx != -1; opnx = nextopnx) {
			open = &myinfo->opens [opnx];
			nextopnx = open->nextopnx;
			
			rqstx = open->firstrqstx;
			/*
			 * If the open still has a primary opener or a backup
			 * opener, it is still good so don't process it.
			 */
			if (open->prifile != -1 && opener->pricpu != -1 || open->bakfile
					!= -1 && opener->bakcpu != -1)
				continue;

			/*
			 * Much of the following code to synthesize messages is
			 * replicated in the CloseD00_SysMsg case of sysmsg_-
			 * receive().  If you add or change code here, consider
			 * making an equivalent change there.
			 */
			if (sysmsg_selected(CloseD00_SysMsg)) {
				if (sysmsg == NULL)
					return (1);

				sysmsg->MsgType = CloseD00_SysMsg;
				sysmsg->variant.CloseD00.TapeDisposition = 0;
				*msglen = sizeof sysmsg->MsgType
				+ sizeof sysmsg->variant.CloseD00;
			} else if ((open->flags & DLGFLAGS_OPEN)
					&& sysmsg_selected(AbortDialog_SysMsg)) {
				if (sysmsg == NULL)
					return (1);

				sysmsg->MsgType = AbortDialog_SysMsg;
				*msglen = sizeof sysmsg->MsgType;

				open->flags |= DLGABORT_OPEN;
			} else if (myinfo->rqstinfo[rqstx].gmsgtag >= 0 && sysmsg_selected(Cancel_SysMsg)) {	
				sysmsg->MsgType = Cancel_SysMsg;
				sysmsg->variant.Cancel.Tag = rqstx;
				*msglen = sizeof sysmsg->MsgType
				+ sizeof sysmsg->variant.Cancel;
			} else {
				open_free(open);
				continue;
			}

			open->flags |= CLOSING_OPEN;

			SET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, SysMsg_ReceiveType,
					pathSendMode);
			SET_RECEIVEINFO_FIELD(rcvInfo, MaxReplyLen, 0, pathSendMode);
			SET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, -2, pathSendMode);
			SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode);
			SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, opnx, pathSendMode);
			if (open->prifile != -1) {
				memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
						opener->priproc, sizeof opener->priproc);
				SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, open->prifile,
						pathSendMode);
			} else {
				memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
						opener->bakproc, sizeof opener->bakproc);
				SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, open->bakfile,
						pathSendMode);
			}

			return (1);
		}

		if (opener->frstopnx != -1) {
			/*
			 * We did not free all of the opener's opens.  Clear
			 * the backup opens if the backup opener was lost, or
			 * promote the backup opens to primary opens if the
			 * primary opener was lost.
			 */
			if (opener->bakcpu == -1) {
				PROCESSHANDLE_NULLIT_(opener->bakproc);
				for (opnx = opener->frstopnx; opnx != -1; opnx
				= myinfo->opens [opnx].nextopnx)
					myinfo->opens [opnx].bakfile = -1;
			} else {
				memcpy(opener->priproc, opener->bakproc, sizeof opener->priproc);
				PROCESSHANDLE_NULLIT_(opener->bakproc);
				for (opnx = opener->frstopnx; opnx != -1; opnx = open->nextopnx) {
					open = &myinfo->opens [opnx];
					open->prifile = open->bakfile;
					open->bakfile = -1;
				}
			}
		}
	}

	return (0); // no msgs need to be synthesized due to lost openers
} // sysmsg_synthesize


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_reply (char *msgbuf, int msglen, short error,
 *                       short msgtag, int *countReplied)
 *
 * Description:
 *      Send a reply message in response to a previously received request or
 *      system message.  The request or system message is identified by the
 *      message tag assigned to it when it was received.
 *
 *      The error parameter specifies a file system error code to send along
 *      with the reply message data.  The operation used by the sender to send
 *      the original request appears to succeed or fail with this error code.
 *
 *      The msgtag parameter identifies which request or system message is
 *      being replied to.  It is normally obtained from the rcvinfo.MessageTag
 *      field of the system information returned when the request is received.
 *      A single-threaded server, however, never has more than one message
 *      outstanding at a time and all of its message tags are zero.  Thus,
 *      single-threaded servers can always supply zero for this parameter.
 *
 *      If the original request was an OPEN system message, the reply message
 *      buffer and length are ignored.  The module accepts or rejects the
 *      sender's attempt to open the server according to whether the error
 *      parameter specifies a success or failure error code.
 *
 * Results:
 *      The function returns a Guardian error code as follows:
 *
 *      FEOK          The reply message was sent successfully.
 *
 *      FENOTOPEN     The module is not initialized by jpwy_open().
 *
 *      FEBADCOUNT    The length of the reply data exceeds the maximum length
 *                    for saved reply data specified via jpwy_open().
 *
 *      FEBADREPLY    The message tag does not designate an outstanding
 *                    request or system message.  This error code is always
 *                    returned if the module has been initialized with a
 *                    receive depth of 0.
 *
 *      other         A file system error occurred while sending the reply.
 *                    The request message is no longer considered outstanding.
 *
 ******************************************************************************/

int jpwy_reply(char *msgbuf, /* (in)  reply message data to return to the
 sender. */
		int msglen, /* (in)  actual size of the reply message. */
		short error, /* (in)  a file system error code to return
 with the reply. */
		short msgtag, /* (in)  identifies the original request. */
		long *countReplied, /* (out) the actual number of bytes replied. */
		int pathSendMode) {
	OPENFILE *sender = NULL;
	RQSTINFO *request = NULL;
	RQSTINFO *prev = NULL;
	short gmsgtag;
	short rplybuf [2];
	short fserr, fierr;
	long fillerCount;
	short repcount=0;
	initialise_functionptr();/*initialising function pointers */
	assert(msgbuf != NULL && msglen >= 0 || msglen == 0);

	request= &(myinfo->rqstinfo[msgtag]);  /*The request or system message is identified by the message tag assigned 
   											to it when it was received */
	

	if (myinfo == NULL)
		return (FENOTOPEN);

	if ((unsigned) msgtag >= myinfo->recvdepth || request->openx == RQSTFREE)
		return (FEBADREPLY);

	if (request->openx >= 0) {
		// request is outstanding and has an associated open
		assert((unsigned) request->openx < myinfo->maxopens);
		sender = &(myinfo->opens [request ->openx]);
        
		//TBD: It might be better if we stored whether open or close request in the
		// future request object instead of relying on flags.
		if (sender->flags & OPENING_OPEN) {
			if (error < FEBADERR) {
				rplybuf [0] = OpenD00_SysMsg;
				rplybuf [1] = request->openx; // this is the open label	
				msgbuf = (char *) rplybuf;
				msglen = sizeof rplybuf;
			} else
				msglen = 0;
		} else if (sender->oldrply != NULL && msglen > myinfo->maxrplylen)
			return (FEBADCOUNT);

	} 
	/* Retreive the gmsgtag stored in RQSTINFO entry rather than depending on Open entries*/
	gmsgtag = request->gmsgtag;


	if (countReplied == NULL)
		countReplied = &fillerCount;
	if(gmsgtag != -1){
	// gmsgtag = -2 for synthesized messages which don't need a call to replyx
	if (pathSendMode
			==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT) {
		if (gmsgtag != -2 && spt_REPLYX(msgbuf, msglen, &repcount, gmsgtag,
				error) != 0) {
			// spt_REPLYX failed get error info
			fierr = FILE_GETINFO_(JPWY_FNUM, &fserr, OMITREF, OMITSHORT,
					OMITREF, OMITREF, OMITREF);
			if (fierr != FEOK)
				fserr = fierr;
		} else
			fserr = FEOK; // REPLYX succeeded or didn't need to be called
		*countReplied=(unsigned short)repcount;
	} else {
		if (gmsgtag != -2 && spt_replyxl(msgbuf, msglen, countReplied, gmsgtag,
				error) != 0) {
			// spt_replyxl failed get error info
			fierr = FILE_GETINFO_(JPWY_FNUM, &fserr, OMITREF, OMITSHORT,
					OMITREF, OMITREF, OMITREF);
			if (fierr != FEOK)
				fserr = fierr;
		} else
			fserr = FEOK; // REPLYXL succeeded or didn't need to be called

	}
	

	/*Resetting the request entries that has been replied*/
	request ->openx= RQSTFREE;
	request ->syncid = 0;
	request ->gmsgtag = -1;
	request ->maxrplylen = -1;
    if(sender != NULL){	
	/*If sender has only one request*/
	if(sender->firstrqstx == sender->lastrqstx && sender->firstrqstx == msgtag)
	{
		sender->firstrqstx=sender->lastrqstx= NORQST;
	}     
	/*	If sender has more than one request and the request to be found is the first request, then
		We need to change the senders first request index also.*/
	else if(sender->firstrqstx==msgtag){
		sender->firstrqstx= request->nextrqstx;		
	}

	else{
      	// Loop through all the requests for the open and until we find the msgtag and keep the prev request safe.
		short prevrqstx,rqstx;
		for( rqstx=sender->firstrqstx; rqstx!=msgtag && rqstx!=ENDOFREQ; rqstx=prev->nextrqstx){
			prev=&myinfo->rqstinfo[rqstx];
			prevrqstx = rqstx;
		}
		//If the found request is the last request of the open then we need to change the last request index for the open
		if(sender->lastrqstx == msgtag){
			prev-> nextrqstx = ENDOFREQ;
			sender->lastrqstx = prevrqstx;
		}
		else{	
			//Reset the pointer to the next request
			if(rqstx!= ENDOFREQ)
				prev-> nextrqstx = request->nextrqstx; 
		}
	}
	
	/*
	 * Update the sender's entry only after the reply is successfully sent.
 	*/
	if ((sender->flags & DLGABORT_OPEN) || (sender->flags & DLGFLAGS_OPEN)
			&& error != FECONTINUE)
		sender->flags &= ~(DLGABORT_OPEN | DLGFLAGS_OPEN);

	if (sender->flags & DROPPED_OPEN) {
		open_free(sender);
	} else if (sender->flags & CLOSING_OPEN) {
		sender->flags &= ~CLOSING_OPEN;
		if (sender->bakfile != -1) {
			sender->prifile = -1;
		} else {
			open_free(sender);
		}
	} else if (sender->flags & OPENING_OPEN) {
		sender->flags &= ~OPENING_OPEN;
		if (error >= FEBADERR) {
			if (sender->bakfile != -1) {
				sender->bakfile = -1;
			}  /*Changes begin for 10-100108-7367
	else {
				open_free(sender);
			} Changes begin for 10-100108-7367 */
		} else {
			if (sender->bakfile == -1) {
				if (myinfo->numopens < 0)
					myinfo->numopens = 0;
				myinfo->numopens++;
			}
		}
	   } else if (sender->oldrply != NULL && request->syncid !=0) {
		(void) reply_save(sender, request->syncid, msgbuf, msglen, error);
	  }
	}
	request->nextrqstx=ENDOFREQ;
	}
	else 
		fserr = FEOK;

	return ((int) fserr);
} // jpwy_reply


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_initrecv (char *msgbuf, int buflen, long tag)
 *
 * Description:
 *      Initiate a nowait read operation on the server's $RECEIVE file.  The
 *      operation must be completed using AWAITIOX() or FILE_COMPLETE_().
 *      jpwy_comprecv() must then be called to notify the module that the
 *      operation is complete.
 *
 *      The message buffer must be at least 250 bytes long, so that it can
 *      hold a system message, even if the system message will be processed
 *      internally by the module.
 *
 * Results:
 *      The function returns a Guardian error code as follows:
 *
 *      FEOK          The receive operation was initiated successfully.
 *
 *      FEEOF         All of the server's openers have terminated or closed
 *                    the server.  No operation was initiated.  If the server
 *                    has selected the system message that triggered this
 *                    condition, the error code is returned on the server's
 *                    next attempt to receive a message, after processing
 *                    the system message.
 *
 *      FESYSMESS     A selected system message was synthesized.  No i/o
 *                    operation was initiated.  Call jpwy_comprecv(),
 *                    without first calling AWAITIOX(), to retrieve the
 *                    message.  jpwy_comprecv() will return FESYSMESS.
 *
 *      FENOTOPEN     The module is not initialized by jpwy_open().
 *
 *      other         A file system error occurred.  No i/o operation was
 *                    initiated.
 *
 ******************************************************************************/

int jpwy_initrecv(char *msgbuf, /* (in)  a message buffer to receive the request
 or system message. */
		int buflen, /* (in)  maximum size of the message buffer. */
		long *countRead, /* (out)  count Transferred */
		JPWY_MSGINFO *msginfo, /* (out) msgInfo returned from spt_RECEIVEREAD */
		int pathSendMode) /*Tsmp Version information */
{
	long err;

	initialise_functionptr();/*initialising function pointers */

	assert(msgbuf != NULL && buflen >= 250);

	if (myinfo == NULL)
		return (FENOTOPEN);

	/*
	 * If the server is single-threaded, make sure the server has already
	 * issued its reply to the previous message.
	 */
	if (myinfo->recvdepth == 1 && myinfo->rqstinfo[0].openx != RQSTFREE)
		return (FEBADREPLY);

	/*
	 * If we have a synthesized message, tell the caller to get it with
	 * jpwy_comprecv().
	 */
	// calling sysmsg_synthesize with null msg buf treats the procedure
	// more as a boolean (true if there are msgs to be synthesized, false
	// if there are not) there is no attempt made to create the
	// synthesized message.
	if (myinfo->goners != -1 && sysmsg_synthesize(NULL, NULL, NULL,
			pathSendMode))
		return (FESYSMESS);

	/*
	 * If we have deleted our last opener, tell the caller to shut down
	 * (but let him continue if he wants to).
	 */
	if (myinfo->numopens == 0) {
		myinfo->numopens = -1;
		return (FEEOF);
	}
	if (pathSendMode
			==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT) {

		err = spt_RECEIVEREAD(JPWY_FNUM, msgbuf, buflen, countRead, -1,
				(short*)&msginfo->rcvinfo.rcvInfo, (short*)&msginfo->dlgflags);
	} else {
		err = spt_receivereadl(JPWY_FNUM, msgbuf, buflen, countRead, -1,
				(short*)&msginfo->rcvinfo.rcvInfo2);
	}
	if (err < 0)
		err = (long)FEREQABANDONED;

	return (int)err; // Return the Guardian Error from spt_RECEIVEREAD

} // jpwy_initrecv


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_comprecv (char *msgbuf, int *msglen,
 *                          JPWY_MSGINFO *msginfo)
 *
 * Description:
 *      Indicate that a previously initiated, nowait receive operation has
 *      completed.  Determine whether the message requires processing by the
 *      server.  Caller should call FILE_GETRECEIVEINFO_ and pass the
 *      information into this procedure.
 *
 *      This function should be called only if the jpwy_initrecv() operation
 *      returned the FEOK or FESYSMESS error codes.  If the operation completed
 *      with an error, the server should perform appropriate recovery actions.
 *
 *      If the message received by the operation is a system message that has
 *      not been selected by the server, the function processes it and replies
 *      to it internally.  In this case, the server should just initiate another
 *      receive operation using jpwy_initrecv().  If the message is a request
 *      message or a selected system message, however, the server should process
 *      it and reply to it.
 *
 *      The function may synthesize and return a system message if this message,
 *      or the previously received system message, indicates that a connection
 *      to the server has been lost, and if the server has selected that type of
 *      system message.  Only CANCEL, dialog abort, and CLOSE system messages
 *      can be synthesized.  Only one message is synthesized for each terminated
 *      connection.  If more than one message applies to a connection and has
 *      been selected, CLOSE messages have highest preference, then dialog abort
 *      messages, then CANCEL messages.
 *
 * Results:
 *      The function returns a Guardian error code as follows:
 *
 *      FEOK          A request message was received from a current opener.
 *
 *      FESYSMESS     A selected system message was received or synthesized.
 *                    The system information describing the sender is valid
 *                    only if the message was sent by an opener of the server
 *                    (or prospective opener in the case of an OPEN message).
 *
 *      FEEOF         All of the server's openers have terminated or closed
 *                    the server.  If the server has selected the system
 *                    message that triggered this condition, the error code
 *                    is returned on the server's next attempt to receive
 *                    a message, after processing the system message.
 *
 *      FECONTINUE    The message was a system message not selected by the
 *                    server.
 *
 *      FENOTOPEN     The module is not initialized by jpwy_open().
 *
 *      other         A file system error occurred while getting information
 *                    about the message.
 *
 ******************************************************************************/

int jpwy_comprecv(char *msgbuf, /* (i/o) the message buffer originally passed
 to jpwy_initrecv() and returned by
 AWAITIOX(). */
		int *msglen, /* (i/o) the actual message length returned by
 AWAITIOX() or FILE_COMPLETE_(). */
		JPWY_MSGINFO *msginfo, /* (i/o) system information associated with the
 received or synthesized message. */
		int pathSendMode) /*Tsmp Version information */
{

	short *dlgflags;
	OPENFILE *sender;
	OLDREPLY *reply;
	short fserr;
	short rqstx;
	RQSTINFO *request;
	short lastrqstx;
	
	ReceiveInfo *rcvInfo=&(msginfo->rcvinfo);
	if (pathSendMode
			==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
		dlgflags=&(msginfo->dlgflags);
	else
		dlgflags=&(msginfo->rcvinfo.rcvInfo2.DialogInfo);

	initialise_functionptr();/*initialising function pointers */

	assert(msgbuf != NULL && msglen != NULL && *msglen >= 0 && msginfo != NULL);

	if (myinfo == NULL)
		return (FENOTOPEN);

	if (myinfo->goners != -1) {
		/*
		 * See if we need to synthesize any messages.  Return one if
		 * we do.  Otherwise, if the message deleted our last opener,
		 * tell the caller to shut down (but let him continue if he
		 * wants to).
		 */

		if (!sysmsg_synthesize((SysMsg_Type *) msgbuf, msglen, rcvInfo,
				pathSendMode)) {
			if (myinfo->numopens != 0)
				return (FECONTINUE);
			*msglen = 0;
			myinfo->numopens = -1;
			return (FEEOF);
		}

		*dlgflags
		= (short)((((SysMsg_Type *) msgbuf)->MsgType
				== AbortDialog_SysMsg ? JPWY_DLGABORT : JPWY_NODIALOG));

		if (GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode) != -1) {
			assert((unsigned) GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel,
					pathSendMode) < myinfo->maxopens);
			sender = &myinfo->opens [GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode)];
			*dlgflags |= ((sender->flags & DLGFLAGS_OPEN) >> 4)
			& JPWY_DLGCHGTRAN;
		} else {
			sender = NULL;
		}

		goto retn_msg;
	}

	if (GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, pathSendMode)
			== SysMsg_ReceiveType) {
		// a system message was read in
		fserr = sysmsg_receive((SysMsg_Type *) msgbuf, msglen, rcvInfo,
				pathSendMode);

		/*
		 * Reply immediately if the message was rejected, unless it is
		 * a selected system message from an opener that was dropped.
		 */
		if (fserr >= FEBADERR && (fserr != FEWRONGID || !sysmsg_selected(((SysMsg_Type *) msgbuf)->MsgType))) {

			if (pathSendMode
					==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
				spt_REPLYX(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), fserr);
			else
				spt_replyxl(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), fserr);
			return (FECONTINUE);
		}

		if (GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode) != -1) {
			assert((unsigned) GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel,
					pathSendMode) < myinfo->maxopens);
			sender = &myinfo->opens [GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode)];
		} else {
			// many system msgs are not assoicated with a particular
			// open and therefore the openLabel will be -1 and there
			// will be no "sender"
			sender = NULL;
		}
	} else { // a user message was read
		/*
		 * Reply immediately if the sender is not known to us.
		 */
		if ((sender = open_find(rcvInfo, pathSendMode)) == NULL) {
			if (pathSendMode
					==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
				spt_REPLYX(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), FEWRONGID);
			else
				spt_replyxl(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), FEWRONGID);
			return (FECONTINUE); // tell user to call jpwy_initrecv again
		}
	}

	if (sender != NULL && myinfo->recvdepth > 0) {
		// there is an open associated with the request and the user
		// is responsible for calling reply therefore we need to check
		// if this is a duplicate request.

		/*
		 * If the request is currently outstanding in the server, the
		 * original sender must have died and his backup is resending
		 * it.  Discard the old request and arrange to send the reply
		 * to the new (backup) sender.
		 */
		
		/*loop through all Outstanding request entries of this OPEN to find any duplicate message*/
		for(rqstx=sender->firstrqstx; rqstx!= ENDOFREQ;rqstx=request->nextrqstx)
		{
			request=&myinfo->rqstinfo[rqstx];
			if (request->gmsgtag >= 0 && GET_RECEIVEINFO_FIELD(rcvInfo, SyncId,
					pathSendMode) == request->syncid) {
				if (pathSendMode
						==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
					spt_REPLYX(NULL, 0, NULL, request->gmsgtag, FEOK);
				else
					spt_replyxl(NULL, 0, NULL, request->gmsgtag, FEOK);
				request->gmsgtag = GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
						pathSendMode);
				return (FECONTINUE); // tell user to call jpwy_initrecv again
			}
		

			/*
		 	* If the request has already been replied to, the original
		 	* sender must have died and his backup is sending a duplicate.
			 * Resend the old reply from our cache of saved replies, if we
		 	* still have it.
			 */
			if (GET_RECEIVEINFO_FIELD(rcvInfo, SyncId, pathSendMode) != 0
					&& GET_RECEIVEINFO_FIELD(rcvInfo, SyncId, pathSendMode)
					<= request->syncid) {
				if ((reply = reply_find(sender, GET_RECEIVEINFO_FIELD(rcvInfo,
						SyncId, pathSendMode))) != NULL) {
					// we found cached reply
					if (pathSendMode
							==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
						spt_REPLYX((char *) (reply + 1), reply->msglen, NULL,
								GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
										pathSendMode), reply->error);
					else
						spt_replyxl((char *) (reply + 1), reply->msglen, NULL,
								GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
										pathSendMode), reply->error);
				} else {
					// we didn't find cached reply
					if (pathSendMode
							==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
						spt_REPLYX(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
								MessageTag, pathSendMode), FEOLDSYNC);
					else
						spt_replyxl(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
								MessageTag, pathSendMode), FEOLDSYNC);
				}
				return (FECONTINUE); // tell user to call jpwy_initrecv again
			}
		}
	}

	if (GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, pathSendMode)
			== SysMsg_ReceiveType && !sysmsg_selected(((SysMsg_Type *) msgbuf)->MsgType)) {
		/*
		 * This is a system message that is not selected by the server.
		 * Process it and reply to it internally.
		 */
		short rplybuf [2];
		short rplylen = 0;

		switch (((SysMsg_Type *) msgbuf)->MsgType) {
		case OpenD00_SysMsg:
			rplybuf [0] = OpenD00_SysMsg;
			rplybuf [1] = GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel,
					pathSendMode);
			rplylen = sizeof rplybuf;

			sender->flags &= ~OPENING_OPEN;
			if (sender->bakfile == -1) {
				if (myinfo->numopens < 0)
					myinfo->numopens = 0;
				myinfo->numopens++;
			}
			break;

		case CloseD00_SysMsg:
			sender->flags &= ~CLOSING_OPEN;
			if (sender->bakfile != -1) {
				sender->prifile = -1;
			} else {
				// we don't have a backup open so the only open was
				// the primary and it's closed the server
				open_free(sender);
			}
			break;

		case Control_SysMsg:
		case ControlBuf_SysMsg:
		case SetMode_SysMsg:
		case SetParam_SysMsg:
			(void) reply_save(sender, GET_RECEIVEINFO_FIELD(rcvInfo, SyncId,
					pathSendMode), (char *) rplybuf, rplylen, fserr);
			break;
		
		default:
			break;
		}

		if (pathSendMode
				==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
			spt_REPLYX((char *) rplybuf, rplylen, NULL, GET_RECEIVEINFO_FIELD(
					rcvInfo, MessageTag, pathSendMode), fserr);
		else
			spt_replyxl((char *) rplybuf, rplylen, NULL, GET_RECEIVEINFO_FIELD(
					rcvInfo, MessageTag, pathSendMode), fserr);
		
		/*
		 * If the message changed the status of opener processes, see
		 * if we need to synthesize any messages.  Return one if we do.
		 * Otherwise, if the message deleted our last opener, tell the
		 * caller to shut down (but let him continue if he wants to).
		 */
		if (myinfo->goners == -1 || !sysmsg_synthesize((SysMsg_Type *) msgbuf,
				msglen, rcvInfo, pathSendMode)) {
			if (myinfo->numopens != 0)
				return (FECONTINUE);
			*msglen = 0;
			myinfo->numopens = -1;
			return (FEEOF);
		}

		*dlgflags
		= (short)((((SysMsg_Type *) msgbuf)->MsgType
				== AbortDialog_SysMsg ? JPWY_DLGABORT : JPWY_NODIALOG));

		if (GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode) != -1) {
			assert((unsigned) GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel,
					pathSendMode) < myinfo->maxopens);
			sender = &myinfo->opens [GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode)];
			*dlgflags |= ((sender->flags & DLGFLAGS_OPEN) >> 4)
			& JPWY_DLGCHGTRAN;
		} else {
			sender = NULL;
		}
	} // end if msg is a system msg that is not user selected

	retn_msg: if (myinfo->rqstinfo == NULL) {
		/*
		 * This server does not use jpwy_reply(), i.e. receive depth
		 * is 0.  Reply to the request.  Suppress the message tag
		 * since it is obsolete.
		 */
		if (GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, pathSendMode) != -2)
			if (pathSendMode
					==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
				spt_REPLYX(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), FEOK);
			else
				spt_replyxl(NULL, 0, NULL, GET_RECEIVEINFO_FIELD(rcvInfo,
						MessageTag, pathSendMode), FEOK);

		if (sender != NULL) // there is a specific open for this request
		{
			sender->flags &= ~(DLGABORT_OPEN | DLGFLAGS_OPEN);

			if (sender->flags & DROPPED_OPEN) {
				open_free(sender);
			} else if (sender->flags & CLOSING_OPEN) {
				sender->flags &= ~CLOSING_OPEN;
				if (sender->bakfile != -1) {
					sender->prifile = -1;
				} else {
					open_free(sender);
				}
			} else if (sender->flags & OPENING_OPEN) {
				sender->flags &= ~OPENING_OPEN;
				if (sender->bakfile == -1) {
					if (myinfo->numopens < 0)
						myinfo->numopens = 0;
					myinfo->numopens++;
				}
			}
		}

		SET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, -1, pathSendMode);
	} else { // user is handling their own replies (receive depth >= 1)
		/*
		 * Allocate a request table entry for this outstanding request.
		 * Replace the request's Guardian message tag by the request
		 * table index.
		 */
		//We need to find a position to insert the new request. openx == RQSTFREE means that 
		//This table entry is free and can be utilized for new request.
		for(rqstx=0; rqstx < (myinfo->recvdepth) ;rqstx++)
		{
			
			if(myinfo->rqstinfo[rqstx].openx==RQSTFREE)
				break;
			
		}

		assert( rqstx < myinfo->recvdepth);



		request=&myinfo->rqstinfo[rqstx];
		
		if (sender != NULL) {
			/* A cancel message can come in while a request is still
			 outstanding.  Cancel messages are not associated with
			 a specific open (i.e. the open label in the rcvinfo is
			 not valid) but sysmsg_receive looks up the associated
			 open and modifies the openLabel so this function can
			 find the sender of the cancel. This is the one case
			 where there can be two outstanding requests at the same
			 time, associated with the same open even though we
			 don't support clients sending more than one request at
			 a time per open.  Therefore, the cancel msg must act
			 like it doesn't have an open so it won't override
			 important fields associated with the original request.
			 Soln: 10-021119-3227
			 */
			int sysMsg = ((SysMsg_Type *) msgbuf)->MsgType;

			// Append the new request to the request list of this open
			if(sender->firstrqstx!=NORQST){
				myinfo->rqstinfo[sender->lastrqstx].nextrqstx=rqstx;
				sender->lastrqstx=rqstx;
			}
			else // If this is the first request
			{
				sender->firstrqstx=rqstx;
				sender->lastrqstx=rqstx;
			}

			if (sysMsg == Cancel_SysMsg) {
				
				/* This is how a msg tag is saved for requests that
				 aren't associated with a particular open.  The tag
				 to be used in the call to REPLYX can be calculated
				 using openx rather than using sender->gmsgtag. */
				/* If a cancel message comes, wrap the gmsgtag in the openx entry as -ve value since no OPEN associated with the Cancel*/
				request->openx = (short)(-GET_RECEIVEINFO_FIELD(rcvInfo,MessageTag,pathSendMode) - 2);	
			} else {
			/* Enter gmsgtag into gmsgtag entry of RQSTINFO array and assign the OPEN index to openx entry*/
				request->gmsgtag = GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
						pathSendMode);
				request->openx =GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel,
						pathSendMode);
			}
		} else {
		    /*change begin for the sol 10-120810-4368*/
		    request->gmsgtag = GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
						pathSendMode);
			/*change end for the sol 10-120810-4368*/
			request->openx = (short)(-GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag,
					pathSendMode) - 2);
		}
		/* Set the message tag of recvinfo with the offset of the request entry*/ 
		SET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, rqstx, pathSendMode) ;
	}

	if (sender != NULL) {
		sender->flags &= ~(RCVTYPE_OPEN | DLGFLAGS_OPEN);
		sender->flags |= (GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType,
				pathSendMode) & RCVTYPE_OPEN) | ((*dlgflags << 4)
						& DLGFLAGS_OPEN);

		request->maxrplylen = (unsigned short)(GET_RECEIVEINFO_FIELD(rcvInfo,
				MaxReplyLen, pathSendMode));

		if (GET_RECEIVEINFO_FIELD(rcvInfo, SyncId, pathSendMode) != 0)
			request->syncid = GET_RECEIVEINFO_FIELD(rcvInfo, SyncId,
					pathSendMode);
	}

	return (GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, pathSendMode)
			== SysMsg_ReceiveType ? FESYSMESS : FEOK);
} // jpwy_comprecv


/******************************************************************************
 *
 * Syntax:
 *      int jpwy_cancelled (short msgtag)
 *
 * Description:
 *      Indicate whether a currently outstanding, received message has been
 *      cancelled by its sender.  The message is identified by the tag assigned
 *      to it when it was received.
 *
 *      A cancelled message must still be replied to, even though the reply
 *      message data and error code are discarded.
 *
 *      The message tag is normally obtained from the rcvinfo.MessageTag field
 *      of the system information returned when the request is received.  A
 *      single-threaded server, however, never has more than one message out-
 *      standing at a time, so all of its message tags are zero.  Thus, single-
 *      threaded servers can always supply a message tag value of zero.
 *
 *      An alternative to using this function is to select message cancella-
 *      tion system messages by specifying CANCELLED_SMM1 to jpwy_open() or
 *      jpwy_setsmm().
 *
 * Results:
 *      The function returns 0 if the message sender is still expecting a
 *      reply to the message, or 1 if the message sender is no longer expecting
 *      a reply.
 *
 ******************************************************************************/

int jpwy_cancelled(short msgtag) /* (in)  identifies the message to check. */
{
	short gmsgtag;

	assert(myinfo != NULL);

	assert((unsigned) msgtag < myinfo->recvdepth && myinfo->rqstinfo[msgtag].openx != RQSTFREE);

	if (myinfo->rqstinfo[msgtag].openx >= 0)
		gmsgtag = myinfo->rqstinfo [msgtag].gmsgtag;
	else
		gmsgtag = (short)(-myinfo->rqstinfo[msgtag].openx - 2);

	return (gmsgtag >= 0 && MESSAGESTATUS(gmsgtag) != 0);
} // jpwy_cancelled


/******************************************************************************
 *
 * Syntax:
 *      short jpwy_activatetransid (short msgtag)
 *
 * Description:
 *      Activate the TMF transaction associated with a currently outstanding,
 *      received message.  The message is identified by the tag assigned to it
 *      when it was received.
 *
 *      The message tag is normally obtained from the rcvinfo.MessageTag field
 *      of the system information returned when the request is received.  A
 *      single-threaded server, however, never has more than one message out-
 *      standing at a time, so all of its message tags are zero.  Thus, single-
 *      threaded servers can always supply a message tag value of zero.
 *
 * Results:
 *      The function returns a Guardian error code as follows:
 *
 *      FEOK          The transaction was activated successfully.
 *
 *      FENOTOPEN     The module is not initialized by jpwy_open().
 *
 *      FEBADREPLY    The message tag does not designate an outstanding
 *                    request or system message.
 *
 *      other         A file system error occurred while resuming the
 *                    transaction.
 *
 ******************************************************************************/

short jpwy_activatetransid(short msgtag) /* (in)  identifies the message to check. */
{
	short gmsgtag;
	short fserr = FEOK, fierr;

	if (myinfo == NULL)
		return (FENOTOPEN);

	if (msgtag == -1) {
		/* tag -1 activates the "kept" transaction; let it through. */
		gmsgtag = msgtag;
	} else if ((unsigned) msgtag >= myinfo->recvdepth
			|| myinfo->rqstinfo[msgtag].openx == -1) {
		return (FEBADREPLY);
	} else if (myinfo->rqstinfo[msgtag].openx >= 0) {
		gmsgtag = myinfo->rqstinfo [msgtag].gmsgtag;
	} else {
		gmsgtag = (short)(-myinfo->rqstinfo[msgtag].openx - 2);
	}

	if (_status_ne(ACTIVATERECEIVETRANSID(gmsgtag))) {
		fierr = FILE_GETINFO_(JPWY_FNUM, &fserr, OMITREF, OMITSHORT, OMITREF,
				OMITREF, OMITREF);
		if (fierr != FEOK)
			fserr = fierr;
	}

	return (fserr);
} // jpwy_activatetransid


/******************************************************************************
 *
 * Syntax:
 *      jpwy_getinfo (JPWY_GETINFO *info)
 *
 * Description:
 *      Return configuration and statistical information about the module.
 *
 ******************************************************************************/

void jpwy_getinfo(JPWY_GETINFO *info) /* (out) information about the module. */
{
	assert(info != NULL);

	if (myinfo == NULL) {
		memset(info, 0, sizeof *info);
	} else {
		register short ndx, ctr;

		info->maxopens = myinfo->maxopens;
		info->maxrqsts = myinfo->recvdepth;
		info->numopens = (short)(_max(myinfo->numopens, 0));

		ctr = 0;
		for (ndx = 0; ndx < myinfo->recvdepth; ndx++)
			if (myinfo->rqstinfo [ndx].openx != -1)
				ctr++;
		info->numrqsts = ctr;
	}
} // jpwy_getinfo


/******************************************************************************
 *
 * Syntax:
 *      jpwy_msginfo (short msgtag, JPWY_MSGINFO *msginfo)
 *
 * Description:
 *      Retrieve the system information associated with a currently outstanding,
 *      received message.  The message is identified by the tag assigned to it
 *      when it was received.
 *
 *      The message tag is normally obtained from the rcvinfo.MessageTag field
 *      of the system information returned when the request is received.  A
 *      single-threaded server, however, never has more than one message out-
 *      standing at a time, so all of its message tags are zero.  Thus, single-
 *      threaded servers can always supply a message tag value of zero.
 *
 ******************************************************************************/

void jpwy_msginfo(short msgtag, /* (in)  identifies the message to retrieve. */
		JPWY_MSGINFO *msginfo, /* (out) system information associated with the
 specified message. */
		int pathSendMode) {

	ReceiveInfo *rcvInfo=&(msginfo->rcvinfo);

	assert(msginfo != NULL && myinfo != NULL);

	assert((unsigned) msgtag < myinfo->recvdepth && myinfo->rqstinfo [msgtag].openx
	                                                                  != -1);

	if (myinfo->rqstinfo [msgtag].openx >= 0) {
		OPENFILE *sender;
		RQSTINFO *request;

		assert((unsigned) myinfo->rqstinfo[msgtag].openx < myinfo->maxopens);
		sender = &myinfo->opens [myinfo->rqstinfo [msgtag].openx];
		request= &myinfo->rqstinfo[msgtag];
		assert((unsigned) sender->openerx < myinfo->maxopeners);

		SET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, (short)(sender->flags
				& RCVTYPE_OPEN), pathSendMode);
		SET_RECEIVEINFO_FIELD(rcvInfo, MaxReplyLen, (short)request->maxrplylen,
				pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, msgtag, pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, (long)(request->syncid),
				pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, request->openx,
				pathSendMode);
		if (sender->prifile != -1) {
			SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, sender->prifile,
					pathSendMode) ;
			memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
					myinfo->openers [sender->openerx].priproc,
					sizeof myinfo->openers->priproc);
		} else {
			SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, sender->bakfile,
					pathSendMode) ;
			memcpy(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode),
					myinfo->openers [sender->openerx].bakproc,
					sizeof myinfo->openers->bakproc);
		}
		if (pathSendMode
				==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
			msginfo->dlgflags = (short)((sender->flags & DLGFLAGS_OPEN) >> 4);
		else
			msginfo->rcvinfo.rcvInfo2.DialogInfo=(short)((sender->flags
					& DLGFLAGS_OPEN) >> 4);
	} else {
		SET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, SysMsg_ReceiveType,
				pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, MaxReplyLen, 0, pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, msgtag, pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, -1, pathSendMode) ;
		SET_RECEIVEINFO_FIELD(rcvInfo, SyncId, 0, pathSendMode) ;
		PROCESSHANDLE_NULLIT_(GET_RECEIVEINFO_FIELD(rcvInfo, PHandle,
				pathSendMode));
		SET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, -1, pathSendMode);
		if (pathSendMode
				==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
			msginfo->dlgflags = JPWY_NODIALOG;
		else
			msginfo->rcvinfo.rcvInfo2.DialogInfo=JPWY_NODIALOG;
	}
} // jpwy_msginfo


/******************************************************************************
 *
 * Syntax:
 *      jpwy_openinfo (short opentag, short *phandle, short *file)
 *
 * Description:
 *      Retrieve information about an open from the server's table of requester
 *      opens maintained by the module.  The open is identified by its open tag.
 *
 *      The opener's primary process handle and his primary open file number are
 *      returned via the phandle and file parameters.  If either of the param-
 *      eters are NULL pointers, the corresponding information is not returned.
 *
 *      The open's entry index can be obtained from the rcvinfo.OpenLabel field
 *      in the system information returned when a message sent under the open is
 *      received.
 *
 ******************************************************************************/

void jpwy_openinfo(short opentag, /* (in)  identifies the open to retrieve. */
		short *phandle, /* (out) the opener's primary process handle. */
		short *file) /* (out) the file number of the primary open. */
{
	OPENFILE *open;

	assert(myinfo != NULL);

	assert((unsigned) opentag < myinfo->maxopens);
	open = &myinfo->opens [opentag];
	assert((unsigned) open->openerx < myinfo->maxopeners);

	if (open->prifile != -1) {
		if (phandle != NULL)
			memcpy(phandle, myinfo->openers [open->openerx].priproc,
					sizeof myinfo->openers->priproc);
		if (file != NULL)
			*file = open->prifile;
	} else {
		if (phandle != NULL)
			memcpy(phandle, myinfo->openers [open->openerx].bakproc,
					sizeof myinfo->openers->bakproc);
		if (file != NULL)
			*file = open->bakfile;
	}
} // jpwy_openinfo


/******************************************************************************
 *
 * Syntax:
 *      jpwy_dropopen (short opentag)
 *
 * Description:
 *      Remove a connection from the server's table of requester connections
 *      maintained by the module.  The connection is identified by its open tag.
 *
 *      The module will reject any subsequent request messages sent via this
 *      connection to this server, as if the opener had never opened the server
 *      process.  The server will still receive any selected system messages
 *      sent via the connection.  However, the rcvinfo.OpenLabel field in the
 *      system information returned with these system messages will contain -1
 *      (this field normally identifies the connection under which the message
 *      was sent).
 *
 *      The open will not count toward the number of current connections to the
 *      server.  The module will indicate that all connections have been closed,
 *      and that the server should shut down, even if this connection is never
 *      closed.
 *
 *      The open's entry index can be obtained from the rcvinfo.OpenLabel field
 *      in the system information returned when a message sent under the open is
 *      received.
 *
 ******************************************************************************/
// This is not currently used by Java but may be useful in the future.
// It's a way for a server to ignore any further requests from an unruly
// requester but then couldn't the server just use FEWRONGID as a reply code.
void jpwy_dropopen(short opentag) /* (in)  identifies the open to be removed. */
{
	short rqstx=0;
	assert(myinfo != NULL);

	assert((unsigned) opentag < myinfo->maxopens);
	if (myinfo->opens [opentag].openerx == -1)
		return;

	if (myinfo->opens [opentag].firstrqstx != NORQST) {
	    myinfo->opens [opentag].flags |= DROPPED_OPEN;
		for(rqstx=myinfo->opens [opentag].firstrqstx;rqstx!=ENDOFREQ;rqstx=myinfo->rqstinfo[rqstx].nextrqstx)
			myinfo->rqstinfo[rqstx]= RQSTINIT;
	} else {
		open_free(&myinfo->opens [opentag]);
	}
} // jpwy_dropopen


/******************************************************************************
 *
 * Syntax:
 *      jpwy_print (FILE *stream)
 *
 * Description:
 *      Print information about the module's internal data structures to the
 *      specified file.  This function is intended for debugging purposes only.
 *
 * Results:
 *      The function returns the number of requests currently outstanding in
 *      the server.
 *
 ******************************************************************************/

int jpwy_print(FILE *stream) {
	short opnrx, opnx, rqstx,msgtag;
	OPENER *opener;
	OPENFILE *open;
	short used1, used2;
	int cnt = 0;

	fprintf(stream, "  jpwy Module Information:\n");

	if (myinfo == NULL) {
		fprintf(stream, "  jpwy module uninitialized\n");
		return (0);
	}
	fprintf(stream, "  jpwy module initialized:\n");
	fprintf(stream, "      myinfo->recvdepth   -     %d\n", myinfo->recvdepth);
	fprintf(stream, "      myinfo->smm [0]     -     0x%08lX\n",
			myinfo->smm [0]);
	fprintf(stream, "      myinfo->smm [1]     -     0x%08lX\n",
			myinfo->smm [1]);
	if (myinfo->recvdepth != 0) {
		fprintf(stream, "      myinfo->syncdepth   -     %hd\n",
				myinfo->syncdepth);
		fprintf(stream, "      myinfo->maxrplylen  -     %d\n",
				myinfo->maxrplylen);
	}

	for (used1 = 0, opnrx = 0; opnrx < myinfo->maxopeners; opnrx++)
		if (myinfo->openers [opnrx].frstopnx != -1)
			used1++;

	fprintf(stream, "      opener entries:\n");
	fprintf(stream, "        allocated         -     %hd\n", myinfo->maxopeners);
	fprintf(stream, "        in use            -     %hd\n", used1);

	if (used1 != 0) {
		for (opnrx = 0, opener = myinfo->openers; opnrx < myinfo->maxopeners;
		opnrx++, opener++) {
			char procname [MAXPROCNAMELEN + 1];
			short fserr, fslen;

			if (opener->frstopnx == -1)
				continue;

			for (used1 = used2 = 0, opnx = opener->frstopnx; opnx != -1; opnx
			= myinfo->opens [opnx].nextopnx) {
				if (myinfo->opens [opnx].prifile != -1)
					used1++;
				if (myinfo->opens [opnx].bakfile != -1)
					used2++;
			}

			fserr = PROCESSHANDLE_TO_STRING_(opener->priproc, procname,
					sizeof procname, &fslen, OMITREF, OMITSHORT, OMITSHORT);
			if (fserr != FEOK)
				sprintf(procname, "(error %hd)", fserr);
			else
				procname [fslen] = '\0';
			fprintf(stream, "        entry[%hd]:\n");
			fprintf(stream, "          procname                  -     %s\n",
					procname);
			fprintf(stream, "          number of opens           -     %hd\n",
					used1);

			if (used2 != 0) {
				fserr = PROCESSHANDLE_TO_STRING_(opener->bakproc, procname,
						sizeof procname, &fslen, OMITREF, OMITSHORT, OMITSHORT);
				if (fserr != FEOK)
					sprintf(procname, "(error %hd)", fserr);
				else
					procname [fslen] = '\0';
				fprintf(stream,
						"          backup                    -     %s\n",
						procname);
				fprintf(stream,
						"          number of opens           -     %hd\n",
						used2);

			}
		}
	}

	for (used1 = opnx = 0; opnx < myinfo->maxopens; opnx++)
		if (myinfo->opens [opnx].openerx != -1)
			used1++;

	fprintf(stream, "      open entries:\n");
	fprintf(stream, "        allocated         -     %hd\n", myinfo->maxopeners);
	fprintf(stream, "        in use            -     %hd\n", used1);

	if (used1 != 0) {
		for (opnx = 0, open = myinfo->opens; opnx < myinfo->maxopens; opnx++,
		open++) {
			if (open->openerx == -1)
				continue;

			fprintf(stream, "        [%hd] opener [%hd]:\n", opnx,
					open->openerx);

			if (open->bakfile == -1 || open->bakfile == open->prifile)
				fprintf(stream,
						"          open file                 -     %hd\n",
						open->prifile);
			else {
				fprintf(stream,
						"          primary file              -     %hd\n",
						open->prifile);
				fprintf(stream,
						"          backup file               -     %hd\n",
						open->bakfile);
			}
			fprintf(stream, "          access                    -     %hd\n",
					(short) ((open->openflags & 017) >> 6));
			fprintf(stream, "          exclusion                 -     %hd\n",
					(short) ((open->openflags & 003) >> 4));
			fprintf(stream, "          sync depth                -     %hd\n",
					(short) ((open->openflags & 077) >> 10));
			fprintf(stream, "          nowait depth              -     %hd\n",
					(short) (open->openflags & 017));
			/*fprintf(stream, "          sync id                   -     %lu\n",
					open->syncid);*/
			fprintf(stream, "          open  flags               -     %d\n",
					open->flags);
			if (open->flags & OPENING_OPEN)
				fprintf(stream,
						"                                 OPENING_OPEN\n");
			if (open->flags & DLGABORT_OPEN)
				fprintf(stream,
						"                                 DLGABORT_OPEN\n");
			if (open->flags & CLOSING_OPEN)
				fprintf(stream,
						"                                 CLOSING_OPEN\n");
			if (open->flags & DROPPED_OPEN)
				fprintf(stream,
						"                                 DROPPED_OPEN\n");
			for (msgtag=-1,rqstx = 0; rqstx < myinfo->recvdepth; rqstx++)
					if (myinfo->rqstinfo[rqstx].openx == opnx) {
						msgtag = rqstx;
						break;
					}
			if (myinfo->rqstinfo[rqstx].gmsgtag != -1) {

				
				fprintf(stream,
						"          current message           -     [%hd]\n",
						msgtag);

				if (myinfo->rqstinfo[rqstx].gmsgtag == -2)
					fprintf(stream, "         synthesized\n");
				else {
					fprintf(stream,
							"          reply tag                 -     %hd\n",
							myinfo->rqstinfo[rqstx].gmsgtag);
					fprintf(stream, "          sync id                   -     %lu\n",
					myinfo->rqstinfo[rqstx].syncid);
					fprintf(stream, "          maxreplylen                  -     %lu\n",
					myinfo->rqstinfo[rqstx].maxrplylen);
				}
			}
		}
	}

	for (used1 = rqstx = 0; rqstx < myinfo->recvdepth; rqstx++)
		if (myinfo->rqstinfo[rqstx].openx != -1)
			used1++;

	fprintf(stream, "      message entries:\n");
	fprintf(stream, "        allocated         -     %hd\n", myinfo->recvdepth);
	fprintf(stream, "        in use            -     %hd\n", used1);

	cnt = used1;

	if (used1 != 0) {
		for (rqstx = 0; rqstx < myinfo->recvdepth; rqstx++) {
			if (myinfo->rqstinfo[rqstx].openx == -1)
				continue;

			if (myinfo->rqstinfo[rqstx].openx >= 0) {
				open = &myinfo->opens [myinfo->rqstinfo[rqstx].openx];

				fprintf(stream, "        [%hd] message from open [%hd]:\n",
						rqstx, myinfo->rqstinfo[rqstx].openx);
				fprintf(stream, "          reply tag         -     %hd\n",
						myinfo->rqstinfo[rqstx].gmsgtag);

				fprintf(stream, "          receive type      -     %hd\n",
						(short) (open->flags & RCVTYPE_OPEN));

				fprintf(stream, "          maximum reply len -     %d\n",
						myinfo->rqstinfo[rqstx].maxrplylen);

				if (open->flags & DLGFLAGS_OPEN)
					fprintf(stream,
							"          dialog flags      -     0x%04hX\n",
							(short) ((open->flags & DLGFLAGS_OPEN) >> 4));

			} else {
				fprintf(stream, "        [%hd] system message:\n", rqstx);
				fprintf(stream, "          reply tag         -     %hd\n",
						(short) (-myinfo->rqstinfo[rqstx].openx - 2));
			}
		}
	}

	return (cnt); 
} // jpwy_print

/******************************************************************************
 *
 * Syntax:
 *      initialise_functionptr()
 *
 * Description:
 *      The function is used to initialise function pointers for spt_INITRECEIVEL,
 * 		spt_RECEIVEREADL and spt_REPLYXL fuctions.
 *
 ******************************************************************************/
void initialise_functionptr() {
	static jboolean bInit = FALSE;
	if (bInit == TRUE)
		return;

	dlopenhandle = dlopen(NULL, RTLD_NOW);
	assert(dlopenhandle);

	spt_initreceivel = (SPT_INITRECEIVEL) dlsym(dlopenhandle,
			"spt_INITRECEIVEL");
	spt_receivereadl=(SPT_RECEIVEREADL) dlsym(dlopenhandle, "spt_RECEIVEREADL");
	spt_replyxl=(SPT_REPLYXL) dlsym(dlopenhandle, "spt_REPLYXL");

	spt_serverclass_sendl_=(spt_SERVERCLASS_SENDL_)dlsym(dlopenhandle,
			"SPT_SERVERCLASS_SENDL_");
	

	spt_serverclass_dialog_beginl_=(spt_SERVERCLASS_DIALOG_BEGINL_)dlsym(dlopenhandle,
			"SPT_SERVERCLASS_DIALOG_BEGINL_");

	spt_serverclass_dialog_sendl_=(spt_SERVERCLASS_DIALOG_SENDL_)dlsym(dlopenhandle,
			"SPT_SERVERCLASS_DIALOG_SENDL_");

	bInit = TRUE;
}

