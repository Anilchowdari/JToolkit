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
#include "com_tandem_ext_guardian_Receive.h" 
#include "com_tandem_ext_util_PathsendModeType.h"
#include <spthread.h>
#include <stdlib.h>
#include <cextdecs.h(CANCEL, FILE_GETINFO_, FILE_GETRECEIVEINFO_, PROCESSHANDLE_NULLIT_, SETMODE)>
#include <tal.h>
#include "tmrqst.h"
// asserts can be turned off for non-debug builds by using -DNDEBUG on the cmd line
#include <assert.h>
#include <stdio.h>
#include "functionpointers.h"
#include "Receive.h"
#include <errno.h>
/*
 * This integer value tracks, whether there is any outstanding read on $RECEIVE.
 * If there is no read outstanding this value will be 0. This allows us
 * to issue an error when the user issues an cancel when there is no read
 * outstanding.
 */
static short OutstandingRead = 0;

void jRcv_throwOutOfMemoryError(JNIEnv *env, const char *msg) {
	jclass clazz = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
	if (clazz == 0)
		return;
	(*env)->ThrowNew(env, clazz, msg);
	return;
} // jRcv_throwOutOfMemoryError

/***********************************************************************************
 *
 * Syntax:
 *      jRcv_throwUnsatisfiedLinkError(JNIEnv *env, const char *msg)
 *
 * Description:
 *      The function is used to throw UnsatisfiedLinkError Exception from native code
 * 		
 *************************************************************************************/
extern void jRcv_throwUnsatisfiedLinkError(JNIEnv *env, const char *msg) {
	jclass clazz = (*env)->FindClass(env, "java/lang/UnsatisfiedLinkError");
	if (clazz == 0)
		return;
	(*env)->ThrowNew(env, clazz, msg);
	return;
} // jRcv_throwUnsatisfiedLinkError


/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    cancel
 * Signature: ()I
 *
 * Cancel outstanding readupdate.  Assumes Java side has already made sure
 * $RECEIVE is open.  Wakes up thread waiting for readupdate to complete.
 * Returns file system error number.
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_cancel(
		JNIEnv *env,
		jobject rcvFile)
{
	spt_error_t spte;
	short fnum = 0;

	if (OutstandingRead == 0) return FENONEOUT; // there is no outstanding read

	// Interrupt the spt_RECEIVEREAD which will cancel the I/O

	spte = spt_interrupt(fnum,SPT_ERROR);

	if(spte != SPT_SUCCESS)
	return FENONEOUT;

	return FEOK;

} // Java_com_tandem_ext_guardian_Receive_cancel

/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    closeReceive
 * Signature: ()I
 *
 * Close $RECEIVE.  If there is an outstanding call to readupdate the I/O is
 * cancelled and the thread woken up because if $RECEIVE is closed while 
 * readupdate is outstanding the file system automatically cancels the 
 * outstanding I/O which won't wake up the thread.  To make sure the thread
 * doing the I/O is woken up we do the cancel first ourselves.

 * Returns: file system error number
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_closeReceive(
		JNIEnv *env,
		jobject rcvFile,
		jint pathSendMode)
{
	short filenum = 0;
	int error = FEOK;
	if (OutstandingRead != 0) {
		error = Java_com_tandem_ext_guardian_Receive_cancel(env, rcvFile);
		if (error != FEOK) return error;
	}

	error = jpwy_close(pathSendMode);
	spt_unregFile(filenum);
	return error;
} /* Java_com_tandem_ext_guardian_Receive_closeReceive */

/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    openReceive
 * Signature: (SSSSSJ)I
 *
 * Open $RECEIVE.  Java code should ensure that $Receive is not already open.
 * Returns: file system error number
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_openReceive(
		JNIEnv *env,
		jobject rcvFile,
		jshort jinitOpens,
		jshort jreceiveDepth,
		jshort jsyncDepth,
		jint jmaxReplyLen,
		jshort joptions,
		jlong jsysmsgMask,
		jint pathSendMode
)
{

	jint error;
	short fnum = 0;
	long regError;
	union smm_t {
		long long whole_smm;
		struct smm_parts_t {
			unsigned long smm2;
			unsigned long smm1;
		}smm_parts;
	}smm;

	smm.whole_smm = jsysmsgMask;

	/*changes begin for tsmp2.4*/

	if(pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_2MB_LIMIT)
	{
		initialise_functionptr();
		if (!spt_initreceivel)
		{
			jRcv_throwUnsatisfiedLinkError(env, "Pthreads version mismatch. Unable to find symbol : spt_INITRECEIVEL");
			return;
		}

	}

	/*changes end for tsmp2.4*/

	error = jpwy_open(jinitOpens, jreceiveDepth, jsyncDepth, jmaxReplyLen,
			joptions, smm.smm_parts.smm1, smm.smm_parts.smm2);
	if(error != FEOK)
	{
		return error;
	}

	/* register with Pthreads that we have $RECEIVE open */
	if (pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
	regError = spt_INITRECEIVE(fnum,jreceiveDepth);// registration for narrow receive
	else
	regError = spt_initreceivel(fnum,jreceiveDepth);//registration for wide receive
	//changes begin for sol: 10-090508-1449
	if (regError==EVERSION)
	{
		jRcv_throwUnsatisfiedLinkError(env, "FileSystem error 4184: version mismatch.");
		return;
	}
	//changes end for sol: 10-090508-1449
	// only possible error is another receive handler is already registered
	if(regError != FEOK) return FEINUSE;

	return regError;

} /* Java_com_tandem_ext_guardian_Receive_openReceive */

/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    read
 * Signature: ([BI[I)I
 *
 * Performs a READUPDATEX on $RECEIVE.  If message read is a system message
 * the user didn't ask for it will post another read until it gets something
 * the user wants.  If read is successful a ReceiveInfo object is created.
 * Returns file system error so Java code can interpret it
 * and throw appropriate exception.
 * Java side does checks to make sure $RECEIVE is open and buffer and read count
 * are valid.
 * Returns: file system error number
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_read (
		JNIEnv *env,
		jobject rcvFile,
		jbyteArray jbuffer,
		jint readCount,
		jintArray jcountRead,
		jboolean notifyNoOpeners,
		jint pathSendMode)
{
	int error = FEOK;
	// minimum buffer size needs to be big enough to hold a system message
	char minMsg[250];
	// buffer ptr we're using to do the read, may or may not point to users buffer
	// depending upon whether it's big enough to hold the max size system message
	char *useBuf = NULL;
	int useBuflen = 0; // len of array pointed to by useBuf
	JPWY_MSGINFO msgInfo; // holds receive info set by callback 
	ReceiveInfo *rcvInfo=&(msgInfo.rcvinfo);
	jint buflen = 0; // to hold size of users buffer
	jbyte *buffer = NULL; // points to body (i.e. read data) of jbuffer
	int countTransferred = 0;
	jclass rcvClass = 0; // hold reference to Receive class 
	jshortArray jPhandleArray = NULL; // holds newly created array of shorts to hold the requesters phandle
	jclass clazz = 0; // holds reference to ReceiveInfo class
	jmethodID mid = 0; // holds reference to ReceiveInfo constructor
	jobject ri = 0; // holds reference to ReceiveInfo object that is created
	jfieldID lastRcvInfo_fieldID; // holds reference to lastRecvInfo_ field

	// This can be set to true using inspect when debugging and want some debugging
	// info printed out.
	static int jrcvDebug = 0;

	// This is just for debugging in Inspect which is currently the only way
	// to set this var to true.
	if (jrcvDebug) {
		int numRqsts = 0;
		fprintf(stderr, "Just before calling ReadUpdate here is the state of the opener table\n");
		numRqsts = jpwy_print(stderr);
		fprintf(stderr, "The total number of outstanding requests is %d\n", numRqsts);
	}

	// We don't have to check things like is the buffer null or is buflen 0
	// because that checking is done in the Java code.
	buflen = (*env)->GetArrayLength(env, jbuffer);
	// This GetByteArrayElements must have a matching release call
	buffer = (*env)->GetByteArrayElements(env, jbuffer, NULL);

	if (buflen < 250 /* minimum size needed for system messages */) {
		useBuf = minMsg;
		useBuflen = 250;
	} else {
		useBuf = (char *)buffer;
		useBuflen = buflen;
	}

	do { /* read until we get msg user wants or get error */

		SET_RECEIVEINFO_FIELD(rcvInfo,ReceiveType,0,pathSendMode);
		SET_RECEIVEINFO_FIELD(rcvInfo,MaxReplyLen,0,pathSendMode);
		SET_RECEIVEINFO_FIELD(rcvInfo,MessageTag,0,pathSendMode);
		SET_RECEIVEINFO_FIELD(rcvInfo,FileNumber,0,pathSendMode);
		SET_RECEIVEINFO_FIELD(rcvInfo,SyncId,0,pathSendMode);
		PROCESSHANDLE_NULLIT_ (GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode));
		SET_RECEIVEINFO_FIELD(rcvInfo,OpenLabel,0,pathSendMode);
		if (pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
		msgInfo.dlgflags = 0;
		else
		msgInfo.rcvinfo.rcvInfo2.DialogInfo=0;

		// Have tmrqst module initiate the readupdate
		OutstandingRead = 1;
		error = jpwy_initrecv(useBuf, useBuflen, &countTransferred,&msgInfo,pathSendMode);
		OutstandingRead = 0;
		
		if (error == FEEOF && !notifyNoOpeners) {
			// user doesn't want to be notified when there are no more openers
			error = FECONTINUE;
			continue;
		}

		if (error != FEOK && error != FESYSMESS) {
			(*env)->ReleaseByteArrayElements(env, jbuffer, buffer, JNI_ABORT);
			return error;
		}

		// tell tmrqst module about the completion and let it interpret the msg
		error = jpwy_comprecv(useBuf, &countTransferred, &msgInfo,pathSendMode);
		if (error == FEEOF && !notifyNoOpeners) {
			// user doesn't want to be notified when there are no more openers
			error = FECONTINUE;
			continue;
		}

	}while (error == FECONTINUE);

	if (error != FEOK && error != FESYSMESS) {
		(*env)->ReleaseByteArrayElements(env, jbuffer, buffer, JNI_ABORT);
		return error;
	}

	// At this point we know readupdate completed successfully and we have a
	// message the user is interested in.
		if (useBuf == minMsg) {
		/*Changes begin for 10-080226-0790 */
		if ( ((unsigned)countTransferred> (unsigned)buflen) && (error==FESYSMESS) ) {
			//user interested system message
			// user received a system message they requested but they didn't allocate
			// enough space for it,  reply for the user and throw exception
			jclass clzz = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
			if (clzz == 0)
			return -1;
			(*env)->ReleaseByteArrayElements(env, jbuffer, buffer, JNI_ABORT);
			(*env)->ThrowNew(env, clzz, "buffer too small to hold requested system message");
			return error;
		}
		else if ( ((unsigned)countTransferred> (unsigned)buflen) && (error==FEOK) ){
		//user interested request message which will be truncated with in the length of the buffer declared by the user
		memcpy(buffer, useBuf, (unsigned)buflen);
		/*Changes end for 10-080226-0790 */
	} else {
		// move msg to users buffer
		//user interested system message or request message whose length is less than the length of the buffer declared by the user
		memcpy(buffer, useBuf, (unsigned)countTransferred);
	}
}

	// we're done with the user's buffer for good
	(*env)->ReleaseByteArrayElements(env, jbuffer, buffer, 0);

	// Create a Java array of shorts to hold the requesters Phandle
	jPhandleArray = (*env)->NewShortArray(env, 10);
	if (jPhandleArray == NULL) {
		return -1; // Java exception already created
	}
	(*env)->SetShortArrayRegion(env, jPhandleArray, 0/*index*/, 10/*len*/,
			GET_RECEIVEINFO_FIELD(rcvInfo, PHandle, pathSendMode));
	if ((*env)->ExceptionOccurred(env) != NULL) {
		return -1;
	}

	// Construct a ReceiveInfo object
	clazz = (*env)->FindClass(env, "com/tandem/ext/guardian/ReceiveInfo");
	if (clazz == 0) {
		return -1; // Java exception already created
	}
	mid = (*env)->GetMethodID(env, clazz, "<init>", "(SISSI[SSS)V");
	if (mid == 0) {
		return -1; // Java exception already created
	}
	if (pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
	ri = (*env)->NewObject(env, clazz, mid,
			GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, pathSendMode),
			(unsigned short)GET_RECEIVEINFO_FIELD(rcvInfo, MaxReplyLen, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, SyncId, pathSendMode),
			jPhandleArray,
			GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode),
			msgInfo.dlgflags);
	else

	ri = (*env)->NewObject(env, clazz, mid,
			GET_RECEIVEINFO_FIELD(rcvInfo, ReceiveType, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, MaxReplyLen, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, MessageTag, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, FileNumber, pathSendMode),
			GET_RECEIVEINFO_FIELD(rcvInfo, SyncId, pathSendMode),
			jPhandleArray,
			GET_RECEIVEINFO_FIELD(rcvInfo, OpenLabel, pathSendMode),
			msgInfo.rcvinfo.rcvInfo2.DialogInfo);

	if (ri == 0) return -1; // Java exception already created

	//set the lastRecvInfo_ field to reflect the new ReceiveInfo
	rcvClass = (*env)->GetObjectClass(env, rcvFile);
	lastRcvInfo_fieldID = (*env)->GetFieldID(env, rcvClass, "lastRecvInfo_",
			"Lcom/tandem/ext/guardian/ReceiveInfo;");
	if (lastRcvInfo_fieldID == NULL) return -1; //Exception already created
	(*env)->SetObjectField(env, rcvFile, lastRcvInfo_fieldID, ri);

	/*Changes begin for 10-080226-0790 */
if (useBuf == minMsg && (unsigned)countTransferred> (unsigned)buflen)
(*env)->SetIntArrayRegion(env, jcountRead, 0 /*offset*/, 1 /*len*/, (long *)&buflen);
/*Changes end for 10-080226-0790 */
else
(*env)->SetIntArrayRegion(env, jcountRead, 0 /*offset*/, 1 /*len*/, (long *)&countTransferred);


	return error;

} // Java_com_tandem_ext_guardian_Receive_read


/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    reply
 * Signature: ([BISS[II)I
 *
 * Replies to message previously read from $RECEIVE.  Java side does
 * checks to verify $RECEIVE is open and reply count is valid and buffer
 * is not null if replycount > 0 etc.
 * Returns: file system error
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_reply(
		JNIEnv *env,
		jobject rcvFile,
		jbyteArray jbuffer,
		jint replyCount,
		jshort errorReturn,
		jshort msgTag,
		jintArray jcountWritten,
		jint pathSendMode)
{
	int error = FEOK;
	char *useBuf = NULL;
	long countWritten = 0;
	long returnCount = 0;
	if (replyCount> 0) {
		// We don't have to check if the buffer = null
		// because that checking is done in the Java code.
		// Get the data the user wants to reply with. This must have a
		// matching ReleaseByteArrayElements call.
		useBuf = (jbyte *)(*env)->GetByteArrayElements(env, jbuffer, NULL);
		if (useBuf == NULL) return 0; // exception already created
	}

	error = jpwy_reply(useBuf,replyCount, errorReturn, msgTag,&countWritten,pathSendMode);
	
	if (error == FEOK) {
		// make sure a negative countWritten value comes out positive when put 
		// in jcountWritten because countWritten can be as much as 57344
		returnCount = countWritten;
		(*env)->SetIntArrayRegion(env, jcountWritten, 0 /*offset*/, 1 /*len*/, &returnCount);
	}

	// At this point the server is no longer participating in any transaction
	// it might have inherited from the requester.  Unfortunately, we don't know
	// that the current thread's transaction context is set to the transaction
	// it inherited so we can't null it out.  And pthreads provides no mechanism
	// for finding all threads who have participated in a certain transaction
	// and nulling out all the txhandle's of those threads.  It means the txhandle
	// of the tcb can contain stale data but it doesn't seem to have a deleterious
	// effect on Java and pthreads.

	if (replyCount> 0) {
		(*env)->ReleaseByteArrayElements(env, jbuffer, (jbyte *)useBuf, 0);
	}

	return error;
} // Java_com_tandem_ext_guardian_Receive_reply

/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    setPriorityQueue
 * Signature: (S)I
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_Receive_setPriorityQueue (
		JNIEnv *env, jobject obj, jshort param1) {

	short fserr = FEOK;
	short fierr = FEOK;

	// it's not necessary to call the nowait setmode because $receive shouldn't block
	if (_status_lt (SETMODE (0, 36, param1))) {
		fierr = FILE_GETINFO_(0, &fserr);
		if (fierr != FEOK) fserr = fierr;
	}

	return fserr;

} // Java_com_tandem_ext_guardian_Receive_setPriorityQueue

/*
 * Class:     com_tandem_ext_guardian_Receive
 * Method:    setSysMsgMask
 * Signature: (J)V
 *
 * Sets/resets the system message mask.  The Java code is responsible for 
 * making sure $RECEIVE is open before calling this procedure.
 * Returns: nothing
 */
JNIEXPORT void JNICALL Java_com_tandem_ext_guardian_Receive_setSysMsgMask (
		JNIEnv *env, jobject obj, jlong jsysmsgMask) {
	union smm_t {
		long long whole_smm;
		struct smm_parts_t {
			unsigned long smm2;
			unsigned long smm1;
		}smm_parts;
	}smm;

	smm.whole_smm = jsysmsgMask;
	jpwy_setsmm(RESET_SMM1, smm.smm_parts.smm1);
	jpwy_setsmm(RESET_SMM2, smm.smm_parts.smm2);
} // end Java_com_tandem_ext_guardian_Receive_setSysMsgMask

