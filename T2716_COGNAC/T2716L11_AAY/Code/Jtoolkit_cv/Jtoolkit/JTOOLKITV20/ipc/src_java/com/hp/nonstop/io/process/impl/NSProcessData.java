package com.hp.nonstop.io.process.impl;

public class NSProcessData implements Comparable {

	private short filenum;
	private int tag;
	private byte[] data;
	private boolean readCompleted = false;
	/**
	 * This error will normally have either the value of 0 or 70. If the value
	 * is 70 it indicates FECONTINUE
	 */
	private int error;
	private String errorDescription="";

	/**
	 * @param filenum
	 * 
	 * @param tag
	 * 
	 */
	public NSProcessData(short filenum, int tag) throws NSProcessException {
		if (filenum < 0)
			throw new NSProcessException("Invalid file number " + filenum);
		if (tag <= 0)
			throw new NSProcessException("Invalid tag " + tag);
		setFilenum(filenum);
		setTag(tag);
	}

	/**
	 * @return the tag
	 */
	public int getTag() {
		return tag;
	}

	/**
	 * @param tag
	 *            the tag to set
	 */
	public void setTag(int tag) {
		this.tag = tag;
	}

	/**
	 * @return the data
	 */
	public byte[] getData() {
		return data;
	}

	/**
	 * @param data
	 *            the data to set
	 */
	public void setData(byte[] data) {
		this.data = data;
	}

	/**
	 * @return the error
	 */
	public int getError() {
		return error;
	}

	/**
	 * @param error
	 *            the error to set
	 */
	public void setError(int error) {
		this.error = error;
	}

	/**
	 * @return the filenum
	 */
	public short getFilenum() {
		return filenum;
	}

	/**
	 * @param filenum
	 *            the filenum to set
	 */
	public void setFilenum(short filenum) {
		this.filenum = filenum;
	}

	public int compareTo(Object obj) {
		int result = ((NSProcessData) obj).getFilenum() - getFilenum();
		if (result != 0)
			return result;
		return (((NSProcessData) obj).getTag() - getTag());
	}

	public boolean equals(Object obj) {
		if (!(obj instanceof NSProcessData))
			return false;
		NSProcessData data = (NSProcessData) obj;
		return compareTo(data) == 0 ? true : false;
	}

	public int hashCode() {
		return (31 + filenum + tag);
	}

	public String toString() {
		return "NSProcessData [filenum=" + filenum + ", tag=" + tag + "]";
	}

	public boolean isReadCompleted() {
		return readCompleted;
	}

	public void setReadCompleted(boolean readCompleted) {
		this.readCompleted = readCompleted;
	}

	public String getErrorDescription() {
		return errorDescription;
	}

	public void setErrorDescription(String errorDescription) {
		this.errorDescription = errorDescription;
	}
}
