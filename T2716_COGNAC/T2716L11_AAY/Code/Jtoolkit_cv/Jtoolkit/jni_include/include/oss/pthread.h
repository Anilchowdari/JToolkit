#ifdef __TANDEM
#pragma columns 100
#if _TANDEM_ARCH_ != 0
#pragma ONCE
#endif
#endif
#ifndef _PTHREAD
#define _PTHREAD

/* T8645Y21 - (05FEB2010) - pthread.h  POSIX thread definitions */

/*
 *  Copyright 2010 Hewlett Packard Development Company, L.P.
 *
 *     ALL RIGHTS RESERVED
 */

/*
 *  This header does not support Standard Posix Thread applications
 */
#ifdef _SPT_MODEL_
   #error Header incompatible with _SPT_MODEL_
#endif

/*
 *  Start of definitions supporting Posix User Thread Model applications
 */
#ifdef _PUT_MODEL_

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef _GUARDIAN_HOST
#include <sys/types.h>
#include <sched.h>
#else
#include <systype.h> nolist
#include <sched.h> nolist
#endif /* _GUARDIAN_HOST */


/*
 *  POSIX threads symbols
 */

#define PTHREAD_CANCEL_ASYNCHRONOUS  0   /* pthread_setcanceltype() 'type' values */
#define PTHREAD_CANCEL_DEFERRED      1

#define PTHREAD_CANCEL_ENABLE     0      /* pthread_setcancelstate() 'state' values */
#define PTHREAD_CANCEL_DISABLE    1

#define PTHREAD_CANCELED  ((void *)-1)   /* 'thread canceled' status value */

#define PTHREAD_CREATE_JOINABLE   0      /* pthread_attr_getdetachstate() /                */
#define PTHREAD_CREATE_DETACHED   1      /*   pthread_attr_setdetachstate() 'state' values */

#define PTHREAD_INHERIT_SCHED     0      /* Thread scheduling attribute values */
#define PTHREAD_EXPLICIT_SCHED    1

#define PTHREAD_MUTEX_DEFAULT     0      /* Mutex type attribute value */
#define PTHREAD_MUTEX_ERRORCHECK  2
#define PTHREAD_MUTEX_NORMAL      0
#define PTHREAD_MUTEX_RECURSIVE   1

#define PTHREAD_PROCESS_PRIVATE   0      /* 'process-shared' attribute values */
#define PTHREAD_PROCESS_SHARED    1

#define PTHREAD_SCOPE_PROCESS     0     /* 'contention scope' attribute values */
#define PTHREAD_SCOPE_SYSTEM      1

                                         /* Default pthread_cond_t initialization attributes */
#define PTHREAD_COND_INITIALIZER {(void *)-1, -1, -1}
                                         /* Default pthread_mutex_t initialization attributes */
#define PTHREAD_MUTEX_INITIALIZER {(void *)-1, -1, -1}
                                         /* Default pthread_once_t initialization attributes */
#define PTHREAD_ONCE_INIT         {0, 0, 0}

/*
 *  Symbols below are defined in limits.h
 *
       _POSIX_THREAD_DESTRUCTOR_ITERATIONS  4
       _POSIX_THREAD_KEYS_MAX             128
       _POSIX_THREAD_THREADS_MAX           64

        PTHREAD_DESTRUCTOR_ITERATIONS       4
        PTHREAD_KEYS_MAX                 1024
        PTHREAD_STACK_MIN                4096
        PTHREAD_THREADS_MAX              2048
 */

/*
 *  Symbols below are defined in unistd.h  (0 for supported, -1 for not supported)
 *
       _POSIX_THREADS                     0
       _POSIX_THREAD_ATTR_STACKADDR      -1
       _POSIX_THREAD_ATTR_STACKSIZE       0
       _POSIX_THREAD_PROCESS_SHARED      -1
       _POSIX_THREAD_SAFE_FUNCTIONS       0
       _POSIX_THREAD_PRIORITY_SCHEDULING  0
       _POSIX_THREAD_PRIO_INHERIT        -1
       _POSIX_THREAD_PRIO_PROTECT        -1
       _POSIX_THREAD_CPUTIME             -1
       _POSIX_THREAD_SPORADIC_SERVER     -1
       _POSIX_READER_WRITER_LOCKS        -1
 */


/*
 *  Definitions below are specified in <sys/types.h>
 *
       typedef struct PTHREAD_HANDLE_T {
                  void       *field1;
                  short int   field2;
                  short int   field3;
               } pthread_handle_t;

       typedef pthread_handle_t pthread_t;
       typedef pthread_handle_t pthread_attr_t;
       typedef pthread_handle_t pthread_cond_t;
       typedef pthread_handle_t pthread_condattr_t;
       typedef int              pthread_key_t;
       typedef pthread_handle_t pthread_mutex_t;
       typedef pthread_handle_t pthread_mutexattr_t;
       typedef struct PTHREAD_ONCE_T {
                  int    field1;
                  int    field2;
                  int    field3;
               } pthread_once_t;
 */

/*
 *  Definition below must match the one in <time.h>
 */
#if _TANDEM_ARCH_ >= 2 /* && __H_Series_RVU >= 621 */ \
    && !defined(_SPT_MODEL_)
#ifndef __timespec_DEFINED
#define __timespec_DEFINED
#ifdef __TANDEM
#ifdef __LP64
#pragma fieldalign platform timespec
#else
#pragma fieldalign shared8 timespec
#endif
#endif
typedef struct timespec {
    time_t              tv_sec;         /* seconds */
    long                tv_nsec;        /* and nanoseconds */
    } timespec_t;
#endif /* __timespec_DEFINED */
#endif /* _TANDEM_ARCH_ >= 2 && !_SPT_MODEL_ */

/* If function, prototype would be:
      void pthread_cleanup_push(void (*)(void *), void *); */
#define pthread_cleanup_push(routine,arg) {  \
                void (*_XXX_proc)(void*) = (routine); \
                int   _XXX_completed = 0; \
                void *_XXX_arg = (arg);   \
        _TRY_ {

/* If function, prototype would be:  void pthread_cleanup_pop(int); */
#define pthread_cleanup_pop(execute)  \
                _XXX_completed = 1; } \
        _FINALLY_ { \
                int _XXX_execute = execute; \
                if ((! _XXX_completed) || (_XXX_execute)) \
                   _XXX_proc (_XXX_arg); } \
        _ENDTRY_ }

/* If function, prototype would be:  int pthread_equal(pthread_t, pthread_t); */
#define pthread_equal(thread1,thread2)          \
        (((thread1).field1 == (thread2).field1) \
        && ((thread1).field2 == (thread2).field2)       \
        && ((thread1).field3 == (thread2).field3))


/*
 *  POSIX threads function prototypes
 */

int   pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
int   pthread_attr_destroy(pthread_attr_t *);
int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int   pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
int   pthread_attr_getinheritsched(const pthread_attr_t *, int *);
int   pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *);
int   pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
int   pthread_attr_getscope( const pthread_attr_t *, int *);
int   pthread_attr_getstackaddr(const pthread_attr_t *, void **);
int   pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
int   pthread_attr_init(pthread_attr_t *);
int   pthread_attr_setdetachstate(pthread_attr_t *, int);
int   pthread_attr_setguardsize(pthread_attr_t *, size_t);
int   pthread_attr_setinheritsched(pthread_attr_t *, int);
int   pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *);
int   pthread_attr_setschedpolicy(pthread_attr_t *, int);
int   pthread_attr_setscope( pthread_attr_t *, int);
int   pthread_attr_setstacksize(pthread_attr_t *, size_t);
int   pthread_cancel(pthread_t);
int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
                             const struct timespec *);
int   pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int   pthread_condattr_destroy(pthread_condattr_t *);
int   pthread_condattr_init(pthread_condattr_t *);
int   pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
int   pthread_detach(pthread_t);
void  pthread_exit(void *);
int   pthread_getconcurrency(void);
int   pthread_getschedparam(pthread_t, int *, struct sched_param *);
void *pthread_getspecific(pthread_key_t);
int   pthread_join(pthread_t, void **);
int   pthread_key_create(pthread_key_t *, void (*)(void *));
int   pthread_key_delete(pthread_key_t);
int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
int   pthread_mutexattr_init(pthread_mutexattr_t *);
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int   pthread_once(pthread_once_t *, void (*)(void));
pthread_t pthread_self(void);
int   pthread_setcancelstate(int, int *);
int   pthread_setcanceltype(int, int *);
int   pthread_setconcurrency(int);
int   pthread_setschedparam(pthread_t, int, const struct sched_param *);
int   pthread_setspecific(pthread_key_t, const void *);
void  pthread_testcancel(void);

/*
 *  Function prototypes below are defined in <signal.h>
 *
      int  pthread_kill(pthread_t, int);
      int  pthread_sigmask( int, const sigset_t *, sigset_t *);
 */


/*
 *  Text below provides definitions valid only for Posix User Thread
 *  Model applications.
 */
#include <setjmp.h> nolist

/*
 * Each _TRY_ clause allocates a context block in the current stack frame.  The
 * blocks that are in use are linked to form a stack of all relevant context
 * blocks in the current thread.
 *
 * Macro _TRY_ allocates and begins initializing a context block, then calls a
 * function to put it on a list and complete the initialization, and finally
 * calls setjmp().
 * Macro _FINALLY_ will 'pop' a context block if it is still in its initial state.
 * Macro _ENDTRY_ will 'pop' a context block if it is still on the list.
 */

#define _TRY_ \
    { \
        __exc_context_t __exc_ctx; \
        __exc_ctx.sentinel = __exc_newexc_c; \
        __exc_ctx.version = __exc_v2ctx_c; \
        __exc_ctx.exc_args[0] = 0; \
        __exc_push_ctx (&__exc_ctx); \
        if (!setjmp (__exc_ctx.jmp)) {
/*              { user's primary block of code goes here }      */
#define _FINALLY_ } \
        if (__exc_ctx.exc_state == __exc_none_c) \
            __exc_pop_ctx (&__exc_ctx); \
        {
/*              { user's epilogue block of code goes here }     */
#define _ENDTRY_ \
        } \
        if (__exc_ctx.exc_state == __exc_none_c || __exc_ctx.exc_state == __exc_active_c) \
            __exc_pop_ctx (&__exc_ctx); \
    }

/*
 * A context block contains an EXCEPTION, a jump buffer, and various other fields.
 * The EXCEPTION is either an "address-kind" or a "status-kind" structure.  Because
 * the first field of both structures is an '__exc_kind_t', the '__exc_kind_t' option
 * of the union exists just for convenience.
 */

typedef enum _EXC_KIND_T {
    __exc_kind_address_c = 0x02130455,
    __exc_kind_status_c  = 0x02130456
    }            __exc_kind_t;
typedef char *   __exc_address_t;
typedef long int __exc_int_t;

typedef struct _EXC_EXT_T {
    __exc_int_t         sentinel;
    __exc_int_t         version;
    __exc_address_t     extend;
    unsigned int       *args;
    } __exc_ext_t;

typedef struct _EXC_KIND_ADDRESS_T {
    __exc_kind_t        kind;
    __exc_address_t     address;
    __exc_ext_t         ext;
    } __exc_kind_address_t;

typedef struct _EXC_KIND_STATUS_T {
    __exc_kind_t        kind;
    __exc_int_t         status;
    __exc_ext_t         ext;
    } __exc_kind_status_t;

typedef struct _EXC_KIND_V1ADDR_T {
    __exc_kind_t        kind;
    __exc_address_t     address;
    __exc_int_t         filler[6];
    } __exc_kind_v1addr_t;

typedef struct _EXC_KIND_V1STATUS_T {
    __exc_kind_t        kind;
    __exc_int_t         status;
    __exc_int_t         filler[6];
    } __exc_kind_v1status_t;

typedef union _EXC_EXCEPTION_T   {
    __exc_kind_t          kind;
    __exc_kind_v1status_t v1status;
    __exc_kind_v1addr_t   v1address;
    __exc_kind_status_t   status;
    __exc_kind_address_t  address;
    } _EXCEPTION;

/*
 * The state of a context block is "none" when no exception has been raised,
 * "active" when one has been raised but has not yet been handled, "handled"
 * when a raised exception has been handled, or "popped" when unlinked from
 * the context block stack.  __exc_push_ctx() initializes the exc_state field.
 */
typedef enum _EXC_STATE_T {
    __exc_active_c      = 0, /* This must be the 0 state */
    __exc_none_c        = 1,
    __exc_handled_c     = 2,
    __exc_popped_c      = 3
    } __exc_state_t;

#define __exc_newexc_c  0x45586732     /* sentinel code for "new" context block */
#define __exc_v2ctx_c    2             /* version code of "new" context block */
#define __exc_excargs_c 40

typedef struct _EXC_CONTEXT {
    jmp_buf             jmp;            /* Jump buffer */
    struct _EXC_CONTEXT *link;          /* Link to context block stack */
    _EXCEPTION          cur_exception;  /* Copy of the current exception */
    __exc_state_t       exc_state;      /* State of handling for this _TRY_*/
    __exc_int_t         sentinel;       /* Identify as a "new" ctx block */
    __exc_int_t         version;        /* Client context version */
    unsigned int        exc_args[__exc_excargs_c];
    } __exc_context_t;

extern void __exc_push_ctx(__exc_context_t *cb);    /* Push a context block */
extern void __exc_pop_ctx (__exc_context_t *cb);    /* Pop a context block */

/*
 *  Text below provides definitions valid only for Posix User Thread
 *  Model applications.
 */
#if defined(_TANDEM_SOURCE)
#ifndef _GUARDIAN_HOST
#include <put/put_extensions.h>
#else
#include <putexts.h> nolist
#endif /* _GUARDIAN_HOST */
#endif /* _TANDEM_SOURCE */


#ifdef __cplusplus
   }
#endif

#endif /* _PUT_MODEL_ */
/*
 *  End of definitions supporting Posix User Thread Model applications
 */

#endif  /* _PTHREAD defined */
