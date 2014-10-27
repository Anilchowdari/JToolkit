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

/**
 * This exception indicates that an error has occurred while trying to
 * parse a delimited string.
 */

class ParseDelimitedStringException extends Exception
{
	private boolean end_of_string = false;
   /**
    * Constructs a <code>ParseDelimitedStringException</code> with the specified
    * descriptive message.
    *
    * @param   msg the descriptive message
    */
    ParseDelimitedStringException(String msg) {
        super(msg);
    } // ParseDelimitedStringException

   /**
    * Constructs a <code>ParseDelimittedStringException</code>
    * indicating an end-of-string condition.
    *
    * @param  eos true if end_of_string
    */
    ParseDelimitedStringException(boolean eos) {
        super("End of string");
		end_of_string = eos;
    } // ParseDelimitedStringException

	/**
	 * Returns true if the end_of_string flag is set.
	 *
	 * @return true if the end_of_string flag is set
	 */
	boolean atEndOfString() {
		return end_of_string;
	} // atEndOfLine

} // ParseDelimitedStringException
