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
 * This exception indicates that a warning has occurred while trying an
 * Enscribe File operation.
 */

public class EnscribeFileWarning extends EnscribeFileException
{

    /**
     * Unknown file system error.
     */

    static final int FEErrorUndefined = -1;
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
     * An open or purge of a partition failed.
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
    static final short FEREADLOCK = 9 ;

    private short errorNumber_ = FEErrorUndefined;

	/**
    * Constructs an <code>EnscribeFileWarning</code> with the specified
    * descriptive message.
    *
    * @param msg descriptive error message
    */
    public EnscribeFileWarning(String msg, short error, String function,
    	String fileName) {
		super(msg,error,function,fileName);
        errorNumber_ = error;
   } // EnscribeFileWarning

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
    * Construct text message from the error number from when the warning was constructed.
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
 		case FEEOF:
 			msg = " End of file: error 1";
 			break;
 		case FEINVALOP:
			msg = " Invalid file operation: error 2";
			break;
 		case FEPARTFAIL:
			msg = " An open or purge of a partition failed: error 3";
			break;
 		case FEKEYFAIL:
			msg = " An open for an alternate key file failed: error 4";
			break;
 		case FESEQFAIL:
			msg = " Sequential block buffering is not being used because there is no room in the PFS: error 5";
			break;
		case FESYSMESS:
			msg = " System message received: error 6";
			break;
		case FENOSYSMESS:
			msg = " Unable to receive system messages: error 7";
			break;
 		case FEREADLOCK:
			msg = " A read through a lock was successful: error 9";
			break;
 		default:
			msg = " File error " + errorNumber_;
			break;
		}
		return super.getMessage() + msg;
    } // getMessage

} // EnscribeFileWarning
