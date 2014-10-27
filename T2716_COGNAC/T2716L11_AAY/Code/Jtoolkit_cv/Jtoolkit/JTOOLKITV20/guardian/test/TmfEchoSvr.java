/* ------------------------------------------------------------------------
* Filename:     TmfEchoSvr.java
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
 * The <code>TmfEchoSvr</code> class contains code to test JPathway functionality.
 * This is a simple single-threaded echo server that expects the messages
 * received to include TMF TX IDs.  An echo server is a server
 * that replies with the same data that was sent to it and does not do database work.
 */
public class TmfEchoSvr {

   /**
    * Constructs a TmfEchoSvr 
    */
   public TmfEchoSvr () {
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
      byte[] maxMsg = new byte[2000];  // holds largest possible request
      short errorReturn = FileSystemErrors.FEOK;
    
      // open $Receive
      try {
         recv = Receive.getInstance();
         System.out.println("Opening $RECEIVE with everything defaulting");
         recv.open();
      } catch (GuardianIOException ex) {
         System.err.println("FATAL ERROR: Unable to open $RECEIVE");
         System.err.println(ex.getMessage());
         System.exit(1);
      }
    
      do { // read messages off $receive until last close
         try {
            System.out.println("Calling Receive.read");
            countRead = recv.read(maxMsg, maxMsg.length);
            System.out.println("Read message " + countRead + " bytes long");
            // print out first 40 bytes of msg or less if msg is smaller
            System.out.println("Msg recd: " + new String(maxMsg, 0, countRead < 40 ? countRead : 40));
            System.out.println("Calling Receive.getLastMessageInfo");
            // -------------Exercise ReceiveInfo class--------------
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
            System.out.println("Calling ReceiveInfo.activateRequestTransID");
            ri.activateRequestTransID();
            // Exercise TMF changes
            System.out.println("Calling Current.Suspend twice in a row");
            Current.Suspend();
            Current.Suspend();
            System.out.println("Re-activating the receive trans id");
            ri.activateRequestTransID();
            System.out.println("Aborting the inherited tx");
            Current.Rollback();
            System.out.println("Making sure can re-activate aborted inherited tx");
            ri.activateRequestTransID();
            // End Exercise TMF changes
            // ----------End Exercise of ReceiveInfo----------------------

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
         } // end try
      } while(moreOpeners);

   } /* end main */

}  /* end class TmfEchoSvr */
