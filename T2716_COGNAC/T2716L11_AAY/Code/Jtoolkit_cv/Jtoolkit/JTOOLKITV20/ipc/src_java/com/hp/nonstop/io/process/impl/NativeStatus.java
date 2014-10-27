/*---------------------------------------------------------------------------
 * Copyright 2009
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 */
package com.hp.nonstop.io.process.impl;

/**
 * Object used by the Native layer to pass back any operation-status to the java
 * layer.
 * 
 * @author Sajad Bashir Qadri
 */
// NOTE: This object is currently also created by the native code
// so be very careful while modifying either the methods or the constructor
public class NativeStatus {

	private static final int JNI_ERRORS = 0x678ABDF;
	public static final int JNI_ERROR = (JNI_ERRORS + 1);
	public static final int AWAITIO_ERROR = (JNI_ERRORS + 2);
	public static final int INVALID_FILENUM_ERROR = (JNI_ERRORS + 3);
	public static final int NULL_PROCESS_NAME_ERROR = (JNI_ERRORS + 4);
	public static final int NULL_REQDATA_ERROR = (JNI_ERRORS + 5);
	public static final int NULL_REPLY_DATA_ERROR = (JNI_ERRORS + 6);
	public static final int TRANSFER_OVERFLOW_ERROR = (JNI_ERRORS + 7);
	public static final int INVALID_STATE = (JNI_ERRORS + 8);
	public static final int CLOSE_INTERRUPED = (JNI_ERRORS + 9);
	public static final int CLASS_NOT_FOUND = (JNI_ERRORS + 10);
	public static final int TIMEDOUT = (JNI_ERRORS + 11);

	// Its a must that this value should match the NonStop file system value for
	// FE_CONTINUE which is 70
	// NOTE: The naming convention is FE_XX are all guardian errors
	public static final int FE_CONTINUE = 70;
	public static final int FE_DEVICE_NOT_FOUND = 14; // Process not
														// running/found
	public static final int FE_DEVICE_IS_DOWN = 201;
	public static final int FS_NO_OUTSTANDING_IO = 26;
	public static final int LARGE_FS_CALL_NOT_FOUND = (JNI_ERRORS + 12);
	public static final int OPERATION_CANCELLED = (JNI_ERRORS + 13);

	protected short filenum = -1;
	protected int code = 0;
	protected byte[] data;
	protected String description;
	protected int tag = 0;

	public NativeStatus() {
	}

	/**
	 * @param filenum
	 *            the file num.
	 * @param code
	 *            return error code
	 * @param description
	 *            description
	 */
	public NativeStatus(short filenum, int code, String description) {
		this.filenum = filenum;
		this.code = code;
		this.description = description;
		data = null;
	}

	// This constructor is used to report a code (not an error) with some (or
	// full) valid data
	public NativeStatus(short filenum, int code, int tag, byte[] data) {
		this.filenum = filenum;
		this.code = code;
		this.description = null;
		this.data = data;
		this.tag = tag;
	}

	public int getCode() {
		return code;
	}

	public String getDescription() {
		return description;
	}

	public byte[] getData() {
		return data;
	}

	public int getTag() {
		return tag;
	}

	/**
	 * @return the filenum
	 */
	public short getFilenum() {
		return filenum;
	}

	public boolean isOK() {
		return (code == 0);
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		sb.append("{Native Status: code = ").append(code);
		sb.append(", description = ").append(description).append("}");
		return sb.toString();
	}

}