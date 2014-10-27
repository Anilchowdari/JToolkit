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
 * <P>EnscribeOpenOptions handles the attributes used
 * to open a disk file.  This class is passed as a
 * parameter to the EnscribeFile.open() method.</P>
 *
 * <P>Example: To open a file, you could do the following:</P>
 *
 *<PRE>
 *  try {
 *      EnscribeFile myfile = new EnscribeFile("/G/data/myvol/myfile");
 *      EnscribeOpenOptions openAttr = new EnscribeOpenOptions();
 *      openAttr.setAccess(READ_WRITE);
 *      openAttr.setExclusion(EXCLUSIVE);
 *      myfile.open(openAttr);
 *  }
 *  catch (com.tandem.enscribe.EnscribeFileException ex) {
 *      System.out.println(ex.getMessage());
 *  }
 *  catch (java.lang.IllegalArgumentException ia) {
 *		System.out.println(ia.getMessage());
 *  }
 *</PRE>
 *
 * <P>Refer to the description of FILE_OPEN_ (documented in the Guardian Procedure
 * Calls Reference Manual) for more information about the file open attributes
 * and their values.</P>
 *
 * @see EnscribeFile
 */

public class EnscribeOpenOptions
{
   /**
    * File Access: read and write access.
    *	The default for File Access is READ_WRITE
    */
    public static final int READ_WRITE = 0;

   /**
    * File Access: read access only.
    */
    public static final int READ_ONLY = 1;

   /**
    * File Access: write access only.
    */
    public static final int WRITE_ONLY = 2;

   /**
    * File Exclusion: Shared.
    *	(The default for File Exclusion is SHARED.)
    */
    public static final int SHARED = 0;

   /**
    * File Exclusion: Exclusive
    */
    public static final int EXCLUSIVE = 1;

   /**
    * File Exclusion: Protected
    */
    public static final int PROTECTED = 3;


    private int access_ = READ_WRITE;
    private int exclusion_ = SHARED;
    private int syncDepth_ = 0;
    private int seqBlockBufferLen_ = 0;

    private boolean unstructuredAccess_ = false;
    private boolean noOpenTimeUpdate_ = false;
  	private boolean use64BitPrimaryKeyValues_ = false;


   /**
    * Construct an EnscribeOpenOptions object.
    */
    public EnscribeOpenOptions() {
    } // EnscribeOpenOptions

   /**
    * Sets the desired access mode of the file to be opened
    *
    * @param access specifies the access mode.  If this parameter is not set, access mode
	* defaults to READ_WRITE.  Valid values are: READ_WRITE, READ_ONLY, WRITE_ONLY.
    * @exception java.lang.IllegalArgumentException if access is
    * not one of: READ_WRITE, READ_ONLY or WRITE_ONLY.
	*
	* @see #getAccess
    */
    public void setAccess(int access) throws java.lang.IllegalArgumentException {

        switch(access) {
            case READ_WRITE:
            case READ_ONLY:
            case WRITE_ONLY:
                access_ = access;
                break;
            default:
                throw new IllegalArgumentException("Invalid access specified for setAccess()");
        } // switch

    } // setAccess

   /**
    * Returns the access mode: READ_WRITE, READ_ONLY, WRITE_ONLY.
    *
    * @return access mode
    *
    * @see #setAccess
    */
    public int getAccess() {
        return access_;
    } // getAccess

   /**
    * Sets the desired mode of compatibility with other openers of the file.
    *
    * @param access specifies the exclusion mode.  If this parameter is
    * not set, exclusion mode defaults to SHARED.
    * Valid values are: SHARED, EXCLUSIVE, PROTECTED.
    * @exception java.lang.IllegalArgumentException if exclusion
    * is not one of: SHARED, EXCLUSIVE, PROTECTED.
    *
    * @see #getExclusion
    */
    public void setExclusion(int exclusion) throws java.lang.IllegalArgumentException {
        switch(exclusion) {
            case SHARED:
            case EXCLUSIVE:
            case PROTECTED:
                exclusion_ = exclusion;
                break;
            default:
                throw new IllegalArgumentException("Invalid exclusion value specified for setExclusion()");
		} // switch

    } // setExclusion

   /**
    * Returns the exclusion mode: SHARED, EXCLUSIVE, or PROTECTED.
    *
    * @return exclusion mode
    *
    * @see #setExclusion
    */
    public int getExclusion() {
        return exclusion_;
    } // getExclusion

   /**
    * Sets the sync depth which specifies the number of nonretryable (that is, write) requests
    * whose completion the file system must remember.
    *
    * @param syncDepth the number of nonretryable write requests the file system must remember;
    * specify 0 (the default) to indicate that internal checkpointing is not requested.
    * @exception java.lang.IllegalArgumentException if syncDepth is
    * less than 0 or greater than 15.
    *
    * @see #getSyncDepth
    */
    public void setSyncDepth(int syncDepth) throws java.lang.IllegalArgumentException{
        if(syncDepth >= 0 && syncDepth <= 15)
            syncDepth_ = syncDepth;
        else
            throw new IllegalArgumentException("Invalid value specified for setSyncDepth");
    } // setSyncDepth

   /**
    * Returns the sync depth.
    *
    * @return sync depth
    *
    * @see #setSyncDepth
    */
    public int getSyncDepth() {
        return syncDepth_;
    } // getSyncDepth


   /**
    * Indicates that sequential block buffering is requested.
    * Specifies the length in bytes of the
    * sequential block buffer.
    *
    * @param seqBlockBufferLen length of sequential block buffer
    * (If set to 0, sequential block buffering is not requested.)
    *
    * @see #getSequentialBlockBufferLength
    */
    public void setSequentialBlockBufferLength(int seqBlockBufferLen) {
        seqBlockBufferLen_ = seqBlockBufferLen;
    } // setSequentialBlockBufferLength

   /**
     * Returns the value of sequential block buffer length.
   *
    * @return sequential block buffer length.
    *
    * @see #setSequentialBlockBufferLength
    */
    public int getSequentialBlockBufferLength() {
        return seqBlockBufferLen_;
    } // getSequentialBlockBufferLength


   /**
    * Specifies that access should occur as if the file is
    * unstructured, that is, without regard to record structures and partitioning.
	* @param value true to use unstructured access; false (the default) otherwise
	* @see #getUnstructuredAccess
    */
    public void setUnstructuredAccess(boolean value){
        unstructuredAccess_ = value;
    } // setUnstructuredAccess

   /**
    * Returns whether unstructured access will be used.
    *
    * @return true if unstructured access has been set.
    *
    * @see #setUnstructuredAccess
    */
    public boolean getUnstructuredAccess() {
        return unstructuredAccess_;
    } // getUnstructuredAccess

   /**
    * Returns whether the "time of the last open" file attribute
    * will NOT be updated by this open.
    * @return true if file attribute will be updated.
    *
    * @see #setNoOpenTimeUpdate
    */
    public boolean getNoOpenTimeUpdate() {
        return noOpenTimeUpdate_;
    } // getNoOpenTimeUpdate

   /**
    * Specifies whether the "time of last open" file attribute should NOT be
    * updated by this open.
    * @param value specify true to indicate that the open time should NOT be updated; false
    * otherwise.
    *
    * @see #getNoOpenTimeUpdate
    */
    public void setNoOpenTimeUpdate(boolean value) {
        noOpenTimeUpdate_ = value;
    } // setNoOpenTimeUpdate

    /** Specifies whether 64-bit primary keys values can be used for
	 * unstructured, relative, or entry-sequenced files.
	 * @param value true if 64-bit primary key values are to be used; false
	 * otherwise.
	 */
	public void setUse64BitPrimaryKeyValues(boolean value) {
		use64BitPrimaryKeyValues_ = value;
	}
 	/** Returns whether 64-bit primary keys values can be used for
	 * unstructured, relative, or entry-sequenced files.
	 * @return true if 64-bit primary key values are to be used; false
	 * otherwise.
	 */
	public boolean getUse64BitPrimaryKeyValues() {
		return use64BitPrimaryKeyValues_;
	}


} // EnscribeOpenOptions
