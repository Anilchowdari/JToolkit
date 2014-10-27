package com.hp.nonstop.io.process;

/**
 * Indicates a timeout of an I/O operation
 * 
 * @author Vyom tewari
 * 
 */
public class ProcessTimeoutException extends ProcessIOException {
	int errorCode = -1;

	/**
	 * Constructs a new <code>ProcessTimeoutException</code> with the specified
	 * detail message.
	 * 
	 * @param msg
	 *            The detail message.
	 */
	public ProcessTimeoutException(String msg) {
		super(msg);

	}

	/**
	 * @param msg
	 *            The detail message.
	 * @param errorCode
	 *            The error code.
	 */
	public ProcessTimeoutException(String msg, int errorCode) {
		super(msg);
		this.errorCode = errorCode;
	}

	/**
	 * Constructs a new <code>ProcessTimeoutException</code> with no detail
	 * message.
	 */
	public ProcessTimeoutException(Throwable cause) {
		super(cause);
	}

	/**
	 * @return The errorCode.
	 */
	public int getError() {
		return errorCode;
	}
}
