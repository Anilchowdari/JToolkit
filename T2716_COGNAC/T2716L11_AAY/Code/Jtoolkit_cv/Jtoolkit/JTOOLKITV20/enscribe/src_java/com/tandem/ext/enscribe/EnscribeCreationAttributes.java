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
import com.tandem.ext.enscribe.EnscribeFileException;
import java.util.*;
/**
 * <P>EnscribeCreationAttributes provides methods to set attributes used
 * to create a structured or unstructured disk file. </P>
 * <P>See the description of the
 * FILE_CREATELIST_ procedure (documented in the Guardian Procedure Calls Reference Manual)
 * for more information about these attributes and their values.</P>
 * @see com.tandem.ext.enscribe.StructuredCreationAttributes
 * @see com.tandem.ext.enscribe.KeySequencedCreationAttributes
 * @see com.tandem.ext.enscribe.UnstructuredCreationAttributes
 */

public abstract class EnscribeCreationAttributes
{
	/**
	 * The file type for an unstructured file
	 */
	public static final int FILETYPE_UNSTRUCTURED = 0;
	/**
	 * The file type for a relative file
	 */
	public static final int FILETYPE_RELATIVE = 1;
	/**
	 * The file type for an entry-sequenced file
	 */
	public static final int FILETYPE_ENTRY_SEQUENCED = 2;
	/**
	 * The file type for a key-sequenced file
	 */
	public static final int FILETYPE_KEY_SEQUENCED = 3;

	// we need to know which attributes have been set
	// the following are flags to tell us.
	boolean fileTypeSet_ = false;
	boolean fileCodeSet_ = false;
	boolean fileFormatSet_ = false;
	boolean primaryExtentSizeSet_ = false;
	boolean secondaryExtentSizeSet_ = false;
	boolean maximumExtentsSet_ = false;
	boolean numberOfPartitionsSet_ = false;
	boolean partitionDescriptorsSet_ = false;
	boolean [] partitionSet_;
	//protected boolean sequentialBlockBufferingFlagSet_ = false;
	boolean expirationTimeSet_ = false;
	int fileType_ = 0;
	int fileCode_ = 0;
	int fileFormat_ = 0;
	int primaryExtentSize_ = 0;
	int secondaryExtentSize_ = 0;
	int maximumExtents_ = 0;
	int allocatedExtents_ = 0;
	int numberOfPartitions_ = 0;
	GregorianCalendar expirationTime_;
	boolean auditedFlag_ = false;
	boolean auditCompressionFlag_ = false;
	PartitionDescriptor [] partitionInfo_;
	boolean refreshEofFlag_ = false;
	boolean writeThroughFlag_ = false;
	boolean verifyWritesFlag_ = false;
	boolean serialWritesFlag_ = false;

	final int    OV_FILE_TYPE = 41;
	final int    OV_FILE_CODE = 42;
	final int    OV_PRIMARY_EXTENT_SIZE = 199;
	final int    OV_SECONDARY_EXTENT_SIZE = 200;
	final int    OV_MAXIMUM_EXTENTS = 52;
	final int    OV_AUDITED_FLAG = 66;
	final int    OV_AUDIT_COMPRESSION_FLAG = 67;
	final int    OV_REFRESH_EOF_FLAG = 70;
	final int    OV_WRITE_THROUGH_FLAG = 72;
	final int    OV_VERIFY_WRITES_FLAG = 73;
	final int    OV_SERIAL_WRITES_FLAG = 74;
	final int    OV_NUMBER_OF_PARTITIONS = 90;
	final int    OV_PARTITION_EXTENTS = 97; /* Sol: 10-030723-8110 */     
	final int    OV_PARTITION_VOLUME_NAME_LENS = 92;
	final int    OV_PARTITION_VOLUME_NAMES = 93;
	final int    OV_PARTITION_PARTIAL_KEY_LENGTH = 94;
	final int    OV_PARTITION_PARTIAL_KEY_VALS = 95;
	final int    OV_EXPIRATION_TIME_GMT = 57;
	final int    OV_FILE_FORMAT = 195;
	final int    OV_LAST_OPTVAL = 9999;


	/**
    * Sets the attributes for the file being created to match those
    * of a named file.  This is equivalent to FUP SET LIKE.
    *
    * @param fileName the name of the file whose attributes are to be matched
    * @param samePartitions true if using the same partition descriptors as those
    * of the file to be matched
    * @exception com.tandem.ext.EnscribeFileException if an error occurs trying
    * to obtain information about fileName, if an invalid filename is specified, or if
    * the file doesn't exist.
    */
    public void setLike(String fileName, boolean samePartitions)
    	throws EnscribeFileException {
		EnscribeFileAttributes efa = new EnscribeFileAttributes(fileName);
		this.setLike(efa, samePartitions);
	} // setLike

	/**
    * Sets the attributes for the file being created to match those
    * of another file.  This is equivalent to FUP SET LIKE.
    *
    * @param efa a EnscribeFileAttributes object
    * @param samePartitions true if using the same partition descriptors as those
    * of the file described by the EnscribeFileAttributes object
    * @exception EnscribeFileException if an error occurs.
    */
    public void setLike(EnscribeFileAttributes efa, boolean samePartitions) throws EnscribeFileException {
		fileType_ = efa.fileType_;
		fileTypeSet_ = true;
		fileCode_ = efa.fileCode_;
		fileCodeSet_ = true;
		fileFormat_ = efa.fileFormat_;
		primaryExtentSize_ = efa.primaryExtentSize_;
		primaryExtentSizeSet_ = true;
		secondaryExtentSize_ = efa.secondaryExtentSize_;
		secondaryExtentSizeSet_ = true;
		maximumExtents_ = efa.maximumExtents_;
		maximumExtentsSet_ = true;
		numberOfPartitions_ = efa.numberOfPartitions_;
		numberOfPartitionsSet_ = true;
                /* Sol: 10-030711-7780 Begin */
                if(efa.expirationTime_ == null) 
                {
                   expirationTimeSet_ = false;
                } else {
                /* Sol: 10-030711-7780 End */
	  	  if(efa.expirationTime_.get(Calendar.YEAR) > -1)
	          {
			expirationTime_ = efa.expirationTime_;
			expirationTimeSet_ = true;
		  }
                } /* Sol: 10-030711-7780 */             

		if(numberOfPartitions_ > 0)
			partitionSet_ = new boolean[numberOfPartitions_];
		int q;
		if(samePartitions && numberOfPartitions_ > 0)
		{
			for(q = 0; q < numberOfPartitions_; q++)
				partitionSet_[q] = true;
			partitionInfo_ = efa.getPartitionDescriptors();
		} else {
			for(q = 0; q < numberOfPartitions_; q++)
				partitionSet_[q] = false;
		}
  } // setLike


	/**
    * Sets the file type for the file being created.  If this method is not
    * invoked, the file type defaults to FILETYPE_UNSTRUCTURED.
    * Refer to item 41 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param fileType the file code for this Enscribe file
    * @exception java.lang.IllegalArgumentException if an invalid file type
    * is specified.  Valid file types are: FILETYPE_UNSTRUCTURED, FILETYPE_RELATIVE,
    * FILETYPE_ENTRY_SEQUENCED, and FILE_TYPE_KEY_SEQUENCED.
	* @see #getFileType
	*/
    public void setFileType(int fileType) throws java.lang.IllegalArgumentException{
        switch(fileType)
		{
			case FILETYPE_UNSTRUCTURED:
			case FILETYPE_RELATIVE:
			case FILETYPE_ENTRY_SEQUENCED:
			case FILETYPE_KEY_SEQUENCED:
			break;
			default:
			throw new IllegalArgumentException("Invalid file type specified for setFileType");
		}
		fileType_ = fileType;
		fileTypeSet_ = true;
    } // setFileType
	/**
    * Gets the file type for the file being created.
    * @return the file type; one of: FILETYPE_UNSTRUCTURED, FILETYPE_RELATIVE,
    * FILETYPE_ENTRY_SEQUENCED, or FILE_TYPE_KEY_SEQUENCED.
	* @see #setFileType
	*/
    public int getFileType() {
       return fileType_;
    } // getFileType

   /**
    * Sets the file code for the file being created. If this method is not
    * invoked, the file code defaults to 0.
    * Refer to item 42 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param fileCode the file code for this Enscribe file
    * @exception java.lang.IllegalArgumentException if the file
    * code is negative
    *
    * @see #getFileCode
    */
    public void setFileCode(int fileCode) throws java.lang.IllegalArgumentException{
		if(fileCode < 0)
			throw new IllegalArgumentException("File code must be positive for setFileCode");
		fileCodeSet_ = true;
		fileCode_ = fileCode;
    } // setFileCode
   /**
    * Gets the file code for the file being created.
    * @return  the file code
    *
    * @see #setFileCode
    */
      public int getFileCode(){
   		return fileCode_;
    } // getFileCode

	/**
    * Sets the primary extent size for the file being created. The extSize
    * parameter specifies the size of the first extent in pages (2048 byte
    * units).  If this method is not invoked, the primary extent size
    * defaults to 1.
    * Refer to item 199 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param extSize the primary extent size for this Enscribe file
    * @exception java.lang.IllegalArgumentException if the extent size is negative or greater than 512,000,000
    *
    * @see EnscribeFileAttributes#getPrimaryExtentSize
    */
    public void setPrimaryExtentSize(int extSize) throws java.lang.IllegalArgumentException{
		if(extSize < 0 || extSize > 512000000)
			throw new IllegalArgumentException("Extent size must be positive and <= 512,000,000");
		primaryExtentSize_ = extSize;
                primaryExtentSizeSet_ = true;  /* Sol: 10-030701-7498 */
    } // setPrimaryExtentSize
	/**
    * Gets the primary extent size for the file being created.
    * @return the primary extent size
    * @see #setPrimaryExtentSize
    */
    public int getPrimaryExtentSize() {
		return primaryExtentSize_;
    } // getPrimaryExtentSize

	/**
    * Sets the secondary extent size for the file being created. The extSize
    * parameter specifies the size of extents after the first one in pages
    * (2048 byte units).  If this method is not invoked, the secondary
    * extent size defaults to the size of the primary extent.
    * Refer to item 200 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param extSize the secondary extent size for this Enscribe file
    * @exception java.lang.IllegalArgumentException if the extent size is negative or greater than 512,000,000
    *
    * @see EnscribeFileAttributes#getSecondaryExtentSize
    */
    public void setSecondaryExtentSize(int extSize) throws java.lang.IllegalArgumentException{
		if(extSize < 0 || extSize > 512000000)
			throw new IllegalArgumentException("Extent size must be positive and <= 512,000,000");
		secondaryExtentSize_ = extSize;
                secondaryExtentSizeSet_ = true; /* Sol: 10-030701-7498 */
    } // setSecondaryExtentSize
 	/**
     * Gets the secondary extent size for the file being created.
     *
     * @return the secondary extent size
     * @see #setSecondaryExtentSize
     */
     public int getSecondaryExtentSize() {
 		return secondaryExtentSize_;
     } // getSecondaryExtentSize

    /**
     * Sets the file format for the file being created.
     * Format 1 files allow only as many as 4 KB blocks and as many as 2 GB partitions.
     * Format 2 files allow larger blocks and partitions.
     * If this method is not invoked, the Guardian Operating System selects
	 * the format based on other file attributes.
     * Refer to item 195 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
     * @param format value can be 0, 1, or 2. Specifying 0 (the default) indicates that the system selects the format based on the values of other
	 * parameters. Specifying 1 indicates that the file is to be a format 1 file; specifying 2 indicates
	 * that the file is to be a format 2 file.
	 * @exception java.lang.IllegalArgumentException if an invalid value is
	 * supplied for format.
	 * @see #getFormat
	 */
	public void setFormat(int format) throws java.lang.IllegalArgumentException {
		switch(format) {
			case 0:
			case EnscribeFileAttributes.FILEFORMAT_1:
			case EnscribeFileAttributes.FILEFORMAT_2:
				fileFormat_ = format;
				fileFormatSet_ = true;
				return;
			default: throw new IllegalArgumentException("Invalid value specified for format");
		}
	}
	/**
	 * Returns the file format specified for the file being created.
	 * @return int the file format
	 */
	public int getFormat() {
		return fileFormat_;
	}

	/**
    * Sets the maximum number of extents for the file being created.  If this
    * method is not invoked, the maximum number of extents, defaults to 16.  For
    * partitioned files that are not key-sequenced, the only value permitted
    * is 16.
    * Refer to item 52 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    *
    * @param extents the maximum number of extents for this Enscribe file
    * @exception java.lang.IllegalArgumentException if the number of maximum extents is negative
    *
    * @see EnscribeFileAttributes#getMaximumExtents
    * @see #getMaximumExtents
    */
    public void setMaximumExtents(int extents) throws java.lang.IllegalArgumentException{
		if(extents < 0 )
			throw new IllegalArgumentException("Maximum extents must be a positive number");
		maximumExtents_ = extents;
		maximumExtentsSet_ = true;
    } // setMaximumExtents
 	/**
     * Gets the maximum number of extents for the file being created.
     *
     * @return the maximum number of extents
     * @see #setMaximumExtents
     */
     public int getMaximumExtents() {
  		return maximumExtents_;
    } // setMaximumExtents

	/**
    * Specifies whether the file being created should be audited.  If this
    * method is not invoked, the file is NOT audited.
    * Refer to item 66 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param value Specify true if the file should be audited; false otherwise
    * @see #getAudited
    */
    public void setAudited(boolean value) {
		auditedFlag_ = value;
    } // setAudited
 	/**
     * Indicates whether the file being created will be audited.
     * @return true if the file will be audited; false otherwise
     *
     * @see EnscribeFileAttributes#isAudited
     * @see #setAudited
     */
     public boolean getAudited() {
 		return auditedFlag_;
    } // getAudited

	/**
    * Specifies whether the audit data for the file being created should be
    * compressed.  If this method is not invoked, the audit data is
    * NOT compressed.
    * Refer to item 67 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param value Specify true if the audit data should be compressed; false otherwise.
    * @see #getAuditCompression
    */
    public void setAuditCompression(boolean value) {
		auditCompressionFlag_ = value;
    } // setAuditCompressionFlag
 	/**
     * Indicates whether the audit data for the file being created will be
     * compressed.
     * @return boolean true if the audit data will be compressed; false otherwise
     * @see #setAuditCompression
     */
     public boolean getAuditCompression() {
 		return auditCompressionFlag_;
    } // getAuditCompressionFlag

	/**
    * Gets the number of partitions for the file being created.
    *
    * @return int the number of partitions
    * @see #setPartitionDescriptors
    */
    public int getNumberOfPartitions() {
		return numberOfPartitions_;
    } // getNumberOfPartitions

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
		numberOfPartitionsSet_ = true;
		partitionInfo_ = descriptors;
	} // setPartitionInfo
	/**
    * Returns an array of PartitionDescriptor objects.
    *
    * @return array of PartitionDescriptor objects.
    *
    * @exception EnscribeFileException if file has no
    * partitions or if no PartitionDescriptor objects were set.
    * @see #setPartitionDescriptors
    *
    */
    public  PartitionDescriptor [] getPartitionDescriptors() throws EnscribeFileException{
		if(numberOfPartitions_ < 1)
			throw new EnscribeFileException("Cannot getPartitionDescriptors because no partitions exist for this file",
				EnscribeFileException.FEINVALOP,"getPartitionDescriptors","UNDEFINED");
		return partitionInfo_;
    } // getPartitionDescriptors

	/**
    * Sets the expiration time before which the file cannot be purged.
    * Refer to item 57 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param expireTime a GregorianCalendar object, the expiration time in GMT.
    *
    * @see EnscribeFileAttributes#getExpirationTime
    * @see #getExpirationTime
    */
    public void setExpirationTime(GregorianCalendar expireTime) {
		expirationTime_ = expireTime;
		expirationTimeSet_ = true;
    } // setExpirationTime
 	/**
     * Gets the value for expiration time that was previously set by a
     * call to setExpirationTime() or setLike().
     * @return the expiration time.
     * @see #setExpirationTime
     */
     public java.util.GregorianCalendar getExpirationTime() {
  		return expirationTime_;
    } // setExpirationTime

	/**
    * Specifies whether a change to the end-of-file value is to cause the
    * file label to be written immediately to disk.  If this method is not
    * invoked,the file label is NOT immediately written to disk.
    * Refer to item 70 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param value If true, a change to the end-of-file value will cause
    * the file label to be written immediately to disk. If false,
    * a change to the end-of-file value will not clause the file label to
    * be written immediately to disk - the write to disk will occur at
    * a system-determined time.
    *
    * @see #getRefreshEOF
    */
    public void setRefreshEOF(boolean value) {
		refreshEofFlag_ = value;
    } // setRefreshEofFlag
	/**
    * Gets the value for Refresh EOF that was previously specified by a call
    * to setRefreshEOF or setLike().
    * @return true if a change to the end-of-file value causes the file
    * label to be immediately written to disk; false if otherwise.
    * @see #setRefreshEOF
    */
    public boolean getRefreshEOF() {
		return refreshEofFlag_;
    } // setRefreshEofFlag

	/**
    * Specifies whether writes through cache are to be done for the
    * file being created.  If this method is not invoked, writes through
    * cache are performed for unaudited files. Buffering is performed for
    * audited files.
    * Refer to item 72 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param true to request writes through cache; false if otherwise. If set to false, writes
    * are buffered for audited files but writes through cache are
    * performed for unaudited files.
    *
    * @see #getWriteThroughCache
    */
    public void setWriteThroughCache(boolean value) {
		writeThroughFlag_ = value;
    } // setWriteThroughFlag
	/**
    * Gets the value for writes through cache that was previously set by a call
    * to setWriteThroughCache or setLike().
    * @return true if writes through cache are to be done; false otherwise.
    *
    * @see #setWriteThroughCache
    */
    public boolean getWriteThroughCache() {
		return writeThroughFlag_;
    } // setWriteThroughFlag
	/**
    * Specifies whether writes to the file being created should be read back
    * and the data verified.  If this method is not invoked, data is NOT
    * verified.
    * Refer to item 73 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param value Specify true if the data should be verified; false otherwise.
    *
    * @see #getVerifyWrites
    */
    public void setVerifyWrites(boolean value) {
		verifyWritesFlag_ = value;
    } // setVerifyWritesFlag
	/**
    * Gets the value for verify writes that was previously specified by a call
    * to setVerifyWrites() or setLike().
    * @return true if writes to the file are verified; false otherwise
    * @see #setVerifyWrites
    */
    public boolean getVerifyWrites() {
		return verifyWritesFlag_;
    } // getVerifyWritesFlag
	/**
    * Specifies whether writes are to be made serially to the mirror if
    * the file to be created resides on a mirrored disk volume.  If this
    * method is not invoked, the Guardian Operating System chooses whether
    * to do serial or parallel writes.
    * Refer to item 74 under FILE_CREATELIST_ in the Guardian Procedure Calls Manual.
    * @param value Specify true if writes are to be made serially; false to let the system decide when to write to the mirror.
    * @see #getSerialWrites
    */
    public void setSerialWrites(boolean value) {
		serialWritesFlag_ = value;
    } // setSerialWrites

	/**
    * Gets the value for serial writes that was specified by a previous call to
    * setSerialWrites() or setLike().
    * @return true if writes are to be done serially to the mirror; false if the system is to
    * determine when writes to the mirror are done.
    * @see #setSerialWrites
    */
    public boolean getSerialWrites() {
		return serialWritesFlag_;
    } // clearSerialWritesFlag

	// --------------------------------------------
	// --------------------------------------------
	/*
	 * Internal method
	 */
	 String getAttributeString(boolean getPartitionInfo)
	{
		StringBuffer str = new StringBuffer(300);
		str.append("").append(OV_FILE_TYPE).append(",").append(fileType_).append(",");

		if(fileCodeSet_)
			str.append(OV_FILE_CODE).append(",").append(fileCode_).append(",");

		if(primaryExtentSizeSet_)
			str.append(OV_PRIMARY_EXTENT_SIZE).append(",").append(primaryExtentSize_).append(",");
		if(secondaryExtentSizeSet_)
			str.append(OV_SECONDARY_EXTENT_SIZE).append(",").append(secondaryExtentSize_).append(",");
		if(maximumExtentsSet_)
			str.append(OV_MAXIMUM_EXTENTS).append(",").append(maximumExtents_).append(",");

		if(expirationTimeSet_ && expirationTime_.get(Calendar.YEAR) > -1) {
			str.append(OV_EXPIRATION_TIME_GMT).append(",").append(expirationTime_.get(Calendar.YEAR)).append(",");
						   str.append(expirationTime_.get(Calendar.MONTH)).append(",");
						   str.append(expirationTime_.get(Calendar.DATE)).append(",");
						   str.append(expirationTime_.get(Calendar.HOUR_OF_DAY)).append(",");
						   str.append(expirationTime_.get(Calendar.MINUTE)).append(",");
						   str.append(expirationTime_.get(Calendar.SECOND)).append(",");
		}

		if(auditedFlag_)
			str.append(OV_AUDITED_FLAG).append(",1,");
		if(auditCompressionFlag_)
			str.append(OV_AUDIT_COMPRESSION_FLAG).append(",1,");

		if(refreshEofFlag_)
			str.append(OV_REFRESH_EOF_FLAG).append(",1,");
		if(writeThroughFlag_)
			str.append(OV_WRITE_THROUGH_FLAG).append(",1,");
		if(verifyWritesFlag_)
			str.append(OV_VERIFY_WRITES_FLAG).append(",1,");
		if(serialWritesFlag_)
			str.append(OV_SERIAL_WRITES_FLAG).append(",1,");

		if(fileFormatSet_)
			str.append(OV_FILE_FORMAT).append(",").append(fileFormat_).append(",");


		if(getPartitionInfo) {
			if(numberOfPartitions_ > 0) {
				str.append(OV_NUMBER_OF_PARTITIONS).append(",").append(numberOfPartitions_).append(",");
				str.append(OV_PARTITION_EXTENTS).append(",");
				PartitionDescriptor pd;
				for(int i=0;i<numberOfPartitions_;i++) {
					pd = partitionInfo_[i];
					str.append(pd.getPrimaryExtentSize()).append(",");
					str.append(pd.getSecondaryExtentSize()).append(",");
				}
				str.append(OV_PARTITION_VOLUME_NAME_LENS).append(",");
				for(int i=0;i<numberOfPartitions_;i++) {
					pd = partitionInfo_[i];
					str.append(pd.getVolumeName().length()).append(",");
				}
				str.append(OV_PARTITION_VOLUME_NAMES).append(",");
				for(int i=0;i<numberOfPartitions_;i++) {
					pd = partitionInfo_[i];
					str.append(pd.getVolumeName()).append(",");
				}
			}
		}

		return str.toString().trim();
	} // getAttributeString

} // EnscribeCreationAttributes
