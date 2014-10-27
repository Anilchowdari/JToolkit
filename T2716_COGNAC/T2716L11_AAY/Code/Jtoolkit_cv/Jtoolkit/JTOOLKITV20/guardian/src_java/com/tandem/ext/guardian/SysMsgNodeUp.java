/* ------------------------------------------------------------------------
* Filename:     SysMsgNodeUp.java
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
 * The <code>SysMsgNodeUp</code> class is a wrapper for a node up system 
 * message read in from $RECEIVE.
 * <p>
 * A node up system message is received if communication is established with
 * a remote node.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgNodeUp {

   /* each node up msg contains the following data */
   private int    _nodeNumber;
   private String _nodeName;

   /**
    * Returns the node number contained in this node up message.
    *
    * @return the node number of the node that the local system established
    * a communication with
    */
   public int getNodeNumber() {return _nodeNumber;}

   /**
    * Returns the node name contained in this node up message.
    *
    * @return the node name of the node that the local system established
    * communication with
    */
   public String getNodeName() {return _nodeName;}

   /**
    * Constructs a SysMsgNodeUp from a byte array.
    * <p>
    * The structure of the node up message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 0   = -111
    *    word 1-3 = Reserved, subject to change
    *    word 4-5 = Node number
    *    word 6   = Length of node name, in bytes
    *    word 7-* = Node name (including the \)
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid node up message
    */
   public SysMsgNodeUp (byte[] msg) {

      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      if (msg.length < 16) {
         throw new IllegalArgumentException(
            "A node up system message should be at least 16 bytes long not " + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_NODEUP) {
            throw new IllegalArgumentException(
               "A node up system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_NODEUP + " not " + msgNum);
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
               "Illegal node name length, node name must be at least 2 bytes: " 
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

}  /* end class SysMsgNodeUp */
