#ifndef _CGILIB
#define _CGILIB

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CGI_NO_EXTERNALS
  void _MAIN(void);
  void (*DummyMainPtr)(void) =  _MAIN;
#endif



/*============================================================
//      CGI_fwrite
//
// This procedure is analogous to the write() procedure
// with the following exceptions: stdout is the only file
// descriptor that works with this procedure, and data is
// written to the Pathway CGI interface rather than to
// stdout.
//
//============================================================*/
size_t CGI_fwrite(const void *buffer,size_t size,size_t num_items,FILE *stream);

/*============================================================
//      CGI_fread
//
// This procedure is analogous to the fread() procedure in
// the C library with the following exceptions:
// Data is read from the Pathway CGI interface rather than
// from stdin, and stdin is the only file descriptor that
// this procedure may be called with.
//
//============================================================*/
size_t CGI_fread (void *buf, size_t size,size_t num_items,FILE *stream);

/*============================================================
//      CGI_printf
//
// This procedure is analogous to the printf() procedure
// in the C library, with one exception: output is passed back
// to the CGI client program via the Pathway interface rather
// than to the stdout file descriptor.
//
//============================================================*/
int CGI_printf(const char *format, ...);

/*============================================================
//      CGI_getc
//
// This procedure gets a character from the CGI input
// stream. It is the same as the Posix function getc(), but
// returns the next byte from the CGI input stream specified
// and moves the file pointer, if defined, ahead one byte in
// the stream.
//
//============================================================*/
int CGI_getc(FILE * stream);

/*============================================================
//      CGI_puts
//
// This procedure writes a string to the CGI output stream. It
// operates the same as its Posix equivalent.
//
//============================================================*/
int CGI_puts(const char *buffer);

/*============================================================
//      CGI_main
//
// This procedure is used as an entry point into a user-written
// CGI server class.
//
//============================================================*/
int CGI_main(int argc, char *argv[]);

/*============================================================
//      ErrorAbort
//
//
//============================================================*/
void ErrorAbort(void);

/*============================================================
//      CGI_connection_abort
//
// This is a stub procedure called whenever the connection
// between the CGI server and the HTTPD program is
// broken. Typically, this happens when the end user at the
// Web client stops an active data transfer prior to receiving
// all of the data being sent. This may also happen when
// there is an internal time-out within the HTTPD server
// itself, in the case where a single connection has existed
// longer than its configured lifetime.
// The intent of a user coded connection abort routine is to
// allows for graceful cleanup of transactions in progress.
//
//============================================================*/
void CGI_connection_abort(void);

/*============================================================
//      CGI_initialize
//
// This is a stub procedure called each time the server comes
// up to allow user-written initialization code (opening
// database files etc.) to be executed at start-up time.
//
//============================================================*/
void CGI_initialize(void);

/*============================================================
//      CGI_terminate
//
// This is a stub procedure called before the server stops or
// aborts to allow user-written cleanup code to be executed
// prior to process termination.
//
//============================================================*/
void CGI_terminate(void);

/*============================================================
//      CGI_fflush
//
// This procedure immediately writes buffered data from a
// CGI program to a specified stream. If the stream specified
// is stdout or stderr, this procedure immediately
// writes any buffered data to the httpd process.
//
//============================================================*/
int CGI_fflush(FILE * stream);

/*============================================================
//      CGI_set_fflush_timer
//
// This procedure specifies the interval at which stdout is
// flushed. To change a flush interval set by a call to
// CGI_set_fflush_timer(), the program must call
// CGI_set_fflush_timer() with seconds set to 0
// (zero), and then call CGI_set_fflush_timer() with
// a nonzero seconds value to set the new flush interval.
// The flush timer uses a signal handling routine that catches a
// SIGALARM signal. When the alarm signal is delivered to a
// process, it may interrupt long I/O operations, and cause an
// error. When this happens, the errno variable is set to 4004
// (interrupted system call. Users wishing to write servers that
// perform their own SIGALARM processing should set this
// value to 0.
// Only a single alarm signal can be in effect for an entire
// process. If you need to implement a customized alarm
// function and still use the fflush timer, you should code an
// alarm signal handler that calls CGI_fflush() when
// appropriate. NOT IMPLEMENTED IN MINICGI.
//
//============================================================*/
int CGI_set_fflush_timer(int seconds);

/*============================================================
//      CGI_Capture
//
// This procedure decodes the name/value pairs encoded in
// form data returned by the POST request method or in the
// QUERY_STRING returned by the GET request method,
// creates an environment variable for each name/value pair
// and sets the value of the variable. MiniCGI does this by 
// default unless the -naf option is used.
//
//============================================================*/
void CGI_Capture(void);

#ifdef __cplusplus
}
#endif

#endif /* CGILIB */
