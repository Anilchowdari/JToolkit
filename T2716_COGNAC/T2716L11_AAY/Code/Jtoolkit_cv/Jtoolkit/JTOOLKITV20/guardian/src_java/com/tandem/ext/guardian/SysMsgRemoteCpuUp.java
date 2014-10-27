/* ------------------------------------------------------------------------
* Filename:     SysMsgRemoteCpuUp.java
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
 * The <code>SysMsgRemoteCpuUp</code> class is a wrapper for a remote CPU up system 
 * message read in from $RECEIVE.
 * <p>
 * A remote CPU up system message is received when a remote CPU is reloaded.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgRemoteCpuUp {

   /* A valid CPU number is between 0 and 15 inclusive */
   private static final short MINCPU = 0;
   private static final short MAXCPU = 15;
   
   /* each remote CPU up msg contains the node number of the remote node */
   private int _nodeNumber;

   /* each remote CPU up msg contains the CPU number that went up */
   private short _cpuNumber;

   /* each remote CPU up msg contains the node number of the remote node */
   private String _nodeName;

   /**
    * Returns the node number contained in the remote CPU up message.
    *
    * @return the number of the node where the CPU was reloaded
    */
   public int getNodeNumber() {return _nodeNumber;}

   /**
    * Returns the CPU number contained in the remote CPU up message.
    *
    * @return the number of the remote CPU was reloaded
    */
   public short getCpuNumber() {return _cpuNumber;}

   /**
    * Returns the node name contained in the remote CPU up message.
    *
    * @return the node name of the node where the CPU was reloaded
    */
   public String getNodeName() {return _nodeName;}

   /**
    * Constructs a SysMsgRemoteCpuUp from a byte array.
    * <p>
    * The structure of the remote CPU up message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 0   = -113
    *    word 1-2 = Node number
    *    word 3   = Processor number
    *    word 4   = Length of node name, in bytes
    *    word 5-7 = Reserved
    *    word 8-* = Node name (including the \) </pre>
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid remote CPU up message
    */
   public SysMsgRemoteCpuUp (byte[] msg) {
      int offset= 0;  // starting offset in byte array

      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      if (msg.length < 16) {
         throw new IllegalArgumentException(
            "A remote CPU up system message should be at least 16 bytes long not "
             + msg.length);
      }

      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_REMOTECPUUP) {
            throw new IllegalArgumentException(
               "A remote CPU up system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_REMOTECPUUP + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _nodeNumber = DataConversion.BIN32ToJavaInt(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid remote node number: " 
                + ex.getMessage());
      }

      offset += 4;
      try {
         _cpuNumber = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid remote CPU number: " 
                + ex.getMessage());
      }

      if ((_cpuNumber < MINCPU) || (_cpuNumber > MAXCPU)) {
         throw new IllegalArgumentException("Illegal remote CPU number: " + _cpuNumber);
      }
      
      offset += 2;
      short nodeNameLen = 0;
      try {
         nodeNameLen = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid node name length: " 
                + ex.getMessage());
      }

      offset += 8; // skip over node name len and reserved bytes
      if ((offset+nodeNameLen) > msg.length) {
         throw new IllegalArgumentException("Remote node name cannot be found");
      }
      _nodeName = new String(msg, offset, nodeNameLen);

   } /* end ctor */

}  /* end class SysMsgRemoteCpuUp */
