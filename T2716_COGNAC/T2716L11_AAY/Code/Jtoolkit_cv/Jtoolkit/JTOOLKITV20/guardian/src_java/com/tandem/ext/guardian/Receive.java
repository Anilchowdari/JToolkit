/* ------------------------------------------------------------------------
 * Filename:     Receive.java
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

import com.tandem.ext.util.DataConversionException;
import com.tandem.ext.util.PathsendModeType;
import com.tandem.ext.util.*;

/**
 * The <code>Receive</code> class is a Singleton class which contains methods for
 * process to process communication using the NonStop Himalaya platform's 
 * message based $RECEIVE functionality.
 * <p>
 * This class opens $RECEIVE for exclusive access.  If this class opens
 * $RECEIVE, no other class in your program can open it.  If something else in
 * your program opens $RECEIVE this class will not be able to open $RECEIVE
 * because it requires exclusive access.
 * <p>
 * $RECEIVE is like one end of a communications conduit.  It is similar to a
 * courier who brings you a message and can be dismissed or sent back with a
 * reply.  To receive a message from another process (including the operating
 * environment), a process must read $RECEIVE.  To reply to a message, a
 * process must write to $RECEIVE.  The operating environment delivers the
 * reply to the originator of the message.  The process does not need to
 * determine where the message came from to reply to it.  As with any other
 * read operation, a thread waiting to receive a message on $RECEIVE waits
 * until a message is delivered.  Before a process can receive a message, some
 * process must have sent a message to it.  To send a message, a process must
 * have the name of the process that is to receive the message.  The sending
 * process opens a file  having the same name as the receiving process.  Then
 * the sending process can write to its file and cause the operating
 * environment to deliver the message to the receiving process.  The receiving
 * process then can read the message on its $RECEIVE.
 *
 * <pre>
 *     Receive recv = null;
 *     ReceiveInfo ri = null;
 *     int countRead; // number of bytes read
 *     boolean moreOpeners = true;
 *     byte[] maxMsg = new byte[2000];  // holds largest possible request
 *     IAccount acct = new IAccount(); // ddl2java generated class
 *     IPerson person = new IPerson(); // ddl2java generated class
 *     AcctReply aReply;  // ddl2java generated class
 *     PersonReply pReply; // ddl2java generated class
 *     ErrorReply eReply = new ErrorReply(); // ddl2java generated class
 *     eReply.setReplyCode(99);
 *     int errorReturn = GError.EOK;
 *
 *     // open $Receive
 *     try {
 *        recv = Receive.getInstance();
 *        recv.open();
 *     } catch (GuardianException ex) {
 *        System.out.println("Unable to open $RECEIVE");
 *        System.out.println(ex.getMessage());
 *        System.exit(1);
 *     }
 *
 *     do { 
 *        // read messages off $receive into a byte array until no
 *        // more processes have this server open
 *        try {
 *           countRead = recv.read(maxMsg, maxMsg.length);
 *           // get information about message and sender
 *           ri = recv.getLastMessageInfo();
 *           short requestCode = DataConversion.BIN16ToJavaShort(maxMsg, 0);
 *           switch (requestCode) {
 *           case 1:  // add new account request
 *              acct.unmarshal(maxMsg, countRead);
 *              aReply = addAccount(acct, ri);
 *              recv.reply(aReply, errorReturn);
 *              break;
 *           case 2: // add new person request
 *              person.unmarshal(maxMsg, countRead);
 *              pReply = addPerson(person, ri);
 *              recv.reply(pReply, errorReturn);
 *              break;
 *           default: // reply with user defined error to handle unknown request
 *              eReply.setMsg("Unsupported request" + rc);
 *              recv.reply(eReply, errorReturn);
 *           } // end switch
 *        } catch (ReceiveNoOpeners ex) {
 *           moreOpeners = false;
 *        } catch (com.tandem.ext.util.DataConversionException ex) {
 *           eReply.setMsg(ex.getMessage());
 *           recv.reply(eReply, errorReturn);
 *        } catch (GuardianException ex) {
 *           System.out.println("Unexpected error on $RECEIVE");
 *           System.out.println(ex.getMessage());
 *           System.exit(1);
 *        } // end try
 *     } while(moreOpeners);
 *       . . .
 * </pre>
 *
 * @version 1.0, 8/2/2001
 * @see com.tandem.ext.guardian.ReceiveInfo
 */
public final class Receive {

	/* load library of native methods */
	static {
		NativeLoader loader = new NativeLoader();
		loader.LoadLibrary(); 
	}

	private static final Receive INSTANCE_ = new Receive();
	// initial space is allocated for this many openers, an arbitrary number
	private short initialOpeners_ = 20;

	// set to Guardian file number when $RECEIVE is open, reset to -1 when closed
	private short filenum_ = -1;
	private String name_ = "$RECEIVE";

	// set to true when $RECEIVE is open, false when closed
	private boolean isOpen_ = false;

	//changes begin for TSMP 2.4

	// set to TSMP_PATHSEND_2MB_LIMIT when $RECEIVE is opened for large pathsend(up to 2 MB), TSMP_PATHSEND_32KB_LIMIT for pathsend up to 32 KB
	private int pathSendMode = PathsendModeType.TSMP_PATHSEND_32KB_LIMIT;

	//changes end for TSMP 2.4

	// user sets this value, default is 1, indicates how many readupdates allowed without a reply
	private int receiveDepth_ = 1;

	// bit mask which controls which system messages the user sees, set by the user
	private long sysMsgMask_ = 0L;

	// flag to determine if user wants to be notified when no more openers
	private boolean notifyNoOpeners_ = true;

	// user sets this value, default is 0
	private int syncDepth_ = 0;

	// controls whether messages are read in priority order, users sets this value, default is false
	private boolean priorityQueuing_ = false;

	// the maximum reply length for saved replies, only applies if syncDepth is > 0
	// user sets this value, default is 0
	private int maxReplyLen_ = 0;

	// controls whether messages are to include their transaction identifiers
	// user sets this value, default is that tids are included
	private boolean tidIncluded_ = true; // options.<12> = 0

	// Controls whether messages include internationalization locale information
	// user sets this value, default is locale info is not included
	// CURRENTLY NOT USED, possible future enhancement 
	//private boolean i18nLocaleSupport_ = false;   // options.<13> = 0

	// This holds the ReceiveInfo object corresponding to the last successful
	// message read in
	private ReceiveInfo lastRecvInfo_;

	// msgsOutstanding_ keeps a count of how many messages have been read and
	// not yet replied to.  The thread doing a read must wait until a reply is 
	// done when this value equals receiveDepth_.
	private int msgsOutstanding_ = 0;

	// readThreadWaiting_ is set to true when a thread trying to read has to wait
	// for a reply because we are already at the max number of msgs outstanding.
	private boolean readThreadWaiting_ = false;

	// readMonitor_ is used to wait and notify a thread when the number of msgs
	// outstanding is at the maximum.
	private Object readMonitor_ = null;

	// readHasBeenCancelled_ is used to keep track of when a read that was waiting
	// to be performed was cancelled.
	private boolean readHasBeenCancelled_ = false;

	/**
	 * Use to turn on receipt of CPU down system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_LOCALCPUDOWN = 0x0000000040000000L;
	/**
	 * Use to turn on receipt of CPU up system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_LOCALCPUUP = 0x0000000020000000L;
	/**
	 * Use to turn on receipt of lost broadcast system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_LOSTSTATUS = 0x0000000000400000L;
	/**
	 * Use to turn on receipt of resetsync system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_RESETSYNC = 0x0000000000000800L;
	/**
	 * Use to turn on receipt of cancel system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_CANCEL = 0x0000000000000080L;
	/**
	 * Use to turn on receipt of remote CPU down system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_REMOTECPUDOWN = 0x8000000000000000L;
	/**
	 * Use to turn on receipt of remote CPU up system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_REMOTECPUUP = 0x0004000000000000L;
	/**
	 * Use to turn on receipt of open system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_OPEN = 0x1000000000000000L;
	/**
	 * Use to turn on receipt of close system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_CLOSE = 0x0800000000000000L;
	/**
	 * Use to turn on receipt of node down system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_NODEDOWN = 0x0020000000000000L;
	/**
	 * Use to turn on receipt of node up system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_NODEUP = 0x0010000000000000L;
	/**
	 * Use to turn on receipt of abort dialog system messages.
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public static final long SMM_ABORTDIALOG = 0x0000040000000000L;

	/* Unused bits in system message mask */
	private static final long SMM_UNUSEDBITS = ~(SMM_LOCALCPUDOWN
			| SMM_LOCALCPUUP | SMM_LOSTSTATUS | SMM_RESETSYNC | SMM_CANCEL
			| SMM_REMOTECPUDOWN | SMM_REMOTECPUUP | SMM_OPEN | SMM_CLOSE
			| SMM_NODEDOWN | SMM_NODEUP | SMM_ABORTDIALOG);

	// Private constructor supresses default public constructor
	private Receive() {
		filenum_ = -1;
		name_ = "$RECEIVE";
		isOpen_ = false; // set to true when file is open, reset when file closed

		// Possible future support
		//i18nLocaleSupport__ = false; // default is no locale information
		receiveDepth_ = 1; // default is 1 concurrent message before reply is required
		sysMsgMask_ = 0; // default is user receives no system messages
		syncDepth_ = 0; // default is no saved replies
		priorityQueuing_ = false; // default is messages are queue in order they are rec'd
		maxReplyLen_ = 0; // default is no replies are saved
		readMonitor_ = new Object();
	}

	/**
	 * Returns the single instance of Receive.
	 *
	 * @return  the Receive singleton object
	 */
	public static Receive getInstance() {
		return INSTANCE_;
	}

	/**
	 * Indicates whether $RECEIVE is open.
	 *
	 * @return true if $RECEIVE is open otherwise returns false
	 */
	public boolean isOpen() {
		return isOpen_;
	}

	/*
	 * Returns system assigned file number associated with the open $RECEIVE.
	 * @return file number of open file
	 */
	//public int getFileNumber() {return filenum_;}
	/*
	 * Indicates whether locale information representing the 
	 * client's locale will be created for each user message.
	 *
	 * @return true if locale information will be created for each user message,
	 * otherwise returns false
	 * @see #setLocaleSupport setLocaleSupport
	 */
	// not currently used, possible future enhancement
	//public boolean isLocaleSupported() {return i18nLocaleSupport_;}
	/*
	 * Turns on or off receipt of locale information for client messages.
	 * Must be called when $RECEIVE is closed.  The default is no locale information.
	 *
	 * @param value true to turn on locale information, false to turn off
	 * @exception IllegalStateException when $RECEIVE is open
	 */
	/* not currently used, possible future enhancement
	 public void setLocaleSupport(boolean value) throws IllegalStateException {
	 if (isOpen()) {
	 throw new IllegalStateException("Cannot change locale support while $RECEIVE is open");
	 }
	 i18nLocaleSupport_ = value;
	 } 
	 */// end setLocaleSupport
	/**
	 * Returns important information about the last message successfully read.  This method
	 * must be called before another call to {@link #read read} is made because
	 * each successful read updates this information.  A {@link ReceiveInfo} 
	 * object is needed to reply to the message if receive depth is greater than 1.
	 * A successful call to {@link #reply reply} will reset this value to null.
	 *
	 * @return the ReceiveInfo object associated with the last successful message read or
	 * null if a read has never been done or reply was called after the last read
	 * @see ReceiveInfo
	 */
	public ReceiveInfo getLastMessageInfo() {
		return lastRecvInfo_;
	}

	/**
	 * Returns the number of messages that can be worked on concurrently.  In
	 * other words it returns the number of requests that can be read in without
	 * a reply.
	 *
	 * @return the maximum number of concurrent messages
	 * @see #setReceiveDepth setReceiveDepth
	 */
	public int getReceiveDepth() {
		return receiveDepth_;
	}

	/**
	 * Sets the number of messages that can be worked on concurrently.  
	 * The receiveDepth parameter specifies how many unreplied-to requests the
	 * server can have outstanding at one time.
	 * <p>If set to 0, reply operations are not permitted.  Sync depth and 
	 * maximum reply length values are ignored if receivedepth = 0.
	 * If receiveDepth is 1
	 * and the user does not call reply before the next read then there will be an 
	 * automatic reply of a zero length message before the read is performed.
	 * If receiveDepth is greater than 1 the server must reply to all messages
	 * received.  
	 * <p>The value must be in the range 0 through 4047.  The default value is 1.
	 * The value can not be changed when $RECEIVE is open.
	 * <p>In the Pathway environment, the value of the LINKDEPTH parameter in
	 * the server-class definition for multithreaded servers should be less than
	 * or equal to the receiveDepth value.  Note that
	 * the LINKDEPTH value must be 1 in all other cases.  For single-threaded 
	 * servers, a LINKDEPTH value greater than 1 disturbs the automatic load 
	 * balancing feature in Pathway.
	 *
	 * @param receiveDepth the maximum number of concurrent messages
	 * @exception IllegalStateException if invoked when $RECEIVE is open 
	 * @exception IllegalArgumentException if parameter is less than 0 or more
	 * than 4047.
	 */
	public void setReceiveDepth(int receiveDepth) throws IllegalStateException,
			IllegalArgumentException {
		if (isOpen()) {
			throw new IllegalStateException(
					"Cannot change receive depth while $RECEIVE is open");
		}
		if (receiveDepth < 0 || receiveDepth > 4047) {
			throw new IllegalArgumentException(
					"Receive depth must be in the range 0 through 4047 inclusive.");
		}

		receiveDepth_ = receiveDepth;
	} // end setReceiveDepth

	/**
	 * Returns the value of the bit mask which indicates which system 
	 * messages the user receives.
	 * <p> Use this method in conjunction with {@link #SMM_LOCALCPUDOWN system message}
	 * mask values to determine which system messages will be received by the user.
	 *
	 * @return the system message bit mask
	 * @see #setSystemMessageMask setSystemMessageMask
	 */
	public long getSystemMessageMask() {
		return sysMsgMask_;
	}

	/**
	 * Sets which system messages the user will receive.  The default is no
	 * system messages.  The JVM receives all system 
	 * messages but returns to the user only those messages that are specified
	 * using this method.  This value can be set at any time, before or after $RECEIVE is
	 * opened.
	 * <p>
	 * Use {@link #SMM_OPEN} and other bit mask constants to set the bit
	 * mask value.  Bits which have no current meaning must be 0.
	 * For example, to receive open and close messages do the following: <pre><code>
	 * setSystemMessageMask(Receive.SMM_OPEN | Receive.SMM_CLOSE);
	 * </code></pre>
	 * <br>To later turn off receipt of all system messages do the following:<pre><code>
	 * setSystemMessageMask(0L);
	 * </code></pre>
	 *
	 * @param sysMsgMask bit mask value 
	 * @exception IllegalArgumentException if unused bits are not 0
	 */
	public void setSystemMessageMask(long sysMsgMask)
			throws IllegalArgumentException {
		// verify unused bits are 0, this is important otherwise user might get a system
		// message they don't expect because underlying tmrqst module supports
		// more system messages that this class currently does.
		if ((sysMsgMask & SMM_UNUSEDBITS) != (long) 0) {
			throw new IllegalArgumentException(
					"Illegal system message mask: 0x"
							+ Long.toHexString(sysMsgMask));
		}

		// if $RECEIVE not open we'll set the system mask when we open $RECEIVE
		// otherwise we set the mask here
		if (isOpen_) {
			setSysMsgMask(sysMsgMask);
		}

		sysMsgMask_ = sysMsgMask;
	} // end setSystemMessageMask

	/**
	 * Indicates if a user will be notified, via a ReceiveNoOpeners exception,
	 * when there are no more openers.
	 *
	 * @return true if user will be notified, otherwise false
	 */
	public boolean isNotifyNoMoreOpeners() {
		return notifyNoOpeners_;
	}

	/** 
	 * Controls whether user is thrown a ReceiveNoOpeners exception when one of the
	 * read methods is called and there are no more openers.  The default is true.
	 * It is recommended that Pathway servers keep this value as true because
	 * a properly coded Pathway server is expected to stop when the last opener
	 * closes it.  This method can be called when $RECEIVE is open or closed.
	 * When this value is false a read method will wait for the next message
	 * rather than throw a ReceiveNoOpeners exception.
	 *
	 * @see #read(byte[], int)
	 * @see #read(GuardianInput)
	 * @param value true if you want the read methods to throw ReceiveNoOpeners
	 * exception, false if you want the read methods to wait for the next message
	 * when there are no more openers.
	 */
	public void setNotifyNoMoreOpeners(boolean value) {
		notifyNoOpeners_ = value;
	}

	/**
	 * Returns the maximum number of saved replies per requester.  
	 * Saved replies are used to reply to messages from a NonStop process pair after a
	 * takeover by the backup process.
	 *
	 * @return returns the maximum number of saved repies per requester
	 * @see #setSyncDepth setSyncDepth
	 */
	public int getSyncDepth() {
		return syncDepth_;
	}

	/**
	 * Sets the maximum number of saved replies per opener.  
	 * Replies are saved for requesters who have opened the server with a sync 
	 * depth greater than zero.  Only the most recent replies
	 * to each requester are saved.  The syncDepth value is ignored if
	 * receiveDepth is set to zero.  If sync depth is zero replies are not saved.
	 * The default is zero.  If the server is inheriting TMF transactions the sync
	 * depth should be zero.  A Pathway server never needs a sync depth value 
	 * greater than one.
	 * <p>
	 * Saved replies are used to reply to duplicate requests that have been resent
	 * due to network communication errors if the requester is remote or
	 * due to a takeover if the requester is a process pair.
	 *
	 * @param syncDepth the number of saved replies per requester
	 * @exception IllegalStateException if invoked when $RECEIVE is open 
	 * @see #setMaxReplyLength setMaxReplyLength
	 * @see #setReceiveDepth setReceiveDepth
	 */
	public void setSyncDepth(int syncDepth) throws IllegalStateException {
		if (isOpen()) {
			throw new IllegalStateException(
					"Cannot change syncdepth while $RECEIVE is open");
		}
		if (syncDepth < 0 || syncDepth > 255) {
			throw new IllegalArgumentException(
					"Syncdepth must be in the range 0 through 255");
		}

		syncDepth_ = syncDepth;
	} // end setSyncDepth

	/**
	 * Indicates whether $RECEIVE queue is reordered according to the priority 
	 * of the sending process.
	 *
	 * @return true if $RECEIVE queue is in sender priority order,
	 * otherwise returns false
	 * @see #setPriorityQueue setPriorityQueue
	 */
	public boolean isPriorityQueue() {
		return priorityQueuing_;
	}

	/**
	 * Turns on or off turn on/off message queuing by priority of the sending
	 * process.  The default is false, meaning that messages are
	 * queued in the order in which they arrive.  This value can be set any time
	 * after $RECEIVE has been opened.
	 *
	 * @param value true to turn on priority queuing, false to turn off
	 * @exception GuardianException if underlying Guardian SETMODE call fails
	 */
	public void setPriorityQueue(boolean value) throws GuardianException {
		short param1 = 0;

		if (!isOpen_) {
			throw new IllegalStateException(
					"Cannot set priority queuing unless $RECEIVE is open");
		}

		if (priorityQueuing_ != value) {
			if (value)
				param1 = 1;
			int error = setPriorityQueue(param1);
			if (error != GError.EOK) {
				throw new GuardianException(error, "SETMODE", name_);
			}
			priorityQueuing_ = value;
		}
	} // end setPriorityQueue

	/**
	 * Returns the maximum number of bytes allowed for a reply message.
	 *
	 * @return the maximum length of a reply message
	 * @see #setMaxReplyLength setMaxReplyLength
	 */
	public int getMaxReplyLength() {
		return maxReplyLen_;
	}

	/**
	 * Specifies the maximum size of each reply.  This value is ignored if the
	 * sync depth is set to 0.  The default is 0.  The value can not be changed
	 * when $RECEIVE is open.  The maxReplyLength value specifies
	 * how much space will be allocated for each saved reply.  The total space
	 * allocated is syncDepth*maxReplyLength per opener.  The space is
	 * allocated when a requester opens the server.
	 *
	 * @param maxReplyLen the maximum length of any reply message
	 * @exception IllegalStateException if invoked when $RECEIVE is open 
	 * @exception IllegalArgumentException if parameter is less than 0 or more
	 * than 57344 when $Receive is opened with open() or if parameter is less than 0 or more
	 * than 2 MB when $Receive is opened with openWide() .
	 * @see #setSyncDepth setSyncDepth
	 */
	public void setMaxReplyLength(int maxReplyLen)
			throws IllegalStateException, IllegalArgumentException {
		if (isOpen()) {
			throw new IllegalStateException(
					"Cannot set maximum reply length while $RECEIVE is open");
		}
		if (pathSendMode == PathsendModeType.TSMP_PATHSEND_32KB_LIMIT) {
			if (maxReplyLen < 0 || maxReplyLen > 57344) {
				throw new IllegalArgumentException(
						"Maximum saved reply length must be in the range 0 through 57344");

			}
		} else {
			if (maxReplyLen < 0 || maxReplyLen > 2097152) {
				throw new IllegalArgumentException(
						"Maximum saved reply length must be in the range 0 through 2097152");

			}
		}

		maxReplyLen_ = maxReplyLen;
	} // end setMaxReplyLength

	/**
	 * Indicates whether messages read from $RECEIVE are to include transaction
	 * identifiers.
	 *
	 * @return true if transaction identifiers are included with messages
	 * read from $RECEIVE otherwise returns false
	 * @see #setTransInherited setTransInherited
	 */
	public boolean isTransInherited() {
		return tidIncluded_;
	}

	/**
	 * Determines whether messages read from $RECEIVE are to include transaction
	 * identifiers.  The default is true, messages will include 
	 * their transaction identifiers.  Set this to false if you don't want 
	 * messages to include their transaction identifier.  This value can only
	 * be set when $RECEIVE is closed.
	 *
	 * @param value true to include transaction identifiers, false to not include them
	 * @exception IllegalStateException when $RECEIVE is open
	 * @see #isTransInherited
	 */
	public void setTransInherited(boolean tidIncluded)
			throws IllegalStateException {
		if (isOpen()) {
			throw new IllegalStateException(
					"Cannot change whether transaction identifiers are included with messages while $RECEIVE is open");
		}
		tidIncluded_ = tidIncluded;
	}

	/**
	 * Cancels the last read operation on $RECEIVE.
	 * The canceled operation might or might not have had effects.  If the call to cancel
	 * was able to cancel the outstanding read before it completed, the thread
	 * that performed the read will get a GuardianException with a file system
	 * error of {@link GError#EREQABANDONED}.
	 *
	 * @exception GuardianException when the underlying Guardian CANCEL call fails
	 */
	public void cancelRead() throws GuardianException {
		int error = GError.EOK;

		if (isOpen_) {
			if (readThreadWaiting_) {
				// We don't want to call the native cancel method because there is 
				// no pending readUpdateX call, instead the thread is waiting for a
				// reply to occur before it can issue the read.  Therefore to cancel
				// the pending read we just need to wake up the thread.
				synchronized (readMonitor_) {
					readHasBeenCancelled_ = true;
					try {
						readMonitor_.notify();
					} catch (IllegalMonitorStateException ex) {
						throw new RuntimeException(
								"Internal Error: got IllegalMonitorStateException on call to notify");
					}
				}
			} else {
				error = cancel();
			}
		} else {
			error = GError.ENOTOPEN;
		}
		if (error != GError.EOK)
			throw new GuardianException(error, "CANCEL", name_);
	}

	/**
	 * Closes $RECEIVE.  The program will no longer be able to read incoming
	 * messages unless $RECEIVE is re-opened.  If $RECEIVE is already closed
	 * this method will have no effect.  If there is a read outstanding on another
	 * thread that read will be cancelled.
	 *
	 * @exception GuardianException when the underlying Guardian FILE_CLOSE procedure
	 *            fails
	 */
	public void close() throws GuardianException {
		if (!isOpen_) {
			return;
		}

		if (readThreadWaiting_) {
			// we need to wake up the thread that is waiting to perform a read
			synchronized (readMonitor_) {
				readHasBeenCancelled_ = true;
				try {
					readMonitor_.notify();
				} catch (IllegalMonitorStateException ex) {
					throw new RuntimeException(
							"Internal Error: got IllegalMonitorStateException on call to notify");
				}
			}
		}

		int result = closeReceive(pathSendMode);
		// no error is retryable
		isOpen_ = false;
		pathSendMode = PathsendModeType.TSMP_PATHSEND_32KB_LIMIT;
		filenum_ = -1;

		if (result != GError.EOK) {
			throw new GuardianException(result, "FILE_CLOSE_", name_);
		}
	} // end close

	/**
	 * Opens $RECEIVE so messages can be read.  If $RECEIVE is already open
	 * this method has no effect. If $RECEIVE is already opened with open(), 
	 * it should  be closed to open it with open().
	 * @exception GuardianException when the underlying Guardian FILE_OPEN_ call
	 *            fails
	 * @exception IllegalAccessException  if $Receive is already opened with openWide()      
	 * @see #setReceiveDepth setReceiveDepth
	 * @see #setTransInherited setTransInherited
	 */
	public void open() throws GuardianException, IllegalAccessException {
		//Begin changes for sol:10-090425-1139 

		if (pathSendMode == PathsendModeType.TSMP_PATHSEND_2MB_LIMIT
				&& isOpen_ == true)
			throw new IllegalAccessException(
					"$Receive is already opened with openWide(), it  should be closed to open it with open()");
		if (isOpen_) {
			return;
		}
		//End changes for sol:10-090425-1139 

		// Setup open options
		short options = 0;
		if (!tidIncluded_) {
			options |= 0x0008; // turn on bit 12
		}
		//Possible future enhancement...
		//if (i18nLocaleSupport_) {
		//   options |= 0x0004; // turn on bit 13
		//} 

		//changes begin for TSMP 2.4

		int result = openReceive(initialOpeners_, (short) receiveDepth_,
				(short) syncDepth_, maxReplyLen_, options, sysMsgMask_,
				pathSendMode);

		if (result != GError.EOK) {
			throw new GuardianException(result, "FILE_OPEN_", name_);
		}

		isOpen_ = true;
		filenum_ = 0;
	} // end open

	//changes begin for TSMP 2.4
	/**
	 * Opens $RECEIVE for reading and replying messages up to 2 MB.  If $RECEIVE 
	 * is already open this method has no effect. If $RECEIVE is already opened with 
	 * open(), it should  be closed  to open it with openWide().
	 *
	 * @exception GuardianException when the underlying Guardian FILE_OPEN_ call
	 *            fails
	 * @exception IllegalAccessException  if $Receive is already opened with open()
	 * @see #setReceiveDepth setReceiveDepth
	 * @see #setTransInherited setTransInherited
	 */

	public void openWide() throws GuardianException, IllegalAccessException {
		
		if (pathSendMode == PathsendModeType.TSMP_PATHSEND_32KB_LIMIT
				&& isOpen_ == true)
			throw new IllegalAccessException(
					"$Receive is already opened with open(), it  should be closed to open it with openWide()");
		if (isOpen_) {
			return;
		}
		pathSendMode = PathsendModeType.TSMP_PATHSEND_2MB_LIMIT;
		open();
	} // end openWide

	//changes end for TSMP 2.4

	/**
	 * Reads a message from $RECEIVE.  There can only be one read operation 
	 * outstanding at a time.  Any calls to read while another thread is blocked
	 * on a read operation will fail with a GuardianException.  If a read is 
	 * performed when there are <code>receive-depth</code> number of 
	 * requests that have not been replied to, the thread calling  <code>read</code>
	 * will suspend until another thread performs a <code>reply</code> or cancels the read.
	 *
	 * <p>If the user has specified that messages 
	 * read from $RECEIVE are to include transaction identifiers (the default
	 * behavior) the current thread will automatically inherit the transaction of
	 * the message if one exists.  This means disk I/O using the current
	 * thread will be TMF protected if the message has a transaction identifier
	 * associated with it and the disk file is TMF audited.  
	 * If another thread is to perform work on behalf of
	 * the message, the other thread should call {@link 
	 * ReceiveInfo#activateRequestTransID ReceiveInfo.activateRequestTransID}
	 * to make the transaction active for that thread.  The server process
	 * can participate in the inherited transaction until the user replies to the
	 * message or aborts the transaction.
	 * <p>
	 * The message read in might be a system message 
	 * if the user has asked to receive system messages
	 * (see {@link #setSystemMessageMask setSystemMessageMask}).  The default is
	 * the user receives no system messages.
	 * This read method synthesizes CANCEL, dialog abort, or CLOSE system messages if
	 * it receives CLOSE, cpu down, or node down system messages that indicate 
	 * terminated connections with the server, and if the server has selected 
	 * to receive the CANCEL, dialog abort or CLOSE system messages.
	 * The advantage of synthesized system messages is that, for example, 
	 * the user does not have to figure out if there are lost openers 
	 * due to a node down system message.  If more than one selected,
	 * synthesizable system message applies, this method synthesizes only the most
	 * comprehensive message (CLOSE is more comprehensive than dialog abort
	 * which is more comprehensive than CANCEL).
	 * <p>
	 * If this method is called when the server has no more openers a ReceiveNoOpeners
	 * exception will be thrown unless {@link #setNotifyNoMoreOpeners} has been set to 
	 * <code>false</code>.  A properly coded Pathway server should stop itself
	 * after receiving this exception.  If the user has selected
	 * to receive the system message that triggered the no more openers
	 * condition, the user will get the system message and the next
	 * time the user calls read the ReceiveNoOpeners exception will be thrown.
	 * <p>
	 * Whether the user needs to reply to the message read in 
	 * depends on the value of receiveDepth (see
	 * {@link #setReceiveDepth setReceiveDepth}).
	 *
	 * @param buffer is a byte array in which the message read is returned.  The
	 * size of the array must be at least as big as the <code>readCount</code>.
	 * @param readCount the number of bytes to be read.  The value must be in
	 *        the range of 0 through 57344 when $RECEIVE is opened with open()
	 *         and 0 through 2 MB $RECEIVE is opened with openWide().
	 * @return a count of the number of bytes read from $RECEIVE into the buffer
	 * @exception GuardianException when the underlying Guardian procedure
	 *            fails with an error.
	 * @exception ReceiveNoOpeners when there are no more openers and $RECEIVE is open
	 * @exception IllegalArgumentException when readCount is less then 0 or
	 *            more than 57344 when $RECEIVE is opened with open() or when readCount is less then 0 or
	 *            more than 2 MB when $RECEIVE is opened with openWide() or when length of buffer is null or 
	 *            less than readCount
	 */
	public int read(byte[] buffer, int readCount) throws GuardianException,
			ReceiveNoOpeners, IllegalArgumentException {
		if (!isOpen())
			throw new GuardianException(GError.ENOTOPEN, "READUPDATEX", name_);
		//changes begin for TSMP 2.4
		if (pathSendMode == PathsendModeType.TSMP_PATHSEND_32KB_LIMIT) {
			if (readCount < 0 || readCount > 57344)
				throw new IllegalArgumentException(
						"Illegal readCount, value must be 0-57344: "
								+ readCount);
		} else {
			if (readCount < 0 || readCount > 2097152)
				throw new IllegalArgumentException(
						"Illegal readCount, value must be 0-2097152: "
								+ readCount);
		}
		//changes end for TSMP 2.4
		if (buffer != null) {
			if (readCount > buffer.length) {
				throw new IllegalArgumentException(
						"buffer not large enought to hold potential message read");
			}
		}

		// if there are already receiveDepth_ number of msgs that haven't yet been
		// replied to then this thread must wait until another thread performs a
		// reply.  Only one thread is allowed to wait.  A reply, a cancel or a
		// close will wake up this waiting thread.
		if ((receiveDepth_ > 0) && (msgsOutstanding_ == receiveDepth_)) {
			if (readThreadWaiting_) {
				throw new GuardianException(GError.ETOOMANY, "READUPDATEX",
						name_);
			}
			// wait for some other thread to reply to an outstanding message
			synchronized (readMonitor_) {
				readThreadWaiting_ = true;
				while (!readHasBeenCancelled_
						&& (msgsOutstanding_ == receiveDepth_)) {
					try {
						readMonitor_.wait();
					} catch (IllegalMonitorStateException ex) {
						throw new RuntimeException(
								"Internal Error: got IllegalMonitorStateException on call to wait");
					} catch (InterruptedException ex) {
					}
				}
				readThreadWaiting_ = false;
			}
			if (readHasBeenCancelled_) {
				int fsError = GError.EREQABANDONED;
				if (!isOpen_)
					fsError = GError.ENOTOPEN;
				readHasBeenCancelled_ = false;
				throw new GuardianException(fsError, "READUPDATEX", name_);
			}
		}

		// countRead is an output parameter to the native read so we make it an array
		// The native read call also sets lastRecvInfo_ to a ReceiveInfo object it
		// creates or NULL if read failed.
		int[] countRead = new int[1];

		int error = read(buffer, readCount, countRead, notifyNoOpeners_,
				pathSendMode);
		switch (error) {
		case GError.EOK:
		case GError.ESYSMESS:
			break;
		case GError.EEOF:
			throw new ReceiveNoOpeners();
		default:
			throw new GuardianException(error, "READUPDATEX", name_);
		}

		msgsOutstanding_++;
		if (msgsOutstanding_ > receiveDepth_)
			throw new RuntimeException(
					"Internal Error: number of outstanding messages("
							+ msgsOutstanding_ + ") exceeds receiveDepth("
							+ receiveDepth_ + ")");

		return countRead[0];
	} //end read

	/**
	 * Reads a message from $RECEIVE.  See {@link #read} for more information
	 * about reading messages from $RECEIVE.
	 *
	 * @param buffer is typically a ddl2java generated class 
	 * @return a count of the number of bytes read from $RECEIVE into the class
	 * @exception GuardianException when the underlying Guardian procedure
	 *            fails with an error.
	 * @exception ReceiveNoOpeners when there are no more openers and $RECEIVE is open
	 * @exception DataConversionException if thrown by the GuardianInput.unmarshal method
	 * @exception IllegalArgumentException when buffer is null or length of buffer is
	 * is more than 57344 when $RECEIVE is opened with open() or length of buffer is more than 2 MB 
	 * when $RECEIVE is opened with openWide()
	 */
	public int read(GuardianInput buffer) throws GuardianException,
			ReceiveNoOpeners, IllegalArgumentException, DataConversionException {
		if (buffer == null)
			throw new IllegalArgumentException("buffer must not be null");
		int len = buffer.getLength();
		byte[] bArr = new byte[len];

		int count = read(bArr, len);

		buffer.unmarshal(bArr, count);
		return count;
	} //end read into I/F

	/**
	 * Replies to the last message with data and error code supplied by the user.  
	 * See {@link #reply(byte[],int,ReceiveInfo,int)}
	 * for more information on calling reply.
	 *
	 * @param buffer is a byte array which contains the data to reply with, may be null
	 * @param replyCount the number of bytes to reply with, must be in the
	 * range of 0 through 57344 when $RECEIVE is opened with open() and if buffer is not null,must be in the
	 * range of 0 through 2 MB when $RECEIVE is opened with openWide() and if buffer is not null , ignored if buffer is null
	 * @param errorReturn the error code returned to the requester process.  This
	 * is the value returned if the requester process calls FILEINFO or one 
	 * of the FILEGETINFO system procedures.
	 * @return a count of the number of bytes written
	 * @exception GuardianException when the underlying Guardian REPLYX call fails
	 * @exception IllegalArgumentException when replyCount is less than 0 or greater than
	 * 57344 when $RECEIVE is opened with open() or when replyCount is less than 0 or greater than
	 * 2 MB when $RECEIVE is opened with openWide() OR when buffer is null when replyCount is greater than 0 
	 */
	public int reply(byte[] buf, int replyCount, int errorReturn)
			throws GuardianException, IllegalArgumentException {
		if (lastRecvInfo_ == null)
			throw new GuardianException(GError.EBADREPLY, "REPLYX", name_);

		// Call Java method
		int count = reply(buf, replyCount, lastRecvInfo_, errorReturn);
		return count;
	} //end reply

	/**
	 * Replies to the last message read, uses the data in a ddl2java
	 * generated class and an error code supplied by the caller.
	 * See {@link #reply(byte[],int,ReceiveInfo,int)}
	 * for more information on calling reply.
	 *
	 * @param data the data to reply with in an object which implements GuardianOutput
	 * The ddl2java tool generates classes like this
	 * @param errorReturn the error code returned to the requester process.  This
	 * is the value returned if the requester process calls FILEINFO or one 
	 * of the FILEGETINFO system procedures.
	 * @return a count of the number of bytes written
	 * @exception DataConversionException if there is a data conversion error
	 * moving the contents of <code>data</code> to a byte array
	 * @exception IllegalArgumentException when data is null or the
	 * number of bytes in data exceeds 57344 when $RECEIVE is opened with open() or the
	 * number of bytes in data exceeds 2 MB when $RECEIVE is opened with openWide()
	 * @exception GuardianException when the underlying Guardian REPLYX call fails
	 */
	public int reply(GuardianOutput data, int errorReturn)
			throws GuardianException, IllegalArgumentException,
			DataConversionException {
		if (data == null)
			throw new IllegalArgumentException("data must not be null");
		if (lastRecvInfo_ == null)
			throw new GuardianException(GError.EBADREPLY, "REPLYX", name_);

		byte[] buf = data.marshal();

		// Call java method
		int count = reply(buf, buf.length, lastRecvInfo_, errorReturn);
		return count;
	} //end reply

	/**
	 * Replies to a specific previously read message using the data in a ddl2java
	 * generated class and an error code supplied by the caller.
	 * See {@link #reply(byte[],int,ReceiveInfo,int)}
	 * for more information on calling reply.
	 *
	 * @param data the data to reply with in an object which implements GuardianOutput
	 * The ddl2java tool generates classes like this
	 * @param recvInfo the receive info object that is associated with the message
	 * being replied to.  This object identifies which message is being
	 * replied to. 
	 * @param errorReturn the error code returned to the requester process.  This
	 * is the value returned if the requester process calls FILEINFO or one 
	 * of the FILEGETINFO system procedures.
	 * @return a count of the number of bytes written
	 * @exception DataConversionException if there is a data conversion error
	 * moving the contents of <code>data</code> to a byte array
	 * @exception IllegalArgumentException when recvInfo or data is null or the 
	 * number of bytes in data exceeds 57344 when $RECEIVE is opened with open() or the 
	 * number of bytes in data exceeds 2 MB when $RECEIVE is opened with openWide()
	 * @exception GuardianException when the underlying Guardian REPLYX call fails
	 */
	public int reply(GuardianOutput data, ReceiveInfo recvInfo, int errorReturn)
			throws GuardianException, IllegalArgumentException,
			DataConversionException {
		if (data == null)
			throw new IllegalArgumentException("data must not be null");

		byte[] buf = data.marshal();

		// Call java method
		int count = reply(buf, buf.length, recvInfo, errorReturn);
		return count;
	} //end reply

	/**
	 * Replies to a specific previously read message with data and
	 * error code supplied by the user.  If the user is replying to an open system
	 * message the buffer parameter is ignored.
	 * <p>
	 * A reply ends the servers participation in the TMF transaction of the request.
	 * <p>
	 * For a context-free Pathway server a error code other 
	 * than FEOK (0) will result in the link (the open) to the server being closed.
	 * Use of non-zero error values is not recommended for context-free servers
	 * because of their effect on the link to the server.  A context-sensitive
	 * Pathway server may reply with FEEOF (1) and FECONTINUE (70) in addition to
	 * FEOK without disturbing the link.
	 * See the TS/MP Pathsend and Server Programming Manual for more
	 * information on Pathway servers.
	 *
	 * @param buffer is a byte array which contains the data to reply with, may be null
	 * @param replyCount the number of bytes to reply with, must be in the
	 * range of 0 through 57344 if buffer is not null and $RECEIVE is opened with open() 
	 * or must be in the range of 0 through 2 MB if buffer is not null and $RECEIVE is 
	 * opened with openWide(), ignored if buffer is null
	 * @param recvInfo the receive info object that is associated with the message
	 * being replied to.  This object identifies which message is being
	 * replied to. 
	 * @param errorReturn the error code returned to the requester process.  This
	 * is the value returned if the requester process calls FILEINFO or one 
	 * of the FILEGETINFO system procedures.
	 * @return a count of the number of bytes written
	 * @exception IllegalArgumentException when replyCount is less than 0 or greater than
	 * 57344 when $RECEIVE is opened with open() OR when replyCount is less than 0 or greater than
	 * 2 MB when $RECEIVE is opened with openWide() or when recvInfo is null or buffer is null when replyCount is greater than 0
	 * @exception GuardianException when the underlying Guardian REPLYX call fails with an error
	 */
	public int reply(byte[] buffer, int replyCount, ReceiveInfo recvInfo,
			int errorReturn) throws GuardianException, IllegalArgumentException {
		if (!isOpen())
			throw new GuardianException(GError.ENOTOPEN, "REPLYX", name_);

		if (receiveDepth_ == 0)
			throw new GuardianException(GError.EBADREPLY, "REPLYX", name_);
		if (recvInfo == null)
			throw new IllegalArgumentException("recvInfo can not be null");
		if (pathSendMode == PathsendModeType.TSMP_PATHSEND_32KB_LIMIT) {
			if (buffer != null && (replyCount < 0 || replyCount > 57344))
				throw new IllegalArgumentException(
						"Illegal value for replyCount, must be in the range 0 to 57344, inclusive");
		} else {
			if (buffer != null && (replyCount < 0 || replyCount > 2097152))
				throw new IllegalArgumentException(
						"Illegal value for replyCount, must be in the range 0 to 2097152, inclusive");
		}

		if (buffer == null && replyCount != 0)
			throw new IllegalArgumentException(
					"Reply count must be 0 when no buffer is supplied");
		if (errorReturn > Short.MAX_VALUE)
			throw new IllegalArgumentException("errorReturn must not exceed "
					+ Short.MAX_VALUE);

		// countWritten is an output parameter to the native reply so we make it an array
		int[] countWritten = new int[1];

		// Call native routine

		int error = reply(buffer, replyCount, (short) errorReturn, recvInfo
				.getMessageTag(), countWritten, pathSendMode);
		if (error != GError.EOK) {
			throw new GuardianException(error, "REPLYX", name_);
		}

		lastRecvInfo_ = null;
		msgsOutstanding_--;
		// wake up thread waiting for number of outstanding msgs to go down
		if (readThreadWaiting_) {
			synchronized (readMonitor_) {
				try {
					readMonitor_.notify();
				} catch (IllegalMonitorStateException ex) {
					throw new RuntimeException(
							"Internal Error: got IllegalMonitorStateException on call to notify");
				}
			}
		}
		if (msgsOutstanding_ < 0)
			throw new RuntimeException(
					"Internal Error: number of outstanding messages has become negative");

		return countWritten[0];
	} //end reply

	// Native Methods
	private native int closeReceive(int pathSendMode);

	private native int openReceive(short initOpens, short receiveDepth,
			short syncDepth, int maxReplyLen, short options, long sysMsgMask,
			int pathSendMode);

	private native int read(byte[] buffer, int readCount, int[] countRead,
			boolean notifyNoOpeners, int pathSendMode);

	private native int reply(byte[] buffer, int replyCount, short errorReturn,
			short msgTag, int[] countWritten, int pathSendMode);

	private native int cancel();

	private native int cancelIfAny();

	private native int setPriorityQueue(short param1);

	private native void setSysMsgMask(long sysMsgMask);

} /* end class Receive */
