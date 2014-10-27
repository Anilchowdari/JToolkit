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

import java.util.Properties;
import com.tandem.ext.guardian.*;
import com.tandem.ext.util.NativeLoader;
import com.tandem.ext.util.DataConversionException;

/**
 * <P>
 * EnscribeFile provides access to Enscribe files through methods that perform
 * standard Guardian operations.
 * 
 * For more information about Enscribe files, see the Enscribe Programmer's
 * Guide.
 * </P>
 * 
 * @see com.tandem.ext.enscribe.EnscribeFileAttributes
 * @see com.tandem.ext.enscribe.EnscribeOpenOptions
 * @see com.tandem.ext.enscribe.EnscribeCreationAttributes
 */
public class EnscribeFile {
	static {
		  NativeLoader loader = new NativeLoader();
		  loader.LoadLibrary();
		}
	private boolean debugFlag_ = false;
	private String fileName_ = null;
	private String guardianName_ = null;
	private EnscribeFileWarning warn_ = null;

	private short fileNumber_ = -1;

	/**
	 * maxReadCount limits the bytes read from a record. default is record size
	 * for structured files and 4096 for unstructured reads.
	 */
	private int maxReadCount_ = -1;
	private int fileMaxReadCount_ = -1;
	private boolean openedUnstructured_ = false;
	private boolean largeTransfers_ = false;

	private int deviceType_ = -1;
	private int deviceSubtype_ = -1;
	private int fileType_ = -1;
	private int fileCode_ = -1;

	private boolean fileNameIsOSS_ = true;

	private boolean fileIsOpen_ = false;
	private final boolean positionInfoSaved_ = false;

	private final short READ_TYPE_READ = 0;
	private final short READ_TYPE_READLOCK = 1;
	private final short READ_TYPE_READUPDATE = 2;
	private final short READ_TYPE_READUPDATELOCK = 3;

	private final short WRITE_TYPE_WRITE = 0;
	private final short WRITE_TYPE_WRITEUPDATE = 2;
	private final short WRITE_TYPE_WRITEUPDATEUNLOCK = 3;

	private final int FI_ARRAY_SIZE = 6;
	private final int FI_FILENUM_IDX = 0;
	private final int FI_DEVTYPE_IDX = 1;
	private final int FI_DEVSUBTYPE_IDX = 2;
	private final int FI_FILETYPE_IDX = 3;
	private final int FI_FILECODE_IDX = 4;
	private final int FI_RECSIZE_IDX = 5;

	private static final int GUARDIAN_MSG_MAX = 57344;

	/**
	 * Specifies Guardian file security for any local ID.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_A = 0;
	/**
	 * Specifies Guardian file security to allow access to a local member of the
	 * owner's group.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_G = 1;
	/**
	 * Specifies Guardian file security to allow access to a local owner.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_O = 2;
	/**
	 * Specifies Guardian file security to allow access for any ID.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_N = 4;
	/**
	 * Specifies Guardian file security to allow access to a member of the
	 * owner's group (local or remote).
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_C = 5;
	/**
	 * Specifies Guardian file security to allow access to the Owner (local or
	 * remote) only.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_U = 6;
	/**
	 * Specifies Guardian file security to allow access to the local super ID
	 * only.
	 * 
	 * <DL>
	 * For use with setFileSecurity.
	 * </DL>
	 */
	public static final int FILE_SECURITY_SUPER = 7;

	/**
	 * Write end of file on unstructured disk file.
	 * 
	 * <DL>
	 * Usage: control(CONTROL_WRITE_EOF)
	 * <DT>See Guardian CONTROL(filenum, 2).
	 * </DL>
	 */
	public static final int CONTROL_WRITE_EOF = 2;

	/**
	 * Purge data from disk file.
	 * 
	 * <DL>
	 * Usage: control(CONTROL_PURGE_DATA)
	 * <DT>See Guardian CONTROL(filenum, 20).
	 * </DL>
	 */
	public static final int CONTROL_PURGE_DATA = 20;

	/**
	 * Allocate extents for disk file.
	 * 
	 * <DL>
	 * Usage: control(CONTROL_ALLOCATE_EXTENTS, param)
	 * <DT> See Guardian CONTROL(filenum, 21, param).
	 * </DL>
	 */
	public static final int CONTROL_ALLOCATE_EXTENTS = 21;

	/**
	 * Request immediate completion of outstanding i/o on disk file.
	 * 
	 * <DL>
	 * Usage: control(CONTROL_REQUEST_IO_COMPLETION)
	 * <DT>See Guardian CONTROL(filenum, 26).
	 * </DL>
	 */
	public static final int CONTROL_REQUEST_IO_COMPLETION = 26;

	/**
	 * Wait for a DP2 disk write.
	 * 
	 * <DL>
	 * Usage: control(CONTROL_WAIT_FOR_DP2_WRITE)
	 * <DT>See Guardian CONTROL(filenum, 27).
	 * </DL>
	 */
	public static final int CONTROL_WAIT_FOR_DP2_WRITE = 27;

	/**
	 * Position at next unused record.
	 * 
	 * <DL>
	 * Usage: position(POSITION_UNUSED)
	 * <DT>See Guardian FILE_SETPOSITION_(filenum, -1).
	 * </DL>
	 */
	public static final int POSITION_UNUSED = -1;

	/**
	 * Position at end of file.
	 * 
	 * <DL>
	 * Usage: position(POSITION_EOF)
	 * <DT>See Guardian FILE_SETPOSITION_(filenum, -2).
	 * </DL>
	 */
	public static final int POSITION_EOF = -2;

	 /**
	    * Set write verification.
	    *
		* <DL> Usage:  setMode(SM_SET_WRITE_VERIFICATION, param1, param2)
		* <DT> param1.<15> <DD>= 0 verified writes off (default).
		*             <DD>= 1 verified writes on.
		* <DT>param2 <DD>= 0 change the open option setting of the verify writes option
		*            (default).
		*        <DD>= 1 change the file label default value of the verify writes
		*            option.
	        * <DT>See Guardian SETMODE(filenum, 3, param1, param2).
		* </DL>
		*/
		private static final int    SM_SET_WRITE_VERIFICATION = 3;

		/**
		 * Set lock mode.
		 *
		 * <DL> Usage:  setMode(SM_SET_LOCK_MODE, param1)
		 * <DT>param1 <DD>= 0 normal mode (default):
		 *        <DD>= 1 reject mode:
		 *        <DD>= 2 read-through/normal mode:
		 *        <DD>= 3 read-through/reject mode:
		 *        <DD>= 6 read-warn/normal mode:
		 *        <DD>= 7 read-warn/reject mode:
		 * <DT>See Guardian SETMODE(filenum, 4, param1).
		 * </DL>
		 */
		private static final int    SM_SET_LOCK_MODE = 4;

		/**
		 * Set serial writes option.
		 *
		 * <DL> Usage:  setMode(SM_SET_SERIAL_WRITES, param1, param2)
	         * <DT>param1 <DD>= 0 system automatically selects serial or parallel writes (default)
		 *        <DD>= 1 change the file label default value of the serial writes option
		 * <DT>param2 <DD>= 0 change the open option setting of the serial writes option
		 *           (default)
		 *        <DD>= 1 change the file label default value of the serial writes option
		 * <DT> See Guardian SETMODE(filenum, 57, param1, param2).
		 * </DL>
	     *
		 */
		private static final int    SM_SET_SERIAL_WRITES = 57;

		/**
		 * Set buffered option defaults same as CREATE.
	         *
		 * <DL> Usage:  setMode(SM_SET_BUFFERED_AS_CREATE, param1, param2)
	         * <DT>param1 <DD>= 0 buffered
		 *        <DD>= 1 write-through
		 * <DT>param2 <DD>= 0 change the open option setting of the buffered option (default)
		 *        <DD>= 1 change the file label default value of the buffered option
		 * <DT>See Guardian SETMODE(filenum, 90, param1, param2).
		 * </DL>
		 * Only for Guardian (not OSS) files.
		 */
		private static final int    SM_SET_BUFFERED_AS_CREATE = 90;

		/**
		 * Set cache and sequential option.
		 *
		 * <DL> Usage:  setMode(SM_SET_CACHE_SEQ, param1)
	         * <DT>param1 <DD>= 0 system managed (default)
		 *        <DD>= 1 direct I/O, bypass disk cache
		 *        <DD>= 2 random access, LRU-chain buffer
		 *        <DD>= 3 sequential access, reuse buffer
		 * <DT>See Guardian SETMODE(filenum, 91, param1).
		 * </DL>
		 */
		private static final int    SM_SET_CACHE_SEQ = 91;

		/**
		 * Set maximum number of extents for a nonpartitioned file.
		 *
		 * <DL> Usage:  setMode(SM_SET_MAX_EXTENTS, param1)
		 * <DT>param1 = new maximum number of extents value.
	         * <DT> See Guardian SETMODE(filenum, 92, param1).
	         * </DL>
		 * Only for Guardian (not OSS) files.
		 */
		private static final int    SM_SET_MAX_EXTENTS = 92;

		/**
		 * Set buffer length for an unstructured file.
		 *
		 * <DL> Usage:  setMode(SM_SET_BUFFER_LENGTH, param1)
		 * <DT>param1 = new BUFFERSIZE value.
		 * <DT>See Guardian SETMODE(filenum, 93, param1).
		 * </DL>
		 * Only for Guardian (not OSS) files.
		 */
		private static final int    SM_SET_BUFFER_LENGTH = 93;

		/**
		 * Set audit-checkpoint compression option for an Enscribe file.
	     *
		 * <DL> Usage:  setMode(SM_SET_AUDIT_CP_COMPRESSION, param1, param2)
	     * <DT>param1 <DD>= 0 no audit-checkpoint compression (default)
		 *        <DD>= 1 audit-checkpoint compression enabled
		 * <DT>param2 <DD>= 0 change the open option setting of the audit-checkpoint
		 *            compression option (default).
		 *        <DD>= 1 change the file label default value of the audit-checkpoint
		 *            compression option
		 * <DT>See Guardian SETMODE(filenum, 94, param1, param2).
		 * </DL>
		 */
		private static final int    SM_SET_AUDIT_CP_COMPRESSION = 94;

		/**
		 * Flush dirty cache buffers.
	     *
		 * <DL> Usage:  setMode(SM_FLUSH_DIRTY_CACHE)
		 * <DT> See Guardian SETMODE(filenum, 95).
		 * </DL>
		 */
		private static final int    SM_FLUSH_DIRTY_CACHE = 95;

		/**
		 * Set the generic lock key length of a key-sequenced file.
		 *
		 * <DL> Usage:  setMode(SM_SET_GENERIC_LOCK_KEY_LEN, param1)
		 * <DT> param1 = lock key length.
		 * <DT> See Guardian SETMODE(filenum, 123, param1).
		 * </DL>
		 */
		private static final int    SM_SET_GENERIC_LOCK_KEY_LEN = 123;

		/**
		 * Enable/disable large transfers.
		 *
		 * <DL> Usage:  setMode(SM_SET_LARGE_TRANSFERS, param1)
	         * <DT> param1 <DD>= 1 enable large transfers
		 *    <DD>= 0 disable large transfers (default value when file is opened)
		 * <DT> See Guardian SETMODE(filenum, 141, param1).
		 * </DL>
		 */
		private static final int    SM_SET_LARGE_TRANSFERS = 141;

		/**
		 * Queue waiters for disk file write.
		 *
		 * <DL> Usage:  setMode(SM_QUEUE_WAITERS, param1)
	         * <DT>param1 <DD>= 0 disables the effect of param1
		 *        <DD>= 1 causes CONTROL 27 requests to be queued and completed
		 *            one at a time; otherwise all are completed by the first write
		 * <DT> See Guardian SETMODE(filenum, 146, param1).
		 * </DL>
		 */
		private static final int    SM_QUEUE_WAITERS = 146;

		/**
		 * Set alternate key insertion locking.
		 *
		 * <DL> Usage:  setMode(SM_ALT_KEY_INSERT_LOCKING, param1)
	         * <DT>param1 <DD>= 0 no automatic locking (default)
		 *        <DD>= 1 automatic locking
		 * <DT> See Guardian SETMODE(filenum, 149, param1).
		 * </DL>
		 */
		private static final int    SM_ALT_KEY_INSERT_LOCKING = 149;

		/**
		 * Set variable-length audit compression.
		 *
		 * <DL> Usage:  setMode(SM_VARLEN_AUDIT_COMPRESS, param1)
	         * <DT>param1 <DD>= 1 enables variable-length audit compression
		 *        <DD>= 0 disables variable-length audit compression
		 * <DT> See Guardian SETMODE(153, param1).
		 * </DL>
		 */
		private static final int    SM_VARLEN_AUDIT_COMPRESS = 153;


	/**
	 * Set normal locking mode: Any attempt to lock a file, or to read or lock a
	 * record, that is already locked is suspended until the existing lock is
	 * released.
	 */
	public static final int NORMAL_LOCKING_MODE = 0;

	/**
	 * Set reject locking mode: Any attempt to lock a file, or to read or lock a
	 * record, that is already locked through a different transaction identifier
	 * (audited files) or file number (nonaudited files) is rejected with
	 * EnscribeFileException (file/record is locked); no data is returned.
	 */
	public static final int REJECT_LOCKING_MODE = 1;

	/**
	 * Set read-through/normal mode: read() and readUpdate() requests ignore
	 * existing record and file locks; encountering a lock does not delay or
	 * prevent reading the record. lockFile(), lockRecord, readLock(), and
	 * readUpdateLock() are treated as if you had specified NORMAL_LOCKING_MODE.
	 */
	public static final int READ_THROUGH_NORMAL_LOCKING_MODE = 2;

	/**
	 * Set read-through/reject mode: read() and readUpdate() requests ignore
	 * existing record and file locks; encountering a lock does not delay or
	 * prevent reading the record. lockfile(), lockRecord(), readLock(), and
	 * readUpdateLock() are treated as in REJECT_LOCKING_MODE.
	 */
	public static final int READ_THROUGH_REJECT_LOCKING_MODE = 3;

	/**
	 * Sets read-warn/normal mode: read() and readUpdate() requests ignore
	 * existing record and file locks; although an existing lock will not delay
	 * or prevent reading the record, it will cause an EnscribeFileWarning to be
	 * created with a warning code of 9. LockFile(), lockRecord(), readlock(),
	 * and readUpdateLock() are treated as in NORMAL_LOCKING_MODE.
	 */
	public static final int READ_WARN_NORMAL_LOCKING_MODE = 6;

	/**
	 * Sets read-warn/reject mode: read() and readUpdate() requests ignore
	 * existing record and file locks; although an existing lock will not delay
	 * or prevent reading the record, it will cause an EnscribeFileWarning to be
	 * created with a warning code of 9. LockFile(), lockRecord(), readLock(),
	 * and readUpdateLock() are treated as in REJECT_LOCKING_MODE.
	 */
	public static final int READ_WARN_REJECT_LOCKING_MODE = 7;

	// ----------------------------------------------------------

	/**
	 * Constructs an EnscribeFile object for the named file.
	 * 
	 * @param fileName
	 *            the physical name of the Guardian file in OSS or Guardian
	 *            format; if you are using this object to create a temporary
	 *            file, fileName should only be the name of the disk volume
	 *            where the temporary file will be created.
	 */
	public EnscribeFile(String fileName) {
		if (fileName == null)
			throw new IllegalArgumentException(
					"EnscribeFileConstructor::fileName cannot be null");
		if (fileName.startsWith("/"))
			fileNameIsOSS_ = true;
		else
			fileNameIsOSS_ = false;
		fileName_ = fileName;
	} // EnscribeFile

	// ----------------------------------------------------------
	/**
	 * Changes certain attributes of a disk file that are normally set when the
	 * file is created. The file must NOT be open. Refer to FILE_ALTERLIST_ in
	 * the Guardian Procedure Calls Manual for more information about altering a
	 * file.
	 * 
	 * @param alterListAttributes
	 *            an EnscribeFileAttributes object.
	 * @param partOnly
	 *            true if altering only this partition, false if all partitions
	 *            are to be altered
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is open.
	 * @exception java.lang.IllegalArgumentException
	 *                if an invalid parameter is specified.
	 */
	public void alter(EnscribeFileAttributes alterAttributes, boolean partOnly)
			throws java.lang.IllegalArgumentException, EnscribeFileException {
		short sPartOnly = 0, error = 0;
		if (fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to alter file because the file is open.",
					EnscribeFileException.FEINUSE, "alter", fileName_);
		if (alterAttributes == null)
			throw new IllegalArgumentException(
					"alterAttributes cannot be null.");
		if (alterAttributes.updatedAltKeyAlterAttr_ != null && partOnly == true)
			throw new IllegalArgumentException(
					"Invalid parameter specified.  You can not try to alter alternate key attributes if partonly is set to true.");
		if (fileNameIsOSS_) {
			if (guardianName_ == null) {
				error = GgetGuardianName(fileName_, fileNameIsOSS_);
				if (error != 0)
					throw new EnscribeFileException(
							"Error changing file name from OSS to Guardian format.",
							error, "alter", fileName_);
			}
		} else {
			guardianName_ = fileName_;
		}
		String alterOptString = alterAttributes.getAttributeString();
		if (partOnly)
			sPartOnly = 1;
		error = GfileAlter(guardianName_, sPartOnly, alterOptString);
		if (error != 0 && error != EnscribeFileException.FEKEYFAIL)
			throw new EnscribeFileException("Unable to alter file.", error,
					"alter", fileName_);

	} // alter

	/**
	 * Perform an I/O operation Refer to CONTROL in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                an error occurs or if the file is not open.
	 * @see #purgeData
	 * @see #writeEOF
	 * @see #waitForDP2Writes
	 */
	public void control(int operation) throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to execute control() because the file is not open.",
					EnscribeFileException.FENOOPEN, "control", fileName_);
		error = Gcontrol(fileNumber_, (short) operation, (short) 0);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe control.",
					error, "control", fileName_);
	} // control

	/**
	 * Perform an I/O operation Refer to CONTROL in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * Method for CONTROL operations that require a parameter.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @see #allocateExtents
	 * @see #deallocateExtents
	 */
	public void control(int operation, int param) throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to execute control() because the file is not open.",
					EnscribeFileException.FENOOPEN, "control", fileName_);
		error = Gcontrol(fileNumber_, (short) operation, (short) param);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe control.",
					error, "control", fileName_);

	} // control

	/**
	 * Purges the data from this file. Refer to CONTROL in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void purgeData() throws EnscribeFileException {
		control(CONTROL_PURGE_DATA);
		return;
	} // purgeData

	/**
	 * Writes an end of file (EOF) on an unstructured file. The EOF points at
	 * the relative byte address indicated by the next record pointer. Refer to
	 * CONTROL in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs, if the file is not open, or if the
	 *                file is not an unstructured file.
	 */
	public void writeEOF() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to writeEOF() because the file is not open.",
					EnscribeFileException.FENOOPEN, "writeEOF", fileName_);
		if (fileType_ != EnscribeFileAttributes.FILETYPE_UNSTRUCTURED)
			throw new EnscribeFileException(
					"Unable to writeEOF because this is not an unstructured file.",
					EnscribeFileException.FEINVALOP, "writeEOF", fileName_);
		control(CONTROL_WRITE_EOF);
		return;
	} // writeEOF

	/**
	 * Deallocates all extents past the end-of-file extent for this Enscribe
	 * file. Refer to CONTROL in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void deallocateExtents() throws EnscribeFileException {
		control(CONTROL_ALLOCATE_EXTENTS, 0);
		return;
	} // deallocateExtents

	/**
	 * Allocates extents for this file. Refer to CONTROL in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @param numberOfExtents
	 *            specify the number of extents for a nonpartitioned file; for a
	 *            partitioned file this value will be multipled by 16.
	 * @param isPartitioned
	 *            true if this file is a partition; false if not.
	 * @exception EnscribeFileException
	 *                if an error occurs of if the file is not open.
	 */
	public void allocateExtents(int numberOfExtents, boolean isPartition)
			throws EnscribeFileException {
		int num;
		if (numberOfExtents == 0)
			throw new IllegalArgumentException("Number of extents cannot be 0");
		if (isPartition) {
			EnscribeFileAttributes efa = this.getFileInfo();
			if (efa.isSecondaryPartition())
				num = numberOfExtents * 16;
			else
				throw new EnscribeFileException(
						"isPartition is true; but this is not a secondary partition",
						EnscribeFileException.FEINVALOP, "allocateExtents",
						fileName_);
		} else {
			num = numberOfExtents;
		}
		control(CONTROL_ALLOCATE_EXTENTS, num);
		return;
	} // allocateExtents

	/**
	 * Specifies that the thread should block until DP2 writes data to this
	 * file. Invoking this method will cause the thread to block until a WRITE,
	 * WRITEUPDATE, or WRITEUPDATEUNLOCK occurs on the DP2 disk file. This
	 * method is not valid for partitioned files. When this method returns, do
	 * not assume that the file contains any new data; assume only that it is
	 * time to check the file for new data. This method also returns when a
	 * WRITE or WRITEUPDATE occurs as part of a logical undo of a transaction by
	 * the backout process or when a volume goes down. Refer to CONTROL in the
	 * Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void waitForDP2Writes() throws EnscribeFileException {
		control(CONTROL_WAIT_FOR_DP2_WRITE);
		return;
	} // waitForDP2Writes

	/**
	 * Cancels an incomplete I/O operation on the file associated with this
	 * EnscribeFile object. The canceled operation might or might not have had
	 * effects. The file position might or might not be changed.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs cancelling the last IO operation.
	 */
	public void cancelLastOperation() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to cancelLastOperation() because the file is not open.",
					EnscribeFileException.FENOOPEN, "cancelLastOperation",
					fileName_);
		short error = GcancelReq(fileNumber_);
		if (error != 0)
			throw new EnscribeFileException(
					"Error occurred during CancelLastOperation.", error,
					"cancelLastOperation", fileName_);
	}

	/**
	 * Deletes the contents of the record at the current position for
	 * key-sequenced, relative, and entry-sequenced files. Refer to WRITEUPDATE
	 * in the Guardian Procedure Calls Manual for more information.
	 * 
	 * deleteRecord() typically follows either the read() or readUpdate()
	 * method.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void deleteRecord() throws EnscribeFileException {
		short error;
		int[] countWritten = new int[2];
		byte[] buffer = new byte[2];
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to delete record because the file is not open.",
					EnscribeFileException.FENOOPEN, "deleteRecord", fileName_);
		error = Gwrite((short) WRITE_TYPE_WRITEUPDATE, (short) fileNumber_,
				buffer, (short) 0, countWritten);
		if (error != 0)
			throw new EnscribeFileException("Record delete error.", error,
					"deleteRecord", fileName_);
	} // deleteRecord

	/**
	 * Deletes and unlocks the contents of the record at the current position
	 * for key-sequenced, relative, and entry-sequenced files. Refer to
	 * WRITEUPDATEUNLOCK in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * deleteRecordUnlock() typically follows either the read() or readUpdate()
	 * method.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void deleteRecordUnlock() throws EnscribeFileException {
		short error;
		int[] countWritten = new int[2];
		byte[] buffer = new byte[2];
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to delete record because the file is not open.",
					EnscribeFileException.FENOOPEN, "deleteRecordUnlock",
					fileName_);
		error = Gwrite((short) WRITE_TYPE_WRITEUPDATEUNLOCK,
				(short) fileNumber_, buffer, (short) 0, countWritten);
		if (error != 0)
			throw new EnscribeFileException("Record delete error.", error,
					"deleteRecordUnlock", fileName_);
	} // deleteRecordUnlock

	/**
	 * Closes an open file. Refer to FILE_CLOSE_ in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * No exception is thrown. If the file is already closed, close does
	 * nothing.
	 */
	public void close() {
		if (fileIsOpen_) {
			GfileClose(fileNumber_);
			fileIsOpen_ = false;
		}
	} // close

	/**
	 * Creates a new structured or unstructured disk file. Refer to
	 * FILE_CREATELIST_ in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param creationAttributes
	 *            an EnscribeCreationAttributes object.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @exception java.lang.IllegalArgumentException
	 *                if a parameter was not set correctly in creation
	 *                Attributes.
	 */
	public void create(EnscribeCreationAttributes creationAttributes)
			throws EnscribeFileException, java.lang.IllegalArgumentException {
		short error;
		String creationOptString = null;
		if (creationAttributes == null)
			throw new IllegalArgumentException(
					"creationAttributes cannot be null.");
		if (fileNameIsOSS_) {
			if (guardianName_ == null) {
				error = GgetGuardianName(fileName_, fileNameIsOSS_);
				if (error != 0)
					throw new EnscribeFileException(
							"Error changing file name from OSS to Guardian format.",
							error, "create", fileName_);
			}
		}
		if (creationAttributes.fileTypeSet_ == false)
			throw new IllegalArgumentException(
					"File type was not set in the EnscribeCreationAttributes object.");
		if (creationAttributes.fileType_ == EnscribeCreationAttributes.FILETYPE_KEY_SEQUENCED)
			creationOptString = creationAttributes.getAttributeString(false);
		else
			creationOptString = creationAttributes.getAttributeString(true);

		error = GfileCreate(fileName_, creationOptString);
		if (error != 0)
			throw new EnscribeFileException("Unable to create file.", error,
					"create", fileName_);

	} // create

	/**
	 * Creates any alternate key files associated with the file this object
	 * represents. Refer to FILE_CREATELIST_ in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs while trying to create the alternate
	 *                key files.
	 * @exception java.lang.IllegalArgumentException
	 *                if no alternate keys were defined for the file this object
	 *                represents.
	 */
	public void createAltKeyFiles() throws EnscribeFileException,
			java.lang.IllegalArgumentException {
		EnscribeFileAttributes efa = this.getFileInfo();
		int numAltKeyFiles = efa.getNumberOfAltkeyfiles();
		if (numAltKeyFiles == 0)
			throw new IllegalArgumentException(
					"No alternate key files have been defined for this file");
		KeySequencedCreationAttributes ca = new KeySequencedCreationAttributes();
		// Set default creation attributes like this file
		ca.setAudited(efa.isAudited());
		ca.setFileCode(efa.getFileCode());
		ca.setMaximumExtents(efa.getMaximumExtents());
		ca.setPrimaryExtentSize(efa.getPrimaryExtentSize());
		ca.setSecondaryExtentSize(efa.getSecondaryExtentSize());
		ca.setBlockLength(efa.getBlockLength());
		int format = efa.getFormat();
		ca.setFormat(format);
		int fileType = efa.getFileType();
		int keylen = 0;
		if (fileType == EnscribeFileAttributes.FILETYPE_KEY_SEQUENCED) {
			keylen = efa.getKeyLength();
		} else {
			if (format == EnscribeFileAttributes.FILEFORMAT_1)
				keylen = 4;
			else
				keylen = 8;
		}
		for (int i = 0; i < numAltKeyFiles; i++) {
			EnscribeFile altfile = new EnscribeFile(efa.altKeyFileInfo_[i]);
			int longestKeyLen = 0;
			for (int n = 0; n < efa.altKeyInfo_.length; n++) {
				if (efa.altKeyInfo_[n].getKeyFileNum() == i) {
					int tempKeyLen = efa.altKeyInfo_[n].getKeyLength();
					if (tempKeyLen > longestKeyLen)
						longestKeyLen = tempKeyLen;
				}
			}
			keylen = keylen + longestKeyLen + 2;
			ca.setKeyLength(keylen);
			ca.setRecordLength(keylen);
			ca.setKeyOffset(0);
			altfile.create(ca);
		}
	}

	/**
	 * Creates a new temporary structured or unstructured disk file; the
	 * fileName specified when this EnscribeFile object was created must only be
	 * the name of the disk volume where this temporary file will be created.
	 * Refer to FILE_CREATE_ in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param creationAttributes
	 *            an EnscribeCreationAttributes object.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void createTemporaryFile(
			EnscribeCreationAttributes creationAttributes)
			throws EnscribeFileException {
		/* Usual PATHNAME_TO_etc. */
		String creationOptString = null;
		short error;
		if (creationAttributes == null)
			throw new IllegalArgumentException(
					"creationAttributes cannot be null.");
		if (fileNameIsOSS_) {
			if (guardianName_ == null) {
				error = GgetGuardianName(fileName_, fileNameIsOSS_);
				if (error != 0)
					throw new EnscribeFileException(
							"Error changing file name from OSS to Guardian format.",
							error, "createTemporaryFile", fileName_);
			}
		}
		if (creationAttributes.fileType_ == EnscribeCreationAttributes.FILETYPE_KEY_SEQUENCED)
			creationOptString = creationAttributes.getAttributeString(false);
		else
			creationOptString = creationAttributes.getAttributeString(true);

		if (debugFlag_)
			System.out.println("createTemporaryFile: " + creationOptString);
		short[] errorArray = new short[2];
		String newFileName = GfileCreateTemp(fileName_, creationOptString,
				errorArray);
		if (errorArray[0] != 0)
			throw new EnscribeFileException("Unable to create temporary file.",
					errorArray[0], "createTemporaryFile", fileName_);
		fileName_ = newFileName;

	} // createTemporaryFile

	/**
	 * Determines whether a file exists.
	 * 
	 * @return true if file exists
	 * 
	 */
	public boolean isExistingFile() {
		short error = GfileExists(fileName_, fileNameIsOSS_);
		if (error != 0 && debugFlag_)
			System.out.println("fileExists returned error " + error);
		return (error == 0);

	} // fileExists

	/**
	 * Opens a structured or unstructured disk file. Refer to FILE_OPEN_ in the
	 * Guardian Procedure Calls Manual for more information.
	 * 
	 * @param openOptions
	 *            an EnscribeOpenOptions object.
	 * @exception EnscribeFileException
	 *                if an error occurs, if if the file is open, or if the
	 *                filename used to create this EnscribeFile object is
	 *                invalid.
	 */
	public void open(EnscribeOpenOptions openOptions)
			throws EnscribeFileException {
		if (fileIsOpen_)
			throw new EnscribeFileException("File is already open.",
					EnscribeFileException.FEINUSE, "open", fileName_);
		if (!isValidFileName(fileName_))
			throw new EnscribeFileException(
					"File can not be opened because the filename is invalid.",
					EnscribeFileException.FEBADFORM, "open", fileName_);
		if (openOptions == null)
			throw new IllegalArgumentException("openOptions cannot be null.");
		short error;
		short[] fileInfo = new short[FI_ARRAY_SIZE];
		openedUnstructured_ = openOptions.getUnstructuredAccess();
		error = GfileOpen(fileName_, fileNameIsOSS_, fileInfo,
				(short) openOptions.getAccess(), (short) openOptions
						.getExclusion(), (short) openOptions.getSyncDepth(),
				(short) openOptions.getSequentialBlockBufferLength(),
				openOptions.getUnstructuredAccess(), openOptions
						.getNoOpenTimeUpdate(), openOptions
						.getUse64BitPrimaryKeyValues());
		if (error != 0)
			throw new EnscribeFileException("File can not be opened.", error,
					"open", fileName_);
		// file is open and positioned at the beginning of the file

		if (debugFlag_)
			System.out.println("Open fileInfo " + fileInfo[0] + " "
					+ fileInfo[1] + " " + fileInfo[2] + " " + fileInfo[3] + " "
					+ fileInfo[4] + " " + fileInfo[5]);

		fileNumber_ = fileInfo[FI_FILENUM_IDX];
		deviceType_ = fileInfo[FI_DEVTYPE_IDX];
		deviceSubtype_ = fileInfo[FI_DEVSUBTYPE_IDX];
		fileType_ = fileInfo[FI_FILETYPE_IDX];
		fileCode_ = fileInfo[FI_FILECODE_IDX];
		fileMaxReadCount_ = (int) fileInfo[FI_RECSIZE_IDX];
		fileIsOpen_ = true;
		maxReadCount_ = fileMaxReadCount_;
	} // open

	/**
	 * Opens a structured or unstructured disk file. This method opens a file
	 * specifying only the access and exclusion options. If you want to set
	 * other options when opening a file, use the open method that requires an
	 * EnscribeOpenOptions object. Refer to FILE_OPEN_ in the Guardian Procedure
	 * Calls Manual for more information.
	 * 
	 * @param access
	 *            one of:
	 *            <DL>
	 *            <DD>EnscribeOpenOptions.READ_WRITE for both read and write
	 *            access
	 *            <DD>EnscribeOpenOptions.READ_ONLY for read access
	 *            <DD>EnscribeOpenOptions.WRITE_ONLY for write access
	 *            </DL>
	 * @param exclusion
	 *            one of:
	 *            <DL>
	 *            <DD>EnscribeOpenOptions.SHARED to allow access to others
	 *            <DD>EnscribeOpenOptions.EXCLUSIVE to prohibit access to
	 *            others
	 *            <DD>EnscribeOpenOptions.PROTECTED to allow read access to
	 *            others
	 *            </DL>
	 * @exception EnscribeFileException
	 *                if an error occurs or if an invalid file name was used to
	 *                construct this EnscribeFile object.
	 */
	public void open(int access, int exclusion) throws EnscribeFileException {

		if (fileIsOpen_)
			throw new EnscribeFileException("File is already open.",
					EnscribeFileException.FEINUSE, "open", fileName_);
		if (!isValidFileName(fileName_))
			throw new EnscribeFileException(
					"File can not be opened because the filename is invalid.",
					EnscribeFileException.FEBADFORM, "open", fileName_);
		short error;
		short[] fileInfo = new short[FI_ARRAY_SIZE];
		openedUnstructured_ = false;
		warn_ = null;
		error = GfileOpen(fileName_, fileNameIsOSS_, fileInfo, (short) access,
				(short) exclusion, (short) 0, (short) 0, false, false, false);
		if (error != 0) {
			switch (error) {
			case EnscribeFileException.FEKEYFAIL:
				warn_ = new EnscribeFileWarning(
						"An open for an alternate key file failed.", error,
						"open", fileName_);
				break;
			case EnscribeFileException.FEPARTFAIL:
				warn_ = new EnscribeFileWarning(
						"An open or a purge of a partition failed.", error,
						"open", fileName_);
				break;
			default:
				throw new EnscribeFileException("File can not be opened.",
						error, "open", fileName_);
				// file is open and positioned at the beginning of the file
			}
		}

		if (debugFlag_)
			System.out.println("Open fileInfo " + fileInfo[0] + " "
					+ fileInfo[1] + " " + fileInfo[2] + " " + fileInfo[3] + " "
					+ fileInfo[4] + " " + fileInfo[5]);

		fileNumber_ = fileInfo[FI_FILENUM_IDX];
		deviceType_ = fileInfo[FI_DEVTYPE_IDX];
		deviceSubtype_ = fileInfo[FI_DEVSUBTYPE_IDX];
		fileType_ = fileInfo[FI_FILETYPE_IDX];
		fileCode_ = fileInfo[FI_FILECODE_IDX];
		fileMaxReadCount_ = (int) fileInfo[FI_RECSIZE_IDX];
		fileIsOpen_ = true;
		maxReadCount_ = fileMaxReadCount_;
	} // open

	/**
	 * Returns the EnscribeFileWarning for the last completed operation.
	 * 
	 * @return EnscribeFileWarning if a warning occurred; null otherwise.
	 */
	public EnscribeFileWarning getWarning() {
		return warn_;
	}

	/**
	 * Indicates whether a warning occurred on the last completed Guardian
	 * operation.
	 * 
	 * @return true if a warning occurred; false otherwise.
	 * @see #getWarning
	 */
	public boolean warningOnLastOperation() {
		if (warn_ == null)
			return false;
		else
			return true;
	}

	/**
	 * Deletes a disk file. The file must NOT be open. Refer to FILE_PURGE_ in
	 * the Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is open.
	 */
	public void purge() throws EnscribeFileException {
		short error;
		if (fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to purge file because the file is open.",
					EnscribeFileException.FEINUSE, "purge", fileName_);
		warn_ = null;
		error = GfilePurge(fileName_, fileNameIsOSS_);
		if (error != 0 && error < 10) {
			warn_ = new EnscribeFileWarning(
					"Warning occurred while purging file.", error, "purge",
					fileName_);
			return;
		}
		if (error != 0) {
			throw new EnscribeFileException("File could not be purged.", error,
					"purge", fileName_);
		}

	} // filePurge

	/**
	 * Renames a Gaurdian disk file. The format of newFileName must be Guardian
	 * format). Refer to FILE_RENAME_ in the Guardian Procedure Calls Manual for
	 * more information.
	 * 
	 * @param newFileName
	 *            a String with the new name of the file.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is open or if
	 *                newFileName is not a valid file name.
	 */
	public void rename(String newFileName) throws EnscribeFileException {
		short error;
		if (newFileName == null)
			throw new IllegalArgumentException("newFileName cannot be null.");
		if (fileNameIsOSS_) {
			if (guardianName_ == null) {
				error = GgetGuardianName(fileName_, fileNameIsOSS_);
				if (error != 0)
					throw new EnscribeFileException(
							"Error changing file name from OSS to Guardian format.",
							error, "create", fileName_);

			}
		}
		if (fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to rename file because the file is open.",
					EnscribeFileException.FEINUSE, "rename", fileName_);
		if (!isValidFileName(newFileName))
			throw new EnscribeFileException(
					"File can not be renamed because the new filename is invalid.",
					EnscribeFileException.FEBADFORM, "rename", fileName_);

		error = GfileRename(fileName_, newFileName, fileNameIsOSS_);
		if (error != 0)
			throw new EnscribeFileException("File can not be renamed.", error,
					"rename", fileName_);
		fileName_ = newFileName;
	} // fileRename

	/**
	 * Returns the file code.
	 * 
	 * @return file code
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public int getFileCode() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to getFileCode because the file is not open.",
					EnscribeFileException.FENOOPEN, "getFileCode", fileName_);
		return (fileCode_);
	} // getFileCode

	/**
	 * Returns the file type.
	 * 
	 * @return file type; which is one of:
	 *         <DL>
	 *         <DD>0 unstructured
	 *         <DD>1 relative
	 *         <DD>2 entry-sequenced
	 *         <DD>3 key-sequenced
	 *         </DL>
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * 
	 * @see EnscribeFileAttributes#FILETYPE_UNSTRUCTURED
	 * @see EnscribeFileAttributes#FILETYPE_RELATIVE
	 * @see EnscribeFileAttributes#FILETYPE_ENTRY_SEQUENCED
	 * @see EnscribeFileAttributes#FILETYPE_KEY_SEQUENCED
	 */
	public int getFileType() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to getFileCode because the file is not open.",
					EnscribeFileException.FENOOPEN, "getFileType", fileName_);
		return (fileType_);
	} // getFileType

	/**
	 * Returns the device type. See the Guardian Procedure Calls Reference
	 * Manual, Appendix A.
	 * 
	 * @return device type
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public int getDeviceType() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to getDeviceCode because the file is not open.",
					EnscribeFileException.FENOOPEN, "getDeviceType", fileName_);
		return (deviceType_);
	} // getDeviceType

	/**
	 * Returns the file name. The file name is returned in OSS or Guardian
	 * format depending on how it was set.
	 * 
	 * @return file name
	 * 
	 */
	public String getFileName() {
		return (fileName_);
	} // getFileName

	/**
	 * Returns the file number if the file is open. If the open method has not
	 * been invoked for this object, returns -1.
	 * 
	 * @return file number
	 */
	int getFileNumber() {
		return (fileNumber_);
	} // getFileNumber

	/**
	 * Returns the maximum number of bytes that can be read with this
	 * EnscribeFile object.
	 * 
	 * @return maximum number of bytes that can be read
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * 
	 */
	public int getMaxReadCount() throws EnscribeFileException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to getMaxReadCount because the file is not open.",
					EnscribeFileException.FENOOPEN, "getMaxReadCount",
					fileName_);
		return (maxReadCount_);
	} // getMaxReadCount

	/**
	 * Returns true if the file associated with this EnscribeFile object is
	 * open.
	 * 
	 * @return true if open
	 * 
	 */
	public boolean isFileOpen() {
		return fileIsOpen_;
	} // isFileOpen

	/**
	 * Indicates whether the fileName associated with this Enscribe object is in
	 * OSS format.
	 * 
	 * @return true if fileName is in OSS format
	 * 
	 */
	public boolean isFileNameOSS() {
		return fileNameIsOSS_;
	} // isFileNameOSS

	/**
	 * Indicates whether the fileName associated with this Enscribe object is
	 * valid.
	 * 
	 * @return true if fileName is valid
	 * 
	 * @param fileName
	 *            a String with the file name to validate
	 */
	public static boolean isValidFileName(String fileName) {
		String fName = fileName;
		if (fileName == null)
			throw new IllegalArgumentException("fileName cannot be null");
		fName.trim();
		if (fName.length() < 1) {
			return false;
		}
		// may want to do some additional checking
		return true;
	} // isValidFileName

	/**
	 * Prohibits other users from accessing the file (and any records within
	 * that file) associated with this EnscribeFile object. Refer to LOCKFILE in
	 * the Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void lockFile() throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to lockFile because the file is not open.",
					EnscribeFileException.FENOOPEN, "lockFile", fileName_);
		error = Glockfile(fileNumber_);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe lockfile.",
					error, "lockFile", fileName_);

	} // lockFile

	/**
	 * Prohibits other users from accessing a record at the current position.
	 * For more information about use with relative, entry-sequenced, and
	 * unstructured files.
	 * 
	 * See LOCKREC in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void lockRecord() throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to lockRecord because the file is not open.",
					EnscribeFileException.FENOOPEN, "lockRecord", fileName_);
		error = Glockrec(fileNumber_);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe lockRecord.",
					error, "lockRecord", fileName_);
	} // lockRecord

	private static int BYTE_ARRAY_PARAM = 0;
	private static int INPUT_RECORD_PARAM = 1;

	private void checkReadInputParams(int maxRead, int bufLen,
			int readParamType, boolean checkLargeTransfers, String method)
			throws EnscribeFileException {

		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to read because the file is not open.",
					EnscribeFileException.FENOOPEN, method, fileName_);

		if (maxRead > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"Attempt to read more than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, method, fileName_);
		if (checkLargeTransfers) {
			if (largeTransfers_) {
				if (!multiple2K(maxRead)) {
					throw new EnscribeFileException(
							"The number of bytes to be read(maxRead or inputRecord.getLength()) must be a multiple of 2048 bytes when doing large transfers.",
							EnscribeFileException.FEBADCOUNT, method, fileName_);
				}
			}
		}
		if (readParamType == BYTE_ARRAY_PARAM) {
			if (maxRead > bufLen)
				throw new EnscribeFileException(
						"Unable to read because maxRead is greater than buf length.",
						EnscribeFileException.FEBADCOUNT, method, fileName_);
		}
	}

	private void checkReadError(short error, String method)
			throws EnscribeFileException {
		switch (error) {
		case EnscribeFileWarning.FEINVALOP:
		case EnscribeFileWarning.FEPARTFAIL:
		case EnscribeFileWarning.FEKEYFAIL:
			warn_ = new EnscribeFileWarning("Warning occurred during read",
					error, method, fileName_);
			break;
		case EnscribeFileWarning.FESEQFAIL:
			warn_ = new EnscribeFileWarning(
					"Sequential block buffering cannot be used during read.",
					error, method, fileName_);
			break;
		case EnscribeFileWarning.FESYSMESS:
		case EnscribeFileWarning.FENOSYSMESS:
			break;
		case EnscribeFileWarning.FEREADLOCK:
			warn_ = new EnscribeFileWarning(
					"A read through lock was successful.", error, method,
					fileName_);
			break;
		case 551:
			warn_ = new EnscribeFileWarning(
					"The read was successful and the current alternate key value is the same as the next alternate key in the path.",
					error, method, fileName_);
		default:
			throw new EnscribeFileException("File read error.", error, method,
					fileName_);
		}
	}

	/**
	 * Sequentially reads records from the file associated with this
	 * EnscribeFile object into a byte array. For key-sequenced, relative, and
	 * entry-sequenced files, reads a subset of records in the file based on the
	 * access path, positioning mode, and comparison length determined by either
	 * the keyPosition or position methods. For unstructured files, data
	 * transfer begins at the position indicated by the next-record pointer.
	 * 
	 * Refer to READ in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param buf -
	 *            the destination buffer.
	 * @param maxRead -
	 *            maximum number of bytes to read.
	 * 
	 * @return the number of bytes read. Returns -1 if end-of-file is reached.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the read operation, if
	 *                the file is not open, if the value of maxRead is greater
	 *                than 57344, if maxRead is not a multiple of 2048 bytes and
	 *                you are doing large transfers, or if maxRead is greater
	 *                than buf.length.
	 * @see #enableLargeTransfers
	 */
	public int read(byte[] buf, int maxRead) throws EnscribeFileException,
			IllegalArgumentException {
		short error;
		int[] countRead = new int[1];

		checkReadInputParams(maxRead, buf.length, 0, true, "read");

		warn_ = null;
		error = Gread(READ_TYPE_READ, fileNumber_, buf, maxRead, countRead);

		if (error != 0) {
			if (error == EnscribeFileWarning.FEEOF)
				countRead[0] = -1;
			else
				checkReadError(error, "read");
		}
		return countRead[0];
	} // read

	/**
	 * Sequentially reads records from the file associated with this
	 * EnscribeFile object into an object that implements the GuardianInput
	 * interface. For key-sequenced, relative, and entry-sequenced files, reads
	 * a subset of records in the file based on the access path, positioning
	 * mode, and comparison length determined by either the keyPosition or
	 * position methods. For unstructured files, data transfer begins at the
	 * position indicated by the next-record pointer. This method calls the
	 * unmarshal method of the object that implements GuardianInput after the
	 * read completes. Refer to READ in the Guardian Procedure Calls Manual for
	 * more information about the read method.
	 * 
	 * @param inputRecord
	 *            an object that implements the GuardianInput interface.
	 * 
	 * @return the number of bytes read. Returns -1 if end-of-file reached.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the read operation, if
	 *                the file is not open, or if the value returned by
	 *                inputRecord.getLength() is greater than 57344.
	 * @exception DataConversionException
	 *                if an error occurs when the unmarshal method of
	 *                inputRecord is invoked.
	 * @see #enableLargeTransfers
	 * @see GuardianInput
	 */
	public int read(GuardianInput inputRecord) throws EnscribeFileException,
			DataConversionException {

		int countRead = 0;
		if (inputRecord == null)
			throw new IllegalArgumentException("inputRecord cannot be null.");
		int maxRead = inputRecord.getLength();

		if (maxRead > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The value returned by inputRecord.getLength() is greater than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "read", fileName_);

		byte[] readBuf = new byte[maxRead];
		countRead = read(readBuf, maxRead);
		if (countRead != -1)
			inputRecord.unmarshal(readBuf, countRead);
		return countRead;
	} // read

	/**
	 * Sequentially locks and reads records into a byte array. Invoking this
	 * method is the same a invoking a combination of the lockRecord and read
	 * methods. For key-sequenced, relative, and entry-sequenced files, reads a
	 * subset of records in the file based on the access path, positioning mode,
	 * and comparison length determined by either the keyPosition or position
	 * methods. For unstructured files, data transfer begins at the position
	 * indicated by the next-record pointer. Refer to READLOCK in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @param buf
	 *            the destination buffer
	 * @param maxRead
	 *            the maximum number of bytes to read.
	 * @return the number of bytes read. Returns -1 if end-of-file reached.
	 * @exception EnscribeFileException
	 *                if a Guardian error occursd during the readLock operation
	 *                or if the file is not open, if you are doing large
	 *                transfers, if maxRead is greater than buf.length, or if
	 *                maxRead is greater than the 57344.
	 * @see #enableLargeTransfers
	 */
	public int readLock(byte[] buf, int maxRead) throws EnscribeFileException,
			IllegalArgumentException {
		short error;
		int[] countRead = new int[1];

		checkReadInputParams(maxRead, buf.length, 0, false, "readLock");

		if (largeTransfers_)
			throw new EnscribeFileException(
					"Unable to readLock when doing large transfers.",
					EnscribeFileException.FEBADCOUNT, "readLock", fileName_);

		warn_ = null;
		error = Gread(READ_TYPE_READLOCK, fileNumber_, buf, maxReadCount_,
				countRead);
		if (error != 0) {
			if (error == EnscribeFileException.FEEOF)
				countRead[0] = -1;
			else
				checkReadError(error, "readLock");
		}
		return countRead[0];
	} // readLock

	/**
	 * Sequentially locks and reads records into an object that implements
	 * GuardianInput. Invoking this method is the same a invoking a combination
	 * of the lockRecord and read methods. For key-sequenced, relative, and
	 * entry-sequenced files, reads a subset of records in the file based on the
	 * access path, positioning mode, and comparison length determined by either
	 * the keyPosition or position methods. For unstructured files, data
	 * transfer begins at the position indicated by the next-record pointer.
	 * This method calls the unmarshal method of the object that implements
	 * GuardianInput after the read completes. Refer to READLOCK in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @param inputRecord
	 *            an object that implements the GuardianInput interface.
	 * 
	 * @return the number of bytes read. Returns -1 if end-of-file reached.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the readLock operation,
	 *                if the value returned from inputRecord.getLength is
	 *                greater than 57344, or if the file is not open or you are
	 *                doing large transfers
	 * @exception DataConversionException
	 *                if an error occurs when the unmarshal method of
	 *                inputRecord is invoked.
	 * @see #enableLargeTransfers
	 */
	public int readLock(GuardianInput inputRecord)
			throws EnscribeFileException, DataConversionException {

		int countRead = 0;

		if (inputRecord == null)
			throw new IllegalArgumentException("inputRecord cannot be null.");
		int maxRead = inputRecord.getLength();

		if (largeTransfers_)
			throw new EnscribeFileException(
					"Unable to readLock when doing large transfers.",
					EnscribeFileException.FEBADCOUNT, "readLock", fileName_);

		if (maxRead > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The value returned from inputRecord.getLength() is greater than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "readLock", fileName_);
		byte[] readBuf = new byte[maxRead];
		countRead = readLock(readBuf, maxRead);
		if (countRead != -1)
			inputRecord.unmarshal(readBuf, countRead);
		return countRead;
	} // readLock

	/**
	 * Reads data into a byte array in anticipation of a subsequent write to the
	 * file. This method is used for random processing. Data is read from the
	 * file at the position of the current-record pointer. Typically follows a
	 * corresponding call to keyPosition() or position(). The values of the
	 * current and next-record pointers do not change with the call to
	 * readUpdate(). Refer to READUPDATE in the Guardian Procedure Calls Manual
	 * for more information.
	 * 
	 * @param buf
	 *            the destination byte array.
	 * @param maxRead
	 *            the maximum number of bytes to read.
	 * @return the actual number of bytes read. Returns -1 if end-of-file
	 *         reached.
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the readUpdate operation
	 *                or if the file is not open, if the value of maxRead is
	 *                greater than 57344, maxRead was not a multiple of 2048
	 *                bytes and you are doing large transfers, or if maxRead is >
	 *                buf.length.
	 * @see #enableLargeTransfers
	 */
	public int readUpdate(byte[] buf, int maxRead) throws EnscribeFileException {
		short error;
		int[] countRead = new int[1];

		checkReadInputParams(maxRead, buf.length, 0, true, "readUpdate");

		warn_ = null;

		error = Gread(READ_TYPE_READUPDATE, fileNumber_, buf, maxReadCount_,
				countRead);
		if (error != 0) {
			if (error == EnscribeFileException.FEEOF)
				countRead[0] = -1;
			else
				checkReadError(error, "readUpdate");
		}
		return countRead[0];
	} // readUpdate

	/**
	 * Reads data into an object that implements the GuardianInput interface in
	 * anticipation of a subsequent write to the file. This method is used for
	 * random processing. Data is read from the file at the position of the
	 * current-record pointer. Typically follows a corresponding call to
	 * keyPosition() or position(). The values of the current and next-record
	 * pointers do not change with the call to readUpdate(). This method calls
	 * the unmarshal method of the object that implements the GuardianInput
	 * interface before returning. Refer to READUPDATE in the Guardian Procedure
	 * Calls Manual for more information.
	 * 
	 * @param inputRecord
	 *            an object that implements the GuardianInput interface.
	 * 
	 * @return the number of bytes read. Returns -1 if end-of-file reached.
	 * 
	 * @exception EnscribeFileException
	 *                a Guardian error occurs during the readUpdate operation or
	 *                if the file is not open, if the value returned by
	 *                inputRecord.getLength() exceeds the 57344, or if
	 *                inputRecord.getLength is not a multiple of 2048 bytes and
	 *                you are doing large transfers
	 * @exception DataConversionException
	 *                if an error occurs when invoking the unmarshal method of
	 *                inputRecord.
	 * @see #enableLargeTransfers
	 */
	public int readUpdate(GuardianInput inputRecord)
			throws EnscribeFileException, DataConversionException {
		int countRead = 0;

		if (inputRecord == null)
			throw new IllegalArgumentException("inputRecord cannot be null.");

		int maxRead = inputRecord.getLength();
		if (maxRead > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The value returned by inputRecord.getLength() exceeds the maximum value allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "readUpdate", fileName_);
		byte[] readBuf = new byte[maxRead];
		countRead = readUpdate(readBuf, maxRead);
		if (countRead != -1)
			inputRecord.unmarshal(readBuf, countRead);
		return countRead;
	} // readUpdate

	/**
	 * Locks and then reads data into a byte array in anticipation of a
	 * subsequent write to the file. readUpdateLock() is functionally equivalent
	 * to lockRecord() followed by readUpdate() but incurs less system
	 * processing. Refer to READUPDATELOCK in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * @param buf
	 *            the destination byte array.
	 * @param maxRead
	 *            the maximum number of bytes to read.
	 * @return the actual number of bytes read. Returns -1 if end-of-file
	 *         reached.
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the readUpdateLock
	 *                operation, if the file is not open, if the value of
	 *                maxRead exceeds 57344, if maxRead exceeds the size of buf,
	 *                or you are doing large transfers
	 * @see #enableLargeTransfers
	 */
	public int readUpdateLock(byte[] buf, int maxRead)
			throws EnscribeFileException {
		short error;
		int[] countRead = new int[1];

		checkReadInputParams(maxRead, buf.length, 0, false, "readUpdateLock");

		if (largeTransfers_)
			throw new EnscribeFileException(
					"Unable to readUpdateLock when doing large transfers.",
					EnscribeFileException.FEBADCOUNT, "readUpdateLock",
					fileName_);

		warn_ = null;
		error = Gread(READ_TYPE_READUPDATELOCK, fileNumber_, buf,
				maxReadCount_, countRead);
		if (error != 0) {
			if (error == EnscribeFileException.FEEOF)
				countRead[0] = -1;
			else
				checkReadError(error, "readUpdateLock");
		}
		return countRead[0];
	} // readUpdateLock

	/**
	 * Locks and then reads data into an object which implements the
	 * GuardianInput interface in anticipation of a subsequent write to the
	 * file. readUpdateLock() is functionally equivalent to lockRecord()
	 * followed by readUpdate() but incurs less system processing. This method
	 * calls the unmarshal method of inputRecord before returning. Refer to
	 * READUPDATELOCK in the Guardian Procedure Calls Manual for more
	 * information about readUpdateLock.
	 * 
	 * @param intputRecord
	 *            an object that implements the GuardianInput interface
	 * 
	 * @return the actual number of bytes read. Returns -1 if end-of-file
	 *         reached.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the readUpdateLock
	 *                operation, if the value returned by
	 *                inputRecord.getLength() is greater than 57344, if the file
	 *                is not open or you are doing large transfers
	 * @exception DataConversionException
	 *                if an error occurs when invoking the unmarshal method of
	 *                inputRecord.
	 * @see GuardianInput
	 */
	public int readUpdateLock(GuardianInput inputRecord)
			throws EnscribeFileException, DataConversionException {

		int countRead = 0;

		if (inputRecord == null)
			throw new IllegalArgumentException("inputRecord cannot be null.");

		if (inputRecord.getLength() > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The value returned by inputRecord.getLength() exceeds the maximum of: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "readUpdateLock",
					fileName_);

		int maxRead = inputRecord.getLength();
		byte[] readBuf = new byte[maxRead];
		countRead = readUpdateLock(readBuf, maxRead);
		if (countRead != -1)
			inputRecord.unmarshal(readBuf, countRead);
		return countRead;
	} // readUpdateLock

	/**
	 * Renames an open Guardian file. Primarily used for a temporary disk file;
	 * invoking this method causes the file to be made permanent. The
	 * newFileName must be in Guardian format. Refer to FILE_RENAME_ in the
	 * Guardian Procedure Calls Manual for more information.
	 * 
	 * @param newFileName
	 *            a String with the new name of the file.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open. or if
	 *                newFileName is not a valid file name.
	 */
	public void renameOpenfile(String newFileName) throws EnscribeFileException {
		short error;

		if (newFileName == null)
			throw new IllegalArgumentException("newFileName cannot be null.");
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to rename file because file is not open.",
					EnscribeFileException.FENOOPEN, "renameOpenfile", fileName_);
		if (!isValidFileName(newFileName))
			throw new EnscribeFileException(
					"File can not be renamed because the new filename is invalid.",
					EnscribeFileException.FEBADFORM, "renameOpenfile",
					fileName_);

		error = GrenameOpenFile(fileNumber_, newFileName);
		if (error != 0)
			throw new EnscribeFileException("File can not be renamed.", error,
					"renameOpenfile", fileName_);
		fileName_ = newFileName;
	} // renameOpenfile

	/**
	 * Positions a disk file to a previously saved position. Refer to
	 * FILE_RESTOREPOSITION_ in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param positionInfo
	 *            a byte array with the positioning information returned by a
	 *            previous invocation of savePosition().
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @exception IllegalArgumentException
	 *                if positionInfo is null or a zero length array.
	 */
	public void restorePosition(byte[] positionInfo)
			throws EnscribeFileException, IllegalArgumentException {
		short error;
		if (positionInfo == null || positionInfo.length == 0)
			throw new IllegalArgumentException(
					"positionInfo cannot be null or a zero length array.");
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to restorePosition because the file is not open.",
					EnscribeFileException.FENOOPEN, "restorePosition",
					fileName_);
		error = GrestorePosition(fileNumber_, positionInfo,
				(short) positionInfo.length);
		if (error != 0)
			throw new EnscribeFileException("File restorePosition error.",
					error, "restorePosition", fileName_);

	} // restorePosition

	/**
	 * Saves a disk file's current positioning information in anticipation of a
	 * need to return to that position. To return to the saved position, call
	 * the restorePosition() method. Refer to FILE_SAVEPOSITION_ in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @return positionInfo, a byte array containing the save position data
	 *         needed by restorePosition.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public byte[] savePosition() throws EnscribeFileException {
		short error;
		short[] saveSize = new short[2];
		// Begin of Changes for Solution Number 10-060501-6193
		// saveSize[0] = 300;
		saveSize[0] = 600;
		// End of Changes for Solution Number 10-060501-6193
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to savePosition because the file is not open.",
					EnscribeFileException.FENOOPEN, "savePosition", fileName_);
		// Begin of Changes for Solution Number 10-060501-6193
		// byte[] posData = new byte[300];
		byte[] posData = new byte[600];
		// End of Changes for Solution Number 10-060501-6193
		error = GsavePosition(fileNumber_, posData, saveSize);
		if (error != 0)
			throw new EnscribeFileException("File savePosition error.", error,
					"savePosition", fileName_);
		return posData;
	} // savePosition

	/**
	 * Changes the Guardian file ownership of a file. Refer to FILE_ALTERLIST_
	 * in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param group
	 *            the group ID from 0 to 255
	 * @param userid
	 *            the user's ID from 0 to 255
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @exception java.lang.IllegalArgumentException
	 *                if the group or user id is out-of-bounds
	 */

	public void setFileOwnership(int group, int userid)
			throws EnscribeFileException, java.lang.IllegalArgumentException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to setFileOwnership because the file is not open.",
					EnscribeFileException.FENOOPEN, "setFileOwnership",
					fileName_);
		if (group < 0 || group > 255)
			throw new IllegalArgumentException("group must be >= 0 and <= 255");
		if (userid < 0 || userid > 255)
			throw new IllegalArgumentException("userid must be >= 0 and <= 255");
		short error = GsetFileOwnership(fileNumber_, (short) group,
				(short) userid);

		if (error != 0)
			throw new EnscribeFileException(
					"Error doing Enscribe void setFileOwnership.", error,
					"setFileOwnership", fileName_);

	} // setFileOwnership

	/**
	 * Sets the file security of the file associated with this EnscribeFile
	 * object. File must be open. Refer to FILE_ALTERLIST_ in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @see #FILE_SECURITY_A
	 * @see #FILE_SECURITY_G
	 * @see #FILE_SECURITY_N
	 * @see #FILE_SECURITY_O
	 * @see #FILE_SECURITY_C
	 * @see #FILE_SECURITY_U
	 * @see #FILE_SECURITY_SUPER
	 * 
	 * @param readOpt
	 *            read security.
	 * @param writeOpt
	 *            write security.
	 * @param executeOpt
	 *            execute security.
	 * @param purgeOpt
	 *            purge security.
	 * @param clearOnPurge
	 *            a boolean, set clear on purge flag if true. See the
	 *            CLEARONPURGE option of the Guardian SETMODE procedure.
	 * @param progid
	 *            a boolean, set progid flag if true. See the PROGID option of
	 *            the Guardian SETMODE procedure.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @exception java.lang.IllegalArgumentException
	 *                if readOpt, writeOpt, executeOpt, or purgeOpt are not
	 *                valid values.
	 * 
	 */
	public void setFileSecurity(int readOpt, int writeOpt, int executeOpt,
			int purgeOpt, boolean clearOnPurge, boolean progid)
			throws EnscribeFileException, java.lang.IllegalArgumentException {
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to setFileSecurity because the file is not open.",
					EnscribeFileException.FENOOPEN, "setFileSecurity",
					fileName_);
		switch (readOpt) {
		case FILE_SECURITY_A:
		case FILE_SECURITY_G:
		case FILE_SECURITY_O:
		case FILE_SECURITY_N:
		case FILE_SECURITY_C:
		case FILE_SECURITY_U:
		case FILE_SECURITY_SUPER:
			break;
		default:
			throw new IllegalArgumentException(
					"Invalid setFileSecurity readOpt value.");
		}
		switch (writeOpt) {
		case FILE_SECURITY_A:
		case FILE_SECURITY_G:
		case FILE_SECURITY_O:
		case FILE_SECURITY_N:
		case FILE_SECURITY_C:
		case FILE_SECURITY_U:
		case FILE_SECURITY_SUPER:
			break;
		default:
			throw new IllegalArgumentException(
					"Invalid setFileSecurity writeOpt value.");
		}
		switch (executeOpt) {
		case FILE_SECURITY_A:
		case FILE_SECURITY_G:
		case FILE_SECURITY_O:
		case FILE_SECURITY_N:
		case FILE_SECURITY_C:
		case FILE_SECURITY_U:
		case FILE_SECURITY_SUPER:
			break;
		default:
			throw new IllegalArgumentException(
					"Invalid setFileSecurity executeOpt value.");
		}
		switch (purgeOpt) {
		case FILE_SECURITY_A:
		case FILE_SECURITY_G:
		case FILE_SECURITY_O:
		case FILE_SECURITY_N:
		case FILE_SECURITY_C:
		case FILE_SECURITY_U:
		case FILE_SECURITY_SUPER:
			break;
		default:
			throw new IllegalArgumentException(
					"Invalid setFileSecurity purgeOpt value.");
		}
		short error;
		error = GsetFileSecurity(fileNumber_, (short) readOpt,
				(short) writeOpt, (short) executeOpt, (short) purgeOpt,
				clearOnPurge, progid);
		if (error != 0)
			throw new EnscribeFileException(
					"Error doing Enscribe setFileSecurity.", error,
					"setFileSecurity", fileName_);

	} // setFileSecurity

	// Begin changes for solution :10-090514-1574

	// since setMode() is private method used only internally by this class it's
	// documentation in javadoc is removed
	// End changes for solution :10-090514-1574

	/**
	 * Sets device-dependent functions. A call to this method blocks the calling
	 * thread but will not block the JVM.
	 * 
	 * Refer to SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @return int[], the first and second element contain the previous settings
	 *         of param1 and param2 respectively.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @see #verifyWrites
	 * @see #serializeWrites
	 * @see #bufferWritesSequentially
	 * @see #useAuditCheckPointCompression
	 */
	private int[] setMode(int function, int param1, int param2)
			throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to execute setMode() because the file is not open.",
					EnscribeFileException.FENOOPEN, "setMode", fileName_);
		short[] lastParams = new short[2];
		error = Gsetmode(fileNumber_, (short) function, (short) param1, false,
				(short) param2, false, lastParams);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe setMode.",
					error, "setMode", fileName_);
		int[] ilastParams = new int[2];
		ilastParams[0] = (int) lastParams[0];
		ilastParams[1] = (int) lastParams[1];
		return ilastParams;
	} // setMode

	/**
	 * Sets device-dependent functions. A call to this method blocks the calling
	 * thread but will not block the JVM.
	 * 
	 * Refer to SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @see #flushDirtyCache
	 */
	private void setMode(int function) throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to execute setMode() because the file is not open.",
					EnscribeFileException.FENOOPEN, "setMode", fileName_);
		short[] lastParams = new short[2];
		error = Gsetmode(fileNumber_, (short) function, (short) 0, true,
				(short) 0, true, lastParams);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe setMode.",
					error, "setMode", fileName_);
	} // setMode

	/**
	 * Sets device-dependent functions. A call to this method blocks the calling
	 * thread but will not block the JVM.
	 * 
	 * Refer to SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @return int containing the previous setting of param1.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @see #setSequentialLRUAccess
	 * @see #setMaxExtents
	 * @see #enableLargeTransfers
	 * @see #disableLargeTransfers
	 * @see #queueWaitersForWrites
	 * @see #setAutomaticAlternateKeyLocking
	 * @see #setVariableLengthAuditCompression
	 * @see #setGenericLockKeyLength
	 * @see #setBufferLength
	 * @see #setFileLockingMode
	 */
	private int setMode(int function, int param1) throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to execute setMode() because the file is not open.",
					EnscribeFileException.FENOOPEN, "setMode", fileName_);

		if (function == SM_SET_LARGE_TRANSFERS) {
			if (!openedUnstructured_)
				throw new EnscribeFileException(
						"Cannot use setMode 141 (SM_SET_LARGE_TRANSFERS) because file was not opened unstructured.",
						EnscribeFileException.FEINVALOP, "setMode", fileName_);
			if (param1 != 0)
				largeTransfers_ = true;
			else
				largeTransfers_ = false;
		}
		short[] lastParams = new short[2];
		error = Gsetmode(fileNumber_, (short) function, (short) param1, false,
				(short) 0, true, lastParams);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe setMode.",
					error, "setMode", fileName_);
		return (int) lastParams[0];
	} // setMode

	/**
	 * Specifies whether writes are to be verified for this file. See function 3
	 * under SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param turnOn
	 *            true if writes are to be verified; false if not.
	 * @param mode
	 *            specify 0 to change the open option of the verifies writes
	 *            option; specify 1 to change the file label default value of
	 *            verifies writes.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void verifyWrites(boolean turnOn, int mode)
			throws EnscribeFileException {
		int param1;
		if (turnOn)
			param1 = 1;
		else
			param1 = 0;
		setMode(SM_SET_WRITE_VERIFICATION, param1, mode);
		return;
	}

	/**
	 * Specifies whether writes are to be serialized for this file. See function
	 * 57 under SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param turnOn
	 *            true if writes are to be serialized; false if not.
	 * @param mode
	 *            specify 0 to change the open option of the serialized writes
	 *            option; specify 1 to change the file label default value of
	 *            serialized writes.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void serializeWrites(boolean turnOn, int mode)
			throws EnscribeFileException {
		int param1;
		if (turnOn)
			param1 = 1;
		else
			param1 = 0;
		setMode(SM_SET_SERIAL_WRITES, param1, mode);
		return;
	}

	// Begin changes for solution :10-090514-1574
	/**
	 * Specifies whether writes are to be buffered sequentially for this file.
	 * See function 90 under SETMODE in the Guardian Procedure Calls Manual for
	 * more information.
	 * 
	 * @param turnOn
	 *            false if writes are to be buffered sequentially; true if not.
	 * @param mode
	 *            specify 0 to change the open option of the buffered writes
	 *            option; specify 1 to change the file label default value of
	 *            buffered writes.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @deprecated As of T2716H10AAK, replaced by
	 *             {@link #bufferWrites(boolean, int)} and
	 */
	public void bufferWritesSequentially(boolean turnOn, int mode)
			throws EnscribeFileException {
		int param1;
		if (turnOn)
			param1 = 1;
		else
			param1 = 0;
		setMode(SM_SET_BUFFERED_AS_CREATE, param1, mode);
		return;
	}

	/**
	 * Specifies whether writes are to be buffered for this file. See function
	 * 90 under SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param turnOn
	 *            true if writes are to be buffered sequentially; false if not.
	 * @param mode
	 *            specify 0 to change the open option of the buffered writes
	 *            option; specify 1 to change the file label default value of
	 *            buffered writes.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void bufferWrites(boolean turnOn, int mode)
			throws EnscribeFileException {
		int param1;
		if (turnOn)
			param1 = 0;
		else
			param1 = 1;
		setMode(SM_SET_BUFFERED_AS_CREATE, param1, mode);
		return;
	}

	// End changes for solution :10-090514-1574
	/**
	 * Specifies whether audit data is to be compressed for this file. See
	 * function 94 under SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param turnOn
	 *            true if audit data is to be compressed; false if not.
	 * @param mode
	 *            specify 0 to change the open option of the audit compression
	 *            option; specify 1 to change the file label default value of
	 *            audit compression.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void useAuditCheckPointCompression(boolean turnOn, int mode)
			throws EnscribeFileException {
		int param1;
		if (turnOn)
			param1 = 1;
		else
			param1 = 0;
		setMode(SM_SET_AUDIT_CP_COMPRESSION, param1, mode);
		return;
	}

	/**
	 * Sets the cache and sequential option for the LRU cache chain. This method
	 * should not be used with alternate-key files. Sequential LRU access
	 * results in random LRU chaining that provides an approximate half-life
	 * within the LRU cache chain. Key-sequenced sequential splits attempt to
	 * leave the inserted record as the last in the old block, in contrast to a
	 * 50/50 split. This helps to ensure a compact key-sequenced structure when
	 * multiple sequential records are inserted. See function 91 under SETMODE
	 * in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param int
	 *            mode specify 0 to use system managed caching (default). DP2
	 *            will detect sequential access; when detected, it will set LRU
	 *            access to sequential and perform key-sequenced sequential
	 *            splits. specify 1 to request direct I/O and bypass disk cache
	 *            specify 2 to request random access, LRU-chain buffer specify 3
	 *            to request sequential access, reuse buffer. This directs DP2
	 *            to set cache LRU access to sequential and perform
	 *            key-sequenced sequential splits.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @return int the previous value of mode.
	 */
	public int setSequentialLRUAccess(int mode) throws EnscribeFileException {
		int ret = setMode(SM_SET_CACHE_SEQ, mode);
		return ret;
	}

	/**
	 * Sets the maximum number of extents for a non-partitioned file. See
	 * function 92 under SETMODE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param int
	 *            maxextents the maximum number of extents.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @return int the previous value of maxextents.
	 */
	public int setMaxExtents(int maxextents) throws EnscribeFileException {
		int ret = setMode(SM_SET_MAX_EXTENTS, maxextents);
		return ret;
	}

	/**
	 * Enables large transfers. When large transfers are enabled a read or write
	 * operation can transfer up to 56K bytes of data to a DP2 disk file if the
	 * operation is local or over a FOX connection, or up 32K bytes if it is
	 * through Expand. When large transfers are enabled, DP2 flushes and removes
	 * all blocks for the file from its cache. This ensures that any updates
	 * done by the user are written to disk before the large transfers are
	 * enabled. The EnscribeOpenOptions.setUnstructuredAccess must have been
	 * called when the file was opened, even if the file is unstructured. There
	 * is no support for alternate key files or partitions. Because the file is
	 * opened for unstructured access, secondary partitions and alternate-key
	 * files are not opened. Refer to function 141 under the SETMODE procedure
	 * for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @return int the value of the files broken flag after DP2 flushes the
	 *         buffers.
	 * @see #disableLargeTransfers
	 */
	public int enableLargeTransfers() throws EnscribeFileException {
		int param = 1;
		int ret = setMode(SM_SET_LARGE_TRANSFERS, param);
		return ret;
	}

	/**
	 * Disables large transfers of data. See function 141 under SETMODE in the
	 * Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @see #enableLargeTransfers
	 */
	public void disableLargeTransfers() throws EnscribeFileException {
		int param = 0;
		int ret = setMode(SM_SET_LARGE_TRANSFERS, param);
		return;
	}

	/**
	 * Queue waiters for disk file writes. Used in conjunction with
	 * waitForDP2Writes. See function 146 under SETMODE in the Guardian
	 * Procedure Calls Manual for more information. param boolean turnOn. If
	 * true and waitForDP2Writes has been invoked, writes are queued and
	 * completed one at a time.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @return previous value of param
	 */
	public boolean queueWaitersForWrites(boolean turnOn)
			throws EnscribeFileException {
		int param = 0;
		if (turnOn)
			param = 1;
		int ret = setMode(SM_QUEUE_WAITERS, param);
		if (ret == 1)
			return true;
		else
			return false;
	}

	/**
	 * Specifies whether writes of records with alternate keys are locked at the
	 * beginning of insertion and unlocked after all associated alternate key
	 * records are written. Not valid for audited files. See function 149 under
	 * SETMODE in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param boolean
	 *            turnOn. If true, records are locked.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void setAutomaticAlternateKeyLocking(boolean turnOn)
			throws EnscribeFileException {
		int param = 0;
		if (turnOn)
			param = 1;
		int ret = setMode(SM_ALT_KEY_INSERT_LOCKING, param);
		return;
	}

	/**
	 * Specifies whether variable length audit compression is used.
	 * 
	 * @param boolean
	 *            turnOn. If true variable length audit compression is used. See
	 *            function 153 under SETMODE in the Guardian Procedure Calls
	 *            Manual for more information.
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * 
	 */
	public void setVariableLengthAuditCompression(boolean turnOn)
			throws EnscribeFileException {
		int param = 0;
		if (turnOn)
			param = 1;
		int ret = setMode(SM_VARLEN_AUDIT_COMPRESS, param);
		return;
	}

	/**
	 * Sets the generic lock key length for a key-sequenced file. The generic
	 * lock key length determines the grouping of records that will share a
	 * single lock. This value must be between 0 and the key length of the file.
	 * If locks are in force at the time of the call, an exception is thrown.
	 * The key length value applies to all partitions of a file. Alternate keys
	 * are not affected. If the lock key length is nonzero and less than the key
	 * length of the file, the keys are not affected. If the lock key length is
	 * nonzero and less than the key length of the file, generic locking is
	 * activated and calls to unLockRec are thus ignored. Generic locking is
	 * turned off by giving a lock key length of 0 or equal to the key length of
	 * the file (which is equivalent to 0). Note that when generic locking is
	 * activated, any write is rejected if it attempts to insert a record having
	 * the same generic lock key as an existing lock owned by another user,
	 * whether for audited or nonaudited files. See function 123 under SETMODE
	 * in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param int
	 *            the generic lock key length.
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file type is NOT
	 *                key-sequenced.
	 * @return int the previous value of lockKeyLength.
	 */
	public int setGenericLockKeyLength(int lockKeyLength)
			throws EnscribeFileException {
		if (fileType_ != EnscribeFileAttributes.FILETYPE_KEY_SEQUENCED)
			throw new EnscribeFileException(
					"Unable to set generic lock key length for a non key-sequenced file.",
					EnscribeFileException.FEINVALOP, "setGenericLockKeyLength",
					fileName_);
		int ret = setMode(SM_SET_GENERIC_LOCK_KEY_LEN, lockKeyLength);
		return ret;
	}

	/**
	 * Flushes dirty cache to disk. See function 95 under SETMODE in the
	 * Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 */
	public void flushDirtyCache() throws EnscribeFileException {
		setMode(SM_FLUSH_DIRTY_CACHE);
		return;
	}

	/**
	 * Sets the buffer length for an unstructured file. See function 93 under
	 * SETMODE in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param int
	 *            bufferlength; this must be a valid DP2 blocksize which is 512,
	 *            1024, 2048, 4096.
	 * @exception EnscribeFileException
	 *                if an error occurs
	 * @return int the previous value of bufferlength.
	 */
	public int setBufferLength(int bufferlength) throws EnscribeFileException {
		switch (bufferlength) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
			break;
		default:
			throw new EnscribeFileException(
					"Invalid value specified for bufferlength: " + bufferlength,
					EnscribeFileException.FEBADCOUNT, "setBufferLength",
					fileName_);
		}
		int ret = setMode(SM_SET_BUFFER_LENGTH, bufferlength);
		return ret;
	}

	/**
	 * Specifies a locking mode to be used for lockFile, lockRec, read,
	 * readUpdate, readLock, and readUpdateLock. See function 4 under SETMODE in
	 * the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param lockingMode
	 *            <DL>
	 *            see
	 *            <DD>{@link #NORMAL_LOCKING_MODE NORMAL_LOCKING_MODE}
	 *            <DD>{@link #REJECT_LOCKING_MODE REJECT_LOCKING_MODE}
	 *            <DD>{@link #READ_THROUGH_NORMAL_LOCKING_MODE READ_THROUGH_NORMAL_LOCKING_MODE}
	 *            <DD>{@link #READ_THROUGH_REJECT_LOCKING_MODE READ_THROUGH_REJECT_LOCKING_MODE}
	 *            <DD>{@link #READ_WARN_NORMAL_LOCKING_MODE READ_WARN_NORMAL_LOCKING_MODE}
	 *            <DD>{@link #READ_WARN_REJECT_LOCKING_MODE READ_WARN_REJECT_LOCKING_MODE}
	 *            </DL>
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @exception java.lang.IllegalArgumentException
	 *                if an invalid value is specified for lockingMode
	 */
	public int setFileLockingMode(int lockingMode)
			throws EnscribeFileException, java.lang.IllegalArgumentException {
		switch (lockingMode) {
		case NORMAL_LOCKING_MODE:
			break;
		case REJECT_LOCKING_MODE:
			break;
		case READ_THROUGH_NORMAL_LOCKING_MODE:
			break;
		case READ_THROUGH_REJECT_LOCKING_MODE:
			break;
		case READ_WARN_NORMAL_LOCKING_MODE:
			break;
		case READ_WARN_REJECT_LOCKING_MODE:
			break;
		default:
			throw new IllegalArgumentException(
					"Invalid value specified for lockingMode.");
		}
		int lastParams = setMode(SM_SET_LOCK_MODE, lockingMode);
		return lastParams;
	}

	/**
	 * Unlocks the disk file associated with this EnscribeFile object. Also
	 * unlocks any records in the file that are currently locked by the user.
	 * The user is defined as the opener of the file (if the file is not
	 * audited) or by the transaction(if the file is audited). Refer to
	 * UNLOCKFILE in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void unlockFile() throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to unlockFile because the file is not open.",
					EnscribeFileException.FENOOPEN, "unlockFile", fileName_);
		error = Gunlockfile(fileNumber_);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe unlockFile.",
					error, "unlockFile", fileName_);
	} // unlockFile

	/**
	 * Unlocks a record currently locked by the user within the disk file
	 * associated with this EnscribeFile object. The user is defined as the
	 * opener of the file (if the file is not audited) or by the transaction(if
	 * the file is audited). Refer to UNLOCKREC in the Guardian Procedure Calls
	 * Manual for more information.
	 * 
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public void unlockRecord() throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to unlockRecord because the file is not open.",
					EnscribeFileException.FENOOPEN, "unlockRecord", fileName_);
		error = Gunlockrec(fileNumber_);
		if (error != 0)
			throw new EnscribeFileException(
					"Error doing Enscribe unlockRecord.", error,
					"unlockRecord", fileName_);
	} // unlockRecord

	private void checkWriteInputParams(int writeLen,
			boolean checkLargeTransfers, String method)
			throws EnscribeFileException {

		if (writeLen > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"writeLen is greater than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, method, fileName_);
		if (checkLargeTransfers) {
			if (largeTransfers_) {
				if (!multiple2K(writeLen)) {
					throw new EnscribeFileException(
							"writeLen must be a multiple of 2048 bytes.",
							EnscribeFileException.FEBADCOUNT, method, fileName_);
				}
			}
		}
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to write because the file is not open.",
					EnscribeFileException.FENOOPEN, method, fileName_);
	}

	/**
	 * Writes data to the file associated with this EnscribeFile object from a
	 * byte array. For key-sequenced files, the record is inserted in the
	 * position indicated by its primary key value. For relative files, the
	 * record is inserted in at the current position in the file as determined
	 * by a preceeding call to position or keyPosition. For entry-sequenced
	 * files, the record is inserted following the last record currently
	 * existing in the file. For an unstructured file, the record is inserted at
	 * the position indicated by the current value of the next-record pointer.
	 * Refer to WRITE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param buffer
	 *            the byte array from which the data is to be written.
	 * @param writeLen
	 *            the number of bytes to write.
	 * @return a count of the number of bytes written.
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the write operation, if
	 *                the file is not open, if writeLen is greater than 57344,
	 *                or if writeLen is not a multiple of 2048 bytes and you are
	 *                doing large transfers
	 * @see #enableLargeTransfers
	 */
	public int write(byte[] buffer, int writeLen) throws EnscribeFileException {
		short error;
		int[] countWritten = new int[1];

		checkWriteInputParams(writeLen, true, "write");

		error = Gwrite(WRITE_TYPE_WRITE, fileNumber_, buffer, writeLen,
				countWritten);
		if (error != 0)
			throw new EnscribeFileException("File write error.", error,
					"write", fileName_);
		return countWritten[0];
	} // write

	/**
	 * Writes data to the file associated with this EnscribeFile object from an
	 * object that implements the GuardianOutput interface. This method calls
	 * the marshal method of the outputRecord object before performing the
	 * write.
	 * 
	 * For key-sequenced files, the record is inserted in the position indicated
	 * by its primary key value. For relative files, the record is inserted in
	 * at the current position in the file as determined by a preceeding call to
	 * position or keyPosition. For entry-sequenced files, the record is
	 * inserted following the last record currently existing in the file. For an
	 * unstructured file, the record is inserted at the position indicated by
	 * the current value of the next-record pointer.
	 * 
	 * Refer to WRITE in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param outputRecord
	 *            an object that implements the GuardianOutput interface.
	 * @return a count of the number of bytes written.
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the WRITE, if the file
	 *                is not open, or if the value returned by
	 *                outputRecord.getLength() is greater than 57344 or is not a
	 *                multiple of 2048 bytes and you are doing large transfers.
	 * @exception DataConversionException
	 *                if an error occurs when invoking the outputRecord.marshal
	 *                method.
	 * @see #enableLargeTransfers
	 * @see GuardianOutput
	 */
	public int write(GuardianOutput outputRecord) throws EnscribeFileException,
			IllegalArgumentException, DataConversionException {

		int countWritten = 0;

		if (outputRecord == null)
			throw new IllegalArgumentException("outputRecord cannot be null.");

		int writeLen = outputRecord.getLength();
		if (writeLen > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The value returned by outputRecord.getLength() is greater than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "write", fileName_);

		byte[] buffer = outputRecord.marshal();
		countWritten = write(buffer, writeLen);

		return countWritten;
	} // write

	/**
	 * Updates the contents of the record at the current position.
	 * 
	 * Typically follows the invocation of either the read() or readUpdate()
	 * method. Refer to WRITEUPDATE in the Guardian Procedure Calls Manual for
	 * more information.
	 * 
	 * @param buffer
	 *            a byte array containing the data to be written.
	 * @param writeLen
	 *            number of bytes to write to the file
	 * 
	 * @return a count of the number of bytes written.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the WRITEUPDATE, if the
	 *                file is not open, if writeLen is greater than 57344, if
	 *                writeLen is not a multiple of 2048 bytes and you are doing
	 *                large transfers, or if writeLen > writeBuf.length.
	 * @see #read
	 * @see #readUpdate
	 * @see #enableLargeTransfers
	 */
	public int writeUpdate(byte[] writeBuf, int writeLen)
			throws EnscribeFileException, IllegalArgumentException {
		short error;
		int[] countWritten = new int[1];

		checkWriteInputParams(writeLen, true, "writeUpdate");

		error = Gwrite(WRITE_TYPE_WRITEUPDATE, fileNumber_, writeBuf, writeLen,
				countWritten);
		if (error != 0)
			throw new EnscribeFileException("File write error.", error,
					"writeUpdate", fileName_);
		return countWritten[0];
	} // writeUpdate

	/**
	 * Updates the contents of the record at the current position using the data
	 * from an object that implements the GuardianOutput interface. This method
	 * calls the marshal method of the outputRecord before doing the write.
	 * 
	 * Typically follows the invocation of either the read() or readUpdate()
	 * method. Refer to WRITEUPDATE in the Guardian Procedure Calls Manual for
	 * more information.
	 * 
	 * @param outputRecord
	 *            an object that implements the GuardianOutput interface.
	 * 
	 * @return a count of the number of bytes written.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the WRITEUPDATE, if the
	 *                file is not open, if the value returned by
	 *                outputRecord.getLength() is greater than 57344, or if the
	 *                value returned from outputRecord.getLength() is not a
	 *                multiple of 2048 bytes and you are doing large transfers.
	 * @exception DataConversionException
	 *                if an error occurs when calling outputRecord.marshal.
	 * @see #read
	 * @see #readUpdate
	 * @see #enableLargeTransfers
	 */
	public int writeUpdate(GuardianOutput outputRecord)
			throws EnscribeFileException, java.lang.IllegalArgumentException,
			DataConversionException {
		int countWritten = 0;

		if (outputRecord == null)
			throw new IllegalArgumentException("outputRecord cannot be null.");

		int writeLen = outputRecord.getLength();
		if (writeLen > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The length of outputRecord exceeds the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "writeUpdate", fileName_);

		byte[] writeBuf = outputRecord.marshal();
		countWritten = writeUpdate(writeBuf, outputRecord.getLength());
		return countWritten;
	} // writeUpdate

	/**
	 * Updates and unlocks the record at the current position.
	 * 
	 * Typically follows the invocation of either the read() or readUpdate()
	 * method. Refer to WRITEUPDATEUNLOCK in the Guardian Procedure Calls Manual
	 * for more information.
	 * 
	 * @param buffer
	 *            a byte array containing the data to be written.
	 * @param writeLen
	 *            number of bytes to write to the file
	 * 
	 * @return a count of the number of bytes written.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the WRITEUPDATELOCK, if
	 *                the file is not open, if writeLen is greater than 57344,
	 *                or you are doing large transfers
	 * @see #enableLargeTransfers
	 */
	public int writeUpdateUnlock(byte[] buffer, int writeLen)
			throws EnscribeFileException {
		short error;
		int[] countWritten = new int[1];

		checkWriteInputParams(writeLen, false, "writeUpdateUnlock");

		if (largeTransfers_)
			throw new EnscribeFileException(
					"writeUpdateUnlock is not valid when doing large transfers.",
					EnscribeFileException.FEBADCOUNT, "writeUpdateUnlock",
					fileName_);

		error = Gwrite(WRITE_TYPE_WRITEUPDATEUNLOCK, fileNumber_, buffer,
				writeLen, countWritten);
		if (error != 0)
			throw new EnscribeFileException("File write error.", error,
					"writeUpdateUnlock", fileName_);
		return countWritten[0];
	} // writeUpdateUnlock

	/**
	 * Updates and unlocks the record at the current position using the data
	 * obtained from an object that implements the GuardianOutput interface.
	 * This method calls the marshal method of the object before performing the
	 * write.
	 * 
	 * Typically follows the invocation of either the read() or readUpdate()
	 * method. Refer to WRITEUPDATEUNLOCK in the Guardian Procedure Calls Manual
	 * for more information.
	 * 
	 * @param outputRecord
	 *            an object that implements the GuardianOutput interface.
	 * 
	 * @return a count of the number of bytes written.
	 * 
	 * @exception EnscribeFileException
	 *                if a Guardian error occurs during the WRITEUPDATEUNLOCK,
	 *                if the file is not open, if the value returned by
	 *                outputRecord.getLength() is greater than 57344, or you are
	 *                doing large transfers
	 * @exception DataConversionException
	 *                if an error occurs when invoking the outputRecord.marshal
	 *                method.
	 * @see #enableLargeTransfers
	 */
	public int writeUpdateUnlock(GuardianOutput outputRecord)
			throws EnscribeFileException, DataConversionException {
		int countWritten = 0;

		if (outputRecord == null)
			throw new IllegalArgumentException("outputRecord cannot be null");
		int len = outputRecord.getLength();
		if (len > GUARDIAN_MSG_MAX)
			throw new EnscribeFileException(
					"The size of outputRecord is greater than the maximum allowed: "
							+ GUARDIAN_MSG_MAX,
					EnscribeFileException.FEBADCOUNT, "writeUpdateUnlock",
					fileName_);

		if (largeTransfers_)
			throw new EnscribeFileException(
					"writeUpdateUnlock is not valid when doing large transfers.",
					EnscribeFileException.FEBADCOUNT, "writeUpdateUnlock",
					fileName_);
		byte[] buffer = outputRecord.marshal();
		countWritten = writeUpdateUnlock(buffer, len);
		return countWritten;
	} // writeUpdateUnlock

	/**
	 * Positions a structured file by primary or alternate key. Also positions
	 * by primary key for key-sequenced files. (To position by primary key for
	 * relative and entry-sequenced files, use position(). Refer to FILE_SETKEY_
	 * in the Guardian Procedure Calls Manual for more information.
	 * 
	 * @param positionOptions
	 *            an EnscribeKeyPositionOptions object
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 */
	public synchronized void keyPosition(
			EnscribeKeyPositionOptions positionOptions)
			throws EnscribeFileException {
		short error;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to keyPosition because the file is not open.",
					EnscribeFileException.FENOOPEN, "keyPosition", fileName_);
		if (positionOptions == null)
			throw new IllegalArgumentException(
					"positionOptions cannot be null.");
		short keylen = positionOptions.getKeyLength();
		if (keylen == -1)
			keylen = (short) positionOptions.getNumericKeyValue().length;

		error = Gkeyposition(fileNumber_, positionOptions
				.isKeySpecifierNumeric(), positionOptions
				.getStringKeySpecifier(), positionOptions
				.getNumericKeySpecifier(),
				positionOptions.getNumericKeyValue(), (short) positionOptions
						.getCompareLength(), keylen, (short) positionOptions
						.getPositioningMode(), positionOptions
						.isPositionedToNext(), positionOptions
						.areReadsReversed(), positionOptions
						.isPositionedToLast());
		if (error != 0)
			throw new EnscribeFileException(
					"Error doing Enscribe keyPosition.", error, "keyPosition",
					fileName_);
	} // keyPosition

	/**
	 * Positions by primary key within relative and entry-sequenced files. For
	 * unstructured files, specifies a new current position. Should not be used
	 * with key-sequenced files, use the keyPosition method instead. Refer to
	 * FILE_SETPOSITION_ in the Guardian Procedure Calls Manual for more
	 * information.
	 * 
	 * @param
	 * <DL>
	 * recordSpecifier
	 * <DT>For relative files, a four-byte record number.
	 * <DT>For unstructured files, a four-byte relative byte address.
	 * <DT>For entry-sequenced files, a four-byte record address.
	 * <DT>POSITION_UNUSED, position to an unused record position.
	 * <DT>POSITION_EOF, position to the end-of-file location.
	 * </DL>
	 * Refer to the Enscribe Programmer's Guide for more information about
	 * record numbers, relative byte addresses, and record address.
	 * 
	 * @exception EnscribeFileException
	 *                an error occurs or if the file is not open.
	 * @see #POSITION_UNUSED
	 * @see #POSITION_EOF
	 */
	public synchronized void position(long recordSpecifier)
			throws EnscribeFileException {
		short error;

		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"Unable to position because the file is not open.",
					EnscribeFileException.FENOOPEN, "position", fileName_);

		error = Gposition(fileNumber_, recordSpecifier);
		if (error != 0)
			throw new EnscribeFileException("Error doing Enscribe position.",
					error, "position", fileName_);

	} // position

	/**
	 * Turns on the internal debug flag. Should be enabled only if instructed by
	 * Tandem support.
	 * 
	 */
	private void enableDebug() {
		debugFlag_ = true;
	} // enableDebug

	/**
	 * Turns off the internal debug flag. Should be enabled only if instructed
	 * by Tandem support.
	 * 
	 */
	private void disableDebug() {
		debugFlag_ = false;
	} // disableDebug

	/**
	 * Returns the internal debug flag.
	 * 
	 * @return true after enableDebug(), false after disableDebug().
	 */
	private boolean isDebugOn() {
		return debugFlag_;
	} // getDebugFlag

	/**
	 * Turns on the internal debug flag in JNI code. Should be enabled only if
	 * instructed by Tandem support.
	 * 
	 * @param dumpCnt
	 *            how many bytes of I/O buffers to dump. Zero indicates none to
	 *            be dumped.
	 */
	private void enableJNIDebug(int dumpCnt) {
		GDebugFlag(true, (short) dumpCnt);
	} // enableJNIDebug

	/**
	 * Returns the internal JNI debug flag.
	 * 
	 * @return true after enableJNIDebug(), false after disableJNIDebug().
	 * @see #enableJNIDebug
	 */
	private boolean isJNIDebugOn() {
		return GgetDebugFlag();
	} // getJNIDebugFlag

	/**
	 * Turns off the internal debug flag. Should be enabled only if instructed
	 * by Tandem support.
	 * 
	 * @see #enableJNIDebug
	 * 
	 */
	private void disableJNIDebug() {
		GDebugFlag(false, (short) 0);
	} // disableDebug

	/**
	 * Returns an EnscribeFileAttributes object containing static information
	 * about this Enscribe file. Refer to FILE_GETINFOLIST_ in the Guardian
	 * Procedure Calls Manual for more information.
	 * 
	 * @return an EnscribeFileAttributes object with detail information about
	 *         this file
	 * @exception EnscribeFileException
	 *                if an error occurs.
	 * @see EnscribeFileAttributes
	 */
	public EnscribeFileAttributes getFileInfo() throws EnscribeFileException {
		return (new EnscribeFileAttributes(fileName_));
	}// getFileInfo

	/**
	 * Returns an EnscribeLastCallStatus object containing transient information
	 * about this Enscribe file.
	 * 
	 * @return an EnscribeLastCallStatus object with detail information about
	 *         this file
	 * @exception EnscribeFileException
	 *                if an error occurs or if the file is not open.
	 * @see EnscribeLastCallStatus
	 */
	public EnscribeLastCallStatus getLastCallStatus()
			throws EnscribeFileException {
		int filetype;
		if (openedUnstructured_)
			filetype = EnscribeFileAttributes.FILETYPE_UNSTRUCTURED;
		else
			filetype = fileType_;
		if (!fileIsOpen_)
			throw new EnscribeFileException(
					"getLastCallStatus called when the file is not open",
					EnscribeFileException.FENOOPEN, "getLastCallStatus",
					fileName_);
		return (new EnscribeLastCallStatus(fileNumber_, filetype, false,
				fileName_));

	}// getCurrentOpenedFileInfo

	/***************************************************************************
	 * Internal function
	 **************************************************************************/
	boolean multiple2K(int len) {
		int countCk = ((len + 2047) / 2048) * 2048;
		if (len != countCk)
			return false;
		else
			return true;
	} // multiple2K

	// ---------------------------------------------------
	// ---------------------------------------------------
	// All JINI calls follow

	// returns error
	private final native short GfileAlter(/* in */String fileName,
	/* in */short partOnly,
	/* in */String creationAttributes);

	private final native short Gposition(/* in */short fileNum,
	/* in */long recordSpecifier);

	private final native short Gkeyposition(short fileNum,
			boolean isKeySpecifierNumeric, String keySpecifier,
			short keySpecifierNumeric, byte[] keyValue, short compareLength,
			short keyLength, short mode, boolean positionToNextRecord,
			boolean readReverse, boolean positionToLastRecord);

	private final native short Gcontrol(/* in */short fileNum,
	/* in */short operation,
	/* in */short param);

	private final native void GfileClose(/* in */short fileNum);

	// returns error
	private final native short GfileCreate(/* in */String fileName,
	/* in */String creationAttributes);

	// returns String
	private final native String GfileCreateTemp(/* in */String volumeName,
	/* in */String creationAttributes,
	/* out */short[] errorArray);

	// returns error
	private final native short GfileExists(/* in */String fileName,
	/* in */boolean fileNameIsOSS);

	// returns error
	private final native short GfileOpen(/* in */String fileName,
	/* in */boolean fileNameIsOSS,
	/* out */short[] fileInfo,
	/* in */short access,
	/* in */short exclusion,
	/* in */short syncDepth,
	/* in */short seqBlockBufferLength,
	/* in */boolean unstructuredAccess,
	/* in */boolean noOpenTimeUpdate,
	/* in */boolean use64BitPrimaryKeyValues_);

	// returns error
	private final native short GfilePurge(/* in */String fileName,
			boolean fileNameIsOSS);

	// returns error
	private final native short GfileRename(/* in */String fileName,
	/* in */String newFileName,
	/* in */boolean fileNameIsOSS);

	private final native short GgetGuardianName(String fileName,
			boolean fileNameIsOSS);

	private final native short Glockfile(/* in */short fileNum);

	private final native short Glockrec(/* in */short fileNum);

	// returns error
	private final native short Gread(/* in */short readType,
	/* in */short fileNum,
	/* out */byte[] buf,
	/* in */int maxReadCount,
	/* out */int[] countRead);

	// returns error
	private final native short GrenameOpenFile(/* in */short fileNum,
	/* in */String newFileName);

	// returns error
	private final native short GrestorePosition(/* in */short fileNum,
	/* in */byte[] posData,
	/* in */short saveSize);

	// returns error
	private final native short GsavePosition(/* in */short fileNum,
	/* out */byte[] posData,
	/* out */short[] saveSize);

	// returns error
	private final native short GsetFileOwnership(/* in */short fileNum,
	/* in */short group,
	/* in */short userid);

	// returns error
	private final native short GsetFileSecurity(/* in */short fileNum,
	/* in */short readOpt,
	/* in */short writeOpt,
	/* in */short executeOpt,
	/* in */short purgeOpt,
	/* in */boolean clearOnPurge,
	/* in */boolean progid);

	// returns error
	private final native short Gsetmode(/*in*/short fileNum,
	/*in*/short function,
	/*in*/short param1,
	/*in*/boolean param1Omitted,
	/*in*/short param2,
	/*in*/boolean param2Omitted,
	/*out*/short[] lastParams);

	private final native short Gunlockfile(/*in*/short fileNum);

	private final native short Gunlockrec(/*in*/short fileNum);

	private final native short GcancelReq(/*in*/short fileNum);

	// returns error
	private final native short Gwrite(/*in*/short writeType,
	/*in*/short fileNum,
	/*in*/byte[] buf,
	/*in*/int writeCount,
	/*out*/int[] countWritten);

	//
	private final native void GDebugFlag(/*in*/boolean flag,
	/*in*/short dumpCnt);

	private final native boolean GgetDebugFlag();

} // EnscribeFile
