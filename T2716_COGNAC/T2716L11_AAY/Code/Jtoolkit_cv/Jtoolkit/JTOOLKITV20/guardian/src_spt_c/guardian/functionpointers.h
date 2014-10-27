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
/* Changes begin for TSMP Version Greater then 2.3 */
#ifndef FUNCTIONPOINTERS_H
#define FUNCTIONPOINTERS_H

#include <dlfcn.h>

dlHandle dlopenhandle;

/*****************************************************************************/
/* Function pointer definition for SPT_INITRECEIVEL
*/
typedef  long (*SPT_INITRECEIVEL)(const short filenum,
		const short receive_depth);

/* Global pointer to function SPT_INITRECEIVEL*/
SPT_INITRECEIVEL spt_initreceivel;

/*****************************************************************************/
/* Function pointer definition for SPT_RECEIVEREADL
*/
typedef  long (*SPT_RECEIVEREADL)(const short filenum,
		char *buffer,
		const long read_count,
		long *count_read,
		const long timelimit,
		short *receive_info2);

/* Global pointer to function SPT_RECEIVEREADL*/
SPT_RECEIVEREADL spt_receivereadl;

/*****************************************************************************/
/* Function pointer definition for SPT_REPLYXL
*/

//Begin changes for sol:10-090425-1140

typedef  long (*SPT_REPLYXL)(const char *buffer,
		const long write_count,
		long *count_written,
		const short msg_tag,
		const short error_return);
//End changes for sol:10-090425-1140
/* Global pointer to function SPT_REPLYXL*/

SPT_REPLYXL	  spt_replyxl;

/*****************************************************************************/
/* Function pointer definition for spt_SERVERCLASS_SENDL_*/
typedef _tal _extensible short (_far *spt_SERVERCLASS_SENDL_) (
		char * pathmon, /* in  */
		short pathmonbytes, /* in  */
		char * serverclass, /* in  */
		char serverclassbytes, /* in  */
		char * writebufferL, /* out  */
		char * readbufferL, /* in  */
		long requestbytes, /* in  */
		long maximumreplybytes, /* in  */
		long * actualreplybytes, /* out optional  */
		long timeout, /* in optional  */
		short flags, /* in optional  */
		short * scsoperationnumber, /* out optional  */
		long long tag /* in optional  */
);

//Global pointer to function spt_SERVERCLASS_SENDL_
spt_SERVERCLASS_SENDL_ spt_serverclass_sendl_;

/*****************************************************************************/
/* Function pointer definition for spt_SERVERCLASS_DIALOG_BEGINL_*/
typedef _tal _extensible short (_far *spt_SERVERCLASS_DIALOG_BEGINL_)(
		long *  dialogid, /* out */
		char *  pathmon, /* in  */
		short   pathmonbytes, /* in  */
		char *  serverclass, /* in  */
		short   serverclassbytes, /* in  */
		char *  writebufferL, /* in  */
		char *  readbufferL, /* out optional  */
		long    requestbytes, /* in  */
		long    maximumreplybytes,	/* in  */
		long *  actualreplybytes, /* out optional  */
		long    timeout, /* in optional  */
		short   flags, /* in optional  */
		short * scsoperationnumber, /* out optional  */
		long long    tag /* in optional  */
);

//Global pointers to function spt_SERVERCLASS_DIALOG_BEGINL_
spt_SERVERCLASS_DIALOG_BEGINL_ spt_serverclass_dialog_beginl_;

/*****************************************************************************/
/* Function pointer definition for spt_SERVERCLASS_DIALOG_SENDL_*/
typedef _tal _extensible short (_far *spt_SERVERCLASS_DIALOG_SENDL_)(
		long    dialogid,           	/* in  */
		char *  writebufferL,      		/* in  */
		char *  readbufferL,        	/* out optional  */
		long    requestbytes,       	/* in  */
		long    maximumreplybytes,  	/* in  */
		long *  actualreplybytes,   	/* out optional  */
		long    timeout,            	/* in optional  */
		short   flags,              	/* in optional  */
		short * scsoperationnumber, 	/* out optional  */
		long long    tag                 /* in optional  */
);

//Global pointers to function spt_SERVERCLASS_DIALOG_SENDL_
spt_SERVERCLASS_DIALOG_SENDL_ spt_serverclass_dialog_sendl_;

#endif  //FUNCTIONPOINTERS_H
