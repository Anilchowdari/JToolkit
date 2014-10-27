/* ------------------------------------------------------------------------
* Filename:     ReceiveInfo.java
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

package com.tandem.ext.guardian;

import com.tandem.ext.util.*;

/**
 * The <code>ReceiveInfo</code> class contains information obtained from a 
 * call to FILE_GETRECEIVEINFO_.  Objects of this class provide information 
 * about a message read from $RECEIVE.  Information the user might find most
 * useful is: 
 * <ul>
 * <li> the maximum number of bytes the sender of the message is expecting 
 * in the reply
 * <li> the process handle of the sender
 * <li> the file number that identifies the file associated with the message in the
 * requesting process
 * <li> for context-sensitive Pathway servers, the dialog information
 * </ul>
 * ReceiveInfo objects also provide
 * a way to uniquely identify a message when replying to a message.
 * <p> A ReceiveInfo object is returned from {@link Receive#getLastMessageInfo 
 * Receive.getLastMessageInfo} for the last message read in.
 *
 * @version 1.0, 8/27/2001
 * @see com.tandem.ext.guardian.Receive
 */
public final class ReceiveInfo  {

   /* load library of native methods */
   static {   
     NativeLoader loader = new NativeLoader();
	 loader.LoadLibrary();

   }

   private short _IOType;
   private int _maxReply; // this is really an unsigned short with max value 57344
   private short _messageTag;
   private short _fileNumber;
   // TBD: this should be a doubleword unsigned int (32 bits unsigned), is it okay to treat as signed?
   private int _syncID;  
   private short _openLabel;
   private ProcessHandle _processHandle;
   private boolean _messageCancelled;
   private boolean _isSystemMsg;
   private short _dialogFlags;  // the whole 16 bits in case new bits become important
   private int _dialogStatus;  // bits .12 and .13 of dialogInfo
   private int _dialogTransMode; // bit .14 of dialogInfo

   /*
    * Constructs a ReceiveInfo.  This is only done by Receive.read so we
    * have package security. 
    *
    */
   ReceiveInfo (short ioType, int maxReply, short msgTag, short fnum, int syncID,
                short[] phandle, short openLabel, short dialogInfo) {
      _IOType = ioType;
      _maxReply = maxReply;
      _messageTag = msgTag;
      _fileNumber = fnum;
      _syncID = syncID;
      _openLabel = openLabel;
      _processHandle = new ProcessHandle(phandle);
      _dialogFlags = dialogInfo;
      _dialogStatus = dialogInfo & 0x000C;  //bits 12 and 13
      _dialogTransMode = dialogInfo & 0x0002;  // bit 14

      if (_IOType == IOTYPE_SYSMSG) {
         _isSystemMsg = true;
      } else {
         _isSystemMsg = false;
      }
   } /* end ctor */

   /**
    * Indicates message is a system message, not a data message.
    * @see #getIOType
    */
   public static final short IOTYPE_SYSMSG = 0;
   /**
    * Indicates sender called WRITE.
    * @see #getIOType
    */
   public static final short IOTYPE_WRITE = 1;
   /**
    * Indicates sender called READ.
    * @see #getIOType
    */
   public static final short IOTYPE_READ = 2;
   /**
    * Indicates sender called WRITEREAD.
    * @see #getIOType
    */
   public static final short IOTYPE_WRITEREAD = 3;

   /**
    * Indicates message is not part of a context sensitive dialog.
    * @see #getDialogStatus
    */
   public static final int DIALOG_NONE = 0;
   /**
    * Indicates message is the start of a new context sensitive dialog.
    * @see #getDialogStatus
    */
   public static final int DIALOG_NEW = 0x0004;
   /**
    * Indicates message is part of a continuing context sensitive dialog.
    * @see #getDialogStatus
    */
   public static final int DIALOG_CONTINUE = 0x0008;
   /**
    * Indicates dialog has been aborted.  No further server-class send operations
    * will be received in this dialog. There is no data buffer associated with
    * this value.
    * @see #getDialogStatus
    */
   public static final int DIALOG_ABORT = 0x000C;

   /**
    * Indicates the requester has started the dialog using the 
    * one-transaction-per-dialog model.  In this model all I/O on the dialog
    * must be performed under the same transaction identifier.
    * @see #getDialogTransMode
    */
   public static final int DIALOG_ONETXMODEL = 0;
   /**
    * Indicates the requester has started the dialog using the 
    * any-transaction-per-dialog model.  In this model each I/O on the dialog
    * may be performed under any transaction identifier.
    * @see #getDialogTransMode
    */
   public static final int DIALOG_ANYTXMODEL = 0x0002;

   /** 
    * A Cancel system message number.
    * @see SysMsgCancel
    */
   public static final short SYSMSG_CANCEL = -38;
   /** 
    * Close system message number.
    * @see SysMsgClose
    */
   public static final short SYSMSG_CLOSE = -104;
   /** 
    * A CPU down system message number.
    * @see SysMsgCpuDown
    */
   public static final short SYSMSG_CPUDOWN = -2;
   /** 
    * A CPU up system message number.
    * @see SysMsgCpuUp
    */
   public static final short SYSMSG_CPUUP = -3;
   /**
    * A dialog abort system message number.  This system message indicates
    * a Pathsend context sensitive dialog was aborted.  There is no data
    * in the system message other than the message number.
    */
   public static final short SYSMSG_DIALOGABORT = -121;
   /**
    * A lost status system message number.  This system message indicates
    * that some broadcast system messages (such as CPU down) were lost 
    * because the process did not read $RECEIVE as fast as the system buffer
    * was filled.  There is no data in the system message other than the message number.
    */
   public static final short SYSMSG_LOSTSTATUS = -13;
   /** 
    * A node down system message number.
    * @see SysMsgNodeDown
    */
   public static final short SYSMSG_NODEDOWN = -110;
   /** 
    * A node up system message number.
    * @see SysMsgNodeUp
    */
   public static final short SYSMSG_NODEUP = -111;
   /**
    * An open system message number.
    * @see SysMsgOpen
    */
   public static final short SYSMSG_OPEN = -103;
   /** 
    * A remote CPU down system message number.
    * @see SysMsgRemoteCpuDown
    */
   public static final short SYSMSG_REMOTECPUDOWN = -100;
   /** 
    * A remote CPU up system message number.
    * @see SysMsgRemoteCpuUp
    */
   public static final short SYSMSG_REMOTECPUUP = -113;
   /**
    * A reset sync system message number.  This system message indicates
    * a requester called RESETSYNC to reset its sync id number.  There is no
    * data in the system message other than the message number.
    */
   public static final short SYSMSG_RESETSYNC = -34;

   /**
    * Indicates the data operation last performed by the message sender.
    *
    * @return one of {@link #IOTYPE_WRITE}, {@link #IOTYPE_READ}, 
    * {@link #IOTYPE_WRITEREAD} or {@link #IOTYPE_SYSMSG}
    */
   public short getIOType() {return _IOType;}

   /**
    * The maximum number of bytes of data the sender will read of the reply.
    * Any bytes in the reply message beyond this value are not read by the sender.
    *
    * @return The maximum number of bytes of data that can be
    * returned by REPLY (as determined by the read count of the sender).
    * @see Receive#reply(byte[],int,ReceiveInfo,int) Receive.reply
    */
   public int getMaxReply() {return _maxReply;}

   /**
    * The value that uniquely identifies the request message.  This is a 
    * synthesized value and is not the real system assigned message tag
    * and therefore can not be directly supplied to Guardian procedure calls
    * such as ACTIVATERECEIVETRANSID.  The message tag will be unique for each message
    * that has not been replied to.  Once a message has been replied to the
    * tag may be re-used for a subsequent message.
    *
    * @return the message tag that uniquely identifies a request
    */
   public short getMessageTag() {return _messageTag;}

   /**
    * Returns the value that identifies the file associated with the message in the
    * requesting process.  If the received message is a system message that is
    * not associated with a specific file open, this value is -1.
    * <p>The file number is used by a server process to identify separate opens by
    * the same requester process.  For context-sensitive Pathway servers it's
    * useful for identifying which dialog was aborted when a dialog abort 
    * system message is received.  The returned file number value is the
    * same as the file number used by the requester to make this request.
    *
    * @return returns the value that identifies the file associated with the 
    * message in the requesting process.
    */
   public short getFileNumber() {return _fileNumber;}

   /**
    * The sync ID associated with this message.  If the received
    * message is a system message, this field is valid only if the message is
    * associated with a specific file open; otherwise this field is not
    * applicable and should be ignored.
    *
    * Each opened process has its own sync ID. Sync IDs are not part of the
    * message data; rather, the receiver of a message obtains the sync ID value
    * associated with a particular message by getSyncID. A
    * file's sync ID is set to 0 when the file is opened and when the RESETSYNC
    * procedure is called for that file (RESETSYNC can be called directly or
    * indirectly through the CHECKMONITOR procedure).  When a request is sent to
    * a process (that is, when a process is the object of a CONTROL,
    * CONTROLBUF, SETMODE, SETPARAM, open, close, read, write, or WRITEREAD
    * operation), the system increments the requester's sync ID just prior to
    * sending the message. (Therefore, a process's first sync ID subsequent to
    * an open has a value of 0.)
    *
    * The sync ID allows the server process (that is, the process reading
    * $RECEIVE) to detect duplicate requests from requester processes.  Such
    * duplicate requests are caused by a backup requester process reexecuting
    * the latest request of a failed primary requester process, or by certain
    * network failures.
    *
    * @return the sync ID assigned by the system for the message
    */
   public int getSyncID() {return _syncID;}

   /**
    * The value assigned by {@link Receive#reply Receive.reply} (when replying to the
    * open system message) to the open on which the received message was sent.
    * If this value is unavailable, -1 is returned.  
    *
    * @return the open label set on reply to the open
    */
   public short getOpenLabel() {return _openLabel;}

   /**
    * Returns the handle of the process that sent the message.
    *
    * @return the sender process handle
    * @see ProcessHandle
    */
   public ProcessHandle getProcessHandle() {return _processHandle;}

   /**
    * Makes the TMF transaction that was included with the message the
    * active transaction for the calling thread.  This happens automatically
    * for the thread that reads in the message.
    * <p>
    * A multithreaded server uses activateRequestTransID either to resume
    * processing for a particular transaction or to properly set the current transaction before
    * replying to a requester. A single-threaded server uses activateRequestTransID
    * to reset the current transaction from the nil state to the transaction identifier of the
    * currently active transaction.
    * <p>
    * com.tandem.tmf.Current.rollback can be used to abort the transaction inherited
    * from a message read from $RECEIVE.  com.tandem.tmf.Current.suspend can be
    * used to suspend the association of the calling thread with its transaction context.
    * In other words, make the calling thread have no transaction associated with it.
    * <p>
    * The underlying TMF procedure ACTIVATERECEIVETRANSID will fail with error 
    * {@link GError#ENOTRANSID} when this method is called and the message
    * read in does <strong>not</strong> include a TMF transaction
    * <p>
    * {@link Receive#setTransInherited} can be used to turn off
    * the inclusion of transaction identifiers.  The default is that a transaction
    * identifier will be included in every message for which there was an active
    * transaction in the requester at the time the message was sent.
    *
    * @exception GuardianException if the underlying ACTIVATERECEIVETRANSID
    * call fails with an error
    */
   public void activateRequestTransID() throws GuardianException {
      int error = activateRequestTransID(_messageTag);
      if (error != GError.EOK) 
         throw new GuardianException(error, "ACTIVATERECEIVETRANSID", "$RECEIVE");
   } // end activateRequestTransID

   /**
    * Indicates whether message has been cancelled.
    *
    * @return true if message has been cancelled, otherwise returns false.
    */
   public boolean isMessageCancelled() {
      if (isMessageCancelled(_messageTag) == 0) {
         return false;
      } else {
         return true;
      }
   } // isMessageCancelled

   /**
    * Indicates whether message is a system message rather than a user message.
    *
    * @return true if message is a system message, otherwise returns false.
    */
   public boolean isSystemMessage() {return _isSystemMsg;}

   /**
    * The system message number or 0 if the message is not a system message.
    *
    * @param msgData the request buffer read from $RECEIVE
    * @return the system message number 
    * @exception DataConversionException if there is a problem converting the first
    * two bytes into a primitive short value
    */
   public short getSystemMessageNumber(byte[] msgData) throws DataConversionException
   {
      if (!isSystemMessage()) return 0;

      return (DataConversion.BIN16ToJavaShort(msgData, 0 /*offset*/));
   } // end getSystemMessageNumber

//getMessageLocale is a possible future enhancement
//returns the locale object which represents the locale of the message

   /**
    * Returns the status of a dialog that was initiated by a requester via a
    * Pathsend procedure call.  This is used by context-sensitive Pathway servers.
    *
    * @return one of {@link #DIALOG_NONE}, {@link #DIALOG_NEW}, 
    * {@link #DIALOG_CONTINUE} or {@link #DIALOG_ABORT}
    */
   public int getDialogStatus() {return _dialogStatus;}

   /**
    * Identifies the transaction model the requester is using for the dialog.
    * The server can abort the dialog, by replying with a reply code of FEEOF,
    * to enforce a level of transaction control that the requester has not
    * specified.
    * @return one of {@link #DIALOG_ONETXMODEL}, {@link #DIALOG_ANYTXMODEL}
    * @see Receive#reply(byte[],int,ReceiveInfo,int) Receive.reply
    */
   public int getDialogTransMode() {return _dialogTransMode;}

   // Native Methods
   private native int activateRequestTransID(short msgTag);
   private native int isMessageCancelled(short msgTag);
   

}  /* end class ReceiveInfo */
