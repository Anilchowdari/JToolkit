/* ------------------------------------------------------------------------
* Filename:     SysMsgNodeDown.java
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
 * The <code>SysMsgNodeDown</code> class is a wrapper for a node down system 
 * message read in from $RECEIVE.
 * <p>
 * A node down system message is received when a remote node has either gone
 * down or has become partitioned from the local node.  This
 * is a logical close for any openers running in the remote node.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgNodeDown {

   /* each node down msg contains the following data */
   private int    _nodeNumber;
   private String _nodeName;

   /**
    * Returns the node number contained in this node down message.
    *
    * @return the node number of the node that the local system lost
    * communication with
    */
   public int getNodeNumber() {return _nodeNumber;}

   /**
    * Returns the node name contained in this node down message.
    *
    * @return the name of the node that the local system lost
    * communication with
    */
   public String getNodeName() {return _nodeName;}

   /**
    * Constructs a SysMsgNodeDown from a byte array.
    * <p>
    * The structure of the node down message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 0   = -110
    *    word 1-3 = Reserved, subject to change
    *    word 4-5 = Node number
    *    word 6   = Length of node name, in bytes
    *    word 7-* = Node name (including the \)</pre>
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid node down message
    */
   public SysMsgNodeDown (byte[] msg) {

      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      if (msg.length < 16) {
         throw new IllegalArgumentException(
            "A node down system message should be at least 16 bytes long not " + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_NODEDOWN) {
            throw new IllegalArgumentException(
               "A node down system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_NODEDOWN + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset = 8/* skip over msgNum and reserved bytes */;
      try {
         _nodeNumber = DataConversion.BIN32ToJavaInt(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid node identifier: " 
                + ex.getMessage());
      }

      offset += 4;
      short nodeNameLen;
      try {
         nodeNameLen = DataConversion.BIN16ToJavaShort(msg, offset);
         if (nodeNameLen < 2) {
            throw new IllegalArgumentException(
               "Illegal node name length, length must be at least 2 bytes: " 
                 + nodeNameLen);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid node name length: " 
                + ex.getMessage());
      }

      offset += 2;
      if ((offset+nodeNameLen) > msg.length) {
         throw new IllegalArgumentException("Remote node name cannot be found in message.");
      }
      _nodeName = new String(msg, offset, nodeNameLen);

   } /* end ctor */

}  /* end class SysMsgNodeDown */
