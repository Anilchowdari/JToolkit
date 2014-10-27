/*
 * @(#)TsmpServerUnavailableException.java	1.0 98/03/06
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
 * This exception indicates that a TsmpServer is unavailable. TsmpServers 
 * may report this exception at any time, and the client accessing the 
 * TsmpServer should behave appropriately. There are two types of
 * unavailability, and sophisticated clients may deal with these differently: 
 *<ul>
 *<li>Permanent unavailability: 
 *      The TsmpServer will not be able to handle client requests until some
 *      administrative action is taken to correct a problem. For example, the 
 *      pathmon associated with the TsmpServer may be  unavailable or the 
 *      serverclass associated with the TsmpServer may be frozen. A
 *      well written client could log the error to provide information for an 
 *      administrator. 
 *
 *<li>Temporary unavailability. 
 *      The TsmpServer can not handle requests at this moment due to a 
 *      resource problem.  A typical example of this would be if TSMP returned 
 *      the error Link Denied; indicating that all available links to the 
 *      serverclass associated with the TsmpServer are currently being used.
 *      The problem may be self correcting, such as those due to excessive load,
 *      or corrective  action may need to be taken by an administrator. 
 *</ul><p>
 * Clients may safely treat both types of exceptions as "permanent", but good 
 * treatment of temporary unavailability leads to more robust clients. 
 * Specifically, requests to the TsmpServer might be blocked (or
 * otherwise deferred) for a TmpServer-suggested amount of time, rather than 
 * being rejected until the service itself restarts.
 *
 * @author  Steve Rispoli
 * @version 1.0, 03/06/98
 * @see     com.tandem.tsmp.TsmpServer
 * @since   JDK1.1
 */

public
class TsmpServerUnavailableException extends TsmpServerException {

    /**
     * Constructs a <code>TsmpServerUnavailableException</code> with the 
     * specified detail message.
     *
     * @param msg the detail message.
     * @since   JDK1.1
     */
    
    protected TsmpServerUnavailableException(String msg) {
	super(msg);
    }

    /**
     * Constructs a <code>TsmpServerUnavailableException</code> with the 
     * specified detail message, the Pathsend error number, the file
     * system error number and the serverclass name.
     *
     * @param msg the detail message.
     * @param pwerror the Pathsend error number.
     * @param fserror the file system error number.
     * @param name the serverclass for which the error occurs.
     * @since   JDK1.1
     */
    
    protected TsmpServerUnavailableException(String msg,int pwerror,int fserror,String name) {
	super(msg,pwerror,fserror,name);
    }
}
