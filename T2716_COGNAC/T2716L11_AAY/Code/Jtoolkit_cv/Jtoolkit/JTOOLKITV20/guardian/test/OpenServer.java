/* ------------------------------------------------------------------------
* Filename:     OpenServer.java
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


import com.tandem.ext.guardian.*;
import com.tandem.tmf.Current;

/**
 * The <code>OpenServer</code> class contains code to test JPathway functionality.
 * This is a simple single-threaded echo server that does nothing with
 * any transaction IDs.  This program gets open msgs and fools around with
 * other system messages turning them on and off.
 */
public class OpenServer {

   /**
    * Constructs a OpenServer 
    */
   public OpenServer () {
      super();
   } /* end ctor */

   public static void main (String args [] )
   {
      // As soon as we're started we open $RECEIVE and wait for messages
      Receive recv = null;
      ReceiveInfo ri = null;
      ProcessHandle phandle = null;
      int countRead; // number of bytes read
      int countReplied; // number of bytes replied
      boolean moreOpeners = true;
      byte[] maxMsg = new byte[57344];  // holds largest possible request
      short errorReturn = FileSystemErrors.FEOK;
    
      // open $Receive
      try {
         recv = Receive.getInstance();
         System.out.println("Asking for open system messages");
         recv.setSystemMessageMask(Receive.OPEN_SMM2);
         System.out.println("Opening $RECEIVE");
         recv.open();
         // setting mask after open goes thru different code
         recv.setSystemMessageMask(Receive.OPEN_SMM2);
         System.out.println("Setting priority queue to true");
         recv.setPriorityQueue(true);
      } catch (GuardianIOException ex) {
         System.err.println("FATAL ERROR: Unable to open $RECEIVE");
         System.err.println(ex.getMessage());
         System.exit(1);
      }
    
      do { // read messages off $receive until last close
         try {
            System.out.println("Calling Receive.read");
            countRead = recv.read(maxMsg, maxMsg.length);
            // -------------Exercise ReceiveInfo class--------------
            System.out.println("Calling Receive.getLastMessageInfo");
            ri = recv.getLastMessageInfo();
            System.out.println("IOType = " + ri.getIOType());
            System.out.println("MaxReply = " + ri.getMaxReply());
            System.out.println("Msg Tag = " + ri.getMessageTag());
            System.out.println("filenum = " + ri.getFileNumber());
            System.out.println("sync ID = " + ri.getSyncID());
            System.out.println("open label = " + ri.getOpenLabel());
            // -------------Exercise ProcessHandle class--------------
            phandle = ri.getProcessHandle();
            System.out.println("phandle = " + phandle); // tests toString method
            System.out.println("phandle CPU = " + phandle.getCPU());
            System.out.println("phandle Pin = " + phandle.getPin());
            System.out.println("phandle node number = " + phandle.getNodeNumber());
            System.out.println("phandle node name = " + phandle.getNodeName());
            String pname = phandle.getProcessName();
            if (pname.length() == 0) 
               System.out.println("opener is running unnamed");
            System.out.println("phandle process name = " + phandle.getProcessName());
            System.out.println("phandle seq # = " + phandle.getSequenceNumber());
            System.out.println("phandle is null? " + phandle.isNull());
            ProcessHandle mine = ProcessHandle.GetMine();
            System.out.println("phandle equals mine? " + phandle.equals(mine));
            short phArr[] = phandle.getPhandleArray();
            // ----------End Exercise of ProcessHandle----------------------
            System.out.println("dialog status = " + ri.getDialogStatus());
            System.out.println("dialog transmode = " + ri.getDialogTransMode());
            System.out.println("isMsgCancelled = " + ri.isMessageCancelled());
            System.out.println("isSystemMessage = " + ri.isSystemMessage());
            // ----------End Exercise of ReceiveInfo----------------------

            if (!ri.isSystemMessage()) {
               // We've got a user msg
               // ----- Need more exercise of ReceiveGenericRequest ------
               try {
                  String rc = ReceiveGenericRequest.getString(maxMsg, 0, 2);
                  System.out.println("ReceiveGenericRequest.getString = " + rc);
               } catch (Exception ex) {
                  System.err.println("Unexpected error with ReceiveGenericRequest.getString");
                  System.err.println(ex.getMessage());
                  System.exit(1);
               }
               System.out.println("Read message " + countRead + " bytes long");
               // print out first 40 bytes of msg or less if msg is smaller
               if (countRead <= 40) {
                  System.out.println("Msg recd: " + new String(maxMsg, 0, countRead));
               } else { 
                  System.out.print("Msg recd: " + new String(maxMsg, 0, 40));
                  //Tack on last byte of msg if msg longer than 40 bytes
                  System.out.println("..." + new String(maxMsg, countRead-1, 1));
               } // end if countRead
            } else {  // it's a system msg
               if (ReceiveGenericRequest.getSystemMessageNumber(maxMsg,ri) == SysMsgOpen.OPEN) {
                  // ----------- Exercise SysMsgOpen --------------------------
                  SysMsgOpen open = new SysMsgOpen(maxMsg);
                  System.out.println("Opener Access Mode = " + open.getAccessMode());
                  System.out.println("Opener Exclusion Mode = " + open.getExclusionMode());
                  System.out.println("Opener nowait depth = " + open.getNowaitDepth());
                  System.out.println("Opener sync depth = " + open.getSyncDepth());
                  System.out.println("Opener Open Options = " + open.getOpenOptions());
                  System.out.println("Opener PAID = " + open.getProcessAccessID());
                  System.out.println("Opener userID verified locally = " + open.isUserIDVerifiedLocally());
                  System.out.println("Opener remote opener = " + open.isRemoteOpener());
                  System.out.println("Opener is backup open = " + open.isBackupOpen());
                  ProcessHandle ph = open.getPrimaryPhandle();
                  if (ph.isNull()) {
                     System.out.println("Opener Primary Phandle is null phandle");
                  } else {
                     System.out.println("Opener Pri Phandle = " + open.getPrimaryPhandle());
                  }
                  System.out.println("Opener open qual = " + open.getOpenQualifier());
                  System.out.println("Opener name = " + open.getOpenerName());
                  System.out.println("Opener pri file num = " + open.getPrimaryFileNumber());
                  System.out.println("Opener CAID = " + open.getCreatorAccessID());
                  System.out.println("Opener hometerm = " + open.getHomeTerminal());
               } // end if open msg
            } // end if not sys msg

            System.out.println("Calling Receive.reply with same data that was read in");
            countReplied = recv.reply(maxMsg, countRead, errorReturn);
            System.out.println("Reply succeeded");
         } catch (ReceiveNoOpeners ex) {
            moreOpeners = false;
            System.out.println("There are no more openers so this server will stop");
         } catch (GuardianIOException ex) {
            System.err.println("Guardian Error on $RECEIVE");
            System.err.println(ex.getMessage());
            System.exit(1);
         } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(1);
         } // end try
      } while(moreOpeners);

   } /* end main */

}  /* end class OpenServer */
