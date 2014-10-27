/*---------------------------------------------------------------------------
 * Copyright 2003
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

package com.tandem.ext.enscribe;

/**
 * <P>UnstructuredCreationAttr handles attributes used
 * when creating an unstructured disk file.  UnstructuredCreationAttr is passed
 * to the EnscribeFile.fileCreate() method.</P>
 *
 * <P>Example:  To create an unstructured file, you could do the following:</P>
 *
 *<PRE>
 *  try {
 *      EnscribeFile myfile = new EnscribeFile("/G/data/myvol/myfile");
 *      UnstructuredCreationAttributes creationAttr = new UnstructuredCreationAttributes();
 *      creationAttr.setPrimaryExtentSize(16);
 *      creationAttr.setSecondaryExtentSize(16);
 *      creationAttr.setMaximumExtents(100);
 *      creationAttr.setOddUnstructured(true);
 *      myfile.create(creationAttr);
 *  } catch (com.tandem.enscribe.EnscribeFileException ex) {
 *      System.out.println(ex.getMessage());
 *  }
 *</PRE>
 *
 * <P>See the description of the FILE_CREATELIST_ procedure (documented in the Guardian
 * Procedure Calls Reference Manual) for more information about unstructured disk file
 * creation attributes and their values.</P>
 * @see com.tandem.ext.enscribe.EnscribeCreationAttributes
 */

public class UnstructuredCreationAttributes extends EnscribeCreationAttributes
{
	boolean oddUnstructuredFlag_ = false;
	final int   OV_ODD_UNSTRUCTURED = 65;


	/**
    * Constructs an UnstructuredCreationAttributes object.
    *
    */
    public UnstructuredCreationAttributes()  {
		fileType_ = FILETYPE_UNSTRUCTURED;
		fileTypeSet_= true;

    } // UnstructuredCreationAttributes

	/**
    * Specifies whether the file has the odd unstructured attribute.  If
    * this odd unstructured attribute is set, I/O transfers occur with the
    * exact count specified; otherwise, transfers are rounded up to an
    * even byte boundary.
    * @param value true if the file is to be odd unstructured;false otherwise.
    *
    */  
    /* Sol: 10-030703-7582 Corrected Sample program in javadoc comments */
    public void setOddUnstructured(boolean value) {
		oddUnstructuredFlag_  = value;
    } // setOddUnstructuredFlag
 	/**
     * Returns the value set for odd unstructured.
     * @return true if the file is to be odd unstructured;false otherwise.
     *
     */
     public boolean getOddUnstructured() {
 		return oddUnstructuredFlag_;
    } // setOddUnstructuredFlag

	// -------------------------------------------
	// -------------------------------------------
	/*
	 * Internal method
	 */
	 String getAttributeString(boolean value)
	{
		String str = "<UC,";
		str += super.getAttributeString(value);
		if(oddUnstructuredFlag_) {
			str += OV_ODD_UNSTRUCTURED + ",1,";
		}

		str += OV_LAST_OPTVAL + ",UC>,";

		return str;
	} // getAttributeString


} // UnstructuredCreationAttributes
