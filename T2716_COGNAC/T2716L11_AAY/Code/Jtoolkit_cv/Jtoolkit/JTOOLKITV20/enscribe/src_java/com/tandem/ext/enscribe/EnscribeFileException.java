/*
 * Copyright 2000 Compaq Computer Corporation
 *           Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
*/

package com.tandem.ext.enscribe;

import java.util.*;

/**
 * This exception indicates that an error has occurred while trying an
 * Enscribe File operation.
 */

public class EnscribeFileException extends com.tandem.ext.guardian.GuardianException
{


    /**
     * Unknown file system error.
     */

    static final short FEErrorUndefined = -1;
	/**
	 * Ok - no file error.
	 */
    static final short FEOK = 0;
	/**
	 * A read procedure reached end-of-file or a
	 * write procedure reached end of tape.
	 */
    static final short FEEOF = 1;
	/**
	 * The operation specified is not allowed on this type of file.
	 */
    static final short FEINVALOP = 2;
    /**
     * A partition could not be opened or purged.
     */
    static final short FEPARTFAIL = 3;
	/**
	 * An alternate key file could not be opened.
	 */
    static final short FEKEYFAIL = 4;
   /**
    * Sequential buffering was not used because
    * no space was available in the PFS.
    */
    static final short FESEQFAIL = 5 ;
    /**
     * System message picked up on $receive
     *
     */
    static final short FESYSMESS = 6;
    /**
     * Unable to receive system messages
     */
    static final short FENOSYSMESS = 7;
   	/**
   	 * A read through a lock was successful.
   	 */
    static final short FEREADLOCK = 9;
	/**
	 * The new record or file could not be created
	 * because a file by that name or a record with
	 * that key already exists.
	 */
    static final short FEDUP = 10 ;
	/**
	 * The file is not in the directory, the
	 * record is not in the file, or the specified
	 * tape file is not on a labeled tape.
	 * */
    static final short FENOTFOUND = 11 ;
 	/**
 	 * The file is in use.
	 */
    static final short FEINUSE = 12 ;
	/**
 	 * The filename was not specified in proper
 	 * form.	 */
    static final short FEBADFORM = 13;
 	/**
 	 * Device does not exist on this system.
	 */
    static final short FENoSuchDev = 14;
    /**
     * The file is not open.
     */
    static final short FENOOPEN = 16;
	/**
	 * The system specified does not exist in the network.
	 */
    static final short FENoSuchSystem = 18;
	/**
	 * An illegal count was specified in a
	 * file system call, or the operation attempted
	 * to transfer too much or too little data.
	 */
    static final short FEBADCOUNT = 21;
	/**
	 * Unable to obtain file system buffer space.
	 */
    static final short FENoBufSpace = 31;
	/**
	 * The operation timed out.
	 */
    static final short FETimedOut = 40;
	/**
	 * Unable to obtain disk space for file extent.
	 */
    static final short FENoDiscSpace = 43;
	/**
	 * The file is full.
	 */
    static final short FEFileFull = 45;
	/**
	 * An invalid key was specified, key length
	 * exceeded 255 bytes on file creation, or
	 * application failed to open an alternate-key
	 * file.
	 * */
    static final short FEInvalidKey = 46;
	/**
	 * Security violation; illegal operation attempted.
	 */
    static final short FESecViol = 48;
	/**
	 * The disk file or record is locked.
	 */
    static final short FELocked = 73;
	/**
	 * Requesting process has no current process
	 * transaction identifier.
	 */
    static final short FENoTransid = 75;
	/**
	 * Transaction is in the process of ending.
	 */
    static final short FETransidEnding = 76;
	/**
	 * Transaction identifier is invalid or obsolete.
	 */
    static final short FEBadTransid = 78;
	/**
	 * Attempt to update or delete a
	 * record which was not previously locked.
	 */
    static final short FERecNotLocked = 79;
	/**
	 * Invalid operation on audited file or
	 * non-audited disk volume.
	 */
    static final short FEFileAudited = 80;
	/**
	 * Transaction aborted.
	 */
    static final short FETransidAborted = 97;
	/**
	 * The current path to the device is down.
	 */
    static final short FEPathDown = 201;
	/**
	 * Network line handler error; operation not
	 * started.
	 */
    static final short FEPathDown240 = 240;
	/**
	 * Network line handler error; operation aborted.
	 */
    static final short FEExpandAbort = 248;
	/**
	 * Network path down.
	 */
    static final short FEPathDown255 = 255;
    /**
     * Request Canceled.
     */
    static final short FEReqAborted = 593;
    private short errorNumber_ = FEErrorUndefined;


	/**
    * Constructs an <code>EnscribeFileException</code> with the specified
    * descriptive message.
    *
    * @param   msg descriptive error message
    */
    public EnscribeFileException(String msg, short error,
    	String function, String filename) {
		super(msg,error,function,filename);
		errorNumber_ = error;
    } // EnscribeFileException

    /**
    * Returns text message from the error number.
    *
    * @param errorNumber the error number
    * @return String message for that error number
    *
    * @see #getMessage
    */

    public String getErrorText (short errorNumber) {
		errorNumber_ = errorNumber;
		return getMessage();
	} // getErrorText

    /**
    * Construct text message from the error number from when the exception was thrown.
    *
    * @return String error message
    */

    public String getMessage () {
        String msg;
        switch (errorNumber_) {
		case FEErrorUndefined:
			msg = "";
			break;
		case FEOK:
			msg = " No file error";
			break;
 		case FEINVALOP:
			msg = " Invalid file operation: error 2";
			break;
 		case FEDUP:
			msg = " Duplicate record or file: error 10";
			break;
 		case FENOTFOUND:
			msg = " Record or file not found: error 11";
			break;
 		case FEINUSE:
			msg = " File in use: error 12";
			break;
 		case FEBADFORM:
			msg = " Filename invalid: error 13";
			break;
 		case FENoSuchDev:
			msg = " No such device: error 14";
			break;
		case FENOOPEN:
			msg = " The file is not open: error 16";
			break;
 		case FENoSuchSystem:
			msg = " No such system: error 18";
			break;
 		case FEBADCOUNT:
			msg = " Bad count: error 21";
			break;
 		case FENoBufSpace:
			msg = " No buffer space: error 31";
			break;
 		case FETimedOut:
			msg = " Operation timed out: error 40"; // should not get this
			break;
 		case FENoDiscSpace:
			msg = " No disc space: error 43";
			break;
 		case FEFileFull:
			msg = " The file is full: error 45";
			break;
 		case FEInvalidKey:
			msg = " An invalid key was specified: error 46";
			break;
 		case FESecViol:
			msg = " Security violation: error 48";
			break;
 		case FELocked:
			msg = " File or record locked: error 73";
			break;
 		case FENoTransid:
			msg = " No TMF transid: error 75";
			break;
 		case FETransidEnding:
			msg = " Transid is ending: error 76";
			break;
 		case FEBadTransid:
			msg = " Bad transid: error 78";
			break;
 		case FERecNotLocked:
			msg = " Record not locked: error 79";
			break;
 		case FEFileAudited:
			msg = " Invalid operation on audited file: error 80";
			break;
 		case FETransidAborted:
			msg = " Transaction aborted by call to ABORTTRANSACTION: error 97";
			break;
 		case FEPathDown:
			msg = " Path down: error 201";
			break;
 		case FEPathDown240:
			msg = " Path down: error 240";
			break;
 		case FEPathDown255:
			msg = " Path down: error 255";
			break;
		case FEReqAborted:
			msg = " The request was canceled: error 593";
			break;
 		case 1234:
			msg = " Internal error registering with CMA: error 1234";
			break;
 		case 4002:
			msg = " No such pathname exists: error 4002";
			break;
 		case 4005:
			msg = " I/O error: error 4005";
			break;
 		case 4006:
			msg = " A prefix within pathname refers to an OSS fileset other than the root fileset that is not mounted: error 4006";
			break;
  		case 4009:
			msg = " Bad file descriptor: error 4009";
			break;
		case 4013:
			msg = " Search permission is denied on a component of the pathname prefix: error 4013";
			break;
 		case 4014:
			msg = " A specified parameter has an invalid address: error 4014";
			break;
 		case 4020:
			msg = " A prefix within pathname refers to a file other than a directory: error 4020";
			break;
 		case 4022:
			msg = " Pathname is invalid: error 4022";
			break;
 		case 4131:
			msg = " The pathname, a component of the pathname, or a symbolic link in the pathname is longer than PATH_MAX characters: error 4131";
			break;
 		case 4200:
			msg = " The pathname or a component of the pathname has too many symbolic links to resolve the specified pathnameA: error 4200";
			break;
 		case 4202:
			msg = " The root fileset is not mounted: error 4202";
			break;
 		case 4211:
			msg = " The resulting pathname is longer than the limit defined in PATH_MAX: error 4211";
			break;
	default:
			msg = " File error " + errorNumber_;
			break;
		}
		return super.getMessage() + msg;
    } // getMessage

} // EnscribeFileException
