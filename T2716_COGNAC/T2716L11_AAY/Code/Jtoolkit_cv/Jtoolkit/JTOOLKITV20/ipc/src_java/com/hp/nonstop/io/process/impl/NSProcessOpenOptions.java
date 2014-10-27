package com.hp.nonstop.io.process.impl;

//NOTE: This class is used by native code. Be very careful while changing the methods
public class NSProcessOpenOptions {

	private String processName;
	private short access;
	private short exclusion;
	private short nowaitDepth;
	private int maxRequestMessageSize;
	private int maxResponseMessageSize;

	/**
	 * Constructor
	 * 
	 */
	protected NSProcessOpenOptions() {

	}

	/**
	 * @return the processName
	 */
	public String getProcessName() {
		return processName;
	}

	/**
	 * @param processName
	 *            the processName to set
	 */
	public void setProcessName(String processName) {
		this.processName = processName;
	}

	/**
	 * @return the access
	 */
	public short getAccess() {
		return access;
	}

	/**
	 * @param access
	 *            the access to set
	 */
	public void setAccess(short access) {
		this.access = access;
	}

	/**
	 * @return the exclusion
	 */
	public short getExclusion() {
		return exclusion;
	}

	/**
	 * @param exclusion
	 *            the exclusion to set
	 */
	public void setExclusion(final short exclusion) {
		this.exclusion = exclusion;
	}

	/**
	 * @return the nowaitDepth
	 */
	public short getNowaitDepth() {
		return nowaitDepth;
	}

	/**
	 * @param nowaitDepth
	 *            the nowaitDepth to set
	 */
	public void setNowaitDepth(final short nowaitDepth) {
		this.nowaitDepth = nowaitDepth;
	}

	/**
	 * Sets the max request message size.
	 * 
	 * @param size
	 */
	public final void setMaxRequestMessageSize(final int size) {
		maxRequestMessageSize = size;
	}

	/**
	 * Gets the max request message size.
	 * 
	 * @param
	 */
	public final int getMaxRequestMessageSize() {
		return maxRequestMessageSize;
	}

	/**
	 * Sets the max response message size.
	 * 
	 * @param size
	 */
	public final void setMaxResponseMessageSize(final int size) {
		maxResponseMessageSize = size;
	}

	/**
	 * Gets the max response message size.
	 * 
	 * @param
	 */
	public final int getMaxResponseMessageSize() {
		return maxResponseMessageSize;
	}
}
