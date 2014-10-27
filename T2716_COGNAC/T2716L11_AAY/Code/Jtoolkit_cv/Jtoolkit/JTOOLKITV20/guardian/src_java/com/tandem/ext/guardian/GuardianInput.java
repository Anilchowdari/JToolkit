/* ------------------------------------------------------------------------
* Filename:     GuardianInput.java
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
 * The <code>GuardianInput</code> interface can be passed to certain input
 * methods.  The ddl2java tool generates classes that implement this interface.
 *
 * @version 1.0, 11/14/2001
 */
public interface GuardianInput  {

   /**
    * Returns the minimum required length of the byte array passed to unmarshal.
    * Use this value to construct a byte array to pass to
    * the {@link #unmarshal} method.
    *
    * @return  minimum required length of the byte array passed to unmarshal
    */
    public int getLength();

   /**
    * Takes the data in the byte array passed in and moves it to individual 
    * fields in the object.  The unmarshal method must do any appropriate 
    * data conversions.  This method is typically called after reading data
    * in.  Methods in classes that are part of the Extensions product (T2716)
    * that accept a GuardianInput parameter will call unmarshal.
    *
    * @param buffer the array of bytes containing the data to be converted and moved to fields
    * @param count the number of bytes in buffer that should be unmarshalled
    * @exception DataConversionException when there has been an unrecoverable
    * error converting data from the byte array into individual fields.
    */
    public void unmarshal(byte[] buffer, int count) throws DataConversionException;


}  /* end class GuardianInput */
