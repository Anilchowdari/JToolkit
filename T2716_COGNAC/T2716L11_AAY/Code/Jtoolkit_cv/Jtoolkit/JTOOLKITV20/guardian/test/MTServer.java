/* ------------------------------------------------------------------------
* Filename:     MTServer.java
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
 * The <code>MTServer</code> class contains code to test JPathway functionality.
 * This is a simple multi-threaded echo server that starts a 2nd thread to
 * verify that only one outstanding readupdate is allowed at a time.
 * This 2nd read should fail with error 28.  Then it tests that the
 * thread with outstanding I/O is woken up with a cancel 
 * when $RECEIVE is closed.
 */
public class MTServer implements Runnable {

   private static Receive recv = null;
   private static final int MAXCOUNT = 57344;
   private static boolean firstMsg = false; // set to true after read 1st msg

   /**
    * Constructs a MTServer 
    */
   public MTServer () {
      super();
   } /* end ctor */
 
   /* this is the code that executes in the 2nd thread */
   final public void run() {
      ReceiveInfo ri = null;
      ProcessHandle phandle = null;
      int countRead; // number of bytes read
      int countReplied; // number of bytes replied
      boolean moreOpeners = true;
      byte[] maxMsg = new byte[MAXCOUNT];  // holds largest possible request
      short errorReturn = FileSystemErrors.FEOK;
      boolean readOutstanding = false;
 
      // wait for 1st thread to get its first message which means someone opened
      // $RECEIVE
      while (!firstMsg) {
         try {Thread.sleep(2000);} catch (Exception exc) {}
      }
 
      System.out.println("In 2nd Thread");
      do { // read messages off $receive until last close
         try {
            if (readOutstanding) {
               readOutstanding = false;
               System.out.println("2nd: read outstanding so closing $RECEIVE");
               // this does a cancel as well as a close
               recv.close();
               return;
            }
            System.out.println("2nd Calling Receive.read");
            countRead = recv.read(maxMsg, maxMsg.length);
            System.out.println("2nd Read message " + countRead + " bytes long");
            // print out first 40 bytes of msg or less if msg is smaller
            //Tack on last byte of msg if msg longer than 40 bytes
            if (countRead <=40 ) {
               System.out.println("2nd Msg recd: " + new String(maxMsg, 0, countRead));
            } else {
               System.out.print("2nd Msg recd: " + new String(maxMsg, 0, 40));
               System.out.println("..." + new String(maxMsg, countRead-1, 1));
            }
            System.out.println("2nd Calling Receive.reply with same data that was read in");
            countReplied = recv.reply(maxMsg, countRead, errorReturn);
            System.out.println("2nd Reply succeeded");
         } catch (ReceiveNoOpeners ex) {
            moreOpeners = false;
            System.out.println("2nd There are no more openers so this thread will stop");
         } catch (GuardianIOException ex) {
            System.err.println("2nd Guardian Error on $RECEIVE");
            System.err.println(ex.getMessage());
//            System.exit(1);
            readOutstanding = true;
//            try {Thread.sleep(2000);} catch (Exception exc) {}
         } catch (Exception ex) {
            System.err.println("2nd Unexpected Exception");
            ex.printStackTrace();
            return;
         } // end try
      } while(moreOpeners);
 
   } /* end run */

   public static void main (String args [] )
   {
      // As soon as we're started we open $RECEIVE and wait for messages
      ReceiveInfo ri = null;
      ProcessHandle phandle = null;
      int countRead; // number of bytes read
      int countReplied; // number of bytes replied
      boolean moreOpeners = true;
      byte[] maxMsg = new byte[MAXCOUNT];  // holds largest possible request
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
    
      try {
         MTServer g = new MTServer();
         new Thread(g).start();
      } catch (Exception e) {}

      do { // read messages off $receive until last close
         try {
            System.out.println("Calling Receive.read");
            countRead = recv.read(maxMsg, maxMsg.length);
            firstMsg = true;
            System.out.println("Read message " + countRead + " bytes long");
            // print out first 40 bytes of msg or less if msg is smaller
            //Tack on last byte of msg if msg longer than 40 bytes
            if (countRead <=40 ) {
               System.out.println("Msg recd: " + new String(maxMsg, 0, countRead));
            } else { 
               System.out.print("Msg recd: " + new String(maxMsg, 0, 40));
               System.out.println("..." + new String(maxMsg, countRead-1, 1));
            }
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

}  /* end class MTServer */
