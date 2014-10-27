/**
 * 
 */
package com.hp.nonstop.io.process;

/**
 * Indicates an IO operation was cancelled
 * 
 * @author Vyom Tewari
 * 
 */
public class ProcessCancelException extends ProcessIOException {
	int errorCode = -1;

	/**
	 * Constructs a new <code>ProcessCancelException</code> with the specified
	 * detail message.
	 * 
	 * @param msg
	 *            the detail message.
	 */
	public ProcessCancelException(String msg) {
		super(msg);

	}

	/**
	 * @param msg
	 *            the detail message.
	 * @param errorCode
	 *            the error code.
	 */
	public ProcessCancelException(String msg, int errorCode) {
		super(msg);
		this.errorCode = errorCode;
	}

	/**
	 * Constructs a new <code>ProcessCancelException</code> with no detail
	 * message.
	 */
	public ProcessCancelException(Throwable cause) {
		super(cause);
	}

	/**
	 * @return the errorCode
	 */
	public int getError() {
		return errorCode;
	}
}
