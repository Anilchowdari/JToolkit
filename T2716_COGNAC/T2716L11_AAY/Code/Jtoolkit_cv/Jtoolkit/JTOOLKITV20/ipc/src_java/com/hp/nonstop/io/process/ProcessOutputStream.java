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
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * An OutputStream to write data to the server process.
 * <p>
 * The OutputStream internally maintains a buffer capable of holding the maximum
 * size of message that can be exchanged between the requester and server (see
 * {@link ProcessOpenOption}). A call to any of the <code>write</code>
 * methods, writes data to the internal buffer. The data is pushed to the server
 * process only after an explicit call to either {@link #flush()} or
 * {@link #close()}. The write method throws an exception upon a buffer
 * overflow. Although an exception is thrown the data in the buffer is not
 * cleared. An explicit call to {@link #clear()} clears the internal buffer.
 * </p>
 * 
 * @author Vyom Tewari
 * 
 */
public abstract class ProcessOutputStream extends OutputStream {

	// This is indicates the buffer size which in turn indicates the maximum
	// size of either a write or a read operation
	private int bufsize;

	// Internal buffer that stores data till data is flushed or the buffer is
	// full
	private ByteBuffer buf = null;

	private boolean closed = false;

	private boolean flushDone = false;
	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.ProcessOutputStream");

	/**
	 * Constructs the ProcessOutputStream object for communicating with the
	 * Process object.
	 * 
	 * @param bufferSize
	 *            The internal buffer, which will be used to store data.
	 */
	public ProcessOutputStream(int bufferSize) {
		bufsize = bufferSize;
		buf = ByteBuffer.allocate(bufsize);
	}

	/**
	 * Writes the specified byte to this output stream. The general contract for
	 * <code>write</code> is that one byte is written to the output stream.
	 * The byte to be written is the eight low-order bits of the argument
	 * <code>b</code>. The 24 high-order bits of <code>b</code> are
	 * ignored.
	 * 
	 * 
	 * @param b
	 *            The data to be written.
	 * 
	 * @exception IOException
	 *                If an I/O error occurs. In particular, an
	 *                <code>IOException</code> may be thrown if the process
	 *                output stream has been closed.
	 * 
	 * @see java.io.OutputStream#write(int)
	 */
	public void write(int b) throws IOException {
		checkProcessOpen();
		checkStreamOpen();
		checkIOOutstanding();

		if (buf == null) {
			buf = ByteBuffer.allocate(bufsize);
		}
		if (buf.limit() - buf.position() > 0) {
			buf.put((byte) b);
			return;
		}
		StringBuffer message = new StringBuffer();
		message.append("Not enough space in internal buffer. Buffer capacity:")
				.append(buf.capacity()).append(". Current size ").append(
						buf.position());
		logger.log(Level.WARNING, message.toString());
		throw new IOException(getNotEnoughSpaceMessage());
	}

	/**
	 * Writes b.length bytes from the specified byte array to the
	 * ProcessOutputStream.The data will be actually send to server after a call
	 * to flush() method. <BR>
	 * <BR>
	 * After sending data to the server, user must call read to receive the
	 * reply data. Any calls to the write method without reading reply for
	 * previous request will empty the previous reply data. <BR>
	 * <BR>
	 * The maximum size of the message transfer supported is 2097153 bytes(2MB).
	 * <p>
	 * If <code>b</code> is <code>null</code>, a
	 * <code>NullPointerException</code> is thrown.
	 * 
	 * @param b
	 *            The data to be written.
	 * 
	 * @throws IOException
	 *             If an error occurs when writing to the ProcessOutputStream.
	 * 
	 * @see java.io.OutputStream#write(byte[])
	 */
	public void write(byte[] b) throws IOException {
		if (b == null)
			throw new NullPointerException();
		write(b, 0, b.length);
	}

	/**
	 * Writes <code>len</code> bytes from the specified byte array starting at
	 * offset <code>off</code> to this process output stream. The general
	 * contract for <code>write(b, off, len)</code> is that some of the bytes
	 * in the array <code>b</code> are written to the process output stream in
	 * order; element <code>b[off]</code> is the first byte written and
	 * <code>b[off+len-1]</code> is the last byte written by this operation.<BR>
	 * 
	 * <BR>
	 * After sending data to the server, user must call read to receive the
	 * reply data. Any calls to the write method without reading reply for
	 * previous request will empty the previous reply data. <BR>
	 * 
	 * <BR>
	 * The maximum size of the message transfer supported is 2097153 bytes(2MB).
	 * 
	 * 
	 * If <code>off</code> is negative, or <code>len</code> is negative, or
	 * <code>off+len</code> is greater than the length of the array
	 * <code>b</code>, then an <tt>IndexOutOfBoundsException</tt> is
	 * thrown.
	 * 
	 * @param b
	 *            The data to be written.
	 * @param off
	 *            The start offset in the data.
	 * @param len
	 *            The number of bytes to write.
	 * @exception IOException
	 *                If an I/O error occurs. In particular, an
	 *                <code>IOException</code> is thrown if the output stream
	 *                is closed.
	 * @exception NullPointerException
	 *                If <code>b</code> is null
	 * @exception IndexOutOfBoundsException
	 *                If <code>off</code> is negative, or <code>len</code>
	 *                is negative, or <code>off+len</code> is greater than the
	 *                length of the array <code>b</code>
	 * @see java.io.OutputStream#write(byte[],int off ,int len)
	 */
	public void write(byte[] b, int off, int len) throws IOException {
		checkProcessOpen();
		checkStreamOpen();
		checkIOOutstanding();
		if (b == null) {
			throw new NullPointerException();
		} else if ((off < 0) || (off > b.length) || (len < 0)
				|| ((off + len) > b.length) || ((off + len) < 0)) {
			throw new IndexOutOfBoundsException();
		} else if (len == 0) {
			return;
		}
		if (buf == null) {
			buf = ByteBuffer.allocate(bufsize);
		}
		if (buf.limit() - buf.position() >= len) {
			buf.put(b, off, len);
			return;
		}
		StringBuffer message = new StringBuffer();
		message.append("Not enough space in internal buffer. Buffer capacity:")
				.append(buf.capacity()).append(". Current size ").append(
						buf.position());
		logger.log(Level.WARNING, message.toString());
		throw new IOException(getNotEnoughSpaceMessage());
	}

	/**
	 * Flushes the ProcessOutputStream and forces any buffered output bytes to
	 * be written out.
	 * 
	 * @throws LargeMessageNotSupportedException
	 *             If you tried to write a message with size greater than 57344
	 *             on the box where large size messages are not supported.
	 * @throws IOException
	 *             If an error occurs when flushing the ProcessOutputStream.
	 */
	public void flush() throws IOException {
		logger.log(Level.FINER,
				"Inside the ProcessOutputStream.flush() method.");
		checkProcessOpen();
		checkStreamOpen();
		if (buf != null) {
			int tag = generateTag();
			// fireEvent(ProcessIOEvent.BEFORE_FLUSH, tag);
			byte[] data = new byte[buf.position()];
			buf.rewind();
			buf.get(data, 0, data.length);
			writeInternal(tag, data, 0, data.length);
			// fireEvent(ProcessIOEvent.AFTER_FLUSH, tag);
			buf.clear();
		}
		flushDone = true;
		setProcessState(ProcessState.REQUEST_SENT);
	}

	/**
	 * If there any data to flush, close methods flushes out the same; otherwise
	 * this method does nothing.
	 * 
	 * @throws IOException
	 *             If an error occurs when flushing the ProcessOutputStream.
	 */
	public void close() throws IOException {
		logger.log(Level.FINER,
				"Inside the ProcessOutputStream.close() method.");
		if (closed)
			return;
		if (!flushDone && (buf != null && buf.position() > 0))
			flush();
		closed = true;
	}

	/**
	 * Open the output stream.
	 * 
	 * @throws IOException
	 *             If an error occurs while opening the ProcessInputStream.
	 */
	public void open() throws IOException {
		logger
				.log(Level.FINER,
						"Inside the ProcessOutputStream.open() method.");
		closed = false;
	}

	/**
	 * Clear all the buffered data in this process Output Stream.
	 */
	public void clear() {
		buf.clear();
	}

	// /**
	// *
	// * @return current size of the internal buffer
	// */
	// public int getSize() {
	// return buf.position();
	// }

	// public void setMaxMessageSize(int size) throws IllegalStateException {
	// if (buf == null) {
	// bufsize = size;
	// return;
	// }
	// throw new IllegalStateException(
	// "Cannot invoke this operation when the stream is in use");
	// }

	// protected abstract void fireEvent(int eventID, int messageTag);

	/**
	 * Generate a unique tag,tag is a value that uniquely identifies the
	 * operation associated with this write.
	 * 
	 */
	protected abstract int generateTag();

	/**
	 * Calls the native write method.
	 * 
	 * @param tag
	 *            tag is a value that uniquely identifies the operation
	 *            associated with this write.
	 * @param buf
	 *            The data to be written.
	 * @param off
	 *            The start offset in the data.
	 * @param len
	 *            The number of bytes to write.
	 * @throws IOException
	 *             If an error occurs when flushing the ProcessOutputStream.
	 */
	protected abstract void writeInternal(int tag, byte[] buf, int off, int len)
			throws IOException;

	/**
	 * Return the current state of process.
	 * 
	 * @return The current process state.
	 */
	protected abstract ProcessState getProcessState();

	/**
	 * Sets the current state of process.
	 * 
	 * @param state
	 *            New process state.
	 */
	protected abstract void setProcessState(byte state);

	/*
	 * Indicate if there is an I/O oustanding. This will just check the internal
	 * state of the process object and is not an indication from a FS point of
	 * view
	 */
	protected abstract boolean isIOPending();

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
	
	private void checkIOOutstanding() throws IOException {
		if(isIOPending()==true)
			throw new IOException(Process.sm.getString("pending.outstanding.io"));
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

	private String getNotEnoughSpaceMessage() {
		StringBuffer nospace = new StringBuffer(Process.sm
				.getString("buffer.space"));
		nospace.append("Max Size ").append(buf.capacity()).append(
				" Current Size ").append(buf.position());
		return nospace.toString();
	}

	/**
	 * Returns the stream close message.
	 * 
	 * @return The stream close message.
	 */
	private String getStreamClosedMessage() {
		return Process.sm.getString("output.stream.closed");
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