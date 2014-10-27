/*---------------------------------------------------------------------------
 * Copyright 2004
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


#ifndef _ERROR_MODULE
#define _ERROR_MODULE
#ifdef _COMMENTS

@@@@@@@@@@@  @@@@@@@@@@   @@@@@@@@@@    @@@@@@@@@   @@@@@@@@@@
@@@@@@@@@@@  @@@@@@@@@@@  @@@@@@@@@@@  @@@@@@@@@@@  @@@@@@@@@@@
@@@          @@@     @@@  @@@     @@@  @@@     @@@  @@@     @@@
@@@          @@@     @@@  @@@     @@@  @@@     @@@  @@@     @@@
@@@@@@@@@    @@@@@@@@@@@  @@@@@@@@@@@  @@@     @@@  @@@@@@@@@@@
@@@@@@@@@    @@@@@@@@@@   @@@@@@@@@@   @@@     @@@  @@@@@@@@@@
@@@          @@@  @@@     @@@  @@@     @@@     @@@  @@@  @@@
@@@          @@@   @@@    @@@   @@@    @@@     @@@  @@@   @@@
@@@@@@@@@@@  @@@    @@@   @@@    @@@   @@@@@@@@@@@  @@@    @@@
@@@@@@@@@@@  @@@     @@@  @@@     @@@   @@@@@@@@@   @@@     @@@

#endif

#pragma PAGE "Usage Considerations"
#ifdef _COMMENTS

Module:    Error
Contents:  Guardian Error Codes
Status:    Corresponds to Guardian D10
Author:    Jim Lyon (but ripped off from Guardian DERROR)

This module encapsulates the Guardian File Errors, NewProcess Errors,
and Define Errors into the standard interface format.

#endif
#pragma PAGE "Justification"
#ifdef _COMMENTS

A standard way of defining error codes is needed for shared tools.
It is not deemed desirable for them to directly source stuff from
a Guardian subvolume;  therefore, this module is a copy of the
error codes that they may want.

#endif
#pragma PAGE "Known Problems"
#ifdef _COMMENTS

This module may lag behing the latest and greatest Guardian definitions.
It should be reviewed at each release, and brought up to date.

#endif
#pragma PAGE "File Error Codes"

/*
 The following error numbers are defined for use by Tandem-supplied software,
 and are subject to meaningful interpretation by Tandem software, particularly
 by the file system and I/O processes.  A block of error numbers has been
 reserved by Tandem for user-defined errors.  Tandem places no meaning on that
 block of error numbers, and Tandem software will not attempt to interpret
 such error numbers.  The range is defined as FEFIRSTUSER through FELASTUSER.
*/


#define FEOK                0 /* no error */

#define FEEOF               1 /* end-of-file on a disc or tape file read
                                 or end of physical media on a write to a
                                 line printer, terminal, etc. */
#define FEINVALOP           2 /* operation not allowed on this type of
                                  file */
#define FEPARTFAIL          3 /* failure to open or purge a partition */
#define FEKEYFAIL           4 /* failure to open alternate key file */
#define FESEQFAIL           5 /* failure to provide sequential buffering */
#define FESYSMESS           6 /* system message picked up on $receive */
#define FENOSYSMESS         7 /* unable to receive system messages */
#define FEREADLOCKED        9 /* DP2 returned a record that was locked */

#define FEBADERR           10 /* first of the errors that set CCL */
#define FEDUP              10 /* file already exists */
#define FENOTFOUND         11 /* file not in directory */
#define FEINUSE            12 /* file is in use */
#define FEBADNAME          13 /* illegal filename specification */
#define FENOSUCHDEV        14 /* device does not exist */
#define FEWRONGDEV         15 /* volume specification to rename does not
                                 agree with name of volume on which file
                                 resides */
#define FENOTOPEN          16 /* file number has not been opened */
#define FENOPRIMARY        17 /* primary process specified is not alive */
#define FENOSUCHSYS        18 /* no such system is defined */
#define FENOMOREDEVS       19 /* no more devices in LDT */

#define FENETVIOL          20 /* attempt to open network file with long crtpid */
#define FEBADCOUNT         21 /* illegal count specification or attempt
                                 to transfer to much or too little data */
#define FEBOUNDSERR        22 /* parameter or buffer out of bounds */
#define FEBADADDR          23 /* disc address out of bounds */
#define FENOTPRIV          24 /* privileged mode required for this
                                 operation */
#define FEWAITFILE         25 /* AWAITIO attempted on waited file */
#define FENONEOUT          26 /* AWAITIO attempted on file with no out-
                                 standing requests */
#define FEIOPEND           27 /* waited operation attempted when there
                                 were outstanding no-wait requests */
#define FETOOMANY          28 /* # of outstanding I/O's would exceed
                                 depth specification */
#define FEMISSPARM         29 /* missing parameter */

#define FENOLCB            30 /* unable to obtain a link control block */
#define FENOBUFSPACE       31 /* unable to obtain file system buffer space */
#define FENOPOOLSPACE      32 /* unable to obtain storage pool space (SYSPOOL) */
#define FENOIOBUFSPACE     33 /* unable to obtain I/O segment space */
#define FENOCBSPACE        34 /* unable to obtain file system control block */
#define FENOIOCBSPACE      35 /* unable to obtain I/O process control block */
#define FENOPHYSMEM        36 /* unable to obtain physical memory */
#define FENOIOPHYSMEM      37 /* unable to obtain physical memory for I/O */
#define FEWRONGTNS         38 /* unable to run on this system (TNS or TNS/II only) */
#define FEOLDSYNC          39 /* ancient syncid encountered - process file */

#define FETIMEDOUT         40 /* operation timed out before completion */
#define FEBADSYNC          41 /* checksum error on file sync block */
#define FEUNALLOC          42 /* attempt to read from unallocated page */
#define FENODISCSPACE      43 /* unable to obtain disc space for file page */
#define FEDIRFULL          44 /* directory is full */
#define FEFILEFULL         45 /* file is full */
#define FEINVKEY           46 /* invalid key specified */
#define FEKEYSBAD          47 /* keys not consistent with file data */
#define FESECVIOL          48 /* security violation */
#define FEACCVIOL          49 /* access violation */

#define FEDIRERR           50 /* directory error (directory is bad) */
#define FEDIRBAD           51 /* directory is messed up */
#define FEDFSERR           52 /* error in disc free space table */
#define FEFSERR            53 /* file system internal error */
#define FEDFSIO            54 /* i/o error in disc free space table or
                                 in DP2 undo area */
#define FEDIRIO            55 /* i/o error in directory */
#define FEVLABIO           56 /* i/o error on volume label */
#define FEDFSFULL          57 /* disc free space table is full */
#define FEDFSBAD           58 /* disc free space table is messed up */
#define FEFILEBAD          59 /* file is messed up */

#define FEWRONGID          60 /* volume mounted is not the one on which
                                 this file resides (volume changed) or
                                 device downed and then upped (old
                                 request) */
#define FEOPENSTOP         61 /* no more opens allowed on this volume */
#define FEUNEXMOUNT        62 /* volume was mounted, but no mount command
                                 was given */
#define FEMOUNTEXP         63 /* waiting for mount interrupt */
#define FEMOUNTINPROG      64 /* mount is in progress on this device */
#define FESPECONLY         65 /* only special requests allowed */
#define FEDEVDOWN          66 /* device is down */

#define FECONTINUE         70 /* continue file operation */
#define FEDUPREC           71 /* duplicate record */
#define FEBADPART          72 /* attempt to access unmounted partition */
#define FELOCKED           73 /* file or record locked */
#define FEBADREPLY         74 /* illegal reply number or no space for reply */
#define FENOTRANSID        75 /* trans mode operation with no transid */
#define FEENDEDTRANSID     76 /* transid in process of ending */

#define FEINVTRANSID       78 /* invalid transid */
#define FENOTRANSLOCK      79 /* transid does not hold lock on record */
#define FEAUDITINVALOP     80 /* invalid op on audited file/non-audited volume */
#define FETRANSNOWAITOUT   81 /* > 0 nowait I/O's for transid outstanding */
#define FETMFNOTRUNNING    82 /* all or part of the TMF subsystem not running */
#define FETOOMANYTRANSBEGINS 83 /* > 1 or > TFILE depth transbegins */
#define FETMFNOTCONFIGURED 84 /* TMF not configured */
#define FEDEVICEDOWNFORTMF 85 /* the device is not up for TMF */
#define FEBEGINTRDISABLED  86 /* BEGINTRANSACTION is disabled */
#define FEEXPECTEDREAD     87 /* waiting on a read request and did not get it */
#define FEINVALIDSEQUENCE  88 /* a control read is pending, new read invalid */
#define FENODEVBUFSPACE    89 /* remote device has no buffer available */
#define FETRANSABRTOWNERDIED 90 /* begintrans process died: system abort */
#define FETRANSABRTBADDBID 91 /* trans aborted by system: dbid's disagree */
#define FETRANSABRTNETDOWN 92 /* trans aborted by system: unreachable node */
#define FETRANSABRTAUDOVFL 93 /* trans aborted by system: spanned too many
                                                          audit files */
#define FETRANSABRTOPRCMD  94 /* trans abrted by system: operator command */
#define FETRANSABRTDISCTKOVR 95 /* trans aborted due to DP2 takeover */
                                /* 96 reserved for sysabort */
#define FEABORTEDTRANSID   97 /* transid was aborted */
#define FENOMORETCBS       98 /* unable to allocate a TCB/NETTCB */

#define FETOOCHEAP         99 /* failure to purchase Enscribe */

#define FENOTRDY          100 /* device not ready */
#define FENOWRTRING       101 /* no write ring in mag tape */
#define FEPAPEROUT        102 /* paper out, bail open or end of ribbon */
#define FEPONNOTRDY       103 /* disc not ready due to power fail */
#define FENORESPONSE      104 /* device not responding */
#define FEVFUERR          105 /* printer VFU error */
#define FEPRIORWRITEFAIL  106 /* a previously-buffered write has failed */

#define FEBREAKONLY       110 /* only break mode requests accepted */
#define FEBREAK           111 /* break occurred on this request */
#define FEOPWRITEABORT    112 /* read or writeread preempted by operator message */
#define FEOPTOOMANY       112 /* too many user console messages to operator consol */
#define FEBADDEFUSE       113 /* invalid DEFINE passed to the tape process */
#define FEL3T20TO         114 /* X25 restart timeout or retry exceeded */
#define FEL3T22TO         115 /* X25 reset timeout or retry exceeded */
#define FEL3T23TO         116 /* X25 clear timeout or retry exceeded */
#define FEERRTOOBIG       119 /* error code > 255 but placed into 8-bit container */
#define FEDATAPARITY      120 /* data parity */
#define FEOVERRUN         121 /* data overrun */
#define FEDATALOSS        122 /* possible data loss due to X.25 circuit reset
                                 or DP2 takeover on buffered, syncdepth 0 file */
#define FESUBBUSY         123 /* sub-device busy */
#define FELINERESET       124 /* a line reset is in progress */

#define FEFIRSTDISC       130 /* 1st disc error */
#define FEADDRCHK         130 /* illegal address to disc drive */
#define FEWRTCHECK        131 /* write check error from disc */
#define FESEEKUNC         132 /* seek incomplete from disc */
#define FEACCNOTRDY       133 /* access not ready on disc */
#define FEADDRCOMP        134 /* address compare error on disc */
#define FEWRTPROTECT      135 /* attempt to write to protected disc */
#define FEUNITOWN         136 /* unit ownership error ( dual port discs) */
#define FEBUFPARITY       137 /* disc controller buffer parity error */
#define FEINTERRUPTOVERRUN 138 /* long error name */
#define FECTLRERR         139 /* controller error */
#define FELASTDISC        139 /* last disc error */

#define FEMODEMERR        140 /* modem disconnected or screwed up */
#define FEDISCONNECT      140 /* link disconnected */
#define FEMOTIONCHK       145 /* card reader motion check error */
#define FEREADCHK         146 /* card reader read check error */
#define FEINVALHOLRTH     147 /* card reader invalid Hollerith code read */
#define FEWORMSEQ         148 /* read unwritten sectors or write already
                                 written sectors on optical disk. */

#define FEEOT             150 /* end of tape */
#define FERUNAWAY         151 /* runaway tape */
#define FEOPABORT         152 /* unusual end (unit went offline) */
#define FETAPEPON         153 /* tape drive power on */
#define FEBOT             154 /* beginning of tape on BSR or BSF */
#define FE9TRKTAPEONLY    155 /* only nine-track magnetic tape permitted */
#define FEIOPROTOCOLERR   156 /* I/O protocol violation detected */
#define FEIOPROCERR       157 /* I/O process internal error */
#define FEBADTHLFUNC      158 /* unknown function for THL */
#define FEWRONGDEVMODE    159 /* device mode wrong for request */
/*
  Data communications error numbers:
         160 through 189
*/
#define FEINVALIDSTATE    160 /* invalid state/ protocol error */
#define FEBADSTATE        161 /* impossible state */
#define FERETRIESEXHAUSTED  162 /* retry count exhausted */
#define FEOPERATIONTIMEDOUT FERETRIESEXHAUSTED /* operation timed out */
#define FERECEIVEDEOT     163 /* eot was received */
#define FERECEIVEDDISC    164 /* disconnect received */
#define FERECEIVEDRVI     165 /* rvi was received */
#define FERECEIVEDENQ     166 /* enq was received */
#define FERECEIVEDEOTBID  167 /* eot received on line bid */
#define FERECEIVEDNAKBID  168 /* nak received on line bid */
#define FERECEIVEDWACKBID 169 /* wack received on line bid */
#define FENOSEQUENCEID    170 /* no id sequence received */
#define FENOTREPLYING     171 /* no reply received */
#define FEIMPROPERREPLY   172 /* reply not proper for protocol */
#define FEMAXNAKS         173 /* too many naks from a 6520/6524 terminal */
#define FERECEIVEDWACK    174 /* wack or busy was received */
#define FEWRONGACK        175 /* ack out of sequence */
#define FEPOLLEND         176 /* poll sequence ended */
#define FETEXTOVERRUN     177 /* text received exceeds buffer size */
#define FEADDRERROR       178 /* illegal operation for address list
                                 or address missing */
#define FEBADFORMAT       179 /* application buffer is incorrect */
#define FERECEIVEDSTATUS  180 /* received status */
#define FERECEIVEDDUPSTATUS 181 /* received duplicate status */

#define FENODATA          187 /* no data available */
#define FEDATALOST        188 /* data and/or context lost */
#define FEDATACOMING      189 /* further reply is forthcoming */
#define FEDEVERR          190 /* undefined device error */
#define FEDEVPON          191 /* device power on */
#define FEDEVXERCISED     192 /* device is being exercised */
#define FENOMICROCODE     193 /* invalid or missing microcode file */
#define FEOPRREJECT       194 /* operator rejects the open request */
#define FENOZSVR          195 /* $zsvr went lunch */
#define FEBADTAPELABEL    196 /* bad label field on a labeled tape */
#define FESQLERR          197 /* A NonStop SQL related error occurred */

#define FEMISSDEF         198 /* missing DEFINE */
#define FEOBIPROTECTED    199 /* disc file is OBI protected */
#define FECATERR          200 /* first of the path switch errors */
#define FEOWNERSHIP       200 /* device is owned by the other port */
#define FEPATHDOWN        201 /* unable to communicate over this path */

#define FEOWNABORT        210 /* ownership changed during operation */
#define FECPUFAIL         211 /* cpu failed during operation */
#define FECHPARITY        213 /* channel data parity error */
#define FECHTIMEOUT       214 /* channel timeout */
#define FEMEMABS          215 /* I/O attempted to absent memory page */
#define FEMEMBRKPT        216 /* memory breakpoint encountered during this I/O */
#define FEMEMPARITY       217 /* memory parity error during this I/O */
#define FEINTTIMEOUT      218 /* interrupt timeout */
#define FEILLRECON        219 /* illegal device reconnect */
#define FEPROTECT         220 /* protect violation */
#define FEHANDSHAKE       221 /* controller handshake violation */
#define FEBADEIOSTAT      222 /* bad channel status from EIO instruction */
#define FEBADIIOSTAT      223 /* bad channel status from IIO instruction */
#define FECTLERR          224 /* controller error */
#define FEUNITPOLLERR     225 /* no unit or multiple units at same unit # */

#define FECPUPON          230 /* cpu power on during operation */
#define FEIOPON           231 /* controller power on during operation */
#define FESMONERR         232 /* access denied, error in communication with SMON */
#define FESCERROR         233 /* SERVERCLASS_SEND_ error */

#define FENETERR          240 /* first of the network errors */
#define FENETERRLOW       240 /* first of the network errors */
#define FELHRETRY         240 /* line handler error, didn't get started */
#define FENETRETRY        241 /* network error, didn't get started */
#define FEBYPASSFAIL      246 /* FOX direct route failed, aborted req */
#define FELHFAIL          248 /* line handler error, aborted */
#define FENETFAIL         249 /* network error, aborted */
#define FENETDOWN         250 /* all paths to the system are down */
#define FENETPROTOCOLERROR  251 /* network protocol error */
#define FENOEXPANDCLASS   252 /* required expand class not available */
#define FELHTOOMANYUPS    255 /* linehandler belched too often */
#define FENETERRHIGH      255 /* last of the network errors */

#define FEFIRSTUSER       300 /* errors 300 to 511 are reserved for */
#define FELASTUSER        511 /* use by the user. */

#define FENOBLOCKMODE     549 /* terminal cannot be put into blockmode now */
#define FEILLPOS          550 /* illegal positioning for bulk transfer */
#define FEDUPALTKEY       551 /* duplicate insertion-ordered alt key warning */
#define FEFATPIN          560 /* The calling or subject process has a pin > 255. */
#define FEBADITEM         561 /* The item code in a list was not recognized. */
#define FEBUFTOOSMALL     563 /* The size of an output buffer was too small 
                                 to fit data into. */
#define FEBADOP           564 /* Operation not supported on this file type
                                 (this is a "bad" error vs. feinvalop). */
#define FEILLEGALREQUEST  565 /* message was inappropriate or badly formed */
#define FEILLEGALREPLY    566 /* response was inappropriate or badly formed */
#define FETOSWRONGFORDEFINE 567 /* Target system's Tos too low for define class */
#define FESEQORDER        570 /* the fs-iop sequence numbers indicate that the
                                 message was received out of order. */
#define FESEQDUP          571 /* the fs-iop sequence numbers indicate that the
                                 message received was a duplicate. */
#define FESEQRESET        572 /* the fs-iop sequence numbers have been reset so
                                 the received sequence number cannot be accepted. */
#define FELDEVPHANDLE     573 /* A user asked for a phandle only available as
                                 a type 5 (so we give an error and a null phandle */
#define FEINVALIDKEEPMSG  586 /* invalid request for keeping transaction */
#define FETOOMANYKEEPS    587 /* too many transactions being kept */
#define FENOKEPTTRANS     588 /* no transaction was kept */
#define FEOUTSTANDINGKEEP 589 /* invalid reply for kept transaction */
#define FEBADPARMVALUE    590 /* parameter value invalid or inconsisent */
#define FEREQABANDONED    593 /* request has been cancelled */
#define FEMISSITEM        597 /* missing required item in item list */
#define FESHORTSTACK      632 /* not enough stack space available */
#define FE_MEASURE_RUNNING  633 /* Measure is active, which prevents the operation */
#define FEFATLDEV         634 /* The specified ldev is greater than 16 bits */
#define FEUNITLOCK        635 /* odp was unable to lock a volume in a drive
                                 because one of the available drive was already
                                 locked. */
#define FEUSEOLDWAY       637 /* used by DP2 in SETCLOSE */
#define FEQUEUEDSTOPMODE1 638 /* The target process is (temp?) unstoppable,
                                 so, the stop was queued until the process
                                 becomes stoppable. */
#define FEQUEUEDSTOPMODE0 639 /* The requesting process does not have permission
                                 to stop the target process, so the stop was
                                 queued until the process calls SETSTOP( 0 ). */

#define FEWRONGRECOVSEQNUM          700 /*  Generated by the DP if the message
                                            sequence number does not match
                                            expected sequence number */
#define FEDEVICEDOWNFORTRANSACTIONS 701 /*  Generated by the DP when the device
                                            is down but is supposed to be up */
#define FEUNSUPPORTEDOP             702 /*  Generated by the DP when it receives
                                            a physical REDO req. and the request
                                            specifies that audit be generated */
#define FEAUDITTOONEW               703 /*  Generated by the DP during recovery
                                            if the CRVSN in the audit trail is
                                            is more recent than the FLAB CRVSN */
#define FEPVSNMISMATCH              704 /*  Generated by the DP when the PVSN
                                            in the audit trail record does not
                                            match the VSN of the data block on
                                            disk */
#define FENOTANADP                  705 /*  Generated by the DP when it receives
                                            an ADP request and it is not an ADP
                                            volume */
#define FEINVALFORADP               706 /*  Generated when the request to the
                                            Auditing Disk Process is invalid. */
#define FEINVALIDDPNAMETIMESTAMP    707 /*  Generated by the DP when the DpName
                                            Time Stamp sent by the recovery
                                            process does not match the current
                                            DpName Time Stamp */
#define FEFILEUNDONEEDED            708 /*  Returned by the DP when a file was
                                            encountered in the Redo/Undo request
                                            that had its UndoNeeded flag set
                                            and the request specified that the
                                            UndoNeeded flag must not be set */
#define FEFILEREDONEEDED            709 /*  Returned by the DP when a file was
                                            encountered in the Redo/Undo request
                                            that had its RedoNeeded flag set
                                            and the request specified that the
                                            RedoNeeded flag must not be set */
#define FENULLOP                    710 /*  No Further work needs to be done to
                                            process the current portion of the
                                            current request */

#define FESCFIRSTERROR              900 /*  First available PATHSEND Error. */
#define FESCINVALIDSERVERCLASSNAME  900 /*  Invalid server class name. */
#define FESCINVALIDPATHMONNAME      901 /*  Invalid Pathmon process name. */
#define FESCPATHMONCONNECT          902 /*  Error with Pathmon
                                            connection (eg. Open, I/O,
                                            etc.). */
#define FESCPATHMONMESSAGE          903 /*  Unknown message received from
                                            Server Monitor. */
#define FESCSERVERLINKCONNECT       904 /*  Error with Server Link
                                            connection
                                            (eg. Open, I/O, etc. ). */
#define FESCNOSERVERAVAILABLE       905 /*  No Server Available. */
#define FESCNOSENDEVERCALLED        906 /*  the user called SC_SEND_INFO
                                            before ever calling SC_SEND_. */
#define FESCINVALIDSEGMENTID        907 /*  The caller is using an extended
                                            segment id that is out of range. */
#define FESCNOSEGMENTINUSE          908 /*  The caller supplied a reference
                                            parameter which is an extended
                                            address, but doesn't have an
                                            extended segment in use. */
#define FESCINVALIDFLAGSVALUE       909 /*  The caller set bits in the flags
                                            parameter that are reserved and
                                            must be 0. */
#define FESCMISSINGPARAMETER        910 /*  A required parameter was not
                                            supplied. */
#define FESCINVALIDBUFFERLENGTH     911 /*  One of the buffer length parameters
                                            is invalid. */
#define FESCPARAMETERBOUNDSERROR    912 /*  A reference parameter is out of
                                            bounds. */
#define FESCSERVERCLASSFROZEN       913 /*  The Server Class is Frozen. */
#define FESCUNKNOWNSERVERCLASS      914 /*  ServerMonitor does not recognize
                                            Server Class name. */
#define FESCPATHMONSHUTDOWN         915 /*  Send denied because Pathmon
                                            is shutting down. */
#define FESCSERVERCREATIONFAILURE   916 /*  Send denied by Server Monitor
                                            because of Server creation
                                            failure. */
#define FESCSERVERCLASSTMFVIOLATION 917 /*  The Tmf Transaction mode of
                                            the Send does not match that
                                            of the ServerClass (eg.
                                            Requester Send has a TransId
                                            and the ServerClass is
                                            configured with TMF OFF). */
#define FESCOPERATIONABORTED        918 /*  Send operation aborted. See
                                            accompanying Guardian error
                                            for more information. */
#define FESCINVALIDTIMEOUTVALUE     919 /*  The caller supplied an invalid
                                            timeout value. */
#define FESCPFSUSEERROR             920 /*  The caller's PFS segment could
                                            not be accessed. */
#define FESCTOOMANYPATHMONS         921 /*  The maximum number of Pathmons
                                            allowed has been exceeded. */
#define FESCTOOMANYSERVERCLASSES    922 /*  The maximum number of server
                                            classes allowed has been exceeded. */
#define FESCTOOMANYSERVERLINKS      923 /*  The maximum number of allowed
                                            server links has been exceeded. */
#define FESCTOOMANYSENDREQUESTS     924 /*  The maximum number of allowed send
                                            requests has been exceeded. */
#define FESCTOOMANYREQUESTERS       925 /*  The maximum number of allowed
                                            requesters has been exceeded. */
#define FESCDIALOGINVALID           926 /*  The dialog id is not valid. */
#define FESCTOOMANYDIALOGS          927 /*  The requester has too many dialogs
                                            currently open. */
#define FESCOUTSTANDINGSEND         928 /*  The requester already has a send
                                            outstanding on this dialog. */
#define FESCDIALOGABORTED           929 /*  The dialog was aborted due to server
                                            termination or loss of communication
                                            between sends. */
#define FESCCHANGEDTRANSID          930 /*  The requester tried to send using a
                                            transaction other than the one active
                                            when the dialog was started. */
#define FESCDIALOGENDED             931 /*  The server ended the dialog. */
#define FESCDIALOGNOTDSERVER        932 /*  The dialog server is not a DSERVER. */
#define FESCDIALOGOUTSTANDING       933 /*  The server has not ended the dialog. */
#define FESCTRANSACTIONABORTED      934 /*  The dialog's transaction was aborted. */
#define FESCTOOMANYTESTPOINTUSERS   946 /*  The maximum number of allowed
                                            test point users has been exceeded. */
#define FESCLINKMONCONNECT          947 /*  Error with Link Monitor connection
                                            (eg. Open, I/O, etc. ). */
#define FESCRETRYOPEN               949 /*  This is an internal error ONLY. */
#define FESCLASTERROR               950 /*  Last available PATHSEND error. */

#define FETOOMUCHCONTROLINFO 3501 /*   Indicates that control information
                                       exceeded maximum for the specified dialect. */
#define FEREQUESTALLOCATIONFAILURE 3502 /*  Indicates that storage was not
                                       available at the server to process the
                                       request because of the demands of other
                                       activities at the time the request was
                                       received. */
#define FEDIALECTUNSUPPORTED 3503 /*   Indicates that the addressee does not
                                       support the dialect of a received request. */
#define FEREQUESTUNSUPPORTED 3504 /*   Indicates that the addressee (server)
                                       supports the dialect but not the request
                                       type of the request. */
#define FESERVERVERSIONTOOLOW 3505 /*  Indicates that the server determined t
                                       the version recorded in
                                       MINIMUM_INTERPRETATION_VERSION was higher
                                       than its own native version. */
#pragma PAGE "NEWPROCESS Error Numbers"
/*
 When a NEWPROCESS request fails, it returns a word describing the
 error.  The primary error code is in bits <0:7>; these codes are
 described below.  If the primary error is caused by a file system
 error, then the file system error is placed in bits <8:15>.
*/

#define NPERR_OK            0 /* No error.  NEWPROCESS successful */
#define NPERR_UNDEFEXT      1 /* Undefined external references. */
#define NPERR_NOPCB         2 /* No free PCB available */

#define NPERR_FSERR         3 /* File system error. See bits <8:15> */
#define NPERR_NOMAP         4 /* Unable to allocate a map */
#define NPERR_NOSWAP        5 /* Unable to allocate a swap file */
#define NPERR_BADFILE       6 /* Illegal object file format
                                 Error subcodes for NPERR_BADFILE */
#define BADFILE_NODETAIL    0 /* Could not determine exact cause of badfile */
#define BADFILE_NOTDISC     1 /* Not a disc file */
#define BADFILE_NOT100      2 /* Not file code 100 */
#define BADFILE_FILSYS      3 /* Not correct file structure */
#define BADFILE_TOSVERSION  4 /* Requires later version of Guardian */
#define BADFILE_NOMAIN      5 /* No main procedure */
#define BADFILE_LIBHASMAIN  6 /* LIB file has main procedure */
#define BADFILE_NODATAPAGES 7 /* No data pages */
#define BADFILE_PEPINVALID  8 /* Invalid PEP */
#define BADFILE_INITSEGS    9 /* Header fields INITSEGS not consistent with size */
#define BADFILE_RESIDENTSIZE 10 /* Resident size greater than code area length */
#define BADFILE_NOFIXUPS     11 /* File not fixed-up by binder */
#define BADFILE_UNDEFBLOCKS  12 /* File has undefined data blocks */
#define BADFILE_DATACODEREF  13 /* Unresolved code block references in data blocks */
#define BADFILE_MANYSPACES   14 /* too many code spaces in object file */

#define BADFILE_RISCCODESIZE   15 /* bad risc code length in object file */
#define BADFILE_RISCCODEADDR   16 /* bad risc code address in object file */
#define BADFILE_RISCDATASIZE   17 /* bad risc data length in object file */
#define BADFILE_RISCDATAADDR   18 /* bad risc data address in object file */
#define BADFILE_MANYRISCSPACES 19 /* too many risc code spaces in object file */
#define BADFILE_RISCRESSIZE    20 /* bad risc resident area(s) */
#define BADFILE_BADOCTHDR      21 /* bad OCT header in code file */
#define BADFILE_WRONGVADDR     22 /* OCTed to the wrong virtual address */
#define BADFILE_BADFIXUPENTRY  23 /* Entry in fixup list had bad xepindex value
                                     or bad code address value */
#define BADFILE_BADEPILORIPIL  24 /* Incorrect format of EPIL or IPIL or XEP */
#define BADFILE_BADOCTOPTIONS  25 /* Used bad UC/UL/SC/SL option on this program */
#define BADFILE_BADOCTVERSION  26 /* Incompatible OCT header version */
#define BADFILE_BADOCTGW       27 /* Bad callable GW table */
#define BADFILE_BADTARGET      28 /* Targetted to wrong cpu type */
#define BADFILE_NOTOCTED       29 /* Entered OCT fixup, but file was not octed */

#define NPERR_NOTLICENSED   7 /* Unlicensed privileged file */
#define NPERR_BADNAME       8 /* Process name error. See bits <8:15> */
#define NPERR_BADLIBRARY    9 /* Library conflict */
#define NPERR_NOMONITOR     10 /* Unable to communicate with Monitor process */
#define NPERR_LIBFSERR      11 /* File system error on library file (see <8:15>) */
#define NPERR_PROGEQLIB     12 /* Program file and library file are same. */
#define NPERR_BADEXTSEG     13 /* Unable to allocate external data segment */
#define NPERR_NOEXTSWAP     14 /* Unable to allocate external seg swapfile */
#define NPERR_BADHOMETERM   15 /* Illegal or non-existent hometerm */
#define NPERR_TERMIO        16 /* I/O error to home terminal */
#define NPERR_CONTEXTERR    17 /* Error related to DEFINEs/context */
#define NPERR_ILLSUBTYPE    18 /* process device subtype illegal */
#define NPERR_NEQSUBTYPE    19 /* process device subtype not same as existing bro. */
#define NPERR_BADPIMFILE    20 /* PIM file error */
#define NPERR_DYNAMICIOPERR 21 /* error in creation of dynamic IOP */
#define NPERR_BADPFSSIZE    22 /* the PFS size was out of range */
#define NPERR_NOPFS         23 /* unable to allocate a PFS. */
#define NPERR_UNKNOWNERR    24 /* got an error that this sw doesn't know */

#pragma PAGE "Define Error Codes"
/*
 These errors are typically returned from DEFINE procs but may be returned by
 other procs;
 a range of 2049-2099 is reserved for DEFINE errors
*/
#define DEOK                   0 /* success */
#define DESYNTAX            2049 /* syntax error in define name */
#define DEALREADYEXISTS     2050 /* define already exists */
#define DEMISSDEF           2051 /* define non-existent */
#define DENOBUFSPACE        2052 /* no file system buffer space */
#define DENOPHYSMEM         2053 /* no physical memory */
#define DEBOUNDS            2054 /* bounds error on parmeter */
#define DEILLATTR           2055 /* illegal attribute */
#define DEMISSATTR          2056 /* attribute not present */
#define DEWORKINCOMP        2057 /* workingset is incomplete */
#define DEWORKINCONSIST     2058 /* workingset is inconsistent */
#define DEWORKINVALID       2059 /* workingset is invalid */
#define DENOMOREDEF         2060 /* no more defines */
#define DENOMOREATTR        2061 /* no more attributes */
#define DEILLATTRNAME       2062 /* illegal attribute name */
#define DEILLDEFLTNAME      2063 /* illegal default name */
#define DEREQDATTR          2064 /* this is a required attribute */
#define DENODEFAULT         2065 /* no default value for this attribute */
#define DEMISSPARM          2066 /* parameter to proc is missing */
#define DEILLVALUE          2067 /* illegal value for this attribute */
#define DENOSUCHCLASS       2068 /* return from DEFINECALLHELPER */
#define DEINSUFFDEFMODE     2069 /* define type isn't allowed by defmode */
#define DECLASSNOTSET       2070 /* temporary for B30 to hide MAP Defines */
#define DECANNOTCONVERT     2071 /* cannot convert define name to network */
                                 /* form (because of 7 char device name) */
#define DEILLOPERATION      2072 /* illegal helper operation */
#define DEILLREPLACE        2073 /* illegal replace operation */
#define DECANTDELETE        2074 /* deletion of this define not allowed */
#define DEBADPARM           2075 /* invalid input option */
#define DESMALLBUF          2076 /* buffer too small for saved define */
#define DEBADSEGID          2077 /* address of buffer in invalid segment */
#define DENOTSAVED          2078 /* not a saved define */
#define DENOTDEFAULT        2079 /* attempt to save working set but: */
                                 /* name is =_DEFAULTS and working set is */
                                 /*    not CLASS DEFAULTS */
#define  DETOOMANYSTACK     2080 /* Too many working sets stacked */
#define  DEFORMAT           2081 /* Bad internal format (at newprocess) */
#endif
