/* ------------------------------------------------------------------------
* Filename:     GuardianOutput.java
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2001
*   Compaq Computer Corporation
*      Protected as an unpublished work.
*         All rights reserved.
*
*   The computer program listings, specifications, and documentation
*   herein are the property of Compaq Computer Corporation or a
*   third party supplier and shall not be reproduced, copied,
*   disclosed, or used in whole or in part for any reason without
*   the prior express written permission of Compaq Computer
*   Corporation.
*
*  @@@ END COPYRIGHT @@@
*
* ------------------------------------------------------------------------ */

package com.tandem.ext.guardian;

import com.tandem.ext.util.DataConversionException;

/**
 * The <code>GuardianOutput</code> interface can be passed to certain 
 * output methods.
 * The ddl2java tool generates classes that implement this interface.
 *
 * @version 1.0, 11/14/2001
 */
public interface GuardianOutput  {

   /**
    * Returns the minimum required length of the byte array passed to marshal.
    * Use this value to construct a byte array to pass to
    * the {@link #marshal} method.
    *
    * @return  minimum required length of the byte array passed to marshal
    */
    public int getLength();

   /**
    * Returns a byte array of the appropriate size to hold the data from
    * the individual fields of the class.  Converts the fields if necessary
    * before moving the values to the byte array.
    * This method is typically called before writing data
    * out.  Output methods that
    * accept a GuardianOutput parameter will call this method for the user.
    *
    * @return  byte array filled with data
    * @exception DataConversionException when there has been an unrecoverable
    * error converting data from the individual fields into the byte array.
    */
    public byte[] marshal() throws DataConversionException;

   /**
    * Moves data from the instance variables to the supplied byte array and 
    * returns the byte array.  Converts the fields if necessary
    * before moving the values to the byte array.  If the byte array
    * passed in is not long enough to hold all the data in the instance
    * variables a DataConversionException will be thrown.  Call {@link #getLength}
    * to find out the minimum size the byte array should be.
    *
    * @param buffer the array of bytes where the data in the instance variables should be moved
    * @return  byte array filled with data
    * @exception DataConversionException when there has been an unrecoverable
    * error converting data from the individual fields into the byte array.
    */
    public byte[] marshal(byte[] buffer) throws DataConversionException;

}  /* end class GuardianOutput */
