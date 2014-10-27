/*---------------------------------------------------------------------------
 * @(#)TsmpRoutUnavailableException.java       21/05/2009
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/
package com.tandem.tsmp;
/**
 * <CODE>TsmpFileSystemException</CODE> exception signals that FileSystem error has occured.
 * 
 * <CODE>TsmpFileSystemException
 * </CODE> provides the following
 * information:
 * <UL>
 * <LI> a string describing the error available via the
 * <CODE>getMessage()</CODE> method.
 * <LI> a file system error number available via the
 * <CODE>getFilesystemError()</CODE> method.
 * <LI> a name representing the concatenation of the Pathway process name and
 * the TS/MP serverclass name available via the method
 * <CODE>getServerClassName()</CODE>.
 * </UL>
 * 
 * @author Babu Cherian
 * @version 1.0, 21/05/2009
 * @see com.tandem.tsmp.TsmpServer
 */
public class TsmpFileSystemException extends Exception {
	private int filesystemErrorNum;
	private String serverClassName;

	/**
	 * Constructs an <code>TsmpFileSystemException</code> with FileSystem
	 * error number and serverclass (or process) name.
	 * 
	 * @param fserror
	 *            the FileSystem error number
	 * @param name
	 *            the pathsend process name or serverclass name
	 * @since JDK1.1
	 */
	protected TsmpFileSystemException(int fserror, String name) {
		this.filesystemErrorNum = fserror;
		this.serverClassName = name;
	}
	/**
	 * Constructs an <code>TsmpFileSystemException</code> with a
	 * specified descriptive message, FileSystem error number and serverclass
	 * (or process) name.
	 * 
	 * @param msg
	 *            a descriptive message.
	 * @param fserror
	 *            the FileSystem error number
	 * @param name
	 *            the pathsend process name or serverclass name
	 * @since JDK1.1
	 */
	protected TsmpFileSystemException(String msg,int fserror, String name) {
		super(msg);
		this.filesystemErrorNum = fserror;
		this.serverClassName = name;
	}
	/**
	 * Returns the File system error number for the last send.
	 * 
	 * @return the file system error number
	 * @since JDK1.1
	 */
	public int getFilesystemError() {
		return filesystemErrorNum;
	}
	/**
	 * Returns the serverclass name for the last send.
	 * 
	 * @return the serverclass name
	 * @since JDK1.1
	 */
	public String getServerClassName() {
		return serverClassName;
	}
	/**
	 * Produces the error message.
	 * 
	 * @return an error message as a String.
	 * @since JDK1.1
	 */
	public String getMessage() {
		int fserror = getFilesystemError();
		String name = getServerClassName();
		String msg = super.getMessage();
		if (msg == null) msg = "";  // need this otherwise we print out the word
		// "null"
		// print out everything
		return (msg + "; FileSystem error " + fserror + "; serverclass name: " + name);
	}
}




