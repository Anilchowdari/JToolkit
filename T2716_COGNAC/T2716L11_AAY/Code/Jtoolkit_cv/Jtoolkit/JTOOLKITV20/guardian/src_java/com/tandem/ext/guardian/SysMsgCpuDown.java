/* ------------------------------------------------------------------------
* Filename:     SysMsgCpuDown.java
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
 * The <code>SysMsgCpuDown</code> class is a wrapper for a CPU down system 
 * message read in from $RECEIVE.
 * <p>
 * A CPU down system message is received if a local processor goes down.  This
 * is a logical close for any openers running in the downed CPU.
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgCpuDown {

   /* A valid CPU number is between 0 and 15 inclusive */
   private static final short MINCPU = 0;
   private static final short MAXCPU = 15;
   
   /* each CPU down msg contains the CPU number that went down */
   private short _cpuNumber;

   /**
    * Returns the CPU number contained in this CPU down message.
    *
    * @return the number of the CPU that went down
    */
   public short getCpuNumber() {return _cpuNumber;}

   /**
    * Constructs a SysMsgCpuDown from a byte array.
    * <p>
    * The structure of the CPU down message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *    word 1 = -2
    *    word 2 = Processor number</pre>
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid CPU down message
    */
   public SysMsgCpuDown (byte[] msg) {
      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      // Note: we can't check for the exact length because the array may be 
      // larger than the message
      if (msg.length < 4) {
         throw new IllegalArgumentException(
            "A CPU down system message should be 4 bytes long not " + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_CPUDOWN) {
            throw new IllegalArgumentException(
               "A CPU down system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_CPUDOWN + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _cpuNumber = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid CPU number: " 
                + ex.getMessage());
      }

      if ((_cpuNumber < MINCPU) || (_cpuNumber > MAXCPU)) {
         throw new IllegalArgumentException("Illegal CPU number: " + _cpuNumber);
      }
   } /* end ctor */

}  /* end class SysMsgCpuDown */
