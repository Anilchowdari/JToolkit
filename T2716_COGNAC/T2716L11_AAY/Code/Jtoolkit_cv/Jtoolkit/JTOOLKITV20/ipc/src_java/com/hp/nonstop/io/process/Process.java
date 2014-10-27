/*
 * Copyright 2010
 * Hewlett Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Hewlett Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett Packard Development Company, L.P.
 */
package com.hp.nonstop.io.process;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.hp.nonstop.io.process.impl.NSProcessChannelHelper;
import com.hp.nonstop.io.process.impl.NSProcessData;
import com.hp.nonstop.io.process.impl.NSProcessException;
import com.hp.nonstop.io.process.impl.NativeStatus;
import com.tandem.ext.guardian.GuardianInput;
import com.tandem.ext.guardian.GuardianOutput;
import com.tandem.ext.util.DataConversionException;

/**
 * <P>
 * A class that can be used to communicate with a NonStop server process using
 * NonStop File System(FS) InterProcess Communication (IPC).
 * </p>
 * 
 * <p>
 * The following are the terms used in this document:
 * <ul>
 * <li>Requester: A java application on NonStop that uses this class to
 * communicate with a NonStop server process</li>
 * <li>Server: A process on NonStop that the requester wants to communicate
 * with. This process opens the $RECEIVE for IPC. The server can be written in
 * any programming language supported on NonStop. The server can either be a
 * Guardian or an OSS process</li>
 * </ul>
 * </p>
 * 
 * <p>
 * Using this class the requester can engage the server either in two-way
 * communication or in one-way communication.
 * <ul>
 * <li>Two-way communication: Here the requester sends a message to the server.
 * After processing the message the server replies to the message. See
 * {@link ProcessOpenOption#ACCESS_READ_WRITE}
 * <li>One-way communication: Here the requester sends a message to the server
 * and does not receive (or expect) a response from the server. See
 * {@link ProcessOpenOption#ACCESS_WRITE_ONLY}
 * </ul>
 * </p>
 * 
 * <h2>Stream based API</h2>
 * This class provides concrete implementations of
 * <code>java.io.InputStream</code> and <code>java.io.OutputStream</code>
 * for communicating with the server process. The OutputStream is used to write
 * data to the server and the InputStream is used to read the response from the
 * server. Following is a code snippet that shows the usage of the output and
 * input streams.<br/>
 * 
 * <p style="padding-left:5%">
 * <code>
 * .....<br/>
 * .....<br/>
 * Process p = new Process(new ProcessIdentifier("TEST"));<br/>
 * p.open();<br/>
 * OutputStreamWriter out = new OutputStreamWriter(p.getOutputStream());<br/>
 * out.write("Pack my box with five dozen liquor jugs");<br/>
 * out.flush();<br/>
 * .....<br/>
 * BufferedReader read = new BufferedReader(new InputStreamReader(p.getInputStream()));<br/>
 * System.out.println("The response from the server is "+read.readLine());<br/>
 * ....<br/>
 * 
 * </code>
 * </p>
 * 
 * <p>
 * Per <code>Process</code> instance, at any given time there can be a maximum
 * of only one outstanding I/O. What this means is once data is written
 * (flushed) to the server, before the server replies, another write to the
 * server cannot happen. For example the following is not allowed as this would
 * results in 2 outstanding I/O to the server<br/>
 * 
 * <p style="padding-left:5%">
 * <code>
 * .....<br/>
 * .....<br/>
 * Process p = new Process(new ProcessIdentifier("TEST"));<br/>
 * p.open();<br/>
 * OutputStreamWriter out = new OutputStreamWriter(p.getOutputStream());<br/>
 * out.write("Pack my box with five dozen liquor jugs");<br/>
 * out.flush();//<b>This would result in the first message sent to the server. So one outstanding I/O</b><br/>
 * <span style="color:red">
 * out.write("This is the second message");<br/>
 * out.flush();//<b>This would result in the second outstanding I/O to the server</b><br/>
 * </span>
 * .....<br/>
 * </code>
 * <p>
 * 
 * </p>
 * 
 * <p>
 * The InputStream and OutputStream objects can be used in different thread
 * contexts. This means while one thread does the writes to the server the other
 * thread can perform the reads from the server. If the message sent to the
 * server has to be within a transaction (initiated by the user application)
 * then its necessary to use the stream objects in the same thread that
 * initiated the transaction.
 * </p>
 * 
 * <h2>Tag generator</h2>
 * Every message sent to the server is identified with a unique tag. Tags are
 * used to match the response with the corresponding request.<br/> The
 * {@link #setTagGenerator(TagGenerator)} method can be used to set a custom
 * {@link TagGenerator}
 * 
 * <h2>Message based API</h2>
 * The Message based API has three methods.
 * {@link #service(GuardianOutput, byte[])},
 * {@link #service(GuardianOutput, GuardianInput)},
 * {@link #service(byte[], int, byte[])} Signature of these methods match those
 * in the {@link com.tandem.tsmp.TsmpServer} and
 * {@link com.tandem.tsmp.TsmpDialog} classes.
 * 
 * @see ProcessIdentifier
 * @see ProcessOpenOption
 * 
 * @author Vyom Tewari
 * 
 */
public class Process {

	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.Process");

	protected static StringManager sm = StringManager.getManager("resources");

	private NSProcessChannelHelper helper;

	// filenum indicates if the server process is open or not. if the value is
	// -1 then the process is not open
	// if the value is != -1 then the server is open
	volatile short filenum = -1;
	// WARNING: Do not use tag as an indication of whether an I/O is done or not
	volatile private int tag = -1;

	// Indicates if a message is sent to the server process
	// Should be set to true only if a message is successfully sent to the
	// server
	// should be reset only if the a messages is successfully read from the
	// server
	// in case of write-only mode this should be reset always !!!
	// I was tempted to use the value of "tag" to determine this
	// but that is not possible because tag can be <> -1 even without the write
	// being called. THis can happen if a read is called first and then a write
	// Could have used ProcessState.
	// TODO
	// Replace the usage for messagesent with that of ProcessState
	volatile private boolean messagesent = false;

	private ProcessIdentifier id = null;
	private ProcessOpenOption option = null;
	private ProcessOutputStream outputStream;
	private ProcessInputStream inputStream;

	TagGenerator generator = new DefaultTagGenerator();

	private ProcessState state = new ProcessState(ProcessState.INIT);

	/**
	 * Constructor with the default {@link ProcessOpenOption}
	 * 
	 * @param id
	 *            The ProcessIdentifier for the process to connect.
	 * @throws IllegalArgumentException
	 *             If process identifier is null
	 */
	public Process(ProcessIdentifier id) throws IllegalArgumentException {
		this(id, new ProcessOpenOption());
	}

	/**
	 * @param id
	 *            The ProcessIdentifier for the process to connect
	 * @param option
	 *            The ProcessOpenOption for the process to connect
	 * @throws IllegalArgumentException
	 *             If process identifier or process open option is null
	 */
	public Process(ProcessIdentifier id, ProcessOpenOption option)
			throws IllegalArgumentException {
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("Constructor[" + id.toString() + "]["
					+ option.toString() + "]");
		}
		if (id == null)
			throw new IllegalArgumentException(sm
					.getString("process.identifier.null"));
		if (option == null)
			throw new IllegalArgumentException(sm
					.getString("process.options.null"));
		this.id = id;
		this.option = option;
		helper = NSProcessChannelHelper.getInstance();

		if (helper == null) {
			throw new NullPointerException(sm.getString("helper.is.null"));
		}
	}

	/**
	 * Checks if the server process is open or not. If the server process was
	 * previously opened by call to {@link #open()} this return
	 * <code>true</code>. This does not check the current state of the link
	 * between the requester and the server. This method just serves as an
	 * indication if {@link #open()} was called successfully once.
	 * 
	 * @return <code>true</code> if the connection is open. <BR>
	 *         <code>false</code> if the connection has been closed or was
	 *         never opened.
	 */
	public boolean isOpen() {
		return (filenum != -1);
	}

	/**
	 * Indicates if there is an outstanding I/O on the server process. An I/O is
	 * considered outstanding if the response to a message sent to the server is
	 * not yet read. This call returns valid data only if the server process is
	 * open
	 * 
	 * @see #isOpen()
	 * @return <code>true</code> if there is an outstanding I/O. <BR>
	 *         <code>false</code> if there is no outstanding I/O
	 */
	public boolean isIOOutstanding() {
		return messagesent;
	}

	/**
	 * Opens the server process thus creating a link between the requester and
	 * the server process. An attempt to obtain the stream objects will result
	 * in an exception if called before opening the server process.
	 * 
	 * @throws ServerProcessNotFoundException
	 *             If server process is not running.
	 * @throws ProcessIOException
	 *             If an error occurs while opening the server process.
	 * @throws IllegalStateException
	 *             If the server process is already open
	 */
	public void open() throws ProcessIOException,
			ServerProcessNotFoundException, IllegalStateException {
		if (isOpen() == true) {
			throw new IllegalStateException(sm
					.getString("process.already.open"));
		}
		try {
			filenum = helper.open(id, option);
			if (logger.isLoggable(Level.FINE)) {
				logger.fine("Opened Process[" + id.toString() + "]["
						+ option.toString() + "]Filenum:" + filenum);
			}
			state.state = ProcessState.OPEN;
		} catch (NSProcessException exception) {
			if (exception.getError() == NativeStatus.FE_DEVICE_NOT_FOUND) {
				throw new ServerProcessNotFoundException(
						exception.getMessage(), exception.getError());
			} else {
				throw new ProcessIOException(exception.getMessage(), exception
						.getError());
			}
		}
	}

	/**
	 * Closes the link with the server process and releases any internal
	 * resources
	 * 
	 * @throws ProcessIOException
	 *             If an error occurs while closing the connection.
	 * @throws IllegalStateException
	 *             If the method is invoked when the process is not open
	 */
	public void close() throws ProcessIOException, IllegalStateException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}
		try {
			if (inputStream != null)
				inputStream.close();
			if (outputStream != null)
				outputStream.close();
		} catch (NSProcessException exception) {
			throw new ProcessIOException(exception.getMessage(), exception
					.getError());
		} catch (IOException exception) {
			throw new ProcessIOException(exception.getMessage());
		}
		if (helper != null) {
			try {
				helper.close(filenum);
				state.state = ProcessState.CLOSED;
				filenum = -1;
				resetTagInternal();
				messagesent = false;
			} catch (NSProcessException exception) {
				throw new ProcessIOException(exception.getMessage(), exception
						.getError());
			}
		}
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("Successfully closed Process:" + id.toString()
					+ ":Filenum:" + filenum);
		}
	}

	/**
	 * Cancel the oldest incomplete operation on this process. The method first
	 * checks to see if a message has been sent to the server.If no message is
	 * sent to the server an <code>IllegalStateException</code> is thrown. If
	 * a message is indeed sent to the server the method tries to cancel the
	 * I/O. <BR>
	 * It could happen that the server process has already responded to the
	 * message and there is no outstanding I/O. In this case a
	 * <code>ProcessIOException</code> is thrown. An error code of 26 in the
	 * <code>ProcessIOException</code> indicates this condition. In such a
	 * case the application is expected to read the response before attempting
	 * to send the next message to the server.
	 * 
	 * 
	 * @throws ProcessIOException
	 *             Thrown if any error happen in canceling the IO operation.
	 * @throws IllegalStateException
	 *             Thrown if the method is invoked without sending a message to
	 *             the server or opening the process
	 */
	public void cancel() throws ProcessIOException, IllegalStateException {
		if ((isOpen() == true) && (isIOOutstanding() == true)) {
			try {
				helper.cancel(filenum, tag);
				resetTagInternal();
				messagesent = false;
			} catch (NSProcessException exception) {
				if (exception.getError() == NativeStatus.FS_NO_OUTSTANDING_IO) {
					throw new ProcessIOException(sm
							.getString("response.already.obtained"), exception
							.getError());
				}
				throw new ProcessIOException(exception.getMessage(), exception
						.getError());
			}
		} else if ((isOpen() == true) && (isIOOutstanding() == false)) {
			// This means the file is open but there are no outstanding i/o
			// operations
			throw new IllegalStateException(sm.getString("no.outstanding.io"));
		} else if (isOpen() == false) {
			// A cancel is invoked when process is not open !!
			throw new IllegalStateException(sm.getString("process.not.open"));
		}

	}

	/**
	 * Returns the input stream associated with this Process object.
	 * 
	 * @return The input stream to read response from the Server process.
	 * @throws IOException
	 * 
	 * @throws IOException
	 *             Thrown if any error happen in obtaining the input stream.
	 * @throws IllegalStateException
	 *             If this method is invoked before opening the server process
	 */
	public ProcessInputStream getInputStream() throws IOException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}
		int messageSize = option.getMaxResponseMessageSize();
		if (inputStream == null)
			inputStream = new InputStreamImpl(messageSize);
		else
			inputStream.open();
		return inputStream;
	}

	/**
	 * Returns the output stream associated with the Process object.
	 * 
	 * @return The output stream for writing data, to the server process.
	 * 
	 * @throws IOException
	 *             Thrown if any error happen in obtaining the output stream.
	 * @throws IllegalStateException
	 *             If this method is invoked before opening the server process
	 * 
	 */
	public ProcessOutputStream getOutputStream() throws IOException,
			IllegalStateException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}
		int messageSize = option.getMaxRequestMessageSize();
		if (outputStream == null)
			outputStream = new OutputStreamImpl(messageSize);
		else
			outputStream.open();
		return outputStream;
	}

	/**
	 * This is a convenience method to send a request to the <i>Server</i> and
	 * wait for a response. A call to this method blocks the calling thread till
	 * a response from the server or an exception.
	 * 
	 * @param request
	 *            The data to be sent to the server
	 * @param length
	 *            The number of bytes to send. Must be less than or equal to the
	 *            size of request byte buffer.
	 * @param response
	 *            Buffer that contains the response from the server. If the
	 *            server response exceeds the size of this buffer data is
	 *            truncated to fit this buffer
	 * @return The actual number of bytes in the server reply.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 * @throws IllegalStateException
	 *             If this method is invoked before opening the server process
	 * @throws IllegalArgumentException
	 *             If length is greater then request length.
	 */
	public int service(byte[] request, int length, byte[] response)
			throws IOException, IllegalArgumentException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}

		if (request == null)
			throw new NullPointerException(sm.getString("request.is.null"));
		if (response == null)
			throw new NullPointerException(sm.getString("response.is.null"));
		if (length > request.length)
			throw new IllegalArgumentException(sm
					.getString("invalid.length.param"));
		int acc = option.getAccess();
		int byteRead = 0;
		if (acc == ProcessOpenOption.ACCESS_READ_WRITE) {
			InputStream inp = getInputStream();
			OutputStream out = getOutputStream();
			if (out != null && inp != null) {
				out.write(request, 0, length);
				out.flush();
				byteRead = inp.read(response);
			}
		} else {
			OutputStream out = getOutputStream();
			if (out != null) {
				out.write(request, 0, length);
				out.flush();
			}
			byteRead = -1;
		}
		return byteRead;
	}

	/**
	 * This is a convenience method to send a request to the <i>Server</i> and
	 * wait for a response. A call to this method blocks the calling thread till
	 * a response from the server or an exception.
	 * 
	 * @param request
	 *            The request to be sent to the server.
	 * @param response
	 *            The response from the server.
	 * @return the actual number of bytes in the server reply.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 * @throws IllegalStateException
	 *             If this method is invoked before opening the server process
	 * 
	 * @throws DataConversionException
	 *             If there is an error marshaling the request data from
	 *             <code>request</code> into a byte array or if there is an
	 *             error unmarshaling the server reply into the fields of
	 *             <code>response</code>.
	 */
	public int service(GuardianOutput request, GuardianInput response)
			throws IOException, DataConversionException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}

		if (request == null)
			throw new NullPointerException(sm.getString("request.is.null"));
		if (response == null)
			throw new NullPointerException(sm.getString("response.is.null"));
		int byteRead = 0;
		byte[] requestByte = request.marshal();
		int len = response.getLength();
		byte[] replyByte = new byte[len];
		byteRead = service(requestByte, requestByte.length, replyByte);
		response.unmarshal(replyByte, byteRead);
		return byteRead;
	}

	/**
	 * This is a convenience method to send a request to the <i>Server</i> and
	 * wait for a response. A call to this method blocks the calling thread till
	 * a response from the server or an exception.
	 * 
	 * @param request
	 *            The request to be sent to the server.
	 * @param response
	 *            The reply from the server, replies from the server that exceed
	 *            the length of this array will be truncated.
	 * 
	 * @return Actual number of bytes in the server reply.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 * @throws IllegalStateException
	 *             If this method is invoked before opening the server process
	 * 
	 * @throws DataConversionException
	 *             If there is an error marshaling the request data from
	 *             <code>request</code> into a byte array.
	 */
	public int service(GuardianOutput request, byte[] response)
			throws IOException, DataConversionException {
		if (isOpen() == false) {
			throw new IllegalStateException(sm.getString("process.not.open"));
		}

		if (request == null)
			throw new NullPointerException(sm.getString("request.is.null"));
		if (response == null)
			throw new NullPointerException(sm.getString("response.is.null"));
		int byteRead = 0;
		byte[] requestByte = request.marshal();
		byteRead = service(requestByte, requestByte.length, response);
		return byteRead;
	}

	/**
	 * Returns the ProcessIdentifier associated with the Process.
	 * 
	 * @return the ProcessIdentifier associated with the Process.
	 */
	public ProcessIdentifier getProcessIdentifier() {
		return id;
	}

	/**
	 * Returns the ProcessOpenOption associated with the Process.
	 * 
	 * @return The ProcessOpenOption associated with the Process.
	 */
	public ProcessOpenOption getProcessOpenOption() {
		return option;
	}

	/**
	 * Sets the tag generator. Tag generator generates the tags for the write
	 * operations done on the server process. Before flushing data to the server
	 * process, getNextTag() on the the tag generator will be called, the number
	 * returned will be used in the write operation done to the server process.
	 * If the tag generator is not set or the tag generator returns an invalid
	 * value, randomly generated tag will be used in the write operation.
	 * 
	 * 
	 * @param generator
	 *            Generates the tags for the write operations done on the server
	 *            process.
	 */
	public void setTagGenerator(TagGenerator generator)
			throws IllegalArgumentException {
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("A custom tag generator is set:"
					+ generator.getClass().getName());
		}
		this.generator = generator;
	}

	protected void finalize() throws Throwable {
		try {
			if (isOpen()) {
				close();
			}
		} finally {
			super.finalize();
		}
	}

	private void resetTagInternal() {
		tag = -1;
	}

	private static final byte[] HEX_CHAR = new byte[] { '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	private static final String dumpBytes(byte[] buffer, int length) {
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < length; i++) {
			sb.append("0x").append((char) HEX_CHAR[(buffer[i] & 0x00F0) >> 4])
					.append((char) HEX_CHAR[(buffer[i] & 0x00F0) >> 4]);
		}
		return sb.toString();
	}

	class OutputStreamImpl extends ProcessOutputStream {

		public OutputStreamImpl(int bufferSize) {
			super(bufferSize);
		}

		/**
		 * Generate a unique tag.
		 * 
		 * @return The generated tag.
		 */
		protected int generateTag() {
			if (tag == -1) {
				tag = generator.getNextTag();
			}
			if (logger.isLoggable(Level.FINE)) {
				StringBuffer buf = new StringBuffer();
				buf.append("Generated a tag:").append(tag);
				logger.fine(buf.toString());
			}
			return tag;
		}

		/**
		 * Internal write method that will calls the native write..
		 * 
		 * @throws ProcessIOException
		 * 
		 * 
		 */
		protected void writeInternal(int tag1, byte[] buf, int off, int len)
				throws ProcessIOException {
			NSProcessData data = null;
			try {
				data = new NSProcessData(filenum, tag1);
			} catch (NSProcessException e) {
				throw new ProcessIOException(e);
			}
			data.setData(buf);
			short acc = option.getAccess();
			if (helper != null) {
				if (acc == ProcessOpenOption.ACCESS_READ_WRITE) {
					try {
						helper.write(data, len, option
								.getMaxResponseMessageSize());
						messagesent = true;
						if (logger.isLoggable(Level.FINEST)) {
							StringBuffer sb = new StringBuffer();
							sb.append("Writing following bytes:").append(
									dumpBytes(buf, len));
						}
					} catch (NSProcessException ex) {
						if (ex.getError() == NativeStatus.LARGE_FS_CALL_NOT_FOUND) {
							throw new LargeMessageNotSupportedException(sm
									.getString("large.message.not.supported"));
						} else {
							throw new ProcessIOException(ex.getMessage(), ex
									.getError());
						}
					}
				} else {
					try {
						helper
								.write(data, option.getMaxRequestMessageSize(),
										0);
						NSProcessData dummyData = new NSProcessData(filenum,
								tag1);
						helper.read(dummyData, option.getTimeout());
						resetTagInternal();
						messagesent = false;
					} catch (NSProcessException ex) {
						if (ex.getError() == NativeStatus.LARGE_FS_CALL_NOT_FOUND) {
							throw new LargeMessageNotSupportedException(sm
									.getString("large.message.not.supported"));
						} else if (ex.getError() == NativeStatus.TIMEDOUT) {
							// this will never happen if the message system is
							// alright. We are in the write-only mode and the
							// write method should complete as soon
							// as the message is delivered to the server process
							throw new ProcessTimeoutException(sm
									.getString("operation.timeout"));
						} else {
							throw new ProcessIOException(ex.getMessage(), ex
									.getError());
						}
					}
				}
			} else {
				throw new NullPointerException(sm.getString("helper.is.null"));
			}
		}

		/**
		 * Gets the current state of process.
		 * 
		 * @return The process state.
		 */
		protected ProcessState getProcessState() {
			return state;
		}

		/**
		 * Sets the current state of process.
		 * 
		 * @param state
		 *            New process state
		 */
		protected void setProcessState(final byte newState) {
			state.state = newState;
		}

		protected boolean isIOPending() {
			return isIOOutstanding();
		}
	}

	class InputStreamImpl extends ProcessInputStream {

		int retcode = 0;

		public InputStreamImpl(int bufferSize) {
			super(bufferSize);
		}

		/**
		 * Internal read method that will calls the native read..
		 * 
		 * @return true is read successful false otherwise.
		 * @throws ProcessIOException
		 */
		protected boolean readInternal(int tag) throws ProcessIOException {
			short acc = option.getAccess();
			if (acc == ProcessOpenOption.ACCESS_WRITE_ONLY) {
				throw new IllegalOperationException(sm
						.getString("process.write.only.mode"));
			}
			NSProcessData data = null;
			try {
				data = new NSProcessData(filenum, tag);
			} catch (NSProcessException e) {
				StringWriter writer = new StringWriter();
				e.printStackTrace(new PrintWriter(writer, true));
				logger.severe("Unexpected exception\n" + writer.toString());
				return false;
			}

			boolean flag = false;
			try {
				flag = helper.read(data, option.getTimeout());
				messagesent = false;
				if (data.getError() != 0
						&& data.getError() != NativeStatus.FE_CONTINUE) {
					if (data.getError() == NativeStatus.FE_DEVICE_IS_DOWN) {
						throw new ServerProcessNotFoundException(sm
								.getString("process.is.down"));
					} else if (data.getError() == NativeStatus.OPERATION_CANCELLED) {
						throw new ProcessCancelException(sm
								.getString("operation.cancelled"));
					} else {
						throw new ProcessIOException(
								data.getErrorDescription(), data.getError());
					}
				} else {
					retcode = data.getError();
				}
			} catch (NSProcessException e) {
				if (e.getError() == NativeStatus.TIMEDOUT) {
					// Actually this is the only error after which issuing
					// another read could fetch data so not resetting the
					// "messagesent" flag
					throw new ProcessTimeoutException(sm
							.getString("operation.timeout"));
				}
				messagesent = false;
				ProcessIOException ex = new ProcessIOException(e);
				throw ex;
			}
			if (flag) {
				if (buffer.remaining() > 0) {
					logger
							.log(
									Level.WARNING,
									"The ProcessInputStream has some buffered data, possibly from a server response to the previous message, that is not yet read. This data is going to be overwritten.");
				}
				/*
				 * this call ensures that the position is 0, limit and capacity
				 * are set to the maximum capacity of the backing buffer its a
				 * must to do this before you put data
				 */
				buffer.clear();
				/*
				 * Put data from the point of 'position' In our case the data
				 * being put to the buffer will never be more than the size of
				 * the backing array. THINK ABOUT IT!!!
				 */
				buffer.put(data.getData());
				/*
				 * this will ensure that the limit is set to length of data put
				 * in the previous step. using flip has the added advantage in
				 * that the buffer will not allow you to read anything more than
				 * the data you just put in the buffer
				 */
				buffer.flip();
				if (logger.isLoggable(Level.FINEST)) {
					StringBuffer sb = new StringBuffer();
					sb.append("Read following bytes:").append(
							dumpBytes(data.getData(), data.getData().length));
				}
			}
			return flag;
		}

		/**
		 * Generate a unique tag.
		 * 
		 * @return The generated tag.
		 */
		protected int generateTag() {
			if (tag == -1) {
				tag = generator.getNextTag();
			}
			if (logger.isLoggable(Level.FINE)) {
				StringBuffer buf = new StringBuffer();
				buf.append("Generated a tag:").append(tag);
				logger.fine(buf.toString());
			}
			return tag;
		}

		/**
		 * Reset the tag to -1 for the process.
		 * 
		 */
		protected void resetTag() {
			resetTagInternal();
		}

		/**
		 * Gets the current state of process.
		 * 
		 * @return The process state.
		 */
		protected ProcessState getProcessState() {
			return state;
		}

		/**
		 * Sets the current state of process.
		 * 
		 * @param state
		 *            New process state
		 */
		protected void setProcessState(final byte newState) {
			state.state = newState;
		}

		public boolean isResponseComplete() {
			return (retcode == 0);
		}
	}

	class DefaultTagGenerator implements TagGenerator {
		Random rnd = new Random((new Date().getTime()));

		/**
		 * Generate the next tag.
		 * 
		 * @return the generated tag.
		 */
		public int getNextTag() {
			int ret = rnd.nextInt(Integer.MAX_VALUE);
			while (ret == 0) {
				ret = rnd.nextInt(Integer.MAX_VALUE);
			}
			return ret;
		}
	}

}
