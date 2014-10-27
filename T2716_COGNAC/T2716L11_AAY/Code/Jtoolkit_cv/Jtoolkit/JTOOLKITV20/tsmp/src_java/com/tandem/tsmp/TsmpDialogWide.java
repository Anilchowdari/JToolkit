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
import com.tandem.tsmp.TsmpDialog;
/**
 * A TsmpDialogWide represents a session with a specific serverclass within a
 * specific Pathway application. TsmpDialogWide is used for making pathsend
 * requests larger than 32K. Establish a dialog between a requester and a
 * server process in a server class, and then send messages within the dialog.
 * 
 * After the dialog is established, the same server process is used for all the
 * messages in the dialog.
 * 
 *  
 * <p>
 * Transaction interactions are dependent on TsmpDialog ,based on the
 * Transaction Model Value of the Dialog i.e. ONE_TRANSACTION_PER_DIALOG_MODEL or
 * ANY_TRANSACTION_PER_DIALOG_MODEL.
 * </p>
 * 
 * @version 1.1 07/01/2011
 * @author Babu Cherian, Pabitra Mohan Dalei
 * @see com.tandem.tsmp.tsmpDialog
 */
public class TsmpDialogWide extends TsmpDialog {	
	/**
	 * Creates a new TsmpDialogWide session and associates the session with the
	 * specified serverclass and Pathmon process.
	 * 
	 * @param pathmonName
	 *            the name of the Pathmon process for the Pathway application
	 * @param serverClassName
	 *            the name of the server class within the Pathway application
	 * @exception IllegalArgumentException
	 *                if either pathmonName or serverClassName is 'null'
	 */     
    protected TsmpDialogWide(String pathmonName, String serverClassName)
    throws IllegalArgumentException {
   	
        super(pathmonName, serverClassName);
        /* for UserBuffer support for dialog large pathsends  */
        allowUserBuffers();
        pathSendMode = PathsendModeType.TSMP_PATHSEND_2MB_LIMIT;
      
    }
    
	/**
	 * Performs USERIOBUFFER_ALLOW_() GPC for allowing more than 7 concurrent 2MB pathsends on NSAA systems
	 */
    private final native void allowUserBuffers();
}
