/*
    Wn: A Server for the HTTP
    File: wn/wn.c
    Version 1.18.1
    
    Copyright (C) 1996-97  <by John Franks>

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

#include "config.h"

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cextdecs.h(DEBUG)>

#include <sys/socket.h>


#include "uniguard.h"
#include "wn.h"
#include "version.h"
#include "cgi.h"

#ifndef USE_WN_READ
#define WN_read(a,b,c)		read(a,b,c)
#endif

#define MAX_POST		(5 * 1024 * 1024)
#define KEEP_ALIVE_TIMEOUT	(20)
#define NETSCAPE2_BUG
#define AUTHENT_TIMEOUT		(300)
#define KEEP_ALIVE_MAX		(10)

#define AWAIT_REQUEST		(1)
#define AWAIT_HEADER		(2)
#define AWAIT_TRANSACTION	(3)

char * out_m[] = {
    /* 0 */ "Access denied, or file does not exist"
  };

extern long	atol();
extern void	parse_request(),
		write_debug();

extern time_t	time();

static void	do_post(),
		get_header(),
		wn_timeout(),
		client_closed();

static Methodtype parse_header( );
char	*get_input();
int wn_getc();

int chkauth( Request *ip);

static int	chk_continue(),
		reset_buf(),
		load_inbuf(),
		await_state,
		await_timeout = KEEP_ALIVE_TIMEOUT;

Request		*this_rp = NULL;

Inheader	*inheadp = NULL;
Outheader	*outheadp = NULL;
Dir_info	*dir_p = NULL;
Connection	*this_conp = NULL;

int miniCgiDebug = 0;
int		port = 0;

main( int	argc, char	*argv[])

{

#ifdef _NSKOSSH
    pid_t pid;
#endif

if (getenv("MINICGI^DEBUG") || getenv ("MINICGI_DEBUG") ||  miniCgiDebug) {
     miniCgiDebug = 1;
    DEBUG();
    }

	wn_init( argc, argv);

if (server_mode == 0 && pathway_server == 0) {
#ifdef _NSKOSSH
    /* fork off the daemon to give back shell prompt */
    pid = fork ();
    if (pid < 0) {
        daemon_logerr( err_m[116], "fork", errno);
        return 0;
        }
    if (pid != 0) 
        return 0;    /* parent quits */
    /* we're the child - a listener daemon */
	if (setsid() == -1 ) {
		daemon_logerr( err_m[30], "setsid", errno);
        return 0;
       }
#endif
#ifdef _TNS_X_TARGET
	signal( SIGQUIT, SIG_IGN);
	signal( SIGINT, SIG_IGN);
#else
#endif
    }

	do_standalone(argv);

	return 	( 0);
}


void do_connection(int sock)
{
	static char		request[BIGLEN];
	static Request		thisreq;
	static Inheader	inheader;
	static Outheader	outheader;
	static Connection	thiscon;
	static Dir_info	thisdir;

	static Inbuffer	readbuf;

	char		buf[20];

    if (pathway_server) {
        memset ((char*)request, 0, sizeof(request));
	    memset ((char*)&thisreq, 0, sizeof(thisreq));
	    memset ((char*)&inheader, 0, sizeof(inheader));
	    memset ((char*)&outheader, 0, sizeof(outheader));
	    memset ((char*)&thiscon, 0, sizeof(thiscon));
	    memset ((char*)&thisdir, 0, sizeof(thisdir));
        }
        
	thiscon.keepalive = FALSE;
	thiscon.trans_cnt = 0;
	thiscon.rfc931name[0] = thiscon.logbuf[0] = '\0';
	thiscon.pid = 0;
	readbuf.bcp = NULL;
	readbuf.cur_sz = 0;
	thiscon.bufp = &readbuf;
	this_rp = &thisreq;
	inheadp = &inheader;
	outheadp = &outheader;
	this_conp = &thiscon;
	this_conp->log_ptr = this_conp->logbuf;
	this_conp->out_ptr = this_conp->outbuf;
	this_conp->scheme = "http";
	this_conp->con_status = 0;
	this_conp->sock = sock;


	 /* Start a debug entry if debugging enabled. */

	if ( debug_log) {
		write_debug( 0, "\nRequest starting: ", VERSION);
	}

	get_remote_ip( );
#ifdef _TNS_X_TARGET
	signal( SIGALRM, wn_timeout );
#else
	signal( SIGTIMEOUT, wn_timeout );
#endif

	while ( this_conp->trans_cnt < KEEP_ALIVE_MAX) {
		alarm( await_timeout);
		this_conp->trans_cnt++;
		await_state = AWAIT_REQUEST;
		request[0] = '\0';
		clear_req( );
		memset( (char *) inheadp, 0, sizeof( Inheader));
		memset( (char *) outheadp, 0, sizeof( Outheader));

		if ( get_input( request, TRUE) == NULL )
			client_closed(); /* no one there? */

		if ( !*request) {
			/* extra CRLF at end of previous request ? */
			if (this_conp->keepalive == TRUE){
            /***11/7/2000- Fixed to correct Keep-alive functionality *****/
                           if(this_conp->more_in_buf==FALSE){
 				flush_outbuf();
                           }
           /************************************************************/		   
                       continue;
                       }
			else
				break;
		}

		await_state = AWAIT_HEADER;

		mystrncpy( thisreq.request, request, BIGLEN);

		if ( parse_header( request) == UNKNOWN ) {
			wn_exit( 0);
		}
		await_state = AWAIT_TRANSACTION;
		await_timeout = KEEP_ALIVE_TIMEOUT;
		/* await_timeout may have been set for authentication */
		/* reset it here */
		alarm( transaction_timeout);

#ifdef NO_KEEPALIVE
		this_conp->keepalive = FALSE;
#else
#ifdef NETSCAPE2_BUG
		if ( this_conp->keepalive &&
			(strncmp( inheadp->ua, "Mozilla/2.0", 11) == 0)) {
			this_conp->keepalive = FALSE;
		}
#endif
#endif
		if ( debug_log && this_conp->keepalive) {
			sprintf( buf, "pid = %d, count = %d\n",	this_conp->pid,
				this_conp->trans_cnt);
			write_debug( 1, "Keep-Alive: ", buf);
		}

		process_url(&thisreq, inheadp->url_path);

		if ( this_rp->status & WN_ABORTED)
			wn_exit( 2);

		if ( !(this_conp->keepalive)) {
			break;
			/* if not keepalive then exit */
		}

		if ( this_conp->more_in_buf )
			continue;
		else
			flush_outbuf();
	}
    alarm(0);

	wn_exit(0);
}

static void
wn_timeout( )
{
	char	buf[SMALLLEN];

	if ( await_state != AWAIT_REQUEST) {
		strcpy( outheadp->status, "408 Timed Out");
		sprintf( buf, "Process %d, await_state (%d) ",
			this_conp->pid, await_state);
		senderr( "408", err_m[60], buf);
	}
	wn_exit (1);
}



static void client_closed( )
{
	if ( debug_log ) {
		char	buf[SMALLLEN];
		sprintf( buf, log_m[19], this_conp->pid, await_state);
		write_debug( 1, buf, "");
	}
	if ( await_state != AWAIT_REQUEST) {
		*(this_rp->length) = '\0';
		get_remote_info();
		writelog( this_rp, log_m[18], (await_state == AWAIT_HEADER 
			? "header" : "transaction"));
	}
	wn_exit( 1);
}

void process_url( Request	*ip, char	*url_path)
{
	Reqtype	itemtype;
    int authenticated;

	/* These may need clearing if we came from redirect */
	ip->attributes = ip->filetype = ip->status = 0;

	parse_request( ip, url_path);

    authenticated = chkauth (ip); 
    if (!authenticated) 
        return;

	itemtype = ip->type;

	if ( (itemtype != RTYPE_DENIED) && (itemtype != RTYPE_NO_AUTH)
				&& (itemtype != RTYPE_FINISHED)) {
		switch (inheadp->method) {
		case HEAD:
			itemtype = RTYPE_HEADER;
			break;
		case CONDITIONAL_GET:
			if ( ip->attributes & WN_DYNAMIC) {
				inheadp->method = GET;
				break;
			}				
			inheadp->method = GET;

			if ( (inheadp->conget & IFMODSINCE)
				&& !date_cmp( ip, inheadp->inmod_date, TRUE))
				itemtype = RTYPE_NOT_MODIFIED;

			if ( (inheadp->conget & IFUNMODSINCE)
				&& date_cmp( ip, inheadp->inmod_date, TRUE)) {
				itemtype = RTYPE_PRECON_FAILED;
				break;
			}

			break;

		case POST:
            itemtype = RTYPE_CGI;
            break;
		case PUT:
			break;
		default:
			break;
		}
	}

	switch (itemtype) {
		case RTYPE_FINISHED:
			break;

		case RTYPE_CGI:
			sendcgi( ip);
			break;

		case RTYPE_INFO:
			sendinfo( ip);
			break;

		case RTYPE_HEADER:
	/***** Fixed code to process HEAD method 18/7/2000********/	
			http_prolog();
	        /************************************************/
			break;

		case RTYPE_REDIRECT:
			break;

		case RTYPE_NOT_MODIFIED:
			break;

		case RTYPE_NO_AUTH:
			break;
		case RTYPE_DENIED:
		case RTYPE_UNCHECKED:
		default:
			senderr( DENYSTATUS, out_m[0], ip->filepath);
			ip->type = RTYPE_FINISHED;
	}
	return;

	/*
	 * End of process_url()
	 * We come here after every error free transaction
	 * or to return above or perhaps exit for some parsed docs.
	 */
}


static Methodtype parse_header( char	*req)
{
	register char	*cp;
	char		errmsg[BIGLEN],
			method[SMALLLEN];
	Inheader	*ih;

	ih = inheadp;
	cp = req;
	while ( *cp && !isspace( *cp))
		cp++;
	*cp++ = '\0';
	mystrncpy( method, req, SMALLLEN);
	req = cp;
	while (  *req && isspace( *req))
		req++;
	cp = req;
	while (  *cp && !isspace( *cp))
		cp++;
	if ( *cp ) { /* There's more, check HTTP Version */
		*cp++ = '\0';
		while (  *cp && isspace( *cp))
			cp++;
	}

	if ( !*cp )
		ih->protocol =  HTTP0_9;
	else if ( strncasecmp( cp, "HTTP/1.1", 8) == 0 )
		/*  ih->protocol =  HTTP1_1;  */
		ih->protocol =  HTTP1_0;  /*  djc mar 6/98 */
	else
		ih->protocol =  HTTP1_0;


	mystrncpy( ih->url_path, req, BIGLEN);

	if ( streq( method, "GET")) {
		ih->method = GET;
		if ( ih->protocol != HTTP0_9 ) 
			get_header( ih);
		return	(ih->method);

	}
	if ( streq( method, "POST") ) {
		if (serv_perm & WN_FORBID_EXEC) {
			senderr( "501", errmsg, "");
			return	(ih->method = UNKNOWN);
		}
		else {
			get_header( ih);
			return	(ih->method = POST);
		}
	}
	if ( streq( method, "HEAD") ) {
		get_header( ih);
		return	(ih->method = HEAD);
	}
	else {
		mystrncpy( errmsg, method, SMALLLEN);
		strcat( errmsg, ": Method not implemented");
		senderr( "501", errmsg, "");
		return	(ih->method = UNKNOWN);
	}
}



static void get_header( Inheader	*ih)
{
	register char	*cp, *cp2;
	char	headerline[BIGLEN];
	int	acceptlen,
		cookielen;

	acceptlen = cookielen = 0;
	this_conp->keepalive = FALSE;

	while ( get_input( headerline, FALSE) != NULL) {
		if ( !*headerline)	/* Blank line, end of header */
			return;

		if ( strncasecmp( headerline, "Accept:", 7) == 0 ) {
			cp = headerline + 7;
			while ( isspace( *cp))
				cp++;
			if ( acceptlen > 0 )
				strcat( ih->accept, ", ");
			acceptlen += strlen( cp);
			if ( acceptlen > ACCEPTLEN)
				logerr( err_m[19], "");
			else {
				strcat( ih->accept, cp);
			}
			continue;
		}

		if ( strncasecmp( headerline, "Accept-Language:", 16) == 0 ) {
			cp = headerline + 16;
			while ( isspace( *cp))
				cp++;
			if ( strlen( cp) > ACCEPTLEN/4 )
				logerr( err_m[19], "");
			else {
				mystrncpy( ih->lang, cp, ACCEPTLEN/4);
			}
			continue;
		}

		if ( strncasecmp( headerline, "Accept-Charset:", 15) == 0 ) {
			cp = headerline + 15;
			while ( isspace( *cp))
				cp++;
			if ( strlen( cp) > ACCEPTLEN/4 )
				logerr( err_m[19], "");
			else {
				mystrncpy( ih->charset, cp, ACCEPTLEN/4);
			}
			continue;
		}

		if ( strncasecmp( headerline, "Cookie:", 7) == 0 ) {
			cp = headerline + 7;
			while ( isspace( *cp))
				cp++;
			if ( cookielen > 0 )
				strcat( ih->cookie, "; ");
			cookielen += strlen( cp);
			if ( cookielen > ACCEPTLEN )
				logerr( err_m[79], "");
			else {
				strcat( ih->cookie, cp);
			}
			continue;
		}

		if ( strncasecmp( headerline, "Authorization:", 14) == 0 ) {
			/* For digest use the URI from URI header */
			if ( ( cp = strstr( headerline, "uri=\"")) ||
					(cp = strstr( headerline, "URI=\""))) {
				cp2 = ih->url_path;
				cp += 5;
				while( *cp && ( *cp != '"'))
					*cp2++ = *cp++;
				*cp2 = '\0';
			}
			cp = headerline + 14;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->authorization, cp, MIDLEN);
			continue;
		}

		if ( strncasecmp( headerline, "Content-type:", 13) == 0 ) {
			cp = headerline + 13;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->content, cp, SMALLLEN);
			continue;
		}
		if ( strncasecmp( headerline, "Content-length:", 15) == 0 ) {
			cp = headerline + 15;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->length, cp, SMALLLEN);
			continue;
		}
		if ( strncasecmp( headerline, "Content-encoding:", 17) == 0 ) {
			cp =  headerline + 17;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->encoding, cp, SMALLLEN);
			continue;
		}
		if ( strncasecmp( headerline, "Host:", 5) == 0 ) {
			cp = headerline + 5;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->host_head,  cp, SMALLLEN);
			continue;
		}
		if ( strncasecmp( headerline, "Referer:", 8) == 0 ) {
			cp = headerline + 8;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->referrer,  cp, MIDLEN);
			continue;
		}
		if ( strncasecmp( headerline, 
				 "Connection: Keep-Alive", 22) == 0 ) {
			this_conp->keepalive = TRUE;
			/* this_conp->keepalive = FALSE;  */  /* djc mar 5/98 */
			continue;
		}
		if ( strncasecmp( headerline,
                                 "Connection: Close", 17) == 0 ) {
                        this_conp->keepalive = FALSE;
                        continue;
                }
                if ( strncasecmp( headerline, "from:", 5) == 0 ) {
			cp = headerline + 5;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->from,  cp, SMALLLEN);
			continue;
		}

		if ( strncasecmp( headerline, "Range:", 6) == 0 ) {
			cp = headerline + 6;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->range,  cp, RANGELEN);
			continue;
		}

		if ( strncasecmp( headerline, "User-Agent:", 11) == 0 ) {
			cp = headerline + 11;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->ua, cp, SMALLLEN); 
			continue;
		}
		if ( strncasecmp( headerline, "If-Modified-Since:", 18) == 0) {
						/* it's a Fielding wart */
			if ( ih->method == POST)
				continue;
			cp = headerline + 18;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->inmod_date, cp, SMALLLEN);
			ih->method = CONDITIONAL_GET;
			ih->conget |= IFMODSINCE;
			continue;
		}

		if ( strncasecmp( headerline, "If-Unmodified-Since:", 20)
									== 0) {
						/* another Fielding wart */
			if ( ih->method == POST)
				continue;
			cp = headerline + 20;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->inmod_date, cp, SMALLLEN);
			ih->method = CONDITIONAL_GET;
			ih->conget |= IFUNMODSINCE;
			continue;
		}

		if ( strncasecmp( headerline, "If-None-Match:", 14) == 0) {
			if ( ih->method == POST)
				continue;
			cp = headerline + 14;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->etag, cp, SMALLLEN);
			ih->conget |= IFNMATCH;
			ih->method = CONDITIONAL_GET;
			continue;
		}

		if ( strncasecmp( headerline, "If-Match:", 9) == 0) {
			if ( ih->method == POST)
				continue;
			cp = headerline + 9;
			while ( isspace( *cp))
				cp++;
			mystrncpy( ih->etag, cp, SMALLLEN);
			ih->conget |= IFMATCH;
			ih->method = CONDITIONAL_GET;
			continue;
		}

		if ( strncasecmp( headerline, "If-Range:", 9) == 0) {
			if ( ih->method == POST)
				continue;
			cp = headerline + 9;
			while ( isspace( *cp))
				cp++;
			if ( strchr( cp, ':') != NULL )   /* It's a date */
				mystrncpy( ih->inmod_date, cp, SMALLLEN);
			else	/* It's an ETag */
				mystrncpy( ih->etag, cp, SMALLLEN);
			ih->conget |= IFRANGE;
			ih->method = CONDITIONAL_GET;
			continue;
		}

	}
}



int
wn_getc()
{
	int		n,
			c;

	Inbuffer	*bp;

	bp = this_conp->bufp;

	if ( bp->bcp >= bp->buffer + bp->cur_sz) {
		n = load_inbuf( bp);
		if ( n <= 0 )
			return EOF;
	}
	c = (unsigned char) *(bp->bcp);
	(bp->bcp)++;
	return c;
}


/*
 * static char *get_input()
 * Returns NULL if no input (client quit), otherwise places next line
 * of input in "line".  Any CRLF are removed.  Thus *line == '\0' indicates
 * end of headers.  This function reads lines from a buffer thiscon.bufp.
 * If the buffer is empty or does not contain a complete line it reloads
 * it.  It looks ahead to see if the line continues (i.e. next line starts
 * with white space.  The lookahead is only done for header lines
 * (rline = FALSE) not for the request line.
 * The only possibilities for "line" are request and headerline.
 * Both have size BIGLEN.
 */

char * get_input( char	*line, int	is_rline)
{
	int	llen = 0,
		n = 0;
	Inbuffer	*bp;

	register char	*cp;

	bp = this_conp->bufp;
	if ( bp->bcp == NULL)
		bp->bcp = bp->buffer;

	*line = '\0';
	while ( TRUE) {
		if (bp->cur_sz <= 0) {
                        /* fill the buffer */
			n = load_inbuf( bp);
			if ( n <= 0) {
				if ( debug_log) {
					char	tmpbuf[SMALLLEN];
					sprintf( tmpbuf, log_m[20],
							this_conp->pid, n);
					write_debug(1, tmpbuf, "");
				}
				return NULL;
			}
		}

		if ( ( cp = strchr( bp->bcp, '\n')) == NULL) {
			/* an incomplete header has been read */
			/* put what there is into line */
			/* if header contains nulls, stuff will be discarded */

			bp->buffer[bp->cur_sz] = '\0';
			mystrncat( line, bp->bcp, BIGLEN - llen);
		/****Type casted to avoid warning-26/7/2000******/	
			llen = (int)strlen( line);
		/***********************************************/
                        bp->cur_sz = 0;
			bp->bcp = bp->buffer;
			bp->buffer[0] ='\0';
                        /* then read more */
			
                        n = load_inbuf( bp);
			if ( n <= 0 ) {
				writelog( this_rp,  err_m[66], line);
				wn_exit( 2);
			}
			continue;
		}
		
		/* cp now points to next NL in the in_bufer */
		*cp++ = '\0';

		mystrncpy( line + llen, bp->bcp, BIGLEN - llen);
		/****Type casted to avoid warning-26/7/2000******/
		llen = (int)strlen( line);
		/***********************************************/

		if ( (llen > 0) && (line[llen-1] == '\r'))
			line[llen-1] = '\0';

		bp->bcp = cp;

		this_conp->more_in_buf = reset_buf (bp);

		if ( debug_log)
			write_debug(1, " -> ", line);

		if ( !*line || is_rline)
			return ( line);
			/* Don't allow continuation for request line */


		if ( ! chk_continue( bp) ) 
			break; /* We're done with this line */
			/* otherwise its a continuation line */
	}
	return ( line);
}



/*
 * static int load_inbuf( bp)
 * Load and adjust input buffer.
 */

static int load_inbuf( Inbuffer	*bp)
{
		char	*base;
		int	n = 0,
			size;

		register char	*cp;

		size = bp->cur_sz;
		base = &(bp->buffer)[0];
		cp = bp->bcp;

		if (cp > base) {
			size = ( base + size - cp);
			size = ( size < 0 ? 0 : size);
			mymemcpy( base, cp, size);
			bp->bcp = cp = base;
		}
		n = recv (this_conp->sock, (base + size), 
				((INBUFFSIZE - 4) - size), 0);
		if (n==-1) {
		    cgi_errno = errno;
            }
		if ( n > 0 ) {
			size += n;
			bp->cur_sz = size;
			*(base + size) = '\0';
		}
		else if ( size <= 0) {
			bp->cur_sz = 0;
			bp->bcp = bp->buffer;
			*base ='\0';
		}
		return n;
}


/*
 * static int chk_continue( bp)
 * Check if input line continues (next line starts with whitespace.
 * If buffer is empty, reinitialize it.
 */

static int chk_continue( Inbuffer	*bp)
{
	char	c;
	int	n;

	if ( bp->cur_sz == 0) {
		n = load_inbuf( bp);
		if ( n <= 0) {
			return FALSE;
		}
	}
	c = *(bp->bcp);
	if ( ( c != ' ') && (c != '\t'))
		return FALSE;
	else
		return TRUE;
}


/*
 * static void reset_buf( bp)
 * Check if buffer is empty and reset it.
 * Return FALSE if empty and TRUE otherwise.
 */

static int reset_buf( Inbuffer	*bp)
{

	if ( bp->bcp - bp->buffer >= bp->cur_sz) {
		/* it's empty */
		bp->bcp = bp->buffer;
		*(bp->bcp) = '\0';
		bp->cur_sz = 0;
		return FALSE;
	}
	else
		return TRUE;  /* it's not empty yet */

}


void clear_req( )
{
	memset( (char *) this_rp, 0, sizeof( Request));
	this_rp->do_wrap_1st_time = TRUE;
}

/*
 * http_prolog() sends the HTTP headers (or does nothing for HTTP/0.9)
 * If it has already been called for this request it returns FALSE
 * and does nothing.  If it has not been called it returns TRUE after
 * writing appropriate headers to stdout.
 */

int http_prolog( )
{

	struct tm	*gmt;
	time_t		clock,
			clock2;
	Request		*ip;
	char		buf[CACHELINE_LEN],
			datebuf[2*TINYLEN];
	unsigned	unbuffered;


	ip = this_rp;
	if ( ip->status & WN_PROLOGSENT )
		return FALSE;
	ip->status |= WN_PROLOGSENT;
	unbuffered = ip->attributes & WN_UNBUFFERED;
	ip->attributes &= ~(WN_UNBUFFERED);  /* always buffer HTTP headers */

	if ( inheadp->protocol ==  HTTP0_9) {
		ip->attributes |= unbuffered;
		return TRUE;
	}

	if ( *outheadp->status)
		sprintf( buf, "%s %s\r\n",
				HTTPVERSION, outheadp->status);
	else {
		strcpy( buf, HTTPVERSION);
		strcat( buf, " 200 OK\r\n");
	}
	send_text_line( buf);
	sprintf( buf, "Server: %s\r\n", VERSION);
	send_text_line( buf);
		/* Find date and serve the HTTP Date header */
	time(&clock);
	gmt = gmtime(&clock);
	strftime( datebuf, SMALLLEN, 
			"Date: %a, %d %h %Y %T GMT\r\n", gmt);
	send_text_line( datebuf);
	if ( strncmp( outheadp->status, "204", 3) == 0 ) {
		send_text_line( "Content-length: 0\r\n\r\n");
		return TRUE;
	}

	if ( strncmp( outheadp->status, "401", 3) == 0 ) {
		send_text_line( outheadp->list);
		if ( this_conp->keepalive)
			send_text_line( "Connection: Keep-Alive\r\n");
		await_timeout = AUTHENT_TIMEOUT;
		sprintf( buf, "Content-length: %s\r\n\r\n", ip->length);
		send_text_line( buf);
		return TRUE;
	}

	if ( *outheadp->redirect) {
		send_text_line( "Content-type: text/html\r\n");
		if ( this_conp->keepalive)
			send_text_line( "Connection: Keep-Alive\r\n");
		send_text_line( "Location: ");
		send_text_line( outheadp->redirect);
		send_text_line( "\r\nContent-length: 0\r\n\r\n");
		return TRUE;
	}

	if ( ip->attributes & (WN_DYNAMIC)) {
		ip->mod_time = (time_t) 0;
		*ip->mod_date = '\0';
		*ip->length = '\0';
	}

	if ( ip->attributes & (WN_NOCACHE)) {
		send_text_line( "Pragma: no-cache\r\n");
		if ( inheadp->protocol ==  HTTP1_1)
			send_text_line( "Cache-control: no-cache\r\n");
	}

	if ( ip->mod_time) {
		gmt = gmtime(&ip->mod_time);
		strftime( ip->mod_date, SMALLLEN,
				"Last-modified: %a, %d %h %Y %T GMT\r\n", gmt);

		if ( (inheadp->protocol ==  HTTP1_1) &&
					((clock - ip->mod_time) > 2))  {
			sprintf( buf, "ETag: \"%s\"\r\n", ip->etag);
			send_text_line( buf);
		}
	}

	if ( *ip->mod_date) {
		send_text_line( ip->mod_date);
	}

	if ( ip->maxage && *ip->maxage) {
		long	delta;

		if ( *ip->maxage == 'L') {
			ip->maxage++;
			clock2 = ip->mod_time + atol( ip->maxage);
			delta = clock2 - clock;

			if ( delta > 0) {
				sprintf( buf, "Cache-Control: max-age=%ld\r\n",
							 delta);
				send_text_line( buf);
			}
		}
		else {
			clock2 = clock + atol( ip->maxage);
			sprintf( buf, "Cache-Control: max-age=%s\r\n",
					ip->maxage);
			send_text_line( buf);
		}
	}

	if ( ip->expires && *ip->expires) {
		sprintf( buf, "Expires: %s\r\n", ip->expires);
		send_text_line( buf);
	}
	else if ( *outheadp->expires) {
		send_text_line( outheadp->expires);
	}
	else if ( ip->maxage && *ip->maxage) {
		gmt = gmtime(&clock2);
		strftime( datebuf, SMALLLEN, 
			"Expires: %a, %d %h %Y %T GMT\r\n", gmt);
		send_text_line( datebuf);
	}

	if ( inheadp->method == CONDITIONAL_GET) {
		/* It's not modified */
		if ( this_conp->keepalive)
			send_text_line( "Connection: Keep-Alive\r\n");
		send_text_line( "\r\n");
		return TRUE;
	}

	if ( ip->content_type) {
		sprintf(buf, "Content-type: %s\r\n", ip->content_type);
		send_text_line( buf);
	}

	if (*ip->length && !(ip->attributes 
				& (WN_PARSE + WN_FILTERED + WN_DYNAMIC))) {
		sprintf( buf, "Content-length: %s\r\n", ip->length);
		send_text_line( buf);
		if ( *outheadp->range )
			send_text_line( outheadp->range);

		if ( !(ip->filetype & WN_BYTERANGE) && !*(outheadp->status)) {
			/* !*(outheadp->status)) means status 200 */
			send_text_line( "Accept-Ranges: bytes\r\n"); 
		}
	}
	else if (inheadp->method != HEAD)
		this_conp->keepalive = FALSE;

	if ( this_conp->keepalive) {
                send_text_line( "Connection: Keep-Alive\r\n");
        }
        else{           
                send_text_line( "Connection: Close\r\n");
        }
	
	if ( ip->encoding && *ip->encoding) {
		sprintf( buf, "Content-encoding: %s\r\n",
			ip->encoding);
		send_text_line( buf);
	}

	if ( *outheadp->list ) {
		send_text_line( outheadp->list);
	}
	if ( inheadp->method == HEAD)
		send_text_line( "\r\n");

	ip->attributes |= unbuffered;
	return TRUE;
}

