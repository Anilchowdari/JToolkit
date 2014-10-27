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
import com.tandem.ext.enscribe.EnscribeFile;
import java.util.*;
import com.tandem.ext.util.NativeLoader;


/**
 * <P>EnscribeFileAttributes provides access to the more static information (attributes)
 * about any Enscribe file.  This information is obtained by a call to the
 * Guardian procedure FILE_GETINFOLISTBYNAME_.  There is no requirement that the file
 * be open to use EnscribeFileAttributes.
 * </P>
 *
 * <P>EnscribeFileAttributes also provides methods to change attributes when you want to alter the
 * characteristics of an existing Enscribe file. These methods are comparable to
 * the parameters of FILE_ALTERLIST_.</P>
 *
 *
 * <P>An EnscribeFileAttributes object is returned from the EnscribeFile.getFileInfo()
 * method.  Alternatively, if you want to determine the attributes of an
 * Enscribe file that is not represented by an EnscribeFile object, you can
 * create an EnscribeFileAttribute object as follows:
 </P>
 *
 *<PRE>
 *  try {
 * 		EnscribeFileAttributes myfileInfo =
 *			new EnscribeFileAttributes("/G/data/myvol/myfile");
 *		int fileType = myfileInfo.getFileType();
 *		System.out.println("The file type is "+fileType);
 *
 *  } catch (com.tandem.ext.enscribe.EnscribeFileException ex) {
 *      System.out.println(ex.getMessage());
 *  }
 *</PRE>
 * See the FILE_GETINFOLIST_ and FILE_ALTERLIST_ procedures in the
 * Guardian Procedure Calls Reference Manual.
 * @see EnscribeFile#getFileInfo
 * @see EnscribeCreationAttributes
 */

public class EnscribeFileAttributes
{
	static {	  
		NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();		
	 }
	/**
	 * The file type for an unstructured file.
	 */
	public static final int FILETYPE_UNSTRUCTURED = 0;
	/**
	 * The file type for a relative file
	 */
	public static final int FILETYPE_RELATIVE = 1;
	/**
	 * The file type for an entry-sequenced file.
	 */
	public static final int FILETYPE_ENTRY_SEQUENCED = 2;
	/**
	 * The file type for a key-sequenced file.
	 */
	public static final int FILETYPE_KEY_SEQUENCED = 3;

	/**
	 * The file format for a type 1 file
	 */
	 public static final int FILEFORMAT_1 = 1;
	/**
	 * The file format for a type 2 file
	 */
	 public static final int FILEFORMAT_2 = 2;


	// for open file and by filename
	 int physicalRecordLength_ = 0;
	 int fileType_ = -1;
	 int fileCode_ = 0;
	 int primaryExtentSize_ = 0;
	 int secondaryExtentSize_ = 0;
	 int maximumExtents_ = 0;
	 int allocatedExtents_ = 0;
	 int fileGuardianOwnerUserid_ = 0;
	 int fileGuardianOwnerGroupid_ = 0;
	 boolean safeguardProtected_ = false;
	 boolean ClearOnPurgeFlag_ = false;
	 String guardianSecurityString_ = "";
	 boolean auditedFlag_ = false;
	 boolean auditCompressionFlag_ = false;
	 boolean refreshEofFlag_ = false;
	 boolean writeThroughFlag_ = false;
	 boolean verifyWritesFlag_ = false;
	 boolean serialWritesFlag_ = false;
	 boolean fileIsOpenFlag_ = false;
	 boolean crashOpenFlag_ = false;
	 boolean rollforwardNeededFlag_ = false;
	 boolean brokenFlag_ = false;
	 boolean corruptFlag_ = false;
	 boolean secondaryPartitionFlag_ = false;
	 int numberOfPartitions_ = 0;
	 boolean sequentialBlockBufferingFlag_ = false;
	 GregorianCalendar lastOpenTime_;
	 GregorianCalendar expirationTime_;
	 GregorianCalendar creationTime_;
	 GregorianCalendar modificationTime_;
	 boolean isossFile_ = false;
	 int ossOwnerGroupId_ = 0;
	 int ossAccessPermissions_ = 0;
	 int ossOwner_ = 0;
	 int fileFormat_ = 1;

         long aggregateEOF_ = 0;       /*Sol: 10-030703-7581 */
         long aggregateMaxFileSize_ = 0;  /*Sol: 10-030703-7581 */  

	// structured file attributes
	 int keyInError_ = 0; // open file only
	 int currentKeySpecifier_ = 0; // open file only
	 int currentKeyLength_ = 0; // open file only
	 Vector currentKeyValue_ = new Vector(32,32); // open file only
	 int logicalRecordLength_ = 0;
	 int blockLength_ = 0;
	 int numberOfAltkeys_ = 0;
	 int numberOfAltkeyfiles_ = 0;
	 int [] altKeyArray_;

	// key sequenced attributes
	 int currentPrimaryKeyLength_ = 0; // open file only
	 Vector currentPrimaryKeyValue_ = new Vector(32,32); // open file only
	 int keyOffset_ = 0;
	 int keyLength_ = 0;
	 int lockKeyLength_ = 0;
	 boolean dataCompressionFlag_ = false;
	 boolean indexCompressFlag_ = false;
	 int indexLevels_ = 0;
	 int partitionPartialKeyLength_ = 0;

	// unstructured attributes
	 boolean progidFlag_ = false;
	 boolean licensedFileFlag_ = false;
	 boolean oddUnstructuredFlag_ = false;

	 PartitionDescriptor [] partitionInfo_;
	 AlternateKeyDescriptor [] altKeyInfo_;
	 String [] altKeyFileInfo_;

	// updated Values
	 boolean updatedAuditedFlag_ = false;
	 boolean updatedAuditedFlagSet_ = false;
	 int updatedFileCode_ = 0;
	 boolean updatedFileCodeSet_ = false;
	 boolean updatedRefreshEOFFlag_ = false;
	 boolean updatedRefreshEOFFlagSet_ = false;
	 boolean updatedBrokenFlag_ = false;
	 boolean updatedBrokenFlagSet_ = false;
	 boolean updatedSecondaryPartitionFlag_ = false;
	 boolean updatedSecondaryPartitionFlagIsSet_ = false;
	 boolean updatedOddunstructuredFlag_ = false;
	 boolean updatedOddunstructuredFlagIsSet_ = false;
	 GregorianCalendar updatedExpirationTime_;
	 boolean updatedExpirationTimeSet_ = false;
	 AlternateKeyAlterAttributes updatedAltKeyAlterAttr_ = null;
	 String[] updatedFileNames_;
	 PartitionDescriptor [] updatedPartitionAlterAttr_;
	 int updatedNumberOfPartitions_ = 0;

	// internal use
	private boolean usingFileName_ = true;
	private boolean ossFormatFileName_ = false;
	private short fileNumber_ = -1;
	private String fileName_ = "";

	private boolean detailInfoRetrieved_ = false;
	private boolean currentInfoRetrieved_ = false;

	private boolean forFileCreate_ = false;

	// ---------- flags for encoding and decoding -------
	// keep in sync with enscribejni.h

        /* Sol: 10-030703-7581 Added OV_AGGREGATE_EOF and 
                OV_AGGREGATE_MAX_FILE_SIZE */
        static final int    OV_AGGREGATE_EOF = 191;
        static final int    OV_AGGREGATE_MAX_FILE_SIZE = 192; 

	static final int    OV_ERROR_DETAIL = 8;
	static final int    OV_PARTITION_IN_ERROR = 9;
	static final int    OV_KEY_IN_ERROR = 10;
	static final int    OV_NEXT_RECORD_POINTER = 201;
	static final int    OV_CURRENT_RECORD_POINTER = 202;
	static final int    OV_CURRENT_KEY_SPECIFIER = 13;
	static final int    OV_CURRENT_KEY_LENGTH = 203;
	static final int    OV_CURRENT_KEY_VALUE = 204;
	static final int    OV_CURRENT_PRIMARY_KEY_LENGTH = 205;
	static final int    OV_CURRENT_PRIMARY_KEY_VALUE = 206;
	static final int    OV_OPEN_ACCESS_MODE = 21;
	static final int    OV_OPEN_EXCLUSION_MODE = 22;
	static final int    OV_PHYSICAL_RECORD_LENGTH = 34;
	static final int    OV_FILE_TYPE = 41;
	static final int    OV_FILE_CODE = 42;
	static final int    OV_LOGICAL_RECORD_LENGTH = 196;
	static final int    OV_BLOCK_LENGTH = 197;
	static final int    OV_KEY_OFFSET = 198;
	static final int    OV_KEY_LENGTH = 46;
	static final int    OV_LOCK_KEY_LENGTH = 47;
	static final int    OV_PRIMARY_EXTENT_SIZE = 199;
	static final int    OV_SECONDARY_EXTENT_SIZE = 200;
	static final int    OV_MAXIMUM_EXTENTS = 52;
	static final int    OV_ALLOCATED_EXTENTS = 53;
	static final int    OV_GUARDIAN_FILE_OWNER = 58;
	static final int    OV_SAFEGUARD_PROTECTED = 59;
	static final int    OV_PROGID_FLAG = 60;
	static final int    OV_CLEAR_ON_PURGE_FLAG = 61;
	static final int    OV_SECURITY_STRING = 62;
	static final int    OV_LICENSED_FILE_FLAG = 63;
	static final int    OV_ODD_UNSTRUCTURED_FLAG = 65;
	static final int    OV_AUDITED_FLAG = 66;
	static final int    OV_AUDIT_COMPRESSION_FLAG = 67;
	static final int    OV_DATA_COMPRESSION_FLAG = 68;
	static final int    OV_INDEX_COMPRESS_FLAG = 69;
	static final int    OV_REFRESH_EOF_FLAG = 70;
	static final int    OV_WRITE_THROUGH_FLAG = 72;
	static final int    OV_VERIFY_WRITES_FLAG = 73;
	static final int    OV_SERIAL_WRITES_FLAG = 74;
	static final int    OV_FILE_IS_OPEN_FLAG = 75;
	static final int    OV_CRASH_OPEN_FLAG = 76;
	static final int    OV_ROLLFORWARD_NEEDED_FLAG = 77;
	static final int    OV_BROKEN_FLAG = 78;
	static final int    OV_CORRUPT_FLAG = 79;
	static final int    OV_SECONDARY_PARTITION_FLAG = 80;
	static final int    OV_INDEX_LEVELS = 81;
	static final int    OV_NUMBER_OF_PARTITIONS = 90;
	static final int    OV_PARTITION_EXTENTS = 97; /*Sol: 10-030723-8110 */ 
	static final int    OV_PARTITION_VOLUME_NAME_LENS = 92;
	static final int    OV_PARTITION_VOLUME_NAMES = 93;
	static final int    OV_PARTITION_PARTIAL_KEY_LENGTH = 94;
	static final int    OV_PARTITION_PARTIAL_KEY_VALS = 95;
	static final int    OV_NUMBER_OF_ALT_KEYS = 100;
	static final int    OV_ALT_KEY_DESCRIPTORS = 101;
	static final int    OV_NUMBER_OF_ALT_KEY_FILES = 102;
	static final int    OV_ALT_KEY_FILE_NAME_LENS = 103;
	static final int    OV_ALT_KEY_FILE_NAMES = 104;
	static final int    OV_SEQUENTIAL_BLOCK_BUFFERING_FLAG = 116;
	static final int    OV_OSS_OWNER_GROUP_ID = 164;
	static final int    OV_OSS_ACCESS_PERMISSIONS = 165;
	static final int    OV_OSS_OWNER = 167;
	static final int    OV_IS_OSS_FILE = 9998;
	static final int    OV_LAST_OPTVAL = 9999;

	static final int    OV_EXPIRATION_TIME_GMT = 57;

	static final int    OV_LAST_OPEN_TIME = 117;
	static final int    OV_EXPIRATION_TIME = 118;
	static final int    OV_CREATION_TIME = 119;
	static final int    OV_MODIFICATION_TIME = 145;
	static final int	OV_FILE_FORMAT = 195;

	private final short  ERR_NOT_DISC_FILE = 3001;
	private final short  DEVICETYPE_DISC = 3;

   /**
    * Construct an EnscribeFileAttributes object for query by filename.
    *
    * @param fileName a String with the physical name of the file in Guardian or OSS format
    * @param ossFormat a boolean, true if fileName is in OSS format
    * @exception java.lang.IllegalArgumentException if filename is not
    * valid
	* @exception com.tandem.ext.enscribe.EnscribeFileException if an error occurs trying to
	* obtain the file attributes.
    */
    public EnscribeFileAttributes(String fileName) throws EnscribeFileException {
		if(fileName == null)
			throw new IllegalArgumentException("fileName cannot be null");
        if(!EnscribeFile.isValidFileName(fileName))
			throw new EnscribeFileException("Invalid file name.",
				EnscribeFileException.FEBADFORM,"EnscribeFileAttributes.constructor",fileName_);
		usingFileName_ = true;
		if(fileName.startsWith("/"))
			ossFormatFileName_ = true;
		else
			ossFormatFileName_ = false;
		fileName_ = fileName;
		this.getStoredFileAttributes();
    } // EnscribeFileAttr


	/**
    * Retrieves details from the disk file label about this Enscribe file.
    * File does not have to be open as the filename is used to access the
    * information.
    *
    * @see #getCurrentOpenFileAttributes
    *
    * @exception com.tandem.enscribe.EnscribeFileException if an error occurs getting file
    * details.
    */
    void getStoredFileAttributes() throws EnscribeFileException{
		short []
			errorArray = new short [2];

		String
			str = GgetFileInfoDetails(fileNumber_, fileName_, usingFileName_,
									  ossFormatFileName_,
									  errorArray);
		if(errorArray[0] == ERR_NOT_DISC_FILE)
			throw new EnscribeFileException("File is not a disc file, cannot get file attributes",
				EnscribeFileException.FEINVALOP,"getStoredFileAttributes",fileName_);
		if(errorArray[0] != 0)
			throw new EnscribeFileException("File error trying to get file attributes",
				errorArray[0],"getStoredFileAttributes",fileName_);
		try
		{
			 setStoredFileAttributesFromString(str);
		}
		catch (Exception ex) {
			throw new EnscribeFileException("Unable to parse file attributes string. Internal error: " + ex.getMessage(),
				EnscribeFileException.FEBADCOUNT,"getStoredFileAttributes",fileName_);
		}
		detailInfoRetrieved_ = true;
    } // getStoredFileAttributes

	/**
    * Returns the physical record length of this file.
    * Corresponds to item 34 of FILE_GETINFOLIST_.
    *
    * @return physical record length
    */
    public int getPhysicalRecordLength() {
        return(physicalRecordLength_);
    } // getPhysicalRecordLength

	/**
	 * Returns the name of the file associated with this object.
	 * Corresponds to item 2 of FILE_GETINFOLIST_.
	 *
	 * @return the file name.
	 */
	public String getFileName() {
		return fileName_;
	}
	/**
    * Returns the file type.
    * Corresponds to item 41 of FILE_GETINFOLIST_.
    *
    * @return file type
    */
    public int getFileType() {
        return(fileType_);
    } // getFileType

	/**
    * Returns the file code.
    * Corresponds to item 42 of FILE_GETINFOLIST_.
    *
    * @return file code
    * @see #updateFileCode
    */
    public int getFileCode() {
        return(fileCode_);
    } // getFileCode
    /**
     * Returns the file format - Format 1 (FILEFORMAT_1) allows
     * only as many as 4KB blocks and as many as 2GB partitions while
     * Format 2 (FILEFORMAT_2) allows larger blocks and partitions.
     * Corresponds to item 195 of FILE_GETINFOLIST_
     * @return either FILEFORMAT_1 or FILEFORMAT_2
     * @see #FILEFORMAT_1
     * @see #FILEFORMAT_2
     */
    public int getFormat() {
		return(fileFormat_);
	} //getFileFormat

    /**
	 * Specifies a new value for the file code attribute.  This value will be
	 * used if this EnscribeFileAttribute object is passed as a parameter to
	 * the EnscribeFile.alter() method.
	 * Corresponds to item 42 of FILE_ALTERLIST_.
	 *
	 * @param fileCode the new file code
	 * @exception java.lang.IllegalArgumentException if the file code
	 * is negative
	 *
	 * @see #getFileCode
	 */
	public void updateFileCode(int fileCode) throws java.lang.IllegalArgumentException {
		if(fileCode < 0)
				throw new IllegalArgumentException("File code must be positive");
		updatedFileCode_ = fileCode;
		updatedFileCodeSet_ = true;
	} // setFileCode

	/**
    * Returns the block length of the file.
    * Corresponds to item 197 of FILE_GETINFOLIST_.
    *
    * @return block length
    *
    *
    */
    public int getBlockLength() {
        return(blockLength_);
    } // getBlockLength

	/**
    * Returns the primary extent size of this file.
    * Corresponds to item 50 of FILE_GETINFOLIST_.
    *
    * @return the primary extent size
    */
    public int getPrimaryExtentSize() {
        return(primaryExtentSize_);
    } // getPrimaryExtentSize

	/**
    * Returns the secondary extent size of this file.
    * Corresponds to item 51 of FILE_GETINFOLIST_.
    *
    * @return the secondary extent size
    *
    */
    public int getSecondaryExtentSize() {
        return(secondaryExtentSize_);
    } // getSecondaryExtentSize

	/**
    * Returns the maximum number of extents of this file.
    * Corresponds to item 52 of FILE_GETINFOLIST_.
    *
    * @return the maximum number of extents
    */
    public int getMaximumExtents() {
        return(maximumExtents_);
    } // getSecondaryExtentSize

/*
   Sol: 10-030703-7581 functions getAggregateEOF and getAggregateFileSize
        are added.
*/

/* Begin Changes : 10-030703-7581 */

        /**
    * Returns the aggregate EOF of this file.
    * Corresponds to item 191 of FILE_GETINFOLIST_.
    *
    * @return the aggregate EOF
    */
    public long getAggregateEOF() {
        return(aggregateEOF_);
    } // getAggregateEOF

        /**
    * Returns the aggregate maximum file size of this file.
    * Corresponds to item 192 of FILE_GETINFOLIST_.
    *
    * @return the aggregate maximum file size
    */
    public long getAggregateMaximumFileSize() {
        return(aggregateMaxFileSize_);
    } // getAggregateMaximumFileSize_

/* End Changes : 10-030703-7581 */                               
                                             


	/**
    * Returns the allocated number of extents of this file.
    * Corresponds to item 53 of FILE_GETINFOLIST_.
    *
    * @return allocated number of extents
    */
    public int getAllocatedExtents() {
        return(allocatedExtents_);
    } // getAllocatedExtents

	/**
    * Returns the userid of the file owner of this Guardian Enscribe file.
    * Corresponds to item 58 of FILE_GETINFOLIST_.
    *
    * @return the userid of the file owner
    */
    public int getGuardianFileOwnerUserid() {
		return(fileGuardianOwnerUserid_);
    } // getGuardianFileOwnerUserid

	/**
    * Returns the group id of the file owner of this Guardian Enscribe file.
    *
    * @return group id of the file owner
    */
    public int getGuardianFileOwnerGroupid() {
		return(fileGuardianOwnerGroupid_);
    } // getGuardianFileOwnerGroupid

	/**
    * Indicates whether the file is protected by Safeguard(tm).
	* Corresponds to item 59 of FILE_GETINFOLIST_.
    *
    * @return true if the file is protected by Safeguard(tm).
    */
    public boolean isSafeguardProtected() {
		return(safeguardProtected_);
    } // isSafeguardProtected

	/**
    * Indicates whether the area of disk occupied by the file will
	* be erased (overwritten with zeros) if the file is purged.
	* Corresponds to item 61 of FILE_GETINFOLIST_.
    *
    * @return true if the area of disk will be erased.
    */
    public boolean isClearedOnPurge() {
		return(ClearOnPurgeFlag_);
    } // clearedOnPurge

	/**
	 * Returns the Guardian security string (4 bytes).
	 * <DL>
	 *	<DT> byte 0 =  read permission
	 *	<DT> byte 1 =  write permission
	 *	<DT> byte 2 =  execute permission
	 *	<DT> byte 3 =  purge permission
	 * </DL>
	 *
	 * @see EnscribeFile#FILE_SECURITY_A
	 * @see EnscribeFile#FILE_SECURITY_G
	 * @see EnscribeFile#FILE_SECURITY_N
	 * @see EnscribeFile#FILE_SECURITY_O
	 * @see EnscribeFile#FILE_SECURITY_C
	 * @see EnscribeFile#FILE_SECURITY_U
	 * @see EnscribeFile#FILE_SECURITY_SUPER
	 *
	 * Corresponds to item 62 of FILE_GETINFOLIST_.

     * @return the 4 byte Guardian security string
     *
     */
    public String getGuardianSecurityString() {
		return(guardianSecurityString_);
    } // getGuardianSecurityString

	/**
    * Indicates whether the file is audited by TMF.
    * Corresponds to item 66 of FILE_GETINFOLIST_.
    *
    * @return true if the file associated with this EnscribeFileAttributes
    * object is audited.
    * @see #updateAudited
    */
    public boolean isAudited() {
		return(auditedFlag_);
    } // getAuditedFlag

	/**
	 * Specifies a new value for the audited attribute. This value is used
	 * if this EnscribeFileAttributes object is passed to the
	 * EnscribeFile.alter() method.
	 * Corresponds to item 66 of FILE_ALTERLIST_.
	 *
	 * @param value true to specify that the file is to be audited, false if not
	 *
	 * @see #isAudited
	 */
	public void updateAudited(boolean value) {
		updatedAuditedFlag_ = value;
		updatedAuditedFlagSet_ = true;
	} // setAuditedFlag

	/**
	 * Indicates whether the file type is key-sequenced.
	 *
	 * @return true if file is a key-sequenced file; false otherwise.
	 * @see #getFileType
	 */
	public boolean isKeySequenced() {
		if(fileType_ == FILETYPE_KEY_SEQUENCED)
			return true;
		else
			return false;
	}
	/**
	 * Indicates whether the file type is relative.
	 *
	 * @return true if file is a relative file; false otherwise.
	 * @see #getFileType
	 */
	public boolean isRelative() {
		if(fileType_ == FILETYPE_RELATIVE)
			return true;
		else
			return false;
	}
	/**
	 * Indicates whether the file type is entry-sequenced.
	 *
	 * @return true if file is an entry-sequenced file; false otherwise.
	 * @see #getFileType
	 */
	public boolean isEntrySequenced() {
		if(fileType_ == FILETYPE_ENTRY_SEQUENCED)
			return true;
		else
			return false;
	}
	/**
	 * Indicates whether the file type is unstructured.
	 *
	 * @return true if file is an unstructured file; false otherwise.
	 * @see #getFileType
	 */
	public boolean isUnstructured() {
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			return true;
		else
			return false;
	}

	/**
    * Indicates whether audit data for this file is compressed.
    * Corresponds to item 67 of FILE_GETINFOLIST_.
    *
    * @return true if the audit data is compressed; false if not.
    */
    public boolean isAuditDataCompressed() {
		return(auditCompressionFlag_);
    } // isAuditDataCompressed

	/**
    * Indicates whether a change in the EOF value causes the file label
    * to be immediately written to disk.
    * Corresponds to item 70 of FILE_GETINFOLIST_.
    *
    * @return true if a change in the EOF value cause the file label to
    * be written.
    * @see #updateRefreshEOF
    */
    public boolean isEOFRefreshed() {
		return(refreshEofFlag_);
    } // refreshEOF

	/**
    * Specifies a new value for the refreshEOF attribute.  This value will be
    * used when this EnscribeFileAttributes object is passed as a
    * parameter to the EnscribeFile.alter() method.
    * Corresponds to item 70 of FILE_ALTERLIST_.
    *
    * @param value true to specify that a change in the EOF value of the file should
    * cause the file label to be immediately written to disk; false if otherwise.
    * @see #isEOFRefreshed
    */
    public void updateRefreshEOF(boolean value) {
		updatedRefreshEOFFlag_ = value;
		updatedRefreshEOFFlagSet_ = true;
    } // refreshEOF
	/**
    * Indicates whether writes through cache are performed.
    * Corresponds to item 72 of FILE_GETINFOLIST_.
    *
    * @return true if writes through cache are performed; false if buffered
    * writes are used.
    */
    public boolean areWritesThroughCache() {
		return(writeThroughFlag_);
    } // writeAreThroughCache

	/**
     * Indicates whether writes to the file are read back
     * and the data verified.
     * Corresponds to item 73 of FILE_GETINFOLIST_.
     *
     * @return true if writes are verified; false otherwise.
     *
     */
    public boolean areWritesVerified() {
		return(verifyWritesFlag_);
    } // writesAreVerified
	/**
    * Indicates if writes are made serially to the mirror if
    * the file resides on a mirrored disk volume.
    * Corresponds to item 74 of FILE_GETINFOLIST_.
    *
    * @return true if writes are made serially
    *
    */
    public boolean areWritesSerial() {
		return(serialWritesFlag_);
    } // writesAreSerial

	/**
    * Indicates if the file is open.
    * Corresponds to item 75 of FILE_GETINFOLIST_.
    *
    * @return true if the file is open
    */
    public boolean isOpen() {
		return(fileIsOpenFlag_);
    } // isOpen

	/**
    * Indicates whether the file was opened for write access when a
    * system failure occurred and that the file has not been opened since
    * the system failure.
    * Corresponds to item 76 of FILE_GETINFOLIST_.
    *
    * @return true if the file is crash open
    *
    */
    public boolean isCrashOpen() {
		return(crashOpenFlag_);
    } // isCrashOpen

	/**
    * Indicates whether the file needs to be rolled forward by TMF.
    * Corresponds to item 77 of FILE_GETINFOLIST_.
    *
    * @return true if a roll forward is needed
    */
    public boolean isRollForwardNeeded() {
		return(rollforwardNeededFlag_);
    } // isRollforwardNeeded

	/**
    * Indicates whether the file is marked broken.
    * Corresponds to item 78 of FILE_GETINFOLIST_.
    *
    * @return true if the file is marked broken
    * @see #updateBroken
    */
    public boolean isBroken() {
		return(brokenFlag_);
    } // isBroken
    /**
	 * Specifies a new value for the broken attribute.  This value will
	 * be used if this object is used as a parameter to the
	 * EnscribeFile.alter() method.
	 * Corresponds to item 78 of FILE_ALTERLIST_.
	 *
	 * @param value true if the file is broken; false if otherwise
	 * @see #isBroken
	 */
	public void updateBroken(boolean value) {
		updatedBrokenFlag_ = value;
		updatedBrokenFlagSet_ = true;
    } // updateBroken

	/**
    * Indicates whether the file is corrupt.
    * Corresponds to item 79 of FILE_GETINFOLIST_.
    *
    * @return true if the file is corrupt
    */
    public boolean isCorrupt() {
		return(corruptFlag_);
    } // isCorrupt

	/**
    * Indicates whether the file is a secondary partition.
    * Corresponds to item 80 of FILE_GETINFOLIST_.
    *
    * @return true if the file is a secondary partition
    * @see #updateSecondaryPartition
    */
    public boolean isSecondaryPartition() {
		return(secondaryPartitionFlag_);
    } // isSecondaryPartition
	/**
    * Specifies a new value for the attribute that indicates
    * whether this file is a secondary partition.  This value
    * is used if this EnscribeFileAttributes object is passed
    * as a parameter to the EnscribeFile.alter() method.
    * Corresponds to item 80 of FILE_ALTERLIST_.
    *
    * @param value true if the file is a secondary partition; false
    * otherwise
    * @see #isSecondaryPartition
    */
    public void updateSecondaryPartition(boolean value) {
		updatedSecondaryPartitionFlag_ = value;
		updatedSecondaryPartitionFlagIsSet_ = true;
    } // isSecondaryPartition

	/**
    * Returns the number of partitions for this file.
    * Corresponds to item 90 of FILE_GETINFOLIST_.
    *
    * @return the number of partitions
    */
    public int getNumberOfPartitions() {
        return(numberOfPartitions_);
    } // getNumberOfPartitions

	/**
    * Indicates if sequential block buffering is being used for this file.
    * Corresponds to item 116 of FILE_GETINFOLIST_
    *
    * @return true if sequential block buffering is being used
    */
    public boolean isBlockBufferingSequential() {
		return(sequentialBlockBufferingFlag_);
    } // usingSequentialBlockBuffering

	/**
    * Returns the last time the file was opened.  
    * Corresponds to item 56 of FILE_GETINFOLIST_.
    *
    * @return a GregorianCalendar object, containing the last open time; returns null if
	* file has never been opened.
    */
    public GregorianCalendar getLastOpenTime() {
		return(lastOpenTime_);
    } // getLastOpenTime

	/**
    * Returns the expiration time (the time before which the file cannot
    * be purged).  
    * Corresponds to item 57 of FILE_GETINFOLIST_.
    *
    * @return a GregorianCalendar object, containing the expiration time; returns
	* null if the expiration time has not been set.
    * @see #updateExpirationTime
    */
    public GregorianCalendar getExpirationTime() {
		return(expirationTime_);
    } // getExpirationTime
    /**
	 * Changes the expiration time (the time before which the file cannot
	 * be purged).  This value will be used if this EnscribeFileAttributes
	 * object is passed to EnscribeFile.alter().
	 * Corresponds to item 57 of FILE_ALTERLIST_.
	 *
	 * @param GregorianCalendar object, containing the new expiration time
	 * @see #getExpirationTime
	 */
	 public void updateExpirationTime(GregorianCalendar value) {
		updatedExpirationTime_ = value;
		updatedExpirationTimeSet_ = true;
	 } // getExpirationTime


	/**
    * Returns the time the file was created.
    * Corresponds to item 54 of FILE_GETINFOLIST_.
    *
    * @return  a GregorianCalendar object containing the creation time
    */
    public GregorianCalendar getCreationTime() {
		return(creationTime_);
    } // getCreationTime

	/**
    * Returns the last time the file was modified.
    *
    * @return GregorianCalendar object, file modification time
    */
    public GregorianCalendar getModificationTime() {
		return(modificationTime_);
    } // getModificationTime

	/**
    * Returns true if the file is an OSS file.
    * Corresponds to item 161 of FILE_GETINFOLIST_
    *
    * @return true if the file is an OSS file
    */
    public boolean isOssFile() {
		return(isossFile_);
    } // isossFile

	/**
    * Returns the file owner group id of this OSS file.
    * Corresponds to item 164 of FILE_GETINFOLIST_
    *
    * @return group id
    * @exception EnscribeFileException if the file is not an oss file
    */
    public int getOssOwnerGroupId() throws EnscribeFileException{
        if(!isossFile_)
			throw new EnscribeFileException("Cannot get oss owner group Id because the file is a Guardian file",
				EnscribeFileException.FEINVALOP,"getOssOwnerGroupId",fileName_);
		return(ossOwnerGroupId_);
    } // getOssOwnerGroupId

	/**
    * Returns the file access permissions of this OSS file.
    * Corresponds to item 165 of FILE_GETINFOLIST_
    *
    * @return file access permissions
    *
    * @exception EnscribeFileException if
    * it is a Guardian file.
    */
    public int getOssAccessPermissions() throws EnscribeFileException{
        if(!isossFile_)
			throw new EnscribeFileException("Cannot get oss access permissions because the file is a Guardian file",
				EnscribeFileException.FEINVALOP,"getOssAccessPermissions",fileName_);
		return(ossAccessPermissions_);
    } // getOssAccessPermissions

	/**
    * Returns the file owner of this OSS file.
    * Corresponds to item 167 of FILE_GETINFOLIST_
    *
    * @return file owner
    *
    * @exception EnscribeFileException if
    * it is a Guardian file.
    */
    public int getOssOwner()throws EnscribeFileException {
        if(!isossFile_)
			throw new EnscribeFileException("Cannot get the oss owner because the file is not an oss file",
				EnscribeFileException.FEINVALOP,"getOssOwner",fileName_);
		return(ossOwner_);
    } // getOssOwner

	/**
    * Returns the logical record length.  Structured files only.
    * Corresponds to item 196 of FILE_GETINFOLIST_.
    *
    * @return the record length
    *
    * @exception EnscribeFileException if file is unstructured.
    */
    public int getRecordLength() throws EnscribeFileException {
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot get the record length because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getRecordLength",fileName_);
		return(logicalRecordLength_);
    } // getRecordLength

	/**
    * Returns the number of alternate keys for structured files only.
    * Corresponds to item 100 of FILE_GETINFOLIST_.
    *
    * @return the number of alternate keys
    *
    * @exception EnscribeFileException if file is unstructured.
    * @see #getAlternateKeyAttributes
    * @see #updateAlternateKeyAttributes
    */
    public int getNumberOfAltkeys() throws EnscribeFileException {
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot get the number of alternate keys because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getNumberOfAltkeys",fileName_);
		return(numberOfAltkeys_);
    } // getNumberOfAltkeys

	/**
    * Returns the number of alternate key files for structured files only.
    * Corresponds to item 102 of FILE_GETINFOLIST_
    *
    * @return number of alternate key files
    *
    * @exception EnscribeFileException if file is unstructured.
    * @see #getAlternateKeyAttributes
    * @see #updateAlternateKeyAttributes
    */
    public int getNumberOfAltkeyfiles() throws EnscribeFileException{
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot get number of alternate key fiels because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getNumberofAltkeyfiles",fileName_);
		return(numberOfAltkeyfiles_);
    } // getNumberOfAltkeyfiles

	/**
    * Returns the primary key offset for key-sequenced files only.
    * Corresponds to item 198 of FILE_GETINFOLIST_.
    *
    * @return the primary key offset
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public int getKeyOffset() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid operation because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"getKeyOffset",fileName_);
		return(keyOffset_);
    } // getKeyOffset

	/**
    * Returns the primary key length for key-sequenced files only.
    * Corresponds to item 46 of FILE_GETINFOLIST_.
    *
    * @return the primary key length
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public int getKeyLength() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid operation because the file is not key-sequenced",
			EnscribeFileException.FEINVALOP,"getKeyLength",fileName_);
		return(keyLength_);
    } // getKeyLength

	/**
    * Returns the lock key length for key-sequenced files only.
    * Corresponds to item 47 of FILE_GETINFOLIST_.
    *
    * @return the lock key length
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public int getLockKeyLength() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid operation because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"getLockKeyLength",fileName_);
		return(lockKeyLength_);
    } // getLockKeyLength

	/**
    * Returns whether data blocks are compressed for key-sequenced files only.
    * Corresponds to item 68 of FILE_GETINFOLIST_.
    *
    * @return true if data blocks are compressed; false otherwise.
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public boolean areDataBlocksCompressed() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid operation because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"areDataBlocksCompressed",fileName_);
		return(dataCompressionFlag_);
    } // getDataCompressionFlag

	/**
    * Returns true if index blocks are compressed; false if otherwise.
    * Corresponds to item 69 of FILE_GETINFOLIST_.
    *
    * @return true if index block compression flag is set
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public boolean areIndexBlocksCompressed() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid operation because the file is not key sequenced",
				EnscribeFileException.FEINVALOP,"areIndexBlocksCompressed",fileName_);
		return(indexCompressFlag_);
    } // areIndexBlocksCompressed


	/**
    * Returns the number of index levels for key-sequenced files only.
    * Corresponds to item 81 of FILE_GETINFOLIST_.
    *
    * @return number of index levels
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public int getIndexLevels() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Cannot get index levels because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"getIndexLevels",fileName_);
		return(indexLevels_);
    } // getIndexLevels

	/**
    * Returns the partition partial key length for key-sequenced files only.
    * Corresponds to item 94 of FILE_GETINFOLIST_.
    *
    * @return partition partial key length
    *
    * @exception EnscribeFileException if file is not key sequenced.
    */
    public int getPartitionPartialKeyLength() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Cannot get the partitial key length because the file is not key sequenced",
				EnscribeFileException.FEINVALOP,"getPartitionPartialKeyLength",fileName_);
		return(partitionPartialKeyLength_);
    } // getPartitionPartialKeyLength

	/**
    * Returns the progid flag for executable files (unstructured) only.
    *
    * @return true if the progid flag is set
    *
    * @exception EnscribeFileException if file is not unstructured.
    */
    public boolean getProgidFlag() throws EnscribeFileException{
		if(fileType_ != FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot get progid because the file is not unstructured",
				EnscribeFileException.FEINVALOP,"getProgidFlag",fileName_);
		return(progidFlag_);
    } // getProgidFlag

	/**
    * Returns the licensed flag for executable files (unstructured) only.
    * Corresponds to item 63 of FILE_GETINFOLIST_.
    *
    * @return true if the licensed flag is set
    *
    * @exception EnscribeFileException if file is not unstructured.
    */
    public boolean isLicensed() throws EnscribeFileException{
		if(fileType_ != FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("File cannot be licensed because the file is not unstructured",
				EnscribeFileException.FEINVALOP,"isLicensed",fileName_);
		return(licensedFileFlag_);
    } // isLicensed

	/**
    * Indicates whether an unstructured file allows odd byte positioning
    * and transfers.
    * Corresponds to item 65 of FILE_GETINFOLIST_.
    *
    * @return true if odd byte positioning and transfers is allowed
    *
    * @exception EnscribeFileException if file is not unstructured.
    * @see #updateOddUnstructured
    */
    public boolean isOddUnstructured() throws EnscribeFileException{
		if(fileType_ != FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot determine the oddUnstructured attribute because the file is not unstructured",
				EnscribeFileException.FEINVALOP,"isOddUnstructured",fileName_);
		return(oddUnstructuredFlag_);
    } // isOddUnstructured
  /**
   * Specifies a new value for the odd-unstructured attribute.  This value is used if
   * this EnscribeFileAttribute object is passed to
   * the EnscribeFile.alter() method.
   * Corresponds to item 65 of FILE_ALTERLIST_.
   *
   * @param value true to allow odd byte positioning and transfers; false if
   * otherwise
   *
   * @exception EnscribeFileException if file is not unstructured.
   * @see #isOddUnstructured
   */
   public void updateOddUnstructured(boolean value) throws EnscribeFileException {
 		if(fileType_ != FILETYPE_UNSTRUCTURED)
 			throw new EnscribeFileException("Cannot update to oddUnstructured because this is not an unstructured file",
 				EnscribeFileException.FEINVALOP,"updateOddUnstructured",fileName_);
 		updatedOddunstructuredFlag_ = value;
 		updatedOddunstructuredFlagIsSet_ = true;
   } // isOddUnstructured

  /**
   * Specifies new the alternate key information to be used when altering
   * the file. This information is used when this EnscribeFileAttributes
   * object is passed as a parameter to the EnscribeFile.alter() method.
   * Corresponds to item 100, 101, and 103 of FILE_ALTERLIST_.
   *
   * @param altKeyDesc an array consisting of one or more AlternateKeyDescriptor objects
   * @exception java.lang.IllegalArgumentException if the altKeyDesc is null.
   * @see #getAlternateKeyAttributes
   */
	public void updateAlternateKeyAttributes(AlternateKeyDescriptor [] altKeyDesc) throws java.lang.IllegalArgumentException{
		int numKeys = 0;
		int i = 0;
		if(altKeyDesc == null)
			throw new java.lang.IllegalArgumentException("The AlternateKeyDescriptor array cannot be null");
		if(altKeyDesc.length == 0)
			throw new java.lang.IllegalArgumentException("The AlternateKeyDescription array cannot be zero(0) length");
		numKeys = altKeyDesc.length;

		Vector fileNames = new Vector(numKeys);
		for(i = 0; i < numKeys; i++) {
			String fn = altKeyDesc[i].getAlternateKeyFileName();
			if(fileNames.contains(fn)) {
				continue;
			} else {
				fileNames.add(i,altKeyDesc[i].getAlternateKeyFileName());
			}
		}
		int fnLen = fileNames.size();
		updatedFileNames_ = new String[fnLen];
		updatedFileNames_ = (String [])fileNames.toArray(updatedFileNames_);
		updatedAltKeyAlterAttr_ = new AlternateKeyAlterAttributes(numKeys,
			updatedFileNames_.length, updatedFileNames_, altKeyDesc);
	} // updateAlternateKeyAttributes

	/**
    * Returns an array of AlternateKeyDescriptor objects for structured files only.
    * Corresponds to item 101 and 104 of FILE_GETINFOLIST_
    *
    * @return an array consisting of one or more AlternateKeyDescriptor objects
    *
    * @exception java.lang.IllegalArgumentException if the file has no
    * alternate keys.
    * @exception EnscribeFileException if the file is unstructured.
    * @see #updateAlternateKeyAttributes
    */
    public  AlternateKeyDescriptor [] getAlternateKeyAttributes() throws EnscribeFileException,
    	java.lang.IllegalArgumentException {
		int i=0;
		int descriptorIndex = 0;
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot get alternate key information because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getAlternateKeyAttributes",fileName_);
		if(numberOfAltkeys_ < 1)
			throw new IllegalArgumentException("Cannot get alternate key information because no alternate keys exist for this file");
		//Set the fileName in each AlternateKeyDescriptor
		for(i = 0; i< altKeyInfo_.length; i++) {
			int filenum = altKeyInfo_[i].getKeyFileNum();
			altKeyInfo_[i].setAlternateKeyFileName(altKeyFileInfo_[filenum]);
		}
		return(altKeyInfo_);
    } // getAlternateKeyAttributes

    AlternateKeyDescriptor [] getAltKeyDescriptors() {
		return altKeyInfo_;
	}

	String [] getAltKeyFileNames() {
		return altKeyFileInfo_;
	}

	/**
	 * Specifies new the partition information to be used when altering
	 * the file. This information is used when this EnscribeFileAttributes
	 * object is passed as a parameter to the EnscribeFile.alter() method.
	 * Corresponds to item 90, 97, and 93 of FILE_ALTERLIST_.
	 *
	 * @param partitionInfo and array of PartitionDescriptor objects.
	 * @see #getPartitionDescriptors
	 *
	 */
	public void updatePartitionAttributes(PartitionDescriptor[] partitionInfo) {
		if(partitionInfo == null)
			throw new IllegalArgumentException("partitionInfo cannot be null");
		updatedPartitionAlterAttr_ = partitionInfo;
		updatedNumberOfPartitions_ = partitionInfo.length;
	} // setAlternateKeyAlterAttr


	/**
    * Returns the array of PartitionDescriptor objects.
    * Corresponds to item 97 and 93 of FILE_GETINFOLIST_.
    *
    * @return an array of PartitionDescriptor objects.
    *
    * @exception EnscribeFileException if the file has no
    * partitions.
    *
    */
    public  PartitionDescriptor [] getPartitionDescriptors() throws EnscribeFileException{
		if(numberOfPartitions_ < 1)
			throw new EnscribeFileException("Cannot get partition descriptors because no partitions exist for this file",
				EnscribeFileException.FEINVALOP,"getPartitionDescriptors",fileName_);
		return partitionInfo_;
    } // getPartitionDescriptors


	/*
	* internal procedure
	*/
	boolean isDetailInfoRetrieved() {
		return detailInfoRetrieved_;
	} // isDetailInfoRetrieved

	// -------------------------------------------------------
	// -------------------------------------------------------
	/*
    * Sets the file information from a "," separated list.
    * Used internally.
    *
    */
    private void setStoredFileAttributesFromString(String str) throws Exception {
		String
			ss = "";
		ParseDelimitedString pds = new ParseDelimitedString(str);
		int i;
		byte [] bfn = new byte[100];

		try
		{
			ss = pds.getNextString();
			if(!ss.equals("<FI"))
				throw new Exception("Invalid use of setStoredFileAttributesFromString - 1");
			boolean
				keepGoing = true;
			while(keepGoing)
			{
				int
					optVal = pds.getNextInt();
				switch(optVal)
				{
					case OV_PHYSICAL_RECORD_LENGTH:
					physicalRecordLength_ = pds.getNextInt();
					break;

					case OV_FILE_TYPE:
					fileType_ = pds.getNextShort();
					break;

					case OV_FILE_CODE:
					fileCode_ = pds.getNextInt();
					break;

					case OV_LOGICAL_RECORD_LENGTH:
					logicalRecordLength_ = pds.getNextInt();
					break;

					case OV_BLOCK_LENGTH:
					blockLength_ = pds.getNextInt();
					break;

					case OV_KEY_OFFSET:
					keyOffset_ = pds.getNextInt();
					break;

					case OV_KEY_LENGTH:
					keyLength_ = pds.getNextInt();
					break;

					case OV_LOCK_KEY_LENGTH:
					lockKeyLength_ = pds.getNextInt();
					break;

					case OV_PRIMARY_EXTENT_SIZE:
					primaryExtentSize_ = pds.getNextInt();
					break;

					case OV_SECONDARY_EXTENT_SIZE:
					secondaryExtentSize_ = pds.getNextInt();
					break;

					case OV_MAXIMUM_EXTENTS:
					maximumExtents_ = pds.getNextInt();
					break;

					case OV_ALLOCATED_EXTENTS:
					allocatedExtents_ = pds.getNextInt();
					break;

					case OV_GUARDIAN_FILE_OWNER:
					fileGuardianOwnerGroupid_ = pds.getNextShort();
					fileGuardianOwnerUserid_ = pds.getNextShort();
					break;

					case OV_SAFEGUARD_PROTECTED:
					safeguardProtected_ = pds.getNextBoolean();
					break;

					case OV_PROGID_FLAG:
					progidFlag_ = pds.getNextBoolean();
					break;

					case OV_CLEAR_ON_PURGE_FLAG:
					ClearOnPurgeFlag_ = pds.getNextBoolean();
					break;

					case OV_SECURITY_STRING:
					guardianSecurityString_ = pds.getNextString();
					break;

					case OV_LICENSED_FILE_FLAG:
					licensedFileFlag_ = pds.getNextBoolean();
					break;

					case OV_ODD_UNSTRUCTURED_FLAG:
					oddUnstructuredFlag_ = pds.getNextBoolean();
					break;

					case OV_AUDITED_FLAG:
					auditedFlag_ = pds.getNextBoolean();
					break;

					case OV_AUDIT_COMPRESSION_FLAG:
					auditCompressionFlag_ = pds.getNextBoolean();
					break;

					case OV_DATA_COMPRESSION_FLAG:
					dataCompressionFlag_ = pds.getNextBoolean();
					break;

					case OV_INDEX_COMPRESS_FLAG:
					indexCompressFlag_ = pds.getNextBoolean();
					break;

					case OV_REFRESH_EOF_FLAG:
					refreshEofFlag_ = pds.getNextBoolean();
					break;

					case OV_WRITE_THROUGH_FLAG:
					writeThroughFlag_ = pds.getNextBoolean();
					break;

					case OV_VERIFY_WRITES_FLAG:
					verifyWritesFlag_ = pds.getNextBoolean();
					break;

					case OV_SERIAL_WRITES_FLAG:
					serialWritesFlag_ = pds.getNextBoolean();
					break;

					case OV_FILE_IS_OPEN_FLAG:
					fileIsOpenFlag_ = pds.getNextBoolean();
					break;

					case OV_CRASH_OPEN_FLAG:
					crashOpenFlag_ = pds.getNextBoolean();
					break;

					case OV_ROLLFORWARD_NEEDED_FLAG:
					rollforwardNeededFlag_ = pds.getNextBoolean();
					break;

					case OV_BROKEN_FLAG:
					brokenFlag_ = pds.getNextBoolean();
					break;

					case OV_CORRUPT_FLAG:
					corruptFlag_ = pds.getNextBoolean();
					break;

					case OV_SECONDARY_PARTITION_FLAG:
					secondaryPartitionFlag_ = pds.getNextBoolean();
					break;

					case OV_INDEX_LEVELS:
					indexLevels_ = pds.getNextInt();
					break;

					case OV_NUMBER_OF_PARTITIONS:
					numberOfPartitions_ = pds.getNextInt();
					partitionInfo_ = new PartitionDescriptor[numberOfPartitions_];
					// get extents
					for(i=0;i<numberOfPartitions_;i++)
					{
						PartitionDescriptor
							pd = new PartitionDescriptor();
						pd.setPrimaryExtentSize(pds.getNextLong());
						pd.setSecondaryExtentSize(pds.getNextLong());
						partitionInfo_[i] = pd;
					}
					// get volume names
					for(i=0;i<numberOfPartitions_;i++)
					{
						PartitionDescriptor pd = partitionInfo_[i];
						pd.setVolumeName(pds.getNextString());
					}
					if(fileType_ == FILETYPE_KEY_SEQUENCED)
					{
						// get partial key
						int
							dummy = pds.getNextInt(); // partial key flag
						partitionPartialKeyLength_ = pds.getNextInt();
						int
							j;
						byte []
							partialKey = new byte [partitionPartialKeyLength_];
						for(i=0;i<numberOfPartitions_;i++)
						{
							PartitionDescriptor
								pd = partitionInfo_[i];
							for(j=0;j< partitionPartialKeyLength_;j++)
							{
								partialKey[j] = (byte)pds.getNextInt();
							}
							pd.setPartialKeyValue(partialKey);
						}
					}
					break;

					case OV_NUMBER_OF_ALT_KEYS:
					numberOfAltkeys_ = pds.getNextInt();
					altKeyInfo_ = new AlternateKeyDescriptor[numberOfAltkeys_];
					altKeyArray_ = new int[numberOfAltkeys_];
					for(i=0;i<numberOfAltkeys_;i++)
					{
						AlternateKeyDescriptor
							akd = new AlternateKeyDescriptor();
						altKeyInfo_[i] = akd;
						akd.setKeySpecifier(pds.getNextShort());
						akd.setKeyLength(pds.getNextInt());
						akd.setKeyOffset(pds.getNextInt());
						int keyNum = pds.getNextInt();
						altKeyArray_[keyNum]++;
						akd.setKeyFileNum(keyNum);
						short
							nullValue = pds.getNextShort();
						if(pds.getNextBoolean())
							akd.setNullValue(nullValue);
						if(pds.getNextBoolean())
							akd.setKeyIsUnique(true);
						if(pds.getNextBoolean())
							akd.setAutomaticUpdating(true);
						if(pds.getNextBoolean())
							akd.setOrderByInsertionSequence(true);

					}
					break;

					case OV_NUMBER_OF_ALT_KEY_FILES:
					numberOfAltkeyfiles_ = pds.getNextInt();
					altKeyFileInfo_ = new String[numberOfAltkeyfiles_];
					for(i=0;i<numberOfAltkeyfiles_;i++)
					{
						altKeyFileInfo_[i] = pds.getNextString();
					}
					break;

					case OV_SEQUENTIAL_BLOCK_BUFFERING_FLAG:
					sequentialBlockBufferingFlag_ = pds.getNextBoolean();
					break;

					case OV_LAST_OPEN_TIME:
					lastOpenTime_ = pds.getNextCalendar();
					break;

					case OV_EXPIRATION_TIME:
					expirationTime_ = pds.getNextCalendar();
					break;

					case OV_CREATION_TIME:
					creationTime_ = pds.getNextCalendar();
					break;

					case OV_MODIFICATION_TIME:
					modificationTime_ = pds.getNextCalendar();
					break;

					case OV_IS_OSS_FILE:
					isossFile_ = pds.getNextBoolean();
					break;

					case OV_OSS_OWNER_GROUP_ID:
					ossOwnerGroupId_ = pds.getNextInt();
					break;

					case OV_OSS_ACCESS_PERMISSIONS:
					ossAccessPermissions_ = pds.getNextInt();
					break;

					case OV_OSS_OWNER:
					ossOwner_ = pds.getNextInt();
					break;

                                 /* Begin Changes. sol: 10-030703-7581 
                                    added the case for OV_AGGREGATE_EOF
                                    and OV_AGGREGATE_MAX_FILE_SIZE         
                                 */         
                                        case OV_AGGREGATE_EOF:
                                        aggregateEOF_ = pds.getNextLong();
                                        break;

                                        case OV_AGGREGATE_MAX_FILE_SIZE:
                                        aggregateMaxFileSize_ = pds.getNextLong();
                                        break;
                                 /* End Changes. sol: 10-030703-7581 */        


					case OV_FILE_FORMAT:
					fileFormat_ = pds.getNextInt();
					break;

					case OV_LAST_OPTVAL:
					keepGoing = false;
					break;

					default:
						throw new Exception("Invalid use of setStoredFileAttributesFromString - ** unexpected optval " + optVal);
				}
			}

			ss = pds.getNextString();
			if(!ss.equals("FI>"))
			{
				throw new Exception("Invalid use of setStoredFileAttributesFromString - 10"+ss);
			}

		} catch (ParseDelimitedStringException ex) {
			throw new Exception("Invalid use of setStoredFileAttributesFromString: " + ex.getMessage());
		}

	} // setStoredFileAttributesFromString


	String getAttributeString()
	{
		StringBuffer str = new StringBuffer(200);
		switch(fileType_) {
			case FILETYPE_UNSTRUCTURED:
				str.append("<UA,");
				break;
			case FILETYPE_RELATIVE:
				str.append("<RA,");
				break;
			case FILETYPE_ENTRY_SEQUENCED:
				str.append("<EA,");
				break;
			case FILETYPE_KEY_SEQUENCED:
				str.append("<KA,");
				break;
			default: break;
		}
		if(updatedFileCodeSet_)
			str.append(OV_FILE_CODE).append(",").append(updatedFileCode_).append(",");
		if(updatedExpirationTimeSet_ && updatedExpirationTime_.get(Calendar.YEAR) > -1) {
			str.append(OV_EXPIRATION_TIME_GMT).append(",");
			str.append(updatedExpirationTime_.get(Calendar.YEAR)).append(",");
			str.append(updatedExpirationTime_.get(Calendar.MONTH)).append(",");
			str.append(updatedExpirationTime_.get(Calendar.DATE)).append(",");
			str.append(updatedExpirationTime_.get(Calendar.HOUR_OF_DAY)).append(",");
			str.append(updatedExpirationTime_.get(Calendar.MINUTE)).append(",");
			str.append(updatedExpirationTime_.get(Calendar.SECOND)).append(",");
		}
		if(updatedAuditedFlagSet_)
		{
			if(updatedAuditedFlag_)
				str.append(OV_AUDITED_FLAG).append(",1,");
			else
				str.append(OV_AUDITED_FLAG).append(",0,");
		}
		if(updatedRefreshEOFFlagSet_)
		{
			if(updatedRefreshEOFFlag_)
				str.append(OV_REFRESH_EOF_FLAG).append(",1,");
			else
				str.append(OV_REFRESH_EOF_FLAG).append(",0,");
		}
		if(updatedNumberOfPartitions_ > 0) {
				str.append(OV_NUMBER_OF_PARTITIONS).append(",").append(updatedNumberOfPartitions_).append(",");
				str.append(OV_PARTITION_EXTENTS).append(",");
				PartitionDescriptor pd;
				for(int p=0;p<updatedNumberOfPartitions_;p++) {
					pd = updatedPartitionAlterAttr_[p];
					str.append(pd.getPrimaryExtentSize()).append(",");
					str.append(pd.getSecondaryExtentSize()).append(",");
				}
				str.append(OV_PARTITION_VOLUME_NAME_LENS).append(",");
				for(int q=0;q<updatedNumberOfPartitions_;q++) {
					pd = updatedPartitionAlterAttr_[q];
					str.append(pd.getVolumeName().length()).append(",");
				}
				str.append(OV_PARTITION_VOLUME_NAMES).append(",");
				for(int r=0;r<updatedNumberOfPartitions_;r++) {
					pd = updatedPartitionAlterAttr_[r];
					str.append(pd.getVolumeName()).append(",");
				}
		}

		if(updatedAltKeyAlterAttr_ != null) {
			str.append(updatedAltKeyAlterAttr_.getAttributeString());
		}

		switch(fileType_) {
			case FILETYPE_UNSTRUCTURED:
				str.append(OV_LAST_OPTVAL).append(",UA>,");
				break;
			case FILETYPE_RELATIVE:
				str.append(OV_LAST_OPTVAL).append(",RA>,");
				break;
			case FILETYPE_ENTRY_SEQUENCED:
				str.append(OV_LAST_OPTVAL).append(",EA>,");
				break;
			case FILETYPE_KEY_SEQUENCED:
				str.append(OV_LAST_OPTVAL).append(",KA>,");
				break;
			default: break;
		}
		return str.toString();
	} // getAttributeString

	// returns String of optValues pairs
   private final native synchronized
	String GgetFileInfoDetails(/*in*/ short fileNumber,
				 /*in*/ String fileName,
				 /*in*/ boolean usingFileName,
				 /*in*/ boolean ossFormatFileName,
				 /*out*/ short[] errorArray);

}
