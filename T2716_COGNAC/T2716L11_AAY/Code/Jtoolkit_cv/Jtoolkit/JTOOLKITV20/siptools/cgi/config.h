/********************************************************
 * CONFIGURATION FILE FOR THE WN PACKAGE (vers 1.18.0)
 ********************************************************/

/********************************************************
 * Compulsory items to fill in.
 ********************************************************/

#define STANDALONE

 #define _NSKOSS  
/*#define TANDEM_GUARDIAN*/		/* Operating System */
/*
   Pick one of the following Operating System types which describes
   your system and replace the SUN_OS4 value above

   AIX, AUX, BSDI, CONVEX_0S, DYNIX, FREE_BSD_2, HPUX, ISC, LINUX, 
   NET_BSD, NEXT, OSF1, PYRAMID, RISCOS, SCO, SGI, SINIX, SOLARIS2, SUN_OS4,
   SVR4, ULTRIX, UNIXWARE, UXPDS, OTHER

   I have no way to test all these, let me know if something doesn't work
   or is incomplete.  If you pick OTHER you must edit the last few lines
   of this file per the directions there.

   If you pick SOLARIS2, SCO, UNIXWARE, NET_BSD, or FREE_BSD_2 you will
   need to edit the Makefile also.
*/


#define ROOT_DIR ""
/*
 * Enter the complete path of your root data directory here.
 */

#define WN_LOGFILE  ""
#define WN_ERRLOGFILE  ""

/*
 * Full path of the file you want to use for a log file and the
 * file for an errorlog. Comment out these lines to turn off logging.  
 * The values set here are overridden by the -L and -l options.  To use
 * system syslog facility run swn or wn with the -S command line option.
 * To do no logging set all the logfiles above to "", the empty string.
 */

/*
#define SWN_PID_FILE	""
 * Full path of the file in which you wish the standalone server swn
 * to deposit its process id on startup.  Comment this out or set
 * it to the empty string "" if you want the pid printed to stdout
 * on startup instead of being placed in a file.
 */


#define MAINTAINER	""
/*
 * Enter "mailto:" followed by the e-mail address of the individual
 * responsible for maintaining your server.  This is a default and
 * can be changed on a per directory basis.  It is used only by WWW
 * clients.
 */


#define MIME_TYPE_FILE  "mimetype"
/*
 * This file contains information permitting wndex to translate 
 * ascertain the MIME "Content-type" from the suffix of a file name
 * If this file is not present "wndex -v" will issue a warning
 * but use internal default values.  The file exists so that you
 * can add to it if you wish to add new kinds of documents to your
 * server.  The format of the file is explained in the file.
 * The default version of the file is in /lib/mime.types.
 * The internal defaults are the same as what is currently in this
 * file.
 */



/*
 ****************************************************************
 * This is the end of the compulsory fill out items (unless you
 * picked OTHER as your OS type above).  If you wish you can change
 * the following defaults; it is a good idea to at least look them
 * over.
 ****************************************************************
 */


/* #define VERBOSELOG */
/* By default WN uses the Common Log Format used by NCSA and CERN httpd.
 * Uncommenting this will cause additional information to be added to
 * the end of each log line.  In particular the User-Agent and Referrer
 * are logged. N.B.  I have chosen to use the correct spelling of
 * Referrer in the log file.  To enable interoperability and to comply
 * with the CGI spec it is necessary elsewhere to use the incorrect spelling
 * "referer" and the CGI environment variable HTTP_REFERER.
 */

 /* #define NO_KEEPALIVE */

/* WN supports the processing of multiple requests in a single TCP
 * request.  Some clients have buggy implementations of this so
 * you may not want it.  Uncommenting this disables the feature.
 */



/* #define USE_LATIN1 */
/* 
 * If you uncomment this then the server will allow *alphabetic* ISO
 * 8859-1 characters in file names which are served.  Normally only ASCII
 * alphanumeric characters are allowed.  The additional allowed
 * characters are those with Hex values from C0 to FF. This includes the
 * (anomolous) division sign.  I recommend that you not do this unless
 * you really know you need it.
 */

#define WN_HOSTNAME	""
/*
 * You may enter your fully qualified host name here.  If the default
 * value of "" is used WN will get the default hostname from your system.
 * If you supply a name it should be a fully qualified domain name.
 * WN will attempt to use the gethostbyname() system call to find the
 * IP address of your server.  
 */


#define SERVER_LOGO	"<hr>\n<address>NSK HTTP Library</address>\n"
/*
 * This is displayed with error messages and some other server
 * generated HTML messages.  You may want to customize it.
 */

/* #define FORBID_CGI */
/*
 * Uncomment this if you wish to forbid the use of the use of
 * CGI scripts on your host.  This will enhance security but will,
 * of course, decrease functionality.
 */


#ifndef USERID
#define USERID          (0)
#define GROUPID         (0)
#endif

/*
 * These are the group and user id you want the server to switch to when
 * running inetd or when swn is run by root.  They are also used by wn running
 * under inetd and in this case should agree with the uid you set in 
 * inetd.conf.  I have heard (but not verified) that it is necessary for
 * HPUX users to use a different user id than the one belonging to 'nobody'.
 * If you are using wn and can't set the uid in your inetd.conf
 * (e.g. Ultrix) be sure to #define SET_UID (uncomment it in this file below).
 */

#define DEFAULT_PORT	(80)
/*
 * If you wish to run at a different port than 80, edit this line.
 */

#define NO_DNS_HOSTNAMES
/*
 * Uncomment this if you do not wish the server to look up DNS hostnames
 * from IP addresses and enter them in logfiles.  Instead only the IP
 * address will be in the log file.  This well reduce the load on your
 * server but probably not improve response time since the lookup is usually
 * done after the transaction anyway.
 */

/* #define CHECK_DNS_HOSTNAMES */
/*
 * Uncomment this if you want to try to check that the client is not 
 * lying about its DNS hostname.  Leave it commented for a slight 
 * efficiency improvement.  The extra check is always done if the 
 * name might be used for access control.
 */

extern char DEFAULT_URI[1024];

#define INDEXFILE_NAME	""   /* must be less than 32 chars long */

#define CACHEFNAME	""

#define CONTROLFILE_NAME	""

#define DEFAULT_CONTENT_TYPE	"text/plain"

#define TEMPDIR		""

#define MAXDEPTH	(10)    /* maximum depth of menus to search */

#define TRANSACTION_TIMEOUT	(0)  
				/*
				 * Time in seconds to wait before timeout 
				 * This time is reset if 256 Kb of binary
				 * data or 512 lines of text are sent. For
				 * large binary files this means an average of
				 * about 425 bytes/sec to avoid a timeout.
				 */


#define CGI_EXT		".cgi"  /* File extension for CGI scripts	*/

/* #define CGI_BIN		"cgibin"   Name of directory for CGI's */


/* #define ACCESS_DENIED_URL	"/noaccess.html" */
/* #define NO_SUCH_FILE_URL	"/noaccess.html" */
/* #define AUTH_DENIED_FILE	"/noauth.html"   */


/* #define DENYHANDLER "/pub/www/deny-handler" */

/* If defined, DENYHANDLER should be a command that is executed when an
 * "Error 404 - Access denied, or file does not exist" condition
 * is encountered.  The command must act like an nph- CGI program and
 * emit an HTTP status line and full document headers.  A CGI environment
 * is initialized based upon the original (bad) URL.  Comment out this
 * definition to get the standard built-in "Error 404" handling.
 * Thanks to Chip Rosenthal.
 */


/* #define USE_VIRTUAL_HOSTS */

/* #define VIRTUAL_HOSTS_FILE	"/full/path/virtual_hosts" */



/*
 * The following lines can be uncommented to enable one of the two
 * methods of allowing alternate data hierarchies.  This is the way
 * you permit users to have data hierarchies in their home directories
 * with the URL syntax like http://hostname/~user/.  Read the chapter
 * of the user's guide on this subject (Chapter 11).
 *
 * At most one of TILDE_TABLE and TILDE_USER_PWFILE should be uncommented!
 * If neither is uncommented (the default) then alternate hierarchies
 * are not allowed.
 *
 * If TILDE_TABLE is defined to the path of a file
 * containing name:directory pairs then URLs starting with 
 * TILDE_USER_STRINGname/foo will be changed to /foo and the root
 * directory will be the directory after the ':' in the table.
 *
 * If TILDE_USER_PWFILE is #defined then the user's name will be looked
 * up in the password file and the corresponding home directory with
 * the string PUB_HTML appended will be the root directory.  This will
 * fail if the user's uid is less than LEAST_UID.
 */

/* #define TILDE_TABLE	"/path/to/rootdir_table" */

/* #define TILDE_USER_PWFILE */

#define TILDE_USER_STRING	"/~"
/*
 * You can also use something like "/homepages/" or "/people/" for this
 * but use "/~"  to make http://hostname/~user/ work.  This appears to
 * be a de facto standard.
 */


#define PUB_HTML	"/public_html"
#define LEAST_UID	(0)

/*
 * Uncomment the following if you want to disable the server's ability
 * to serve documents not listed in an index.cache file.  See the
 * manual section docs/index_desc.html for more details.
 */

/* #define NO_SERVEALL */


/*
 * Uncomment the following if you want the server to attempt to do RFC931
 * (aka RFC1413, IDENT, or TAP) lookups on clients.  This may be good for
 * additional logging information, but should not be trusted for
 * authentication.
 *
 * Because of bugs in many PC TCP/IP stacks that may cause ident
 * connections to time out instead of being refused (certain firewall
 * configurations may cause the same problem) this defaults to off, and
 * the timeout should be kept low.  10 seconds is about as high as I
 * recommend; remember, this timeout will need to be fulfilled on EVERY
 * request from that client, so pages with lots of inline images will
 * become painful.
 *
 * The support code was modified by Christopher Davis <ckd@kei.com> based
 * on the RFC931 lookup code in Wietse Venema's tcp_wrappers 7.2.
 */

/* #define RFC931_TIMEOUT	(10) */



/********************************************************/

#define LOGFACILITY   LOG_DAEMON
#define LOG_PRIORITY    LOG_INFO
#define ERRLOG_PRIORITY    LOG_ERR
/*
 * Change this if you wish to use a local syslog facility.  This only has
 * effect if you are using the "-S" option.  If you use it, select the
 * priority at which you want the messages (LOG_NOTICE or LOG_INFO)?
 *
 */


/********************************************************
 * Edit the lines in this section if you chose OTHER as 
 * your OS type.
 ********************************************************/

/*
 * #define NO_LINGER
 *
 * Comment this out if you have problems with truncated documents being
 * returned to requests from remote hosts.  WN is slightly more efficient
 * with it uncommented.
 *
 */


/*
 * #define BSD_LIKE
 * This should be defined if your system is BSD-like, i.e. tends
 * to use BSD system calls rather than SysV system calls.
 */


#define NO_FLOCK 
/*
 * Uncomment this if your system does not have the flock() system call.
 * Doing this means that if more than one client is using your server 
 * at the same time, your logfile might be garbled.  There are no other
 * ill effects.
 *
 */


/*
 * #define SET_UID
 *
 *  Uncomment this if you are using an inetd without the capability 
 *  to set UID on startup (e.g., Ultrix). The user id and group id
 *  under which wn runs will then be set to the values of USERID and
 *  GROUPID set above.
 */


#define NO_SYSLOG
/*
 * Uncomment this if your system does not have the openlog()/syslog()
 * system calls.  Doing this means that you won't be able to have your
 * log entries put in the system log ( the "-S" option to gn).  You
 * can still use the "-L" option to write the entries to a named log file.
 *
 */

/*
 * #define NO_VOID_PTR
 *
 * Uncomment this if your compiler does not support void pointers.
 *
 */

/* #define NO_CC_CONST */
/*
 *
 * Uncomment this if your compiler does not support "const" arrays.
 *
 */

/*
 * #define NEED_PUTENV
 *
 * Uncomment this if your compiler library does not have the function
 * putenv().  
 *
 */

/*
 * #define NEED_STRFTIME
 *
 * Uncomment this if your compiler library does not have the function
 * strftime()
 *
 */

/*
 * #define NEED_STRNCASECMP
 *
 * Uncomment this if your compiler library does not have the function
 * strncasecmp()
 *
 */

/*
 * #define NEED_STRCASECMP
 *
 * Uncomment this if your compiler library does not have the function
 * strcasecmp()
 *
 */


/*************************************************************
 * Hopefully you shouldn't need to change things below here.
 *************************************************************/




/************************/


