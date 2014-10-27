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
#include "com_tandem_ext_guardian_ReceiveInfo.h"
#include <pthread.h>
#include "tmrqst.h"

/*
 * Class:     com_tandem_ext_guardian_ReceiveInfo
 * Method:    activateRequestTransID
 * Signature: (S)I
 *
 * Makes the TMF tx associated with a particular message the current transaction
 * for the thread.
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_ReceiveInfo_activateRequestTransID(
                             JNIEnv *env,
                             jobject thisFile,
                             jshort msgTag)
{
   short error = FEOK;

   //SPT_TMF_TxHandle_t txHandle;
   PUT_TMF_TxHandle_t txHandle;
   error = jpwy_activatetransid(msgTag);

   if (error == FEOK) {
      // make Pthreads aware of the transaction so when the thread suspends and
      // resumes, the transaction will become current again.

      //SPT_TMF_GetTxHandle(&txHandle);
      //error=SPT_TMF_SetTxHandle(&txHandle);
	   PUT_TMF_GetTxHandle(&txHandle);
	   error=PUT_TMF_SetTxHandle(&txHandle);
   }

   return error;       
}

/*
 * Class:     com_tandem_ext_guardian_ReceiveInfo
 * Method:    isMessageCancelled
 * Signature: (S)I
 *
 * Returns 1 if message has been cancelled by the sender, returns 0 otherwise.
 */
JNIEXPORT jint JNICALL Java_com_tandem_ext_guardian_ReceiveInfo_isMessageCancelled(
                             JNIEnv *env,
                             jobject thisFile,
                             jshort msgTag)
{
   return(jpwy_cancelled(msgTag));
}
