/* ------------------------------------------------------------------------
* Filename:     GError.java
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


/**
 * The <code>GError</code> class contains static variables for
 * Guardian file system error numbers.  This allows users to code the
 * following: <pre><code>
 * import com.tandem.ext.guardian.*;
 * try {
 *    . . .
 * } catch (GuardianException ex) {
 *   // ignore file not found errors
 *   if (ex.getError() != GError.ENOTFOUND) throw ex;
 * } 
 * <code></pre>
 * Please read the Guardian Procedure Errors and Messages Manual for more
 * information on the meaning of the errors and how to recover from them.
 * @version 1.0, 11/14/2001
 */
public class GError {

   /** (0) The operation completed successfully. */
   public static final int EOK = 0;

   /** (1) A read procedure reached end-of-file
       or a write procedure reached end of
       tape. */
   public static final int EEOF              = 1; 
   /** (2) The operation specified is not 
       allowed on this type of file. */  
   public static final int EINVALOP          = 2; 
   /** (3) An open or purge of a partition 
       failed. */ 
   public static final int EPARTFAIL         = 3; 
   /** (4) An open operation for an 
       alternate key file failed. */   
   public static final int EKEYFAIL          = 4; 
   /** (5) Sequential buffering not used 
       because no space was available
          in the PFS for the sequential
       block buffer. */
   public static final int ESEQFAIL          = 5; 
   /** (6) A system message was received from 
       another process.*/ 
   public static final int ESYSMESS          = 6; 
   /** (7) This process cannot accept a 
       CONTROL, CONTROLBUF, SETMODE, or
          RESETSYNC because $RECEIVE is not
          opened correctly.*/    
   public static final int ENOSYSMESS        = 7; 
   /** (8) The operation was successful 
       (examine MCW for additional 
          status). */
   public static final int EENVOYOPOK             = 8;
   /** (9) A read through a locked record 
       was successful. */
   public static final int EREADLOCKED       = 9; 

   /** (10) The new record or file could 
       not be created because a file 
          by that name or a record with
       that key already exists. */    
   public static final int EBADERR          = 10; 
   /** (11) The file is not in the directory
       or the record is not in the file,
          or the specified tape file is not
          on a labeled tape. */
   public static final int ENOTFOUND        = 11; 
   /** (12) The file is in use. */ 
   public static final int EINUSE           = 12; 
   /** (13) The filename was not specified 
       in proper form. */      
   public static final int EBADNAME         = 13; 
   /** (14) That device does not exist on 
       this system; device or process is
       inaccessible from C-series system 
          (e.g. device or process running at
          a pin greater than 255, device has
          logical device greater than 4095 
          or device has type or subtype 
       greater than 63). */               
   public static final int ENOSUCHDEV       = 14; 
   /** (15) The volume or system specified 
       for a file RENAME operation does not 
          match the name of the volume or 
       system of the file. */
   public static final int EWRONGDEV        = 15; 
   /** (16) No file with that file number 
       has been opened. */
   public static final int ENOTOPEN         = 16; 
   /** (17) A paired-open was specified and 
       the file is not open by the primary
       process, the parameters supplied 
          do not match the parameters supplied 
          when the file was opened by the 
       primary, or the primary process 
       is not alive. */
   public static final int ENOPRIMARY       = 17; 
   /** (18) The system specified does not 
       exist in the network. */
   public static final int ENOSUCHSYS       = 18; 
   /** (19) The system specified does not 
       exist in the network. */
   public static final int ENOMOREDEVS      = 19; 
   /** (20) File name is too long to be 
       opened across a network, or swap 
       file orsystem name is not 
       acceptable. */
   public static final int ENETVIOL         = 20; 
   /** (21) An illegal <count> was 
       specified in a file-system call,
          or the operation attempted to 
          transfer too much or too little 
       data. */
   public static final int EBADCOUNT        = 21; 
   /** (22) The application parameter or 
       buffer address is out of bounds. */
   public static final int EBOUNDSERR       = 22; 
   /** (23) The disk address is out of bounds,
       or the maximum number of blocks in
          an alternate-key file is exceeded. */
   public static final int EBADADDR         = 23; 
   /** (24) Privileged mode is required for
       this operation. */
   public static final int ENOTPRIV         = 24; 
   /** (25) AWAITIO[X] or CANCEL attempted on
       file opened for waited I/O. */
   public static final int EWAITFILE        = 25; 
   /** (26) AWAITIO[X] or CANCEL or CONTROL 
       22 attempted on a file with no 
          outstanding I/O requests. */
   public static final int ENONEOUT         = 26; 
   /** (27) An operation was attempted with 
       outstanding no-waited I/O requests
       pending. */
   public static final int EIOPEND          = 27; 
   /** (28) The number of outstanding nowait 
       operations would exceed that 
          specified; an attempt was made to
          open a disk file or $RECEIVE with
          the maximum number of concurrent
          operations more than 1; an attempt
          to ADD more than the configured
          maximum number of subdevices for an
          I/O process; or sync depth exceeds
          number the opener can handle; or 
          trying to run more than 254 processes
       from the same object file. */
   public static final int ETOOMANY         = 28; 
   /** (29) A required parameter is missing 
       in a procedure call, or two mutually
       exclusive parameters were supplied. */
   public static final int EMISSPARM        = 29;
   /** (30) Message system is unable to 
       obtain memory because either no entry 
          is available in the message block pool
          or the maximum number of RECEIVE or
       SEND message blocks are already in 
       use. */
   public static final int ENOLCB           = 30; 
   /** (31) Unable to obtain file-system 
       buffer space. */
   public static final int ENOBUFSPACE      = 31; 
   /** (32) Unable to obtain storage pool 
       space (SYSPOOL or EXTPOOL). */
   public static final int ENOPOOLSPACE     = 32; 
   /** (33) I/O process is unable to obtain 
       sufficient buffer space. */
   public static final int ENOIOBUFSPACE    = 33; 
   /** (34) Unable to obtain a file-system 
       control block. */
   public static final int ENOCBSPACE       = 34; 
   /** (35) Unable to obtain an I/O process 
       control block, or the transaction 
          or open lock unit limit has been 
       reached. */
   public static final int ENOIOCBSPACE     = 35; 
   /** (36) Unable to lock physical memory; 
       not enough memory available. */
   public static final int ENOPHYSMEM       = 36; 
   /** (37) I/O process is unable to lock 
       physical memory. 
       (device type: any except 2) */      
   public static final int ENOIOPHYSMEM     = 37;
   /** (38) Attempt to perform operation on 
       wrong type of TNS system. */
   public static final int EWRONGTNS        = 38; 
   /** (39) The server process received a 
       request with a sync ID older 
          than the set of saved replies. */
   public static final int EOLDSYNC         = 39; 
   /** (40) The operation timed out. AWAITIO[X]
       or FILE_COMPLETE_ did not complete
       within the time specified by its
       <time-limit> parameter. */
   public static final int ETIMEDOUT        = 40; 
   /** (41) A checksum error occurred on a
       file synchronization block. */
   public static final int EBADSYNC         = 41; 
   /** (42) Attempt to read from unallocated
       extent. */
   public static final int EUNALLOC         = 42; 
   /** (43) Unable to obtain disk space for
       file extent. */
   public static final int ENODISCSPACE     = 43; 
   /** (44) The disk directory or DCT is 
       full.*/
   public static final int EDIRFULL         = 44; 
   /** (45) The file is full; or two entries 
       for <process-name> were already 
          in the PPD. */
   public static final int EFILEFULL        = 45; 
   /** (46) An invalid key was specified; key
       length passed to CREATE exceeds 255
       bytes; or application failed to 
       open an alternate-key file. */
   public static final int EINVKEY          = 46; 
   /** (47) The alternate key data is not 
       consistent with primary file data.*/
   public static final int EKEYSBAD         = 47; 
   /** (48) Security violation; illegal 
       operation attempted. */
   public static final int ESECVIOL         = 48; 
   /** (49) Access violation; attempt to use 
       an unexpired labeled tape for 
          output; or a mismatch between a
          DEFINE Use attribute (input or 
          output/extend) and the current
       operation (read or write). */
   public static final int EACCVIOL         = 49; 
   /** (50) Directory error on a disk 
       volume. */
   public static final int EDIRERR          = 50; 
   /** (51) Directory on a disk volume is 
       marked bad. */
   public static final int EDIRBAD          = 51; 
   /** (52) Error in the disk free space 
       table. */
   public static final int EDFSERR          = 52; 
   /** (53) File system internal error or 
       CP6100 file management interface
       error. */
   public static final int EFSERR           = 53; 
   /** (54) I/O error in disk free space 
       table or in DP2 undo area. */
   public static final int EDFSIO           = 54; 
   /** (55) I/O error in disk directory; the
       file is no longer accessible. */
   public static final int EDIRIO           = 55; 
   /** (56) I/O error on disk volume label; 
       the volume is no longer accessible. */
   public static final int EVLABIO          = 56; 
   /** (57) The disk free space table is
       full.*/
   public static final int EDFSFULL         = 57; 
   /** (58) The disk free space table is 
       marked bad. */
   public static final int EDFSBAD          = 58; 
   /** (59) The disk file is bad; there is  
       a mismatch in the internal FCB, or 
       the file structure in a structured
       file is inconsistent. */
   public static final int EFILEBAD         = 59; 

   /** (60) The file resides on a removed
          volume, the device is downed or not
          open, or a server has failed and a 
          process has been replaced by a 
          different process with the same name 
          since the server was opened.  */
   public static final int EWRONGID         = 60; 
   /** (61) No more file opens are permitted 
       on this volume or device; the system
       operator issued a STOPOPENS command
          or the number of open files reached
       the maximum allowed. */
   public static final int EOPENSTOP        = 61; 
   /** (62) The volume was mounted but no 
       mount order was given, so the file 
       open is not permitted. */
   public static final int EUNEXMOUNT       = 62; 
   /** (63) The volume was mounted and the 
       mount is in progress, so a file
          open is not permitted. */
   public static final int EMOUNTEXP        = 63; 
   /** (64) The volume was mounted and the 
       mount is in progress, so a file
          open is not permitted. */
   public static final int EMOUNTINPROG     = 64; 
   /** (65) Only special requests permitted--
       or special request to mirrored 
          disk pair attempted with only
       one device in special state. */
   public static final int ESPECONLY        = 65; 
   /** (66) The device is downed, the LIU is
       not yet downloaded, a hard failure
       occurred on the controller, the 
          disk and controller are not 
          compatible (DP1/DP2), or both halves
       of a mirrored disk are down. */
   public static final int EDEVDOWN         = 66; 
   /** (70) Continue the file operation. */
   public static final int ECONTINUE        = 70; 
   /** (71) A duplicate record was 
       encountered. */
   public static final int EDUPREC          = 71; 
   /** (72) An attempt was made to access an 
       unmounted or nonexistent partition,
          or to access a secondary 
       partition. */
   public static final int EBADPART         = 72; 
   /** (73) The disk file or record is 
       locked. */
   public static final int ELOCKED          = 73; 
   /** (74) Number of READUPDATES without 
       replies exceeds <receive-depth>, 
          or ACTIVATERECEIVETRANSID or 
       REPLY called with an invalid 
       <message tag>. */
   public static final int EBADREPLY        = 74; 
   /** (75) Requesting process has no current 
       process transaction identifier.*/
   public static final int ENOTRANSID       = 75; 
   /** (76) Transaction is in the process of 
       ending.*/
   public static final int EENDEDTRANSID    = 76; 
   /** (78) Transaction identifier is invalid 
       or obsolete. */
   public static final int EINVTRANSID      = 78; 
   /** (79) A transaction attempted to update
       or delete a record which it has not
       previously locked. */
   public static final int ENOTRANSLOCK     = 79; 
   /** (80) Invalid operation on audited file 
       or non-audited disk volume. */
   public static final int EAUDITINVALOP    = 80; 
   /** (81) Operation is not valid for a 
       transaction which still has nowait 
          I/Os outstanding on a disk or
       process file. */
   public static final int ETRANSNOWAITOUT  = 81; 
   /** (82) TMF not running on this system or 
       on the remote system. */
   public static final int ETMFNOTRUNNING   = 82; 
   /** (83) Attempt to begin more concurrent 
       transactions than can be handled. */
   public static final int ETOOMANYTRANSBEGINS = 83; 
   /** (84) TMF has not been configured on 
       this system or on the remote 
       system. */
   public static final int ETMFNOTCONFIGURED = 84; 
   /** (85) A device has not been started for 
       TMF. */
   public static final int EDEVICEDOWNFORTMF = 85; 
   /** (86) BEGINTRANSACTION is disabled either
       by the operator or because one or more
       TMF limits have been reached. */
   public static final int EBEGINTRDISABLED = 86; 
   /** (87) Waiting on a READ request and did 
       not get it. */
   public static final int EEXPECTEDREAD    = 87; 
   /** (88) A CONTROL READ is pending so a 
       second READ is not valid. */
   public static final int EINVALIDSEQUENCE = 88; 
   /** (89) A remote device cannot accept text 
       because it has no buffer 
       available. */
   public static final int ENODEVBUFSPACE   = 89; 
   /** (90) The transaction was aborted by the 
       system because its parent process died,
       a server using the transaction failed,
          or a message to a server using the
       transaction was cancelled. */
   public static final int ETRANSABRTOWNERDIED = 90; 
   /** (91) A TMF crash occurred during committment
       of the transaction; the transaction may
       or may not have been committed. */
   public static final int ETRANSABRTBADDBID = 91; 
   /** (92) Distributed transaction aborted 
       by system because the path to a remote
       system that was part of the 
       transaction was down. */
   public static final int ETRANSABRTNETDOWN = 92; 
   /** (93) A transaction was aborted because 
       it spanned too many audit trail 
       files. */
   public static final int ETRANSABRTAUDOVFL = 93; 
   /** (94) A transaction was aborted by operator 
       command. */
   public static final int ETRANSABRTOPRCMD = 94; 
   /** (95) A transaction was aborted because 
       of disc process takeover. */             
   public static final int ETRANSABRTDISCTKOVR = 95; 
   /** (96) The transaction was aborted because 
       it exceeded the AUTOABORT timeout 
       duration. */
   public static final int ETRANSABRTTIMEOUT = 96;   
   /** (97) Transaction aborted by call to 
       ABORTTRANSACTION. */
   public static final int EABORTEDTRANSID  = 97; 
   /** (98) Allocation of a Transaction Control 
       Block failed because the local table
       is full, or the table on a remote
       system is full. */
   public static final int ENOMORETCBS      = 98; 
   /** (99) Process attempted to use features 
       of a microcode option that is not
       installed on this system. */
   public static final int ENOTINSTALLED   = 99; 
   /** (100) Device is not ready or the 
       controller is not operational. */
   public static final int ENOTRDY         = 100; 
   /** (101) The tape is write 
       protected. */
   public static final int ENOWRTRING      = 101; 
   /** (102) Printer paper out, bail open 
       or end of ribbon. */
   public static final int EPAPEROUT       = 102; 
   /** (103) Disk not ready due to power 
       failure. */
   public static final int EPONNOTRDY      = 103; 
   /** (104) No response from printer. */
   public static final int ENORESPONSE     = 104; 
   /** (105) Invalid printer VFU buffer. */
   public static final int EVFUERR         = 105; 
   /** (106) A buffered WRITE has failed; 
       data in printer buffer was lost. */
   public static final int EPRIORWRITEFAIL = 106; 
   /** (110) Only BREAK access is 
       permitted. */
   public static final int EBREAKONLY      = 110; 
   /** (111) Operation aborted because 
       of BREAK.*/
   public static final int EBREAK          = 111; 
   /** (112) READ or WRITEREAD preempted by 
       operator message. */
   public static final int EOPWRITEABORT   = 112; 
   /** (112) Too many user console 
       messages. */   
   public static final int EOPTOOMANY      = 112; 
   /** (113) DEFINE class or attributes 
       are not valid for the attempted 
       function. */
   public static final int EBADDEFUSE      = 113; 
   /** (114) X25 Network problem 
         - RESTART FAILURE. */
   public static final int EL3T20TO        = 114; 
   /** (115) X25 Network problem 
         - RESET FAILURE. */
   public static final int EL3T22TO        = 115; 
   /** (119) Error code value was too large 
       to fit into an 8-bit buffer; 
          file-system error number is greater 
       than 255. */
   public static final int EERRTOOBIG      = 119; 
   /** (120) Data parity error, or attempt 
       to access a tape whose density is 
          higher than the switch setting
       on the tape drive. */
   public static final int EDATAPARITY     = 120; 
   /** (121) Data overrun error, hardware 
       problem. */
   public static final int EOVERRUN        = 121; 
   /** (122) Request aborted due to 
       possible data loss caused by reset 
          of circuit, CLB sequence error; or 
       DP2 disk process takeover. */
   public static final int EDATALOSS       = 122; 
   /** (123) Subdevice is busy. */
   public static final int ESUBBUSY        = 123; 
   /** (124) A line reset is in progress, 
       loss of data is possible. */
   public static final int ELINERESET      = 124; 
   /** (130) 1st disc error. */ 
   public static final int EFIRSTDISC      = 130; 
   /** (130) Illegal disk address requested, 
       or formatting error occurred. */                   
   public static final int EADDRCHK        = 130; 
   /** (131) Write-check error from disk; 
       internal circuitry fault. */
   public static final int EWRTCHECK       = 131; 
   /** (132) Seek incomplete from disk;
       cylinder address not reached after
       retry. */
   public static final int ESEEKUNC        = 132; 
   /** (133) Access not ready on disk; 
       cylinder address not reached. */
   public static final int EACCNOTRDY      = 133; 
   /** (134) Address compare error on 
       disk. */
   public static final int EADDRCOMP       = 134; 
   /** (135) Write-protect violation with 
       disk write. */
   public static final int EWRTPROTECT     = 135; 
   /** (136) Disk unit ownership error 
       (dual-port disk). */
   public static final int EUNITOWN        = 136; 
   /** (137) Controller buffer parity 
       error. */
   public static final int EBUFPARITY      = 137; 
   /** (138) Interrupt overrun; a device 
       interrupted the processor before
          the software could respond. */
   public static final int EINTERRUPTOVERRUN = 138; 
   /** (139) Controller error; internal 
       diagnostic failure. */
   public static final int ECTLRERR        = 139; 
   /** (139) Last disc error. */
   public static final int ELASTDISC       = 139; 

   /** (140) Modem error. (communication 
       link not yet established, modem 
          failure, momentary loss of carrier,
          or disconnect) */
   public static final int EMODEMERR       = 140; 
   /** (140) FOX link to an EXPAND line 
       handler is down. */
   public static final int EDISCONNECT     = 140; 
   /** (145) Card reader motion check 
       error. */
   public static final int EMOTIONCHK      = 145; 
   /** (146) Card reader read check 
       error. */
   public static final int EREADCHK        = 146; 
   /** (147) Card reader invalid Hollerith 
       code read. */
   public static final int EINVALHOLRTH    = 147; 
   /** (148) Attempt to read unwritten 
       data or to over-write existing data 
          on an optical disk. */
   public static final int EWORMSEQ        = 148; 
   /** (150) End-of-tape marker 
       detected. */
   public static final int EEOT            = 150; 
   /** (151) Runaway tape detected, or 
       attempt to access a tape whose density 
          is lower than the switch setting
       on the tape drive. */
   public static final int ERUNAWAY        = 151; 
   /** (152) Unusual end--tape unit went 
       offline or CP6100 file closed 
          while requests still in progress. */
   public static final int EOPABORT        = 152; 
   /** (153) Tape drive power restored. */
   public static final int ETAPEPON        = 153; 
   /** (154) BOT detected during backspace 
       files or backspace records. */
   public static final int EBOT            = 154; 
   /** (155) Only nine-track magnetic tape 
       allowed on this system. */
   public static final int E9TRKTAPEONLY   = 155; 
   /** (156) Tape command rejected, TIL or TLAM
       protocol violation detected, or X25AM
       level-4 ITI protocol violation. */
   public static final int EIOPROTOCOLERR  = 156; 
   /** (157) I/O process internal system
       error. */
   public static final int EIOPROCERR      = 157; 
   /** (158) Invalid function code 
       requested for Hyper Link. */
   public static final int EBADTHLFUNC     = 158; 
   /** (159) Device mode wrong for Hyper 
       Link I/O request, or attempt to 
          execute PUP PRIMARY or SCF PRIMARY 
          command while the tape drive is 
          waiting for a labeled tape to be 
       mounted. */
   public static final int EWRONGDEVMODE   = 159; 
   /*
     Data communications error numbers:
            160 through 189
   */
   /** (160) Request is invalid for device 
       state; protocol error. */
   public static final int EINVALIDSTATE   = 160; 
   /** (161) Impossible event occurred for 
       line state. */
   public static final int EBADSTATE       = 161; 
   /** (162) Operation timed out */
   public static final int EOPERATIONTIMEDOUT = 162;  
   /** (163) EOT received or power at autocall 
       unit is off. */
   public static final int ERECEIVEDEOT    = 163; 
   /** (164) Disconnect received or data 
       line is occupied (busy). */
   public static final int ERECEIVEDDISC   = 164; 
   /** (165) RVI received or data line not 
       occupied after setting call 
       request. */
   public static final int ERECEIVEDRVI    = 165; 
   /** (166) ENQ received or auto call 
       unit failed to set 
       present-next-digit. */
   public static final int ERECEIVEDENQ    = 166; 
   /** (167) EOT received on line bid, or 
       data-set-status not set. */
   public static final int ERECEIVEDEOTBID = 167; 
   /** (168) NAK received on line bid, or 
       auto-call unit failed to clear
       present-next-digit after 
       digit-present was set. */
   public static final int ERECEIVEDNAKBID = 168; 
   /** (169) WACK received on line bid, 
       auto-call unit set 
          abandon-call-and-retry, or
       station disabled or undefined. */
   public static final int ERECEIVEDWACKBID = 169; 
   /** (170) No ID sequence received during 
       circuit assurance mode or invalid
          MCW entry number on write. */
   public static final int ENOSEQUENCEID   = 170; 
   /** (171) No response received on 
       bid/poll/select, or reply invalid. */
   public static final int ENOTREPLYING    = 171; 
   /** (172) Reply not proper for protocol; 
       invalid control sequence or 
       invalid data. */
   public static final int EIMPROPERREPLY  = 172; 
   /** (173) Maximum allowable NAKs received 
       (transmission error), invalid MCW
          on WRITE, or invalid request ID. */
   public static final int EMAXNAKS        = 173; 
   /** (174) WACK received or CLB frame 
       aborted. */
   public static final int ERECEIVEDWACK   = 174; 
   /** (175) Incorrect alternating ACK 
       received, or command rejected. */
   public static final int EWRONGACK       = 175; 
   /** (176) Poll sequence ended with no 
       responder. */
   public static final int EPOLLEND        = 176;
   /** (177) Text overrun (insufficient 
       buffer space for data transfer). */
   public static final int ETEXTOVERRUN    = 177;
   /** (178) No address list specified. */
   public static final int EADDRERROR      = 178;
   /** (179) Application buffer is 
       incorrect, control request pending,
          or autopoll active. */
   public static final int EBADFORMAT      = 179;
   /** (180) Unknown device status 
       received. */
   public static final int ERECEIVEDSTATUS = 180;
   /** (181) Subdevice expected status 
       information but received data 
       instead. */
   public static final int ERECEIVEDDUPSTATUS = 181;
   /** (182) SNALU access method outbound RU 
       error; see SNALU error code in status
          area of SNAX header. 
       (device type: 14.2) */
   public static final int ESNALUOUTBOUNDRU   = 182;
   /** (183) SNA session has ended. 
       (device type: 14.2) */
   public static final int ESNASESSIONENDED   = 183;
   /** (187) Operation returning with no 
       useful data. */
   public static final int ENODATA         = 187;
   /** (188) Damage to logical flow of
       events.*/
   public static final int EDATALOST       = 188;
   /** (189) Response not yet available. */
   public static final int EDATACOMING     = 189;
   /** (190) Device error; hardware 
       problem. */
   public static final int EDEVERR         = 190;
   /** (191) Device power on, or terminal 
       reset. */
   public static final int EDEVPON         = 191;
   /** (192) Device in diagnose mode; 
       system operator running 
       diagnostics. */
   public static final int EDEVXERCISED    = 192;
   /** (193) Invalid or missing microcode 
       file.*/
   public static final int ENOMICROCODE    = 193;
   /** (194) Device use or mount request 
       rejected by operator. */
   public static final int EOPRREJECT      = 194;
   /** (195) Operation requires use of 
       $ZSVR but it is not running; tape 
          operation is not allowed. */
   public static final int ENOZSVR         = 195;
   /** (196) A tape label record is 
       missing or incorrect. */
   public static final int EBADTAPELABEL   = 196;
   /** (197) An SQL error has occurred. */
   public static final int ESQLERR         = 197;
   /** (198) A DEFINE of the given name 
       could not be found. */
   public static final int EMISSDEF        = 198;
   /** (199) The disk file is SAFEGUARD 
       protected. */
   public static final int EOBIPROTECTED   = 199;
   /** (200) The device is owned by an 
       alternate port. */
   public static final int EOWNERSHIP      = 200;
   /** (201) The current path to the device 
       is down, an attempt was made to write
          to a non-existent process, the 
          message-system request was 
          incorrectly formatted, or an error
       was found in the message system 
       interface. */
   public static final int EPATHDOWN       = 201;
   /** (210) Device ownership changed 
       during operation. */
   public static final int EOWNABORT       = 210;
   /** (211) The CPU performing the 
       operation failed during the 
       operation. */
   public static final int ECPUFAIL        = 211;
   /** (213) Channel data parity error 
       (path error). */
   public static final int ECHPARITY       = 213;
   /** (214) Channel timeout 
       (path error). */
   public static final int ECHTIMEOUT      = 214;
   /** (215) I/O attempted to absent 
       memory page 
       (hardware path error). */
   public static final int EMEMABS         = 215;
   /** (216) Memory breakpoint encountered 
       during this I/O operation.*/
   public static final int EMEMBRKPT       = 216;
   /** (217) Memory parity error during 
       this I/O (hardware path error). */
   public static final int EMEMPARITY      = 217;
   /** (218) Interrupt timeout occurred on 
       a channel, or a controller, modem,
          or the line between, or lost the
       modem clock (path error). */
   public static final int EINTTIMEOUT     = 218;
   /** (219) Illegal device reconnect 
       (path error). */
   public static final int EILLRECON       = 219;
   /** (220) Protect violation; an I/O
       controller attempted an illegal 
       write. */
   public static final int EPROTECT        = 220;
   /** (221) Controller handshake 
       violation (path error). */
   public static final int EHANDSHAKE      = 221;
   /** (222) Bad channel status from EIO 
       instruction (path error). */
   public static final int EBADEIOSTAT     = 222;
   /** (223) Bad channel status from IIO 
       instruction (path error). */
   public static final int EBADIIOSTAT     = 223;
   /** (224) Controller error 
       (fatal error). */
   public static final int ECTLERR         = 224;
   /** (225) No unit assigned or multiple 
       units assigned to the same unit
          number (path error). */
   public static final int EUNITPOLLERR    = 225;
   /** (230) CPU power failed, then 
       restored. */
   public static final int ECPUPON         = 230;
   /** (231) Controller power failed, then 
       restored. */
   public static final int EIOPON          = 231;
   /** (232) Access is denied due to error
       in communication with SMON. */
   public static final int ESMONERR        = 232;
   /** (233) Error in call to 
       SERVERCLASS_SEND_. */
   public static final int ESCERROR        = 233;
   /** (240) First of the network 
       errors. */                 
   public static final int ENETERRLOW      = 240;
   /** (240) Network line handler error; 
       operation not started. EXPAND 
          performed an ownership switch. */
   public static final int ELHRETRY        = 240;
   /** (241) Network error; operation not 
       started. */
   public static final int ENETRETRY       = 241;
   /** (246) External cluster bypass error; 
       operation aborted. */
   public static final int EBYPASSFAIL     = 246;
   /** (248) Network line handler error; 
       operation aborted.  */
   public static final int ELHFAIL         = 248;
   /** (249) Network error; operation 
       aborted. */
   public static final int ENETFAIL        = 249;
   /** (250) All paths to the system are 
       down. */
   public static final int ENETDOWN        = 250;
   /** (251) Network protocol error 
       (path error). */
   public static final int ENETPROTOCOLERROR = 251;
   /** (252) Required EXPAND class is not 
       available. */
   public static final int ENOEXPANDCLASS  = 252;
   /** (255) Net line handler flooded; 
       too many interrupts. */
   public static final int ELHTOOMANYUPS   = 255;
   /** (255) Last of the network errors. */
   public static final int ENETERRHIGH     = 255;

   /* errors 300 to 511 are reserved for use by the user. */
   
   /** (541) A data structure version is 
       incompatible with the requested 
          operation. */
   public static final int EINCOMPATIBLEVERSION = 541;
   /** (549) Blockmode is not currently allowed 
       on this terminal. */
   public static final int ENOBLOCKMODE    = 549;
   /** (550) File operation attempted at 
       illegal position. */
   public static final int EILLPOS         = 550;
   /** (551) Duplicate exists for 
       insertion-ordered alternate key. */
   public static final int EDUPALTKEY      = 551;
   /** (560) The function cannot be 
       performed because one of the  
          processes involved cannot run 
          with a PIN greater than 255. */
   public static final int EFATPIN         = 560;
   /** (561) The item code in a list is 
       not recognized. */
   public static final int EBADITEM        = 561;
   /** (563) The size of an output buffer 
       was too small. */
   public static final int EBUFTOOSMALL    = 563;
   /** (564) The operation is not supported 
       for this file type. */
   public static final int EBADOP          = 564;
   /** (565) A malformed request was 
       denied. */
   public static final int EILLEGALREQUEST = 565;
   /** (566) This reply is malformed or not 
       appropriate. */
   public static final int EILLEGALREPLY   = 566;
   /** (567) The define used is 
       incompatible for use with target 
          system's TOS version. */
   public static final int ETOSWRONGFORDEFINE = 567;
   /** (570) An out-of-sequence message was 
       received. */
   public static final int ESEQORDER       = 570;
   /** (571) A duplicate message was 
       received. */
   public static final int ESEQDUP         = 571;
   /** (572) Message cannot be accepted 
       because sequence number has been
       reset. */
   public static final int ESEQRESET       = 572;
   /** (573) The requested process handle 
       cannot be returned. */
   public static final int ELDEVPHANDLE    = 573;
   /** (578) The block size specified is
       too large. */
   public static final int EBLOCKTOOBIG    = 578;
   /** (579) The record size specified is 
       too large for the given block size, 
          file type and format. */
   public static final int  ERECTOOBIG    = 579;
   /** (580) An open failed because the 
       file was oversize and the opener 
          did not specify use of 64-bit 
          primary keys.*/
   public static final int EOVERSIZEFILE  = 580;
   /** (581) An operation involving 32-bit
       primary keys was attempted on an
          open which specified use of 64-bit
       keys. */
   public static final int EBIGKEYS       = 581;
   /** (582) Alternate key information 
       could not be returned because it 
          cannot be expressed in the 
          superseded format of the 
       parameter. */
   public static final int EALTSUPERSEDED = 582;
   /** (583) An extent size specified is 
       too large. */
   public static final int EEXTTOOBIG     = 583;
   /** (584) The operation could not be 
       performed because a software 
          component does not support format 
       2 disk files. */
   public static final int ENOFORMAT2SUPPORT = 584;
   /** (586) Transaction_Keep_ was attempted 
       without a valid transaction dialog
       message. */
   public static final int EINVALIDKEEPMSG = 586;
   /** (587) Transaction_Keep_ was called 
       when there was already a kept 
       transaction. */
   public static final int ETOOMANYKEEPS   = 587;
   /** (588) There was no kept transaction 
       but the operation required one. */
   public static final int ENOKEPTTRANS    = 588;
   /** (589) The operation could not be 
       performed because there was a 
       kept transaction.*/
   public static final int EOUTSTANDINGKEEP = 589;
   /** (590) The parameter value is invalid 
       or inconsistent with another. */
   public static final int EBADPARMVALUE   = 590;
   /** (593) The request was cancelled. */
   public static final int EREQABANDONED   = 593;
   /** (594) A DSM/TC error was returned to 
       $ZSVR. Refer to the EMS log for 
          detailed information. */
   public static final int EDZMTCERRTOZCVR = 594;
   /** (595) A ZSSI error was returned to
       $ZSVR. Refer to the EMS log for 
          detailed information. */
   public static final int EZSSIERRTOZCVR  = 595;
   /** (597) A required item is missing 
       from an item list.  */
   public static final int EMISSITEM       = 597;
   /** (632) Not enough stack space to
       complete request. */
   public static final int ESHORTSTACK     = 632;
   /** (633) Operation is invalid because 
       MEASURE is running. */
   public static final int E_MEASURE_RUNNING = 633;
   /** (634) A logical device number exceeded
       16 bits. */
   public static final int EFATLDEV        = 634;
   /** (635) A volume cannot be accessed 
       because the other side is locked. */
   public static final int EUNITLOCK       = 635;
   /** (637) Used by DP2 in SETCLOSE. */
   public static final int EUSEOLDWAY       = 637;
   /** (638) Process cannot be stopped until 
       process returns to stopmode 1. */
   public static final int EQUEUEDSTOPMODE1 = 638;
   /** (639) Process cannot be stopped until 
       process goes to stopmode 0. */
   public static final int EQUEUEDSTOPMODE0 = 639;
   /** (700) The sequence number of the 
       message received by the disk process 
          from a recovery process does not 
       match. */
   public static final int EWRONGRECOVSEQNUM          = 700;
   /** (701) The disk process received a message
       from a recovery process that requires
       that the disk process be in the UP 
          state; it is currently in the DOWN 
       state. */
   public static final int EDEVICEDOWNFORTRANSACTIONS = 701;
   /** (702) The disk process received an erroneous
       message from a recovery process. The 
          message requested a physical REDO and 
          also requested that audit be generated. */
   public static final int EUNSUPPORTEDOP            = 702;
   /** (703) The disk process encountered a Creation
       Volume Sequence Number in an audit record
          sent by a recovery process that is more
          recent than the CRVSN of the File Label. */
   public static final int EAUDITTOONEW              = 703;
   /** (704) The disk process encountered a Previous
       Volume Sequence Number in an audit record
          sent by a recovery process that does not 
          match the VSN of the data block on disk. */
   public static final int EPVSNMISMATCH             = 704;
   /** (705) Generated by the disk process when it 
       receives a TMF Auditing Disk Process
       request type message and the disk 
       process is not an ADP volume. */
   public static final int ENOTANADP                 = 705;
   /** (706) Generated by the disk process when it 
       receives a request message that is
       inappropriate for an Auditing Disk 
       Process. */
   public static final int EINVALFORADP              = 706;
   /** (707) Generated by the disk process when the
       Disk Process Name Stamp (DPNameTimeStamp) 
          in the message sent by a recovery process 
          does not match the current DPNameTimeStamp
       of the disk. */
   public static final int EINVALIDDPNAMETIMESTAMP   = 707;
   /** (708) The disk process encountered a File Label
       that had its UndoNeeded flag set when a 
          recovery request specified that the 
          UndoNeeded flag must not be set. */
   public static final int EFILEUNDONEEDED           = 708;
   /** (709) The disk process encountered a File 
       Label that had its RedoNeeded flag set 
          when a recovery request specified that the 
          RedoNeeded flag must not be set. */
   public static final int EFILEREDONEEDED           = 709;
   /** (710) No Further work needs to be done to
       process the current portion of the
       current request. */
   public static final int ENULLOP                   = 710;
   /** (711) The disk process received a corrupt
       audit record in a message from a
       recovery process. */
   public static final int EINVALAUDITREC            = 711;
   /** (712) Resource manager already registered. */
   public static final int ERMALREADYREGISTERED      = 712;
   /** (713) Outstanding transactions are running
       for this resource manager. */
   public static final int ERMOUTSTANDINGTRANS       = 713;
   /** (714) Invalid or unsupported protocol 
       option*/
   public static final int EINVALIDPROTOCOL          = 714;
   /** (715) Invalid Transaction Handle. */
   public static final int EINVALIDTXHANDLE          = 715;

   /** (900) First available PATHSEND Error. */
   public static final int ESCFIRSTERROR             = 900;
   /** (900) Invalid server class name. */
   public static final int ESCINVALIDSERVERCLASSNAME = 900;
   /** (901) Invalid Pathmon process name. */
   public static final int ESCINVALIDPATHMONNAME     = 901;
   /** (902) Error with Pathmon
       connection (eg. Open, I/O, etc.). */
   public static final int ESCPATHMONCONNECT          = 902;
   /** (903) Unknown message received from
       Server Monitor. */
   public static final int ESCPATHMONMESSAGE         = 903;
   /** (904) Error with Server Link
       connection (eg. Open, I/O, etc. ). */
   public static final int ESCSERVERLINKCONNECT      = 904;
   /** (905) No Server Available. */
   public static final int ESCNOSERVERAVAILABLE      = 905;
   /** (906) The user called SC_SEND_INFO
       before ever calling SC_SEND_. */
   public static final int ESCNOSENDEVERCALLED       = 906;
   /** (907) The caller is using an extended
       segment id that is out of range. */
   public static final int ESCINVALIDSEGMENTID       = 907;
   /** (908) The caller supplied a reference
       parameter which is an extended
       address, but doesn't have an
       extended segment in use. */
   public static final int ESCNOSEGMENTINUSE         = 908;
   /** (909) The caller set bits in the flags 
       parameter that are reserved and must be 0. */
   public static final int ESCINVALIDFLAGSVALUE      = 909;
   /** (910) A required parameter was not 
       supplied. */
   public static final int ESCMISSINGPARAMETER       = 910;
   /** (911) One of the buffer length parameters
       is invalid. */
   public static final int ESCINVALIDBUFFERLENGTH    = 911;
   /** (912) A reference parameter is out of 
       bounds. */
   public static final int ESCPARAMETERBOUNDSERROR   = 912;
   /** (913) The Server Class is Frozen. */
   public static final int ESCSERVERCLASSFROZEN      = 913;
   /** (914) ServerMonitor does not recognize
       Server Class name. */
   public static final int ESCUNKNOWNSERVERCLASS     = 914;
   /** (915) Send denied because Pathmon is shutting 
       down. */
   public static final int ESCPATHMONSHUTDOWN        = 915;
   /** (916) Send denied by Server Monitor because of
       Server creation failure. */
   public static final int ESCSERVERCREATIONFAILURE  = 916;
   /** (917) The Tmf Transaction mode of the Send does not
       match that of the ServerClass (eg. Requester 
          Send has a TransId and the ServerClass is
       configured with TMF OFF). */
   public static final int ESCSERVERCLASSTMFVIOLATION = 917;
   /** (918) Send operation aborted. See accompanying 
       Guardian error for more information. */
   public static final int ESCOPERATIONABORTED       = 918;
   /** (919) The caller supplied an invalid
       timeout value. */
   public static final int ESCINVALIDTIMEOUTVALUE    = 919;
   /** (920) The caller's PFS segment could not be 
       accessed. */
   public static final int ESCPFSUSEERROR            = 920;
   /** (921) The maximum number of Pathmons allowed
       has been exceeded. */
   public static final int ESCTOOMANYPATHMONS        = 921;
   /** (922) The maximum number of server classes 
       allowed has been exceeded. */
   public static final int ESCTOOMANYSERVERCLASSES   = 922;
   /** (923) The maximum number of allowed server links
       has been exceeded. */
   public static final int ESCTOOMANYSERVERLINKS     = 923;
   /** (924) The maximum number of allowed send 
       requests has been exceeded. */
   public static final int ESCTOOMANYSENDREQUESTS    = 924;
   /** (925) The maximum number of allowed 
       requesters has been exceeded. */
   public static final int ESCTOOMANYREQUESTERS      = 925;
   /** (926) The dialog id is not valid. */
   public static final int ESCDIALOGINVALID          = 926;
   /** (927) The requester has too many dialogs 
       currently open. */
   public static final int ESCTOOMANYDIALOGS         = 927;
   /** (928) The requester already has a send 
       outstanding on this dialog. */
   public static final int ESCOUTSTANDINGSEND        = 928;
   /** (929) The dialog was aborted due to server 
       termination or loss of communication
       between sends. */
   public static final int ESCDIALOGABORTED          = 929;
   /** (930) The requester tried to send using a
       transaction other than the one active
       when the dialog was started. */
   public static final int ESCCHANGEDTRANSID         = 930;
   /** (931) The server ended the dialog. */
   public static final int ESCDIALOGENDED            = 931;
   /** (932) The dialog server is not a DSERVER. */
   public static final int ESCDIALOGNOTDSERVER       = 932;
   /** (933) The server has not ended the dialog. */
   public static final int ESCDIALOGOUTSTANDING      = 933;
   /** (934) The dialog's transaction was aborted. */
   public static final int ESCTRANSACTIONABORTED     = 934;
   /** (947) Error with Link Monitor connection
       (eg. Open, I/O, etc. ). */
   public static final int ESCLINKMONCONNECT         = 947;
   /** (950) Last available PATHSEND error. */
   public static final int ESCLASTERROR              = 950;
   /** (3501) More control information was 
       received than this dialect allows. */
   public static final int ETOOMUCHCONTROLINFO = 3501;
   /** (3502) Server does not have sufficient 
       available storage for this request. */
   public static final int EREQUESTALLOCATIONFAILURE = 3502;
   /** (3503) Server does not support this 
       dialect. */
   public static final int EDIALECTUNSUPPORTED = 3503;
   /** (3504) Server does not support this request 
       type. */
   public static final int EREQUESTUNSUPPORTED = 3504;
   /** (3505) Server version is not current. */
   public static final int ESERVERVERSIONTOOLOW = 3505;
   
   
}  /* end class GError */
