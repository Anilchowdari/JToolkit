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
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * An InputStream to read data from the <i>Server</i> process.
 * 
 * 
 * <p>
 * The following scenarios explain the behavior of the <code>read</code>
 * method.
 * 
 * <h2>For one-way communication</h2>
 * <b>A call to any of the read methods will throw an IOException.</b>
 * 
 * <h2>For two-way communication</h2>
 * 
 * The behavior of <code>read</code> depends on whether the requester has sent
 * a message to the server process or not. The following explains the various
 * scenarios
 * <ul>
 * <li>If no message has been sent to the server, a call to <code>read</code>
 * blocks the calling thread. This situation can occur if the application uses
 * two different threads, one for write and the other for read. The thread that
 * executes the <code>read</code> may not know if the other thread has written
 * data to the server or not. The <code>read</code> method will anyway
 * complete upon timeout</li>
 * 
 * 
 * <li>If a message has already been sent to the server process, the
 * <code>read</code> waits till a response is obtained or a timeout occurs.
 * Upon obtaining a response and all data in the response is read,
 * <code>read</code> return <code>EOF</code> (-1). Any further calls to
 * <code>read</code> continues to return <code>EOF</code> till another
 * message is sent to the server. Once a message is sent to the server a call to
 * <code>read</code> blocks the calling thread</li>
 * 
 * 
 * <li>After a successful write to the server process, any errors in the
 * communication or message delivery will be notified by a call to any of the
 * <code>read</code> methods.</li>
 * 
 * </ul>
 * </p>
 * 
 * @author Vyom Tewari
 * 
 */
public abstract class ProcessInputStream extends InputStream {

	private boolean closed = false;

	/**
	 * The internal buffer, which will be used to store data(response) from
	 * server.
	 * 
	 */
	protected ByteBuffer buffer = null;
	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.ProcessInputStream");

	/**
	 * Constructs the input stream for communicating with the Process object.
	 * 
	 * @param buffersize
	 *            The internal buffer that will be used to store response from
	 *            the server.
	 */
	ProcessInputStream(int bufferSize) {
		buffer = ByteBuffer.allocate(bufferSize);
		/*
		 * a flip is called for multiple purposes. A> This will ensure that no
		 * data can be read from the buffer until some data is put in the buffer
		 * B> The very first readInternal will check if there are any remaining
		 * bytes to be read. now if I do not do this flip here, then the
		 * readInternal will think that there are some bytes to be read and will
		 * throw a warning message.
		 */
		buffer.flip();
	}

	/**
	 * Reads the next byte of data from the process input stream. The value byte
	 * is returned as an <code>int</code> in the range <code>0</code> to
	 * <code>255</code>. If no byte is available because the end of the
	 * stream has been reached, the value <code>-1</code> is returned. This
	 * method blocks until input data is available, the end of the stream is
	 * detected, or an exception is thrown.
	 * 
	 * 
	 * @return The next byte of data, or <code>-1</code> if the end of the
	 *         stream is reached.
	 * @exception IOException
	 *                If an I/O error occurs.
	 * @exception ProcessTimeoutException
	 *                If request got timed out.
	 * @exception ProcessCancelException
	 *                If request canceled by file system or explicitly by user.
	 */
	public int read() throws IOException {
		logger.log(Level.FINER, "Inside ProcessInputStream.read() method.");
		checkProcessOpen();
		checkStreamOpen();
		ProcessState ps = getProcessState();
		if (ps.isRequestSent() || ps.isOpen()) {
			int tag = generateTag();
			readInternal(tag);
			resetTag();
			setProcessState(ProcessState.REPLY_OBTAINED);
		}
		if (buffer.remaining() > 0) {
			return buffer.get();
		} else {
			return -1;
		}
	}

	/**
	 * Reads some number of bytes from the process input stream and stores them
	 * into the buffer array <code>b</code>. The number of bytes actually
	 * read is returned as an integer. This method blocks until input data is
	 * available, end of file is detected, or an exception is thrown.
	 * 
	 * <p>
	 * If the length of <code>b</code> is zero, then no bytes are read and
	 * <code>0</code> is returned; otherwise, there is an attempt to read at
	 * least one byte. If no byte is available because the stream is at the end
	 * of the file, the value <code>-1</code> is returned; otherwise, at least
	 * one byte is read and stored into <code>b</code>.
	 * 
	 * <p>
	 * The first byte read is stored into element <code>b[0]</code>, the next
	 * one into <code>b[1]</code>, and so on. The number of bytes read is, at
	 * most, equal to the length of <code>b</code>. Let <i>k</i> be the
	 * number of bytes actually read; these bytes will be stored in elements
	 * <code>b[0]</code> through <code>b[</code><i>k</i><code>-1]</code>,
	 * leaving elements <code>b[</code><i>k</i><code>]</code> through
	 * <code>b[b.length-1]</code> unaffected.
	 * 
	 * <p>
	 * The <code>read(b)</code> method for class
	 * <code>ProcessInputStream</code> has the same effect as:
	 * 
	 * <pre>
	 * <code>
	 * read(b, 0, b.length)
	 * </code>
	 * </pre>
	 * 
	 * <BR>
	 * If read is called before writing the data to the ProcessOutputStream, the
	 * method will block until the data is available.
	 * 
	 * <BR>
	 * 
	 * If the read is called after writing data to the ProcessOutputStream and
	 * the data is not available in ProcessInputStream, the read will be block
	 * until the data is available.If the data is available in the
	 * ProcessInputStream, data is read from the ProcessInputStream into the
	 * buffer b. <BR>
	 * 
	 * @param b
	 *            The buffer into which the data is read.
	 * @return The total number of bytes read into the buffer, or
	 *         <code>-1</code> is there is no more data because the end of the
	 *         stream has been reached.
	 * @exception IOException
	 *                If the first byte cannot be read for any reason other than
	 *                the end of the file, if the input stream has been closed,
	 *                or if some other I/O error occurs.
	 * @exception NullPointerException
	 *                If <code>b</code> is <code>null</code>.
	 * @exception ProcessTimeoutException
	 *                If request got timed out.
	 * @exception ProcessCancelException
	 *                If request canceled by file system or explicitly by user.
	 * @see ProcessInputStream#read(byte[], int, int)
	 */
	public int read(byte[] b) throws IOException {
		return read(b, 0, b.length);
	}

	/**
	 * Reads up to <code>len</code> bytes of data from the process input
	 * stream into an array of bytes. An attempt is made to read as many as
	 * <code>len</code> bytes, but a smaller number may be read. The number of
	 * bytes actually read is returned as an integer.
	 * 
	 * <p>
	 * This method blocks until input data is available, end of file is
	 * detected, or an exception is thrown.
	 * 
	 * <p>
	 * If <code>len</code> is zero, then no bytes are read and <code>0</code>
	 * is returned; otherwise, there is an attempt to read at least one byte. If
	 * no byte is available because the stream is at end of file, the value
	 * <code>-1</code> is returned; otherwise, at least one byte is read and
	 * stored into <code>b</code>.
	 * 
	 * <p>
	 * The first byte read is stored into element <code>b[off]</code>, the
	 * next one into <code>b[off+1]</code>, and so on. The number of bytes
	 * read is, at most, equal to <code>len</code>. Let <i>k</i> be the
	 * number of bytes actually read; these bytes will be stored in elements
	 * <code>b[off]</code> through <code>b[off+</code><i>k</i><code>-1]</code>,
	 * leaving elements <code>b[off+</code><i>k</i><code>]</code> through
	 * <code>b[off+len-1]</code> unaffected.
	 * 
	 * <p>
	 * In every case, elements <code>b[0]</code> through <code>b[off]</code>
	 * and elements <code>b[off+len]</code> through <code>b[b.length-1]</code>
	 * are unaffected.
	 * 
	 * 
	 * @param b
	 *            The buffer into which the data is read.
	 * @param off
	 *            The start offset in array <code>b</code> at which the data
	 *            is written.
	 * @param len
	 *            The maximum number of bytes to read.
	 * @return The total number of bytes read into the buffer, or
	 *         <code>-1</code> if there is no more data because the end of the
	 *         stream has been reached.
	 * @exception IOException
	 *                If the first byte cannot be read for any reason other than
	 *                end of file, or if the input stream has been closed, or if
	 *                some other I/O error occurs.
	 * @exception NullPointerException
	 *                If <code>b</code> is <code>null</code>.
	 * @exception IndexOutOfBoundsException
	 *                If <code>off</code> is negative, <code>len</code> is
	 *                negative, or <code>len</code> is greater than
	 *                <code>b.length - off</code>
	 * @exception ProcessTimeoutException
	 *                If request got timed out.
	 * @exception ProcessCancelException
	 *                If request canceled by file system or explicitly by user.
	 * @see java.io.InputStream#read(byte[] b,int off,int len)
	 */
	public int read(byte[] b, int off, int len) throws IOException {
		logger.log(Level.FINER,
				"Inside ProcessInputStream.read(byte,int,int) method.");
		checkProcessOpen();
		checkStreamOpen();
		if (b == null) {
			throw new NullPointerException();
		} else if (off < 0 || len < 0 || len > (b.length - off)) {
			throw new IndexOutOfBoundsException();
		} else if (len == 0) {
			return 0;
		}

		ProcessState ps = getProcessState();
		if (ps.isRequestSent() || ps.isOpen()) {
			int tag = generateTag();
			readInternal(tag);
			resetTag();
			setProcessState(ProcessState.REPLY_OBTAINED);
		}
		if (buffer.remaining() >= len) {
			buffer.get(b, off, len);
			return len;
		} else {
			int byteRead = 0;
			if ((byteRead = (buffer.remaining())) > 0) {
				buffer.get(b, off, byteRead);
				return byteRead;
			} else {
				return -1;
			}
		}
	}

	/**
	 * Returns an estimate of the number of bytes that can be read (or skipped
	 * over) from this input stream without blocking by the next invocation of a
	 * method for this input stream. The next invocation might be the same
	 * thread or another thread. A single read or skip of this many bytes will
	 * not block, but may read or skip fewer bytes.
	 * 
	 * @return An estimate of the number of bytes that can be read (or skipped
	 *         over) from this process input stream without blocking or
	 *         {@code 0} when it reaches the end of the input stream.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 */
	public int available() throws IOException {
		return buffer.remaining();
	}

	/**
	 * Marks the current position in this ProcessInputStream. <br>
	 * A subsequent call to the reset method repositions this stream at the last
	 * marked position so that subsequent reads re-read the same bytes. <br>
	 * The readlimit arguments tells this input stream to allow that many bytes
	 * to be read before the mark position gets invalidated. <br>
	 * if the method markSupported returns true, the ProcessInputStream somehow
	 * remembers all the bytes read after the call to mark and stands ready to
	 * supply those same bytes again if and whenever the method reset is called.
	 * <br>
	 * 
	 * @param readlimit
	 *            Maximum limit of bytes that can be read before the mark
	 *            position becomes invalid.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 */
	public void mark(int readlimit) {
		buffer.mark();
	}

	/**
	 * Repositions this ProcessInputStream to the position at the time the mark
	 * method was last called on this input stream.
	 * 
	 * Contract of reset is: <BR>
	 * •If the method markSupported returns true, then: the stream is reset to a
	 * state such that all the bytes read since the most recent call to mark (or
	 * since the start of the file, if mark has not been called) will be re
	 * supplied to subsequent callers of the read method, followed by any bytes
	 * that otherwise would have been the next input data as of the time of the
	 * call to reset. <BR>
	 * <BR>
	 * •If the method markSupported returns false, then: the stream is reset to
	 * a fixed state that depends on the particular type of the input stream and
	 * how it was created. The bytes that will be supplied to subsequent callers
	 * of the read method depend on the particular type of the input stream.
	 * 
	 * @throws IOException
	 *             If an I/O error occurs.
	 */

	public void reset() throws IOException {
		buffer.reset();
	}

	/**
	 * Tests if this input stream supports the mark and reset methods.
	 * 
	 * @return true if the ProcessInputStream supports mark and reset methods.
	 *         else returns false.
	 */
	public boolean markSupported() {
		return false;
	}

	/**
	 * Skips over and discards <code>n</code> bytes of data from this input
	 * stream. The <code>skip</code> method may, for a variety of reasons, end
	 * up skipping over some smaller number of bytes, possibly <code>0</code>.
	 * This may result from any of a number of conditions; reaching end of file
	 * before <code>n</code> bytes have been skipped is only one possibility.
	 * The actual number of bytes skipped is returned. If <code>n</code> is
	 * negative, no bytes are skipped.
	 * 
	 * @param n
	 *            Number of bytes to be skipped.
	 * @return Number of bytes actually skipped.
	 */
	public int skip(int n) {
		logger.log(Level.FINER, "Inside ProcessInputStream.skip(int) method.");
		if (n <= 0) {
			return 0;
		}
		if ((buffer.limit() - buffer.position()) >= n) {
			buffer.position((buffer.position() + n));
			return n;
		} else {
			int remaining = 0;
			if ((remaining = (buffer.limit() - buffer.position())) > 0) {
				buffer.position((buffer.position() + remaining));
				return remaining;
			} else {
				return 0;
			}
		}
	}

	/**
	 * Closes this input stream and releases any system resources associated
	 * with the stream.
	 * 
	 * @throws IOException
	 *             If an error occurs while closing the ProcessInputStream.
	 */
	public void close() throws IOException {
		logger.log(Level.FINER, "Inside ProcessInputStream.close() method.");
		if (closed)
			return;
		closed = true;
	}

	/**
	 * Open the input stream.
	 * 
	 * @throws IOException
	 *             If an error occurs while opening the ProcessInputStream.
	 */
	public boolean open() throws IOException {
		logger.log(Level.FINER, "Inside ProcessInputStream.open() method.");
		closed = false;
		return (!closed);
	}

	/**
	 * This method indicates if the server response is complete or not. Server
	 * processes can return data with an error code 70 (<code>FE_CONTINUE</code>).
	 * An error code of 70 is an indication to the requester that there is more
	 * data to be sent to the requester. This can happen if maximum size of
	 * response that the server is allowed to send is less than the size of the
	 * response message that the server wants to send. This method returns
	 * <code>false</code> if the server responded with error code 70 (<code>FE_CONTINUE</code>).
	 * This method returns valid result only if the <code>read</code> methods
	 * complete without any exception.
	 * 
	 * @return <code>true</code> if response is complete <BR>
	 *         <code>false</code> if server returns data with error code 70
	 */
	public abstract boolean isResponseComplete();

	/**
	 * Calls the native read method.
	 * 
	 * @param tag
	 *            tag is a value that uniquely identifies the operation
	 *            associated with this read.
	 * @throws IOException
	 *             If an error occurs while reading from the ProcessInputStream.
	 */
	protected abstract boolean readInternal(int tag) throws IOException;

	/**
	 * Generate a unique tag.
	 * 
	 * @return The unique tag generated.
	 */
	protected abstract int generateTag();

	/**
	 * Reset the tag.
	 * 
	 */
	protected abstract void resetTag();

	/**
	 * get the current state of process.
	 * 
	 * @return The current state of process.
	 */
	protected abstract ProcessState getProcessState();

	/**
	 * Sets the current state of process.
	 * 
	 * @param state
	 *            New process state.
	 */
	protected abstract void setProcessState(byte state);

	/**
	 * Checks, weather process is open.
	 * 
	 * @throws IOException
	 *             if any error has occurred.
	 */
	private void checkProcessOpen() throws IOException {
		if (getProcessState().isClosed())
			throw new IOException(getProcessNotOpenMessage());
	}

	/**
	 * Checks, weather stream is open.
	 * 
	 * @throws IOException
	 *             if any error has occurred.
	 */
	private void checkStreamOpen() throws IOException {
		if (closed)
			throw new IOException(getStreamClosedMessage());
	}

	/**
	 * Returns the stream close message.
	 * 
	 * @return The stream close message.
	 */
	private String getStreamClosedMessage() {
		return Process.sm.getString("input.stream.closed");
	}

	/**
	 * Returns the process not open message.
	 * 
	 * @return The process not open message.
	 */
	private String getProcessNotOpenMessage() {
		return Process.sm.getString("process.not.open");
	}
}
