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
#include <sys/types.h>
#include <cextdecs.h>
#include <tal.h>
#include <error.h>

#include <spthread.h>


/* -------------------------------------------------------------- */
/* Callback for SPT*/
/*
 *  FUNCTIONAL DESCRIPTION:
 *
 *	JEnscribe_callback:
 *
 *  This is the file i/o handler to be registered with the dispatcher
 *  It will be called each time an Enscribe completion occurs.
 *
 *  FORMAL PARAMETERS:
 *
 *  filenum           - Guardian file number whose IO has completed. 
 *  tag               - tag of completed IO. 
 *  count_transferred - transfer count of completed IO. 
 *  error             - Guardian error number for completed IO. 
 *  userdata          - address of user data area. Unused by this callback. 
 *
 *  IMPLICIT INPUTS: The filenum is used as tag
 *
 *  IMPLICIT OUTPUTS:  none
 *
 *  FUNCTION VALUE:  none
 *
 *  SIDE EFFECTS:
 *
 *	The thread associated with the completion will be readied.
 */

void JEnscribe_callback (short filenum,
                         long  tag,
                         long  completionCount,
                         long  fserror,
                         void  *userdata)
{                                                                            
   short error;

   /*
    * Completion is of an Enscribe file IO.  Need to use fnum
    * instead of tag to get the tcb because FILE_OPEN_ doesn't take a tag.
    * Since the file number is the same as the tag when
    * we register, this is ok.
    */

    tag = (long)filenum;
    FILE_GETINFO_(filenum, &error);
    if (error != 0) fserror = error;

    /* Signal the operation complete */
    spt_wakeup(filenum,tag,completionCount,fserror); 

}	/* end function JEnscribe_callback */

/* ---------------------------------------------------------------------------
 * This routine is where the thread waits until the Enscribe File operation
 * completes. it returns error and sets countTransferred to the correct value.
 * --------------------------------------------------------------------------*/
short JEnscribeWaitForCompletion(short fnum, long tag, long* countTransferred)
{

    long fserror;
    spt_error_t spte;

	*countTransferred = 0;
	/* Await the io completion which calls the callback handler
	 * registered for the filenumber.  The callback handler will 
	 * FILE_GETINFO_ and store the results before waking this thread.
        */

    spte = spt_awaitio(fnum,tag,-1,countTransferred,&fserror,NULL);

    if(spte != SPT_SUCCESS) (void) CANCELREQ(fnum,tag);

    return (short)fserror;

} /* JEnscribeWaitForCompletion */

/* ----------------------------------------------------------------------------
 * This routine cancels an outstanding request and wakes up the
 * thread that was waiting.
 * ---------------------------------------------------------------------------- */
short JEnscribeCancelReq(short fnum, long tag)
{

    spt_error_t spte;
    short error;

    /* Interrupt the thread that is waiting on I/O */

    if(_status_lt(CANCELREQ(fnum,tag))) /* Cancel the I/O Operation */
    {
      FILE_GETINFO_(fnum,&error);
      return error;
    }

    /* If the cancel is successful wake up the thread with
       error FEREQABANDONED.                                                 
    */

    spte=spt_wakeup(fnum,tag,(long)0,FEREQABANDONED);

    if(spte==SPT_SUCCESS)
      return FEOK;
    else
      return FENONEOUT;
}
/*------------------------------------------------------------
 * Cancels any outstanding request on a filenum
 *-----------------------------------------------------------*/
void JEnscribeCancelAnyOutstandingReq(short fnum, long tag)
{
	JEnscribeCancelReq(fnum,tag);
	return;
}
