/*---------------------------------------------------------------------------
 * Copyright 2009
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 */
package com.hp.nonstop.io.process.impl;

import java.io.IOException;

/**
 * Thrown to indicate that there was an error in NSProcessChannel.
 * 
 * @author Sajad Bashir Qadri
 * @author Suveer Nagendra
 */
public class NSProcessException extends IOException {
	int error = -1;
	short filenum = -1;
	int tag = -1;

	/**
	 * An exception that has occurred on some unspecific filenum and tag 
	 * @param msg
	 */
	protected NSProcessException(String msg) {
		super(msg);
	}

	/**
	 * An exception that has occurred on a particular filenum and tag
	 * @param msg
	 * @param error
	 * @param filenum
	 * @param tag
	 */
	protected NSProcessException(String msg, int error, short filenum, int tag){
		super(msg);
		this.error = error;
		this.filenum = filenum;
		this.tag = tag;
	}

	/**
	 * Constructs a new <code>ChannelException</code> with no detail message.
	 */
	protected NSProcessException() {
		super();
	}

	/**
	 * @return the errorCode
	 */
	public int getError() {
		return error;
	}
	
	public short getFilenum(){
		return filenum;
	}
	
	public int getTag(){
		return tag;
	}
	
}