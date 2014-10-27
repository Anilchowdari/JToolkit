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
 * <CODE>TsmpRoutUnavailable</CODE> exception signals that send is failed due
 * to ROUT failure.
 * <P>
 * The <CODE>TsmpRoutUnavailableException</CODE> is thrown to complete the
 * outstanding sends that are not completed due to ROUT failure and it also
 * gives an indication that the sends are failed and have to be re-initiated if
 * required.
 * <P>
 * @author Babu Cherian
 * @version 1.0, 21/05/2009
 * @see com.tandem.tsmp.TsmpServer
 */
public class TsmpRoutUnavailableException extends TsmpFileSystemException {
	private int filesystemErrorNum;
	private String serverClassName;

	/**
	 * Constructs an <code>TsmpRoutUnavailableException</code> with FileSystem
	 * error number and serverclass (or process) name.
	 * 
	 * @param fserror
	 *            the FileSystem error number
	 * @param name
	 *            the pathsend process name or serverclass name
	 * @since JDK1.1
	 */
	protected TsmpRoutUnavailableException(int fserror, String name) {
		super(fserror,name);
	}
	/**
	 * Constructs an <code>TsmpRoutUnavailableException</code> with a
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
	protected TsmpRoutUnavailableException(String msg,int fserror, String name) {
		super(msg,fserror,name);
			}
	
	
}




