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
import com.tandem.ext.enscribe.EnscribeFileException;
import com.tandem.ext.enscribe.EnscribeFile;
import java.util.*;
import com.tandem.ext.util.NativeLoader;


/**
 * <P>EnscribeLastCallStatus provides access to transient information (attributes)
 * about an opened Enscribe file.  This class provides methods to obtain
 * information about the current state of the file after a Guardian operation
 * was performed on the file.  Therefore, the information provided by this
 * class is transient and only valid for the period of time that exists after one Guardian operation is performed and before the
 * next Guardian operation is performed.
 * </P>
 *
 * <P>  This information is obtained by a call to the Guardian procedure FILE_GETINFOLIST_.
 * </P>
 *
 * <P> The information provided by the methods of this class can be divided into
 * four areas:
 * <OL>
 * <LI> Additional error information about the file including:
 * <UL>
 * <LI>	More information about the error reported by an EnscribeFileException
 * <LI> The number of the partition (if any) on which the error reported by EnscribeFileException occurred
 * <LI> For files with alternate keys, the key specifier associated with the error reported
 * by EnscribeFileException.
 * </UL>
 * <LI> Pointers (record specifiers) for the current and next records in the file.  These
 * values can be passed to the Enscribe.position() method.
 * <LI> The value and length(in bytes) of the current key.
 * <LI> For key-sequenced files only, the value and length(in bytes) of the
 * primary key.
 * </OL>
 </P>
 *
 * @see EnscribeFile#getLastCallStatus
 */

public class EnscribeLastCallStatus
{
	static {	 
		 NativeLoader loader = new NativeLoader();
		 loader.LoadLibrary();		
	 }
	/**
	 * The file type for an unstructured file.
	 */
	private static final int FILETYPE_UNSTRUCTURED = 0;
	/**
	 * The file type for a relative file
	 */
	private static final int FILETYPE_RELATIVE = 1;
	/**
	 * The file type for an entry-sequenced file.
	 */
	private static final int FILETYPE_ENTRY_SEQUENCED = 2;
	/**
	 * The file type for a key-sequenced file.
	 */
	private static final int FILETYPE_KEY_SEQUENCED = 3;

	// For open file only
	int errorDetail_ = 0;
	int partitionInError_ = 0;
	int openAccessMode_ = 0;
	int openExclusionMode_ = 0;
	long nextRecordPointer_ = 0;
	long currentRecordPointer_ = 0;
	int fileType_ = 0;
	int fileCode_ = 0;
	boolean isossFile_ = false;
	private boolean detailInfoRetrieved_ = false;
	private boolean currentInfoRetrieved_ = false;
	int ossOwner_ = 0;
	String fileName_;



	// structured file attributes
	int keyInError_ = 0; // open file only
	int currentKeySpecifier_ = 0; // open file only
	int currentKeyLength_ = 0; // open file only
	byte[] currentKeyValue_ ; // open file only
	int logicalRecordLength_ = 0;

	// key sequenced attributes
	int currentPrimaryKeyLength_ = 0; // open file only
	byte[] currentPrimaryKeyValue_ ; // open file only

	// internal use
	private short fileNumber_ = -1;
	private final short  ERR_NOT_DISC_FILE = 3001;
	private final short  DEVICETYPE_DISC = 3;

	// ---------- flags for encoding and decoding -------
	// keep in sync with enscribejni.h
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
	static final int    OV_LAST_OPTVAL = 9999;

   /**
    * Construct an EnscribeLastCallStatus object for query by filenumber.
    *
    * @param fileNumber the file number of the open file
    * @param fileType the type of the open file
    * @param isOssfile true if the file is an oss file??
    * @exception com.tandem.enscribe.EnscribeFileException an error occurs trying to obtain the file attributes.
    */
    EnscribeLastCallStatus(short fileNumber, int fileType, boolean isOssFile, String fileName) throws EnscribeFileException {
        fileNumber_ = fileNumber;
        fileType_ = fileType;
		isossFile_ = isOssFile;
		fileName_ = fileName;
		this.getLastCallStatus();
    } // EnscribeLastCallStatus

	/**
    * Retrieves the current attributes of an open file.
    *
    * @exception com.tandem.enscribe.EnscribeFileException if an error occurs getting file
    * details.
    */
    void getLastCallStatus() throws EnscribeFileException{
		short []
			errorArray = new short [2];

		String
			str = GgetCurrentFileInfo(fileNumber_, (short)fileType_,
									  isossFile_,
									  errorArray);
		if(errorArray[0] == ERR_NOT_DISC_FILE)
			throw new EnscribeFileException("File is not a disc file, cannot get file attributes",
				EnscribeFileException.FEINVALOP,"getLastCallStatus",fileName_);
		if(errorArray[0] != 0)
			throw new EnscribeFileException("File error trying to get file attributes",
				errorArray[0],"getLastCallStatus",fileName_);
		try
		{
			 setCurrentFileAttributesFromString(str);
		}
		catch (Exception ex) {
			throw new EnscribeFileException("Unable to parse file attributes string. Internal error: " + ex.getMessage(),
				EnscribeFileException.FEBADFORM,"getLastCallStatus",fileName_);
		}
		currentInfoRetrieved_ = true;
    } // getCurrentOpenFileAttributes


	/**
    * Returns the error detail associated with the last operation on
    * this EnscribeFile object.
    *
    * @return a number containing the error detail
    */
    public int getErrorDetail() {
		return(errorDetail_);
    } // getErrorDetail

	/**
	* Returns the partition in error during the last operation on
    * this file.
    *
    * @return the number of the partition in error
     */
    public int getPartitionInError() {
        return(partitionInError_);
    } // getPartitionInError

	/**
    * Returns the next record pointer for this open file.  This value can
    * be saved and used in EnscribeFile.position().
    *
    * @see EnscribeFile#position
    *
    * @return next record pointer
    */
    public long getNextRecordPointer() {
        return(nextRecordPointer_);
    } // getNextRecordPointer

	/**
    * Returns the current record pointer for this open file.  This value can
    * be saved and used in EnscribeFile.position().
    *
    * @see EnscribeFile#position
    *
    * @return current record pointer
    */
    public long getCurrentRecordPointer() {
        return(currentRecordPointer_);
    } // getCurrentRecordPointer

	/**
    * Returns the key in error only for structured files that are open.
    *
    * @return key in error
    *
    * @exception EnscribeFileException if the file is unstructured.
    */
    public int getKeyInError() throws EnscribeFileException {
 		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot getKeyInError because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getKeyInError",fileName_);
		return(keyInError_);
    } // getKeyInError

	/**
    * Returns the current key specifier only for structured files that are
    * open.
    *
    * @return current key specifier
    *
    * @exception EnscribeFileException if the file is unstructured.
    */
    public int getCurrentKeySpecifier() throws EnscribeFileException{
 		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot call getCurrentKeySpecifier because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getCurrentKeySpecifier",fileName_);
		return(currentKeySpecifier_);
    } // getCurrentKeySpecifier

	/**
    * Returns the current key length only for structured files that are
    * open.
    *
    * @return current key length
    *
    * @exception EnscribeFileException if the file is unstructured.
    */
    public int getCurrentKeyLength() throws EnscribeFileException{
 		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot call getCurrentKeyLength because the file is unstructured",
				EnscribeFileException.FEINVALOP,"getCurrentKeyLength",fileName_);
		return(currentKeyLength_);
    } // getCurrentKeyLength

	/**
    * Returns the current key value only for structured files that are
    * open.
    *
    * @return byte array, current key value
    *
    * @exception EnscribeFileException if the file is unstructured.
    */
    public byte[] getCurrentKeyValue() throws EnscribeFileException {
		if(fileType_ == FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException("Cannot call getCurrentKeyValue for an unstructured file",
				EnscribeFileException.FEINVALOP,"getCurrentKeyValue",fileName_);
		return currentKeyValue_;
    } // getCurrentKeyValue

	/**
    * Returns the current primary key length for key-sequenced files only.
    *
    * @return current primary key length
    *
    * @exception EnscribeFileException if file is not key-sequenced.
    */
    public int getCurrentPrimaryKeyLength() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid call to getCurrentPrimaryKeyLength because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"getCurrentPrimaryKeyLength",fileName_);
		return(currentPrimaryKeyLength_);
    } // getCurrentPrimaryKeyLength

	/**
    * Returns the current primary key value for key-sequenced files only.
    *
    * @return byte array current primary key length
    *
    * @exception EnscribeFileException if file is not key-sequenced.
    */
    public byte [] getCurrentPrimaryKeyValue() throws EnscribeFileException{
		if(fileType_ != FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException("Invalid call to getCurrentPrimaryKeyValue because the file is not key-sequenced",
				EnscribeFileException.FEINVALOP,"getCurrentPrimaryKeyValue",fileName_);
		return currentPrimaryKeyValue_;
    } // getCurrentPrimaryKeyValue

	/*
	* internal procedure
	*/
	boolean isDetailInfoRetrieved() {
		return detailInfoRetrieved_;
	} // isDetailInfoRetrieved

	// -------------------------------------------------------
	// -------------------------------------------------------
	/*
    * Sets the current file information from a "," separated list.
    * Used internally.
    *
    */
    private void setCurrentFileAttributesFromString(String str) throws Exception {
		String
			ss = "";
		ParseDelimitedString
			pds = new ParseDelimitedString(str);
		int
			i;
		int
			j;
		byte [] bfn = new byte[100];

		try
		{
			ss = pds.getNextString();
			if(!ss.equals("<FI"))
				throw new Exception("Invalid use of setCurrentFileAttributesFromString - 1");
			boolean
				keepGoing = true;
			while(keepGoing)
			{
				int
					optVal = pds.getNextInt();
				switch(optVal)
				{
					case OV_ERROR_DETAIL:
					errorDetail_ = pds.getNextInt();
					break;

					case OV_PARTITION_IN_ERROR:
					partitionInError_ = pds.getNextInt();
					break;

					case OV_KEY_IN_ERROR:
					keyInError_ = pds.getNextInt();
					break;

					case OV_NEXT_RECORD_POINTER:
					nextRecordPointer_ = pds.getNextLong();
					break;

					case OV_CURRENT_RECORD_POINTER:
					currentRecordPointer_ = pds.getNextLong();
					break;

					case OV_CURRENT_KEY_SPECIFIER:
					currentKeySpecifier_ = pds.getNextShort();
					break;

					case OV_CURRENT_KEY_LENGTH:
					currentKeyLength_ = pds.getNextInt();
					break;

					case OV_CURRENT_KEY_VALUE:
					currentKeyValue_ = new byte [currentKeyLength_];
					for(j=0;j< currentKeyLength_;j++)
					{
						currentKeyValue_[j] = (byte)pds.getNextInt();
					}
					break;

					case OV_CURRENT_PRIMARY_KEY_LENGTH:
					currentPrimaryKeyLength_ = pds.getNextInt();
					break;

					case OV_CURRENT_PRIMARY_KEY_VALUE:
					currentPrimaryKeyValue_ = new byte [currentPrimaryKeyLength_];
					//System.out.println("currentPrimaryKeyLength_ = "+currentPrimaryKeyLength_);
					for(j=0;j< currentPrimaryKeyLength_;j++)
					{
						currentPrimaryKeyValue_[j] = (byte)pds.getNextInt();
					}
					//System.out.println("length of byte array is "+currentPrimaryKeyValue_.length);
					break;

					case OV_OPEN_ACCESS_MODE:
					openAccessMode_ = pds.getNextInt();
					break;

					case OV_OPEN_EXCLUSION_MODE:
					openExclusionMode_ = pds.getNextInt();
					break;

					case OV_LAST_OPTVAL:
					keepGoing = false;
					break;

					default:
						throw new Exception("Invalid use of setCurrentFileAttributesFromString - ** unexpected optval " + optVal);
				}
			}

			ss = pds.getNextString();
			if(!ss.equals("FI>"))
			{
				throw new Exception("Invalid use of setCurrentFileAttributesFromString - 10");
			}

		} catch (ParseDelimitedStringException ex) {
			throw new Exception("Invalid use of setCurrentFileAttributesFromString: " + ex.getMessage());
		}

	} // setCurrentFileAttributesFromString

	// returns String of optValues pairs
   private final native synchronized
	String GgetCurrentFileInfo(/*in*/ short fileNumber,
				 /*in*/ short fileType,
				 /*in*/ boolean ossFormatFileName,
				 /*out*/ short[] errorArray);

}
