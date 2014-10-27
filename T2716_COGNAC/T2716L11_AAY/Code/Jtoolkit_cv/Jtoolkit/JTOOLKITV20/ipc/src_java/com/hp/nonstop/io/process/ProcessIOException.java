/**
 * 
 */
package com.hp.nonstop.io.process;

import java.io.IOException;

/**
 * An exception indicating an error in an IO operation. This exception is thrown
 * when the nature of error cannot be determined.
 * 
 * @author tewariv
 * 
 */
public class ProcessIOException extends IOException {
	private int error = -1;

	/**
	 * Constructs a new <code>ProcessIOException</code> with the specified
	 * detail message.
	 * 
	 * @param msg
	 *            the detail message.
	 */
	public ProcessIOException(String msg) {
		super(msg);
	}

	/**
	 * @param msg
	 *            the detail message.
	 * @param error
	 *            the error code.
	 */
	public ProcessIOException(String msg, int error) {
		super(msg);
		this.error = error;
	}

	/**
	 * Constructs a new <code>ProcessIOException</code> with no detail
	 * message.
	 */
	public ProcessIOException(Throwable cause) {
		// TODO
		// JToolkit should be compliant with JDK1.4 which does not have the
		// IOException(Throwable) constructor. SO what do we do ?????
		// This is the best we can do now !!!
		super.setStackTrace(cause.getStackTrace());
	}

	/**
	 * @return the errorCode
	 */
	public int getError() {
		return error;
	}
}
