/* ------------------------------------------------------------------------
* Filename:     ProcessHandle.c
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 12/4/2001
*   Compaq Computer Corporation
*      Protected as an unpublished work.
*         All rights reserved.
*
*   The computer program listings, specifications, and documentation
*   herein are the property of Compaq Computer Corporation or a
*   third party supplier and shall not be reproduced, copied,
*   disclosed, or used in whole or in part for any reason without
*   the prior express written permission of Compaq Computer
*   Corporation.
*
*  @@@ END COPYRIGHT @@@
*
* ------------------------------------------------------------------------ */


/** include files **/
#include "com_tandem_ext_guardian_ProcessHandle.h" 
#include <cextdecs.h(PROCESSHANDLE_DECOMPOSE_, PROCESSHANDLE_GETMINE_, PROCESSHANDLE_TO_STRING_)>
#include <assert.h>
//#include <string.h>

/** external functions **/
/*
 * Class:     com_tandem_ext_guardian_ProcessHandle
 * Method:    decompose
 * Signature: ([S)V
 */
JNIEXPORT void JNICALL Java_com_tandem_ext_guardian_ProcessHandle_decompose
  (JNIEnv *env, jobject obj, jshortArray jphandle)
{
   short error1;
   short error2;
   short *phandle = NULL;
   short cpu;
   short pin;
   int nodeNumber;
   //Begin changes for solution  no:10-081019-6637 
   char nodeName[9] = {0};  // init to null string 
   //End changes for solution  no:10-081019-6637 
   short nodeMaxlen = 8;
   short nodeNameLength;
   //Begin changes for solution no:10-080614-3852 
   char procName[7] = {0};  // init to null string
    //End changes for solution  no:10-080614-3852 
   short nameMaxlen = 6;
   short procNameLength = 0;
   long long seqNumber;
   jclass clazz = NULL;
   jmethodID mid = NULL;
   jthrowable throwObj = NULL;
   jstring jfuncName = NULL;
   jstring jfileName = NULL;
   jfieldID fid = NULL;
   jstring jstr = NULL;

   // get short[] from jphandle
   phandle = (*env)->GetShortArrayElements(env, jphandle, NULL);
   if (phandle == NULL) return; // exception already taken care of 

   // get everything but the name because if the process doesn't exist and we're
   // asking for the name error 14 will be returned
   error1 = PROCESSHANDLE_DECOMPOSE_(phandle, &cpu, &pin, &nodeNumber, nodeName,
                            nodeMaxlen, &nodeNameLength, procName, nameMaxlen,
                            &procNameLength, &seqNumber);
   // now get the name of the process, we ignore the error 14 returned if process
   // doesn't exist because if named process is dead we'll just report
   // the name as an empty string but any other error is fatal
   error2 = PROCESSHANDLE_DECOMPOSE_(phandle, , , , , , , procName, nameMaxlen,
                                    &procNameLength);
   if (error1 != 0 || error2 != 0 && error2 != 14) {
      // throw GuardianException
      (*env)->ReleaseShortArrayElements(env, jphandle, phandle, JNI_ABORT);
      clazz = (*env)->FindClass(env, "com/tandem/ext/guardian/GuardianException");
      if (clazz == 0) return;
      mid = (*env)->GetMethodID(env, clazz, "<init>", "(ILjava/lang/String;Ljava/lang/String;)V");
      if (mid == 0) return;
//TBD: does the JVM automatically assume it can gc new strings that are created
// in native code?
      jfuncName = (*env)->NewStringUTF(env, "PROCESSHANDLE_DECOMPOSE_");
      if (jfuncName == 0) return;
      jfileName = (*env)->NewStringUTF(env, "$RECEIVE");
      if (jfileName == 0) return;
      error1 = (short)(error1 ? error1 : error2);
      throwObj = (jthrowable)(*env)->NewObject(env, clazz, mid, error1, jfuncName, jfileName);
      if (throwObj == 0) return;
      (*env)->Throw(env, throwObj);
      return;
   }

   // set individual fields of object
   clazz = (*env)->GetObjectClass(env, obj);
   if (clazz == NULL) goto done;
   // cpu_ field
   fid =  (*env)->GetFieldID(env, clazz, "cpu_", "S");
   if (fid == NULL) goto done;
   (*env)->SetShortField(env, obj, fid, cpu); 
   // pin_ field
   fid =  (*env)->GetFieldID(env, clazz, "pin_", "S");
   if (fid == NULL) goto done;
   (*env)->SetShortField(env, obj, fid, pin); 
   // nodeNumber_ field
   fid =  (*env)->GetFieldID(env, clazz, "nodeNumber_", "I");
   if (fid == NULL) goto done;
   (*env)->SetIntField(env, obj, fid, nodeNumber); 
   // sequenceNumber_ field
   fid =  (*env)->GetFieldID(env, clazz, "sequenceNumber_", "J");
   if (fid == NULL) goto done;
   (*env)->SetLongField(env, obj, fid, seqNumber); 
   // nodeName_ field
   fid =  (*env)->GetFieldID(env, clazz, "nodeName_", "Ljava/lang/String;");
   if (fid == NULL) goto done;
   jstr = (*env)->NewStringUTF(env, nodeName);
   if (jstr == NULL) goto done;
   (*env)->SetObjectField(env, obj, fid, jstr); 
   // processName_ field
//TBD: assumption, if process is unnamed the decompose call will leave procName 
// alone so it will return its empty string value, same with when named process has died
   fid =  (*env)->GetFieldID(env, clazz, "processName_", "Ljava/lang/String;");
   if (fid == NULL) goto done;
   jstr = (*env)->NewStringUTF(env, procName);
   if (jstr == NULL) goto done;
   (*env)->SetObjectField(env, obj, fid, jstr); 

done:
   (*env)->ReleaseShortArrayElements(env, jphandle, phandle, 0);
} // decompose

/*
 * Class:     com_tandem_ext_guardian_ProcessHandle
 * Method:    getString
 * Signature: ([S)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_tandem_ext_guardian_ProcessHandle_getString
  (JNIEnv *env, jobject obj, jshortArray jphandle)
{
   short *phandle;
   short error;
   char processString[20] = {0}; // init to null string
   short plen = 0;
   jstring jstr = NULL;

   // get short[] from jphandle
   phandle = (*env)->GetShortArrayElements(env, jphandle, NULL);
   if (phandle == NULL) return NULL; // exception already taken care of 

   // errors are ignored, if there is an error an empty string will be returned to Java
   error = PROCESSHANDLE_TO_STRING_(phandle, processString, 20, &plen,,,0);

   // create a new Java String and set it with results of call
   jstr = (*env)->NewStringUTF(env, processString);

   (*env)->ReleaseShortArrayElements(env, jphandle, phandle, 0);
   return jstr;
} // getString

/*
 * Class:     com_tandem_ext_guardian_ProcessHandle
 * Method:    nativeGetMine
 * Signature: ()[S
 */
JNIEXPORT jshortArray JNICALL Java_com_tandem_ext_guardian_ProcessHandle_nativeGetMine
  (JNIEnv *env, jclass clazz)
{
   short error;
   short phandle[com_tandem_ext_guardian_ProcessHandle_PHANDLE_SIZE];
   jshortArray  jPhandleArray = NULL;  // holds newly created array of shorts to hold phandle

   // Create a Java array of shorts to hold the requesters Phandle
   jPhandleArray = (*env)->NewShortArray(env, com_tandem_ext_guardian_ProcessHandle_PHANDLE_SIZE);
   if (jPhandleArray == NULL) return 0; // Java exception already created

   error = PROCESSHANDLE_GETMINE_(phandle);
   assert(!error);  // the only errors are programming errors
    
   (*env)->SetShortArrayRegion(env, jPhandleArray, 0/*index*/, 
            com_tandem_ext_guardian_ProcessHandle_PHANDLE_SIZE/*len*/, phandle);
   if ((*env)->ExceptionOccurred(env) != NULL) {
      return NULL; 
   }

   return jPhandleArray;
} // nativeGetMine

