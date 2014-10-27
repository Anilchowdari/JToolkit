/* ------------------------------------------------------------------------
* Filename:     SysMsgOpen.java
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2002
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

import com.tandem.ext.util.DataConversion;
import com.tandem.ext.util.DataConversionException;

/**
 * The <code>SysMsgOpen</code> class is a wrapper for an open system 
 * message read in from $RECEIVE.
 * <p>
 * An open system message is received when another process opens the server
 * process.  Once the server process has been opened it can receive user
 * messages from the opener..
 * </p>
 *
 * @version 1.0, 9/6/2001
 * @see com.tandem.ext.guardian.Receive
 */
public class SysMsgOpen {

   /* the data found in an open message */
   private short _accessMode;
   private short _exclusionMode;
   private short _nowaitDepth;
   private short _syncDepth;
   private short _openOptions;
   private short _PAID;
   private boolean _PAIDVerifiedLocally;
   private boolean _remoteOpener;
   private boolean _backupOpen;
   private ProcessHandle _primaryPhandle;
   private String _openQualifier;
   private String _openerName;
   private short _primaryFileNumber;
   private short _CAID;
   private String _homeTerminal;

   /**
    * Returns the access mode specified by the opener.
    * <pre>
    *  0 = Read-write
    *  1 = Read only
    *  2 = Write only
    *  3 = Extend (supported only for tape)
    * </pre>
    *
    * @return the access mode in the open system message
    */
   public short getAccessMode() {return _accessMode;}

   /**
    * Returns the exclusion mode specified by the opener.
    * <pre>
    *  0 = Shared
    *  1 = Exclusive
    *  2 = Process exclusive (supported only for Optical Storage Facility)
    *  3 = Protected
    * </pre>
    *
    * @return the exclusion mode in the open system message
    */
   public short getExclusionMode() {return _exclusionMode;}

   /**
    * Returns the nowait depth specified by the opener.
    * <code>nowaitDepth</code> specifies the number of nowait I/O operations that can be 
    * in progress for the file.  The maximum value is 15.
    *
    * @return the nowait depth in the open system message
    */
   public short getNowaitDepth() {return _nowaitDepth;}

   /**
    * Returns the open options specified by the opener.
    * Open options specifies various optional characteristics.  
    * The bits when set to 1, indicate the following: <pre>
    *    <0> Unstructured access. For disk files, access is to occur as if the file were
    *    unstructured, that is, without regard to record structures and partitioning.
    *    (For unstructured files, setting this bit to 1 causes secondary partitions to
    *    be inaccessible.) Must be 0 for other devices.
    *    <1> Reserved (specify 0).
    *    <2> No open time update. For disk files, the "time of last open" file attribute
    *    is not updated by this open. Must be 0 for other devices.
    *    <3> Any file number for backup open. When performing a backup open,
    *    specifies that the system can use any file number for the backup open. 0
    *    specifies that the backup open is to have the same file number as the
    *    primary open. Error 12 is returned if that file number is already in use.
    *    <4:9> Reserved (specify 0)
    *    <10> Open an OSS file by its OSS pathname. Specifies that the file to be
    *    opened is identified by the pathname parameter.
    *    <11:15> Reserved (specify 0)
    *
    * @return the open options in the open system message
    */
   public short getOpenOptions() {return _openOptions;}

   /**
    * Returns the process access ID of the opener.
    *
    * @return the user ID in the open system message
    */
   public short getProcessAccessID() {return _PAID;}

   /**
    * Returns information on verification of opener user ID.
    *
    * @return true if the user ID of the opener was verified locally (in the
    * receiver's node) otherwise returns false. 
    */
   public boolean isUserIDVerifiedLocally() {return _PAIDVerifiedLocally;}

   /**
    * Returns information on whether opener is on a different node than the
    * receiver.
    *
    * @return true if the opener is on a different node, false if opener is on
    * the same node as the receiver
    */
   public boolean isRemoteOpener() {return _remoteOpener;}

   /**
    * Returns information on whether open message is from a backup process
    * of a process pair.
    *
    * @return true if the opener is a backup process otherwise returns false 
    */
   public boolean isBackupOpen() {return _backupOpen;}

   /**
    * If this open represents an open by a backup process ({@link #isBackupOpen}
    * returns true) then this returns
    * the ProcessHandle of the primary process.  The ProcessHandle will represent
    * a null phandle (this is not the same as returning a null) if the system
    * message does not represent an open by a backup process.  If the ProcessHandle
    * is a null phandle {@link ProcessHandle#isNull} returns true.
    *
    * @return the ProcessHandle of primary process
    * @see com.tandem.ext.guardian.ProcessHandle 
    */
   public ProcessHandle getPrimaryPhandle() {return _primaryPhandle;}

   /**
    * Returns the qualifier portion of the name used
    * to open the process, in external form (for example, "#PORT2.CTL").
    *
    * @return a String representing the open qualifier used by the opener.  If
    * no qualifier was used an empty string will be returned. 
    */
   public String getOpenQualifier() {return _openQualifier;}

   /**
    * Returns the name of the opener process in external process
    * descriptor form (system, name, and sequence number).
    *
    * @return the name of the opener process or an empty string if the opener
    * is unnamed
    */
   public String getOpenerName() {return _openerName;}

   /**
    * This returns the file number used by the primary when the open message
    * is from a backup process ({@link #isBackupOpen} returns true).  It is typically,
    * but not always, the same as the backup.
    *
    * @return the file number used by the primary process when the open is a 
    * backup open.  When the open is not from a backup process this returns -1.
    */
   public short getPrimaryFileNumber() {return _primaryFileNumber;}

   /**
    * Returns the creator access ID of the opener.
    *
    * @return the creator access ID of the opener
    */
   public short getCreatorAccessID() {return _CAID;}

   /**
    * Returns the opener's home terminal name in external form.
    * An empty string is returned if the opening operating system
    * version is earlier than C10.
    *
    * @return the home terminal name of the opener
    */
   public String getHomeTerminal() {return _homeTerminal;}

   /**
    * Returns the sync depth value the opener used in the call to FILE_OPEN_.
    * The sync depth of the opener specifies whether or not an I/O operations is automatically 
    * redirected to the backup process if the primary process or its processor
    * module fails.  If this parameter is greater than or
    * equal to 1, the server is expected to save or be able to regenerate that 
    * number of replies.  If this parameter is 0, and if an I/O operation cannot be
    * performed to the primary process of a proces spair, an error indication is
    * returned to the originator of the message.  On a subsequent I/O operation,
    * the file system redirects the request to the backup process.
    *
    * @return the sync depth value used in the FILE_OPEN_ call
    */
   public short getSyncDepth() {return _syncDepth;}

   /**
    * Constructs a SysMsgOpen from a byte array.
    * <p>
    * The structure of the open message is expected to be as it is
    * read off $RECEIVE which is:<pre>
    *   word 0 = -103
    *   word 1 = Access mode 
    *   word 2 = Exclusion mode
    *   word 3 = Nowait depth 
    *   word 4 = Sync depth 
    *   word 5 = Open options 
    *   word 6 = User ID of opener (process access ID)
    *   word 7 = Miscellaneous:
    *      .<0:12> = currently undefined; subject to change
    *      .<13> = the opener's user ID has not been verified locally (in the
    *              receiver's node)
    *      .<14> = the opener is on a different node from the receiver
    *      .<15> = backup open: this is an open by a backup
    *   word 8-17 = For a backup open, this contains the process handle of the 
    *      primary process; for a normal open, this is a null process handle
    *   word 18 = The length in bytes of the qualifier name given below
    *   word 19 = The offset in bytes from the beginning of the message to 
    *      the beginning of the opener process name appearing below
    *   word 20 = The length in bytes of the opener process name appearing below
    *   word 21 = For a backup open, the file number used by the primary.
    *   word 22 = The creator access ID of the opener. 
    *   word 23 = The offset in bytes from the beginning of the message to 
    *      the beginning of the opener home terminal name
    *   word 24 = The length in bytes of the opener home terminal name appearing below
    *   word 25-29 = Reserved, subject to change
    *   word 30 FOR * = The qualifier portion of the name used to open the process, 
    *      in external form (for example, "#PORT2.CTL"). The
    *      length of this item is in word 18.  It is zero length if no qualifier was given.
    *   word [] FOR * = For a named opener, this is the process name in external process
    *      descriptor form (system, name, and sequence number).  If the opening
    *      process is unnamed, the length is zero.  The offset and length of the
    *      name are in word 19 and word 20.
    *   word [] FOR * = The opener's home terminal name in external form.  The
    *     offset and length of the name are in word 23 and word 24. </pre>
    *
    * @param msg the system message read off $RECEIVE
    * @exception IllegalArgumentException when the argument does not contain
    *            a valid open system message
    */
   public SysMsgOpen (byte[] msg) {

      if (msg == null) {
         throw new IllegalArgumentException("System message may not be null");
      }

      if (msg.length < 48) {
         throw new IllegalArgumentException(
            "An open system message should be a minimum of 48 bytes long not "
             + msg.length);
      }

      int offset= 0;  // starting offset in byte array
      try {
         short msgNum = DataConversion.BIN16ToJavaShort(msg, offset);
         if (msgNum != ReceiveInfo.SYSMSG_OPEN) {
            throw new IllegalArgumentException(
               "An open system message should have a message number of " 
                 + ReceiveInfo.SYSMSG_OPEN + " not " + msgNum);
         }
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid system message number: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _accessMode = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid access mode in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _exclusionMode = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid exclusion mode in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _nowaitDepth = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid nowait depth in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _syncDepth = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid sync depth in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _openOptions = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid open options in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _PAID = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid process access user ID in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         short misc = DataConversion.BIN16ToJavaShort(msg, offset);
         _PAIDVerifiedLocally = (misc & 0x0004) == 0x0004; // true if bit13 set
         _remoteOpener        = (misc & 0x0002) == 0x0002; // true if bit14 is set
         _backupOpen          = (misc & 0x0001) == 0x0001; // true if bit15 is set
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("The eighth word in the message is not a valid short: " 
                + ex.getMessage());
      }

      offset += 2;
      int len = 10;  // length in words of the phandle of the primary process
      try {
          short[] tempPhandle = new short[len];
         /* for every 2 bytes in the array, construct a short
            and put it in the right place in the array of shorts */
         for (int i=0; i<len; i++) {
            tempPhandle[i] = DataConversion.BIN16ToJavaShort(msg, offset);
            offset += 2;
         }
         _primaryPhandle = new ProcessHandle(tempPhandle);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid primary phandle in message: " 
                + ex.getMessage());
      }

      try {
         short qualLen = DataConversion.BIN16ToJavaShort(msg, offset);
//TBD: instead of using offset+24 could just spell out 60 etc.
// which might make it easier to check if we're going out of bounds
         // Note: this may be an empty string if there was no qualifier used on the open
         _openQualifier = new String(msg, offset+24, qualLen);
      } catch (Exception ex) {  // can get DataConversionException or IndexOutOfBounds
         throw new IllegalArgumentException("Invalid open qualifier in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         short nameOffset = DataConversion.BIN16ToJavaShort(msg, offset);
         offset += 2;
         short nameLen = DataConversion.BIN16ToJavaShort(msg, offset);
         // Note: this may be an empty string if the opener is not running named
         _openerName = new String(msg, nameOffset, nameLen);
      } catch (Exception ex) {  // can get DataConversionException or IndexOutOfBounds
         throw new IllegalArgumentException("Invalid opener process name in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _primaryFileNumber = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid primary file number in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         _CAID = DataConversion.BIN16ToJavaShort(msg, offset);
      } catch (DataConversionException ex) {
         throw new IllegalArgumentException("Invalid creator access ID of the opener in message: " 
                + ex.getMessage());
      }

      offset += 2;
      try {
         short hometermOffset = DataConversion.BIN16ToJavaShort(msg, offset);
         offset += 2;
         short hometermLen = DataConversion.BIN16ToJavaShort(msg, offset);
         // Note: this may be an empty string if the opening operating system version is earlier than C10
         _homeTerminal = new String(msg, hometermOffset, hometermLen);
      } catch (Exception ex) {  // can get DataConversionException or IndexOutOfBounds
         throw new IllegalArgumentException("Invalid home terminals name in message: " 
                + ex.getMessage());
      }

   } /* end ctor */

}  /* end class SysMsgOpen */
