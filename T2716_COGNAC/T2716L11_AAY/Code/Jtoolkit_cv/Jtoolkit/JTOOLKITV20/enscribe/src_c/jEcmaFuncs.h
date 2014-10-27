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
#ifndef JECMAFUNCS_H
#define JECMAFUNCS_H

void JEnscribe_callback (short filenum,
                         long tag,
                         long completionCount,
                         long fserror,
                         void *userdata);          

short JEnscribeWaitForCompletion(short fnum, long tag, int* countTransferred);
short JEnscribeCancelReq(short fnum, long tag);
void JEnscribeCancelAnyOutstandingReq(short fnum, long tag);

#endif /* JECMAFUNCS_H */
