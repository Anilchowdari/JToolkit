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
import java.util.Vector;

/**
 * <P>The AlternateKeyAlterAttributes class handles attributes that describe
 * alternate key information for structured files.
 *
 * An AlternateKeyAlterAttribute object is returned from
 * EnscribeFileAttributes.getAlternateKeyAttributes().
 * To change the alternate key attributes, use the set methods of
 * this object, then pass this object as a parameter to the
 * EnscribeFileAttributes.updateAlternateKeyAttributes() method.  After
 * you have made all the changes that you want to the attributes of a file,
 * pass the EnscribeFileAttributes object to the EnscribeFile.alter()
 * method.
 *
 * Refer to the description FILE_ALTERLIST_
 * procedure (documented in the Guardian Procedure Calls Reference Manual) for more
 * information on alternate key descriptor attributes and their values.</P>
 *
 * Example: To alter attributes of a file, you could do
 * the following:</P>
 *<PRE>
 *  try {
 *      EnscribeFile myfile = new EnscribeFile("/G/data/myvol/myfile");
 *      EnscribeFileAttributes fileAttr = myfile.getFileInfo();
 *      AlternateKeyAlterAttributes altKeyAttr = fileAttr.getAlternateKeyAlterAttributes();
 *      AlternateKeyDescriptor []
 *           altKeyDescr = altKeyAttr.getAltkeyInfo();
 *      altKeyDescr[0].setKeyIsUnique();
 *      altKeyAttr.setAltKeyDesc(altKeyDescr[0], 0);
 *      fileAttr.setAlternateKeyAttributes(altKeyAttr);
 *      myfile.alter(fileAttr, false);
 *  } catch (com.tandem.enscribe.EnscribeFileException ex) {
 *      System.out.println(ex.message);
 *  }
 *</PRE>
 * @see EnscribeFileAttributes
 * @see AlternateKeyDescriptor
 */

class AlternateKeyAlterAttributes
{
	int numberOfAltkeys_ = 0;
	int numberOfAltkeyfiles_ = 0;

	AlternateKeyDescriptor[] altKeyInfo_;
	String [] altKeyFileInfo_;
	int index_ = 0;

	AlternateKeyAlterAttributes(int numberOfAltkeys, int numberOfAltkeyfiles,
		String [] fileNames, AlternateKeyDescriptor [] akd) {

		numberOfAltkeys_ = numberOfAltkeys;
		numberOfAltkeyfiles_ = numberOfAltkeyfiles;
 		if(numberOfAltkeyfiles_ > 0) 	    {
			altKeyFileInfo_= fileNames;
		}
		altKeyInfo_ = akd;
	} // constructor for AlternateKeyAlterAttributes

	/**
    * Returns an array of AlternateKeyDescriptor objects.
    *
    * @return array of AlternateKeyDescriptor objects
    *
    * @exception com.tandem.enscribe.EnscribeFileException if file has no
    * alternate keys.
    *
    */
    AlternateKeyDescriptor [] getAltKeyInfo() throws EnscribeFileException{
		if(numberOfAltkeys_ < 1)
			throw new EnscribeFileException("No alternate keys exist for this file",
				EnscribeFileException.FEINVALOP,"alter","UNDEFINED");
		return altKeyInfo_;
    } // getAltkeyInfo

	/**
    * Returns an array of alternate key filenames.
    *
    * @return String array of alternate key filenames
    *
    * @exception com.tandem.enscribe.EnscribeFileException if file has no
    * alternate keys.
    */
    String [] getAltKeyFileNames() throws EnscribeFileException{
		if(numberOfAltkeyfiles_ < 1)
			throw new EnscribeFileException("No alternate keys exist for this file",
				EnscribeFileException.FEINVALOP,"alter","UNDEFINED");
		return altKeyFileInfo_;
    } // getAltkeyFileNames

	// ---------------------------------------------
	// ---------------------------------------------
	// ---------------------------------------------
	/*
	 * Internal method
	 */
	 String getAttributeString()
	{
		StringBuffer str = new StringBuffer(200);
		str.append(EnscribeFileAttributes.OV_NUMBER_OF_ALT_KEYS).append(",").append(numberOfAltkeys_).append(",");
		if(numberOfAltkeys_ > 0)
		{
			str.append(EnscribeFileAttributes.OV_ALT_KEY_DESCRIPTORS).append(",");
			AlternateKeyDescriptor
				akd;
			for(int i=0;i<numberOfAltkeys_;i++)
			{
				akd = altKeyInfo_[i];
				str.append(akd.getAttributeString());
			}

		}

		str.append(EnscribeFileAttributes.OV_NUMBER_OF_ALT_KEY_FILES).append(",").append(numberOfAltkeyfiles_).append(",");
		if(numberOfAltkeyfiles_ > 0)
		{
			str.append(EnscribeFileAttributes.OV_ALT_KEY_FILE_NAME_LENS).append(",");
			String akfName;
			for(int i=0;i<numberOfAltkeyfiles_;i++)
			{
				akfName = altKeyFileInfo_[i];
				str.append(akfName.length()).append(",");
			}

			str.append(EnscribeFileAttributes.OV_ALT_KEY_FILE_NAMES).append(",");
			for(int i=0;i<numberOfAltkeyfiles_;i++)
			{
				str.append(altKeyFileInfo_[i]).append(",");
			}

		}

		return str.toString();
	} // getAttributeString

} // AlternateKeyAlterAttr
