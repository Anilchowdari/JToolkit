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
 * A TsmpServer represents a session with a specific serverclass within a
 * specific Pathway application. Within the context of a TsmpServer, requests
 * are issued and replies are received.
 * 
 * <p>
 * Transaction interactions are independent of TsmpServer.
 * 
 * @version 1.5 03/17/2004
 * @author Steve Rispoli
 * @see com.tandem.ext.guardian.GuardianInput
 * @see com.tandem.ext.guardian.GuardianOutput
 * @see com.tandem.tsmp.TsmpServerReply
 * @see com.tandem.tsmp.TsmpGenericServerReply
 * @see com.tandem.tsmp.TsmpServerRequest
 */

public class TsmpServer extends Object {

	/* load library of native methods */
	static {
			NativeLoader loader = new NativeLoader();
			loader.LoadLibrary();
		}

	/**
	 * Specifies the Pathmon process name for the Pathway application.
	 */

	public String pathmonName;

	/**
	 * Specifies the serverclass name within the Pathway Application.
	 */

	public String serverClassName;

	/**
	 * Specifies the concatentation of the Pathmon process name and the i
	 * serverclass name within the Pathway Application.
	 */

	private String fullServerClassName;

	// Begin of Change Solution Number 10-030808-8620

	/**
	 * Specifies the maximum amount of time the Linkmon Process wait for the
	 * completion of the send operation.
	 */

	// change
	protected int timeout;

	/**
	 * Specifies the default value of time out.
	 */

	private static final int INDEFINITE = -1;

	// End of Change Solution Number 10-030808-8620

	// TBD: change to use GError for all errors
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

	// Begin of Change Solution Number 10-030808-8620

	private static final int FEScInvalidTimeoutValue = 919;

	// End of Change

	private static final int FEScPFSUseError = 920;
	private static final int FEScTooManyPathmons = 921;
	private static final int FEScTooManyServerClasses = 922;
	private static final int FEScTooManyServerLinks = 923;
	private static final int FEScTooManySendRequests = 924;
	private static final int FEScTooManyRequesters = 925;
	private static final int FEScLinkmonConnect = 947;

	/**
	 * Pathsend errors not processed. Included in case we add
	 * SERVERCLASS_SEND_DIALOG_ to this.
	 */

	private static final int FEScDialogInvalid = 926;
	private static final int FEScTooManyDialogs = 927;
	private static final int FEScOutstandingSend = 928;
	private static final int FEScDialogAborted = 929;
	private static final int FEScChangedTransid = 930;
	private static final int FEScDialogEnded = 931;

	/* there is no error 932 */

	private static final int FEScDialogOutstanding = 933;
	private static final int FEScTransactionAborted = 934;

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

	// changes for wide pathsend
	public int pathSendMode = PathsendModeType.TSMP_PATHSEND_32KB_LIMIT;

	/**
	 * Creates a new TsmpServer session and associates the session with the
	 * specified serverclass and Pathmon process.
	 * 
	 * @param pathmonName
	 *            the name of the Pathmon process for the Pathway application
	 * @param serverClassName
	 *            the name of the server class within the Pathway application
	 */
	public TsmpServer(String pathmonName, String serverClassName) {
		this.pathmonName = pathmonName;
		this.serverClassName = serverClassName;
		this.fullServerClassName = pathmonName + "." + serverClassName;

		// Begin of Change Solution Number 10-030808-8620

		this.timeout = INDEFINITE; // Setting the timeout value to default		
		 		 
		// End of Change
	}

	// Begin of Change Solution Number 10-030808-8620

	/**
	 * Creates a TsmpDialog object. The same Pathmon name and Serverclass name
	 * used for constructing the TsmpServer object is used for creating the
	 * TsmpDialog object.
	 * 
	 * @return TsmpDialog object.
	 */

	public TsmpDialog createDialog() {
		return new TsmpDialog(this.pathmonName, this.serverClassName);
	}

	/**
	 * Set the Timeout value in hundredths of a second. Default value is INDEFINITE
	 * 
	 * @param timeout
	 *            value should be INDEFINITE or greater than 0 .
	 * @exception IllegalArgumentException
	 *                if the timeout value is not any of INDEFINITE or greater
	 *                than 0.
	 */

	public void setTimeout(int timeout) {
		if (timeout == INDEFINITE || timeout > 0)
			this.timeout = timeout;
		else
			throw new IllegalArgumentException(
			"Timeout value should be -1 or greater than 0");
	}

	/**
	 * Get the Timeout value of the Service operation
	 * 
	 * @return Timeout value of the Service operation
	 */

	public int getTimeout() {
		return this.timeout;
	}

	// End of Change Solution Number 10-030808-8620

	/**
	 * Carries out a single Pathsend request for a client using byte arrays for
	 * request and reply. If successful, <CODE>reply</CODE> will contain the
	 * reply from the server.
	 * 
	 * @param request
	 *            the data to be send to the server
	 * @param requestLen
	 *            the number of bytes to send, must be less than or equal to the
	 *            length of <code>request</code>
	 * @param reply
	 *            the reply from the server, replies from the server that exceed
	 *            the length of this array will be truncated
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply is null or if requestLen is larger
	 *                than length of request
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 */
	public int service(byte[] request, int requestLen, byte[] reply)
	throws IllegalArgumentException, TsmpSendException,
	TsmpServerUnavailableException, TsmpRoutUnavailableException,
	TsmpFileSystemException {

		// This is the service method called by the other 2 non-deprecated
		// service
		// methods and by users.

		int tsmpError[] = null;
		int actualReplyLen[] = new int[1];
		
		// Begin changes for Solution:10-091023-5640 
		  if ((this.serverClassName == null) && (this.pathmonName == null))
		   throw new NullPointerException(
		   "pathmonName and serverClassName must not be null");
		  if (this.pathmonName == null)
		   throw new NullPointerException("pathmonName must not be null");
		  if (this.serverClassName == null)
		   throw new NullPointerException(
		   "serverClassName must not be null");
		   // End changes for Solution:10-091023-5640 

		if (request == null)
			throw new IllegalArgumentException("request must not be null");
		if (requestLen < 0)
			throw new IllegalArgumentException(
			"requestLen must not be negative");
		if (reply == null)
			throw new IllegalArgumentException("reply must not be null");

		// Begin of Change Solution Number 10-030808-8620

		tsmpError = scSend(this.pathmonName, this.serverClassName, request,
				requestLen, reply, reply.length, actualReplyLen, this.timeout,
				pathSendMode);
		// End of Change
		interpretResults(tsmpError[0], tsmpError[1]);

		return actualReplyLen[0];
	} // end service byte[], len, byte[]

	/**
	 * Carries out a single Pathsend request for a client using GuardianInput
	 * and GuardianOutput interfaces for the request and reply. If successful,
	 * <CODE>reply</CODE> will contain the reply from the TsmpServer.
	 * 
	 * @param request
	 *            the request to be sent to the server
	 * @param reply
	 *            holds the reply from the server
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply are null
	 * @exception DataConversionException
	 *                if there is an error marshaling the request data from
	 *                <code>request</code> into a byte array or if there is an
	 *                error unmarshaling the server reply into the fields of
	 *                <code>reply</code>.
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 */
	public int service(GuardianOutput request, GuardianInput reply)
	throws IllegalArgumentException, DataConversionException,
	TsmpSendException, TsmpServerUnavailableException,
	TsmpRoutUnavailableException, TsmpFileSystemException {

		if (request == null)
			throw new IllegalArgumentException("request must not be null");
		if (reply == null)
			throw new IllegalArgumentException("reply must not be null");

		byte[] baRequest = request.marshal();
		int len = reply.getLength();
		byte[] baReply = new byte[len];
		int count = service(baRequest, baRequest.length, baReply);

		reply.unmarshal(baReply, count);

		return count;
	} // end service GuardianOutput, GuardianInput

	/**
	 * Carries out a single Pathsend request for a client using the
	 * GuardianOutput interface for the request and a byte array for reply. If
	 * successful, <CODE>reply</CODE> will contain the reply from the
	 * TsmpServer.
	 * 
	 * @param request
	 *            the request to be sent to the server
	 * @param reply
	 *            the reply from the server, replies from the server that exceed
	 *            the length of this array will be truncated
	 * 
	 * @return the actual number of bytes in the server reply
	 * 
	 * @exception IllegalArgumentException
	 *                if request or reply are null
	 * @exception DataConversionException
	 *                if there is an error marshaling the request data from
	 *                <code>request</code> into a byte array.
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 */
	public int service(GuardianOutput request, byte[] reply)
	throws IllegalArgumentException, DataConversionException,
	TsmpSendException, TsmpServerUnavailableException,
	TsmpRoutUnavailableException, TsmpFileSystemException {

		if (request == null)
			throw new IllegalArgumentException("request must not be null");
		if (reply == null)
			throw new IllegalArgumentException("reply must not be null");

		byte[] baRequest = request.marshal();

		int count = service(baRequest, baRequest.length, reply);

		return count;
	} // end service GuardianOutput, byte[]

	/**
	 * Carries out a single request for a client. <CODE>request</CODE> must
	 * contain the information required by TsmpServer. If successful,
	 * <CODE>reply</CODE> will contain the reply from the TsmpServer.
	 * 
	 * @param request
	 *            the TsmpServerRequest
	 * @param reply
	 *            the TsmpServerReply or TsmpGenericServerReply
	 * 
	 * @exception TsmpServerException
	 *                if there is a general server related error.
	 * @exception TsmpServerUnavailableException
	 *                if the server is unavailable.
	 * @exception TsmpSendException
	 *                if there is an error when sending or receiving the request
	 * @exception TsmpRoutUnavailableException
	 *                if the send operation has failed because of ROUT failure
	 * @exception TsmpFileSystemException
	 *                if a file-system error occurs during the send operaiton 
	 * @deprecated As of T2716V10, replaced by
	 *             {@link #service(byte[],int,byte[])} and
	 *             {@link #service(GuardianOutput,GuardianInput)} and
	 *             {@link #service(GuardianOutput, byte[])}.
	 */
	public void service(TsmpServerRequest request, TsmpServerReply reply)
	throws TsmpServerException, TsmpSendException,
	TsmpServerUnavailableException, TsmpRoutUnavailableException,
	TsmpFileSystemException {

		/*
		 * Error handling array. element 0 = pathsend error, element 1 = file
		 * system error.
		 */
		int tsmpError[] = null;
		byte outArray[] = null;
		// objectArray is used to hold the byte array that holds the reply
		// buffer,
		// the byte array is constructed in the native clientRequest and
		// returned via this parameter.
		Object objectArray[] = new Object[1];

		int maxReplyLength = reply._replyLength;

		// Convert the instance variables to the appropriate data types.
		// marshal throws TsmpRequestException if it cannot convert the
		// data.
		request.marshal();

		// No exception was thrown from marshal, so make the request.
		// Call the native method that makes the call to the pthreads
		// SERVERCLASS_SEND_ routine CMA__SC_SEND.
		// The native method subsequently calls CMA__SC_SEND_INFO
		// to return error information so we can throw the appropriate
		// exception if necessary.
		// objectArray is array of Object, so it is passed by referece.

		// tsmpError = clientRequest(this.pathmonName, this.serverClassName,
		// request._requestBuffer, objectArray,
		// maxReplyLength);

		// Begin of Change Solution Number 10-030808-8620

		tsmpError = clientRequest(this.pathmonName, this.serverClassName,
				request._requestBuffer, objectArray, maxReplyLength,
				this.timeout);
		// End of Change

		if (tsmpError[0] != 0) {
			switch (tsmpError[0]) {
			case -1:
				// This only occurs with the deprecated service method
				throw new TsmpServerException(
				"The server returned a reply with length of more than the maximum allowable reply length of 32,000 bytes.");
			case -2:
				// This only occurs with the deprecated service method
				throw new TsmpServerException(
				"Maximum expected reply length must have a value ranging between 1 and 32000.");
			 /* Changes Begin for sol_10-131017-9304 */
			case -3:
			     throw new TsmpServerException(
				 "The server returned a reply with length less than 1 byte");
			 /* Changes End for sol_10-131017-9304 */
			default:
				interpretResults(tsmpError[0], tsmpError[1]);
			}
		}

		// No exception was thrown from the actual SERVERCLASS_SEND_
		// Set the outarray to the objectArray
		// Copy the servers reply into the reply buffer
		// Set actualReplyLength of reply
		outArray = (byte[]) objectArray[0];
		reply.setBuffer(outArray);
		reply.setActualReplyLength(outArray.length);
		// Move the data returned by the legacy TS/MP server to the
		// instance variables of the class. unmarshal throws TsmpReplyException
		// if there is an error converting data.
		reply.unmarshal();
	} // end deprecated service method

	/**
	 * Used to return a String object representing this TsmpServer's value.
	 */
	public String toString() {
		String TSMPString = "Pathmon.Serverclass Name: "
			+ this.fullServerClassName;
		return TSMPString;
	}

	// --- Private Helper Methods ---
	// change
	private void interpretResults(int psError, int fsError)
	throws TsmpSendException, TsmpServerUnavailableException,
	TsmpRoutUnavailableException, TsmpFileSystemException {

		// Handle Pathmon error result appropriately. Error messages are
		// differentiated by Pathsend error numbers only, not file system
		// error numbers. A generic message can be thrown in which case the
		// code below becomes much simpler (i.e all the "sub" switches go
		// away). Code was left in just in case we need to differentiate
		// error messages based on file system errors as well.

		if (psError != 0) {
			switch (psError) {
			case FEScInvalidServerClassName:
				throw new TsmpSendException("Invalid serverclass name.",
						psError, fsError, this.fullServerClassName);
			case FEScInvalidPathmonName:
				throw new TsmpSendException("Invalid Pathmon process name.",
						psError, fsError, fullServerClassName);
			case FEScPathmonConnect:
				throw new TsmpServerUnavailableException(
						"An error has occurred in the requester's communication with the Pathmon process.",
						psError, fsError, this.fullServerClassName);
			case FEScPathmonMessage:
				throw new TsmpServerUnavailableException(
						"LINKMON received and unrecognizable message from the PATHMON process.",
						psError, fsError, this.fullServerClassName);
			case FEScServerLinkConnect:
				throw new TsmpServerUnavailableException(
						"An error has occurred with the link to the server.",
						psError, fsError, this.fullServerClassName);
			case FEScNoServerLinkAvailable:
				throw new TsmpServerUnavailableException(
						"LINKMON had no links to the server class available.",
						psError, fsError, this.fullServerClassName);
			case FEScInvalidBufferLength:
				throw new TsmpSendException(
						"The buffer length in the request length or maximum reply length is invalid.",
						psError, fsError, this.fullServerClassName);
			case FEScServerClassFrozen:
				throw new TsmpServerUnavailableException(
						"The server class is frozen.", psError, fsError,
						this.fullServerClassName);
			case FEScUnknownServerClass:
				throw new TsmpServerUnavailableException(
						"The server class is not configured.", psError,
						fsError, this.fullServerClassName);
			case FEScPathmonShutdown:
				throw new TsmpSendException(
						"The send operation has been denied.", psError,
						fsError, this.fullServerClassName);
			case FEScServerCreationFailure:
				throw new TsmpServerUnavailableException(
						"Server creation failure.", psError, fsError,
						this.fullServerClassName);
			case FEScServerClassTmfViolation:
				throw new TsmpSendException(
						"The transaction mode of the Pathmon process does not match that of the server class.",
						psError, fsError, this.fullServerClassName);
			case FEScSendOperationAborted:
				throw new TsmpSendException(
						"The send operation has been terminated at an indeterminate point.",
						psError, fsError, this.fullServerClassName);
			case FEScInvalidTimeoutValue:
				throw new TsmpSendException("Invalid Time out value", psError,
						fsError, this.fullServerClassName);
			case FEScPFSUseError:
				throw new TsmpSendException(
						"The callers process file segment could not be accessed.",
						psError, fsError, this.fullServerClassName);
			case FEScTooManyPathmons:
				throw new TsmpServerUnavailableException(
						"The maximun number of PATHMON processes are already in use.",
						psError, fsError, this.fullServerClassName);
			case FEScTooManyServerClasses:
				throw new TsmpServerUnavailableException(
						"The maximum number of server classes are already in use.",
						psError, fsError, this.fullServerClassName);
			case FEScTooManyServerLinks:
				throw new TsmpServerUnavailableException(
						"The maximum number of server links are already in use.",
						psError, fsError, this.fullServerClassName);
			case FEScTooManySendRequests:
				throw new TsmpServerUnavailableException(
						"The maximum number of concurrent serverclass send operations has been exceeded.",
						psError, fsError, this.fullServerClassName);
			case FEScTooManyRequesters:
				throw new TsmpServerUnavailableException(
						"The maximum number of requesters allowed has been reached.",
						psError, fsError, this.fullServerClassName);
			case FEScLinkmonConnect:
				throw new TsmpServerUnavailableException(
						"Problem communicating with the LINKMON process.",
						psError, fsError, this.fullServerClassName);
			default:
				throw new TsmpSendException("Pathsend error.", psError,
						fsError, this.fullServerClassName);
			}

		}
		//	Changes begin for solution:10-081125-7556 
		if (psError == 0 && fsError != 0) {
			switch (fsError) {

			case FEPathDown:
				throw new TsmpRoutUnavailableException(
						"Path to Rout is down. Re-initiate the send", fsError,
						this.fullServerClassName);
			default:
				throw new TsmpFileSystemException(fsError,
						this.fullServerClassName);
			}
		}
		//		Changes end  for solution:10-081125-7556 
	} // end interpretResults

	// --- Native Methods ---
	/**
	 * Performs serverclass_send_ call on behalf of non-deprecated service
	 * methods..
	 */
	// Begin of Change Solution Number 10-030808-8620
	// change
	private final native int[] scSend(String pathmonName,
			String serverClassName, byte[] request, int requestLen,
			byte[] reply, int maxReplyLength, int[] actualReplyLength,
			int timeout, int pathSendMode);

	// End of Change

	/**
	 * Performs serverclass_send_ call on behalf of deprecated service method.
	 */

	// Begin of Change Solution Number 10-030808-8620
	private final native int[] clientRequest(String pathmonName,
			String serverClassName, byte[] inbuf, Object[] objectArray,
			int expectedReplyLength, int timeout);
	
	// End of Change
}
