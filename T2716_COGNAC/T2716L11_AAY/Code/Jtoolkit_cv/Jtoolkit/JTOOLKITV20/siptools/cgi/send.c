/*
    Wn: A Server for the HTTP
    File: wn/send.c
    Version 1.17.10
    
    Copyright (C) 1996-7  <by John Franks>

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

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <sys/socket.h>
#include <cextdecs.h(DEBUG)>

#define _CGI_NO_EXTERNALS
#include "cgilib.h"

#include "wn.h"
#include "version.h"

#ifndef USE_WN_WRITE
#define WN_write(a,b,c)		write(a,b,c)
#endif

#define BYTECHUNK	(256*1024)
#define MAX_REDIRECT	(10)


extern long	atol();


void senderr( char	*status, char *msg, char *file)
{
	char	buf[MIDLEN];
	int	prolog_sent;

	get_remote_info( );

	sprintf( outheadp->status, "%s %s", status, msg);
	strcpy( outheadp->list, "Content-type: text/html\r\n");

	if ( streq( status, SERV_ERR))
		logerr( msg, file);
	else
		writelog( this_rp, msg, file);
 	/****Type casted to avoid warning-26/7/2000******/
	prolog_sent = (int)this_rp->status & WN_PROLOGSENT;
	/************************************************/
	clear_req( );

	this_rp->status |= prolog_sent;
	if ( !prolog_sent) {
		set_interface_root( );

		sprintf( buf,
		"<html>\n<head>\n<title>%s %s</title>\n</head>\n<body>\n",
					status, msg);
		sprintf( buf + strlen( buf),
				"<h2>Error code %s</h2>\n%s\n", status, msg);
		sprintf( buf + strlen( buf),
			"\n<hr>\n<address>%s</address>\n", VERSION);
		sprintf( buf + strlen(buf), "\n</body>\n</html>\n");
		sprintf( this_rp->length, "%d", strlen(buf));

		http_prolog( );
		send_text_line( buf);
	}

	this_rp->status |= WN_ERROR;
	this_rp->type = RTYPE_FINISHED;

}


void sendinfo( Request	*ip)

{
	char	*cp,
		owner[MIDLEN],
		len[TINYLEN],
		con[SMALLLEN],
		enc[SMALLLEN],
		buf[2*BIGLEN];

	int	i;

	struct tm *gmt;


	if ( *ip->length) {
		mystrncpy( len, ip->length, TINYLEN);
		*ip->length = '\0';
	}
	
        if( ip->encoding != NULL) {
              mystrncpy( enc, ip->encoding, SMALLLEN);
	      *ip->encoding = '\0';
 	}
      
        if( ip->content_type != NULL)
	      mystrncpy( con, ip->content_type, SMALLLEN);
	ip->content_type = "text/html";
      	

	http_prolog( );

	sprintf(buf, "<html>\n<head>\n<title>URL information </title>\n");
	send_text_line( buf);
	cp = MAINTAINER;
	sprintf( owner, "<link rev=\"made\" href=\"%s\">\n", cp);
	sprintf(buf,"%.256s</head>\n<body>\n<h2>URL information</h2>\n",
					owner);
	send_text_line( buf);

	sprintf(buf, "<dl>\n<dt><b>Title:</b>\n<dd>%.256s\n", ip->title);
	send_text_line( buf);

	sprintf(buf, "<dt><b> Filename:</b>\n<dd>%.128s\n", ip->basename);
	send_text_line( buf);

	if ( ip->keywords != NULL) {
		sprintf(buf, "<dt><b>Keywords:</b>\n<dd>%.1024s\n",
						ip->keywords);
		send_text_line( buf);
	}
	for ( i = 0; i < NUMFIELDS; i++) {
		if ( ip->field[i] != NULL) {
			sprintf(buf,
				"<dt><b>User field %d:</b>\n<dd>%.1024s\n",
							i, ip->field[i]);
			send_text_line( buf);
		}
	}
	if ( ip->expires && *ip->expires) {
		sprintf(buf, "<dt><b>Expires:</b>\n<dd>%.64s\n", ip->expires);
		send_text_line( buf);
	}

	if ( *len ) {
		sprintf(buf, "<dt><b>Size:</b>\n<dd>%s\n", len);
		send_text_line( buf);
	}
	sprintf(buf, "<dt><b>Content-type:</b>\n<dd>%.128s\n", con);
	send_text_line( buf);

	if ( *enc) {
		sprintf(buf, "<dt><b>Content-encoding:</b>\n<dd>%.128s\n",enc);
		send_text_line( buf);
	}

	if ( ip->mod_time != NULL) {
		gmt = gmtime(&ip->mod_time);
		strftime( buf, SMALLLEN,
		"<dt><b>Last-modified:</b>\n<dd> %a, %d %h %Y %T GMT\n", gmt);
		send_text_line( buf);
	}

	sprintf(buf, "<dt><b>Maintainer:</b>\n<dd>%.256s\n", cp);
	send_text_line( buf);

	sprintf(buf, "</dl>\n<hr>\n<address>%s</address>\n", VERSION);
	send_text_line( buf);

	send_text_line( "\n</body>\n</html>\n");

	writelog( ip, log_m[14], ip->relpath);
}


void send_text_line( char	*line)
{
	/****Type casted to avoid warning-26/7/2000******/
	send_out_mem( line, (int)strlen(line));
	/********************************************/
}


void sendredirect( Request	*ip, char	*status, char *location)
{
	static int	num = 0;

	num++;
	if ( num > MAX_REDIRECT) {
		senderr( SERV_ERR, err_m[55], err_m[64]);
		wn_exit( 1);
	}

	if ( strncasecmp( location, "<null>", 6) == 0) {
		send204( ip);
		return;
	}

	if ( location != outheadp->redirect)
		mystrncpy( outheadp->redirect, location, MIDLEN);
	strcpy( outheadp->status, status);
	ip->content_type = ip->encoding = NULL;
	*ip->length = '\0';
	*outheadp->list = '\0';

	http_prolog( );

	writelog( ip, log_m[9], location);


	if ( ip->attributes & (WN_PARSE + WN_DYNAMIC + WN_FILTERED) )
		wn_exit(1);
	/*
	 * Exit since we aren't doing keepalive.  The problematic place
	 * for return is in dolocation call in parse.c
	 */

	/* If we are doing keepalive then return */
	return;
}

void send204( Request	*ip)
{
	strcpy( outheadp->status, "204 No Response");

	http_prolog();

	writelog( ip, log_m[16], "");
	if ( ip->attributes & (WN_PARSE + WN_DYNAMIC +WN_FILTERED) )
		wn_exit(0);
	/*
	 * We aren't doing keepalive.  The problematic place
	 * for return is in dolocation call in parse.c
	 */
	return;
}




void send_out_mem( char	*buf, int	len)
{
	register char	*cp,
			*cp2,
			*end;

	int		remlen;

	end = this_conp->outbuf + OUT_BUFFSIZE;
	cp = buf;

	this_rp->logcount += (long) len;
	while ( len > 0 ){
		cp2 = this_conp->out_ptr;
		if ( end > cp2 + len ) {
			/* it all fits in buffer */
			memcpy( cp2, cp, len);
			this_conp->out_ptr += len;
			if ( this_rp->attributes & WN_UNBUFFERED)
				flush_outbuf();
			return;
		}
		else {
			remlen = this_conp->outbuf
					+ OUT_BUFFSIZE - cp2;
			len -= remlen;
			memcpy( cp2, cp, remlen);
			cp += remlen;
			this_conp->out_ptr = end;
			flush_outbuf();
		}
	}
}


void flush_outbuf( )
{

	int		len,
			n;

	register char	*cp;

	cp = this_conp->outbuf;
	len = this_conp->out_ptr - cp;
	if ( len == 0 )
		return;
	while ( (n = send (this_conp->sock, cp, len, 0)) < len ) {
		if ( n == -1 && errno == EINTR) 
			continue;
		if ( n <= 0) {
			if ( n == -1 ) {
				char   buf[TINYLEN];
				cgi_errno = errno;
            /****** Changed code to handle stop button processing 30/6/2000*******/
		if (errno == EPIPE ||
                    errno == ENOTCONN ||
                    errno == EBADF ||
                    errno == ECONNRESET ||
      		    errno == ESHUTDOWN) 
	    /************************************************************/
		   { /* Error on client side or user pressed stop button */
			    sprintf( buf, "flush, errno = %d", errno);
			    logerr( err_m[75], buf);
                    }
                wn_exit (10);  /* djc 10/17/97  */
			}
			break;
		}
		len -= n;
		cp += n;
		this_conp->bytecount += n;
	}
	if ( n >= 0 ) {
		this_conp->bytecount += n;
	}

	if ( this_conp->bytecount >= BYTECHUNK) {
		alarm( transaction_timeout);
		this_conp->bytecount = 0L;
	}
	this_conp->out_ptr = this_conp->outbuf;
}

