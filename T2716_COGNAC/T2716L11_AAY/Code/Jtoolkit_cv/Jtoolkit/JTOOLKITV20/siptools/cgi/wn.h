/*
    Wn: A Server for the HTTP
    File: wn/wn.h
    Version 1.18.1
    
    Copyright (C) 1996, 1997  <by John Franks>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <setjmp.h>
#ifdef NEED_TIME_H
#include <time.h>
#endif
#ifdef _NSKOSS
#include <limits.h>
#else
#endif

#ifndef WN_CONST
#define WN_CONST const
#endif

#include "uniguard.h"
#include "common.h"

/*
#ifdef XXX_TNS_E_TARGET
extern unsigned	alarm();

extern int 
		chop(),
		wild_match(),
		modified(),
		getfpath(),
		getfpath2(),
		chkaccess(),
		amperline(),
		http_prolog(),
		set_show(),
		get_parse_token(),
		myisalnum(),
		eval_if(),
		read_cache_file();

extern void	wn_init(),
		init_mime(),
		wn_abort(),
		wn_exit(),
		flush_outbuf(),
		send_out_mem(),
		send_out_fd(),
		load_virtual(),
		get_mtype(),
		set_etag(),
		rfc931(),
		get_stat(),
		chk_cntrl(),
		check_perm(),
		exec_ok(),
		exit(),
		clear_req(),
		do_standalone(),
		do_connection(),
		dolocation(),
		file_open(),
		process_url(),
		writelog(),
		read_cache_dirinfo(),
		list_search(),
		cache_search(),
		send_nomatch(),
		send_isearch(),
		sendtext(),
		sendbin(),
		image(),
		do_wrap(),
		do_nomatchsub(),
		sendgrep(),
		sendinfo(),
		sendcgi(),
		send_markline_doc(),
		send_text_line(),
		search_prolog(),
		search_epilog(),
		get_remote_ip(),
		get_remote_info(),
		get_local_info(),
		startlog(),
		wn_cleanup(),
		www_unescape(),
		sendredirect(),
		send204(),
		cgi_env(),
		check_query(),
		do_swrap(),
		update_mod_time(),
		date_cmp(),
		www_err(),
		set_interface_root(),
		reset_parse_err(),
		parse_html_err(),
		daemon_logerr(),
		logerr(),
		senderr(),
		*mystrncpy(),
		*mystrncat(),
		*mymemcpy(),
		*strlower();
#endif
*/

#define HTTPVERSION "HTTP/1.0"
#define WN_HTML_MARK    "WN_mark"
#define ACCEPTLEN (2048)
#define MAXDIRLEN (256)
#define NUMFIELDS (20)

#define FREE	(0)
#define ROOTCHK	(1)
#define DIRCHK	(2)

#define FRP_FILE	(0)
#define FRP_PIPE	(1)

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	(256)
#endif

#define CLIENT_ERR	"400"
#define DENYSTATUS	"404"
#define PRECON_STATUS	"412"
#define SERV_ERR	"500"

extern FILE	*safer_popen();

extern 
char * 	err_m[];
extern 
char * log_m[];
extern 
char * search_m[];
extern 
char * out_m[];

extern char	rootdir[],
		wnlogfile[],
		errlogfile[],
		pid_file[],
		vhostfile[],
		cfname[],
		afname[],
		empty[],
		hostname[],
		remotehost[],
		remaddr[],
		**mtypelist,
		**suflist;

extern unsigned	cache_id,
		acache_id,
		interface_num;

extern int	errno,
		port,
		admin_mode,
		debug_log
	;


typedef enum { 
	RTYPE_DENIED,
	RTYPE_UNCHECKED,
	RTYPE_FILE,
	RTYPE_CGI,
	RTYPE_NPH_CGI,
	RTYPE_CGI_HANDLER,
	RTYPE_GSEARCH,
	RTYPE_CONTEXTSEARCH,
	RTYPE_LINESSEARCH,
	RTYPE_ISEARCH,
	RTYPE_MARKLINE,
	RTYPE_TSEARCH,
	RTYPE_KSEARCH,
	RTYPE_TKSEARCH,
	RTYPE_FIELDSEARCH,
	RTYPE_LISTSEARCH,
	RTYPE_INFO,
	RTYPE_HEADER,
	RTYPE_REDIRECT,
	RTYPE_NOT_MODIFIED,
	RTYPE_NO_AUTH,
	RTYPE_FINISHED,
	RTYPE_IMAGEMAP,
	RTYPE_NOACCESS,
	RTYPE_PRECON_FAILED
} Reqtype;

typedef struct struct_Request {
	char	request[BIGLEN],	/* The original request */
		cacheline[CACHELINE_LEN],	/* filled in by chkcache */
			/* These are pointers into cacheline */
		*title,			/* Item title */
		*content_type,		/* MIME content type */
		*encoding,		/* MIME content-transfer-encoding */
		*keywords,		/* string of keywords */
		*field[NUMFIELDS],	/* user defined fields */
		*includes,		/* comma separated insert files */
		*wrappers,		/* comma separated wrapper files */
		*list_incl,		/* comma separated include list */
		*swrapper,		/* search wrapper files */
		*nomatchsub,		/* Substitute for empty search result*/
		*filter,		/* Path to filter  */
		*handler,		/* Path to handler  */
		*maxage,                /* Maxage in ascii seconds */
		*expires,		/* Expiration date */
			/* These two are pointers into filepath */
		*relpath,		/* Path rel to rootdir */
		*basename,		/* Base name of file  */

		*inclptr,		/* Ptr to current wrap or include */
		*inclistp,		/* Ptr to current include list item */

		contype[SMALLLEN],	/* Use content_type, not this */
		rootdir[SMALLLEN],	/* Complete pathname of root dir  */
		filepath[MIDLEN],	/* Complete pathname of file  */
		gname[MAXGUARDFILE],  /* Pathname in [\sys.]$vol.subvol.file format */
 		cachepath[MIDLEN],	/* Complete pathname of cache file */
		gcachepath[MAXGUARDFILE], /* name in [\sys.]$vol.subvol.file format */
		query[MIDLEN],		/* Stuff after '?' in URL */
		param_field[SMALLLEN],	/* Stuff after ';' before '=' in URL */
		*param_value,		/* Stuff after '=' before '?' in URL */
		pathinfo[MIDLEN],	/* PATH_INFO for CGI */
		mod_date[SMALLLEN],	/* Last-Modified HTTP header line */
		authuser[SMALLLEN],     /* username with Basic auth */
		range[RANGELEN],	/* range from range header */
		etag[TINYLEN],		/* ETag in ASCII calculated locally */
		length[TINYLEN];	/* File length in ASCII */
		
	FILE	*fp;
	int	fptype;			/* FP_PIPE or FP_FILE */

	long	datalen,		/* Length of file as a long */
		logcount;		/* Bytes sent (for log) */

	time_t	mod_time;		/* File modification time */

	Reqtype	type;			/* RTYPE_FILE, RTYPE_CGI, etc. */

	unsigned	attributes,
			attrib2,
			status,
			filetype,

			cache_uid,
			cache_gid;

	int		do_wrap_1st_time;

} Request;


extern Request		*this_rp;


/* Bits in the Request attributes  and attrib2 are in common.h */

/* Bits in the request status */
#define	WN_CANT_STAT		(1)
#define	WN_NOT_WORLD_READ	(2)

#define WN_PROLOGSENT		(8)
#define WN_CGI_SET		(16)
#define WN_ABORTED		(32)
#define WN_ERROR		(64)

/* Bits in the Request filetype */
#define	WN_TEXT			(1)
#define	WN_DIR			(2)
#define WN_ISHTML		(4)
#define	WN_DEFAULT_DOC		(8)
#define	WN_IMAGEMAP		(16)
#define	WN_BYTERANGE		(32)
#define	WN_LINERANGE		(64)
#define	WN_RFC_BYTERANGE	(128)

typedef struct Dir_info {
	char	dirline[BIGLEN],
		*accessfile,
		*swrapper,
		*defincludes,
		*defwrapper,
		*deflist,
		*nomatchsub,
		*subdirs,
		*dir_owner,
		*cachemod,		/* Cache data base module */
		*filemod,		/* File data base  module */
		*indexmod,		/* Index search module  module */
		*authtype,		/* Type of authorization */
		*authrealm,		/* Realm for authorization */
		*authmod,		/* module to do authorization */
		*defdoc,		/* default document for this dir */
		*default_content,	/* default content type */
		*default_maxage,	/* default value of maxage  */
		*def_handler,		/* default CGI handler  */
		*def_filter,		/* default filter  */
		authmodule[MIDLEN],
		filemodule[MIDLEN],
		cachemodule[MIDLEN],
		indexmodule[MIDLEN],
		cantstat_url[MIDLEN/2],
		authdenied_file[MIDLEN/2],
		noaccess_url[MIDLEN/2];

	unsigned	attributes,
			defattributes;

} Dir_info;

extern Dir_info	*dir_p;

/* Bits in the Dir attributes */
#define	WN_DIRNOSEARCH	(1)
#define WN_DIRWRAPPED	(2)
#define WN_SERVEALL	(4)


typedef struct Cache_entry {
	char	*line,
		headerlines[BIGLEN],
		*basename,
		*title,
		*keywords,
		*field[NUMFIELDS],	/* user defined fields */
		*content,
		*encoding,	/* MIME content-transfer-encoding */
		*includes,	/* comma separated include files */
		*wrappers,	/* comma separated wrapper files */
		*list_incl,	/* comma separated include list */
		*swrapper,	/* comma separated search wrapper files */
		*nomatchsub,
		*filter,
		*handler,
		*maxage,                /* Maxage in ascii seconds */
		*expires,
		*dynamic,	/* doc should not be cached if "true" */
		*cgi,		/* Create CGI environment, "true" or "false" */
		*url,		/* URL link to remote object */
		*redirect,	/* URL link to redirected object */
		*end;

	unsigned	attributes,
			attrib2,
			filetype;
} Cache_entry;

typedef enum { 
	GET,
	CONDITIONAL_GET,
	POST,
	PUT,
	HEAD,
	UNKNOWN
} Methodtype;

typedef enum { 
	HTTP0_9,
	HTTP1_0,
	HTTP1_1
} Prottype;

typedef struct Inheader {
	char	accept[ACCEPTLEN],
		cookie[ACCEPTLEN],
		charset[ACCEPTLEN/4],
		lang[ACCEPTLEN/4],
		url_path[BIGLEN],
		content[SMALLLEN],
		encoding[SMALLLEN],	/* MIME content-transfer-encoding */
		length[SMALLLEN],
		referrer[MIDLEN],
		ua[SMALLLEN],
		from[SMALLLEN],
		host_head[SMALLLEN],
		authorization[MIDLEN],
		inmod_date[SMALLLEN],
		etag[SMALLLEN],
		range[RANGELEN],
		tmpfile[MIDLEN];

	Methodtype	method;
	Prottype	protocol;

	unsigned	conget;

} Inheader;

extern Inheader	*inheadp;


/* Bits in inheadp->conget */

#define	IFNMATCH	(1)
#define	IFMATCH		(2)
#define	IFMODSINCE	(4)
#define	IFUNMODSINCE	(8)
#define	IFRANGE		(16)



typedef struct Outheader {
	char	
		list[BIGLEN],
		redirect[MIDLEN],
		expires[SMALLLEN],
		range[SMALLLEN],
		status[SMALLLEN];
} Outheader;

extern Outheader	*outheadp;


typedef struct Inbuffer {
	char	buffer[INBUFFSIZE],
		*bcp;
	int	cur_sz;
} Inbuffer;


#define LOGBUFLEN	(BIGLEN + MIDLEN)
			/* must be bigger than BIGLEN+SMALLLEN */


typedef struct Connection {
	        int		pid,
			keepalive,	/* boolean */
			more_in_buf,	/* boolean */
			trans_cnt;

	char		logbuf[LOGBUFLEN],
			outbuf[OUT_BUFFSIZE],
			rfc931name[SMALLLEN],
			*out_ptr,
			*log_ptr,
			*scheme;	/* "http" or something else */

	long		bytecount;

	unsigned	con_status;

	Inbuffer	*bufp;
	int         sock;
} Connection;

extern Connection	*this_conp;

extern int auto_form_decoding;
extern int authenticate_users;
extern int nologbanner;
extern char snw2addr[64];
#ifdef _NSKOSS
extern char executablePath[PATH_MAX];
#else
extern char executablePath[40];
#endif
extern int cgi_errno;
extern int server_mode;
extern int pathway_server;
extern int pathway_reply_needed;
extern char pathmon[32];
extern char serverClass[32];
extern jmp_buf jump_environment;
extern int miniCgiDebug;
extern int transaction_timeout;

extern unsigned	serv_perm;

/* Bits in serv_perm */
#define	WN_TRUSTED_UID	(1)
#define	WN_TRUSTED_GID	(2)
#define	WN_FORBID_EXEC	(4)
#define	WN_RESTRICT_EXEC	(8)
#define	WN_COMP_UID	(16)
#define	WN_COMP_GID	(32)
#define	WN_ATRUSTED_UID	(64)
#define	WN_ATRUSTED_GID	(128)





int strncasecmp( char *s1, char *s2, int n);
int strcasecmp( char *s1, char *s2);

#define streq( a, b)	( strcmp( (a), (b)) == 0 )
#define iswndir( x)	( x->filetype & WN_DIR  )
#define isdirwrapped( x)	( x->attributes & WN_DIRWRAPPED  )

unsigned int alarm (unsigned int seconds);
void sendcgi( Request	*ip);
void cgi_env( Request	*ip, int	auth);
static void cgi_error (char * msg);
char *util_getvar(char * varname);
void doHttpHeaders (const char * buf);
void CGI_internal_init(Request   *ip);
void CGI_Capture(void);
int CGI_puts (const char * string);
int CGI_getc(FILE * stream);
int CGI_printf(const char *format, ...);
int CGI_fflush(FILE * stream);
void ErrorAbort (void);
int CGI_set_fflush_timer(int seconds);
static void decode64( char	*bufcoded, char *out);
int chkauth( Request	*ip);
static void sendauth( Request	*ip, char	*noncearg, char *logmsg);
int checkUser (char * user, char * password, char * msg);
void wn_init( int	argc, char	*argv[]);
void close_wnlog ();
int open_wnlog(void);
void logerr( char	*msg, char *msg2);
void daemon_logerr( char	*msg, char * msg2, int	error);
void writelog( Request	*ip, char	*msg, char *msg2);
void write_debug( int	n, char	*msg, char *msg2);
static void log_logfile( Request	*ip, char	*msg,char  *msg2);
static void append_gmtoff( char	*date, struct tm *ltm);
static void start_log( int	restart);
void wn_exit( int	status);
static void dump_log();
int date_cmp( Request	*ip, char	*ext_date, int	ims);
static int getnum( char	*s, int	n);
static int mon2int( char	*month);
void parse_request(  Request	*ip, char	*url_path);
void get_stat( Request	*ip);
void set_etag( struct stat *sbp);
static void parse_cgi( Request	*ip, char	*path);
static void path_security( Request	*ip, char	*path, int	is_pathinfo);
static int dedot( Request	*ip, char	*path);
int getfpath( char	*path, char *fname, Request	*ip);
int getfpath2( char	*path, char *fname, char *cachepath);
void set_interface_root(void);
static FILE *fsocket( int     domain, int     type, int     protocol);
static void timeout(int sig);
void    rfc931( struct sockaddr_in *rmt_sin, struct sockaddr_in *our_sin, char   *dest);
void senderr( char	*status, char *msg, char *file);
void sendinfo( Request	*ip);
void send_text_line( char	*line);
void sendredirect( Request	*ip, char	*status, char *location);
void send204( Request	*ip);
void send_out_mem( char	*buf, int	len);
void flush_outbuf( );
void do_connection(int sock);
static void client_closed( );
void process_url( Request	*ip, char	*url_path);
static Methodtype parse_header( char	*req);
static void get_header( Inheader	*ih);
char * get_input( char	*line, int	is_rline);
static int load_inbuf( Inbuffer	*bp);
static int chk_continue( Inbuffer	*bp);
static int reset_buf( Inbuffer	*bp);
void clear_req( );
int http_prolog( );
/****Changed declaration of sock to int to avoid warning 26/7/2000***/
int cgilib_tcpwait (int sock, long * tag, long timelimit);
/*******************************************************************/
void filepathToGuardian (char *guardian, char * filepath);
static int      fileExternalToMsg (char *Pexternal, char *PmsgFormat);
int unispawn (char **argv, char * sockargv);
char * strlower (char * s);
void get_remote_ip( );
void get_remote_info( );
void get_rfc931();
char * mystrncpy( char	*s1, char *s2, int	n);
char * mystrncat( char	*s1, char *s2,  int	n);
char * mymemcpy( char	*p1, char *p2, int n);
int chop( char *line);
int amperline ( char	*p1, char *p2);
void get_local_info( int	sockdes);
/*****changed initgroups declaration on upgradation to y24 compilers ****/
int initgroups( const char	*gp_name, gid_t	group_id);
/***********************************************************************/
char * T9999D43_25Mar98_swn_JavaScript (void);
void www_unescape( char	*path, char pluschar);
void do_standalone(char *argv[]);

