/**
 * 
 */
package com.hp.nonstop.io.process;

/**
 * On certain older versions of the operating system, NonStop File System does
 * not provide calls to send messages of more than 57344 bytes.
 * 
 */
public class LargeMessageNotSupportedException extends ProcessIOException {

	private int errorCode = -1;

	/**
	 * Constructs a new <code>LargeMessageNotSupportedException</code> with
	 * the specified detail message.
	 * 
	 * @param msg
	 *            the detail message.
	 */
	public LargeMessageNotSupportedException(String msg) {
		super(msg);
	}

	/**
	 * @param msg
	 *            the detail message.
	 * @param errorCode
	 *            the error code.
	 */
	public LargeMessageNotSupportedException(String msg, int errorCode) {
		super(msg);
		this.errorCode = errorCode;
	}

	/**
	 * Constructs a new <code>LargeMessageNotSupportedException</code> with no
	 * detail message.
	 */
	public LargeMessageNotSupportedException(Throwable cause) {
		super(cause);
	}

	/**
	 * @return the errorCode
	 */
	public int getError() {
		return errorCode;
	}
}
