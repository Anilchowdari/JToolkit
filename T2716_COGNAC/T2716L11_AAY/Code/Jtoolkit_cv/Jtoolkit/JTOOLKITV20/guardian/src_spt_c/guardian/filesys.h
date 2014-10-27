/* ------------------------------------------------------------------------
* Filename:     filesys.h
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2001
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

#ifndef _FILESYS_MODULE
#define _FILESYS_MODULE

#include "tandem.h"

#ifdef _COMMENTS

@@@@@@@@@@@      @@@      @@@          @@@@@@@@@@@   @@@@@@@@@   @@@     @@@   @@@@@@@@@
@@@@@@@@@@@      @@@      @@@          @@@@@@@@@@@  @@@@@@@@@@@  @@@     @@@  @@@@@@@@@@@
@@@              @@@      @@@          @@@          @@@     @@@   @@@   @@@   @@@     @@@
@@@              @@@      @@@          @@@          @@@            @@@ @@@    @@@
@@@@@@@@@        @@@      @@@          @@@@@@@@@    @@@@@@@@@@      @@@@@     @@@@@@@@@@
@@@@@@@@@        @@@      @@@          @@@@@@@@@     @@@@@@@@@@      @@@       @@@@@@@@@@
@@@              @@@      @@@          @@@                  @@@      @@@              @@@
@@@              @@@      @@@          @@@          @@@     @@@      @@@      @@@     @@@
@@@              @@@      @@@@@@@@@@@  @@@@@@@@@@@  @@@@@@@@@@@      @@@      @@@@@@@@@@@
@@@              @@@      @@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@       @@@       @@@@@@@@@

#endif

#pragma PAGE "Table of Contents"

#pragma PAGE "Usage Considerations"
#ifdef _COMMENTS

Module:    FileSys
Contents:  File System Definitions
Status:    Specification
Author:    Jim Lyon


This module contains definitions of constants and 1-word structures
defined by the file system.

#endif
#pragma PAGE "Justification"
#ifdef _COMMENTS

We encapsulate these into one known place.

#endif

#pragma PAGE "Known Problems"
#ifdef _COMMENTS

This must be kept up to date to track Guardian.

#endif

#pragma PAGE "BLOCK FileSys_Defs"

/*
The implementations of the items in this BLOCK should be considered
a public, exported by Guardian.
*/

#pragma PAGE "DevType_Struct"  /*  Device Types */

/*
----------------------------------------------------------------------

       Device Types.  This word is returned by 'DEVICEINFO', et al.

       The authoritity on device types is \FOXII.$OPSYS.NEWBIBLE.DEVTYPE

======================================================================
*/

#pragma FIELDALIGN SHARED2 DevType_Struct
typedef struct  DevType_Struct
{
        unsigned int    Demountable : 1;    /* 1 iff disk pack is demountable. */

        unsigned int    Audited : 1;        /* 1 iff disk pack is audited. */

        unsigned int    : 2;                 /* reserved bits. */

        unsigned int    Type : 6;           /* device type, see below. */

        unsigned int    SubType : 6;        /* device subtype, see below. */
} DevType_Type;


/*       Some Values for the 'Type' field:     */

#define Process_DevType           0     /* process, or nonexistant device */
#define Console_DevType           1     /* operator log */
#define Receive_DevType           2     /* $RECEIVE */
#define Disk_DevType              3     /* disk volume */
#define Tape_DevType              4     /* magnetic tape */
#define Printer_DevType           5     /* line printer */
#define Terminal_DevType          6     /* TERMPROCESS terminal */
#define CardReader_DevType        8     /* card reader */
#define Tmp_DevType               21    /* TMF pseudo-file device type */

/*       Almost any openable device not in the list above should be treated     */
/*       as a terminal, in the absence of knowledge to the contrary.            */


/*       Subtypes for Type = 0 (Process):       */

#define Vanilla_ProcessType       0     /* ordinary process */
#define CMP_ProcessType           1     /* CMP */
#define SafeGuard_ProcessType     2     /* Safeguard process */
#define MlServer_ProcessType      29    /* Multilan file server */
#define Simulator_ProcessType     30    /* Process simulating anything else */
#define Spooler_ProcessType       31    /* Spool collector */


/*       Subtypes for Type = 6 (Terminals):     */

#define Dumb_TerminalType         0     /* dumb, conversational-only terminal */
#define T6510_TerminalType        1     /* ADM2 */
#define T6520_TerminalType        2     /* Zentec */
#define T6520R_TerminalType       3     /* Zentec using AM6520 */
#define T6530_TerminalType        4     /* 652x/653x/654x terminals (Dragon) */
#define T6530R_TerminalType       5     /* 652x/653x/654x using AM6520 */
/*       Many other subtypes for 3270 emulation, SNA, etc.      */

#pragma PAGE "FileType_Struct"  /*  File Types  */

/*
---------------------------------------------------------------------

       File Types.  This word is returned by 'FILEINFO', et al.

======================================================================
*/

#pragma FIELDALIGN SHARED2 FileType_Struct
typedef struct  FileType_Struct
{
        unsigned int    : 2;           /* reserved bits. */

        unsigned int    Audited : 1;        /* file is audited by TMF. */

        unsigned int    : 6;      /* reserved bits. */

	unsigned int    QueueFile : 1;      /* 1 iff file is a Queue File */
                                	/* (Type must be KeySequenced_FileType) */

        unsigned int    RefreshEof : 1;     /* 1 iff EOF must be forced to disk */
                                        /* every time that it changes. */

        unsigned int    IndexComp : 1;      /* 1 iff using index compression */
                                        /* (only for key-sequenced files). */

        unsigned int    DataComp : 1;       /* 1 iff using data compression */
                                        /* (only for key-sequenced files). */

        unsigned int    Type : 3;           /* File type.  See below. */
}  FileType_Type;

#define  /*unsigned(1)*/   OddUnstruct     DataComp
                                        /* 1 iff odd-length records allowed */
                                        /* (only for unstructured files). */


#define Unstructured_FileType     0     /* File is not structured. */
#define Relative_FileType         1     /* File is relative. */
#define EntrySequenced_FileType   2     /* File is entry-sequenced. */
#define KeySequenced_FileType     3     /* File is key-sequenced. */

#pragma PAGE "File Codes"
/*
----------------------------------------------------------------------

       File Codes.  This word is returned by 'FILEINFO', et al.

       The authoritity on device codes is \FOXII.$OPSYS.NEWBIBLE.FILECODE

======================================================================
*/

#define Object_FileCode           100   /* object code file. */
#define Edit_FileCode             101   /* file is an edit file. */
#define TText_FileCode            102   /* file is edit file + extras. */
#define Spooler_FileCode          129   /* DSPOOL spool job image file. */
#define AuditTrail_FileCode       134   /* TMF Audit Trail file. */
#define CData_FileCode            180   /* "C" Data file. */

#pragma PAGE "SQL Object Types"
/*
---------------------------------------------------------------------

       Types of SQL Objects.  These types are returned by
       the FILE_GETINFOLIST_ procedure, et al.

======================================================================
*/

#define Directory_SqlType         -1    /* object is a volume or a subvolume. */
#define NonSql_SqlType            0     /* object is not a SQL object. */
#define Table_SqlType             2     /* object is a SQL table. */
#define Index_SqlType             4     /* object is a SQL index. */
#define ProtectionView_SqlType    5     /* object is a SQL protection view. */
#define ShorthandView_SqlType     7     /* object is a SQL shorthand view. */

#pragma PAGE "OpenFlags_Struct"  /*  Flags for OPEN */
/*
---------------------------------------------------------------------

       Flags short passed to OPEN.

======================================================================
*/

#pragma FIELDALIGN SHARED2 OpenFlags_Struct
typedef struct  OpenFlags_Struct
{
        unsigned int    DontUpdateOpenTime : 1;
                                        /* 1 => that the "LastOpenTime" of a */
                                        /* disk file should not be updated as */
                                        /* a result of this open.  Must be zero */
                                        /* for non-disk files. */

        unsigned int    WantSysMsg : 1;     /* 1 => Want Open/Close/Control/Setmode */
                                        /* messages on $RECEIVE.  Must be zero */
                                        /* for all other files. */

        unsigned int    Unstructured : 1;   /* 1 => Want unstructured access to a */
                                        /* structured file. */

        unsigned int    AccessMode : 3;     /* Access mode.  See below. */

        unsigned int    Reserved6 : 2;      /* Reserved. */

        unsigned int    NoWaitOpen : 1;     /* 1 => Send open message nowait.  This */
                                        /* is not honored for all devices. */

        unsigned int    Exclusion : 3;      /* Exclusion mode.  See below. */

        unsigned int    NoWaitDepth : 4;    /* Max number of nowait requests at */
                                        /* one time.  Zero => waited I/O. */

}  OpenFlags_Type;
/*
       Values for the 'AccessMode' field:

       These values are used with 'OPEN' procedure

*/

#define ReadWrite_AccessMode      0     /* Read/Write access. */
#define Read_AccessMode           1     /* Read only access. */
#define Write_AccessMode          2     /* Write only access. */
#define Execute_AccessMode        3     /* Execute access (disk only). */
#define Extend_AccessMode         3     /* Extend access (tape only). */
#define Purge_AccessMode          4     /* Purge access (internal to FS and DP2) */
#define OnlineDump_AccessMode     5     /* OnlineDump access. */
#define Backout_AccessMode        6     /* Backout access. */
#define Recovery_AccessMode       7     /* Recovery access. (this was formerly */
                                        /*   RollForward_AccessMode.  FOR TMF3 */
                                        /*   this value will be used for 'old' */
                                        /*   OPEN's for Volume, File & Backout */
                                        /*   recovery) */

/*
       Values for the 'Exclusion' field:  These values can also be used
       as the 'Exclusion' parameter to FILE_OPEN_.
*/

#define Shared_Exclusion          0     /* Others may read or write file. */
#define Exclusive_Exclusion       1     /* Others may not access file. */
#define Process_Exclusion         2     /* Other opens in same process may */
                                        /* read or write; no other process */
                                        /* may access file. */
#define Protected_Exclusion       3     /* Others may read file. */

#pragma PAGE "OpenFlagsD00_Struct"  /*  Flags for FILE_OPEN_ */

/*
---------------------------------------------------------------------

       Flags short passed to FILE_OPEN_.

======================================================================
*/
#pragma FIELDALIGN SHARED2 OpenFlagsD00_Struct
typedef struct  OpenFlagsD00_Struct
{
        unsigned int    Unstructured : 1;   /* 1 => Want unstructured access to a */
                                        /* structured file. */

        unsigned int    NoWaitOpen : 1;     /* 1 => Send open message nowait.  This */
                                        /* is not honored for all devices. */

        unsigned int    DontUpdateOpenTime : 1;
                                        /* 1 => that the "LastOpenTime" of a */
                                        /* disk file should not be updated as */
                                        /* a result of this open.  Must be zero */
                                        /* for non-disk files. */

        unsigned int    AnyFileNumForBackup : 1;
                                        /* 1 => the file number assigned for a */
                                        /* backup open need not be the same as */
                                        /* the file number assigned for the */
                                        /* primary open.  Must be zero for */
                                        /* primary open requests. */

        unsigned int    Reserved : 1;       /* Reserved bits. */

        unsigned int    OldSysMsgFormat : 1;
                                        /* For $RECEIVE only: */
                                        /* 0 =>  Deliver system messages in the */
                                        /*       new format. */
                                        /* 1 =>  Deliver system messages in the */
                                        /*       old format. */
                                        /* Must be zero for all other files. */

        unsigned int    WantSysMsg : 1; /* 0 =>  Want Open/Close/Control/Setmode */
                                        /*       messages on $RECEIVE. */
                                        /* 1 =>  Suppress Open/Close/Control/ */
                                        /*       Setmode messages on $RECEIVE. */
                                        /* Must be zero for all other files. */

}  OpenFlagsD00_Type;

#pragma PAGE "CreateFlagsD00_Struct"  /*  Flags for FILE_CREATE_" */
/*
---------------------------------------------------------------------

       Flags short passed to FILE_CREATE_.

======================================================================
*/

#pragma FIELDALIGN SHARED2 CreateFlagsD00_Struct
typedef struct CreateFlagsD00_Struct

{
        unsigned int    : 10;       /* Reserved bits. */

        unsigned int    Refresh : 1;        /* 1 => every modification to EOF should */
                                        /* be written to disk. */

        unsigned int    IndexCompress : 1;  /* 1 => compress indices in the B-Tree. */
                                        /* Allowed only on structured files. */

        unsigned int    DataCompress : 1;   /* 1 => compress data in the B-Tree. */
                                        /* Allowed only on structured files. */

        unsigned int    AuditCompress : 1;  /* 1 => compress audit generated for */
                                        /* this file. Allowed only on audited */
                                        /* files. */

        unsigned int    Audit : 1;          /* 1 => file is TMF-protected. */

        unsigned int    OddUnstructured : 1;/* 1 => odd-length reads/writes are not */
                                        /* to be padded.  Allowed only on */
                                        /* unstructured files. */

} CreateFlagsD00_Type;

#pragma PAGE "Access Values for FILE_OPEN_"
/*
---------------------------------------------------------------------

       Following are values for the 'Access' parameter to FILE_OPEN_.


======================================================================
*/

#define ReadWrite_AccessD00       0     /* Read/Write access. */
#define Read_AccessD00            1     /* Read only access. */
#define Write_AccessD00           2     /* Write only access. */
#define Extend_AccessD00          3     /* Extend access (tape only). */
#define Execute_AccessD00         16    /* Execute access (disk only). */
#define OnlineDump_AccessD00      17    /* OnlineDump access. */
#define Backout_AccessD00         18    /* Backout access. */
#define Recovery_AccessD00        19    /* Recovery access (formerly */
                                        /*   FileRecovery_AccessD00). */
                                        /*   For TMF3 this value will */
                                        /*   be passed for all requests */
                                        /*   to FILE_OPEN of ENSCRIBE */
                                        /*   files. */

#pragma PAGE "FindStartFlags_Struct"  /*  Flags passed to FileName_FindStart_  */

/*
---------------------------------------------------------------------

       Flags word passed to FileName_FindStart_.

======================================================================
*/

#pragma FIELDALIGN SHARED2 FindStartFlags_Struct
typedef struct FindStartFlags_Struct
{
        unsigned int    Reserved : 9;       /* Must be zero. */

        unsigned int    Nowait : 1;         /* Perform FileName_FindNext_ nowaited. */

        unsigned int    NoDevSim : 1;       /* Don't let subtype 30 processes */
                                        /* simulate other devices. */

        unsigned int    NoSubprocesses : 1; /* Don't return subprocesses within */
                                        /* user processes. */

        unsigned int    ReportOffline : 1;  /* Report errors when part of the */
                                        /* name space is offline. */

        unsigned int    NotDevSubType : 1;  /* Invert match sense on device subtype. */

        unsigned int    NotDevType : 1;     /* Invert match sense on device type. */

        unsigned int    SkipIfSame : 1;     /* Don't return entity matching the */
                                        /* StartName. */

} FindStartFlags_Type;
#pragma PAGE "Security_Struct - File Security Information"

/*
---------------------------------------------------------------------

       File security vectors.  This struct is in the form used by Param1
       of 'Security_SetMode'.

======================================================================
*/

#pragma FIELDALIGN SHARED2 Security_Struct
typedef struct  Security_Struct
{
        unsigned int    ProgId : 1;         /* 1 => this program will always run */
                                        /* using its owner's user id. */

        unsigned int    ClearOnPurge : 1;   /* 1 => data will be erased when the */
                                        /* file is purged. */

        unsigned int    : 2;       /* reserved bits. */

        unsigned int    ReadSecurity : 3;   /* Who is allowed to read this file. */
                                        /* See the 'xxxx_Security' #defines. */

        unsigned int    WriteSecurity : 3;  /* Who is allowed to write this file. */
                                        /* See the 'xxxx_Security' #defines. */

        unsigned int    ExecuteSecurity : 3;/* Who is allowed to execute this file. */
                                        /* See the 'xxxx_Security' #defines. */

        unsigned int    PurgeSecurity : 3;  /* Who is allowed to purge this file. */
                                        /* See the 'xxxx_Security' #defines. */

}  Security_Type;


/*
----------------------------------------------------------------------

       Values for the 'xxxxSecurity' fields above.

       The First letter of each of the #defines below correspond to
       the letter used when typing a security vector, except that
       'Super_Security' is typed as "-".

======================================================================
*/

#define Any_Security              0     /* Any local user is permitted. */
#define Group_Security            1     /* Any local user belonging to the */
                                        /* same group as the owner is permitted. */
#define Owner_Security            2     /* The local owner is permitted. */
#define Network_Security          4     /* Any network user is permitted. */
#define Community_Security        5     /* Any network user belonging to the */
                                        /* same group as the owner is permitted. */
#define User_Security             6     /* The owner is permitted. */
#define Super_Security            7     /* Only the local SUPER.SUPER is */
                                        /* permitted. */
#pragma PAGE "Control Operations"
/*
----------------------------------------------------------------------

       Function codes for the CONTROL system procedure.
       In the following descriptions, the parameter value is referred
       to as 'p'.

       Those lines below that contain values but no #define name
       correspond to CONTROL functions that have been assigned to
       various groups, but have either not been released, or are
       too poorly documented to understand.

       The authority on CONTROL functions is \FOXII.$OPSYS.NEWBIBLE.CTRLCODE.

======================================================================
*/

#define NewPage_Control           1     /* Printer:  Vertical spacing: */
                                        /*  p = 0 => send FF (page eject) */
                                        /*  p = 1..15 => send VT (vertical tab) */
                                        /*       or skip to carriage channel p */
                                        /*  p > 15 => send (p-16) LF's */

#define WriteEof_Control          2     /* Tape:  Write tape mark; */
                                        /* Disk file:  Set EOF to current */
                                        /*       position. */

#define UnloadTape_Control        3     /* Tape:  Rewind and unload reel. */
                                        /*       Returns immediately. */

#define OfflineTape_Control       4     /* Tape:  Take tape offline. */

#define RewindNoWait_Control      5     /* Tape:  Rewind tape.  Returns */
                                        /*       immediately. */

#define Rewind_Control            6     /* Tape:  Rewind tape.  Returns after */
                                        /*       tape is rewound. */

#define SkipForward_Control       7     /* Tape:  Skip forward past (p) */
                                        /*       tape marks. */

#define SkipBackward_Control      8     /* Tape:  Skip backward past (p) */
                                        /*       tape marks. */

#define SkipRecords_Control       9     /* Tape:  Skip forward past (p) records. */

#define Backspace_Control         10    /* Tape:  Backspace over (p) records. */

#define Connect_Control           11    /* Terminal:  Wait for modem connection. */
                                        /*       Returns after connection is */
                                        /*       established. */

#define Disconnect_Control        12    /* Terminal:  Hang up modem. */

/*                                13..19 */  /* unassigned */

#define PurgeData_Control         20    /* Disk file:  Purge all data in */
                                        /*       the file. */

#define Allocate_Control          21    /* Disk file:  Allocate/deallocate */
                                        /*       extents: */
                                        /*  p = 0 => deallocate unused extents. */
                                        /*  p > 0 => Make the file have (p) */
                                        /*       extents. */

/*                                22    */  /* CTRL_READINIT: reserved for AXCESS */

#define PseudoFileFull_Control    23    /* Disk file:  Force subsequent write */
                                        /*       attempts to result in error */
                                        /*       FEFILEFULL. */

#define EndOfVolume_Control       24    /* Tape:  For labelled output tape, */
                                        /*       go to the next reel. */

/*                                25    */ /* CTRL_SETFIXUPID: modify file label */
                                        /*       "fixed up volid" field */

/*                                26    */  /* terminate I/O similar to cancel. */

#define WaitForWrite_Control      27    /* Disk file:  Wait for the next write */
                                        /*       to the file.  Returns when */
                                        /*       some other opener next writes */
                                        /*       to the file. */

/*                                28    */ /* reserved for printer. */

/*                                29    */ /* reserved for DDN. */

/*                              = 30..39 */ /* Reserved for customer use. */

#pragma PAGE "SetMode Operations"
/*
---------------------------------------------------------

       Operation codes for the SETMODE and SETMODENOWAIT
       system procedure.

       In the following descriptions, the parameter values are referred
       to as 'p1' and 'p2'.

       Those lines below that contain values but no #define name
       correspond to SETMODE functions that have been assigned to
       various groups, but have either not been released, or are
       too poorly documented to understand.

       The authority on SETMODE operations is \FOXII.$OPSYS.NEWBIBLE.SETMODE.

======================================================================
*/

#define Security_SetMode          1     /* Disk file: Set security: */
                                        /*  p1 = Security_Struct. */

#define Owner_SetMode             2     /* Disk file: Set ownership: */
                                        /*  p1 = UserId */

#define VerifyWrites_SetMode      3     /* Disk file: Set write verify toggle: */
                                        /*  p1 = 0 => don't verify writes, */
                                        /*  p1 = 1 => verify writes. */
                                        /*  p2 = 0 => affect this open only */
                                        /*  p2 = 1 => affect all future opens */

#define LockWait_SetMode          4     /* Disk file: Set lock wait mode: */
                                        /*  p1 = 0 => (normal) suspend on lock */
                                        /*               conflict. */
                                        /*  p1 = 1 => (reject) return FELOCKED */
                                        /*                on lock conflict. */
                                        /*  p1 = 2 => (readthru/normal) READ */
                                        /*               ignores locks; READLOCK */
                                        /*               suspends on conflict. */
                                        /*  p1 = 3 => (readthru/reject) READ */
                                        /*               ignores locks; READLOCK */
                                        /*               returns FELOCKED on */
                                        /*               conflict. */
                                        /*  p1 = 6 => (readwarn/normal) READ */
                                        /*               returns FEREADLOCKED */
                                        /*               with record; READLOCK */
                                        /*               suspends on conflict. */
                                        /*  p1 = 7 => (readwarn/reject) READ */
                                        /*               returns FEREADLOCKED */
                                        /*               with record; READLOCK */
                                        /*               returns FELOCKED on */
                                        /*               conflict. */

#define PerforationSkip_SetMode   5     /* Printer: Set automatic skip over */
                                        /* page performation: */
                                        /*  p1 = 0 => don't skip (66 lines/page) */
                                        /*  p1 = 1 => skip (60 lines/page) */


#define LineSpacing_SetMode       6     /* Printer,Terminal: Set line spacing */
                                        /* characters for output lines: */
                                        /*  p1,p2 = 0,0 => append CR only */
                                        /*  p1,p2 = 0,1 => append nothing */
                                        /*  p1,p2 = 1,0 => append CR,LF */

#define AutoLineFeed_SetMode      7     /* Terminal: Set automatic output of LF */
                                        /* after input lines: */
                                        /*  p1 = 0 => don't send LF */
                                        /*  p1 = 1 => send LF */

#define BlockMode_SetMode         8     /* Terminal: Set block mode: */
                                        /*  p1 = 0 => conversational mode */
                                        /*  p1 = 1 => block mode */

#define shorterruptChars_SetMode  9     /* Terminal: Set interrupt characters: */
                                        /*  p1.<0:7>, p1.<8:15>, p2.<0:7> and */
                                        /*  p2.<8:15> = the interrupt chars. */

#define ParityChecking_SetMode    10    /* Terminal: Set parity checking of */
                                        /* input characters: */
                                        /*  p1 = 0 => don't check parity */
                                        /*  p1 = 1 => check parity */

#define BreakOwnership_SetMode    11    /* Terminal: Set ownership of break */
                                        /* key: */
                                        /*  p1 = 0 => ignore break key */
                                        /*  p1 = MYPID => I get the break key */
                                        /*  p1 = value previously returned in */
                                        /*       OldParams => give break back */
                                        /*       to some other process. */
                                        /*  p2 = 0 => put terminal in normal */
                                        /*       mode after break is pressed. */
                                        /*  p2 = 1 => put terminal in break */
                                        /*       mode after break is pressed. */

#define BreakMode_SetMode         12    /* Terminal:  Set break mode: */
                                        /*  p1 = 0 => (normal) Any opener may */
                                        /*       access the terminal. */
                                        /*  p1 = 1 => (break) Only openers in */
                                        /*       break mode may access terminal. */
                                        /*  p2 = 0 => (normal) Put current open */
                                        /*       into normal mode. */
                                        /*  p2 = 1 => (break) Put current open */
                                        /*       into break mode. */

#define EtxTermination_SetMode    13    /* Terminal:  Set read termination on */
                                        /*       receipt of ETX character: */
                                        /*  p1 = 0 => don't terminate on ETX. */
                                        /*  p1 = 1 => stop 1 char after ETX. */
                                        /*  p1 = 2 => stop 2 chars after ETX. */

#define ReadTermination_SetMode   14    /* Terminal:  Set read termination on */
                                        /*       receipt of interrupt char mode: */
                                        /*  p1 = 0 => don't terminate read */
                                        /*  p1 = 1 => terminate read */

/*                                15    */ /* SM_RETRY: set recovery data */
/*                                16    */ /* SM_LINEPARM: set line parameters */
/*                                17    */ /* SM_BLOCKSIZE: set intermediate block */
/*                                      */ /*       size */
/*                                18    */ /* SM_THRESHOLD: set line statistics */
/*                                      */ /*       threshold */
/*                                19    */ /* SM_TRACE: set trace table */

#define Echo_SetMode              20    /* Terminal:  Set echo of input chars: */
                                        /*  p1 = 0 => don't echo input */
                                        /*  p1 = 1 => echo input */

#define CardReader_SetMode        21    /* CardReader:  Set input mode: */
                                        /*  p1 = 0 => ASCII */
                                        /*  p1 = 1 => column binary */
                                        /*  p1 = 2 => packed binary */

#define LineSpeed_SetMode         22    /* Printer,Terminal:  Set line speed: */
                                        /*  p1 =  0 =>    50   baud */
                                        /*  p1 =  1 =>    75   baud */
                                        /*  p1 =  2 =>   110   baud */
                                        /*  p1 =  3 =>   134.5 baud */
                                        /*  p1 =  4 =>   150   baud */
                                        /*  p1 =  5 =>   300   baud */
                                        /*  p1 =  6 =>   600   baud */
                                        /*  p1 =  7 =>  1200   baud */
                                        /*  p1 =  8 =>  1800   baud */
                                        /*  p1 =  9 =>  2000   baud */
                                        /*  p1 = 10 =>  2400   baud */
                                        /*  p1 = 11 =>  3600   baud */
                                        /*  p1 = 12 =>  4800   baud */
                                        /*  p1 = 13 =>  7200   baud */
                                        /*  p1 = 14 =>  9600   baud */
                                        /*  p1 = 15 => 19200   baud */
                                        /*  p1 = 16 =>   200   baud */

#define CharSize_SetMode          23    /* Printer,Terminal: Set character size: */
                                        /*  p1 = 0 => 5-bit characters */
                                        /*  p1 = 1 => 6-bit characters */
                                        /*  p1 = 2 => 7-bit characters */
                                        /*  p1 = 3 => 8-bit characters */

#define ParityGeneration_SetMode   24   /* Printer,Terminal: Set parity creation */
                                        /* on output: */
                                        /*  p1 = 0 => set odd parity */
                                        /*  p1 = 1 => set even parity */
                                        /*  p1 = 2 => leave parity bit alone */

#define PageSize_SetMode          25    /* Printer: Set page size: */
                                        /*  p1 = page length, in lines */

#define VerticalTab_SetMode       26    /* Printer: Set/clear vertical tabs: */
                                        /*  p1 = -1 => clear all vert. tabs */
                                        /*  p1 > -1 => set tab at line p1 */
                                        /*       (counting from zero). */


#define PreSpace_SetMode          27    /* Printer,Terminal:  Set spacing */
                                        /* before output lines: */
                                        /*  p1 = 0 => space after output */
                                        /*  p1 = 1 => space before output */

#define ResetTerminal_SetMode     28    /* Printer,Terminal:  Reset device to */
                                        /* default settings: */
                                        /*  p1 = 0 (required) */

#define AutoAnswer_SetMode        29    /* Printer,Terminal:  Set modem for */
                                        /* automatic answer: */
                                        /*  p1 = 0 => don't automatically answer */
                                        /*  p1 = 1 => automatically answer phone */

#define AnyOrder_SetMode          30    /* Any:  Allow nowait operations to */
                                        /* complete in any order: */
                                        /*  p1 = 0 => oldest operation always */
                                        /*       completes first. */
                                        /*  p1 = 1 => operations complete in */
                                        /*       any order. */

#define PacketSize_SetMode        31    /* X25: Set packet size: */
                                        /*  p1 = 0 => no operation. */
                                        /*  p1 = 1 => set packet size from p2. */
                                        /*  p2 = 0 => use default packet size. */
                                        /*  p2 > 0 => p2 is size of packet, in */
                                        /*       bytes. */

#define CallOptions_SetMode       32    /* X25: Set call options: */
                                        /*  p1.<0> = 0 => don't accept collect */
                                        /*               calls. */
                                        /*  p1.<0> = 1 => accept collect calls. */
                                        /*  p1.<1> = 0 => place normal call. */
                                        /*  p1.<1> = 1 => place collect call. */
                                        /*  p1.<2> = 0 => non-priority call. */
                                        /*  p1.<2> = 1 => priority call. */
                                        /*  p1.<8:15> = port number. */

#define SevenTrackTape_SetMode    33    /* Tape: Set 7-track character assembly */
                                        /* mode: */
                                        /*  p1 = 0 => convert ASCII to 6bit BCD */
                                        /*  p1 = 1 => 3 bytes in memory is 4 */
                                        /*       characters on tape (odd parity) */
                                        /*  p1 = 2 => 2 bytes in memory is 3 */
                                        /*       characters on tape (odd parity) */
                                        /*  p1 = 3 => 1 byte in memory is 1 */
                                        /*       character on tape (odd parity) */

/*                                34    */ /* SM_SETTILTIMEOUT: set timeout for */
/*                                      */ /*       Tandem-to-IBM LINK */
/*                                35    */ /* SM_TRANSLATE: set translation mode */

#define PriorityQueueing_SetMode  36    /* Receive:  Queue requests in order */
                                        /* of priority of requester: */
                                        /*  p1 = 0 => queue requests FIFO */
                                        /*  p1 = 1 => queue by priority */

#define PrinterStatus_SetMode     37    /* Printer:  Get current status into */
                                        /*       LastParams. */

#define SpecialLineTerm_SetMode   38    /* Terminal: Set MUMPS special line */
                                        /*       termination mode: */
                                        /*  p1 = 0 => terminate read on receipt */
                                        /*       of char equal to p2.<8:15>. */
                                        /*       Don't include char in count */
                                        /*       read; don't echo CR or LF. */
                                        /*  p1 = 1 => terminate read on receipt */
                                        /*       of char equal to p2.<8:15>. */
                                        /*       Include char in count read; */
                                        /*       don't echo CR or LF. */
                                        /*  p2 = 2 => Clear special line */
                                        /*       termination mode. */

#define IgnoreChecksum_SetMode    39    /* Disk file:  Ignore checksum errors: */
                                        /*  p1 = 0 => don't ignore errors */
                                        /*  p1 = 1 => ignore errors */

/*                                40..49 *  reserved for EXCHANGE */

#define Enable3270Copy_SetMode    50    /* AM3270:  Enable COPY function: */
                                        /*  p1 = 0 => disable COPY function */
                                        /*  p1 = 1 => enable COPY function */

#define Set3270Status_SetMode     51    /* TR3271:  Set status: */
                                        /*  p1 = status flags */

#define SuppressShortWrite_SetMode   52 /* Tape:  Allow short writes (less than */
                                        /*       24 bytes): */
                                        /*  p1 = 0 => allow short writes */
                                        /*  p1 = 1 => suppress short writes */

#define Enable3270Status_SetMode   53   /* AM3270:  Enable receipt of */
                                        /* 'T3270Status_SysMsg' messages: */
                                        /*  p1 = 0 => disable receipt */
                                        /*  p1 = 1 => enable receipt */

#define Get3270CuStatus_SetMode   54    /* AM3270:  Get control-unit status. */
                                        /*  LastParams = current status. */

/*                                55    */ /* SM_STATUSREPORT: set status (error) */
/*                                      */ /*       reporting mode for THSL */
/*                                56    */ /* SM_SETASSEMMODE: set (dis)assembly */
/*                                      */ /*       mode for THSL */

#define SerialWrites_SetMode      57    /* Disk file: Set ordering of writes */
                                        /* to primary and mirror disks: */
                                        /*  p1 = 0 => parallel writes allowed */
                                        /*  p1 = 1 => serial writes required */
                                        /*  p2 = 0 => affect this open only */
                                        /*  p2 = 1 => affect all future opens */

/*                                58    */ /* SM_READPENDING: (internal only) */
/*                                      */ /*       terminal simulator helper */
/*                                59    */ /* SM_2STEPSIZE: return actual read size */
/*                                      */ /*       for a 2-step read */
/*                                60    */ /* SM_SMEARPASTEOF:  obsolete. */

#define DialBufferLength_SetMode  61    /* X21:  Set length of dial buffer. */
                                        /*  p1 = new length, in bytes. */

#define TapeDensity_SetMode       66    /* Tape: Set tape density. */
                                        /*  p1 = 0 => 800 BPI */
                                        /*  p1 = 1 => 1600 BPI */
                                        /*  p1 = 2 => 6250 BPI */
                                        /*  p1 = 3 => honor switches on drive */

#define AutoDisconnect_SetMode    67    /* Terminal: Automatically hang up modem */
                                        /* on loss of carrier. */
                                        /*  p1 = 0 => don't do this */
                                        /*  p1 = 1 => do this */

#define HorizontalPitch_SetMode   68    /* Printer:  Set horizontal pitch: */
                                        /*  p1 = 0 => normal pitch */
                                        /*  p1 = 1 => condensed pitch */
                                        /*  p1 = 2 => expanded pitch */

/*                                69    */ /* reserved for TERMPROCESS */

#define BufferedIo_SetMode        70    /* All:  Set whether file system will */
                                        /* copy the user's buffer: */
                                        /*  p1 = 0 => FS may use caller's buffer */
                                        /*       in place */
                                        /*  p1 = 1 => FS must copy caller's */
                                        /*       buffer */
                                        /* Obsolete as of D00. */

#define ServiceClass_SetMode      71    /* All:  Set EXPAND service class for */
                                        /* requests. */

#define ForceSystemBuffering_SetMode   72
                                        /* All:  Force system buffering: */
                                        /*  p1 = 0 =>  Use the caller's buffer */
                                        /*       directly when sending messages. */
                                        /*  p1 = 1 =>  Copy caller's buffer */
                                        /*       to PFS before sending mesages. */

#define NoRetry_SetMode           73    /* Process: Diable automatic retry. */
                                        /*  p1 = 0 =>  Automatically retry to */
                                        /*       server's backup on path errors. */
                                        /*  p1 = 1 =>  Don't automatically retry. */

#define NoTimeOutCancel_SetMode   74    /* All:  Control cancelling outstanding */
                                        /* operations when AWAITIOX times out: */
                                        /*  p1 = 0 => Cancel oldest operation. */
                                        /*  p1 = 1 => Don't cancel any operation. */

/*                                75..79 * reserved for FILESYSTEM */
/*                                      */ /*       local modes */

#define ReceiveOptions_SetMode    80    /* Receive:  Enable new functions: */
                                        /*  p1.<13> = 1 => Enable Cancel */
                                        /*       messages. */
                                        /*  p1.<14> = 1 => Enable SetParam */
                                        /*       messages. */
                                        /*  p1.<15> = 1 => Enable new form of */
                                        /*       SetMode messages. */

#define NoListen_SetMode          81    /* Receive:  Diable calling Msg_Listen_. */
                                        /* p1 = 0 => Normal behavior of $RECEIVE. */
                                        /* p1 = 1 => $RECEIVE can only complete */
                                        /*           nowait operations like */
                                        /*           DeviceInfo2 and */
                                        /*           File_FileNext_. */
                                        /* You must be PRIV when calling SetMode */
                                        /* with this option. */

#define ForceRemoteTmf_SetMode    82    /* Process:  For remote TMF negotiation */
                                        /* even if server is local. */
                                        /* p1,p2 = remote node number, or -1D */
                                        /* to revert to normal behavior.  You */
                                        /* must be PRIV when calling SetMode */
                                        /* with this option. */

#define WriteThru_SetMode         90    /* Disk file:  Set write-thru option: */
                                        /*  p1 = 0 => Buffer writes in cache. */
                                        /*  p1 = 1 => Write thru to disk. */
                                        /*  p2 = 0 => affect this open only */
                                        /*  p2 = 1 => affect all future opens */

#define CacheMode_SetMode         91    /* Disk file:  Set cache protocol: */
                                        /*  p1 = 0 => system-mananged. */
                                        /*  p1 = 1 => bypass cache. */
                                        /*  p1 = 2 => random access. */
                                        /*  p1 = 3 => sequential access. */

#define MaxExtents_SetMode        92    /* Disk file:  Set max extents allowed: */
                                        /*  p1 = maximum number of extents */

#define DiskBufferLength_SetMode  93    /* Disk file:  Set cache buffer length */
                                        /* for unstructured files */
                                        /*  p1 = new buffer length, in bytes */
                                        /*       (must be a power of 2) */

#define AuditCompress_SetMode     94    /* Disk file:  Set compression of */
                                        /* audit records */
                                        /*  p1 = 0 => don't compress audit */
                                        /*  p1 = 1 => compress audit */
                                        /*  p2 = 0 => affect this open only */
                                        /*  p2 = 1 => affect all future opens */

#define Flush_SetMode             95    /* Disk file:  Flush dirty buffers */

#define ResetBroken_SetMode       96    /* Disk file:  Reset the broken flag */

#define License_SetMode           97    /* Disk file:  Set license flag. */
                                        /*  p1 = 0 => clear license flag. */
                                        /*  p1 = 1 => set license flag. */

/*                                98    */ /* reserved for DP2 */

#define BufferedTape_SetMode      99    /* Tape:  Set Buffered write mode. */
                                        /*  p1 = 0 => don't buffer writes */
                                        /*  p1 = 1 => buffer writes */

/*                                100..109 */ /* reserved for customers */

#define Siso_SetMode              110   /* Terminal:  Set SISO protocol for */
                                        /* folding 8bit characters to 7 bits. */
                                        /*  p1 = 0 => disable protocol. */
                                        /*  p1 = 1 => enable protocol. */

#define Diagnostic_SetMode        111   /* Printer:  Set Diagnostic mode. */
                                        /*  p1 = 0 => normal mode. */
                                        /*  p1 = 1 => diagnostic mode. */

/*                                112   */ /* reserved for SNAX */

#define ScreenSize_SetMode        113   /* Terminal:  Set screen size: */
                                        /*  p1 = width (40,66,80,132) */
                                        /*  p2 = height (25,28,??) */

/*                                114   */ /* reserved for TERMPROCESS */
/*                                115   */ /* reserved for SNAX */
/*                                116   */ /* reserved for ENVOYACP */

#define SuppressTransId_SetMode   117   /* Process:  Suppress sending TransId, */
                                        /* if any. */
                                        /*  p1 = 0 => don't suppress transid. */
                                        /*  p1 = 1 => suppress transid. */

/*                                118   */ /* Level 1 NAK format for RMIP */
/*                                119   */ /* Tape streaming */

#define AllowEot_SetMode          120   /* Tape:  Return physical end-of-tape */
                                        /* to user. */
                                        /*  p1 = 0 => don't do this */
                                        /*  p1 = 1 => do this */

/*                                121:122 */ /* reserved for TAPESTRY */

#define GenericLockLength_SetMode 123   /* Disk file: Set length of key prefix */
                                        /* for generic locking. */
                                        /*  p1 = 0 => use whole key. */
                                        /*  p1 > 0 => use first p1 bytes of key. */

/*                                124   */ /* SM_WEIGHED_CACHE: set cache block */
/*                                      */ /*       weighing (DP2) */
/*                                125   */ /* SM_RMIACCESS: access RMI destructive */
/*                                      */ /*       commands */
/*                                126   */ /* reserved for DP2 */

#define NoReadThruDelete_SetMode  127   /* Disk file: Cause sequential reads */
                                        /*       to wait when they encounter */
                                        /*       a locked deleted record. */
                                        /*  p1 = 0 => Don't do this; return */
                                        /*       the next record. */
                                        /*  p1 = 1 => Do this. */

#define QueueFileTimeout_SetMode  128   /* Disk Queue file: set dequeuing timeout */
                                        /*       limit, in hundredths of a second: */
                                        /*  p1 = high-order 16 bits of timeout. */
                                        /*  p2 = low-order 16 bits of timeout. */

#define QueueFileBlocks_SetMode   129   /* Disk Queue file: Get number of blocks */
                                        /*       used by file, in last-params. */

/*                                130   */ /* SM_IMMEDIATE_RR: reserved for X25 */
/*                                131   */ /* SM_REDIRECTION: reserved for X25 */
/*                                132..134 */ /* reserved for X25 */
/*                                135..137 */ /* reserved for ENVOY */

#define CorruptFlag_SetMode       138   /* Disk file:  Set/clear the corrupt */
                                        /* flag.  This flag indicates that the */
                                        /* Btree structure may be incomplete or */
                                        /* incorrect. */
                                        /*  p1 = 0 => clear the CORRUPT flag. */
                                        /*  p1 = 1 => set the CORRUPT flag. */

/*                                139   */ /* SM_CHECK_BLOCKS: set/reset block */
/*                                      */ /*       checks on unstr reads(dp2) */
/*                                140   */ /* reserved for DDN */

#define LargeTransfer_SetMode     141   /* Disk file:  Allow large reads/writes */
                                        /*  p1 = 0 => normal mode. */
                                        /*  p1 = 1 => large xfer mode.  All */
                                        /*       reads/writes must be multiples */
                                        /*       of 2Kbytes, and located at 2K */
                                        /*       byte boundaries in the file. */

/*                                142   */ /* select character set */
/*                                143   */ /* SM_SQLFLAGS: set/reset SQL object */
/*                                      */ /*       flags */
/*                                144   */ /* SNAX LU - return char set, */
/*                                      */ /*       for KANJI support */

#define InsertionOrder_SetMode    145   /* Disk file:  Set insertion ordering */
                                        /* feature for alternate keys. */
                                        /*  p1 = 0 => disable this feature. */
                                        /*  p1 = 1 => enable this feature. */

#define Control27_SetMode         146   /* Disk file:  Special operating mode */
                                        /*       for Control 27 operations: */
                                        /*  p1 = 0 => A write will satisfy */
                                        /*       all Control 27's outstanding. */
                                        /*  p1 = 1 => A write will satisfy a */
                                        /*       single outstanding Control 27. */

#define IgnoreDsr_SetMode         147   /* Terminal:  Ignore DSR handshake line */
                                        /*       when executing Control 11. */
                                        /*  p1 = 0 => Control 11 waits for DSR */
                                        /*       to be asserted. */
                                        /*  p1 = 1 => Control 11 ignores DSR. */

/*                                148   */ /* Reserved for DP2. */

#define InsertionLocking_SetMode   149  /* Disk file:  Lock alternate key record */
                                        /*       during inserts.  Valid only on */
                                        /*       non-auditted files. */
                                        /*  p1 = 0 => Never lock alternate key */
                                        /*       records. */
                                        /*  p1 = 1 => During insert, lock alt. */
                                        /*       key records until the all alt. */
                                        /*       keys have been processed, then*/
                                        /*       unlock alt. key records. */

/*                                150   */ /* SNAX/CDF Start Session */
/*                                151   */ /* Temp disk file inquiry */
/*                                152:161  */ /* Reserved for DP2 */
/*                                162   */ /* Tapeprocess */
/*                                163   */ /* set enhanded CDE for SNAX */
/*                                164   */ /* SYSWAY (get a system nuber) */
/*                                165   */ /* T9064 toggle ER mode */
/*                                166:170  */ /* reserved for ENVOY ACP/XF */

/*                                255   */ /* IOPRM inhibit block mode */
/*                                256   */ /* IOPRM trace enable/disable */
/*                                257   */ /* IOPRM functions */

/*                                260   */ /* SM_PCL_POSTSCRIPT (Spooler) */

#pragma PAGE "Completion Codes"
/*
---------------------------------------------------------

       Completion Codes.  These values are returned in Stop/Abend
       System Messages.  They describe why or how a process stopped.

       The authority on completion codes is \FOXII.$OPSYS.NEWBIBLE.COMPCODE.

======================================================================
*/

#define NetFailure_CompCode       -5
                                /* Communication has been lost with the node */
                                /* on which the process is/was running. The */
                                /* process may or may not still exist, and */
                                /* it is impossible to tell which.  This value */
                                /* does not actually occur in a Stop/Abend */
                                /* message, but is reserved to describe */
                                /* this event. */

#define CpuFailure_CompCode       -4
                                /* The process died when its CPU failed.  This */
                                /* value does not actually occur in a Stop/Abend */
                                /* message, but is reserved to describe this */
                                /* event. */

#define BadParams_CompCode        -3
                                /* The process voluntarily terminated, but */
                                /* it passed bad parameters to STOP or ABEND. */

#define ResourceFailure_CompCode   -2
                                /* The process voluntarily terminated, but */
                                /* GUARDIAN had insufficient resources to */
                                /* capture the actual completion information. */

#define Trap_CompCode             -1
                                /* The process suffered a trap which it was */
                                /* not prepared to handle. */

#define Normal_CompCode           0
                                /* The process completed normally. */

#define Warning_CompCode          1
                                /* The process completed, but issued warnings. */

#define Error_CompCode            2
                                /* The process completed, but found errors. */

#define Fatal_CompCode            3
                                /* The process found a fatal error, and */
                                /* terminated prematurely. */

#define NotStarted_CompCode       4
                                /* The process never got started.  Usually, */
                                /* this indicates either a NewProcess error, */
                                /* or that CMON refused to allow the program */
                                /* to be run.  This value does not actually */
                                /* occur in a Stop/Abend message, but is */
                                /* reserved to describe this event. */

#define Abend_CompCode            5
                                /* The process voluntarily called ABEND. */

#define ForcedStop_CompCode       6
                                /* Another process forced this process to */
                                /* STOP or ABEND. */

#define Restart_CompCode          7
                                /* Whoever decided to stop this process */
                                /* thought that it might be useful to */
                                /* start the process over from the beginning. */

#pragma PAGE "Receive Types"
/*
---------------------------------------------------------------------

       $RECEIVE Message Types.  This word is returned by 'RECEIVEINFO'.

======================================================================
*/

#define SysMsg_ReceiveType        0     /* Message is a system message. */
#define Write_ReceiveType         1     /* Requester called 'Write'. */
#define Read_ReceiveType          2     /* Requester called 'Read'. */
#define WriteRead_ReceiveType     3     /* Requester called 'WriteRead'. */

#pragma PAGE "ReceiveInfo_Struct"
/*
---------------------------------------------------------------------

       ReceiveInfo_Struct:  Information returned by FILE_RECEIVEINFO_.

======================================================================
*/

 
#pragma FIELDALIGN SHARED2  ReceiveInfo_Struct
typedef struct  ReceiveInfo_Struct
{
        short             ReceiveType;
                                /* Type of incoming message.  See the */
                                /* 'xxxx_ReceiveType' #defines. */

        short             MaxReplyLen;
                                /* Length of largest accepted reply. */
                                /* A 'ByteLen'. */

        short             MessageTag;
                                /* Tag for this message.  You must pass */
                                /* this tag to REPLY. */

        short             FileNumber;
                                /* File number used by the requester when */
                                /* he sent this message.  Zero for system- */
                                /* generated messages. */

        long             SyncId; /* Synchronization Id for this message. */

        short            PHandle[10];
                                /* PHandle of the sender of this message. */
                                /* 'Nil_PHandle' for many system-generated */
                                /* messages. */

        short             OpenLabel;
                                /* Value used in the reply to the open message */
                                /* when the file was first opened.  This value */
                                /* will be -1 if the message came from a */
                                /* pre-D00 system. */

    }  ReceiveInfo_Type;

    /*changes begin for TSMP 2.4*/
    
#pragma FIELDALIGN SHARED2  ReceiveInfo2_Struct
typedef struct ReceiveInfo2_Struct
{
    	short   ReceiveType;
    	short   FileNumber;
    	short   MessageTag;
    	short   OpenLabel;
    	long	MaxReplyLen; 
    	long    SyncId;
    	short   PHandle[10];
    	short	DialogInfo; // Additional field
}  ReceiveInfo2_Type;

    
    /*changes end for TSMP 2.4  */
    
    
 typedef union   ReceiveInfo_t  ReceiveInfo;
 union ReceiveInfo_t
{
	ReceiveInfo_Type rcvInfo; //34 bytes
	ReceiveInfo2_Type rcvInfo2; // 38 bytes
} ;

    
#pragma PAGE "File Information List Codes"
/*
----------------------------------------------------------------

       File Information List Codes.  These codes are used by the
       FILE_CREATELIST_, FILE_ALTERLIST_, FILE_GETINFOLIST_ and
       FILE_GETINFOLISTBYNAME_ procedures.

       Not all of these codes are usable with all procedures.  Each
       #define is followed by a set of keywords indicating which procedures
       can use that code, as follows:
               CREATE  - code is usable by FILE_CREATELIST_.
               ALTER   - code is usable by FILE_ALTERLIST_.
               NAME    - code is usable by FILE_GETINFOLISTBYNAME_.
       All codes are usable by FILE_GETINFOLIST_.

======================================================================
*/

#define FileNameLen_InfoCode              1     /* (GetInfoList only) */
                /* short   Length of the fully qualified name that the file had */
                /*       at the time that it was opened.  A 'ByteLen'. */

#define FileName_InfoCode                 2     /* (GetInfoList only) */
                /* char[0:?]  char-form fully qualified name that the file */
                /*       had at the time that it was opened. */

#define CurFileNameLen_InfoCode           3     /* (GetInfoList only) */
                /* short   Length of the fully qualified name that the file */
                /*       currently has.  A 'ByteLen'. */

#define CurFileName_InfoCode              4     /* (GetInfoList only) */
                /* char[0:?]  char-form fully qualified name that the file */
                /*       currently has. */

#define DefineNameLen_InfoCode            5     /* (GetInfoList only) */
                /* short   Length of the DEFINE name that was opened.  If no */
                /*       define was used, then this length is zero. */
                /*       A 'ByteLen'. */

#define DefineName_InfoCode               6     /* (GetInfoList only) */
                /* char[0:?]  char-form name of the DEFINE that was opened. */
                /*       If no define was used, this string is null. */

#define LastError_InfoCode                7     /* (GetInfoList only) */
                /* short   The file system error code for the most recently */
                /*       performed operation.  Zero implies no error. */

#define LastErrorDetail_InfoCode          8     /* (GetInfoList only) */
                /* short   The error code detail for the most recently performed */
                /*       operation. */

#define PartInError_InfoCode              9     /* (GetInfoList only) */
                /* short   For partitioned files, the number of the partition on */
                /*       which an error most recently occurred. */

#define AltKeyInError_InfoCode            10    /* (GetInfoList only) */
                /* short   For files with alternate keys, the specifier of the */
                /*       key associated with the most recent error. */

#define NextRecord_InfoCode               11    /* (GetInfoList only) */
                /* long  For disk files other than key-sequenced, the RBA */
                /*       of the next record to read/write. */

#define CurRecord_InfoCode                12    /* (GetInfoList only) */
                /* long  For disk files other than key-sequenced, the RBA */
                /*       of the most recently read/written record. */

#define CurAltKeyName_InfoCode            13    /* (GetInfoList only) */
                /* short   The specifier of the current key of a file.  Zero */
                /*       implies the primary key. */

#define CurKeyLen_InfoCode                14    /* (GetInfoList only) */
                /* short   For structured disk files, the length of the current */
                /*       key.  A 'ByteLen'. */

#define CurKeyValue_InfoCode              15    /* (GetInfoList only) */
                /* char[0:?]  For structured disk files, the value of the */
                /*       current key. */

#define CurPriKeyLen_InfoCode             16    /* (GetInfoList only) */
                /* short   For structured disk files, the length of the current */
                /*       primary key. */

#define CurPriKeyValue_InfoCode           17    /* (GetInfoList only) */
                /* char[0:?]  For structured disk files, the value of the */
                /*       current primary key. */

#define TapeVolume_InfoCode               18    /* (GetInfoList only) */
                /* char[0:5]  For labelled tape files, the volume serial */
                /*       number of the volume currently being processed. */

#define HighestFileNum_InfoCode           19    /* (GetInfoList only) */
                /* short   The largest filenumber of any currently opened file */
                /*       in the process. */

#define NextFileNum_InfoCode              20    /* (GetInfoList only) */
                /* short   The next largest open filenumber currently in use, or */
                /*       -1 if there is none. */

#define Access_InfoCode                   21    /* (GetInfoList only) */
                /* short   The current access mode of the file.  See the */
                /*       'xxxx_AccessD00' #defines. */

#define Exclusion_InfoCode                22    /* (GetInfoList only) */
                /* short   The current exclusion mode of the file.  See the */
                /*       'xxxx_Exclusion' #defines. */

#define NoWaitDepth_InfoCode              23    /* (GetInfoList only) */
                /* short   The nowait depth under which the file was opened. */

#define SyncDepth_InfoCode                24    /* (GetInfoList only) */
                /* short   The sync depth under which the file was opened. */

#define OpenFlags_InfoCode                25    /* (GetInfoList only) */
                /* OpenFlagsD00_Struct  The open flags that were honored */
                /*       for the current open.  This may be a subset of */
                /*       those flags that were requested at OPEN time. */

#define OperationInfo_InfoCode            26    /* (GetInfoList only) */
                /* ????? */

#define QueueFileSysKey_InfoCode          27    /* (GetInfoList only) */
                /* TM64I   For Queue files, the system-assigned key suffix */
                /*       for the last record inserted via the open */

#define DevType_InfoCode                  30    /*              NAME            */
                /* short   The device type of the device on which the file */
                /*       resides.  See the 'xxxx_DevType' #defines. */

#define DevSubType_InfoCode               31    /*              NAME            */
                /* short   The device subtype of the device on which the */
                /*       file resides.  See the 'xxxx_yyyyType' #defines. */

#define DemountableDisk_InfoCode          32    /*              NAME            */
                /* Boolean  True iff the file resides on a demountable disk. */

#define AuditedDisk_InfoCode              33    /*              NAME            */
                /* Boolean  True iff the file resides on a disk which is */
                /*       allowed to contain audited files. */

#define PhysRecordLen_InfoCode            34    /*              NAME            */
                /* short   Physical record length for the device.  A 'ByteLen'. */
                /*       Note that devices with no a priori maximum will */
                /*       often return a value of 132. */

#define LogicalDevNum_InfoCode            35    /*              NAME            */
                /* short   The logical device number of the device holding the */
                /*       file.  For disk files, this is the device holding the */
                /*       primary partition.  For processes, a -1 is returned. */

#define SubDeviceNum_InfoCode             36    /*              NAME            */
                /* short   The logical subdevice number of the device holding */
                /*       the file.  This value is zero if the device does not */
                /*       understand subdevices. */

#define SqlType_InfoCode                  40    /*              NAME            */
                /* short   The SQL object type of the file.  See the */
                /*       'xxx_SqlType' #defines. */

#define FileType_InfoCode                 41    /* CREATE       NAME            */
                /* short   For disk files, the file type.  See the */
                /*       'xxx_FileType' #defines. */

#define FileCode_InfoCode                 42    /* CREATE ALTER NAME */
                /* short   For disk files, the file code.  See the */
                /*       'xxx_FileCode' #defines. */

#define RecordLen_InfoCode                43    /* CREATE       NAME            */
                /* short   For structured disk files, the maximum logical */
                /*       record length.  A 'ByteLen'. */

#define BlockLen_InfoCode                 44    /* CREATE       NAME            */
                /* short   For disk files, the physical block length. */
                /*       A 'ByteLen'. */

#define KeyOffset_InfoCode                45    /* CREATE       NAME            */
                /* short   For key-sequenced disk files, the offset of the */
                /*       primary key from the beginning of the logical record. */
                /*       A 'ByteOffset'. */

#define KeyLen_InfoCode                   46    /* CREATE       NAME            */
                /* short   For key-sequenced disk files, the length of the */
                /*       primary key.  A 'ByteLen'. */

#define LockKeyLen_InfoCode               47    /*              NAME            */
                /* short   For key-sequenced disk files, the length of the */
                /*       key prefix used for generic locking.  A 'ByteLen'. */

#define PrimaryExtent_InfoCode            50    /* CREATE       NAME            */
                /* short   For disk files, the size in pages (2048 bytes) of */
                /*       the primary extent. */

#define SecondaryExtent_InfoCode          51    /* CREATE       NAME            */
                /* short   For disk files, the size in pages (2048 bytes) of */
                /*       the secondary extent. */

#define MaxExtents_InfoCode               52    /* CREATE       NAME            */
                /* short  For disk files, the largest number of extents that */
                /*       is allowed to be allocated for the file. */

#define AllocExtents_InfoCode             53    /*              NAME            */
                /* short  For disk files, the number of extents currently */
                /*       allocated for the file. */

#define CreationDate_InfoCode             54    /*              NAME            */
                /* TM64I  For disk files, the julian timestamp of the */
                /*       creation of the file. */

#define LastOpenDate_InfoCode             56    /*              NAME            */
                /* TM64I  For disk files, the julian timestamp at which */
                /*       the file was last opened. */

#define ExpirationDate_InfoCode           57    /* CREATE ALTER NAME */
                /* TM64I  For disk files, the julian timestamp before */
                /*       which the file cannot be purged. */

#define Owner_InfoCode                    58    /*              NAME            */
                /* short   For disk files, the UserId of the owner of */
                /*       the file. */

#define SafeGuarded_InfoCode              59    /*              NAME            */
                /* Boolean  For disk files, True iff the file is under */
                /*       SafeGuard security. */

#define ProgId_InfoCode                   60    /*              NAME            */
                /* Boolean  For disk files, True iff a process executing */
                /*       the file will run under the UserId of the owner */
                /*       of the file. */

#define ClearOnPurge_InfoCode             61    /*              NAME            */
                /* Boolean  For disk files, True iff all data in the file */
                /*       will be erased when the file is purged. */

#define Security_InfoCode                 62    /*              NAME            */
                /* char[0:3]  For disk files, the Read/Write/Execute/Purge */
                /*       security attributes of the file.  See the */
                /*       'xxx_Security' #defines. */

#define Licensed_InfoCode                 63    /*              NAME            */
                /* Boolean  For disk files, True iff the file is licensed. */

#define OddUnstructured_InfoCode          65    /* CREATE ALTER NAME */
                /* Boolean  For unstructured disk files, True iff odd-length */
                /*       writes will not result in a padding byte. */

#define Audited_InfoCode                  66    /* CREATE ALTER NAME */
                /* Boolean  For disk files, True iff the file is audited by TMF. */

#define AuditCompress_InfoCode            67    /* CREATE       NAME            */
                /* Boolean  For audited disk files, True iff audit compression */
                /*       will be used. */

#define DataCompress_InfoCode             68    /* CREATE       NAME            */
                /* Boolean  For key-sequenced disk files, True iff data */
                /*       compression is used in storing the file. */

#define IndexCompress_InfoCode            69    /* CREATE       NAME            */
                /* Boolean  For key-sequenced disk files, True iff compression */
                /*       is used in storing the index of the file. */

#define Refresh_InfoCode                  70    /* CREATE ALTER NAME */
                /* Boolean  For disk files, True iff an updated EOF value needs */
                /*       to be written to disk immediately whenever the size */
                /*       of the file changes. */

#define CreateFlags_InfoCode              71    /* CREATE       NAME            */
                /* short  Struct  For disk files, miscellaneous attributes */
                /*       in the format accepted by the FILE_CREATE_ procedure. */

#define BufferedWrites_InfoCode           72    /* CREATE       NAME            */
                /* Boolean  For disk files, True iff updates to the file need */
                /*       not be immediately written to disk. */

#define VerifyWrites_InfoCode             73    /* CREATE       NAME            */
                /* Boolean  For disk files, True iff all writes to the disk */
                /*       will be verified by reading the disk. */

#define SerialWrites_InfoCode             74    /* CREATE       NAME            */
                /* Boolean  For disk files, True iff all updates must be */
                /*       written to each mirror serially. */

#define FileIsOpen_InfoCode               75    /*              NAME            */
                /* Boolean  For disk files, True iff the file is open. */

#define CrashOpen_InfoCode                76    /*              NAME            */
                /* Boolean  For disk files, True iff the file is marked */
                /*       "CrashOpen".  This indicates that the file was */
                /*       open for writing at the time of a volume or system */
                /*       failure. */

#define RollforwardNeeded_InfoCode        77    /*              NAME            */
                /* Boolean  For audited disk files, True iff the file is */
                /*       marked "RollForward Needed".  This indicates that */
                /*       the file is not usable for normal operations. */

#define Broken_InfoCode                   78    /*        ALTER NAME            */
                /* Boolean  For disk files, True iff the file is marked */
                /*       "Broken".  This indicates that some blocks in the */
                /*       file may contain unusable data. */

#define Corrupt_InfoCode                  79    /*              NAME            */
                /* Boolean  For disk files, True iff the file is marked */
                /*       "Corrupt".  This indicates that part of the */
                /*       file may be missing. */

#define SecondaryPart_InfoCode            80    /*              NAME            */
                /* Boolean  For disk files, True iff the file is a secondary */
                /*       partition of another file. */

#define IndexLevels_InfoCode              81    /*              NAME            */
                /* short   For key-sequenced disk files, the number of levels */
                /*       of index structure in use for the file. */

#define SqlProgram_InfoCode               82    /*              NAME            */
                /* Boolean  For disk files, True iff the file is a program */
                /*       file containing compiled SQL statements. */

#define SqlValid_InfoCode                 83    /*              NAME            */
                /* Boolean  For disk files, True iff the file contains compiled */
                /*       SQL statements and that compilation is probably still */
                /*       valid. */

#define SqlCatNameLen_InfoCode            84    /*              NAME            */
                /* short   For disk files, the length of the name of the SQL */
                /*       catalog that owns the object.  A 'ByteLen'.  Zero */
                /*       if no catalog is associated with the object. */

#define SqlCatName_InfoCode               85    /*              NAME            */
                /* char[0:?]  For disk files, the string-form name of */
                /*       the SQL catalog associated with the file.  If there */
                /*       is none, this string is null. */

#define NumPartitions_InfoCode            90    /* CREATE ALTER NAME */
                /* short   For disk files, the number of partitions in the */
                /*       file. */

#define PartDescriptors_InfoCode          91    /* CREATE ALTER NAME */
                /* short[0:?][0:1]  For partitioned disk files, a list of the */
                /*       primary and secondary extent sizes (in 2048-byte pages) */
                /*       for each partition. */

#define PartVolumeLens_InfoCode           92    /* CREATE ALTER NAME */
                /* short[0:?]  For partitioned disk files, a list of the */
                /*       lengths of the volume names for each partition. */
                /*       An array of 'ByteLen's. */

#define PartVolumeNames_InfoCode          93    /* CREATE ALTER NAME */
                /* char[0:?]  For partitioned disk files, a list of the */
                /*       volume names of each partition, in string-form, */
                /*       concatenated end-to-end. */

#define PartKeyLen_InfoCode               94    /* CREATE       NAME    */
                /* short   For partitioned key-sequenced disk files, the length */
                /*       of the key prefixes that determine the cutover points */
                /*       between partitions.  A 'ByteLen'. */

#define PartKeyValue_InfoCode             95    /* CREATE       NAME    */
                /* char[0:?][0:?]  For partitioned key-sequenced disk files, */
                /*       an array of the key-prefixes of the cutover points */
                /*       for each partition. */

#define PartTotalNameLen_InfoCode         96    /*              NAME            */
                /* short   For disk files, the total length of the string of */
                /*       concatenated volume names of the secondary partitions. */
                /*       A 'ByteLen'. */

#define NumAltKeys_InfoCode               100   /* CREATE ALTER NAME */
                /* short   For structured disk files, the number of alternate */
                /*       keys that are defined. */

#define AltKeyDescrs_InfoCode             101   /* CREATE ALTER NAME */
                /* ??    For structured disk files, an array of descriptors */
                /*       of each alternate key.  See EXCEED File System ES */
                /*       for details. */

#define NumAltKeyFiles_InfoCode           102   /* CREATE ALTER NAME */
                /* short   For structured disk files, number of files defined */
                /*       to hold alternate key information. */

#define AltKeyFileLens_InfoCode           103   /* CREATE ALTER NAME */
                /* short[0:?]  For structured disk files, an array of the lengths */
                /*       of the filenames of the alternate key files.  This is */
                /*       an array of 'ByteLen's. */

#define AltKeyFileNames_InfoCode          104   /* CREATE ALTER NAME */
                /* char[0:?]  For structured disk files, the names of the */
                /*       alternate key files, in string-form, strung end-to-end. */

#define AltTotalNameLen_InfoCode          105   /*              NAME            */
                /* short   For structured disk files, the total length of the */
                /*       names of the alternate key files, strung end-to-end. */

#define VolumeCapacity_InfoCode           110   /*              NAME            */
                /* long  For disk objects, the total capacity of the */
                /*       disk, in pages (2048 bytes). */

#define VolumeFreeSpace_InfoCode          111   /*              NAME            */
                /* long  For disk objects, the number of pages on the */
                /*       disk that are available for allocation.  A page */
                /*       is 2048 bytes. */

#define VolumeFragments_InfoCode          112   /*              NAME            */
                /* long  For disk objects, the number of contiguous fragments */
                /*       of free-space that exist on the disk. */

#define LargestFragment_InfoCode          113   /*              NAME            */
                /* long  For disk objects, the size of the largest contigous */
                /*       fragment of free space, in pages (2048 bytes). */

#define DiskDriveTypes_InfoCode           114   /*              NAME            */
                /* char[0:1][0:7]  For disk objects, the model numbers of */
                /*       the primary and mirror disk drives.  Each model */
                /*       number is eight bytes of ASCII, left-justified and */
                /*       padded with spaces. */

#define DiskCapacities_InfoCode           115   /*              NAME            */
                /* long[0:1]  For disk objects, the capacities (in 2048- */
                /*       byte pages) of the primary and mirror disk drives. */

#define SequentialBlockBuffer_InfoCode    116   /*              NAME            */
                /* ????? */

#define LastOpenDateLct_InfoCode          117   /*              NAME            */
                /* TM64I  For disk files, the time of the last open of the */
                /*       file, in local civil time. */

#define ExpirationDateLct_InfoCode        118   /*              NAME            */
                /* TM64I  For disk files, the expiration date of the file, */
                /*       in local civil time. */

#define CreationDateLct_InfoCode          119   /*              NAME            */
                /* TM64I  For disk files, the creation date of the file, */
                /*       in local civil time. */

#define PartModificationDate_InfoCode     140   /*              NAME            */
                /* TM64I  For disk files, the modification date of the */
                /*       current partition of the file. */

#define PartModDateLct_InfoCode           141   /*              NAME            */
                /* TM64I  For disk files, the modification date of the */
                /*       current partition of the file, in local civil time. */

#define EndOfFile_InfoCode                142   /*              NAME            */
                /* long  For disk objects other than shorthand views, the */
                /*       number of bytes stored on disk for the object.  For */
                /*       unstructured disk files, this is the actual number */
                /*       of bytes in the file. */

#define MaximumSize_InfoCode              143   /*              NAME            */
                /* long  For disk objects other than shorthand views, the */
                /*       maximum number of bytes that the file is allowed */
                /*       to occupy. */

#define ModifyDate_InfoCode               144   /*              NAME            */
                /* TM64I  For disk files, the julian timestamp at which the */
                /*       file was last modified. */

#define SqlPackedRecLength_InfoCode       153   /*              NAME            */
                /* ????? */

#define PartModDateLct48_InfoCode         160   /*              NAME            */
                /* ????? */

#define TMFRedoNeeded_InfoCode            162   /* (GetInfoList only) */
                /* Boolean  For disk files, True if the 'RedoNeeded' flag is */
                /*       set in the File Label. */

#define TMFUndoNeeded_InfoCode            163   /* (GetInfoList only) */
                /* Boolean  For disk files, True if the 'UndoNeeded' flag is */
                /*       set in the File Label. */

#define TFileFNum_InfoCode                170   /* (GetInfoList only) */
                /* short  For file number -1, the file number of the transaction */
                /*       pseudofile opened by this process, or -1 if not open. */

#pragma PAGE "Process Information List Codes"
/*
---------------------------------------------------------

       Process Information List Codes.  These codes are used by the
       PROCESS_GETINFOLIST_, PROCESS_SETINFO_ and PROCESS_SETINFOchar_
       procedures.

       Not all of these codes are usable with all procedures.  Each
       #define is followed by a set of keywords indicating which procedures
       can use that code, as follows:
               SEARCH  - usable as a search argument to PROCESS_GETINFOLIST_.
               GET     - usable as an info argument to PROCESS_GETINFOLIST_.
               SET     - usable as an argument to PROCESS_SETINFO_.
               SELFSET - usable as an argument to PROCESS_SETINFO_, but only
                         when altering attributes of the current process.
               SETSTR  - usable as an argument to PROCESS_SETINFOchar_.
               SELFSETSTR - usable as an arguemnt to PROCESS_SETINFOchar_,
                         only when altering attributes of the current process.

======================================================================
*/

#define CreatorUserId_ProcInfoCode        1     /* SEARCH GET */
                /* short   UserId of the creator of this process. */

#define ProcessUserId_ProcInfoCode        2     /* SEARCH GET */
                /* short   UserId under which this process is running. */

#define MaxPriority_ProcInfoCode          3     /* SEARCH */
                /* short   Maximum priority of process.  Usable only as a */
                /*       search attribute.*/

#define ProgramFile_ProcInfoCode          4     /* SEARCH GET */
                /* short,char    Number of bytes in the program file name, */
                /*       followed by the program file name of the process. */
                /*       If the length of the program file name is odd, the */
                /*       name is followed by a byte of garbage. */

#define HomeTerminal_ProcInfoCode         5     /* SEARCH GET SELFSETSTR */
                /* short,char    Number of bytes in the home terminal name, */
                /*       followed by the home terminal name of the process. */
                /*       If the length of the home terminal name is odd, the */
                /*       name is followed by a byte of garbage. */

#define GMomPHandle_ProcInfoCode          6     /* SEARCH GET SELFSET */
                /* PHandle_Struct        PHandle of the GMom of the process. */

#define JobId_ProcInfoCode                7     /* SEARCH GET SELFSET */
                /* short   JobId of the process. */

#define ProcessSubType_ProcInfoCode       8     /* SEARCH GET */
                /* short   Process subtype of the process. */

#define MinPriority_ProcInfoCode          9     /* SEARCH */
                /* short   Minimum priority of the process.  Usable only as a */
                /*       search attribute. */

#define ProcessTime_ProcInfoCode          30    /*        GET           */
                /* TM64I  Cpu time that the process has consumed so far, in */
                /*       microseconds. */

#define WaitState_ProcInfoCode            31    /*        GET           */
                /* short   What events the process is waiting on, as the following */
                /*       bit fields: */
                /*               <8>  = CPU power on */
                /*               <9>  = I/O power on */
                /*               <10> = interrupt */
                /*               <11> = cancellation of incoming message */
                /*               <12> = inspect event */
                /*               <13> = completion of outgoing message */
                /*               <14> = TMF event */
                /*               <15> = receipt of incoming message */

#define ProcessState_ProcInfoCode         32    /*        GET           */
                /* short   State of the process, as the following bit fields: */
                /*               <0>     = process contains privileged code */
                /*               <1>     = waiting on page fault */
                /*               <2>     = process is on ready list */
                /*               <3>     = system process */
                /*               <4:5>   = reserved */
                /*               <6>     = process has a memory access */
                /*                         breakpoint in system code */
                /*               <7>     = process is not accepting */
                /*                         any incoming messages */
                /*               <8>     = temporary system process */
                /*               <9>     = process has logged on */
                /*               <10>    = process is in a pending state */
                /*               <11:15> = one of the following: */
                /*                          0 = process doesn't exist */
                /*                          1 = starting */
                /*                          2 = runnable */
                /*                          3 = suspended */
                /*                          4 = DEBUG memory access breakpoint */
                /*                          5 = DEBUG breakpoint */
                /*                          6 = DEBUG trap */
                /*                          7 = DEBUG request */
                /*                          8 = INSPECT memory access breakpoint */
                /*                          9 = INSPECT breakpoint */
                /*                         10 = INSPECT trap */
                /*                         11 = INSPECT request */
                /*                         12 = SAVEABEND */
                /*                         13 = terminating */

#define LibraryFile_ProcInfoCode          33    /*        GET           */
                /* short,char    Number of bytes in the library file name, */
                /*       followed by the library file name.  If the length of */
                /*       the library file name is odd, it will be followed by */
                /*       a byte of garbage. */

#define SwapFile_ProcInfoCode             34    /*        GET           */
                /* short,char    Number of bytes in the swap file name, */
                /*       followed by the swap file name.  If the length of the */
                /*       swap file name is odd, it will be followed by a */
                /*       byte of garbage. */

#define ContextChanges_ProcInfoCode       35    /*        GET           */
                /* short   The number of changes that have occurred to the */
                /*       DEFINE context of the process since it was created. */

#define DefineMode_ProcInfoCode           36    /*        GET           */
                /* short   The DEFINE mode of the process: */
                /*               <0:13>  = reserved for future use. */
                /*               <14:15> = one of: */
                /*                               0 = DEFINEs are disabled */
                /*                               1 = DEFINEs are enabled */

#define Licenses_ProcInfoCode             37    /*        GET           */
                /* short   The licenses of the process: */
                /*               <0:14>  = reserved for future use. */
                /*               <15>    = program licensed by FUP */

#define Pin_ProcInfoCode                  38    /*        GET           */
                /* short   The PIN of the process. */

#define FileName_ProcInfoCode             39    /*        GET           */
                /* short,char    The length of the name by which the process */
                /*       can be opened, followed by the name itself.  If the */
                /*       length of the name is odd, the name is followed by */
                /*       a byte of garbage. */

#define MomPHandle_ProcInfoCode           40    /*        GET     SET   */
                /* PHandle_Struct        The PHandle for the MOM of the process. */
                /*       If the process is a member of a process pair, this will */
                /*       usually be the PHandle of the other member of the pair. */

#define FileSecurity_ProcInfoCode         41    /*        GET SELFSET   */
                /* Security_Struct       The security attribute that will be */
                /*       assigned to any files created by the process. */

#define CurrentPriority_ProcInfoCode      42    /*        GET     SET   */
                /* short   The current priority of the process. */

#define InitialPriority_ProcInfoCode      43    /*        GET           */
                /* short   The initial priority of the process. */

#define RemoteCreator_ProcInfoCode        44    /*        GET           */
                /* short   0 iff the process's creator was on the same node, and */
                /*         the process's creator's RemoteCreator flag was zero. */
                /*       1 otherwise. */

#define LogonState_ProcInfoCode           45    /*        GET           */
                /* short   1 iff the process is logged on. */
                /*       0 otherwise. */

#define ExtSwapFile_ProcInfoCode          46    /*        GET           */
                /* short,char    The length of the process's extended swap file */
                /*       name, followed by the name itself.  If the length of */
                /*       the name is odd, the name is followed by a byte of */
                /*       garbage. */

#define Primary_ProcInfoCode              47    /*        GET SELFSET   */
                /* short   1 iff Guardian thinks that the process is the primary */
                /*         process of a named process pair. */
                /*       0 iff either Guardian thinks that the process is the */
                /*         the backup process of a pair, or the process is */
                /*         unnamed. */

#define PHandle_ProcInfoCode              48    /*        GET           */
                /* PHandle_Struct  The PHandle for the process. */

#define QualfierAvailable_ProcInfoCode    49    /*        GET SELFSET   */
                /* short   1 iff the process supports queries against its internal */
                /*         qualifiers via FILENAME_FIND... procedures. */
                /*       0 otherwise. */

#define TsnLogon_ProcInfoCode             50    /*        GET           */
                /* short   1 iff a TSN logon has taken place. */
                /*       0 otherwise. */

#define ForcedLow_ProcInfoCode            51    /*        GET           */
                /* short   1 iff the process's creator forced this process to run       */
                /*         in a low pin. */
                /*       0 otherwise. */

#pragma PAGE "System Message"
/*
--------------------------------------------------------------------

       System Message Types.  Following are the definitions for the
       first word of every system message.

       The authority on system messages is \FOXII.$OPSYS.NEWBIBLE.SYSMSGS

======================================================================
*/

#define CpuDown_SysMsg            -2   /* Local CPU went down. */
#define CpuUp_SysMsg              -3   /* Local CPU came up. */
#define Stop_SysMsg               -5   /* Process has stopped. */
#define Abend_SysMsg              -6   /* Process has abended. */
#define AOpr_SysMsg               -7   /* $AOPR message. */
#define NetStatus_SysMsg          -8   /* A remote CPU went up or down. */
#define Start_SysMsg              -9   /* A descendent process has started. */
#define SetTime_SysMsg            -10  /* Operator reset the clock. */
#define PowerOn_SysMsg            -11  /* A CPU has been turned on. */
#define NewProcess_SysMsg         -12  /* NEWPROCESSNOWAIT has completed. */
#define LostStatusMsgs_SysMsg     -13  /* Some Cpu/Net Up/Down Msgs lost. */
/*                                -18  */ /* "Reserved for Rabbit" */
#define Break_SysMsg              -20  /* BREAK occurred at terminal. */
#define T3270Status_SysMsg        -21  /* 3270 status change occurred. */
#define TimeOut_SysMsg            -22  /* SIGNALTIMEOUT has completed. */
#define LockComplete_SysMsg       -23  /* LOCKMEMORY completed. */
#define LockFailed_SysMsg         -24  /* LOCKMEMORY failed. */
#define TmfCatalog_SysMsg         -25  /* Event for TMF Catalog. */
#define ProcessTimeOut_SysMsg     -26  /* SIGNALPROCESSTIMEOUT has completed. */
#define TapeDevice_SysMsg         -27  /* Event for $ZSVR process. */
#define SPI_SysMsg                -28  /* A SPI event has occurred. */
#define Open_SysMsg               -30  /* Requester called OPEN. */
#define Close_SysMsg              -31  /* Requester called CLOSE. */
#define Control_SysMsg            -32  /* Requester called CONTROL. */
#define SetMode_SysMsg            -33  /* Requester called SETMODE. */
#define ResetSync_SysMsg          -34  /* Requester called RESETSYNC. */
#define ControlBuf_SysMsg         -35  /* Requester called CONTROLBUF. */
/*                                -36  */ /* "Name Server Open Message" */
#define SetParam_SysMsg           -37  /* Requester called SETPARAM. */
#define Cancel_SysMsg             -38  /* Previous request is cancelled. */
/*                                -39  */ /* "Write/WriteRead" */
#define DeviceInfo_SysMsg         -40  /* Requester called DEVICEINFO. */
#define DeviceInfoNowait_SysMsg   -41  /* Nowait DeviceInfo completed. */
/*                                -42  */ /* "Rabbit" */
#define RemoteCpuDown_SysMsg      -100 /* Remote CPU went down. */
#define StopD00_SysMsg            -101 /* A process has stopped. */
#define ProcessCreate_SysMsg      -102 /* Nowait PROCESS_CREATE_ has completed. */
#define OpenD00_SysMsg            -103 /* Requester called OPEN. */
#define CloseD00_SysMsg           -104 /* Requester called CLOSE. */
#define BreakD00_SysMsg           -105 /* BREAK occurred at terminal. */
#define DeviceInfoD00_SysMsg      -106 /* Requester called DEVICEINFO. */
#define NameInquiry_SysMsg        -107 /* Requester called FILE_FINDNEXT_. */
#define FileGetInfoByName_SysMsg  -108 /* Nowait FILE_GETINFOBYNAME_ completed. */
#define FileFindNext_SysMsg       -109 /* Nowait FILE_FINDNEXT_ has completed. */
#define NodeDown_SysMsg           -110 /* Remote node has gone down. */
#define NodeUp_SysMsg             -111 /* Remote node has come up. */
#define StartD00_SysMsg           -112 /* A descendent process has started. */
#define RemoteCpuUp_SysMsg        -113 /* A remote CPU came up. */
#define TapeDeviceD00_SysMsg      -114 /* Event for $ZSVR process. */
#define OpenSpooler_SysMsg        -115 /* OPEN message to spool collector. */
#define TmfCatalogD00_SysMsg      -116 /* Event for TMF Catalog. */
/*                                -117 */ /* "LoggerNormal" message. */
/*                                -118 */ /* "LoggerException" message. */
/*                                -119 */ /* "LastClose" message. */
/* #define EndDialog_SysMsg          -120 */ /* "EndDialog" message. */
#define AbortDialog_SysMsg        -121 /* "AbortDialog" message. */
/*                                -122 */ /* "MLAN to Multi Lan Resource manager" */
/*                                -123 */ /* "X.400 GPI" */
/*                                -124 */ /* "POSIX - fcntl()" */
/*                                -125 */ /* "POSIX - tcsendbreak()" */
/*                                -126 */ /* "POSIX - tcdrain()" */
/*                                -127 */ /* "POSIX - tcflush()" */
/*                                -128 */ /* "POSIX - tcflow()" */
/*                                -129 */ /* "POSIX - tcgetattr()" */
/*                                -130 */ /* "POSIX - tcsetattr()" */
#define LDoneCompleteion_SysMSg   -131 /* LDone completion via $Receive */
/*                                -132 */ /* "POSIX - fstat()" */
/*                                -133 */ /* "POSIX - open completion" */
/*                                -134 */ /* "POSIX - stat()" */
/*                                -135 */ /* "POSIX - recall message" */
#pragma PAGE "System Message Templates"
/*
--------------------------------------------------------------------

       Structures of the various system messages.  Each different system
       message is defined by a variant within this structure.

       There is no authoritative reference for all of these messages.

======================================================================
*/

#pragma FIELDALIGN SHARED2 SysMsg_Struct
typedef struct SysMsg_Struct
{

        short     MsgType;        /* Type of this message.  One of the */
                                /* 'xxxxx_SysMsg' #defines. */

        union { /* Contents vary by message type.  See below. */

#pragma PAGE "Break System Message"
        struct

/*
----------------------------------------------------------------------

       Break System Message.  This variant applies when MsgType = -20.

       This message indicates that the break key was pressed on a
       terminal.

       Receipt of this message is controlled by 'BreakOwnership_SetMode'
       on the terminal in question.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see 'BreakD00'.

======================================================================
*/

          {

                short     Ldev;   /* Logical device number of the terminal that */
                                /* generated the BREAK.  -1 if the terminal */
                                /* has no LDEV number. */

                short     System; /* System number on which the terminal resides. */

                long      Tag;    /* The break tag, established by SETPARAM 3. */

                short     Reserved;
                                /* Reserved, currently always zero. */

            } Break;

#pragma PAGE "D00 Break System Message"
        struct

/*
---------------------------------------------------------------------

       Break System Message.  This variant applies when MsgType = -20.

       This message indicates that the break key was pressed on a
       terminal.

       Receipt of this message is controlled by 'BreakOwnership_SetMode'
       on the terminal in question.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/

            {

                short     FileNumber;
                                /* File number of the opened terminal, or */
                                /* -1 if the file number is not available. */
                                /* This field will always be -1 if the */
                                /* terminal is on a pre-D00 system. */

                long    Tag;    /* The break tag, established by SETPARAM 3. */

            } BreakD00;

#pragma PAGE "Cancel System Message"
        struct

/*
---------------------------------------------------------

       Cancel System Message.  This variant applies when MsgType = -38.

       This message indicates that a previously received $RECEIVE message
       has been cancelled by the caller or the message system.  [A
       reply is still required, but the contents of the reply will not
       be delivered anywhere.]

       Receipt of these messages is controlled by Param1.<13> of
       'ReceiveOptions_SetMode' on $RECEIVE.

======================================================================
*/

          {

                short     Tag;    /* $RECEIVE tag for the message that was */
                                /* cancelled. */

            } Cancel;

#pragma PAGE "Close System Message"
        struct

/*
---------------------------------------------------------

       Close System Message.  This variant applies when MsgType = -31.

       This message indicates an opener of the current process has
       closed it.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see 'CloseD00'.

======================================================================
*/

          {

                short    NoData[1];
                                /* No additional data here. */

            } Close;

#pragma PAGE "D00 Close System Message"
        struct

/*
---------------------------------------------------------

       D00 Close System Message.  This variant applies when MsgType = -104.

       This message indicates an opener of the current process has
       closed it.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/

            {

                short     TapeDisposition;
                                /* The TapeDisposition parameter to File_Close_. */

            } CloseD00;

#pragma PAGE "Control System Message"
        struct

/*
---------------------------------------------------------------------

       Control System Message.  This variant applies when MsgType = -33.

       This message indicates that an opener of the current process has
       called the system procedure 'Control'.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

======================================================================
*/

            {

                short     Operation;
                                /* The 'Operation' parameter to CONTROL. */

                short     Parameter;
                                /* The 'Parameter' parameter to CONTROL. */

            } Control;

#pragma PAGE "ControlBuf System Message"
        struct

/*
--------------------------------------------------------------------

       ControlBuf System Message.  This variant applies when MsgType = -35.

       This message indicates that an opener of this process called
       the system procedure 'ControlBuf'.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

======================================================================
*/
            {

                short     Operation;
                                /* The 'Operation' parameter to CONTROLBUF. */

                short     Count;  /* The 'Count' parameter to CONTROLBUF.  This */
                                /* is the number of bytes in the 'Buffer' array. */

                char  Buffer[1];
                                /* The data buffer parameter to CONTROLBUF. */
                                /* This is 'Count' bytes long. */

            } ControlBuf;

#pragma PAGE "CpuDown System Message"
        struct

/*
----------------------------------------------------------------------

       Cpu Down System Message.  This variant applies when MsgType = -2.

       Note that there are two different flavors of Cpu Down messages.
       One contains a cpu number, and indicates that that CPU has failed.
       The other contains a process name, and indicates that that process
       pair was deleted because of a cpu failure.  This second variety
       is only sent on behalf of named processes, and only to the creator
       and group leader of the process pair.

       Receipt of the first variety of this message is controlled by
       the system procedure 'MonitorCpus'.

       The second variety of Cpu Down message will not be received if
       you request the new style of messages when you call FILE_OPEN_
       to open $RECEIVE.

======================================================================
*/
            {
                union {

                short     Cpu[1];
                                /* Number of the CPU that failed.  This sub- */
                                /* variant Applies if Cpu is in the range 0..15. */

                short    ProcessName[4];
                                /* Name of a deleted process.  This subvariant */
                                /* applies if Cpu is not in the range */
                                /* 0..15. */
                 } cpudown1;
            } CpuDown;

#pragma PAGE "CpuUp System Message"
        struct

/*
-----------------------------------------------------------------------

       Cpu Up System Message.  This variant applies when MsgType = -3.

       This message indicates that a local CPU has been reloaded.

       Receipt of this message is controlled by the system procedure
       'MonitorCpus'.

======================================================================
*/
            {

                short     Cpu;
                                /* Number of the CPU that has been reloaded. */

            } CpuUp;

#pragma PAGE "DeviceInfo System Message"
        struct

/*
----------------------------------------------------------------------

       DeviceInfo System Message.  This variant applies when MsgType = -40.

       This message indicates that another process is inquiring about
       the [simulated] device type of the current process.

       This message is delivered only to processes with a subtype of 30
       ('Simulator_SubType').

       You must reply to this message using variant 'DeviceInfo' of
       'SysMsgReply_Struct'.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see
       'DeviceInfoD00'.

======================================================================
*/
            {

                short    Directory[4];
                                /* Directory component of the name passed */
                                /* to 'DeviceInfo'. */

                short    SimpleName[4];
                                /* Simple name component of the name passed */
                                /* to 'DeviceInfo'. */

            } DeviceInfo;

#pragma PAGE "D00 DeviceInfo System Message"
        struct

/*
------------------------------------------------------------------

       D00 DeviceInfo System Message.  This variant applies when
       MsgType = -106.

       This message indicates that another process is inquiring about
       the [simulated] device type of the current process.

       This message is delivered only to processes with a subtype of 30
       ('Simulator_SubType').

       You must reply to this message using variant 'DeviceInfoD00' of
       'SysMsgReply_Struct'.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {

                short    Reserved[3];
                                /* Reserved words. */

                short     QualifierLen;
                                /* Length of the 'Qualifier' field. */
                                /* A 'ByteLen'. */

                char  Qualifier[1];
                                /* The qualifier string for the file about */
                                /* which information is desired.  Its length */
                                /* is given by 'QualifierLen'. */

            } DeviceInfoD00;

#pragma PAGE "DeviceInfo Completion System Message"
        struct

/*
-----------------------------------------------------------------------

       DeviceInfo Completion System Message.  This variant applies when
       MsgType = -41.

       This message indicates that a previous DeviceInfo2 request with
       the nowait option has completed.

       Receipt of this message is requested by calling the system
       procedure 'DeviceInfo2' with the 'Options' parameter bit <13> set.

======================================================================
*/
            {

                long      Tag;    /* 'TagOrTimeout' value specified in the call */
                                /* to DeviceInfo2. */

                short     ErrorCode;
                                /* File system error code for the request. */
                                /* Zero => success. */

                short     DevType;
                                /* The device type of the requested file. */

                short     RecordBytes;
                                /* Maximum physical record size for the device. */

                short     DiskProcessVersion;
                                /* For disk processes, */
                                /*       0 => disk process is DP1, */
                                /*       1 => disk process is DP2. */
                                /* Undefined for non-disk processes. */

            } DeviceInfoComp;

#pragma PAGE "File_FindNext_ Completion System Message"
        struct

/*
---------------------------------------------------------------------

       File_FindNext_ Completion System Message.  This variant
       applies when MsgType = -109.

       This message indicates that a previous File_FindNext_
       request with the nowait option has completed.

       Receipt of this message is requested by calling the system
       procedure 'File_FindNext_' with the nowait option.

======================================================================
*/

            {

                short     SearchId;
                                /* The 'SearchId' parameter to 'File_FindNext_'. */

                short     ErrorCode;
                                /* File system error code for the request. */
                                /* Zero => success. */

                short     DevType;
                                /* The device type of the requested file. */
                                /* See the 'xxx_DevType' #defines. */

                short     SubType;
                                /* The subtype of the requested file. */
                                /* See the 'xxxx_yyyyType' #defines. */

                short     ObjectType;
                                /* The SQL type of the object. */
                                /* See the 'xxx_SqlType' #defines. */

                short     FileType;
                                /* The file type of the object. */
                                /* See the 'xxxx_FileType' #defines. */
                                /* This field is relevant only when */
                                /* DevType is 'Disk_DevType'. */

                short     FileCode;
                                /* The file code of the object. */
                                /* See the 'xxxx_FileCode' #defines. */
                                /* This field is relevant only when */
                                /* DevType is 'Disk_DevType'. */

                short     NameStrLen;
                                /* Length of the returned 'NameStr'. */
                                /* A 'ByteLen'. */

                short     Reserved1[5];
                                /* Reserved words. */

                char  NameStr[1];
                                /* The returned name, in string form.  Its */
                                /* length is given by 'NameStrLen'. */

            } FileFindNext;

#pragma PAGE "File_GetInfoByName_ Completion System Message"
        struct

/*
----------------------------------------------------------------------

       File_GetInfoByName_ Completion System Message.  This variant
       applies when MsgType = -108.

       This message indicates that a previous File_GetInfoByName_
       request with the nowait option has completed.

       Receipt of this message is requested by calling the system
       procedure 'File_GetInfoByName_' with the nowait option.

======================================================================
*/
            {

                long Tag;    /* 'TagOrTimeout' value specified in the call */
                                /* to File_GetInfoByName_. */

                short     ErrorCode;
                                /* File system error code for the request. */
                                /* Zero => success. */

                short     DevType;
                                /* The device type of the requested file. */
                                /* See the 'xxx_DevType' #defines. */

                short     SubType;
                                /* The subtype of the requested file. */
                                /* See the 'xxxx_yyyyType' #defines. */

                short     ObjectType;
                                /* -1 => Object is a volume or subvolume. */
                                /*  0 => Object is a non-SQL file. */
                                /* >0 => SQL type of the file. */
                                /* This field is relevant only when */
                                /* DevType is 'Disk_DevType'. */

                short     FileType;
                                /* The file type of the object. */
                                /* See the 'xxxx_FileType' #defines. */
                                /* This field is relevant only when */
                                /* DevType is 'Disk_DevType'. */

                short     FileCode;
                                /* The file code of the object. */
                                /* See the 'xxxx_FileCode' #defines. */
                                /* This field is relevant only when */
                                /* DevType is 'Disk_DevType'. */

                short     RecordBytes;
                                /* Maximum physical record size for the device. */

            } FileGetInfoByName;

#pragma PAGE "LDone Completion System Message"
        struct

/*
---------------------------------------------------------------------

       LDone Completion System Messages.  This variant applies when
       MsgType = -131.

       This message indicates that a message initiated via Msg_Link_
       with LDone queuing has completed.

======================================================================
*/
            {

                long MsgId;  /* Message system identifier for the message. */

                long Tag;    /* User tag for the message. */

            }  LDone;

#pragma PAGE "LockMemory Completion System Message"
        struct

/*
--------------------------------------------------------------------------

       LockMemory Completion System Messages.  This variant applies when
       MsgType = -23 (successful completion) or -24 (failed completion).

       This message indicates that an attempt to lock memory has completed
       or failed.  This attempt is initiated by a call to the system
       procedure 'LockMemory'.

======================================================================
*/
            {

                short    Param1; /* The 'Param1' value supplied to LockMemory. */

                long Param2; /* The 'Param2' value supplied to LockMemory. */

            } LockMemory;

#pragma PAGE "Lost Status Messages System Message"
        struct

/*
------------------------------------------------------------------------

       Lost Status Messages System Message.  This variant applies when
       MsgType = -13.

       This message indicates that one or more messages reflecting a CPU
       going up or down anywhere in the network may have been lost.
       No information about the contents of the lost messages is available.

       This message can occur if you are monitoring processor or network
       status, and a large number of status changes have occurred since
       the last time that you read from $RECEIVE.

======================================================================
*/
            {

                short    NoData[1];
                                /* No additional data here. */

            } LostStatus;

#pragma PAGE "Name Inquiry System Message"
        struct

/*
----------------------------------------------------------------------

       Name Inquiry System Message.  This variant applies when
       MsgType = -107.

       This message indicates that some other process is enquiring
       about a name owned by the current process.

       You must reply to this message using the 'NameInquiry' variant of
       'SysMsgReply_Struct'.

       Receipt of this message is enabled by calling PROCESS_SETINFO_.

======================================================================
*/
            {

/*                char  NamePattern[2:1]; */
                                /* The expression NamePattern[NamePatternOffset] */
                                /* yields the start of a pattern string.  Only */
                                /* information about names that match this */
                                /* pattern string should be returned.  The */
                                /* length of this string is given by */
                                /* 'NamePatternLen'. */

                unsigned int     : 15;
                                /* Reserved bits. */

                unsigned int     SkipSameName : 1;
                                /* 0 =>  Return information about the StartName */
                                /*       below, if it exists, and otherwise */
                                /*       about the first name following the */
                                /*       StartName. */
                                /* 1 =>  Return information about the first */
                                /*       name following the StartName, even if */
                                /*       the StartName exists. */

                short     StartNameLen;
                                /* Length of 'StartName'.  A 'ByteLen'. */

                short     NamePatternOffset;
                                /* Offset, from the beginning of the message,*/
                                /*       to 'PatternName'.  A 'ByteOffset'. */

                short     NamePatternLen;
                                /* Length of 'NamePattern'.  A 'ByteLen'. */

                short     Reserved2[3];
                                /* Reserved words. */

                char  StartName[1];
                                /* Name string to use as the starting point */
                                /* of the search for a name to return.  Its */
                                /* length is given by 'StartNameLen'. */

            } NameInquiry ;

#pragma PAGE "Network Status System Message"
        struct

/*
-----------------------------------------------------------------------

       Network Status System Message.  This variant applies when
       MsgType = -8.

       This message indicates that a CPU somewhere in the network
       went up or down.

       Receipt of this message is controlled by the system procedure
       'MonitorNet'.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see
       'RemoteCpuUpDown'and 'NodeUpDown'.

======================================================================
*/
            {

                char  System; /* Number of the system whose CPU changed state. */

                char  CpuCount;
                                /* Number of CPUs on that system. */

                short    CpuMask;
                                /* Bit <i> is set iff CPU 'i' is currently up. */

                short    OldCpuMask;
                                /* Bit <i> is set iff CPU 'i' was up before the*/
                                /* event that generated this message.*/

            } NetStatus;

#pragma PAGE "NewProcess Completion System Message"
        struct

/*
-----------------------------------------------------------------------

       NewProcess Completion System Message.  This variant applies when
       MsgType = -12.

       This message indicates that a process creation request, initiated
       by calling 'NewProcessNoWait', has completed.

======================================================================
*/
            {

                short     ErrorCode;
                                /* The same meaning as the 'Error' parameter to */
                                /* 'NewProcess'.  Zero indicates no error. */

                long      Tag;    /* The tag value supplied to 'NewProcessNoWait'. */

                short     ProcessName[4];
                                /* The name of the newly created process. */

            } NewProcess;

#pragma PAGE "Node Up/Down System Message"
        struct

/*
-----------------------------------------------------------------------

       Node Up and Node Down System Messages.  This variant applies when
       MsgType = -110 (Node Down) or -111 (Node Up).

       This message indicates that a remote node has become unreachable or
       reachable.

       Receipt of this message is controlled by the system procedure
       'MonitorNet'.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {
                short     Reserved[3];
                                /* Reserved words. */

                long      System; /* System number containing the CPU. */

                short     SystemNameByteLen;
                                /* Length of 'SystemName', in bytes. */

                char     SystemName[1];
                                /* Name of the system on which the CPU */
                                /* went down. */

            } NodeUpDown;

#pragma PAGE "Open System Message"
        struct

/*
----------------------------------------------------------------------

       Open System Message.  This variant applies when MsgType = -30.

       This message indicates that another process is attempting to
       open this process.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see 'OpenD00'.

======================================================================
*/
            {

                short    Flags;  /* Flags parameter from the call to OPEN. */

                short     SyncDepth;
                                /* SyncDepth parameter from the call to OPEN. */

                short    PrimaryProcess[4];
                                /* For backup opens, the 'ProcessName' of the */
                                /* primary opener.  For normal opens, these */
                                /* words will all be binary zeros. */

                short     PrimaryFileNum;
                                /* For backup opens, the file number of the */
                                /* primary open.  For normal opens, this */
                                /* word will be zero. */

                short    OpenerUserId;
                                /* The user id of the process calling OPEN. */

                short    Directory[4];
                                /* The directory portion of the name */
                                /* being opened. */

                short    SimpleName[4];
                                /* The simple name portion of the name */
                                /* being opened. */

                short   OpenerUserIsLocal;
                                /* Nonzero => the opener has logged onto the */
                                /*       local system. */
                                /* Zero => either the opener is remote, or the */
                                /*       the opener is a descendent of a process */
                                /*       that was started remotely. */

                char OpenerIsRemote;  /* was once boolean */
                                /* Nonzero => the opener is on a different */
                                /*       system from the receiver. */
                                /* Zero => the opener is on the same system */
                                /*       as the receiver. */

            } Open;


#pragma PAGE "D00 Open System Message"
        struct

/*
-----------------------------------------------------------------------

       Open System Message.  This variant applies when MsgType = -103.

       This message indicates that another process is attempting to
       open this process.

       You may optionally reply to this message using the 'OpenD00'
       variant of 'SysMsgReply_Struct'.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {

/*                char  ProcNameStr[2:1]; */
                                /* The expression ProcNameStr[ProcNameStrOffset] */
                                /* gives the start of the string-form process */
                                /* name of the opener.  Its length is given */
                                /* by 'ProcNameStrLen'.  This field is empty */
                                /* [the length is zero] if the opener is */
                                /* not named. */

/*                char  HomeTermStr[2:1]; */
                                /* The expression HomeTermStr[HomeTermStrOffset] */
                                /* gives the start of the string-form name */
                                /* of the opener's home terminal.  Its length */
                                /* is given by 'HomeTermStrLen'. */

                short     Access; /* Requested access mode of open.  See */
                                /* the 'xxxxx_AccessD00' #defines. */

                short     Exclusion;
                                /* Requested exclusion mode.  See the */
                                /* 'xxxxx_Exclusion' #defines. */

                short     NowaitDepth;
                                /* Requested nowait depth. */

                short     SyncDepth;
                                /* Requested sync depth. */

                struct  OpenFlagsD00_Struct Flags;
                                /* Flags word from the open call. */

                short    UserId; /* Requester's user id. */

                unsigned int    : 13;
                                /* Reserved bits. */

                unsigned int    RemoteUser : 1;
                                /* True iff the user has not logged on to */
                                /* the local system. */

                unsigned int    RemoteProcess : 1;
                                /* True iff the opener is on a different system. */

                unsigned int    BackupOpen : 1;
                                /* True iff this is a backup open. */

                short    PrimaryPHandle[10];
                                /* For a backup open, this contains the PHandl */
                                /* of the primary opener. */

                short     QualifierLen;
                                /* Length of 'Qualifiers'.  A 'ByteLen'. */

                short     ProcNameStrOffset;
                                /* Offset of 'ProcNameStr', from the beginning */
                                /* of this message.  A 'ByteOffset'. */

                short     ProcNameStrLen;
                                /* Length of 'ProcNameStr'.  A 'ByteLen'. */

                short     PrimaryFileNum;
                                /* For a backup open, the file number of */
                                /* the primary open. */

                short    CreatorUserId;
                                /* The creator accessor id of the opener. */
                                /* A 'UserId'.  [This is the ID of the */
                                /* user that started the requestor process, */
                                /* even if that process is PROGID'd. */

                short     HomeTermStrOffset;
                                /* Offset of 'HomeTermStr', from the beginning */
                                /* of this message.  A 'ByteOffset'. */

                short     HomeTermStrLen;
                                /* Length of 'HomeTermStr'.  A 'ByteLen'. */

                short     Reserved2[5];
                                /* Reserved words. */

                char  Qualifier[1];
                                /* The qualifiers used in opening this process. */
                                /* Their length is given by 'QualifierLen'. */

            } OpenD00;

#pragma PAGE "PowerOn System Message"
        struct

/*
------------------------------------------------------------------------

       PowerOn System Message.  This variant applies when MsgType = -11.

       This message indicates that a CPU has successfully recovered from
       a power failure.

       Receipt of this message is controlled by the system procedure
       'MonitorNew'.

======================================================================
*/
            {

                short     Cpu;
                                /* Number of the CPU that has been powered on. */

            } PowerOn;

#pragma PAGE "Process_Create_ Completion System Message"
        struct

/*
-----------------------------------------------------------------------

       Process_Create_ Completion System Message.  This variant applies when
       MsgType = -102.

       This message indicates that a process creation request, initiated
       by calling 'Process_Create_', has completed.

======================================================================
*/
            {

                long Tag;    /* The tag value supplied to 'Process_Create_'. */

                short    PHandle[10];
                                /* PHandle of the newly created process. */

                short     ErrorCode;
                                /* The same meaning as the 'Error' parameter to */
                                /* 'Process_Create_'.  Zero indicates no error. */

                short     ErrorDetail;
                                /* The same meaning as the 'ErrorDetail' */
                                /* parameter to 'Process_Create_'. */

                short     ProcNameStrLen;
                                /* Length of the 'ProcNameStr' of the newly */
                                /* created process. */

                short     Reserved[4];
                                /* Reserved words. */

                char  ProcNameStr[1];
                                /* char-form name of the newly created */
                                /* process. */

            } ProcessCreate;

#pragma PAGE "Remote Cpu Up/Down System Message"
        struct

/*
-----------------------------------------------------------------------

       Remote Cpu Up and Remote Cpu Down System Messages.  This variant
       applies when MsgType = -100 (Cpu Down) or -113 (Cpu Up).

       This message indicates that a remote CPU somewhere in the network
       went up or down.

       Receipt of this message is controlled by the system procedure
       'MonitorNet'.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {

                long  System; /* System number containing the CPU. */

                short     Cpu;    /* Cpu number that went up or down. */

                short     SystemNameByteLen;
                                /* Length of 'SystemName', in bytes. */

                short     Reserved[3];
                                /* Reserved words. */

                char  SystemName[1];
                                /* Name of the system on which the CPU */
                                /* went down. */

            } RemoteCpuUpDown;

#pragma PAGE "ResetSync System Message"
        struct

/*
----------------------------------------------------------------------

       ResetSync System Message.  This variant applies when MsgType = -34.

       This message indicates that an opener of this process called
       the system procedure 'ResetSync'.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

======================================================================
*/
            {

                short    NoData[1];
                                /* No additional data here. */

            } ResetSync;

#pragma PAGE "SetMode System Message"
        struct

/*
---------------------------------------------------------------------

       SetMode System Message.  This variant applies when MsgType = -33.

       This message indicates that an opener of the current process
       has called the system procedure 'SetMode'.

       Receipt of these messages is controlled by the 'WantSysMsg' bit
       in the open flags for $RECEIVE.

       If you have enabled extended SetMode messages (via
       ReceiveOptions_SetMode), then you must reply to this message
       using the 'SetMode' variant of 'SysMsgReply_Struct'.

======================================================================
*/
            {

                short     Function;
                                /* The 'Function' parameter to SETMODE. */

                short     Param1; /* The 'Param1' parameter to SETMODE. */

                short     Param2; /* The 'Param2' parameter to SETMODE. */

/*                short    Flags[0:-1]; */
                                /* A flags word.  This word exists only if */
                                /* the receiving process has performed a */
                                /* 'ReceiveOptions_SetMode' on $RECEIVE with */
                                /* Param1.<15> = 1. */

                unsigned int    :13;

                unsigned int    Param1Exists : 1;
                                /* 'Param1' was supplied in the call to SETMODE. */

                unsigned int    Param2Exists : 1;
                                /* 'Param2' was supplied in the call to SETMODE. */

                unsigned int    LastParamsExist : 1;
                                /* 'LastParams' was supplied in the call to */
                                /* SETMODE. */

            } SetMode;

#pragma PAGE "SetParam System Message"
        struct

/*
-----------------------------------------------------------------------

       SetParam System Message.  This variant applies when MsgType = -37.

       This message indicates that an opener of this process called
       the system procedure 'SetParam'.

       Receipt of these messages is controlled by Param1.<14> of
       'ReceiveOptions_SetMode' on $RECEIVE.

       You must reply to this message using the 'SetParam' variant
       of 'SysMsgReply_Struct'.

======================================================================
*/
            {

                short     Function;
                                /* The 'Function' parameter to SETPARAM. */

/*                short    Flags[0:-1]; */
                                /* Flag word, as follows: */

                unsigned int    :14;

                unsigned int    ParamArrayExists : 1;
                                /* The caller supplied the 'ParamArray' */
                                /* parameter to SETPARAM. */

                unsigned int    LastParamArrayExists : 1;
                                /* The caller supplied the 'LastParamArray' */
                                /* parameter to SETPARAM. */

                short     Count;  /* Number of bytes in 'ParamArray'. */

                char  ParamArray[1];
                                /* The 'ParamArray' parameter to SETPARAM. */
                                /* This array is 'Count' bytes long. */

            } SetParam;

#pragma PAGE "SetTime System Message"
        struct

/*
-----------------------------------------------------------------------

       SetTime System Message.  This variant applies when MsgType = -10.

       This message indicates that the system time has been changed.

       Receipt of this message is controlled by the system procedure
       'MonitorNew'.

======================================================================
*/
            {

                short     Cpu;
                                /* Number of the CPU in which the time was */
                                /* changed. */

                TM64I   TimeDelta;
                                /* The new time, minus the old time, in */
                                /* microseconds. */

                short     Reason; /* Why the time changed. */
                                /*   0 => time is initially set. */
                                /*   1 => time (GMT and LCT) was manually */
                                /*        changed. */
                                /*   2 => Daylight saving time transition */
                                /*        (GMT unchanged, but LCT changed). */

            } SetTime;

#pragma PAGE "Process Start System Message"
        struct

/*
----------------------------------------------------------------------

       Process Start System Message.  This variant applies when
       MsgType = -9.

       This message is sent to the group leader whenever a process
       within that group creates a new process that is also in the
       group.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see
       'StartD00'.

=====================================================================
*/
            {

                short    ProcessName[4];
                                /* Name of the new process in the group. */

                short    JobId;  /* The number of the job to which this */
                                /* process belongs. */

            } Start;

#pragma PAGE "D00 Process Start System Message"
        struct

/*
---------------------------------------------------------------------

       D00 Process Start System Message.  This variant applies when
       MsgType = -112.

       This message is sent to the group leader whenever a process
       within that group creates a new process that is also in the
       group.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {

                short    JobId;  /* The number of the job to which this */
                                /* process belongs. */

                short    PHandle[10];
                                /* PHandle of the newly created process. */

            }  StartD00;

#pragma PAGE "Stop and Abend System Messages"
        struct

/*
-------------------------------------------------------------------------

       Stop and Abend System Messages.  This variant applies when MsgType
       is -5 (stop) or -6 (abend).

       This message indicates that an unnamed process, or the last member
       of a named process pair has stopped or abended.  It is sent to
       the process's creator, and to the process's group leader, if any.

       Unfortunately, this message exists in at least three subvariants:

       a)      A short message.  This subvariant stops after the
               process name.  This subvariant is generated when either
               the process or the receiver of the message is on a system
               prior to C00.  It can be recognized by its length; it
               stops at field 'ShortMsgEnd'.

       b)      A "forced stop" message.  This subvariant is genereated
               a process is stopped by another process.  It can be
               recognized by its length; it stops at field 'ForceMsgEnd'.

       c)      A normal message.  This subvariant is generated whenever
               the process stops itself.

       You will not receive this message if you request the new format
       of system messages when you call FILE_OPEN.  Instead, see 'StopD00'.

======================================================================
*/
            {

                short    ProcessName[4];
                                /* Name of the process that has terminated. */

      /*          short    ShortMsgEnd[0:-1]; */
                                /* A SHORT termination message ends here. */

                short     HeaderSize;
                                /* The number of words that occur in the message */
                                /* AFTER this word, but before the start of the */
                                /* completion text. */

/*                short    CompText[0:-1]; */
                                /* The expression 'CompText[HeaderSize]' yields */
                                /* the beginning of the text string that was */
                                /* passed to 'Stop' or 'Abend'.  Note that this */
                                /* data actually occurs in the message AFTER */
                                /* all of the other fields.  This data only */
                                /* occurs in NORMAL stop messages. */

                TM64I    CpuTime;
                                /* Amount of time consumed by the process, in */
                                /* microseconds. */

                short    JobId;  /* The number of the job to which this process */
                                /* belonged.  This value is meaningful only to */
                                /* the job's group leader. */

                short     CompCode;
                                /* Completion code for this process.  See the */
                                /* 'xxxx_CompCode' #defines. */

                short    TerminationInfo;
                                /* An additional word passed to the Stop or */
                                /* Abend procedure, for use by the user.  This */
                                /* field is valid in NORMAL stop messages. */
                                /* or */
                                /* The userid of the process that stopped this */
                                /* one.  This field is valid in FORCED */
                                /* stop messages. */

                short    SubSysVendor[4];
                                /* The name of the vendor that defined the */
                                /* subsystem that decided to stop the process. */
                                /* This is "TANDEM  " for Tandem-defined */
                                /* subsystems.  This field is only valid in */
                                /* NORMAL stop messages. */
                                /* or */
                                /* The process name of the process that forced */
                                /* this process to stop.  This field is only */
                                /* valid in FORCED stop messages. */


                short     SubSysId;
                                /* The number of the subsystem that decided to */
                                /* stop the process.  This field is only valid */
                                /* in NORMAL stop messages. */
                                /* and */
                                /* End of Forced stop messages; */

                short     SubSysVersion;
                                /* The version of the subsystem that decided */
                                /* to stop the process.  This field is only */
                                /* valid in NORMAL stop messages. */



                short     TextLength;
                                /* Length of the completion text.  A 'ByteLen'. */

            } StopAbend;

#pragma PAGE "D00 Stop System Message"
        struct

/*
---------------------------------------------------------------------

       Stop System Message.  This variant applies when MsgType = -101.

       This message indicates that an unnamed process, or the last member
       of a named process pair has stopped or abended.  It is sent to
       the process's creator, and to the process's group leader, if any.

       This message is also generated when a NAMED process is lost in
       a CPU failure.  It is not generated when an unnamed process is
       lost, nor when a process is lost in a network partition.

       You will only receive this message if you request the new format
       of system messages when you call FILE_OPEN_.

======================================================================
*/
            {

/*                char  ProcNameStr[2:1]; */
                                /* The expression ProcNameStr[ProcNameStrOffset] */
                                /* is the beginning of the 'ProcNameStr' of the */
                                /* process that terminated. */

                short    PHandle[10];
                                /* PHandle of the process that terminated. */

                TM64I   CpuTime;
                                /* Amount of time consumed by the process, in */
                                /* microseconds. */

                short    JobId;  /* The number of the job to which this process */
                                /* belonged.  This value is meaningful only to */
                                /* the job's group leader. */

                short     CompCode;
                                /* Completion code for this process.  See the */
                                /* 'xxxx_CompCode' #defines. */

                short    TerminationInfo;
                                /* An additional word passed to the Stop or */
                                /* Abend procedure, for use by the user.  This */
                                /* field is only valid in NORMAL stop messages. */
                                /* or */
                                /* The userid of the process that stopped this */
                                /* one.  This field is only valid when */
                                /* 'CompCode' = 'ForcedStop_CompCode'. */

                short    SubSysVendor[4];
                                /* The name of the vendor that defined the */
                                /* subsystem that decided to stop the process. */
                                /* This is "TANDEM  " for Tandem-defined */
                                /* subsystems.  This field is only valid in */
                                /* NORMAL stop messages. */

                short     SubSysId;
                                /* The number of the subsystem that decided to */
                                /* stop the process.  This field is only valid */
                                /* in NORMAL stop messages. */

                short     SubSysVersion;
                                /* The version of the subsystem that decided */
                                /* to stop the process.  This field is only */
                                /* valid in NORMAL stop messages. */

                short    StopperPHandle[10];
                                /* The PHandle of the process that forced */
                                /* this process to stop.  If a process stopped */
                                /* itself, this field is 'Null_Phandle'. */

                short     TextLength;
                                /* Length of 'CompText'.  A 'ByteLen'. */

                short     ProcNameStrOffset;
                                /* Offset, from the beginning of this message, */
                                /* to the start of the 'ProcNameStr' of the */
                                /* terminated process.  A 'ByteOffset'. */

                short     ProcNameStrLen;
                                /* Length of the 'ProcNameStr' of the */
                                /* terminated process.  A 'ByteLen'. */

                unsigned int    :15;
                                /* Reserved bits. */

                unsigned int    Abend : 1;
                                /* True iff the process abended. */

                short     Reserved2[3];
                                /* Reserved words. */

                char  CompText[1];
                                /* The text string passed to STOP or ABEND. */

            } StopD00;

#pragma PAGE "3270 Status System Message"
        struct

/*
-------------------------------------------------------------------------

       3270 Terminal Status Change System Message.  This variant applies
       when MsgType = -21.

       This message indicates that there has been a status change on
       a 3270-type terminal.

       Receipt of this message is controlled by 'Enable3270Status_SetMode'
       on the terminal in question.

======================================================================
*/
            {

                short    ResponseId;
                                /* The response id from the terminal. */

                char  SenseByte;
                                /* The sense byte from the terminal. */

                char  Status;
                                /* The status byte from the terminal. */

                short    Tr3271Status;
                                /* Status in a form suitable for passing */
                                /* to TR3271 via 'Set3270Status_SetMode'. */

            }T3270Status;

#pragma PAGE "TimeOut System Message"
        struct

/*
----------------------------------------------------------------------

       Timeout system message.  This variant applies when MsgType = -22
       (from SignalTimeout) or -26 (from SignalProcessTimeout).

       This message indicates that a time interval created by the
       system procedure 'SignalTimeout' or 'SignalProcessTimeout' has expired.

======================================================================
*/
            {

                short Param1; /* The 'Param1' value supplied to SignalTimeout */
                                /* or SignalProcessTimeout. */

                long Param2; /* The 'Param2' value supplied to SignalTimeout */
                                /* or SignalProcessTimeout. */

            } TimeOut;

        } variant;   /* close the union */

#pragma PAGE "End of System Messages"

    } SysMsg_Type;

#pragma PAGE "System Message Reply Templates"
/*
-------------------------------------------------------------------

       Structures for the replies to various system messages.
       Only a few of the system messages require a non-empty reply.
       The formats of these replies are given in the following
       variants.

       There is no authoritative reference for all of these messages.

       You can use the functional define 'Struct_VariantByteLen'
       (from NSTRUCT) to get the length of any of these replies,
       except for 'SetParam', which is variable-length and must be
       computed at run time.

======================================================================
*/
#pragma FIELDALIGN SHARED2 SysMsgReply_Struct
typedef struct SysMsgReply_Struct
{

        short     MsgType;        /* Type of this message.  This must be the */
                                /* same value contained in the message being */
                                /* replied to. */

        union { /* Contents vary by message type.  See below. */

#pragma PAGE "DeviceInfo System Message Reply"
        struct

/*
---------------------------------------------------------------------

       DeviceInfo System Message Reply.  MsgType = -40.

       This reply passes a process's simulated device type and record
       size back to a caller of 'DeviceInfo' or 'DeviceInfo2'.

======================================================================
*/
            {

                short    DevType;
                                /* Type of device being simulated.  See */
                                /* DeviceInfo_Struct. */

                short    RecordSize;
                                /* Maximum size of the device's "logical */
                                /* record", in bytes.  132 is the traditional */
                                /* value returned by devices that don't */
                                /* really have a maximum. */

            } DeviceInfo;

#pragma PAGE "D00 DeviceInfo System Message Reply"
        struct

/*
---------------------------------------------------------------------

       D00 DeviceInfo System Message Reply.  MsgType = -106.

       This reply passes a process's simulated device type and record
       size back to a caller of 'DeviceInfo' or 'DeviceInfo2'.

       It is used when new-format system messages are used.

======================================================================
*/
            {

                short     DevType;
                                /* Type of the device being simulated. */
                                /* See the 'xxx_DevType' #defines. */

                short     SubType;
                                /* Subtype of the device being simulated. */
                                /* See the 'xxx_yyyyType' #defines. */

                short     MustBeMinusOne[3];
                                /* Reserved words.  These words must be */
                                /* set to -1. */

                short    RecordSize;
                                /* Maximum size of the device's "logical */
                                /* record", in bytes.  132 is the traditional */
                                /* value returned by devices that don't */
                                /* really have a maximum. */

            } DeviceInfoD00;

#pragma PAGE "Name Inquiry System Message"
        struct

/*
--------------------------------------------------------------------

       Name Inquiry System Message Reply.  MsgType = -107.

       This reply conveys information about an enquired name back
       to the requester.

       Note that if the current process owns no name that matches
       the given search criteria, it should reply to the request with
       error FEEOF.

       This struct is variable-length.  You must use
               $OFFSET(NameInquiry.Name) + NameInquiry.NameLen
       as the length of the reply.

======================================================================
*/
            {

                short     DevType;
                                /* The device type of the returned name.  See */
                                /* the 'xxxx_DevType' #defines.  This field is */
                                /* relevant only for subtype 30 processes. */

                short     SubType;
                                /* The device subtype of the returned name. */
                                /* See the 'xxxx_yyyyType' #defines.  This */
                                /* field is relevant only for subtype 30 */
                                /* processes. */

                short     MustBeMinusOne[3];
                                /* Reserved words, which you must set to -1. */

                short     NameLen;
                                /* Length of the returned name.  A 'ByteLen'. */

                char  Name[1];
                                /* char-form qualifiers of the item being */
                                /* enquired about.  The name length is given */
                                /* by 'NameLen'. */

            } NameInquiry;

#pragma PAGE "D00 Open System Message Reply"
        struct

/*
---------------------------------------------------------------------

       D00 Open System Message Reply.  MsgType = -103.

       This reply may optionally be used to associate information with
       an open, so that this information can later be determined from
       an incoming request by calling File_GetReceiveInfo_.

       Note that this feature is supported only when the requester and
       server are BOTH on D00 or later systems.

======================================================================
*/
            {

                short    Tag;    /* A tag to be associated with the */
                                /* open. */

            } OpenD00;

#pragma PAGE "SetMode System Message Reply"
        struct

/*
---------------------------------------------------------------------

       SetMode System Message Reply.  MsgType = -33.

       This reply passes the previous value of the setmode parameters
       back to the requester.

======================================================================
*/
            {

                short     Param1; /* The old value of 'Param1'. */

                short     Param2; /* The old value of 'Param2'. */

            }SetMode;

#pragma PAGE "SetParam System Message Reply"
        struct

/*
---------------------------------------------------------------------

       SetParam System Message Reply.  MsgType = -37.

       This reply passes the previous value of the parameter array back
       to the requester.

======================================================================
*/
            {

                short     Count;  /* Number of bytes in 'ParamArray'. */

                char  ParamArray[1];
                                /* The previous contents of the parameter array. */
                                /* This array is 'Count' bytes long. */

            } SetParam;
        } variant;  /* close the union */
#pragma PAGE "End of System Message Replies"

    } SysMsgReply_Type;

#endif
