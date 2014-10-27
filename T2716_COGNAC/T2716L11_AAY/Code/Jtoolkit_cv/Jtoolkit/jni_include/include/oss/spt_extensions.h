/* 
 * @ @ @ START COPYRIGHT @ @ @
 *
 * Copyright 2005
 * Hewlett Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and 
 * documentation herein are the property of Compaq Computer 
 * Corporation and successor entities such as Hewlett Packard 
 * Development Company, L.P., or a third party supplier and 
 * shall not be reproduced, copied, disclosed, or used in whole 
 * or in part for any reason without the prior express written 
 * permission of Hewlett Packard Development Company, L.P.
 *
 * @ @ @ END COPYRIGHT @ @ @
 */

/*
 * HISTORY
 * $Log: spt_extensions.h,v $
 *
 * 2010/09/28
 * Fix Soln: 10-100526-0628. OSS - pthreads  SPT_xxx wrapper functions.
 * Changes: Changed the byte count parameters (read_count, write_count & *count_read)
 * from short to unsigned short in the thread aware wrappers to match the current 
 * definitions of the NSK I/O functions.
 *
 * $EndLog$
 */

#include <spt_ucontext.h>
#include <dirent.h>

#ifndef _SPT_EXTENSIONS_H /* { */
#define _SPT_EXTENSIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SPT_THREAD_SIGNAL

#define alarm(seconds) spt_alarm(seconds)

#define signal(sig,handler) spt_signal((sig),(handler))

#endif

#ifdef SPT_THREAD_AWARE

#define sleep(seconds) spt_sleep(seconds)

#define usleep(useconds) spt_usleep(useconds)

#define accept(socket, address, address_len) \
spt_accept((socket), (address), (address_len))

#define connect(socket, address, address_len) \
spt_connect((socket), (address), (address_len))

#define read(filedes, buffer, nbytes) spt_read((filedes), (buffer), (nbytes))

#define readv(filedes, iov, iov_count) spt_readv((filedes), (iov), (iov_count))

#define recv(socket, buffer, length, flags) \
spt_recv((socket), (buffer), (length), (flags))

#define recvfrom(socket, buffer, length, flags, address, address_len) \
spt_recvfrom((socket), (buffer), (length), (flags), (address), \
(address_len))

#define recvmsg(socket, message, flags) \
spt_recvmsg((socket), (message), (flags))

#ifdef SPT_SELECT_SINGLE
#define select(nfds, readfds, writefds, errorfds, timeout) \
spt_select_single_np((nfds), (readfds), (writefds), (errorfds), (timeout))

#else
#define select(nfds, readfds, writefds, errorfds, timeout) \
spt_select((nfds), (readfds), (writefds), (errorfds), (timeout))

#endif /* SPT_SELECT_SINGLE */

#define send(socket, buffer, length, flags) \
spt_send((socket), (buffer), (length), (flags))

#define sendto(socket, buffer, length, flags, dest_addr, dest_len) \
spt_sendto((socket), (buffer), (length), (flags), (dest_addr), (dest_len))

#define sendmsg(socket, message, flags) \
spt_sendmsg((socket), (message), (flags))

#define system(command) spt_system(command)

#define waitpid(pid, stat_loc, options) \
spt_waitpid((pid), (stat_loc), (options))

#define write(filedes, buffer, nbytes) spt_write((filedes), (buffer), (nbytes))

#define writev(filedes, iov, iov_count) \
spt_writev((filedes), (iov), (iov_count))

#define close(filedes) spt_close(filedes)

#undef getchar
#define getchar() spt_getchar()

#define fgetc(stream) spt_fgetc(stream)

#define fread(pointer, size, num_items, stream) \
spt_fread((pointer), (size), (num_items), (stream))

#undef getc
#define getc(stream) spt_getc(stream)

#define getw(stream) spt_getw(stream)

#define gets(string) spt_gets(string)

#define fgets(string, n, stream) spt_fgets((string), (n), (stream))

#define getwc(stream) spt_getwc(stream)

#define fgetwc(stream) spt_fgetwc(stream)

#define getwchar() spt_getwchar()

#undef putchar
#define putchar(c) spt_putchar(c)

#define fputc(c, stream) spt_fputc((c), (stream))

#define fwrite(pointer, size, num_items, stream) \
spt_fwrite((pointer), (size), (num_items), (stream))

#undef putc
#define putc(c, stream) spt_putc((c), (stream))

#define putw(c, stream) spt_putw((c), (stream))

#define puts(string) spt_puts(string)

#define fputs(string, stream) spt_fputs((string), (stream))

#define putwc(c, stream) spt_putwc((c), (stream))

#define fputwc(c, stream) spt_fputwc((c), (stream))


#define putwchar(c) spt_putwchar(c)

#define fprintf spt_fprintf

#define vfprintf(stream, format, printarg) \
spt_vfprintf((stream), (format), (printarg))

#define printf spt_printf

#define vprintf(format, printarg) spt_vprintf((format), (printarg))

#define fflush(stream) spt_fflush(stream)

#define fclose(stream) spt_fclose(stream)

#endif /* SPT_THREAD_AWARE */


/* Adonis R1 - Begin */
#if defined(SPT_THREAD_AWARE_NONBLOCK) || defined(SPT_THREAD_AWARE_XNONBLOCK)
/* Adonis R1 - End */

#define accept(socket, address, address_len) \
spt_acceptx((socket), (address), (address_len))

#define connect(socket, address, address_len) \
spt_connectx((socket), (address), (address_len))

/* Adonis R1 - Begin */
#ifdef SPT_THREAD_AWARE_XNONBLOCK

#define close(filedes) spt_closez((filedes))

#define fcntl   spt_fcntlz
/* forward declarations */
int spt_fstatz(int filedes, struct stat *buffer);

int spt_ftruncatez(int filedes, off_t length);

off_t   spt_lseekz(int filedes, off_t offset, int whence);

#if (_FILE_OFFSET_BITS == 64) 
#if (_LARGEFILE64_SOURCE == 1)
#define fstat(filedes, buffer)         spt_fstat64z((filedes), (struct stat64 *)(buffer))

#define ftruncate(filedes, length)     spt_ftruncate64z((filedes), (off64_t)(length))

#define lseek(filedes, offset, whence) spt_lseek64z((filedes), (off64_t)(offset), (whence))

#define spt_fstatz(filedes, buffer)    spt_fstat64z((filedes), (struct stat64 *)(buffer))

#define spt_ftruncatez(filedes, length)        spt_ftruncate64z((filedes), (off64_t)(length))

#define spt_lseekz(filedes, offset, whence)    spt_lseek64z((filedes), (off64_t)(offset), (whence))
#else 
/* forward declarations of these 64-bit functions with special parameters */
int spt_fstat64z(int filedes, struct stat *buffer);

int spt_ftruncate64z(int filedes, off_t length);

off_t spt_lseek64z(int filedes, off_t offset, int whence);
#define fstat(filedes, buffer)         spt_fstat64z((filedes), (buffer))

#define ftruncate(filedes, length)     spt_ftruncate64z((filedes), (length))

#define lseek(filedes, offset, whence) spt_lseek64z((filedes), (offset), (whence))

#define spt_fstatz(filedes, buffer)    spt_fstat64z((filedes), (buffer))

#define spt_ftruncatez(filedes, length)        spt_ftruncate64z((filedes), (length))

#define spt_lseekz(filedes, offset, whence)    spt_lseek64z((filedes), (offset), (whence))

#endif /* (_LARGEFILE64_SOURCE == 1) */
#else 

#define fstat(filedes, buffer)         spt_fstatz((filedes), (buffer))

#define ftruncate(filedes, length)     spt_ftruncatez((filedes), (length))

#define lseek(filedes, offset, whence) spt_lseekz((filedes), (offset), (whence))

#endif /* (_FILE_OFFSET_BITS == 64)  */

#define fsync(filedes)                 spt_fsyncz((filedes))

#define read(filedes, buffer, nbytes)  spt_readz((filedes), (buffer), (nbytes))

#define readv(filedes, iov, iov_count) spt_readvz((filedes), (iov), (iov_count))

#define write(filedes, buffer, nbytes) spt_writez((filedes), (buffer), (nbytes))

#define writev(filedes, iov, iov_count) \
spt_writevz((filedes), (iov), (iov_count))

#if (_LARGEFILE64_SOURCE == 1)

/* forward declarations of these 64-bit functions */
int spt_fstat64z(int filedes, struct stat64 *buffer);

int spt_ftruncate64z(int filedes, off64_t length);

off64_t spt_lseek64z(int filedes, off64_t offset, int whence);
#define fstat64(filedes, buffer)       spt_fstat64z((filedes), (buffer))

#define ftruncate64(filedes, length)   spt_ftruncate64z((filedes), (length))

#define lseek64(filedes, offset, whence) spt_lseek64z((filedes), (offset), (whence))

#endif

#else
/* Adonis R1 - End */

#define close(filedes) spt_closex(filedes)

#define fcntl   spt_fcntlx

#define read(filedes, buffer, nbytes) spt_readx((filedes), (buffer), (nbytes))

#define readv(filedes, iov, iov_count) spt_readvx((filedes), (iov), (iov_count))

#define write(filedes, buffer, nbytes) spt_writex((filedes), (buffer), (nbytes))

#define writev(filedes, iov, iov_count) \
spt_writevx((filedes), (iov), (iov_count))

/* Adonis R1 - Begin */
#endif /* SPT_THREAD_AWARE_XNONBLOCK */
/* Adonis R1 - End */

#define dup2(filedes, newfd) spt_dup2x((filedes), (newfd))

#define recv(socket, buffer, length, flags) \
spt_recvx((socket), (buffer), (length), (flags))

#define recvfrom(socket, buffer, length, flags, address, address_len) \
spt_recvfromx((socket), (buffer), (length), (flags), (address), \
(address_len))

#define recvmsg(socket, message, flags) \
spt_recvmsgx((socket), (message), (flags))

#ifdef SPT_SELECT_SINGLE
#define select(nfds, readfds, writefds, errorfds, timeout) \
spt_select_single_np((nfds), (readfds), (writefds), (errorfds), (timeout))

#else
#define select(nfds, readfds, writefds, errorfds, timeout) \
spt_select((nfds), (readfds), (writefds), (errorfds), (timeout))

#endif /* SPT_SELECT_SINGLE */

#define send(socket, buffer, length, flags) \
spt_sendx((socket), (buffer), (length), (flags))

#define sendto(socket, buffer, length, flags, dest_addr, dest_len) \
spt_sendtox((socket), (buffer), (length), (flags), (dest_addr), (dest_len))

#define sendmsg(socket, message, flags) \
spt_sendmsgx((socket), (message), (flags))

#define system(command) spt_system(command)

#define waitpid(pid, stat_loc, options) \
spt_waitpid((pid), (stat_loc), (options))

#undef getchar
#define getchar() spt_getcharx()

#define fgetc(stream) spt_fgetcx(stream)

#define fread(pointer, size, num_items, stream) \
spt_freadx((pointer), (size), (num_items), (stream))

#undef getc
#define getc(stream) spt_getcx(stream)

#define getw(stream) spt_getwx(stream)

#define gets(string) spt_getsx(string)

#define fgets(string, n, stream) spt_fgetsx((string), (n), (stream))

#define getwc(stream) spt_getwcx(stream)

#define fgetwc(stream) spt_fgetwcx(stream)

#define getwchar() spt_getwcharx()

#undef putchar
#define putchar(c) spt_putcharx(c)

#define fputc(c, stream) spt_fputcx((c), (stream))

#define fwrite(pointer, size, num_items, stream) \
spt_fwritex((pointer), (size), (num_items), (stream))

#undef putc
#define putc(c, stream) spt_putcx((c), (stream))

#define putw(c, stream) spt_putwx((c), (stream))

#define puts(string) spt_putsx(string)

#define fputs(string, stream) spt_fputsx((string), (stream))

#define putwc(c, stream) spt_putwcx((c), (stream))

#define fputwc(c, stream) spt_fputwcx((c), (stream))


#define putwchar(c) spt_putwcharx(c)

#define fprintf spt_fprintfx

#define vfprintf(stream, format, printarg) \
spt_vfprintfx((stream), (format), (printarg))

#define printf spt_printfx

#define vprintf(format, printarg) spt_vprintfx((format), (printarg))

#define fflush(stream) spt_fflushx(stream)

#define fclose(stream) spt_fclosex(stream)

#endif /* SPT_THREAD_AWARE_XNONBLOCK or SPT_THREAD_AWARE_NONBLOCK */

#ifdef SPT_THREAD_SIGNAL_PRAGMA

#pragma function alarm (alias("spt_alarm"))

#pragma function signal (alias("spt_signal"))

#endif

#ifdef SPT_THREAD_AWARE_PRAGMA

#pragma function accept (alias("spt_accept"))

#pragma function connect (alias("spt_connect"))

#pragma function read (alias("spt_read"))

#pragma function readv (alias("spt_readv"))

#pragma function recv (alias("spt_recv"))

#pragma function recvfrom (alias("spt_recvfrom"))

#pragma function recvmsg (alias("spt_recvmsg"))

#ifdef SPT_SELECT_SINGLE
#pragma function select (alias("spt_select_single_np"))

#else
#pragma function select (alias("spt_select"))

#endif/*SPT_SELECT_SINGLE*/

#pragma function send (alias("spt_send"))

#pragma function sendto (alias("spt_sendto"))

#pragma function sendmsg (alias("spt_sendmsg"))

#pragma function system (alias("spt_system"))

#pragma function waitpid (alias("spt_waitpid"))

#pragma function write (alias("spt_write"))

#pragma function writev (alias("spt_writev"))

#pragma function close (alias("spt_close"))

/* Added for Solution : 10-050929-1703 */
#undef getchar
#pragma function getchar (alias("spt_getchar"))

#pragma function fgetc (alias("spt_fgetc"))

#pragma function fread (alias("spt_fread"))

/* Added for Solution : 10-050929-1703 */
#undef getc
#pragma function getc (alias("spt_getc"))

#pragma function getw (alias("spt_getw"))

#pragma function gets (alias("spt_gets"))

#pragma function fgets (alias("spt_fgets"))

#pragma function getwc (alias("spt_getwc"))

#pragma function fgetwc (alias("spt_fgetwc"))

#pragma function getwchar (alias("spt_getwchar"))

/* Added for Solution : 10-050929-1703 */
#undef putchar
#pragma function putchar (alias("spt_putchar"))

#pragma function fputc (alias("spt_fputc"))

#pragma function fwrite (alias("spt_fwrite"))

/* Added for Solution : 10-050929-1703 */
#undef putc
#pragma function putc (alias("spt_putc"))

#pragma function putw (alias("spt_putw"))

#pragma function puts (alias("spt_puts"))

#pragma function fputs (alias("spt_fputs"))

#pragma function putwc (alias("spt_putwc"))

#pragma function fputwc (alias("spt_fputwc"))

#pragma function putwchar (alias("spt_putwchar"))

#pragma function fprintf (alias("spt_fprintf"))

#pragma function vfprintf (alias("spt_vfprintf"))

#pragma function printf (alias("spt_printf"))

#pragma function vprintf (alias("spt_vprintf"))

#pragma function fflush (alias("spt_fflush"))

#pragma function fclose (alias("spt_fclose"))

#pragma function sleep (alias("spt_sleep"))

#pragma function usleep (alias("spt_usleep"))

#endif /* SPT_THREAD_AWARE_PRAGMA */


/* Adonis R1 - Begin */
#if defined(SPT_THREAD_AWARE_PRAGMA_XNONBLOCK) || defined(SPT_THREAD_AWARE_PRAGMA_NONBLOCK)
/* Adonis R1 - End */

#pragma function accept (alias("spt_acceptx"))

#pragma function connect (alias("spt_connectx"))

/* Adonis R1 - Begin */
#ifdef SPT_THREAD_AWARE_PRAGMA_XNONBLOCK

#pragma function close (alias("spt_closez"))

#pragma function fcntl (alias("spt_fcntlz"))

#pragma function fsync (alias("spt_fsyncz"))
int spt_fstatz(int filedes, struct stat *buffer);

int spt_ftruncatez(int filedes, off_t length);

off_t   spt_lseekz(int filedes, off_t offset, int whence);
#if (_FILE_OFFSET_BITS == 64)

#pragma function fstat (alias("spt_fstat64z"))

#pragma function ftruncate (alias("spt_ftruncate64z"))

#pragma function lseek (alias("spt_lseek64z"))

#pragma function spt_fstatz (alias("spt_fstat64z"))

#pragma function spt_ftruncatez (alias("spt_ftruncate64z"))

#pragma function spt_lseekz (alias("spt_lseek64z"))

#else

#pragma function fstat (alias("spt_fstatz"))

#pragma function ftruncate (alias("spt_ftruncatez"))

#pragma function lseek (alias("spt_lseekz"))

#endif

#pragma function read (alias("spt_readz"))

#pragma function readv (alias("spt_readvz"))

#pragma function write (alias("spt_writez"))

#pragma function writev (alias("spt_writevz"))

#if (_LARGEFILE64_SOURCE == 1)

/* forward declarations of these 64-bit functions */
int spt_fstat64z(int filedes, struct stat64 *buffer);

int spt_ftruncate64z(int filedes, off64_t length);

off64_t spt_lseek64z(int filedes, off64_t offset, int whence);
#pragma function fstat64 (alias("spt_fstat64z"))

#pragma function ftruncate64 (alias("spt_ftruncate64z"))

#pragma function lseek64 (alias("spt_lseek64z"))

#endif

#else
/* Adonis R1 - End */

#pragma function close (alias("spt_closex"))

#pragma function fcntl (alias("spt_fcntlx"))

#pragma function read (alias("spt_readx"))

#pragma function readv (alias("spt_readvx"))

#pragma function write (alias("spt_writex"))

#pragma function writev (alias("spt_writevx"))

#endif /* SPT_THREAD_AWARE_PRAGMA_XNONBLOCK */

#pragma function dup2 (alias("spt_dup2x"))

#pragma function recv (alias("spt_recvx"))

#pragma function recvfrom (alias("spt_recvfromx"))

#pragma function recvmsg (alias("spt_recvmsgx"))

#ifdef SPT_SELECT_SINGLE
#pragma function select (alias("spt_select_single_np"))

#else
#pragma function select (alias("spt_select"))

#endif/*SPT_SELECT_SINGLE*/

#pragma function send (alias("spt_sendx"))

#pragma function sendto (alias("spt_sendtox"))

#pragma function sendmsg (alias("spt_sendmsgx"))

#pragma function system (alias("spt_system"))

#pragma function waitpid (alias("spt_waitpid"))

#undef getchar
#pragma function getchar (alias("spt_getcharx"))

#pragma function fgetc (alias("spt_fgetcx"))

#pragma function fread (alias("spt_freadx"))

#undef getc
#pragma function getc (alias("spt_getcx"))

#pragma function getw (alias("spt_getwx"))

#pragma function gets (alias("spt_getsx"))

#pragma function fgets (alias("spt_fgetsx"))

#pragma function getwc (alias("spt_getwcx"))

#pragma function fgetwc (alias("spt_fgetwcx"))

#pragma function getwchar (alias("spt_getwcharx"))

#undef putchar
#pragma function putchar (alias("spt_putcharx"))

#pragma function fputc (alias("spt_fputcx"))

#pragma function fwrite (alias("spt_fwritex"))

#undef putc
#pragma function putc (alias("spt_putcx"))

#pragma function putw (alias("spt_putwx"))

#pragma function puts (alias("spt_putsx"))

#pragma function fputs (alias("spt_fputsx"))

#pragma function putwc (alias("spt_putwcx"))

#pragma function fputwc (alias("spt_fputwcx"))

#pragma function putwchar (alias("spt_putwcharx"))

#pragma function fprintf (alias("spt_fprintfx"))

#pragma function vfprintf (alias("spt_vfprintfx"))

#pragma function printf (alias("spt_printfx"))

#pragma function vprintf (alias("spt_vprintfx"))

#pragma function fflush (alias("spt_fflushx"))

#pragma function fclose (alias("spt_fclosex"))

#endif /* SPT_THREAD_AWARE_PRAGMA_XNONBLOCK or SPT_THREAD_AWARE_PRAGMA_NONBLOCK */


enum spt_error {
	SPT_SUCCESS, SPT_ERROR, SPT_INTERRUPTED, SPT_TIMEDOUT 

};

/*
 * DESCRIPTION
 *
 *   Callback type required by spt_regFileIOHandler().  
 *
 * PARAMETERS
 *
 *   filenum           - Guardian file number whose IO has completed.
 *   tag               - tag of completed IO.
 *   count_transferred - transfer count of completed IO.
 *   error             - Guardian error number for completed IO.
 *   userdata          - address of user data area.  Set when user called
 *                       spt_awaitio().
 *
 * RETURN VALUES
 *
 *   None.
 */
typedef void (*spt_FileIOHandler_p)(const short filenum, const long
	tag, const long count_transferred, const long error, void *userdata);

/*
 * DESCRIPTION
 *
 *   Callback type required by spt_regOSSFileIOHandler().
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
typedef void (*spt_OSSFileIOHandler_p)(const int filedes,
	const int read_set, const int write_set, const int error_set);

/*
 * DESCRIPTION
 *
 *   Callback type required by spt_regTimerHandler().
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
typedef long (*spt_TimerHandler_p)(void);

typedef enum spt_error spt_error_t;

extern int spt_accept(int socket, struct sockaddr *a, 
	size_t *address_len);

extern unsigned int spt_alarm(unsigned int seconds);

extern int spt_connect(int socket, const struct sockaddr *address,
	size_t address_len);

extern ssize_t spt_read(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_readv(int filedes, struct iovec *iov, int iov_count);

extern ssize_t spt_recv(int socket, void *buffer, size_t length, int flags);

extern ssize_t spt_recvfrom(int socket, void *buffer, size_t length,
	int flags, struct sockaddr *address, size_t *address_len);

extern ssize_t spt_recvmsg(int socket, struct msghdr *message, int flags);

extern int spt_select(int nfds, fd_set *readfds, fd_set *writefds,
	fd_set *errorfds, struct timeval *timeout);

extern int spt_select_single_np(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *errorfds, struct timeval *timeout);

extern ssize_t spt_send(int socket, const void *buffer, size_t length,
	int flags);

extern ssize_t spt_sendto(int socket, const void *buffer, size_t
	length, int flags, const struct sockaddr *dest_addr, size_t dest_len);

extern ssize_t spt_sendmsg(int socket, const struct msghdr *message,
	int flags);

extern void * spt_signal(int sig,void (* handler)(int));

extern unsigned int sleep(unsigned int seconds);

extern int usleep(unsigned int useconds);

extern unsigned int spt_sleep(unsigned int seconds);

extern int spt_usleep(unsigned int useconds);

extern int spt_system(const char *command);

extern pid_t wait(int _ptr64 *stat_loc);

extern pid_t spt_waitpid(pid_t pid, int *stat_loc, int options);

extern ssize_t spt_write(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_writev(int filedes, struct iovec *iov, int iov_count);

extern int spt_close(int filedes);

extern char *spt_fgets(char *string, int n, FILE *stream);

extern char *spt_gets(char *string);

extern int spt_fgetc(FILE *stream);

extern int spt_fprintf(FILE *stream, const char *format, ...);

extern int spt_fputc(int c, FILE *stream);

extern int spt_fputs(const char *string, FILE *stream);

extern int spt_getc(FILE *stream);

extern int spt_getchar(void);

extern int spt_getw(FILE *stream);

extern int spt_printf(const char *format, ...);

extern int spt_putc(int c, FILE *stream);

extern int spt_putchar(int c);

extern int spt_puts(const char *string);

extern int spt_putw(int c, FILE *stream);

extern int spt_vprintf(const char *format, va_list printarg);

extern size_t spt_fread(void *pointer, size_t size, size_t num_items,
	FILE *stream);

extern size_t spt_fwrite(const void *pointer, size_t size, size_t num_items,
	FILE *stream);

extern wint_t spt_fgetwc(FILE *stream);

extern wint_t spt_fputwc(wint_t c, FILE *stream);

extern wint_t spt_getwc(FILE *stream);

extern wint_t spt_getwchar(void);

extern wint_t spt_putwc(wint_t c, FILE *stream);

extern wint_t spt_putwchar(wint_t c);

extern int spt_vfprintf(FILE *stream, const char *format, va_list printarg);

extern int spt_fflush(FILE *stream);

extern int spt_fclose(FILE *stream);

extern int spt_acceptx(int socket, struct sockaddr *a,
        size_t *address_len);

extern int spt_connectx(int socket, const struct sockaddr *address,
        size_t address_len);

extern ssize_t spt_readx(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_readvx(int filedes, struct iovec *iov, int iov_count);

extern int spt_fcntlx(int filedes, int request, ...);

extern int spt_dup2x(int filedes, int newfd);

extern ssize_t spt_recvx(int socket, void *buffer, size_t length, int flags);

extern ssize_t spt_recvfromx(int socket, void *buffer, size_t length,
        int flags, struct sockaddr *address, size_t *address_len);

extern ssize_t spt_recvmsgx(int socket, struct msghdr *message, int flags);

extern ssize_t spt_sendx(int socket, const void *buffer, size_t length,
        int flags);

extern ssize_t spt_sendtox(int socket, const void *buffer, size_t
        length, int flags, const struct sockaddr *dest_addr, size_t dest_len);

extern ssize_t spt_sendmsgx(int socket, const struct msghdr *message,
        int flags);

extern ssize_t spt_writex(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_writevx(int filedes, struct iovec *iov, int iov_count);

extern int spt_closex(int filedes);

extern char *spt_fgetsx(char *string, int n, FILE *stream);

extern char *spt_getsx(char *string);

extern int spt_fgetcx(FILE *stream);

extern int spt_fprintfx(FILE *stream, const char *format, ...);

extern int spt_fputcx(int c, FILE *stream);

extern int spt_fputsx(const char *string, FILE *stream);

extern int spt_getcx(FILE *stream);

extern int spt_getcharx(void);

extern int spt_getwx(FILE *stream);

extern int spt_printfx(const char *format, ...);

extern int spt_putcx(int c, FILE *stream);

extern int spt_putcharx(int c);

extern int spt_putsx(const char *string);

extern int spt_putwx(int c, FILE *stream);

extern int spt_vprintfx(const char *format, va_list printarg);

extern size_t spt_freadx(void *pointer, size_t size, size_t num_items,
        FILE *stream);

extern size_t spt_fwritex(const void *pointer, size_t size, size_t num_items,
        FILE *stream);

extern wint_t spt_fgetwcx(FILE *stream);

extern wint_t spt_fputwcx(wint_t c, FILE *stream);

extern wint_t spt_getwcx(FILE *stream);

extern wint_t spt_getwcharx(void);

extern wint_t spt_putwcx(wint_t c, FILE *stream);

extern wint_t spt_putwcharx(wint_t c);

extern int spt_vfprintfx(FILE *stream, const char *format, va_list printarg);

extern int spt_fflushx(FILE *stream);

extern int spt_fclosex(FILE *stream);

/* Adonis R1 Begin */
extern int spt_closez(int filedes);

extern int spt_fcntlz(int filedes, int request, ...);

extern int spt_fsyncz(int filedes);

extern ssize_t spt_readz(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_readvz(int filedes, struct iovec *iov, int iov_count);

extern ssize_t spt_writez(int filedes, void *buffer, size_t nbytes);

extern ssize_t spt_writevz(int filedes, struct iovec *iov, int iov_count);

/* Adonis R1 End */

extern int spt_fd_read_ready(const int fd, struct timeval *timeout);

extern int spt_fd_write_ready(const int fd, struct timeval *timeout);

extern spt_error_t spt_regOSSFileIOHandler(const int filedes,
	const spt_OSSFileIOHandler_p functionPtr);

extern spt_error_t spt_unregOSSFileIOHandler(const int filedes);

extern spt_error_t spt_setOSSFileIOHandler(const int filedes,
	const int set_read, const int set_write, const int set_error);

extern spt_error_t spt_awaitio(const short filenum, const long tag,
	const long timelimit, long *count_transferred, long *error,
	void *userdata);

extern spt_error_t spt_interrupt(const short filenum, const spt_error_t
	errorSPT);

extern spt_error_t spt_interruptTag(const short filenum, const long
	tag, const spt_error_t errorSPT);

extern spt_error_t spt_regFile(const short filenum);

extern spt_error_t spt_regFileIOHandler(const short filenum, 
	const spt_FileIOHandler_p functionPtr);

extern spt_error_t spt_unregFile(const short filenum);

extern spt_error_t spt_postfork_unregFile(const short filenum);

extern spt_error_t spt_wakeup(const short filenum, const long tag,
	const long count_transferred, const long error);

extern long spt_INITRECEIVE(const short filenum, const short receive_depth);

/* Changes begin for TS/MP Version greater then 2.3 */

extern long spt_INITRECEIVEL(const short filenum, const short receive_depth);

/* Changes end for TS/MP Version greater then 2.3 */

extern long spt_RECEIVEREAD(const short filenum, char *buffer, 
	const short read_count, long *count_read, const long timelimit, 
	short *receive_info, short *dialog_info);
/* Changes begin for TSMP Version greater the 2.3 */
extern long spt_RECEIVEREADL(const short filenum, char *buffer,
	const long read_count, long *count_read, const long timelimit,
	short *receive_info2);
/* Changes end for TS/MP Version Greater then 2.3 */

extern long spt_REPLYX(const char *buffer, const short write_count, 
	short *count_written, const short msg_tag, const short error_return);

/* Changes begin for TS/MP Version Greater then 2.3 */

extern long spt_REPLYXL(const char *buffer, const long write_countL,
        long *count_writtenL, const short msg_tag, const short error_return);

/* Changes end for TS/MP Version Greater then 2.3 */

extern long spt_generateTag(void);

extern spt_error_t spt_regTimerHandler(const spt_TimerHandler_p functionPtr);

extern spt_error_t spt_regPathsendFile(const short fileno);

extern spt_error_t spt_regPathsendTagHandler(const long tag,
        spt_FileIOHandler_p callback,
        void * userdata);

extern spt_error_t spt_unregPathsendTagHandler(const long tag);

extern int pthread_mutexattr_setkind_np(pthread_mutexattr_t *,int);

extern int pthread_mutexattr_getkind_np(pthread_mutexattr_t);

/* Define nonportable extensions */

#define pthread_equal_np(thread1,thread2) \
(((thread1).field1 == (thread2).field1) \
&& ((thread1).field2 == (thread2).field2) \
&& ((thread1).field3 == (thread2).field3))

#define pthread_getunique_np(handle) \
((unsigned int)((pthread_t *)handle)->field2)

extern int pthread_get_expiration_np(struct timespec *, struct timespec *);

extern int pthread_delay_np(const struct timespec *);

/*Start addition for soln # 10-060518-6588*/
#ifdef SPT_THREAD_UCONTEXT
extern int pthread_getcontext_np(pthread_t target, spt_ucontext_t *ucp);
#else
extern int pthread_getcontext_np(pthread_t target, ucontext_t *ucp);
#endif
/*End addition for soln # 10-060518-6588*/

extern int pthread_lock_global_np(void);

extern int pthread_unlock_global_np(void);

extern int pthread_signal_to_cancel_np(sigset_t *,pthread_t *);

extern int pthread_getattr_np(const pthread_t, pthread_attr_t **);

extern int pthread_attr_setguardsize_np(pthread_attr_t *, size_t);

extern int pthread_attr_getguardsize_np(pthread_attr_t *, size_t *);

extern int pthread_cond_signal_int_np(pthread_cond_t *);

/* Tandem-provided jackets */

_tal _extensible short  SPT_SERVERCLASS_SEND_ (
	char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long    tag                 /* in optional  */
);

/* Changes begin for TS/MP Release greater then 2.3 */

_tal _extensible short SPT_SERVERCLASS_SENDL_ (
char * pathmon,             /* in  */
short  pathmonbytes,        /* in  */
char * serverclass,         /* in  */
short  serverclassbytes,    /* in  */
char * writebufferL,        /* out  */
char * readbufferL,         /* in  */
long   requestbytes,       /* in  */
long   maximumreplybytes,  /* in  */
long * actualreplybytes,   /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long long    tag                 /* in optional  */
);
 
/* Changes end for TS/MP Release greater then 2.3 */

_tal _extensible short  SPT_SERVERCLASS_DIALOG_BEGIN_ (
long *  dialogid,           /* out */
char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long    tag                 /* in optional  */
);

_tal _extensible short  SPT_SERVERCLASS_DIALOG_SEND_ (
long    dialogid,           /* in  */
char *  messagebuffer,      /* in/out  */
short   requestbytes,       /* in  */
short   maximumreplybytes,  /* in  */
short * actualreplybytes,   /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long    tag                 /* in optional  */
);

/* Changes begin for TS/MP 2.5 Release */

_tal _extensible short  SPT_SERVERCLASS_DIALOG_BEGINL_ (
long *  dialogid,           /* out */
char *  pathmon,            /* in  */
short   pathmonbytes,       /* in  */
char *  serverclass,        /* in  */
short   serverclassbytes,   /* in  */
char *  writebufferL,       /* out  */
char *  readbufferL,        /* in  */
long   requestbytes,        /* in  */
long   maximumreplybytes,   /* in  */
long * actualreplybytes,    /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long long  tag              /* in optional  */
);

_tal _extensible short  SPT_SERVERCLASS_DIALOG_SENDL_ (
long    dialogid,           /* in  */
char *  writebufferL,       /* out  */
char *  readbufferL,        /* in  */
long   requestbytes,        /* in  */
long   maximumreplybytes,   /* in  */
long * actualreplybytes,    /* out optional  */
long    timeout,            /* in optional  */
short   flags,              /* in optional  */
short * scsoperationnumber, /* out optional  */
long long  tag              /* in optional  */
);

/* Changes end for TS/MP 2.5 Release */

_tal _extensible short SPT_SERVERCLASS_DIALOG_END_ (
                 long    dialogid            /* in  */
);

_tal _extensible short SPT_SERVERCLASS_DIALOG_ABORT_ (
                 long    dialogid            /* in  */
);

_tal _extensible short SPT_SERVERCLASS_SEND_INFO_ (
	short * serverclasserror,
	short * filesystemerror 
	);


_tal _extensible short SPT_FILE_OPEN_ (
const char *filename, 		/* in  */
short length,			/* in  */		
short *filenum,			/* in/out  */
short access,			/* in optional  */ 
short exclusion,		/* in optional  */ 
short nowait_depth,		/* in optional  */ 
short sync_or_receive_depth,	/* in optional  */ 
short options,			/* in optional  */ 
short seq_block_buffer_id,	/* in optional  */ 
short seq_block_buffer_len,	/* in optional  */ 
short *primary_processhandle,	/* in optional  */ 
long elections  		/* in optional  */
);

_tal _extensible short SPT_READX (
short filenum,			/* in  */
char *buffer,			/* out  */
unsigned short read_count,		/* in  */
unsigned short *count_read, 		/* out optional  */
long tag 			/* in optional  */ 
);

_tal _extensible short SPT_READUPDATEX (
short filenum,		/* in  */	
char *buffer,		/* out  */
unsigned short  read_count,	/* in  */
unsigned short *count_read,	/* out optional  */
long tag		/* in optional  */
);

_tal _extensible short SPT_READLOCKX (
short filenum,		/* in  */
char *buffer,		/* out  */
unsigned short read_count,	/* in  */
unsigned short *count_read, 	/* out optional  */
long tag  		/* in optional  */
);

_tal _extensible short SPT_READUPDATELOCKX (
short filenum,		/* in  */
char *buffer,		/* out  */
unsigned short read_count,	/* in  */
unsigned short *count_read, 	/* out optional  */
long tag 		/* in optional  */
);

_tal _extensible short SPT_WRITEX (
short filenum,			/* in  */
const char *buffer,			/* in  */
unsigned short write_count,		/* in  */	
unsigned short *count_written, 		/* out optional  */
long tag			/* in optional  */
);


_tal _extensible short SPT_WRITEUPDATEX (
short filenum, 		/* in  */
const char *buffer,	/* in  */
unsigned short write_count,	/* in  */
unsigned short *count_written,	/* out optional  */ 
long tag 		/* in optional  */
);

_tal _extensible short SPT_WRITEUPDATEUNLOCKX (
short filenum,		/* in  */
const char *buffer,		/* in  */
unsigned short write_count,	/* in  */
unsigned short  *count_written,	/* out optional  */
long tag		/* in optional  */
);

_tal _extensible short SPT_LOCKFILE (
short filenum,	/* in  */
long tag 	/* in optional  */
);

_tal _extensible short SPT_UNLOCKFILE (
short filenum,	/* in  */
long tag 	/* in optional  */
);

_tal _extensible short SPT_LOCKREC (
short filenum,	/* in  */
long tag 	/* in optional  */
);

_tal _extensible short SPT_UNLOCKREC (
short filenum,  /* in  */
long tag        /* in optional  */
);

_tal _extensible short SPT_CANCEL (
short filenum	/* in  */
);

_tal _extensible short SPT_CONTROL (
short filenum,		/* in  */
short operation,	/* in  */
short param, 		/* in optional  */
long tag		/* in optional  */
);

_tal _extensible short SPT_SETMODE (
short filenum,		/* in  */
short function,		/* in  */
short param1,		/* in optional  */
short param2,		/* in optional  */
short *last_params	/* out optional  */
);

_tal _extensible short SPT_FILE_CLOSE_ (
short filenum,          /* in  */
short tape_disposition	/* in  optional */
);

_tal _extensible short SPT_WRITEREADX (
short filenum,                  /* in  */
char *buffer,                   /* out  */
unsigned short write_count,    	/* in  */
unsigned short read_count,   	/* in  */
unsigned short *count_read,     /* out optional  */
long tag                        /* in optional  */
);


#define SPT_TMF_TXHANDLE_WORD_SIZE 10
typedef struct {
	short data[ SPT_TMF_TXHANDLE_WORD_SIZE ];
} SPT_TMF_TxHandle_t;

extern int   spt_setTMFConcurrentTransactions(short);
extern int   spt_getTMFConcurrentTransactions(void);
extern short SPT_TMF_GetTxHandle( SPT_TMF_TxHandle_t *tx_handle );
extern short SPT_TMF_SetTxHandle( SPT_TMF_TxHandle_t *tx_handle );
extern short SPT_BEGINTRANSACTION (long * transaction_tag);
extern short SPT_RESUMETRANSACTION (long transaction_tag);
extern short SPT_ABORTTRANSACTION ( void );
extern short SPT_ENDTRANSACTION ( void );
/* Start of addition for solution # 10-060920-9199 */
extern short SPT_TMF_SUSPEND ( long long *txid );
extern short SPT_TMF_RESUME ( long long txid );
/* End of addition for solution # 10-060920-9199 */

/* Changes begin for SQL/MX 3.2 Support. */
short SPT_BEGINTRANSACTION_EXT_( 
long * transaction_tag,    /* in, required if caller has multiple threads */
                           /* but optional if caller has only one thread  */
long timeout, 		   /* in, optional */
long long type_flags	   /* in, optional */
);
#pragma function SPT_BEGINTRANSACTION_EXT_  (extensible, tal)
/* Changes end for SQL/MX 3.2 Support. */

extern short SPT_TMF_Init( void );

extern short SPT_TMF_SetAndValidateTxHandle( SPT_TMF_TxHandle_t *tx_handle );


extern char *ctime_r (const time_t *clock, char *buf);
extern char *asctime_r(const struct tm *timeptr, char *buf);
extern char *ctermid_r(char *buf);
extern char *tmpnam_r(char *s);
extern char *strtok_r(char *s, const char *sep, char **lasts);
extern double gamma_r(double x, int *signgamp);
extern double lgamma_r(double x, int *signgamp);
extern int rand_r(unsigned int *seed);
extern int getlogin_r(char *name, size_t namesize);
extern int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
extern int  ttyname_r(int fildes, char *name, size_t namesize);
extern struct tm *gmtime_r(const time_t *clock, struct tm *result);
extern struct tm *localtime_r(const time_t *clock, struct tm *result);
#ifdef _SPT_MODEL_
extern struct group *getgrent_r(struct group *grp, char *buffer,size_t buflen);
#endif
extern struct hostent *gethostbyaddr_r(const  char  *addr,  int length,                                                int type, struct hostent *host,                                                 char *buffer, int buflen, int *h_errnop);
extern struct hostent *gethostbyname_r(const char *name, struct hostent *host,                                         char *buffer, int buflen, int *h_errnop);
extern struct hostent *gethostent_r(struct  hostent  *host,  char *buffer,                                          int buflen,int  *h_errnop);
extern struct netent *getnetbyaddr_r(long  net, int type, struct netent *ret,                                        char *buffer,int buflen);
extern struct netent *getnetbyname_r(const  char  *name,   struct netent *ret,                                       char *buffer, int buflen);
extern struct netent *getnetent_r(struct  netent  *ret,   char *buffer,                                           int buflen);
extern struct protoent *getprotobyname_r(const char *name, struct protoent *ret,                                         char *buffer, int buflen);
extern struct protoent *getprotobynumber_r(int proto, struct protoent *ret,                                                char *buffer, int buflen);
extern struct protoent *getprotoent_r(struct protoent *entry, char *buffer,                                           int buflen);
#ifdef _SPT_MODEL_
extern struct passwd *getpwent_r(struct passwd *ret, char *buf, size_t buflen);
#endif
extern struct servent *getservbyname_r(const char *name, const char *proto,                                            struct servent *ret, char *buffer,                                              int buflen);
extern struct servent *getservbyport_r(int port, const char *proto,                                                    struct servent *ret,char *buffer,                                               int buflen);
extern struct servent *getservent_r(struct servent *ret, char *buffer,                                              int buflen);
extern int getgrgid_r(gid_t gid, struct group *grp, char *buffer,                                     size_t buflen,struct group **result);
extern int getgrnam_r(const  char  *name,  struct  group *grp, char *buffer,                          size_t buflen,struct group **result);
extern int getpwnam_r(const char *name, struct passwd *ret, char *buffer,                             size_t buflen, struct passwd **result);
extern int getpwuid_r(uid_t uid, struct passwd *ret, char *buffer,                                    size_t buflen,struct passwd **result);


/*
 * pthread_stack_info_np is the structure used to store the memory used,
 * register used,memory base,register base,memory size and register
 * size for a thread.
 */
typedef struct PTHREAD_STACK_INFO_NP {
        int stk_flags;

        /* Size of the stack and register stack */
        size_t  stk_stacksize;
        size_t  stk_rsesize;

        /* Amount of stack and register stack used */
        size_t  stk_stackused;
        size_t  stk_rseused;

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

/*
 * Functions to set and get the register stack percentage for non main threads
 */

extern spt_error_t pthread_attr_set_rsestackpercent_np(short reg_percent);
extern short pthread_attr_get_rsestackpercent_np();

#ifdef __cplusplus
}
#endif   /* end of C++ wrapper */

#endif	/* } _SPT_EXTENSIONS_H */

