/*---------------------------------------------------------------------------
 * Copyright 2009
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 */
package com.hp.nonstop.io.process.impl;

import java.util.logging.Level;
import java.util.logging.Logger;
import com.tandem.ext.util.NativeLoader;

import com.hp.nonstop.io.process.ProcessIdentifier;

/**
 * This class implements a client end-point for the communication to a NS
 * Process.
 * 
 * Before creating an instance of this class, the required JNI library should be
 * loaded into the JVM.
 * 
 * @author Vyom Tewari
 */
public class NSProcessChannel {

	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.impl.NSProcessChannel");

	static {
		 	 NativeLoader loader = new NativeLoader();
			loader.LoadLibrary();
			jniInit();
	}

	/**
	 * default constructor.
	 */
	protected NSProcessChannel() {
	}

	/**
	 * Open the process
	 * 
	 * @param optiones
	 * 
	 * @exception NSProcessException
	 *                if an error occurs while opening the process.
	 */
	public short open(NSProcessOpenOptions options) throws NSProcessException {
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Opening process[").append(options.getProcessName())
					.append("]access[").append(options.getAccess()).append(
							"]exclusion[").append(options.getExclusion())
					.append("]nowaitdepth[").append(options.getNowaitDepth())
					.append("]");
			logger.fine(buf.toString());
		}

		// This jniOpen is a bit strangely written. Wonder why
		// it returns a Short upon success, otherwise it returns NativeStatus
		// !!!
		Object result = jniOpen(options);
		checkErrors(result, "jniOpen");
		if (!(result instanceof Short)) {
			// Typically this error should never occur.
			// jniOpen either returns a Short or a NativeStatus
			String msg = "Invalid object returned by jniOpen " + " : "
					+ result.getClass().getName();
			throw new NSProcessException(msg);
		}
		return ((Short) result).shortValue();
	}

	/**
	 * Calls the native read method with a time out value. Now this is how this
	 * function works: It invokes the jniReadAny. jniReadAny returns a
	 * NativeStatus. The getCode() method of NativeStatus indicates if there is
	 * an error or not. If code is 0 or 70 then its assumed that NativeStatus
	 * contains valid response data from the server. Any other error has to be
	 * propagated all the way to calling function.
	 * 
	 * An exception is thrown if NativeStatus indicates an error of other other
	 * 0 or 70(FE_CONTINUE)
	 * 
	 * 
	 * @param timeout
	 *            the value is in seconds
	 * 
	 * @exception NSProcessException
	 *                if an error occurs while reading to process.
	 */
	public NSProcessData readAny(int timeout) throws NSProcessException {
		int code = 0;

		Object result = jniReadAny(timeout);

		if (result == null) {
			String msg = "jniReadAny returned a null response.";
			throw new NSProcessException(msg);
		}
		if (!(result instanceof NativeStatus)) {
			String msg = "Invalid object returned by jniReadAny " + " : "
					+ result.getClass().getName();
			throw new NSProcessException(msg);
		}

		NativeStatus readResult = (NativeStatus) result;

		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("jniReadAny returned the following:")
					.append("filename:").append(readResult.getFilenum())
					.append(":tag:").append(readResult.getTag()).append(
							":error:").append(readResult.getCode());
			logger.fine(buf.toString());
		}

		code = readResult.getCode();
		if (code != 0 && code != NativeStatus.FE_CONTINUE) {
			StringBuffer buf = new StringBuffer();
			buf.append("Error[").append(code).append("][").append(
					readResult.getDescription()).append("],filenum[").append(
					readResult.getFilenum()).append("]");
			throw new NSProcessException(buf.toString(), code, readResult
					.getFilenum(), readResult.getTag());
		}

		NSProcessData ret = new NSProcessData(readResult.getFilenum(),
				readResult.getTag());
		ret.setData(readResult.getData());
		ret.setError(readResult.getCode());
		return ret;
	}

	/**
	 * Writes data in <code>src</code> to the filenum and tag indicated by
	 * <code>src</code>. The length of data to be written is indicated by
	 * <code>requestMessageSize</code> and the maximum response expected for
	 * this write is indicated by <code>responseMessageSize</code>
	 * 
	 * @param src
	 *            Source of filenum, tag and actual data to be written
	 * @param requestMessageSize
	 *            length of data to be written
	 * @param responseMessageSize
	 *            maximum length of reply expected from the server
	 * @return
	 * @throws NSProcessException
	 */
	public int write(NSProcessData src, final int requestMessageSize,
			final int responseMessageSize) throws NSProcessException {
		Object result;
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Writing data to filenum[").append(src.getFilenum())
					.append("]tag[").append(src.getTag()).append("]sendsize[")
					.append(requestMessageSize).append("]maxreplysize[")
					.append(responseMessageSize).append("]");
			logger.fine(buf.toString());
		}
		try {
			result = jniWrite(src.getFilenum(), src.getTag(), src.getData(),
					requestMessageSize, responseMessageSize);
		} catch (UnsatisfiedLinkError e) {
			// Currently the native call jniWrite throws this
			// UnsatisfiedLinkError. This error can occur when the write is
			// trying to write a large message (>57344) while the function call
			// FILE_WRITEREADL_ cannot be found
			throw new NSProcessException("Large FS API not found",
					NativeStatus.LARGE_FS_CALL_NOT_FOUND, src.getFilenum(), src
							.getTag());
		}
		checkErrors(result, "jniWrite");
		return 0;
	}

	/**
	 * Closes the file.
	 * <p>
	 * Any thread currently blocked for a "read" operation, upon this channel,
	 * will unblock and return -1.
	 */
	public void close(short filenum) throws NSProcessException {
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Closing file[").append(filenum).append("]");
			logger.fine(buf.toString());
		}
		Object result = jniClose(filenum);
		checkErrors(result, "jniClose");
	}

	/**
	 * Set the Log level for Native code for this file. The following log levels
	 * can be set:<BR>
	 * <li>TRACE 0
	 * <li>DEBUG 1
	 * <li>INFO 2
	 * <li>WARN 3
	 * <li>ERROR 4
	 * <li>FATAL 5
	 * 
	 * Level value above FATAL value is considered as FATAL. Level value below
	 * TRACE value is considered as TRACE.
	 * 
	 * @param level
	 *            log level to be set.
	 */
	public synchronized void setNativeLogLevel(short level) {
		jniSetLogLevel(level);
	}

	/**
	 * Cancel the oldest incomplete operation on this process.
	 * 
	 * @param filenum
	 */
	public void cancel(short filenum, int tag) throws NSProcessException {
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Cancelling operation on file[").append(filenum).append(
					"],tag[").append(tag).append("]");
			logger.fine(buf.toString());
		}
		Object result = jniCancel(filenum, tag);
		checkErrors(result, "jniCancel");
	}

	/**
	 * Checks the result object for any errors. Throws exception in below cases: -
	 * if "result" is null - if "deepCheck" is false and "result" is an instance
	 * of NativeStatus. - if "deepCheck" is true and "result" is an instance of
	 * NativeStatus with non-OK status code.
	 * 
	 * @param result
	 *            object checked for errors.
	 * @param opName
	 *            operation for which this result was obtained. This variable is
	 *            used just for logging and exception message.
	 * @param deepCheck
	 *            determines whether to check the status code of the
	 *            NativeStatus object or not.
	 * @throws NSProcessException
	 *             thrown if there are errors in the result.
	 */
	private void checkErrors(Object result, String opName)
			throws NSProcessException {
		if (result == null) {
			String msg = opName + " returned a null response.";
			throw new NSProcessException(msg);
		}

		if (result instanceof NativeStatus) {
			NativeStatus res = (NativeStatus) result;
			if (res.isOK() != true) {
				StringBuffer buf = new StringBuffer();
				buf.append("Error[").append(res.getCode()).append("][").append(
						res.getDescription()).append("],Operation[").append(
						opName).append("]");
				if (logger.isLoggable(Level.FINE)) {
					logger.fine(buf.toString());
				}
				throw new NSProcessException(buf.toString(), res.getCode(), res
						.getFilenum(), res.getTag());
			}
		}
	}

	/**
	 * Construct the process name with the process identifier.
	 * 
	 * @param pid
	 *            Process identifier.
	 * @return process name string
	 */
	String constructNamedProcessName(ProcessIdentifier pid) {
		StringBuffer buf = new StringBuffer();
		buf.append((pid.getNode() == null) ? "" : ("\\" + pid.getNode() + "."))
				.append("$").append(pid.getProcessName()).append(
						(pid.getSeqNo() == -1) ? "" : ":" + pid.getSeqNo())
				.append(
				         /*change begin for the sol 10-120803-4312 */
						pid.getQualifier1() == null ? "" : ".#"  
								+ pid.getQualifier1()).append(
						pid.getQualifier2() == null ? "" : "."   
								+ pid.getQualifier2());
						/*change end for the sol 10-120803-4312 */
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("Constructed named process " + buf.toString());
		}
		return buf.toString();
	}

	/**
	 * Construct the unname process string with the process identifier.
	 * 
	 * @param pid
	 *            Process identifier.
	 * @return process string.
	 */
	String constructUnnamedProcessName(ProcessIdentifier pid) {
		StringBuffer buf = new StringBuffer();
		buf.append(pid.getNode() == null ? "" : ("\\" + pid.getNode() + "."))
				.append("$:").append(pid.getCpu()).append(":").append(
						pid.getPin()).append(":").append(pid.getSeqNo());
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("Constructed unnamed process " + buf.toString());
		}
		return buf.toString();
	}

	// Begin: Native Methods

	/*
	 * NSProcessChannel class is a wrapper around the native methods and is
	 * responsible for maintaining the communication state. So, any call to the
	 * native method should be allowed only throw this wrapper. Thus, all the
	 * native methods should be marked as "private".
	 */

	/**
	 * Initilization.
	 */
	private static native Object jniInit();

	/**
	 * Opens a file connection with the process, whose name is passed.
	 * 
	 * @return Integer on successful open of the process connection. Or,
	 *         <code>NativeStatus</code> object if any error occurred.
	 */
	private static native Object jniOpen(NSProcessOpenOptions options);

	/**
	 * Writes data to the passed file connection.
	 * 
	 * @return <code>NativeStatus</code>. "code" in <code>NativeStatus</code>
	 *         object will be 0 if the "write" operation was successful.
	 *         Otherwise, the "code" will be set to the error occurred.
	 */
	private static native Object jniWrite(short filenum, int ioTag,
			byte[] data, int reqSize, int resSize);

	/**
	 * Reads data from the passed file connection.
	 * 
	 * @return <code>NativeStatus</code> object if any error occurred. If
	 *         there were no read errors, byte[] containing read data will be
	 *         returned.
	 */
	private native Object jniRead(short filenum, int ioTag, int timeout);

	/**
	 * Reads any completed IO.
	 * 
	 * @param timeout
	 *            in seconds
	 * @return <code>NativeStatus</code> object if any error occurred. If
	 *         there were no read errors, byte[] containing read data will be
	 *         returned.
	 */
	private native Object jniReadAny(int timeout);

	/**
	 * Opens a file connection with the process, whose name is passed.
	 * 
	 * @return <code>NativeStatus</code>. "code" in <code>NativeStatus</code>
	 *         object will be 0 if the "close" operation was successful.
	 *         Otherwise, "code" will be set to the error occurred.
	 */
	private static native Object jniClose(short filenum);

	/**
	 * Sets the log level for the native code.
	 * 
	 * @param level
	 *            log level.
	 */
	private static native Object jniSetLogLevel(short level);

	/**
	 * Cancel the oldest incomplete operation on this process.
	 * 
	 * @param filenum
	 */
	private static native Object jniCancel(short filenum, int tag);

	// End: Native Methods

}
