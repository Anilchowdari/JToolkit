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
import java.util.Vector;
import java.util.Enumeration;
/**
 * <P>StructuredCreationAttributes handles the attributes used
 * to create a structured disk file, specifically a relative or
 * entry-sequenced file.</P>
 * <P>See the description of the
 * FILE_CREATELIST_ procedure (documented in the Guardian Procedure Calls Reference Manual)
 * for more information about structured file attributes and their values.</P>
 *
 * <P>The following shows how a StructuredCreationAttributes object can
 * be used to create a relative file:</P>
 *
 * <PRE>
 *	try {
 *		EnscribeFile newFile = new EnscribeFile("$data.foo.newfile");
 *		StructuredFileCreationAttributes createAttr =
 *			new StructuredFileCreationAttributes();
 *		createAttr.setFileType(FILETYPE_RELATIVE);
 *		createAttr.setMaximumExtents(100);
 *		createAttr.setBlockLength(1028);
 *		createAttr.setRecordLength(257);
 *		newFile.create(createAttr);
 *	} catch (Exception (ex))
 * 		System.out.println(ex.getMessage());
 *	}
 * </PRE>
 *
 * @see com.tandem.ext.enscribe.EnscribeCreationAttributes
 * @see com.tandem.ext.enscribe.KeySequencedCreationAttributes
 * @see com.tandem.ext.enscribe.UnstructuredCreationAttributes
 */

public class  StructuredCreationAttributes extends EnscribeCreationAttributes
{
	// we need to know which attributes have been set
	// the following are flags to tell us.
	boolean blockLengthSet_ = false;
	boolean logicalRecordLengthSet_ = false;
	boolean numberOfAltkeysSet_ = false;
	boolean numberOfAltkeyfilesSet_ = false;
	boolean alternateKeyFileNamesSet_ = false;
	boolean alternateKeyDescriptorsSet_ = false;
	boolean [] altDescSet_;
	int logicalRecordLength_ = 0;
	int blockLength_ = 0;
	int numberOfAltkeys_ = 0;
	int numberOfAltkeyfiles_ = 0;
	AlternateKeyDescriptor [] altdesc_;
	Vector altKeyFileNames_;

	private static final int    OV_LOGICAL_RECORD_LENGTH = 196;
	private static final int    OV_BLOCK_LENGTH = 197;
	private static final int    OV_NUMBER_OF_ALT_KEYS = 100;
	private	static final int    OV_ALT_KEY_DESCRIPTORS = 101;
	private	static final int    OV_NUMBER_OF_ALT_KEY_FILES = 102;
	private	static final int    OV_ALT_KEY_FILE_NAME_LENS = 103;
	private static final int    OV_ALT_KEY_FILE_NAMES = 104;


	/**
    * Constructs a StructuredCreationAttr object.
    *
    */
    public StructuredCreationAttributes()  {
		altKeyFileNames_ = new Vector();

    } // StructuredCreationAttributes

    /**
	 * Sets the attributes for the file being created to match those
	 * of the named file.  This is equivalent to FUP SET LIKE.
	 *
	 * @param fileName The name of the file whose attributes are to be
	 * matched.
	 * @param samePartitions a boolean, true if using same partition descriptors
	 * @exception EnscribeFileException if an error occurs trying to obtain information
	 * about fileName.
	 */
	public void setLike(String fileName, boolean samePartitions)
		throws EnscribeFileException {
		EnscribeFileAttributes attr = new EnscribeFileAttributes(fileName);
		this.setLike(attr,samePartitions);

  	} // setLike

	/**
    * Sets the attributes for the file being created to match those
    * of an existing EnscribeFileAttributes object.  This is equivalent to FUP SET LIKE.
    *
    * @param fileAttributes EnscribeFileAttributes object to be used
    * @param samePartitions a boolean, true if using same partition descriptors
    * @exception EnscribeFileException if an error occurs.
    */
    public void setLike(EnscribeFileAttributes fileAttributes, boolean samePartitions)
    	throws EnscribeFileException {
		super.setLike(fileAttributes, samePartitions);
		logicalRecordLength_ = fileAttributes.logicalRecordLength_;
  		logicalRecordLengthSet_ = true;
		blockLength_ = fileAttributes.blockLength_;
		blockLengthSet_ = true;
		String [] fileNames = null;

		numberOfAltkeys_ = fileAttributes.numberOfAltkeys_;
		numberOfAltkeyfiles_ = fileAttributes.numberOfAltkeyfiles_;
		if(numberOfAltkeys_ > 0)
	   {
			altdesc_ = fileAttributes.getAltKeyDescriptors();
		}
 		if(numberOfAltkeyfiles_ > 0)
	    {
			fileNames = fileAttributes.getAltKeyFileNames();
		}
		if(fileNames != null) {
			for(int i = 0; i < fileNames.length; i++) {
				altKeyFileNames_.insertElementAt(fileNames[i],i);
			}
		}


  } // setLike

	/**
    * Sets the block length for the file that is being created. It this method is
    * not invoked, block length defaults to 4096.
    * Refer to item 197 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param blockLen the block length
    * @exception java.lang.IllegalArgumentException if the blockLen
    * is negative or greater than 4096
    *
    * @see EnscribeFileAttributes#getBlockLength
    * @see #getBlockLength
    */
    public void setBlockLength(int blockLen) throws IllegalArgumentException{
		if(blockLen < 0 || blockLen > 4096)
			throw new IllegalArgumentException("Block length must be positive and <= 4096");
		blockLength_ = blockLen;
                blockLengthSet_ = true; /* Sol: 10-030724-8159 */
    } // setBlockLength

	/**
    * Gets the block length for the file that is being created
    * @return the current setting for block length
    * @see #setBlockLength
    */
    public int getBlockLength() {
		return blockLength_;
	} // getBlockLength

   /**
    * Sets the maximum record length in bytes.
    * If the record length is not set, the default value is 80.
    * Refer to item 196 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param recordLen maximum number of bytes for a record.
    *
    * @see #getRecordLength
    * @see #getRecordLength
    */
    public void setRecordLength(int recordLen) {
        logicalRecordLength_ = recordLen;
		logicalRecordLengthSet_ = true;
    } // setRecordLength


   /**
    * Gets the current value for the record length
	* @return the current value for record length
	* @see #setRecordLength
    */
    public int getRecordLength() {
		return logicalRecordLength_ ;
    } // getRecordLength
   /**
    * Gets the number of alternate keys specified for the file being created.
    *
    * @return the number of alternate keys
    *
    * @see #setAlternateKeyDescriptors
    * @see #getAlternateKeyDescriptors
   */
    public int getNumberOfAltkeys() {
		return numberOfAltkeys_;
	}

	/**
    * Sets information about the alternate keys for the file being created.  If
    * this method is not invoked, the file has NO alternate keys.
    * Refer to item 101 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
	*
    * @param altKeyDescriptors an array of AlternateKeyDescriptor objects
    * @exception java.lang.IllegalArgument if the
    * array contains less than 1 AlternateKeyDescriptor, or if any one
    * of the AlternateKeyDescriptor  objects does not have a file name set or if the
    * total number of file names specified for the AlternateKeyDescriptor objects exceeds 100.
    * @see AlternateKeyDescriptor
    * @see #getAlternateKeyDescriptors
    */
    public void setAlternateKeyDescriptors(AlternateKeyDescriptor[] altKeyDescriptors)
		throws java.lang.IllegalArgumentException {

		int i = 0;
		int numKeys = 0;
		int numFiles = 0;

		if(altKeyDescriptors == null)
			throw new IllegalArgumentException("altKeyDescriptors cannot be null");
		int length = altKeyDescriptors.length;
		if(length == 0)
			throw new IllegalArgumentException("At least one AlternateKeyDescriptor object must be supplied");
		if(length > 100)
			throw new IllegalArgumentException("More than 100 AlternateKeyDescriptor objects were supplied");
		numKeys = length;
		if(!altKeyFileNames_.isEmpty()) {
			altKeyFileNames_.clear();
			numberOfAltkeyfiles_ = 0;
		}

		for(i = 0; i < length; i++) {
			if(altKeyDescriptors[i].alternateKeyFileNameSet_) {
				setDescriptorForFile(altKeyDescriptors[i].getAlternateKeyFileName(),
					altKeyDescriptors[i]);
			} else {
				throw new IllegalArgumentException("AlternateKeyDescriptor["+i+"] does not have a file name set");
			}
		}
		if(numberOfAltkeyfiles_ > 100) {
			numberOfAltkeyfiles_ = 0;
			throw new IllegalArgumentException("More than 100 alternate key files specified");
		}
		numberOfAltkeysSet_ = true;
		numberOfAltkeyfilesSet_ = true;
		numberOfAltkeys_ = length;
		altdesc_ = altKeyDescriptors;
	} // setAlternateKeyDescriptors
	/**
    * Returns an array of AlternateKeyDescriptor objects that will be used when the file is created.
    *
    * @return an array of AlternateKeyDescriptor objects
    *
    * @exception EnscribeFileException if no AlternateKeyDescriptor objects have been set.
    * @see AlternateKeyDescriptor
    * @see #setAlternateKeyDescriptors
    */
    public  AlternateKeyDescriptor [] getAlternateKeyDescriptors() throws EnscribeFileException{
		if(numberOfAltkeys_ < 1)
			throw new EnscribeFileException("No alternate keys were set for this object",
				EnscribeFileException.FEINVALOP,"getAlternateKeyDescriptors","UNDEFINED");
		return(altdesc_);
    } // getAlternateKeyDescriptors

	void setDescriptorForFile(String fileName, AlternateKeyDescriptor desc) {
		int i = 0;
		if(altKeyFileNames_.isEmpty()) {
			altKeyFileNames_.insertElementAt(fileName,numberOfAltkeyfiles_);
			desc.setKeyFileNum(0);
			numberOfAltkeyfiles_++;
			return;
		}
		for (Enumeration e = altKeyFileNames_.elements(); e.hasMoreElements() ;) {
			String tempName = (String)e.nextElement();
			if(tempName.equalsIgnoreCase(fileName)) {
				desc.setKeyFileNum(i);
				return;
			}
			i++;
	    }

		altKeyFileNames_.insertElementAt(fileName,i);
		numberOfAltkeyfiles_++;
		desc.setKeyFileNum(i);
		return;
	}

	// ---------------------------------------------------
	// ---------------------------------------------------
	/*
	 * Internal method
	 */
	 String getAttributeString(boolean getPartitionInfo)
	{
		String str1;
		if(fileType_ == com.tandem.ext.enscribe.EnscribeFileAttributes.FILETYPE_RELATIVE) {
			str1 = "<RC," + super.getAttributeString(true);
		} else {
			if (fileType_ == com.tandem.ext.enscribe.EnscribeFileAttributes.FILETYPE_ENTRY_SEQUENCED) {
				str1 = "<EC," + super.getAttributeString(true);
			} else {
	    		str1 = super.getAttributeString(false);
			}
		}
		StringBuffer str = new StringBuffer(str1.length() + 200);
		str.append(str1);

		if(logicalRecordLengthSet_)
			str.append(OV_LOGICAL_RECORD_LENGTH).append(",").append(logicalRecordLength_).append(",");
		if(blockLengthSet_)
			str.append(OV_BLOCK_LENGTH).append(",").append(blockLength_).append(",");

		//System.out.println("number of Alt keys = "+numberOfAltkeys_);
		if(numberOfAltkeys_ > 0)
		{
			//System.out.println("numberOfAltKeys_ = "+numberOfAltkeys_);
			str.append(OV_NUMBER_OF_ALT_KEYS).append(",").append(numberOfAltkeys_).append(",");
			str.append(OV_ALT_KEY_DESCRIPTORS).append(",");
			AlternateKeyDescriptor
				akd;
			for(int i=0;i<numberOfAltkeys_;i++)
			{
				akd = altdesc_[i];
				str.append(akd.getAttributeString());
			}

		}
		//System.out.println("num alt key files "+numberOfAltkeyfiles_);

		if(numberOfAltkeyfiles_ > 0)
		{
			str.append(OV_NUMBER_OF_ALT_KEY_FILES).append(",").append(numberOfAltkeyfiles_).append(",");
			str.append(OV_ALT_KEY_FILE_NAME_LENS).append(",");
			String 	akfName;
			StringBuffer sb = new StringBuffer(100);

			for (Enumeration e = altKeyFileNames_.elements() ; e.hasMoreElements() ;) {
				akfName = (String)e.nextElement();
				str.append(akfName.length()).append(",");
				sb.append(akfName).append(",");
			}

			str.append(OV_ALT_KEY_FILE_NAMES).append(",");
			str.append(sb.toString());
			//System.out.println("sb.toString = "+sb.toString());
		}
		if(fileType_ == com.tandem.ext.enscribe.EnscribeFileAttributes.FILETYPE_RELATIVE)
			str.append(OV_LAST_OPTVAL).append(",RC>,");
		else if (fileType_ == com.tandem.ext.enscribe.EnscribeFileAttributes.FILETYPE_ENTRY_SEQUENCED)
			str.append(OV_LAST_OPTVAL).append(",EC>,");

		return str.toString().trim();
	} // getAttributeString
} // StructuredCreationAttr
