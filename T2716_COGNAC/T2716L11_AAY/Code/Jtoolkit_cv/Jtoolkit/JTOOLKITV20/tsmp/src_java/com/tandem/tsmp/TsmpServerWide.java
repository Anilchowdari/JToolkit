/*---------------------------------------------------------------------------
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/

package com.tandem.tsmp;

import com.tandem.ext.util.PathsendModeType;

/**
 * A TsmpServerWide represents a session with a specific serverclass within a
 * specific Pathway application. TsmpServerWide is used for making pathsend
 * requests larger than 32K. Within the context of a TsmpServerWide, requests
 * are issued and replies are received.
 * 
 * <p>
 * Transaction interactions are independent of TsmpServerWide.
 * </p>
 *
 * @version 1.1 07/01/2011
 * @author Babu Cherian, Pabitra Mohan Dalei.
 * @see com.tandem.ext.guardian.GuardianInput
 * @see com.tandem.ext.guardian.GuardianOutput
 * @see com.tandem.tsmp.TsmpServer
 * @see com.tandem.tsmp.TsmpServerReply
 * @see com.tandem.tsmp.TsmpGenericServerReply
 * @see com.tandem.tsmp.TsmpServerRequest
 */

public class TsmpServerWide extends TsmpServer {

	/**
	 * Creates a new TsmpServerWide session and associates the session with the
	 * specified serverclass and Pathmon process.
	 * 
	 * @param pathmonName
	 *            the name of the Pathmon process for the Pathway application
	 * @param serverClassName
	 *            the name of the server class within the Pathway application
	 * @exception IllegalArgumentException
	 *            if either pathmonName or serverClassName is 'null'
	 */
	public TsmpServerWide(String pathmonName, String serverClassName)
	throws IllegalArgumentException {

		super(pathmonName, serverClassName);
		/*for UserBuffer support for server class large pathsends  */
        allowUserBuffers();
		pathSendMode = PathsendModeType.TSMP_PATHSEND_2MB_LIMIT;
	}

	/* Creates a TsmpDialogWide object. The same Pathmon name and Serverclass name
     * used for constructing the TsmpServerWide object is used for creating the
     * TsmpDialog object.
     * 
     * @return TsmpDialogWide object.
     */
	public TsmpDialogWide createDialogWide(){
	return new TsmpDialogWide(this.pathmonName,this.serverClassName);
	}
	
	/**
	 * Performs USERIOBUFFER_ALLOW_() GPC for allowing more than 7 concurrent
     * 2MB pathsends on NSAA systems
	 */	
	private final native void allowUserBuffers();

}
