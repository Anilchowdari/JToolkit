/**
 * 
 */
package com.hp.nonstop.io.process;

/**
 * Indicates that the server process could not be found
 * 
 * @author tewariv
 * 
 */
public class ServerProcessNotFoundException extends ProcessIOException {

	private int errorCode = -1;

	/**
	 * Constructs a new <code>ServerProcessNotFoundException</code> with the
	 * specified detail message.
	 * 
	 * @param msg
	 *            the detail message.
	 */
	public ServerProcessNotFoundException(String msg) {
		super(msg);
	}

	/**
	 * @param msg
	 *            the detail message.
	 * @param errorCode
	 *            the error code.
	 */
	public ServerProcessNotFoundException(String msg, int errorCode) {
		super(msg);
		this.errorCode = errorCode;
	}

	/**
	 * Constructs a new <code>ServerProcessNotFoundException</code> with no
	 * detail message.
	 */
	public ServerProcessNotFoundException(Throwable cause) {
		super(cause);
	}

	/**
	 * @return the errorCode
	 */
	public int getError() {
		return errorCode;
	}
}
