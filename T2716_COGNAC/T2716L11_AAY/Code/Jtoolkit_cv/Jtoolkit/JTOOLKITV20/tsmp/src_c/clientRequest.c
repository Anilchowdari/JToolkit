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
 
/*
 * This file is responsible for processing SERVERCLASS based Pathsend APIs in 32/64-bit PUT Environment.
 * 32-bit Enviroment:
 * supports Pathsend APIs _SERVERCLASS_SEND_ and _SERVERCLASS_SENDL_
 * Corresponding Pthread APIs PUT_SERVERCLASS_SEND_ and PUT_SERVERCLASS_SENDL_
 *
 * 64-bit Environment:
 * supports Pathsend API _SERVERCLASS_SENDL64_ (no small Pathsend supported)
 * Corresponding Pthread APIs PUT_SERVERCLASS_SENDL_
 */
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "com_tandem_tsmp_TsmpServer.h"
#include "com_tandem_tsmp_TsmpServerWide.h"
#include "com_tandem_ext_util_PathsendModeType.h"
#include  "functionpointers.h"
#include "Receive.h"
#define MAXREPLYLENGTH 32767  /* Changes begin for sol_10-131017-9304 */
#define NOWAITIO 0             /* Pthreads routine only accepts 0 for this */
#define INDEFINITETIMEOUT -1
#define OMITTEDPARAM 0
#define ERROR_ARRAY_SIZE 2

/*
 * Class:     com_tandem_tsmp_TsmpServer
 * Method:    allowUserBuffers
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tandem_tsmp_TsmpServerWide_allowUserBuffers
(JNIEnv *env, jobject obj)
{
	USERIOBUFFER_ALLOW_();
}

/*
 * Class:     com_tandem_tsmp_TsmpServer
 * Method:    scSend
 * Signature: (Ljava/lang/String;Ljava/lang/String;[BI[BI[I[I)[I
 *
 * This native methods supports the new service method that uses byte arrays
 * for the request and reply objects.
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpServer_scSend(JNIEnv *env, jobject obj, jstring pmonName,
		jstring scName, jbyteArray request, jint requestlength, jbyteArray reply,
		jint maxReplyLen, jintArray jActualReplyLen,jint timeout,jint pathSendMode) {
	//changed
	short pmonnamelength;
	short scnamelength;
	jintArray errorArray;
	//changed
	short actualReplyLen = 0;
	int actualReplyLenW = 0; // For large path sends
	short opNum = 0;
	jint actualLen = 0;
	short pwerror = 0;
	short fserror = 0;
	int error = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};
	const char *pmonname;
	const char *scname;
	jbyte *reqbuf;
	jbyte *readbuf;
	jbyte *msgbuf;

	jbyte *baseaddrmsg;
	jbyte *baseaddrreadbuff;
	int remainder=0;

	if (pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_2MB_LIMIT 
#ifdef __LP64
	|| pathSendMode==com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT
	/* In 64-bit check for the presence of PUT_SERVERCLASS_SENDL_ as it
	is used to handle both small as well as large type of pathsends. */
#endif
	)
	{

		initialise_functionptr();
		if (!put_serverclass_sendl_)
		{
			jRcv_throwUnsatisfiedLinkError(env, "Pthreads version mismatch. Unable to find symbol : PUT_SERVERCLASS_SENDL_");
			return;
		}

	}
	pmonname = (*env)->GetStringUTFChars(env, pmonName, 0);
	scname = (*env)->GetStringUTFChars(env, scName, 0);
	reqbuf = (*env)->GetByteArrayElements(env, request, 0);
	//changed
	pmonnamelength = (short)strlen(pmonname);
	scnamelength = (short)strlen(scname);

#ifndef __LP64
	/* Start of Solution: 10-130903-8648. 
	 * Small Pathsend support introduced for 32-bit PUT environment
	 */
	if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT) {
		//10-100617-1177 - To avoid JPathsend hang
		/*
		changed malloc to malloc_pages for getting page aligned address since User Buffer memory is used instead of PFS memory 
		*/
		msgbuf = (jbyte *) malloc_pages(((maxReplyLen > requestlength) ? maxReplyLen : requestlength));
		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		memcpy(msgbuf, reqbuf, requestlength);
		//changes begin for sol: 10-091125-6645
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);
		//changes end for sol: 10-091125-6645
		
		error = PUT_SERVERCLASS_SEND_((char *)pmonname,
				pmonnamelength,
				(char *)scname,
				scnamelength,
				(char *)msgbuf,
				(short)requestlength,
				(short)maxReplyLen,
				&actualReplyLen,
				timeout,
				NOWAITIO,
				&opNum, 0);
	} //end of Solution 10-130903-8648
	else {
#endif
		//10-100617-1177 -To avoid JPathsend hang
		/*
		changed malloc to malloc_pages for getting page aligned address since User Buffer memory is used instead of PFS memory 
		*/
		msgbuf = (jbyte *) malloc_pages(requestlength);
		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		readbuf = (jbyte *) malloc_pages(maxReplyLen);
		/* Solution 10-140421-1562: Jtoolkit: 64-bit NonStop Java 7 fails with fatal error in native method: 
		heap allocation failed when a value of new byte[0] is referenced */
		if(!readbuf && maxReplyLen)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		memcpy(msgbuf, reqbuf, requestlength);
		//changes begin for sol: 10-091125-6645
		(*env)->ReleaseByteArrayElements(env, request, reqbuf, JNI_ABORT);
		// changes end for sol: 10-091125-6645

		error = put_serverclass_sendl_((char *)pmonname,
				pmonnamelength,
				(char *)scname,
				scnamelength,
				(char *)msgbuf,
				(char *)readbuf,
				requestlength,
				maxReplyLen,
				&actualReplyLenW,
				timeout,
				NOWAITIO,
				&opNum, 0);
#ifndef __LP64
	}
#endif
	//changed end
	// Begin Changes for Solution Number:10-090508-1451
	if (error==EVERSION)
	{
		jRcv_throwUnsatisfiedLinkError(env, "Wrong TS/MP Version, Unable to find symbol :SERVERCLASS_SENDL_");
		return;
	}
	// End Changes for Solution Number:10-090508-1451
	if (error == 0 )
	{
		//Begin changes for Rout RFE sol:10-081125-7556
		error = PUT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
		if (pwerror ==0 && fserror==0)
			//End changes for Rout RFE sol:10-081125-7556
			if (
#ifndef __LP64
			actualReplyLen> 0 ||
#endif 
			actualReplyLenW> 0)
			{
				// Put reply data into reply byte array
#ifndef __LP64
				if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
					(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLen, msgbuf);
				else
#endif
					(*env) ->SetByteArrayRegion(env, reply, 0, actualReplyLenW, readbuf);

			}
	}
	else {
		// send failed, get pathsend and file system errors
		error = PUT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
	}

	/*
	 * Clean up the Java strings and arrays.
	 */
	(*env)->ReleaseStringUTFChars(env, pmonName, pmonname);
	(*env)->ReleaseStringUTFChars(env, scName, scname);


	//10-100617-1177 -To avoid JPathsend hang
	free(msgbuf);

#ifndef __LP64
	if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_2MB_LIMIT)
#endif
		//10-100617-1177 -To avoid JPathsend hang
		free(readbuf);
	// set the actual reply bytes so the Java side can see it
#ifndef __LP64
	if (pathSendMode == com_tandem_ext_util_PathsendModeType_TSMP_PATHSEND_32KB_LIMIT)
		actualLen = (jint)actualReplyLen;
	else
#endif
		actualLen = (jint)actualReplyLenW;
	(*env)->SetIntArrayRegion(env, jActualReplyLen, 0, 1, &actualLen);

	/*
	 * Fill in the error buffer
	 */
	errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
	errorbuf[0] = (int)pwerror;
	errorbuf[1] = (int)fserror;
	(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);

	return errorArray;
} // end scSend


/*
 * Class:     com_tandem_tsmp_TsmpServer
 * Method:    clientRequest
 * Signature: (Ljava/lang/String;Ljava/lang/String;[B[Ljava/lang/Object;I;I)[I
 *
 * Functional Description
 *
 * clientRequest.c (com_tandem_tsmp_TsmpServer_clientRequest)
 *
 * JNI native method which makes the pthreads call to SERVERCLASS_SEND_ for
 * use with JPathsend.
 *
 * Formal Paramaters
 *  env, The Java environment pointer                        input
 *  pmonName, String representing the Pathmon process name   input
 *  scName, String representing the Pathmon serverclass name input
 *  inbuf, String representing the request buffer            input
 *  timeout, Integer representing the timeout value           input
 *  outbuf, String representing the reply buffer             output
 *
 * Implicit Inputs:
 *  serverclasserror and filesystemerror - obtained only if there
 *    is an error on the SERVERCLASS_SEND_.
 *
 * Implicit Outputs:
 *  None.
 *
 * Return Value:
 *  Array of pathsend/file system errors.
 *
 */
JNIEXPORT jintArray JNICALL
Java_com_tandem_tsmp_TsmpServer_clientRequest(JNIEnv *env, jobject obj, jstring pmonName,
		jstring scName, jbyteArray inputbuf, jobjectArray objectArray, jint maxReplyLen,jint timeout) {
		
/*
	Dividing the function in 2 different sections: 32-bit and 64-bit environments. 
	32-bit supports both small and large pathsends.
	64-bit supports only large pathsends.
	
	Implemented on 28th September 2013. @Pabitra
*/	

#ifdef __LP64
//64-bit code 

	short pmonnamelength;
	short scnamelength;
	jintArray errorArray;
	short opNum = 0;
	int actualreplylength = 0; /*changes made for sol_10-131017-9304 */
	short pwerror = 0;
	short fserror = 0;
	int error = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};

	const char *pmonname = (*env)->GetStringUTFChars(env, pmonName, 0);
	const char *scname = (*env)->GetStringUTFChars(env, scName, 0);
	jbyte *inbuf = (*env)->GetByteArrayElements(env, inputbuf, 0);
	jbyteArray outbuf;
	jsize requestlength = (*env)->GetArrayLength(env, inputbuf);

	char *msgbuf = 0;
	//char *writebuf = 0;
	pmonnamelength = (short)strlen(pmonname);
	scnamelength = (short)strlen(scname);

	/*
	 Special cased errors.
	 -1 The actual reply length from the server is not from 1 through 32,000
	 (MAXREPLYLENGTH).
	 -2 The max expected reply length is not less than 1.

	 The allowable reply length is 0 through 32767, but max value
	 is limited to 32,000 because of expand limitations.
	 */

	if ( maxReplyLen < 1 )
	{
		pwerror = -2;
	}
	else
	{
		if (maxReplyLen> MAXREPLYLENGTH)
			maxReplyLen = MAXREPLYLENGTH;
		/* allocate memory large enough to hold max of request and reply */
		if (maxReplyLen >= requestlength)
		{
			msgbuf = (char *) malloc(maxReplyLen);
		}
		else
		{
			msgbuf = (char *) malloc(requestlength);
		}

		// Begin Changes for Solution Number 10-040505-5748

		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");
		
		 /*Make the call to the pthreads version of SERVERCLASS_SEND_.
		 */
		memcpy(msgbuf,inbuf,requestlength);
	

		// Begin of Change Solution Number 10-030808-8620

		error = PUT_SERVERCLASS_SENDL_((char *)pmonname,
				pmonnamelength,
				(char *)scname,
				scnamelength,
				msgbuf,,
				(int)requestlength,
				(int)maxReplyLen,
				(int *)&actualreplylength,
				(int)timeout,
				NOWAITIO,
				(short *)&opNum,
				0);

		// End of Change Solution Number 10-030808-8620

		if (error == 0 )
		{
			/* This can never actually happen (unless there's a filesystem bug)
			 * but it doesn't hurt to check I guess.*/
			 
			 /* Changes Begin for sol_10-131017-9304 */
			if (actualreplylength > MAXREPLYLENGTH)
				pwerror = -1;
			   else if( actualreplylength < 1 )
			   pwerror = -3;
			 /* Changes End for sol_10-131017-9304 */
			   
			else
			{
				/*
				 If the call is ok, place the contents of the message buffer
				 into an array whose length is the actual reply length,
				 and pass that array back to the java side in the first element
				 of the objectArray.
				 */

				outbuf = (*env) ->NewByteArray(env, actualreplylength);
				(*env) ->SetByteArrayRegion(env, outbuf, 0,
						actualreplylength, (jbyte *)msgbuf);
				(*env) ->SetObjectArrayElement(env, objectArray, 0, outbuf);
			}
		}
		else /* error != 0 */
		{ /*
		 ...make the call to the pthreads version of SERVERCLASS_SEND_INFO_
		 and return the Pathsend error number.
		 */
			error = PUT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
		}
	} /* if (maxReplyLen < 1 )  */

	/*
	 * Clean up the Java strings and arrays.
	 */
	(*env)->ReleaseStringUTFChars(env, pmonName, pmonname);
	(*env)->ReleaseStringUTFChars(env, scName, scname);
	(*env)->ReleaseByteArrayElements(env, inputbuf, inbuf, JNI_ABORT);

	/*
	 * Fill in the error buffer
	 */
	errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
	errorbuf[0] = (int)pwerror;
	errorbuf[1] = (int)fserror;
	(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);
	free(msgbuf);

	return errorArray;
//end of 64-bit code

#else
//32-bit code
	short pmonnamelength;
	short scnamelength;
	jintArray errorArray;
	short opNum = 0;
	short actualreplylength = 0;
	short pwerror = 0;
	short fserror = 0;
	int error = 0;
	int errorbuf[ERROR_ARRAY_SIZE] = {0,0};

	const char *pmonname = (*env)->GetStringUTFChars(env, pmonName, 0);
	const char *scname = (*env)->GetStringUTFChars(env, scName, 0);
	jbyte *inbuf = (*env)->GetByteArrayElements(env, inputbuf, 0);
	jbyteArray outbuf;
	jsize requestlength = (*env)->GetArrayLength(env, inputbuf);

	char *msgbuf = 0;
	pmonnamelength = (short)strlen(pmonname);
	scnamelength = (short)strlen(scname);

	/*
	 Special cased errors.
	 -1 The actual reply length from the server is not from 1 through 32,000
	 (MAXREPLYLENGTH).
	 -2 The max expected reply length is not less than 1.

	 The allowable reply length is 0 through 32767, but max value
	 is limited to 32,000 because of expand limitations.
	 */

	if ( maxReplyLen < 1 )
	{
		pwerror = -2;
	}
	else
	{
		if (maxReplyLen> MAXREPLYLENGTH)
			maxReplyLen = MAXREPLYLENGTH;
		/* allocate memory large enough to hold max of request and reply */
		if (maxReplyLen >= requestlength)
			msgbuf = (char *) malloc(maxReplyLen);
		else
			msgbuf = (char *) malloc(requestlength);

		// Begin Changes for Solution Number 10-040505-5748

		if(!msgbuf)
			(*env)->FatalError(env ,"malloc failure: heap allocation failed");

		// End  Changes for Solution Number 10-040505-5748

		/*
		 Make the call to the pthreads version of SERVERCLASS_SEND_.
		 */
		memcpy(msgbuf,inbuf,requestlength);

		// Begin of Change Solution Number 10-030808-8620

		error = PUT_SERVERCLASS_SEND_((char *)pmonname,
				pmonnamelength,
				(char *)scname,
				scnamelength,
				msgbuf,
				(short)requestlength,
				(short)maxReplyLen,
				&actualreplylength,
				timeout,
				NOWAITIO,
				&opNum,
				0);

		// End of Change Solution Number 10-030808-8620

		if (error == 0 )
		{
			/* This can never actually happen (unless there's a filesystem bug)
			 * but it doesn't hurt to check I guess.*/
			 
			 /* Changes Begin for sol_10-131017-9304 */
			if (actualreplylength > MAXREPLYLENGTH)
			    pwerror = -1;
				else if( actualreplylength < 1 )
				pwerror = -3;
			 /* Changes Begin for sol_10-131017-9304 */
			else
			{
				/*
				 If the call is ok, place the contents of the message buffer
				 into an array whose length is the actual reply length,
				 and pass that array back to the java side in the first element
				 of the objectArray.
				 */

				outbuf = (*env) ->NewByteArray(env, actualreplylength);
				(*env) ->SetByteArrayRegion(env, outbuf, 0,
						actualreplylength, (jbyte *)msgbuf);
				(*env) ->SetObjectArrayElement(env, objectArray, 0, outbuf);
			}
		}
		else /* error != 0 */
		{ /*
		 ...make the call to the pthreads version of SERVERCLASS_SEND_INFO_
		 and return the Pathsend error number.
		 */
			error = PUT_SERVERCLASS_SEND_INFO_(&pwerror, &fserror);
		}
	} /* if (maxReplyLen < 1 )  */

	/*
	 * Clean up the Java strings and arrays.
	 */
	(*env)->ReleaseStringUTFChars(env, pmonName, pmonname);
	(*env)->ReleaseStringUTFChars(env, scName, scname);
	(*env)->ReleaseByteArrayElements(env, inputbuf, inbuf, JNI_ABORT);

	/*
	 * Fill in the error buffer
	 */
	errorArray = (*env)->NewIntArray(env,ERROR_ARRAY_SIZE);
	errorbuf[0] = (int)pwerror;
	errorbuf[1] = (int)fserror;
	(*env)->SetIntArrayRegion(env, errorArray, 0, ERROR_ARRAY_SIZE, (jint *)&errorbuf[0]);
	free(msgbuf);

	return errorArray;
#endif
//end of 32-bit code
}
