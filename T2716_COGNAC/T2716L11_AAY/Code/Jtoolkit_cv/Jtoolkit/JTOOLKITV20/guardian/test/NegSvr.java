/* ------------------------------------------------------------------------
* Filename:     NegSvr.java
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
 * The <code>NegSvr</code> class contains code to test JPathway functionality.
 * This is a simple single-threaded server that does some negative tests.
 */
public class NegSvr {

   /**
    * Constructs a NegSvr 
    */
   public NegSvr () {
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
         System.out.println("Try changing options that can only be changed when $RECEIVE is closed");
         System.out.println("Trying to open $RECEIVE twice, this should be a no-op I think");
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
            ri = recv.getLastMessageInfo();

            System.out.println("Try reading again before reply");
            countRead = recv.read(maxMsg, maxMsg.length);
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

}  /* end class NegSvr */
