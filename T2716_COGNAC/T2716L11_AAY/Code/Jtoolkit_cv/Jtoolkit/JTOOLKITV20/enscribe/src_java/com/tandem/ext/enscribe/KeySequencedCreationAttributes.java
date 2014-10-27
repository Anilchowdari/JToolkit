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
 * <P>KeySequencedCreationAttributes sets attributes used to
 * create a key-sequenced file.</P>
 *
 * <P>Example: To create a key-sequenced file, you could do the following:</P>
 *
 *<PRE>
 *  try {
 *      EnscribeFile myfile = new EnscribeFile("/G/data/myvol/myfile");
 *      KeySequencedCreationAttributes creationAttr =
 *			new KeySequencedCreationAttributes();
 *      creationAttr.setPrimaryExtentSize(16);
 *      creationAttr.setSecondaryExtentSize(16);
 *      creationAttr.setMaximumExtents(100);
 *      creationAttr.setAudited();
 *      creationAttr.setRecordLength(54);
 *      creationAttr.setBlockLength(4096);
 *      creationAttr.setKeyLength(2);
 *      creationAttr.setKeyOffset(0);
 *      myfile.create(creationAttr);
 *  }
 *  catch (com.tandem.enscribe.EnscribeFileException ex) {
 *      System.out.println(ex.getMessage());
 *  }
 *  catch (java.lang.IllegalArgumentException ia) {
 *		System.out.pritnln(ia.getMessage());
 *  }
 *</PRE>
 *
 * <P>See the description of the FILE_CREATELIST_ procedure (documented in the Guardian
 * Procedure Calls Reference Manual) for more information about key-sequenced disk
 * file creation attributes and their values.</P>
 *
 * @see com.tandem.ext.enscribe.EnscribeCreationAttributes
 * @see com.tandem.ext.enscribe.StructuredCreationAttributes
 * @see com.tandem.ext.enscribe.UnstructuredCreationAttributes
*/

public class KeySequencedCreationAttributes extends StructuredCreationAttributes
{
	boolean partitionPartialKeyLengthSet_ = false;
	int keyOffset_ = 0;
	int keyLength_ = 0;
	boolean dataCompressionFlag_ = false;
	boolean indexCompressFlag_ = false;
	int partitionPartialKeyLength_ = 0;
	private static final int    OV_KEY_OFFSET = 198;
	private static final int    OV_KEY_LENGTH = 46;
	private static final int    OV_DATA_COMPRESSION_FLAG = 68;
	private static final int    OV_INDEX_COMPRESS_FLAG = 69;

   /**
    * Constructs a KeySequencedFileCreationAttributes object.
    */
    public KeySequencedCreationAttributes() {
		super();
		fileType_ = FILETYPE_KEY_SEQUENCED;
		fileTypeSet_ = true;
    } // KeySequencedCreationAttributes
	/**
    * Sets the attributes for this key-sequenced file to match the
    * attributes of the named existing file.  This is equivalent to SET LIKE.
    *
    * @param fileName a String containing the name of the file to match
    * @param samePartitions a boolean, true if using same partition descriptors
    * @exception com.tandem.enscribe.EnscribeFileException if an error occurs.
    * @exception java.lang.IllegalArgumentException if the file name specified is invalid
    * or if the file does not exist.
    */
    public void setLike(String fileName, boolean samePartitions) throws EnscribeFileException, java.lang.IllegalArgumentException {
		EnscribeFileAttributes efa = new EnscribeFileAttributes(fileName);
		this.setLike(efa,samePartitions);
    } // setLike
	/**
    * Sets the attributes for this key-sequenced file to match the
    * attributes of an existing EnscribeFileAttributes object.  This is equivalent to SET LIKE.
    *
    * @param fileAttributes the EnscribeFileAttributes object to use
    * @param samePartitions a boolean, true if using same partition descriptors
    * @exception EnscribeFileException if an error occurs.
    */
    public void setLike(EnscribeFileAttributes fileAttributes, boolean samePartitions)
    	throws EnscribeFileException {
		super.setLike(fileAttributes, samePartitions);
		indexCompressFlag_ = fileAttributes.indexCompressFlag_;
		dataCompressionFlag_ = fileAttributes.dataCompressionFlag_;
		keyLength_ = fileAttributes.keyLength_;
		keyOffset_ = fileAttributes.keyOffset_;
		partitionPartialKeyLength_ = fileAttributes.partitionPartialKeyLength_;

		if(numberOfPartitions_ > 0)
		{
			partitionPartialKeyLengthSet_ = true;
		}
		else
			partitionPartialKeyLengthSet_ = false;


    } // setLike


   /**
    * Specifies whether index blocks are to be compressed for the file that is
    * being created.  If this method is not invoked, index blocks are
    * NOT compressed.
	* @param value Specify true if index blocks are to be compressed; false otherwise
    * @see #getIndexCompression
    */
    public void setIndexCompression(boolean value) {
        indexCompressFlag_ = value;
    } // setIndexCompression

   /**
    * Gets the value for index compression that will be used when the file is created.
	* @return true if index compression is to be used; false otherwise
    * @see #setIndexCompression
    */
    public boolean getIndexCompression() {
		return indexCompressFlag_;
    } // getIndexCompression

   /**
    * Specifies whether primary key values are to be compressed for the file that
    * is being created.  If this method is not invoked, primary key values are
    * NOT compressed.
	* @param value Specify true if primary key values are to be compressed; false otherwise.
    * @see #getDataCompression
    */
    public void setDataCompression(boolean value) {
        dataCompressionFlag_ = value;
    } // setDataCompression

   /**
    * Indicates whether primary key values will be compressed.
	* @return true if primary key values are to be compressed; false otherwise
    * @see #setDataCompression
    */
    public boolean getDataCompression() {
		return dataCompressionFlag_;
    } // getDataCompression

    /**
	 * Sets information about partitions for the file to be created.  If this
	 * method is not invoked, the file has NO partitions.
	 * Refer to item 97 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
	 * @param numberOfPartitions the number of partitions for the file to be created.  Specifying 0 for numberOfPartitions
	 * indicates that the file to be created has no partitions (the default).
	 * @param descriptors an array of PartitionDescriptors, the array must contain one descriptor for each
	 * partition.  The array can be empty if 0 is specified for numberOfPartitions.
	 * @exception java.lang.IllegalArgumentException if numberOfPartitions is negative
	 * or if descriptors.length is not equal to numberOfPartitions
	 *
	 * @see #getPartitionDescriptors
	 * @see #getNumberOfPartitions
	 */
	 public void setPartitionDescriptors(int numberOfPartitions, PartitionDescriptor[] descriptors)
	   	throws java.lang.IllegalArgumentException{
		if(numberOfPartitions < 0 )
			throw new IllegalArgumentException("Number of partitions must be positive");
		numberOfPartitions_ = numberOfPartitions;
		if(numberOfPartitions == 0) {
			numberOfPartitionsSet_ = false;
			return;
		}
		if(descriptors == null)
			throw new IllegalArgumentException("descriptors cannot be null");
		if(numberOfPartitions != descriptors.length) {
			numberOfPartitionsSet_ = false;
			throw new IllegalArgumentException("descriptors.length must equal numberOfPartitions");
		}

		byte [] partKey = descriptors[0].getPartialKeyBytesValue();
		int partialKeyLen = partKey.length;
		boolean keyLengthMatches = true;
		for(int i = 1; i < descriptors.length; i++) {
			partKey = descriptors[i].getPartialKeyBytesValue();
			if(partKey.length != partialKeyLen)
				throw new IllegalArgumentException("Partial key values of varying lengths were specified in descriptors");
		}
		numberOfPartitionsSet_ = true;
		partitionInfo_ = descriptors;
		setPartitionPartialKeyLength(partialKeyLen);
	} // setPartitionInfo


   /**
    * Specifies the maximum length in bytes of the primary key field.  This
    * attribute is REQUIRED.
    *
    * @param keyLen length in bytes of the primary key field for key-sequenced files.
    * This attribute is required.  See item code 46 of
    * the Guardian FILE_CREATELIST_ procedure.
    * @exception java.lang.IllegalArgumentException if keyLen is greater than
    * 255 or is negative
    * @see #getKeyLength
    */
    public void setKeyLength(int keyLen) throws java.lang.IllegalArgumentException{
        if(keyLen > 255 || keyLen < 1)
			throw new IllegalArgumentException("Invalid key length");
		keyLength_ = keyLen;
    } // setKeyLength
	/**
    * Gets the maximum length in bytes of the primary key field that was specified for this file.
    * @return the primary key length
    * @see #getKeyLength
    */
    public int getKeyLength() {
  		return keyLength_ ;
    } // getKeyLength


   /**
    * Sets the offset from the beginning of the record
	* to the beginning of the primary key field.
	* This attribute is REQUIRED.
    *
    * See item code 198 of the Guardian FILE_CREATELIST_ procedure.
    * @param keyOffset specifies the offset.  This attribute is required.
    * @exception java.lang.IllegalArgumentException if keyOffset
    * is negative or is greater than the record length.
    *
    * @see #getKeyOffset
    */
    public void setKeyOffset(int keyOffset) throws java.lang.IllegalArgumentException {
        if(keyOffset > logicalRecordLength_ || keyOffset < 0)
			throw new IllegalArgumentException("Invalid key offset");
        keyOffset_ = keyOffset;
    } // setKeyOffset

 	/**
    * Gets the offset from the beginning of the record
	* to the beginning of the primary-key field.
    *
    * See item code 45 of the Guardian FILE_CREATELIST_ procedure.
    * @return the keyOffset
    *
    * @see #getKeyOffset
    */
    public int getKeyOffset(){
  		return keyOffset_;
    } // getKeyOffset


   /**
    * Sets the length of the partial key that will be used for all partitions when this file is created.
    * This method is useful when this KeySequencedCreationAttributes object has
    * been initialized using the setLike() method.
    * See item code 94 of the Guardian FILE_CREATELIST_ procedure.
    *
    * @param partitionPartialKeyLength length of the partition partial key
    * @exception java.lang.IllegalArgumentException if the length of the partition partial
    * key is negative or is greater than the key length
    *
    * @see #getPartitionPartialKeyLength
   */
    public void setPartitionPartialKeyLength(int partitionPartialKeyLength) throws java.lang.IllegalArgumentException{
		if(partitionPartialKeyLength < 0 )
			throw new IllegalArgumentException("Partition partial key length must be positive");
		if(partitionPartialKeyLength > keyLength_)
			throw new IllegalArgumentException("Partition partial key length must be < keyLength: " + keyLength_);
		partitionPartialKeyLength_ = partitionPartialKeyLength;
		if(numberOfPartitions_ > 0)
		{
			partitionPartialKeyLengthSet_ = true;
		}
		else
			partitionPartialKeyLengthSet_ = false;
    } // setPartitionPartialKeyLength

 	/**
    * Gets the length of the partition partial key that will be used when this file is created.
    * This method is useful when this KeySequencedCreationAttributes object has
    * been initalized using the setLike() method.
    * See item code 94 of the Guardian FILE_CREATELIST_ procedure.
    *
    * @return the length of the partition partial key
    *
    * @see #setPartitionPartialKeyLength
   */
    public int getPartitionPartialKeyLength() {
		return partitionPartialKeyLength_;
    } // getPartitionPartialKeyLength


	// ----------------------------------------------
	// ----------------------------------------------
	/*
	 * Internal method
	 */
	 String getAttributeString(boolean getPartitionInfo)
	{
		int i = 0;
		String	str1 = "<KC," + super.getAttributeString(false);
		StringBuffer str = new StringBuffer(str1.length() + 300);
		str.append(str1);
		// We ignore the value of getPartitionInfo and get it here
		// If we get part from the super and part here Guardian
		// complains because it all has to be described sequentially.
		if(numberOfPartitions_ > 0) {
			str.append(OV_NUMBER_OF_PARTITIONS).append(",").append(numberOfPartitions_).append(",");
			str.append(OV_PARTITION_EXTENTS).append(",");
			PartitionDescriptor pd;
			for(i=0;i<numberOfPartitions_;i++) {
				pd = partitionInfo_[i];
				str.append(pd.getPrimaryExtentSize()).append(",");
				str.append(pd.getSecondaryExtentSize()).append(",");
			}
			str.append(OV_PARTITION_VOLUME_NAME_LENS).append(",");
			for(i=0;i<numberOfPartitions_;i++) {
				pd = partitionInfo_[i];
				str.append(pd.getVolumeName().length()).append(",");
			}
			str.append(OV_PARTITION_VOLUME_NAMES).append(",");
			for(i=0;i<numberOfPartitions_;i++) {
				pd = partitionInfo_[i];
				str.append(pd.getVolumeName()).append(",");
			}
			if(partitionPartialKeyLengthSet_ && partitionPartialKeyLength_ > 0) {
				str.append(OV_PARTITION_PARTIAL_KEY_LENGTH).append(",").append(partitionPartialKeyLength_).append(",");
				str.append(OV_PARTITION_PARTIAL_KEY_VALS).append(",");
				byte [] pkv;
				for(i=0;i<numberOfPartitions_;i++) {
					pd = partitionInfo_[i];
					pkv= pd.getPartialKeyBytesValue();
					int j;
					int keyLen = pkv.length;
					if(keyLen > partitionPartialKeyLength_) {
						keyLen = partitionPartialKeyLength_;
						for(j=0;j<keyLen;j++)
							str.append(pkv[j]).append(",");
					} else {
						for(j=0;j<pkv.length;j++)
							str.append(pkv[j]).append(",");
					}
				}
			}

		}
		str.append(OV_KEY_LENGTH).append(",").append(keyLength_).append(",");
		str.append(OV_KEY_OFFSET).append(",").append(keyOffset_).append(",");
		if(dataCompressionFlag_)
			str.append(OV_DATA_COMPRESSION_FLAG).append(",1,");
		if(indexCompressFlag_)
			str.append(OV_INDEX_COMPRESS_FLAG).append(",1,");





		str.append(OV_LAST_OPTVAL).append(",KC>,");
		return str.toString().trim();
	} // getAttributeString

} // KeySequencedCreationAttr
