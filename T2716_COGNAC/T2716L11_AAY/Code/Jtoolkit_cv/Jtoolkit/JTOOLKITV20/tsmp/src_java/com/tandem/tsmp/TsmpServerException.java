/*
 * @(#)TsmpServerException.java       1.0 98/03/06
 *
 *  @@@ START COPYRIGHT @@@
 *
 *             Copyright 2002 Compaq Computer Corporation
 *             Protected as an unpublished work.
 * 
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
 *
 *  @@@ END COPYRIGHT @@@
 *
 */

package com.tandem.tsmp;

/**
 * Signals that an <CODE>TsmpServer</CODE> exception has occurred.
 *
 * <P>The <CODE>TsmpServerException</CODE> class provides information on
 * TS/MP access errors.
 *
 * <P>Each <CODE>TsmpServerException</CODE> provides the following information:
 * <UL>
 * <LI> a string describing the error available via the <CODE>getMessage()</CODE> method.
 * <LI> a TS/MP error number available via the <CODE>getPathsendError()</CODE> method.
 * <LI> a file system error number available via the <CODE>getFilesystemError()</CODE> method.
 * <LI> a name representing the concatenation of the Pathway process name
 * and the TS/MP serverclass name available via the method
 * <CODE>getServerClassName()</CODE>.
 * </UL>
 *
 * @author  Steve Rispoli
 * @version 1.0, 03/06/98
 * @see     com.tandem.tsmp.TsmpServer
 * @since   JDK1.1
 */

public
class TsmpServerException extends Exception {
    /**
     * Constructs an <code>TsmpServerException</code> with a specified
     * descriptive message.  The Pathsend error and file system error values are
     * set to -1.  The serverclass/process name is set to null.
     *
     * @param   msg a descriptive message.
     * @since   JDK1.1
     */

    protected TsmpServerException(String msg) {
        super(msg);
        this.pathsendErrorNum = -1;
        this.filesystemErrorNum = -1;
        this.serverClassName = null;
    }

    /**
     * Constructs an <code>TsmpServerException</code> with a specified
     * descriptive message, Pathsend error number and serverclass (or process) name.
     * The file system error is set to -1.
     *
     * @param msg a descriptive message.
     * @param pwerror the pathsend error number
     * @param name the pathsend process name or serverclass name
     * @since   JDK1.1
     */

    protected TsmpServerException(String msg,int pwerror, String name) {
        super(msg);
        this.pathsendErrorNum = pwerror;
        this.filesystemErrorNum = -1;
        this.serverClassName = name;
    }

    /**
     * Constructs an <code>TsmpServerException</code> with a specified
     * descriptive message, Pathsend error number, File System error number and
     * the serverclass name.
     *
     * @param msg a descriptive message.
     * @param pwerror the pathsend error number
     * @param fserror the filesystem error number
     * @param name the fully qualified serverclass name
     * @since   JDK1.1
     */

    protected TsmpServerException(String msg,int pwerror,int fserror,String name) {
        super(msg);
        this.pathsendErrorNum = pwerror;
        this.filesystemErrorNum = fserror;
        this.serverClassName = name;
    }

    /**
     * Produces the error message.
     * @return an error message as a String.
     * @since    JDK1.1
     */

    public String getMessage() {

        int pwerror = getPathsendError();
        int fserror = getFilesystemError();
        String name = getServerClassName();
        String msg = super.getMessage();
        if (msg == null) msg = "";  // need this otherwise we print out the word "null"

        // The following code assumes (given current constructors) that one
        // can't set fserror without also setting pwerror and serverclass name
        if (pwerror == -1 && fserror == -1 && name == null) {
           return msg;
        } else {
           if (fserror == -1) {
              //print out everything but fserror
                 return (msg + "; TS/MP error " + pwerror + 
                         "; serverclass name: " + name);
           } else {
              // print out everything
              return (msg + "; TS/MP error " + pwerror + "; File system error " 
                      + fserror + "; serverclass name: " + name);
           }
        }
    }

    /**
     * Returns the Pathsend error number for the last send.
     * @return the Pathsend error number
     * @since   JDK1.1
     */

    public int getPathsendError() {
         return pathsendErrorNum;
    }

    /**
     * Returns the File system error number for the last send.
     * @return the file system error number
     * @since   JDK1.1
     */

    public int getFilesystemError() {
         return filesystemErrorNum;
    }

    /**
     * Returns the serverclass name for the last send.
     * @return the serverclass name
     * @since   JDK1.1
     */

    public String getServerClassName() {
         return serverClassName;
    }

    private int pathsendErrorNum;
    private int filesystemErrorNum;
    private String serverClassName;
}
