/**
 * 
 */
package com.hp.nonstop.io.process;

/**
 * @author Vyom Tewari
 * 
 */
class ProcessState {

	static final byte INIT = ((byte) 0x01);
	static final byte OPEN = ((byte) 0x03);
	static final byte REQUEST_SENT = ((byte) 0x07);
	static final byte REPLY_OBTAINED = ((byte) 0x0F);
	static final byte CLOSED = ((byte) 0x11);

	byte state;

	/**
	 * Default constructor.
	 * 
	 * @param state
	 *            The new process state.
	 */
	ProcessState(byte state) {
		this.state = state;
	}

	/**
	 * Tells weather Process is initialized.
	 * 
	 * @return true if process is initialized, false otherwise.
	 */
	public boolean isInitialized() {
		return (state & (byte) 0x01) > 0 ? true : false;
	}

	/**
	 * Tells weather Process is closed.
	 * 
	 * @return true if process is closed, false otherwise.
	 */
	public boolean isClosed() {
		return state == 0x11;
	}

	/**
	 * Tells weather Process is open.
	 * 
	 * @return true if process is opened, false otherwise.
	 */
	public boolean isOpen() {
		return state == 0x03;
	}

	/**
	 * Tells weather request is sent to server process.
	 * 
	 * @return true if request is sent to server, false otherwise.
	 */
	public boolean isRequestSent() {
		return state == 0x07;
	}

	/**
	 * Tells weather reply obtained from server process.
	 * 
	 * @return true if reply obtained from server, false otherwise.
	 */
	public boolean isReplyObtained() {
		return state == 0x0F;
	}
}
