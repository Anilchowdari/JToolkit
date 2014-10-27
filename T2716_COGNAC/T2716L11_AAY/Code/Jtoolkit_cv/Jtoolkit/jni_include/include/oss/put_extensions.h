#ifdef __TANDEM
#pragma columns 90
#if _TANDEM_ARCH_ != 0
#pragma ONCE
#endif
#endif
#ifndef _PUT_EXTENSIONS
#define _PUT_EXTENSIONS

/* T8645Y21 - (05FEB2010) - put/put_extensions.h */

/*
 *  Copyright 2010 Hewlett Packard Development Company, L.P.
 *
 *     ALL RIGHTS RESERVED
 */

/*
 * HISTORY
 * $Log: put_extensions.h,v $
 *
 * 2010/09/28
 * Fix Soln: 10-100526-0628. OSS - pthreads  SPT_xxx wrapper functions.
 * Changes: Changed the byte count parameters (read_count, write_count & *count_read)
 * from short to unsigned short in the thread aware wrappers to match the current 
 * definitions of the NSK I/O functions.
 *
 * $EndLog$
 */


#ifdef __cplusplus
   extern "C" {
#endif


#ifndef _GUARDIAN_HOST
#include <sys/time.h>
#else
#include <systimh>  nolist
#endif
#include <signal.h> nolist

#define PTHREAD_STACK_MAX_NP                   (32 * 1024 * 1024) /* 32MB */
#define PTHREAD_STACK_MAX_PROTECTED_STACK      (16 * 1024 * 1024) /* 16MB */
#define PTHREAD_STACK_MIN_PROTECTED_STACK      (48 * 1024) /* 48KB */

enum put_error {
        PUT_SUCCESS, PUT_ERROR, PUT_INTERRUPTED, PUT_TIMEDOUT

};

typedef enum put_error put_error_t;

/* Define nonportable extensions */

#define pthread_getunique_np(handle) \
((unsigned int)((pthread_t *)handle)->field2)

/*
 * Functions to set and get the register stack percentage for non main threads
 */
extern int pthread_attr_get_rsestackpercent_np( const pthread_attr_t*,
                                                short *rsepercent );

extern int pthread_attr_set_rsestackpercent_np( pthread_attr_t*,
                                                short reg_percent);

extern int pthread_cond_signal_int_np(pthread_cond_t *);

/* pthread_cond_timedwait_np - perform a timed wait on a condition variable.  */

extern int pthread_cond_timedwait_np( pthread_cond_t *, pthread_mutex_t *,
                                      long long);

extern put_error_t pthread_default_set_rsestackpercent_np(short reg_percent);

extern int pthread_delay_np(const struct timespec *);

extern int pthread_get_expiration_np(struct timespec *, struct timespec *);

extern int pthread_getjmpbuf_np(pthread_t   thread, long long *jmpbuf);

extern int pthread_getjmpbuf_main_np(long long *jmpbuf);

/*
 * pthread_stack_info_np is the structure used to store the memory used,
 * register used,memory base,register base,memory size and register
 * size for a thread.
 */
typedef struct PTHREAD_STACK_INFO_NP {
        int stk_flags;

        /* Size of the stack and register stack */
        unsigned int /*size_t*/  stk_stacksize;
        unsigned int /*size_t*/  stk_rsesize;

        /* Amount of stack and register stack used */
        unsigned int /*size_t*/  stk_stackused;
        unsigned int /*size_t*/  stk_rseused;

        /* Address of the base of the stack */
        void    *stk_stack_base;

        /* Base of RSE BS.  Filled in only for IA64. */
        void    *stk_rse_base;

        /* Reserved for future use */
        int     stk_reserved[25];
    } pthread_stack_info_np;

/*
 *  Function to populate pthread_stack_info_np structure
 */
extern short pthread_get_stackinfo_np(pthread_t *tid,pthread_stack_info_np *stack);

/* Start of addition solution#10-070402-3720 */
/* pthread_get_threadstateinfo_np - get the thread state info */
extern int pthread_get_threadstateinfo_np(pthread_t *, char *);
/* End of addition for solution#10-070402-3720 */

extern int pthread_getattr_np(const pthread_t, pthread_attr_t **);

/* pthread_kill_np - send a signal to a thread from a signal handler*/
extern int pthread_kill_np(pthread_t, int);

extern int pthread_lock_global_np(void);

extern int pthread_signal_to_cancel_np(sigset_t *,pthread_t *);

extern int pthread_unlock_global_np(void);

/*
 * DESCRIPTION
 *
 *   Callback type required by put_regFileIOHandler().
 *
 * PARAMETERS
 *
 *   filenum           - Guardian file number whose IO has completed.
 *   tag               - tag of completed IO.
 *   count_transferred - transfer count of completed IO.
 *   error             - Guardian error number for completed IO.
 *   userdata          - address of user data area.  Set when user called
 *                       put_awaitio().
 *
 * RETURN VALUES
 *
 *   None.
 */
typedef void (*put_FileIOHandler_p)(const short filenum, const long
        tag, const long count_transferred, const long error, void *userdata);

/*
 * DESCRIPTION
 *
 *   Callback type required by put_regOSSFileIOHandler().
 *
 * PARAMETERS
 *
 *   filedes           - OSS file descriptor whose IO has completed.
 *   read              - filedes is read ready.
 *   write             - filedes is write ready.
 *   error             - filedes has an exception pending.
 *
 * RETURN VALUES
 *
 *   None.
 */
typedef void (*put_OSSFileIOHandler_p)(const int filedes,
        const int read_set, const int write_set, const int error_set);

/*
 * DESCRIPTION
 *
 *   Callback type required by put_regTimerHandler().
 *
 * PARAMETERS
 *
 *   None.
 *
 * RETURN VALUES
 *
 *   0  - Callback has readied a thread to run.  Callback will be invoked
 *        again as soon as possible.
 *   -1 - Callback has not readied a thread.  Callback will be invoked again
 *        as soon as possible.
 *   >0 - Callback has not readied a thread.  Return value is the hundreths
 *        of a second until callback should be invoked again.
 */
typedef long (*put_TimerHandler_p)(void);

extern int put_select(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *errorfds, struct timeval *timeout);

extern int put_select_single_np(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *errorfds, struct timeval *timeout);

extern int put_fd_read_ready(const int fd, struct timeval *timeout);

extern int put_fd_write_ready(const int fd, struct timeval *timeout);

extern put_error_t put_regOSSFileIOHandler(const int filedes,
        const put_OSSFileIOHandler_p functionPtr);

extern put_error_t put_unregOSSFileIOHandler(const int filedes);

extern put_error_t put_setOSSFileIOHandler(const int filedes,
        const int set_read, const int set_write, const int set_error);

extern put_error_t put_awaitio(const short filenum, const long tag,
        const long timelimit, int *count_transferred, int /*long*/ *error,
        void *userdata);

extern put_error_t put_interrupt(const short filenum, const put_error_t
        errorPUT);

extern put_error_t put_interruptTag(const short filenum, const long
        tag, const put_error_t errorPUT);

extern put_error_t put_regFile(const short filenum);

extern put_error_t put_regFileIOHandler(const short filenum,
        const put_FileIOHandler_p functionPtr);

extern put_error_t put_unregFile(const short filenum);

extern put_error_t put_postfork_unregFile(const short filenum);

extern put_error_t put_wakeup(const short filenum, const long tag,
        const long count_transferred, const long error);

extern long put_INITRECEIVE(const short filenum, const short receive_depth);

/* Changes begin for TS/MP Version greater than 2.3 */

extern long put_INITRECEIVEL(const short filenum, const short receive_depth);

/* Changes end for TS/MP Version greater than 2.3 */

extern long put_RECEIVEREAD(const short filenum, char *buffer,
        const short read_count, int *count_read, const long timelimit,
        short *receive_info, short *dialog_info);
/* Changes begin for TSMP Version greater than 2.3 */
extern long put_RECEIVEREADL(const short filenum, char *buffer,
        const int read_count, int *count_read, const long timelimit,
        short *receive_info2);
/* Changes end for TS/MP Version Greater than 2.3 */

extern long put_REPLYX(const char *buffer, const short write_count,
        short *count_written, const short msg_tag, const short error_return);

/* Changes begin for TS/MP Version Greater than 2.3 */

extern long put_REPLYXL(const char *buffer, const int write_countL,
        int *count_writtenL, const short msg_tag, const short error_return);

/* Changes end for TS/MP Version Greater than 2.3 */

extern long put_generateTag(void);

extern put_error_t put_regTimerHandler(const put_TimerHandler_p functionPtr);

extern put_error_t put_regPathsendFile(const short fileno);

extern put_error_t put_regPathsendTagHandler(const long tag,
        put_FileIOHandler_p callback,
        void * userdata);

extern put_error_t put_unregPathsendTagHandler(const long tag);

/* Tandem-provided jackets */

short PUT_SERVERCLASS_SEND_ (
char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional */
int     timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional */
long    tag                 /* in optional  */
);
#pragma function PUT_SERVERCLASS_SEND_  (extensible, tal)

/* Changes begin for TS/MP Release greater than 2.3 */

short PUT_SERVERCLASS_SENDL_ (
char * pathmon,             /* in  */
short  pathmonbytes,        /* in  */
char * serverclass,         /* in  */
short  serverclassbytes,    /* in  */
char * writebufferL,        /* out */
char * readbufferL,         /* in  */
int    requestbytes,        /* in  */
int    maximumreplybytes,   /* in  */
int  * actualreplybytes,    /* out optional */
int    timeout,             /* in optional  */
short  flags,               /* in optional  */
short * scsoperationnumber, /* out optional */
long long    tag            /* in optional  */
);
#pragma function PUT_SERVERCLASS_SENDL_  (extensible, tal)

/* Changes end for TS/MP Release greater than 2.3 */

short PUT_SERVERCLASS_DIALOG_BEGIN_ (
int  *  dialogid,           /* out */
char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional */
int     timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional */
long    tag                 /* in optional  */
);
#pragma function PUT_SERVERCLASS_DIALOG_BEGIN_  (extensible, tal)

short PUT_SERVERCLASS_DIALOG_SEND_ (
int     dialogid,           /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional */
int     timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional */
long    tag                 /* in optional  */
);
#pragma function PUT_SERVERCLASS_DIALOG_SEND_  (extensible, tal)

/* Changes begin for U64 DLCP Support for TS/MP Version greater than 2.5 */
short  PUT_SERVERCLASS_DIALOG_BEGINL_ (
int *   dialogid,           /* out */
char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  writebufferL,       /* out  */
char *  readbufferL,        /* in  */
int     requestbytes,       /* in  */
int     maximumreplybytes,  /* in  */
int *   actualreplybytes,   /* out optional  */
int     timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long long  tag              /* in optional  */
);
#pragma function PUT_SERVERCLASS_DIALOG_BEGINL_  (extensible, tal)

short  PUT_SERVERCLASS_DIALOG_SENDL_ (
int     dialogid,           /* in  */
char *  writebufferL,       /* out  */
char *  readbufferL,        /* in  */
int     requestbytes,       /* in  */
int     maximumreplybytes,  /* in  */
int *   actualreplybytes,   /* out optional  */
int     timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long long  tag              /* in optional  */
);
#pragma function PUT_SERVERCLASS_DIALOG_SENDL_  (extensible, tal)
/* Changes end for U64 DLCP Support for TS/MP Version greater than 2.5 */


short PUT_SERVERCLASS_DIALOG_END_ (
                 int     dialogid            /* in  */
);
#pragma function PUT_SERVERCLASS_DIALOG_END_  (extensible, tal)

short PUT_SERVERCLASS_DIALOG_ABORT_ (
                 int     dialogid            /* in  */
);
#pragma function PUT_SERVERCLASS_DIALOG_ABORT_  (extensible, tal)

short PUT_SERVERCLASS_SEND_INFO_ (
        short * serverclasserror,
        short * filesystemerror
        );
#pragma function PUT_SERVERCLASS_SEND_INFO_  (extensible, tal)


short PUT_FILE_OPEN_ (
const char *filename,           /* in  */
short length,                   /* in  */
short *filenum,                 /* in/out  */
short access,                   /* in optional  */
short exclusion,                /* in optional  */
short nowait_depth,             /* in optional  */
short sync_or_receive_depth,    /* in optional  */
short options,                  /* in optional  */
short seq_block_buffer_id,      /* in optional  */
short seq_block_buffer_len,     /* in optional  */
short *primary_processhandle,   /* in optional  */
int   elections                 /* in optional  */
);
#pragma function PUT_FILE_OPEN_  (extensible, tal)

short PUT_READX (
short filenum,                  /* in  */
char *buffer,                   /* out */
#ifdef __LP64
int   read_count,               /* in  */
int  *count_read,               /* out optional */
#else
unsigned short read_count,      /* in  */
unsigned short *count_read,     /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_READX  (extensible, tal)

short PUT_READUPDATEX (
short filenum,                  /* in  */
char *buffer,                   /* out */
#ifdef __LP64
int   read_count,               /* in  */
int  *count_read,               /* out optional */
#else 
unsigned short  read_count,     /* in  */
unsigned short *count_read,     /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_READUPDATEX  (extensible, tal)

short PUT_READLOCKX (
short filenum,                  /* in  */
char *buffer,                   /* out */
#ifdef __LP64
int   read_count,               /* in  */
int  *count_read,               /* out optional */
#else 
unsigned short read_count,      /* in  */
unsigned short *count_read,     /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_READLOCKX  (extensible, tal)

short PUT_READUPDATELOCKX (
short filenum,                  /* in  */
char *buffer,                   /* out */
#ifdef __LP64
int   read_count,               /* in  */
int  *count_read,               /* out optional */
#else 
unsigned short read_count,      /* in  */
unsigned short *count_read,     /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_READUPDATELOCKX  (extensible, tal)

short PUT_WRITEX (
short filenum,                  /* in  */
const char *buffer,             /* in  */
#ifdef __LP64
int   write_count,              /* in  */
int  *count_written,            /* out optional */
#else 
unsigned short write_count,     /* in  */
unsigned short *count_written,  /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_WRITEX  (extensible, tal)

short PUT_WRITEUPDATEX (
short filenum,                  /* in  */
const char *buffer,             /* in  */
#ifdef __LP64
int   write_count,              /* in  */
int  *count_written,            /* out optional */
#else 
unsigned short write_count,     /* in  */
unsigned short *count_written,  /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_WRITEUPDATEX  (extensible, tal)

short PUT_WRITEUPDATEUNLOCKX (
short filenum,                  /* in  */
const char *buffer,             /* in  */
#ifdef __LP64
int   write_count,              /* in  */
int  *count_written,            /* out optional */
#else 
unsigned short write_count,     /* in  */
unsigned short *count_written,  /* out optional */
#endif
long tag                        /* in optional  */
);
#pragma function PUT_WRITEUPDATEUNLOCKX  (extensible, tal)

short PUT_LOCKFILE (
short filenum,  /* in  */
long tag        /* in optional  */
);
#pragma function PUT_LOCKFILE  (extensible, tal)

short PUT_UNLOCKFILE (
short filenum,  /* in  */
long tag        /* in optional  */
);
#pragma function PUT_UNLOCKFILE  (extensible, tal)

short PUT_LOCKREC (
short filenum,  /* in  */
long tag        /* in optional  */
);
#pragma function  PUT_LOCKREC  (extensible, tal)

short PUT_UNLOCKREC (
short filenum,  /* in  */
long tag        /* in optional  */
);
#pragma function PUT_UNLOCKREC  (extensible, tal)

short PUT_CANCEL (
short filenum   /* in  */
);
#pragma function PUT_CANCEL  (extensible, tal)

short PUT_CONTROL (
short filenum,          /* in  */
short operation,        /* in  */
short param,            /* in optional  */
long tag                /* in optional  */
);
#pragma function PUT_CONTROL  (extensible, tal)

short PUT_SETMODE (
short filenum,          /* in  */
short function,         /* in  */
short param1,           /* in optional  */
short param2,           /* in optional  */
short *last_params      /* out optional */
);
#pragma function PUT_SETMODE  (extensible, tal)

short PUT_FILE_CLOSE_ (
short filenum,          /* in  */
short tape_disposition  /* in optional */
);
#pragma function PUT_FILE_CLOSE_  (extensible, tal)

short PUT_WRITEREADX (
short filenum,                  /* in  */
char *buffer,                   /* out */
#ifdef __LP64
int   write_count,              /* in  */
int   read_count,               /* in  */
int  *count_read,               /* out optional */
#else 
unsigned short write_count,     /* in  */
unsigned short read_count,      /* in  */
unsigned short *count_read,     /* out optional */
#endif 
long tag                        /* in optional  */
);
#pragma function PUT_WRITEREADX  (extensible, tal)

short PUT_FILE_WRITEREAD_ (
short filenum,                  /* in  */
char *write_buffer,             /* in  */
char *read_buffer,              /* out optional */
int   write_count,              /* in  */
int   read_count,               /* in  */
int  *count_read,               /* out optional */
long tag                        /* in optional  */
);
#pragma function PUT_FILE_WRITEREAD_  (extensible, tal)


#define PUT_TMF_TXHANDLE_WORD_SIZE 10
typedef struct {
        short data[ PUT_TMF_TXHANDLE_WORD_SIZE ];
} PUT_TMF_TxHandle_t;

extern int   put_setTMFConcurrentTransactions(short);
extern int   put_getTMFConcurrentTransactions(void);
extern short PUT_TMF_GetTxHandle( PUT_TMF_TxHandle_t *tx_handle );
extern short PUT_TMF_SetTxHandle( PUT_TMF_TxHandle_t *tx_handle );
extern short PUT_BEGINTRANSACTION (int * transaction_tag);
extern short PUT_RESUMETRANSACTION (int transaction_tag);
extern short PUT_ABORTTRANSACTION ( void );
extern short PUT_ENDTRANSACTION ( void );
/* Start of addition for solution # 10-060920-9199 */
extern short PUT_TMF_SUSPEND ( long long *txid );
extern short PUT_TMF_RESUME ( long long txid );
/* End of addition for solution # 10-060920-9199 */
/* Changes begin for SQL/MX 3.2 Release. */
#if _TANDEM_ARCH_ >= 2
short PUT_BEGINTRANSACTION_EXT_ (
int * transaction_tag,     /* in, required if caller has multiple threads */
                           /* but optional if caller has only one thread  */
int   timeout,             /* in, optional */
long long type_flags       /* in, optional */
);
#pragma function PUT_BEGINTRANSACTION_EXT_  (extensible, tal)
#endif /* _TANDEM_ARCH_ >= 2 */
/* Changes end for SQL/MX 3.2 Release. */

extern short PUT_TMF_Init( void );

extern short PUT_TMF_SetAndValidateTxHandle( PUT_TMF_TxHandle_t *tx_handle );

/* List of defines provided for compatibility (with the SPT library) */
#define pthread_attr_default        NULL
#define pthread_condattr_default    NULL
#define pthread_mutexattr_default   NULL


#ifdef __cplusplus
   }
#endif

#endif  /* _PUT_EXTENSIONS defined */
