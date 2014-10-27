/* ------------------------------------------------------------------------
* Filename:     ReceiveNoOpeners.java
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2001
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

package com.tandem.ext.guardian;

/**
 * Signals there are no more openers to send messages on $RECEIVE.  A properly
 * coded Pathway server will stop when there are no more openers.
 *
 * @version 1.0, 8/27/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class ReceiveNoOpeners extends Exception {

   /**
    * Constructs a ReceiveNoOpeners exception object. 
    */
   public ReceiveNoOpeners () {
      super("$RECEIVE: No more processes have this process open.");
      
   } /* end ctor */

}  /* end class ReceiveNoOpeners */


