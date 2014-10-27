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

#ifndef _Included_EnscribeFile_h
#define _Included_EnscribeFile_h

#define FI_FILENUM_IDX  0
#define FI_DEVTYPE_IDX  1
#define FI_DEVSUBTYPE_IDX  2
#define FI_FILETYPE_IDX  3
#define FI_FILECODE_IDX  4
#define FI_RECSIZE_IDX  5

#define DEVICETYPE_DISC 3

#define FILETYPE_UNSTRUCTURED 0
#define FILETYPE_RELATIVE 1
#define FILETYPE_ENTRY_SEQUENCED 2
#define FILETYPE_KEY_SEQUENCED 3

#define READ_TYPE_READ  0
#define READ_TYPE_READLOCK  1
#define READ_TYPE_READUPDATE  2
#define READ_TYPE_READUPDATELOCK  3

#define WRITE_TYPE_WRITE  0
#define WRITE_TYPE_WRITEUPDATE  2
#define WRITE_TYPE_WRITEUPDATEUNLOCK  3

/* ---------- flags for encoding and decoding -------    */
/* keep in sync with EnscribeFileAttr.java */
#define OV_ERROR_DETAIL 8
#define OV_PARTITION_IN_ERROR 9
#define OV_KEY_IN_ERROR 10
#define OV_CURRENT_KEY_SPECIFIER 13
#define OV_OPEN_ACCESS_MODE 21
#define OV_OPEN_EXCLUSION_MODE 22
#define OV_PHYSICAL_RECORD_LENGTH 34
#define OV_FILE_TYPE 41
#define OV_FILE_CODE 42
#define OV_KEY_LENGTH 46
#define OV_LOCK_KEY_LENGTH 47
#define OV_MAXIMUM_EXTENTS 52
#define OV_ALLOCATED_EXTENTS 53
#define OV_EXPIRATION_TIME_GMT 57
#define OV_GUARDIAN_FILE_OWNER 58
#define OV_SAFEGUARD_PROTECTED 59
#define OV_PROGID_FLAG 60
#define OV_CLEAR_ON_PURGE_FLAG 61
#define OV_SECURITY_STRING 62
#define OV_LICENSED_FILE_FLAG 63
#define OV_ODD_UNSTRUCTURED_FLAG 65
#define OV_AUDITED_FLAG 66
#define OV_AUDIT_COMPRESSION_FLAG 67
#define OV_DATA_COMPRESSION_FLAG 68
#define OV_INDEX_COMPRESS_FLAG 69
#define OV_REFRESH_EOF_FLAG 70
#define OV_WRITE_THROUGH_FLAG 72
#define OV_VERIFY_WRITES_FLAG 73
#define OV_SERIAL_WRITES_FLAG 74
#define OV_FILE_IS_OPEN_FLAG 75
#define OV_CRASH_OPEN_FLAG 76
#define OV_ROLLFORWARD_NEEDED_FLAG 77
#define OV_BROKEN_FLAG 78
#define OV_CORRUPT_FLAG 79
#define OV_SECONDARY_PARTITION_FLAG 80
#define OV_INDEX_LEVELS 81
#define OV_NUMBER_OF_PARTITIONS 90
#define OV_PARTITION_EXTENTS 97   /* Sol: 10-030723-8110 item 91 -> 97 */
#define OV_PARTITION_VOLUME_NAME_LENS 92
#define OV_PARTITION_VOLUME_NAMES 93
#define OV_PARTITION_PARTIAL_KEY_LENGTH 94
#define OV_PARTITION_PARTIAL_KEY_VALS 95
#define OV_NUMBER_OF_ALT_KEYS 100
#define OV_ALT_KEY_DESCRIPTORS 101
#define OV_NUMBER_OF_ALT_KEY_FILES 102
#define OV_ALT_KEY_FILE_NAME_LENS 103
#define OV_ALT_KEY_FILE_NAMES 104
#define OV_SEQUENTIAL_BLOCK_BUFFERING_FLAG 116
#define OV_OSS_OWNER_GROUP_ID 164
#define OV_OSS_ACCESS_PERMISSIONS 165
#define OV_OSS_OWNER 167
#define OV_AGGREGATE_EOF 191      /* Sol: 10-030703-7581 */
#define OV_AGGREGATE_MAX_FILE_SIZE 192 /* Sol: 10-030703-7581 */
#define OV_FILE_FORMAT 195
#define OV_LOGICAL_RECORD_LENGTH 196
#define OV_BLOCK_LENGTH 197
#define OV_KEY_OFFSET 198
#define OV_PRIMARY_EXTENT_SIZE 199
#define OV_SECONDARY_EXTENT_SIZE 200
#define OV_NEXT_RECORD_POINTER 201
#define OV_CURRENT_RECORD_POINTER 202
#define OV_CURRENT_KEY_LENGTH 203
#define OV_CURRENT_KEY_VALUE 204
#define OV_CURRENT_PRIMARY_KEY_LENGTH 205
#define OV_CURRENT_PRIMARY_KEY_VALUE 206

#define OV_PARTITION_PRIMARY_EXTENT 9996
#define OV_PARTITION_SECONDARY_EXTENT 9997
#define OV_IS_OSS_FILE 9998
#define OV_LAST_OPTVAL 9999


#define OV_LAST_OPEN_TIME 117
#define OV_EXPIRATION_TIME 118
#define OV_CREATION_TIME 119
#define OV_MODIFICATION_TIME 145

#define ERR_NOT_DISC_FILE 3001

#define OPEN_OP 0
#define CONTROL_OP 1
#define LOCKFILE_OP 2
#define LOCKREC_OP 3
#define READ_OP 4
#define SETFILEOWN_OP 5
#define SETFILESEC_OP 6
#define SETMODE_OP 7
#define UNLOCKFILE_OP 8
#define UNLOCKREC_OP 9
#define WRITE_OP 10
#define WRITEREAD_OP 11

#define MAX_LENGTH 35
#define CSERIES_LENGTH 24

#ifndef true
#define true  1
#endif

#ifndef false
#define false 0
#endif

#endif // _Included_EnscribeFile_h
