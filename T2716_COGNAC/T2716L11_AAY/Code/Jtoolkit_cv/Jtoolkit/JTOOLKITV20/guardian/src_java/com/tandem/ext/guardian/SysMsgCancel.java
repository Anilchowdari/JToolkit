/* ------------------------------------------------------------------------
* Filename:     SysMsgCancel.java
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
 * The <code>SysMsgCancel</code> class is a wrapper for a cancel system 
 * message read in from $RECEIVE.
 * <p>
 * A cancel system message is received if a requester cancels a request
 * read in by the server but not yet replied to.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgCancel {

   /* each cancel msg contains the message tag of the cancelled msg */
   private short _msgTag;

   /**
    * Returns the message tag contained in the cancel message.  The tag
    * identifies which message has been cancelled.
    *
    * @return the message tag of the cancelled message
    */
   public short getMessageTag() {return _msgTag;}

   /**
    * Constructs a SysMsgCancel from a byte array.
    * <p>
    * The structure of the cancel message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 1 = -38
    *    word 2 = The message-tag of the canceled message</pre>
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    * a valid cancel message
    */
   public SysMsgCancel (byte[] msg) {

      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      // Note: we can't check for the exact length because the array may be 
      // larger than the message
      if (msg.length < 4) {
         throw new IllegalArgumentException(
            "A cancel system message should be 4 bytes long not " + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_CANCEL) {
            throw new IllegalArgumentException(
               "A cancel system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_CANCEL + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _msgTag = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid message tag: " 
                + ex.getMessage());
      }

   } /* end ctor */

}  /* end class SysMsgCancel */
