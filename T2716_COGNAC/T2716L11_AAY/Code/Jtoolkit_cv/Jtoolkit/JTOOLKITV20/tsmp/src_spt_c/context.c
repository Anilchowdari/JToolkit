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

#include <spthread.h>
#include <string.h>
#include <stdio.h>
#include "com_tandem_tsmp_TsmpDialog.h"
#include "com_tandem_ext_util_PathsendModeType.h"
#include  "functionpointers.h"
#include "Receive.h"

#define NOWAITIO 0            /* Pthreads routine only accepts 0 for this */
#define TM_ANY_TRANSACTION_PER_DIALOG 1 /* Transaction Model value for any number                                           r of transaction per dialog */
#define ERROR_ARRAY_SIZE 2

/*
* Performs USERIOBUFFER_ALLOW_() GPC for allowing more than 7 concurrent 2MB pathsends on NSAA systems
*/
JNIEXPORT void JNICALL Java_com_tandem_tsmp_TsmpDialogWide_allowUserBuffers
(JNIEnv *env, jobject obj)
{
    USERIOBUFFER_ALLOW_();
}
/*
 * Class:     com_tandem_tsmp_TsmpDialog
 * Method:    scDialogBegin
 * Signature: ([ILjava/lang/String;Ljava/lang/String;[BI[BI[III)[I
 *
 * Functional Description:
 *
 * JNI native method for TsmpDialog which makes call to SPT_SERVERCLASS_DIALOG_BEGIN_ of pthread.
 *
 * Formal Paramaters:
 *
 *  env, The Java environment pointer                                   input
 *  obj, Java object representing TsmpDialog object                     input
 *  jdlgId, Integer Array representing dialog Id                        output
 *  pmonName, String representing the Pathmon process name              input
 *  scName, String representing the Pathmon serverclass name            input
 *  request, Byte Array representing the request buffer                 input
 *  requestlength, Integer representing the request length              input
 *  reply, Byte Array representing the reply buffer                     output
 *  maxReplyLen, Integer representing the maximum reply length          input
 *  jActualReplyLen, Integer Array representing the actual reply length output
 *  timeout, Integer representing the timeout value                     input
 *  transModel, Integer representing dialog transaction model value     input
 *	pathSendMode, Integer representing the pathsend mode type           input
 *
 * Implicit Inputs:
 *
 *  None.
 *
 * Implicit Outputs:
 *
 *  None.
 *
 * Return Value:
 *
 * pathsend and file system errors.
 *
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpDialog_scDialogBegin(JNIEnv *env, jobject obj,
		jintArray jdlgId, jstring pmonName, jstring scName, jbyteArray request,
		jint requestlength, jbyteArray reply, jint maxReplyLen,
		jintArray jActualReplyLen, jint timeout, jint transModel, jint pathSendMode) {

	jintArray errorArray;
	short actualReplyLen = 0;
	// Begin of Changes for Solution Number 10-060117-3897

	jint actualLen = 0;

	// End of Changes for Solution Number 10-060117-3897
	long dlgId=-1;
	short pwerror = 0;
	short fserror = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};
	short flags = 0;
	const char *pmonname = (*env)->GetStringUTFChars(env, pmonName, 0);
	const char *scname = (*env)->GetStringUTFChars(env, scName, 0);
	jbyte *reqbuf = (*env)->GetByteArrayElements(env, request, 0);
	jbyte *msgbuf = NULL;
	if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
	{
		//allocate memory large enough to hold max of request and reply
		if(maxReplyLen >= requestlength)
			/*
			changed malloc to malloc_pages for getting page aligned address since User Buffer memory is used instead of PFS memory 
			*/
			msgbuf = (jbyte *) malloc_pages(maxReplyLen);
		else
			msgbuf = (jbyte *) malloc_pages(requestlength);

		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
			memcpy(msgbuf, reqbuf, requestlength);

		/* set the flags value corresponding to the dialog transaction model value */
		if(transModel == TM_ANY_TRANSACTION_PER_DIALOG)
			flags = (short)(flags | 2);
		/*
		 Make the call to the pthreads version of SERVERCLASS_DIALOG_BEGIN_.
		 */
		fserror = SPT_SERVERCLASS_DIALOG_BEGIN_ (&dlgId,
			(char *)pmonname,
			(short)strlen(pmonname),
			(char *)scname,
			(short)strlen(scname),
			(char *)msgbuf,
			(short)requestlength,
			(short)maxReplyLen,
			&actualReplyLen,
			(long)timeout,
			flags);

		if (fserror == 0 || fserror == 70 ) {
			//Begin changes for Rout RFE sol:10-081125-7556
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
			if (pwerror ==0 && (fserror==0 || fserror==70))
				//End changes for Rout RFE sol:10-081125-7556
				if (actualReplyLen> 0)
				// Put reply data into reply byte array
				(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLen, msgbuf);
		}
		else
		// send failed, get pathsend and file system errors
		SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);

		// set the actual reply bytes so the Java side can see it
		// Begin of Change for Solution Number 10-060117-3897
		/*  (*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, (jint *)&actualReplyLen); */
		actualLen = (jint)actualReplyLen;
		(*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, &actualLen);
		// End of Changes for  Solution Number 10-060117-3897
		// set the dialog id
		(*env)->SetIntArrayRegion(env, jdlgId, 0, 1, (jint *)&dlgId);
		/*
		 * Fill in the error buffer
		 */
		errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
		errorbuf[0] = (int)pwerror;
		errorbuf[1] = (int)fserror;
		(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);
		/*
		 * Clean up the Java strings and arrays.
		 */
		(*env)->ReleaseStringUTFChars(env, pmonName, pmonname);
		(*env)->ReleaseStringUTFChars(env, scName, scname);
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);

		free(msgbuf);
	}

	else if(pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_2MB_LIMIT){

		int actualReplyLenW = 0; // For large path sends
		jbyte *readbuf = NULL;
		
		//Check to ensure that the SUT contains the appropriate Pthread DLL for TSMP 2.5 support
		initialise_functionptr();
		if (!spt_serverclass_dialog_beginl_)
		{
			jRcv_throwUnsatisfiedLinkError(env, "Pthreads version mismatch. Unable to find symbol : SPT_SERVERCLASS_DIALOG_BEGINL_/SPT_SERVERCLASS_DIALOG_SENDL_\n");
			return;
		}		
		/* Using malloc_pages instead of malloc for userbuffer memory */
		readbuf = (jbyte *) malloc_pages(maxReplyLen);
		if(!readbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		msgbuf = (jbyte *) malloc_pages(requestlength);
		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		memcpy(msgbuf, reqbuf, requestlength);
		/* set the flags value corresponding to the dialog transaction model value */
		if(transModel == TM_ANY_TRANSACTION_PER_DIALOG)
				flags = (short)(flags | 2);

		/* Make the call to the pthreads version of SERVERCLASS_DIALOG_BEGINL_ for
		 * 2MB support
		 */
		fserror = spt_serverclass_dialog_beginl_(&dlgId,
				(char *)pmonname,
				(short)strlen(pmonname),
				(char *)scname,
				(short)strlen(scname),
				(char *)msgbuf,
				(char *)readbuf,
				(long)requestlength,
				(long)maxReplyLen,
				&actualReplyLenW,
				(long)timeout,
				flags);
		/*
		 * Check to ensure that the SUT contains the necessary TSMP 2.5 objects
		 */
		if (fserror==EVERSION)
		{
			jRcv_throwUnsatisfiedLinkError(env, "Wrong TS/MP Version, Unable to find symbol :SERVERCLASS_DIALOG_BEGINL_");
			free(readbuf);
			free(msgbuf);
			return;
		}
		
		if (fserror == 0 || fserror == 70 ) 
		{
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
			if (pwerror ==0 && (fserror==0 || fserror==70))
				if (actualReplyLenW> 0)
					// Put reply data into reply byte array
					(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLenW, readbuf);
		}
		else
			// send failed, get pathsend and file system errors
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);

		// set the actual reply bytes so the Java side can see it
		actualLen = (jint)actualReplyLenW;
		(*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, &actualLen);

		// set the dialog id
		(*env)->SetIntArrayRegion(env, jdlgId, 0, 1, (jint *)&dlgId);
		/*
		* Fill in the error buffer
		*/
		errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
		errorbuf[0] = (int)pwerror;
		errorbuf[1] = (int)fserror;
		(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);
		/*
		 * Clean up the Java strings and arrays.
		 */
		(*env)->ReleaseStringUTFChars(env, pmonName, pmonname);
		(*env)->ReleaseStringUTFChars(env, scName, scname);
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);

		free(msgbuf);
		free(readbuf);
	}

	return errorArray;

} // end scDialogBegin


/*
 * Class:     com_tandem_tsmp_TsmpDialog
 * Method:    scDialogSend
 * Signature: (I[BI[BI[II)[I
 *
 * Functional Description:
 *
 * JNI native method for TsmpDialog which makes call to SPT_SERVERCLASS_DIALOG_SEND_ of pthread.

 * Formal Paramaters:
 *
 *  env, The Java environment pointer                                   input
 *  obj, Java object representing TsmpDialog object                     input
 *  jdlgId, Integer representing dialog Id                              input
 *  request, Byte Array representing the request buffer                 input
 *  requestlength, Integer representing the request length              input
 *  reply, Byte Array representing the reply buffer                     output
 *  maxReplyLen, Integer representing the maximum reply length          input
 *  jActualReplyLen, Integer Array representing the actual reply length output
 *  timeout, Integer representing the timeout value                     input
 *  pathSendMode, Integer representing the Pathsend Mode value          input
 * Implicit Inputs:
 *
 *  None.
 *
 * Implicit Outputs:
 *
 *  None.
 *
 * Return Value:
 *
 * pathsend and file system errors.
 *
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpDialog_scDialogSend(JNIEnv *env, jobject obj,
		jint jdlgId, jbyteArray request, jint requestlength,
		jbyteArray reply, jint maxReplyLen, jintArray jActualReplyLen, jint timeout, jint pathSendMode){

	jintArray errorArray;
	short actualReplyLen = 0;

	// Begin of Changes for Solution Number 10-060117-3897

	jint actualLen = 0;

	// End of Changes for Solution Number 10-060117-3897
	short pwerror = 0;
	short fserror = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};

	jbyte *reqbuf = (*env)->GetByteArrayElements(env, request, 0);
	jbyte *msgbuf = NULL;

	if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
	{
		//allocate memory large enough to hold max of request and reply
		if(maxReplyLen >= requestlength)
			/*
			changed malloc to malloc_pages for getting page aligned address since User Buffer memory is used instead of PFS memory 
			*/
			msgbuf = (jbyte *) malloc_pages(maxReplyLen);
		else
			msgbuf = (jbyte *) malloc_pages(requestlength);
		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");

		memcpy(msgbuf, reqbuf, requestlength);
		/*
		 Make the call to the pthreads version of SERVERCLASS_DIALOG_SEND_.
		 */
		fserror = SPT_SERVERCLASS_DIALOG_SEND_((long)jdlgId,
			(char *)msgbuf,
			(short)requestlength,
			(short)maxReplyLen,
			&actualReplyLen,
			(long)timeout,
			 NOWAITIO);

		if (fserror == 0 || fserror == 70 ) {
			//Begin changes for Rout RFE sol:10-081125-7556
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
			if (pwerror ==0 && (fserror==0 || fserror==70))
				//End changes for Rout RFE sol:10-081125-7556
				if (actualReplyLen> 0)
					// Put reply data into reply byte array
					(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLen, msgbuf);
		}
		else
			// send failed, get pathsend and file system errors
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);

		// set the actual reply bytes so the Java side can see it
		actualLen = (jint)actualReplyLen;
		(*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, &actualLen);

		/*
		 * Fill in the error buffer
		 */
		errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
		errorbuf[0] = (int)pwerror;
		errorbuf[1] = (int)fserror;
		(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);

		/*
		 * Clean up the Java strings and arrays.
		 */
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);

		free(msgbuf);
	}

	else if(pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_2MB_LIMIT)
	{
		int actualReplyLenW = 0; // For large path sends
		jbyte *readbuf = NULL;
		
		//Check to ensure that the SUT contains the appropriate Pthread DLL for TSMP 2.5 support
		initialise_functionptr();
		if (!spt_serverclass_dialog_beginl_)
		{
			jRcv_throwUnsatisfiedLinkError(env, "Pthreads version mismatch. Unable to find symbol : SPT_SERVERCLASS_DIALOG_BEGINL_/SPT_SERVERCLASS_DIALOG_SENDL_\n");
			return errorArray;
		}
		/*
		changed malloc to malloc_pages for getting page aligned address since User Buffer memory is used instead of PFS memory 
		*/
		readbuf = (jbyte *) malloc_pages(maxReplyLen);
		if(!readbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");

		msgbuf = (jbyte *) malloc_pages(requestlength);
		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		memcpy(msgbuf, reqbuf, requestlength);	
		
		/* Make the call to the pthreads version of SERVERCLASS_DIALOG_SENDL_
		 * for 2MB support.
		 */
		fserror = spt_serverclass_dialog_sendl_((long)jdlgId,
						      (char *)msgbuf,
						      (char *)readbuf,
						      (long)requestlength,
						      (long)maxReplyLen,
						      &actualReplyLenW,
						      (long)timeout,
						      NOWAITIO);			  
		/*
		 * Check to ensure that the SUT contains the necessary TSMP 2.5 objects
		 */
		if (fserror==EVERSION)
		{
		     jRcv_throwUnsatisfiedLinkError(env, "Wrong TS/MP Version, Unable to find symbol :SERVERCLASS_DIALOG_SENDL_");
			 free(readbuf);
			 free(msgbuf);
		     return;
		}
		if (fserror == 0 || fserror == 70 ) {
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
			if (pwerror ==0 && (fserror==0 || fserror==70))
				if (actualReplyLenW> 0)
					// Put reply data into reply byte array
					(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLenW, readbuf);
		}
		else
			// send failed, get pathsend and file system errors
			SPT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);

		// set the actual reply bytes so the Java side can see it
		actualLen = (jint)actualReplyLenW;
		(*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, &actualLen);
		/*
		* Fill in the error buffer
		*/
		errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
		errorbuf[0] = (int)pwerror;
		errorbuf[1] = (int)fserror;
		(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);

		/*
		* Clean up the Java strings and arrays.
		*/
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);

		free(msgbuf);
		free(readbuf);
	}

	return errorArray;

} // end scDialogSend


/*
 * Class:     com_tandem_tsmp_TsmpDialog
 * Method:    scDialogAbort
 * Signature: (I)[I
 *
 * Functional Description
 *
 * JNI native method for TsmpDialog which makes call to SIP_SERVERCLASS_DIALOG_ABORT_ of pthread.
 *
 * Formal Paramaters:
 *
 *  env, The Java environment pointer                       input
 *  obj, Java object representing TsmpDialog object         input
 *  jdlgId, Integer representing dialog Id                  input
 *
 * Implicit Inputs:
 *
 *  None.
 *
 * Implicit Outputs:
 *
 *  None.
 *
 * Return Value:
 *
 * pathsend and file system errors.
 *
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpDialog_scDialogAbort(JNIEnv *env, jobject obj,
		jint jdlgId) {

	jintArray errorArray;
	short pwerror = 0;
	short fserror = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};

	fserror = SPT_SERVERCLASS_DIALOG_ABORT_((long)jdlgId);

	if(fserror != 0)
		SPT_SERVERCLASS_SEND_INFO_(&pwerror,&fserror);

	/*
	 * Fill in the error buffer
	 */
	errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
	errorbuf[0] = (int)pwerror;
	errorbuf[1] = (int)fserror;
	(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);

	return errorArray;

} // scDialogAbort

/*
 * Class:     com_tandem_tsmp_TsmpDialog
 * Method:    scDialogEnd
 * Signature: (I)[I
 *
 * Functional Description:
 *
 * JNI native method for TsmpDialog which makes call to SPT_SERVERCLASS_DIALOG_END_ of pthread.
 *
 * Formal Paramaters:
 *
 *  env, The Java environment pointer                       input
 *  obj, Java object representing TsmpDialog object         input
 *  jdlgId, Integer representing dialog Id                  input
 *
 * Implicit Inputs:
 *
 *  None.
 *
 * Implicit Outputs:
 *
 *  None.
 *
 * Return Value:
 *
 *  pathsend and file system errors.
 *
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpDialog_scDialogEnd(JNIEnv *env, jobject obj,
		jint jdlgId) {
	jintArray errorArray;
	short pwerror = 0;
	short fserror = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};

	fserror = SPT_SERVERCLASS_DIALOG_END_((long)jdlgId);

	if(fserror != 0)
		SPT_SERVERCLASS_SEND_INFO_(&pwerror,&fserror);

	/*
	 * Fill in the error buffer
	 */
	errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
	errorbuf[0] = (int)pwerror;
	errorbuf[1] = (int)fserror;

	(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);

	return errorArray;

} //scDialogEnd

