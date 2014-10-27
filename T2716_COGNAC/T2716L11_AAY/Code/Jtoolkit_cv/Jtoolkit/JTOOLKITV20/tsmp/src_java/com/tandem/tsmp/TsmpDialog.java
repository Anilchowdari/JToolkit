/*---------------------------------------------------------------------------
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/

package com.tandem.tsmp;

import com.tandem.ext.util.DataConversionException;
import com.tandem.ext.util.PathsendModeType;
import com.tandem.ext.util.NativeLoader;
import com.tandem.ext.guardian.GuardianInput;
import com.tandem.ext.guardian.GuardianOutput;

/**
 * A TsmpDialog represents a session with a specific serverclass within a
 * specific Pathway application. Establish a dialog between a requester and a
 * server process in a server class, and then send messages within the dialog.
 * 
 * 
 * After the dialog is established, the same server process is used for all the
 * messages in the dialog;
 * 
 * <p>
 * Transaction interactions are dependent on TsmpDialog ,based on the
 * Transaction Model Value of the Dialog ie ONE_TRANSACTION_PER_DIALOG_MODEL or
 * ANY_TRANSACTION_PER_DIALOG_MODEL.
 * </p>
 * 
 * @version 1.1 07/01/2011
 * @author Sarala, Babu Cherian, Pabitra Mohan Dalei
 * @see com.tandem.ext.guardian.GuardianInput
 * @see com.tandem.ext.guardian.GuardianOutput
 */
//changes for wide pathsend

public class TsmpDialog extends Object {

	/* load library of native methods */
	static {
		 NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();	
	}

	/**
	 * Specifies the Pathmon process name for the Pathway application.
	 */

	protected String pathmonName;

	/**
	 * Specifies the serverclass name within the Pathway Application.
	 */

	protected String serverClassName;

	/**
	 * Specifies the dialog status of the Dialog.
	 */

	private int dialogStatus;

	/**
	 * Specifies the maximum amount of time the Linkmon Process wait for the
	 * completion of the send operation.
	 */

	private int timeout;

	/**
	 * Specifies the default value of the time out
	 */

	private static final int INDEFINITE = -1;

	/**
	 * Specifies the identifier of the dialog that can be used for the
	 * subsequent operations on the dialog.
	 */

	private int dialogId;

	/**
	 * Specifies the transaction model value of the dialog.
	 */

	private int dialogTransactionModel;

	// Status of the Dialog.

	/**
	 * Indicates there is no dialog.
	 */

	public static final int DS_NODIALOG = -1;

	/**
	 * Indicates dialog has been ended.
	 */

	public static final int DS_ENDED = 0;

	/**
	 * Indicates dialog has been aborted.
	 */

	public static final int DS_ABORTED = 1;

	/**
	 * Indicates dialog is in active state.
	 */

	public static final int DS_CONTINUE = 70;

	// Transaction model value of the dialog.

	/**
	 * The transaction cannot commit, calls to ENDTRANSACTION will fail until
	 * the server ends the dialog.
	 */

	public static final int TM_ONE_TRANSACTION_PER_DIALOG = 0;

	/**
	 * The transaction can be committed at any time, no restriction on
	 * ENDTRANSACTION.
	 */

	public static final int TM_ANY_TRANSACTION_PER_DIALOG = 1;

	/**
	 * Pathsend errors that are processed.
	 */

	private static final int FEScInvalidServerClassName = 900;
	private static final int FEScInvalidPathmonName = 901;
	private static final int FEScPathmonConnect = 902;
	private static final int FEScPathmonMessage = 903;
	private static final int FEScServerLinkConnect = 904;
	private static final int FEScNoServerLinkAvailable = 905;

	/*
	 * FEScNoSendEverCalled(906),FEScInvalidFlagsValue(909) and
	 * FEScMissingParameter(910) should never occur unless we have a programming
	 * error. 907 and 908 are no longer returned by Pathsend.
	 */
	private static final int FEScInvalidBufferLength = 911;

	/* FEScParameterBoundsError(912) should not occur */

	private static final int FEScServerClassFrozen = 913;
	private static final int FEScUnknownServerClass = 914;
	private static final int FEScPathmonShutdown = 915;
	private static final int FEScServerCreationFailure = 916;
	private static final int FEScServerClassTmfViolation = 917;
	private static final int FEScSendOperationAborted = 918;
	private static final int FEScInvalidTimeoutValue = 919;
	private static final int FEScPFSUseError = 920;
	private static final int FEScTooManyPathmons = 921;
	private static final int FEScTooManyServerClasses = 922;
	private static final int FEScTooManyServerLinks = 923;
	private static final int FEScTooManySendRequests = 924;
	private static final int FEScTooManyRequesters = 925;
	private static final int FEScDialogInvalid = 926;
	private static final int FEScTooManyDialogs = 927;
	private static final int FEScOutstandingSend = 928;
	private static final int FEScDialogAborted = 929;
	private static final int FEScChangedTransid = 930;
	private static final int FEScDialogEnded = 931;

	/* there is no error 932 */

	private static final int FEScDialogOutstanding = 933;
	private static final int FEScTransactionAborted = 934;

	private static final int FEScLinkmonConnect = 947;

	/**
	 * File system errors we process.
	 */

	private static final int FEOK = 0;
	private static final int FEInvalOp = 2;
	private static final int FEInUse = 12;
	private static final int FENoSuchDev = 14;
	private static final int FEBadCount = 21;
	private static final int FENoBufSpace = 31;
	private static final int FETimedOut = 40;
	private static final int FENoDiscSpace = 43;
	private static final int FESecViol = 48;
	private static final int FEPathDown = 201;
	private static final int FEPathDown240 = 240;
	private static final int FEPathDown255 = 255;
	
	/**
	* Selecting 32KB pathsend mode type
	*/
	public int pathSendMode = PathsendModeType.TSMP_PATHSEND_32KB_LIMIT;

	/**
	 * Creates a new TsmpDialog session and associates the session with the
	 * specified serverclass and Pathmon process.
	 * 
	 * Set the default value for the dialog as follows dialogStatus to
	 * DS_NODIALOG, dialogTransactionModel to TM_ONE_TRANSACTION_PER_DIALOG,
	 * timeout to INDEFINITE
	 * 
	 * @param pathmonName
	 *            the name of the Pathmon process for the Pathway application
	 * 
	 * @param serverClassName
	 *            the name of the server class within the Pathway application
	 * @throws OutOfMemoryError
	 *             when the Java Virtual Machine cannot allocate an object
	 *             because it is out of memory, and no more memory could be made
	 *             available by the garbage collector
	 * 
	 */

	protected TsmpDialog(String pathmonName, String serverClassName)
	throws OutOfMemoryError {
		this.pathmonName = pathmonName;
		this.serverClassName = serverClassName;
		this.dialogStatus = DS_NODIALOG;
		this.dialogTransactionModel = TM_ONE_TRANSACTION_PER_DIALOG;
		
		this.dialogId = DS_NODIALOG;
		this.timeout = INDEFINITE;

	} // end of constructor

	/**
	 * Reset the status of the dialog to DS_NODIALOG.
	 * 
	 * @exception IllegalStateException
	 *                if state of the dialog is DS_CONTINUE.
	 */

	public void reset() throws IllegalStateException {
		if (dialogStatus == DS_CONTINUE)
			throw new IllegalStateException("Cannot reset a dialog on progress");

		if (dialogStatus == DS_NODIALOG)
			return;

		dialogStatus = DS_NODIALOG;
		dialogId = DS_NODIALOG;
	} // end of function

	/**
	 * Get the Pathmon process name of the Pathway application for which the
	 * Dialog has been created.
	 * 
	 * @return the Pathmon process name
	 */

	public String getPathmonName() {
		return this.pathmonName;
	} // end of function

	/**
	 * Get the ServerClass name within the Pathway application for which the
	 * Dialog has been created.
	 * 
	 * @return the ServerClass name
	 */

	public String getServerClassName() {
		return this.serverClassName;
	} // end of function

	/**
	 * Get the status of the Dialog.
	 * 
	 * @return the status of the Dialog
	 */

	public int getStatus() {
		return dialogStatus;
	} // end of function

	/**
	 * Get the status of the Dialog as a string
	 * 
	 * @return the status of the Dialog as a string
	 */

	private String getStatusString() {
		switch (dialogStatus) {
		case DS_NODIALOG:
			return new String("NODIALOG");
		case DS_ENDED:
			return new String("ENDED");
		case DS_ABORTED:
			return new String("ABORTED");
		case DS_CONTINUE:
			return new String("CONTINUE");
		default:
			return new String("UNKNOWN");
		}
	} // end of function

	/**
	 * Returns a new String object representing the Pathmon process name,
	 * ServerClass name and the Dialog Status.
	 * 
	 * @return the Pathmon process name.ServerClass name [ DialogStatus ]
	 */

	public String toString() {
		return new String(pathmonName.trim() + "." + serverClassName.trim()
				+ " [ " + getStatusString() + " ]");
	} // end of function

	/**
	 * Set the TransactionModel value. It can be set only when the state of the
	 * Dialog is DS_NODIALOG.
	 * 
	 * @param transModel
	 *            is either TM_ONE_TRANSACTION_PER_DIALOG or
	 *            TM_ANY_TRANSACTION_PER_DIALOG
	 * 
	 * @exception IllegalStateException
	 *                if the state of the dialog is other than DS_NODIALOG.
	 * 
	 * @exception IllegalArgumentException
	 *                if the model value is not any of
	 *                TM_ONE_TRANSACTION_PER_DIALOG or
	 *                TM_ANY_TRANSACTION_PER_DIALOG.
	 */

	public void setTransactionModel(int transModel)
	throws IllegalArgumentException, IllegalStateException {

		if (dialogStatus != DS_NODIALOG)
			throw new IllegalStateException(
			"Cannot set Transaction model at this state");

		if (transModel != TM_ONE_TRANSACTION_PER_DIALOG
				&& transModel != TM_ANY_TRANSACTION_PER_DIALOG) {
			throw new IllegalArgumentException("Invalid Transaction Model");
		}
		this.dialogTransactionModel = transModel;
	} // end of function

	/**
	 * Get the TransactionModel value of the Dialog.
	 * 
	 * @return TransactionModel value of the Dialog
	 */

	public int getTransactionModel() {
		return this.dialogTransactionModel;
	} // end of function

	/**
	 * Set the Timeout value in hundredths of a second. Default value is INDEFINITE
	 * 
	 * @param timeout
	 *            value should be INDEFINITE or greater than 0.
	 * @exception IllegalArgumentException
	 *                if the timeout value is not any of INDEFINITE or greater
	 *                than 0.
	 */

	public void setTimeout(int timeout) {
		if (timeout == INDEFINITE || timeout > 0)
			this.timeout = timeout;
		else {
			throw new IllegalArgumentException(
			"Timeout value should be INDEFINITE or greater than 0");
		}
	} // end of function

	/**
	 * Get the Timeout value of the Dialog.
	 * 
	 * @return Timeout value of the Dialog
	 */

	public int getTimeout() {
		return this.timeout;
	} // end of function

	/**
	 * Cleans up the resources for the specified dialog after the server has
	 * ended it .
	 * 
	 * @throws TsmpSendException,TsmpServerUnavailableException.
	 */

	private void end() throws TsmpSendException,
	TsmpServerUnavailableException, TsmpRoutUnavailableException,TsmpFileSystemException {

		int tsmpError[] = null;

		if (dialogStatus == DS_CONTINUE) {
			tsmpError = scDialogEnd(dialogId);
			if (tsmpError[0] != 0)
				checkError(tsmpError[0], tsmpError[1]);
			else
				dialogStatus = DS_ENDED;
		}

	} // end of function

	/**
	 * Aborts the dialog.
	 * 
	 * @exception IllegalStateException
	 *                if the state of the Dialog is DS_NODIALOG or DS_ENDED .
	 * 
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * 
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the
	 *                request.
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 */

	public void abort() throws TsmpSendException,
	TsmpServerUnavailableException, TsmpRoutUnavailableException,TsmpFileSystemException {

		int tsmpError[] = null;

		if (dialogStatus == DS_ABORTED)
			return;

		if (dialogStatus != DS_CONTINUE)
			throw new IllegalStateException("Cannot abort a dialog in "
					+ getStatusString() + " state");

		tsmpError = scDialogAbort(dialogId);

		if (tsmpError[0] != 0)
			checkError(tsmpError[0], tsmpError[1]);
		else
			dialogStatus = DS_ABORTED;

	} // end of function

	/**
	 * Begin a dialog if the state of the dialog is DS_NODIALOG or performs a
	 * send if the state of the dialog is DS_CONTINUE.
	 * 
	 * The send methods are synchronized to make sure that only one send is
	 * outstanding at any time on the same TsmpDialog object.
	 * 
	 * The send methods throw IllegalStateException if the dialog status is not
	 * any of DS_NODIALOG or DS_CONTINUE.
	 * 
	 * @param request
	 *            the data to be send to the server
	 * 
	 * @param requestLen
	 *            the number of bytes to send, must be less than or equal to the
	 *            length of <code>request</code>
	 * 
	 * @param reply
	 *            the reply from the server, replies from the server that exceed
	 *            the length of this array will be truncated.
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalStateException
	 *                if the dialog status is not any of DS_NODIALOG or
	 *                DS_CONTINUE.
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply is null or if requestLen is larger
	 *                than length of request
	 * 
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * 
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 *                
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
     */

	public synchronized int send(byte[] request, int requestLen, byte[] reply)
	throws TsmpSendException, TsmpServerUnavailableException,
	IllegalStateException, IllegalArgumentException,
	TsmpRoutUnavailableException,TsmpFileSystemException {

		int tsmpError[] = null;
		int actualReplyLen[] = new int[1];
		int dlgId[] = new int[1];
		if (dialogStatus != DS_NODIALOG && dialogStatus != DS_CONTINUE)
			throw new IllegalStateException(
					"Cannot send when the state of the dialog is "
					+ getStatusString());
		// Begin changes for Solution:  10-091023-5640 
		  if ((this.serverClassName == null) && (this.pathmonName == null))
		   throw new NullPointerException(
		   "pathmonName and serverClassName must not be null");
		  if (this.pathmonName == null)
		   throw new NullPointerException("pathmonName must not be null");
		  if (this.serverClassName == null)
		   throw new NullPointerException(
		   "serverClassName must not be null");
		   // End changes for Solution:  10-091023-5640 
		  
		if (request == null)
			throw new IllegalArgumentException("Request should not be null");

		if (requestLen < 0)
			throw new IllegalArgumentException(
			"Request length should be greater than 0");

		if (reply == null)
			throw new IllegalArgumentException("Reply must should be null");

		switch (dialogStatus) {

		case DS_NODIALOG:
			dialogStatus = DS_CONTINUE;
			
			
			tsmpError = scDialogBegin(dlgId, this.pathmonName,
					this.serverClassName, request, requestLen, reply,
					reply.length, actualReplyLen, this.timeout,
					this.dialogTransactionModel, pathSendMode);
			
			dialogId = dlgId[0];
			
			
			break;

		case DS_CONTINUE:
			
			
			tsmpError = scDialogSend(this.dialogId, request, requestLen, reply,
					reply.length, actualReplyLen, this.timeout, pathSendMode);
			
			break;
		}
		
		if (tsmpError[0]==0 &&  tsmpError[1]==FEPathDown )
		{
			scDialogEnd(dialogId);
			dialogStatus = DS_ENDED;
			checkError(tsmpError[0], tsmpError[1]);
		}
		
		switch (tsmpError[1]) {
		case 70:
			dialogStatus = DS_CONTINUE;
			break;
		case 0:
			end();
			break;
		default:
			scDialogAbort(dialogId);
		dialogStatus = DS_ABORTED;
		}

		if (tsmpError[0] != 0) {

			if (dialogStatus == DS_CONTINUE) {
				scDialogAbort(dialogId);
				dialogStatus = DS_ABORTED;
			}

			checkError(tsmpError[0], tsmpError[1]);
		}

		return actualReplyLen[0];
	} // end of function

	/**
	 * Carries out a single Pathsend request for a client using GuardianInput
	 * and GuardianOutput interfaces for the request and reply. If successful,
	 * <CODE>reply</CODE> will contain the reply from the GuardianInput
	 * Interface.
	 * 
	 * @param request
	 *            the request to be sent to the server
	 * 
	 * @param reply
	 *            holds the reply from the server
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalStateException
	 *                if the dialog status is not any of DS_NODIALOG or
	 *                DS_CONTINUE.
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply are null
	 * 
	 * @exception DataConversionException
	 *                if there is an error marshaling the request data from
	 *                <code>request</code> into a byte array or if if there is
	 *                an error unmarshaling the server reply into the fields of
	 *                <code>reply</code>.
	 * 
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * 
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 */

	public synchronized int send(GuardianOutput request, GuardianInput reply)
	throws TsmpSendException, TsmpServerUnavailableException,
	DataConversionException, IllegalStateException,
	IllegalArgumentException, TsmpRoutUnavailableException,TsmpFileSystemException {

		if (dialogStatus != DS_NODIALOG && dialogStatus != DS_CONTINUE)
			throw new IllegalStateException(
					"Cannot send when the state of the dialog is "
					+ getStatusString());

		if (request == null)
			throw new IllegalArgumentException("Request must not be null");

		if (reply == null)
			throw new IllegalArgumentException("Reply must not be null");

		byte[] baRequest = request.marshal();
		int len = reply.getLength();
		byte[] baReply = new byte[len];

		int count = send(baRequest, baRequest.length, baReply);

		if (count > 0)
			reply.unmarshal(baReply, count);

		return count;
	} // end of function

	/**
	 * Carries out a single Pathsend request for a client using the
	 * GuardianOutput interface for the request and a byte array for reply. If
	 * successful,<CODE>reply</CODE> will contain the reply from TsmpServer.
	 * 
	 * @param request
	 *            the request to be sent to the server
	 * 
	 * @param reply
	 *            the reply from the server, replies from the server that exceed
	 *            the length of this array will be truncated
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalStateException
	 *                if the dialog status is not any of DS_NODIALOG or
	 *                DS_CONTINUE.
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply are null
	 * 
	 * @exception DataConversionException
	 *                if there is an error marshaling the request data from
	 *                <code>request</code> into a byte array.
	 * 
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * 
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton                
	 */

	public synchronized int send(GuardianOutput request, byte[] reply)
	throws IllegalArgumentException, DataConversionException,
	TsmpSendException, IllegalStateException,
	TsmpServerUnavailableException ,TsmpRoutUnavailableException,TsmpFileSystemException{

		if (dialogStatus != DS_NODIALOG && dialogStatus != DS_CONTINUE)
			throw new IllegalStateException(
					" Cannot send when the state of the dialog is  "
					+ getStatusString());

		if (request == null)
			throw new IllegalArgumentException("Request must not be null");

		if (reply == null)
			throw new IllegalArgumentException("Reply must not be null");

		byte[] baRequest = request.marshal();

		int count = send(baRequest, baRequest.length, reply);

		return count;
	} // end of function

	// Handle Pathmon error result appropriately. Error messages are
	// differentiated by Pathsend error numbers only, not file system
	// error numbers.

	private void checkError(int psError, int fsError) throws TsmpSendException,
	TsmpServerUnavailableException, TsmpRoutUnavailableException,TsmpFileSystemException {

		if (psError != 0) {

			switch (psError) {

			case FEScInvalidServerClassName:
				throw new TsmpSendException("Invalid serverclass name.",
						psError, fsError, toString());

			case FEScInvalidPathmonName:
				throw new TsmpSendException("Invalid Pathmon Process name.",
						psError, fsError, toString());

			case FEScPathmonConnect:
				throw new TsmpSendException(
						"An error has occurred in the requesters communication with the PATHMON process.",
						psError, fsError, toString());

			case FEScPathmonMessage:
				throw new TsmpSendException(
						"The LINKMON process received an Unrecognizable message from the PATHMON process.",
						psError, fsError, toString());

			case FEScServerLinkConnect:
				throw new TsmpSendException(
						"An Error has occured with link to the server.",
						psError, fsError, toString());

			case FEScNoServerLinkAvailable:
				throw new TsmpSendException(
						"The LINKMON process had no links to the server class and was unable to get a link from the PATHMON process to satisfy this request.",
						psError, fsError, toString());

			case FEScInvalidBufferLength:
				throw new TsmpSendException(
						"The Buffer length in the request-len or maximum-reply-len is invalid.",
						psError, fsError, toString());

			case FEScServerClassFrozen:
				throw new TsmpSendException(
						"The server class the process tried to send is frozen.",
						psError, fsError, toString());

			case FEScUnknownServerClass:
				throw new TsmpSendException(
						"The server is not configured through the specified PATHMON process.",
						psError, fsError, toString());

			case FEScPathmonShutdown:
				throw new TsmpSendException(
						"The send operation has been denied.", psError,
						fsError, toString());

			case FEScServerCreationFailure:
				throw new TsmpSendException(
						"The LINKMON process was unable to get a link to the server class due to a server creation failure.",
						psError, fsError, toString());

			case FEScServerClassTmfViolation:
				throw new TsmpSendException(
						"The transaction mode of the Pathsend program does not match of the server class.",
						psError, fsError, toString());

			case FEScSendOperationAborted:
				throw new TsmpSendException(
						"The send operation has been terminated at an indeterminate point.",
						psError, fsError, toString());

			case FEScInvalidTimeoutValue:
				throw new TsmpSendException("Invalid Timeout Value.", psError,
						fsError, toString());

			case FEScPFSUseError:
				throw new TsmpSendException(
						"The caller's process file segment(PFS) could not be accessed.",
						psError, fsError, toString());

			case FEScTooManyPathmons:
				throw new TsmpSendException(
						"The maximum number of pathmon is already in use.",
						psError, fsError, toString());

			case FEScTooManyServerClasses:
				throw new TsmpSendException(
						"The maximum number of server classes is already in use.",
						psError, fsError, toString());

			case FEScTooManyServerLinks:
				throw new TsmpSendException(
						"The maximum number of concurrent links to the server is already in use.",
						psError, fsError, toString());

			case FEScTooManySendRequests:
				throw new TsmpSendException(
						"The maximum number of concurrent server-class send operations allowed has exceeded.",
						psError, fsError, toString());

			case FEScTooManyRequesters:
				throw new TsmpSendException(
						"The maximum number of requesters allowed has exceeded.",
						psError, fsError, toString());

			case FEScDialogInvalid:
				throw new TsmpSendException(
						"The Dialog identifier is not valid.", psError,
						fsError, toString());

			case FEScTooManyDialogs:
				throw new TsmpSendException(
						"The maximum number Dialogs per requester is already open.",
						psError, fsError, toString());

			case FEScOutstandingSend:
				throw new TsmpSendException(
						"The requester has an outstanding send operation on this dialog.",
						psError, fsError, toString());

			case FEScDialogAborted:
				throw new TsmpSendException("The dialog has been aborted.",
						psError, fsError, toString());

			case FEScChangedTransid:
				throw new TsmpSendException(
						"Different Transaction identifier has been used.",
						psError, fsError, toString());

			case FEScDialogEnded:
				throw new TsmpSendException(
						"Server had already ended the dialog.", psError,
						fsError, toString());

			case FEScDialogOutstanding:
				throw new TsmpSendException(
						"The server has not ended the dialog.", psError,
						fsError, toString());

			case FEScTransactionAborted:
				throw new TsmpSendException(
						"The transaction associated with the dialog has been aborted.",
						psError, fsError, toString());

			case FEScLinkmonConnect:
				throw new TsmpSendException(
						"Problem communicating with the LINKMON process.",
						psError, fsError, toString());

			default:
				throw new TsmpSendException("Pathsend Error.", psError,
						fsError, toString());

			}
		}

		if (psError == 0 && fsError != 0) {
			switch (fsError) {

			case FEPathDown:
				throw new TsmpRoutUnavailableException(
						"Path to Rout is down. Re-initiate the send", fsError,
						toString());
			default:
				throw new TsmpFileSystemException(fsError, toString());
			}
		}


	} // end of function

	/**
	 * Aborts the dialog if the state of the Dialog is DS_CONTINUE.
	 */

	protected void finalize() {

		int tsmpError[] = null;
		if (dialogStatus == DS_CONTINUE) {
			tsmpError = scDialogAbort(dialogId);
		}

	} // end of function

	// ------------------------- Native Methods ------------------------------

	/**
	 * Performs serverclass_dialog_begin on behalf of send methods..
	 */

	private final native int[] scDialogBegin(int[] dlgId, String pathmonName,
			String serverClassName, byte[] request, int requestLen,
			byte[] reply, int maxReplyLen, int[] actualReplyLen, int timeout,
			int transModel, int pathSendMode);

	/**
	 * Performs serverclass_dialog_send on behalf of send methods..
	 */

	private final native int[] scDialogSend(int dlgId, byte[] request,
			int requestLen, byte[] reply, int maxReplyLen,
			int[] actualReplyLen, int timeout, int pathSendMode);

	/**
	 * Performs serverclass_dialog_abort on behalf of abort method.
	 */

	private final native int[] scDialogAbort(int dlgId);

	/**
	 * Performs serverclass_dialog_end on behalf of end method.
	 */

	private final native int[] scDialogEnd(int dlgId);

	// ----------------------End of native Methods
	// ------------------------------

}
