/*
 * @(#)TsmpSendException.java	1.0 98/03/06
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
 * This exception indicates that an error has occurred while a {@link TsmpServer}
 * is trying to send data to a legacy TS/MP server. 
 *
 * @author  Steve Rispoli
 * @version 1.0, 03/06/98
 * @see     com.tandem.tsmp.TsmpServer
 * @since   JDK1.1
 */

public
class TsmpSendException extends TsmpServerException {

    /**
     * Constructs a <code>TsmpSendException</code> with a descriptive 
     * message, the Pathsend error number, the file system error
     * number and the fully qualified serverclass name. 
     *
     * @param  msg a message describing the exception, should not be null
     * @param  psError the Pathsend error number
     * @param  fsError the file system error detail number
     * @param  serverclassName the fully qualified serverclass name the service
     * method failed against (the name should include both the Pathmon and 
     * server class name).
     * @since  T2716V10
     */
    public TsmpSendException(String msg, int psError, int fsError, String serverclassName) {
         super(msg, psError, fsError, serverclassName);
    }

    /**
     * Constructs a <code>TsmpSendException</code> with the specified detail 
     * message.
     *
     * @param  msg the detail message.
     * @since  JDK1.1
     * @deprecated  As of T2716V10, replaced by
     * {@link #TsmpSendException(String, int, int, String)} and
     */
    protected TsmpSendException(String msg) {
         super(msg);
    }

    /**
     * Constructs a <code>TsmpSendException</code> with the specified detail 
     * message, indicating that a send error has occurred and specifying the
     * TS/MP error number and the serverclass name. 
     *
     * @param  msg the detail message.
     * @param  errorNumber the TS/MP error number
     * @param  serverclassName the name of the server class we are trying to
     * send to.
     * @since  JDK1.1
     * @deprecated  As of T2716V10, replaced by
     * {@link #TsmpSendException(String, int, int, String)} and
     */
    protected TsmpSendException(String msg, int errorNumber,String serverclassName) {
         super(msg,errorNumber,serverclassName);
    }
}
