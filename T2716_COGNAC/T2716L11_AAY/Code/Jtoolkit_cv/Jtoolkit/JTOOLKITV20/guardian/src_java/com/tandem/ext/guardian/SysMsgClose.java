/* ------------------------------------------------------------------------
* Filename:     SysMsgClose.java
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2002
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

import com.tandem.ext.util.DataConversion;
import com.tandem.ext.util.DataConversionException;

/**
 * The <code>SysMsgClose</code> class is a wrapper for a close system 
 * message read in from $RECEIVE.
 * <p>
 * A close system message is received if a requester closes the server process.
 * There will be no more messages received from the requester until it opens
 * the server process again.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgClose {

   /* each close msg contains the tapedisposition parameter to FILE_CLOSE_ */
   private short tapeDisposition_;

   /**
    * Returns the tape disposition parameter the opener used in the call to
    * close.
    *
    * @return the tape disposition of the closed message
    */
   public short getTapeDisposition() {return tapeDisposition_;}

   /**
    * Constructs a SysMsgClose from a byte array.
    * <p>
    * The structure of the close message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 1 = -104
    *    word 2 = The tapedisposition parameter to FILE_CLOSE_
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid close message
    */
   public SysMsgClose (byte[] msg) {
      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      // Note: we can't check for the exact length because the array may be 
      // larger than the message
      if (msg.length < 4) {
         throw new IllegalArgumentException(
            "A close system message should be 4 bytes long not " + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_CLOSE) {
            throw new IllegalArgumentException(
               "A close system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_CLOSE + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         tapeDisposition_ = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid tape disposition: " 
                + ex.getMessage());
      }

   } /* end ctor */

}  /* end class SysMsgClose */
