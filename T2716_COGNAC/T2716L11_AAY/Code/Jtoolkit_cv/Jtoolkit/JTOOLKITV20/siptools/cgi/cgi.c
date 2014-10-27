/*
    Wn: A Server for the HTTP
    File: wn/cgi.c
    Version 1.17.12

    Copyright (C) 1996  <by John Franks>

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
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include "wn.h"
#include "version.h"
#include "cgi.h"

#define _CGI_NO_EXTERNALS
#include "cgilib.h"

extern char ** environ;

static CGI_data	*cgip = NULL;

char * get_input( char * line, int is_rline);

void CGI_internal_term(void);
void CGI_internal_init( Request   *ip);
int cgi_putenv (char *var);

#define DBGVAR(v) {if (debug_log) {char buf[256]; char loc[256]; \
                      sprintf(buf,"%s", (getenv(v)?getenv(v):"(null)")); \
                      sprintf (loc, "%s %ld", __FILE__, __LINE__); \
                      write_debug (1, loc, buf);  \
                      } \
                  }

static char * strdup (char *src) 
	{
	size_t size;
    char * dest;
	size = strlen (src);
	dest=(char *) malloc(size+2); 
	strcpy(dest,src);
	return dest;
	}

/*
 * sendcgi( ip)  Open pipe from "ip->filepath" command
 * and send output using CGI standards
 */

void sendcgi( Request	*ip)
{
    int i;
	register char	*cp;
    static char firstTime = 1;

	char	command[MIDLEN],
		location[MIDLEN],
		cgibuf[OUT_BUFFSIZE + 4],
		buf[BIGLEN],
		*bufptr;

    DBGVAR("QUERY_STRING");

	location[0] = '\0';
	/*** Commented code to get correct content length-24/7/2000***/
	/* *ip->length = '\0';*/
	/**********************************************************/
	
	/* if ( NOT CHUNKING ) */
	/***Commented code to enble Keep-Alive-20/7/2000*****/
	/* this_conp->keepalive = FALSE; */ 
	/****************************************************/

		mystrncpy( command, ip->filepath, MIDLEN - SMALLLEN);

	mystrncpy( buf, ip->filepath, MIDLEN);
	cp = strrchr( buf, '/');
	/*******Code modified to prevent ABEND - 19/6/2000 **********/
        if (cp != NULL)
        	*cp = '\0';
	/*****************************/

    {
    char * argv[1];

    DBGVAR("QUERY_STRING");
    CGI_internal_init(ip);
    DBGVAR("QUERY_STRING");

    if (firstTime)
	    (void) CGI_initialize();

#ifdef _NSKOSS
    argv[0] = ip->filepath;
#else
    if (rootdir[0] == '/')  /* djc oct 24/98 */
       argv[0] = ip->filepath;
    else
       argv[0] = ip->gname;
#endif
    DBGVAR("QUERY_STRING");
    (void) CGI_main (1, argv);
    if (firstTime && pathway_server == 0)
	     (void) CGI_terminate();

    firstTime = 0;

    writelog( ip, log_m[8], ip->filepath);  

    DBGVAR("QUERY_STRING");
    CGI_internal_term (); 
    DBGVAR("QUERY_STRING");
    }

}


/*
 * cgi_env( ip, auth) Create environment variables required for CGI.
 * Also WN_ROOT and WN_DIR_PATH.  If auth = TRUE then only do a
 * small subset of the variables for authentication.
 */

void cgi_env( Request	*ip, int	auth)
{
	char	*cp;

	if ( ip->status & WN_CGI_SET)
		return;		/* CGI environment vars already set */

	if ( cgip == NULL) {
		if ((cgip = (CGI_data *) malloc(sizeof (CGI_data))) == NULL ) {
			logerr( err_m[64], "cgi_env");
			return;
		}
	}
	if (pathway_server)
		memset ((char *) cgip, 0, sizeof (CGI_data));

    DBGVAR("QUERY_STRING");
	strcpy( cgip->method, "REQUEST_METHOD=");
	switch ( inheadp->method) {
	case GET:
		strcat( cgip->method, "GET");
		break;
	case POST:
		strcat( cgip->method, "POST");
		strcpy( cgip->postfile, "HTTP_POST_FILE=");
		mystrncat( cgip->postfile, inheadp->tmpfile, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->postfile)); else putenv( cgip->postfile);
		break;
	case PUT:
		strcat( cgip->method, "PUT");
		strcpy( cgip->postfile, "HTTP_PUT_FILE=");
		mystrncat( cgip->postfile, inheadp->tmpfile, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->postfile)); else putenv( cgip->postfile);
		break;
	default:
		break;
	}
	if (pathway_server) cgi_putenv(strdup( cgip->method)); else putenv( cgip->method);

	strcpy( cgip->dirpath, "WN_DIR_PATH=");
	mystrncat( cgip->dirpath, ip->cachepath, SMALLLEN);
	if ( (cp = strrchr( cgip->dirpath, '/')) != NULL )
		*cp = '\0';
	if (pathway_server) cgi_putenv(strdup( cgip->dirpath)); else putenv( cgip->dirpath);

	if ( *(ip->authuser)) {
		strcpy( cgip->ruser, "REMOTE_USER=");
		mystrncat( cgip->ruser, ip->authuser, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->ruser)); else putenv( cgip->ruser);
	}

	if ( *(inheadp->host_head) ) {
		strcpy( cgip->http_myhost, "HTTP_HOST=");
		mystrncat( cgip->http_myhost, inheadp->host_head, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_myhost)); else putenv( cgip->http_myhost);
	}

    if ( authenticate_users) {
		strcpy( cgip->authtype, "AUTH_TYPE=");
		mystrncat( cgip->authtype, "Basic", TINYLEN - 10);
        }

	strcpy( cgip->lochost, "SERVER_NAME=");
	strcat( cgip->lochost, hostname);
	if (pathway_server) cgi_putenv(strdup( cgip->lochost)); else putenv( cgip->lochost);

	strcpy( cgip->dataroot, "WN_ROOT=");
	strcat( cgip->dataroot, ip->rootdir);
	if (pathway_server) cgi_putenv(strdup( cgip->dataroot)); else putenv( cgip->dataroot);

	if ( !(this_conp->con_status & WN_CON_CGI_SET)) {
		strcpy( cgip->raddr, "REMOTE_ADDR=");
		mystrncat( cgip->raddr, remaddr, TINYLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->raddr)); else putenv( cgip->raddr);

		sprintf( cgip->lport,"SERVER_PORT=%d", port);
		if (pathway_server) cgi_putenv(strdup( cgip->lport)); else putenv( cgip->lport);
	}


	/* End of auth set of CGI variables */
	if ( auth)
		return;

	if ( !(this_conp->con_status & WN_CON_CGI_SET)) {

		this_conp->con_status |=  WN_CON_CGI_SET;

		if ( !*remotehost )
				/* Get remote hostname if not already done */
			get_remote_info( );

		strcpy( cgip->rhost, "REMOTE_HOST=");
		mystrncat( cgip->rhost, remotehost, MAXHOSTNAMELEN);
		if (pathway_server) cgi_putenv(strdup( cgip->rhost)); else putenv( cgip->rhost);

		sprintf( cgip->scheme,"URL_SCHEME=%s", this_conp->scheme);
		if (pathway_server) cgi_putenv(strdup( cgip->scheme)); else putenv( cgip->scheme);

		if (pathway_server) cgi_putenv(strdup("GATEWAY_INTERFACE=CGI/1.1")); else putenv("GATEWAY_INTERFACE=CGI/1.1");

		strcpy( cgip->servsoft, "SERVER_SOFTWARE=");
		strcat( cgip->servsoft, VERSION);
		if (pathway_server) cgi_putenv(strdup( cgip->servsoft)); else putenv( cgip->servsoft);

		if ( *(this_conp->rfc931name)) {
			strcpy( cgip->rident, "REMOTE_IDENT=");
			mystrncat( cgip->rident,
					this_conp->rfc931name, SMALLLEN);
			if (pathway_server) cgi_putenv(strdup( cgip->rident)); else putenv( cgip->rident);
		}
	}

	if ( *(ip->query)) {
        if (debug_log) write_debug (1,"cgi_env", ip->query);
        DBGVAR("QUERY_STRING");
		strcpy( cgip->query, "QUERY_STRING=");
		mystrncat( cgip->query, ip->query, MIDLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->query)); else putenv( cgip->query);
		DBGVAR("QUERY_STRING");
        if (debug_log) write_debug (1,"cgi_env", cgip->query);
	}

	strcpy( cgip->servsoft, "SERVER_SOFTWARE=");
	strcat( cgip->servsoft, VERSION);
	if (pathway_server) cgi_putenv(strdup( cgip->servsoft)); else putenv( cgip->servsoft);
		DBGVAR("QUERY_STRING");

	strcpy( cgip->serv_protocol, "SERVER_PROTOCOL=");
	switch ( inheadp->protocol) {
	case HTTP0_9:
		strcat(cgip->serv_protocol, "HTTP/0.9");
		break;
	case HTTP1_1:
		strcat(cgip->serv_protocol, "HTTP/1.1");
		break;
	default:
		strcat( cgip->serv_protocol, "HTTP/1.0");
		break;
	}

	if (pathway_server) cgi_putenv(strdup( cgip->serv_protocol)); else putenv( cgip->serv_protocol);
		DBGVAR("QUERY_STRING");

	if ( *(ip->pathinfo) ) {
		strcpy( cgip->pathinfo, "PATH_INFO=");
		mystrncat( cgip->pathinfo, ip->pathinfo, MIDLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->pathinfo)); else putenv( cgip->pathinfo);
	}
		DBGVAR("QUERY_STRING");

	strcpy( cgip->tpath, "PATH_TRANSLATED=");
	strcat( cgip->tpath, ip->rootdir);
	mystrncat( cgip->tpath,	ip->pathinfo, MIDLEN);
	if (pathway_server) cgi_putenv(strdup( cgip->tpath)); else putenv( cgip->tpath);
		DBGVAR("QUERY_STRING");

	strcpy( cgip->scrname, "SCRIPT_NAME=");
	if ( (ip->type == RTYPE_CGI_HANDLER) && (*ip->handler))
		mystrncat( cgip->scrname, ip->handler, MIDLEN - 20);
	else {
        if (ip->filetype & WN_DEFAULT_DOC) {
            strcat  (cgip->scrname, DEFAULT_URI);
            /*  dec 12/98 djc 
            if (rootdir[0] == '/') 
                if (strrchr (executablePath, (int) '/'))
                    strcat  (cgip->scrname, strrchr (executablePath, (int) '/')+1);
			    else
				    strcat  (cgip->scrname, executablePath);
            else
                strcat  (cgip->scrname, strrchr (executablePath, (int) '.')+1);
            */
            }
        else {
		    cp = ip->filepath + strlen( ip->rootdir);
		    mystrncat( cgip->scrname, cp, MIDLEN - 20);
            }
	}
	if (pathway_server) cgi_putenv(strdup( cgip->scrname)); else putenv( cgip->scrname);
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->content) ) {
		strcpy( cgip->content, "CONTENT_TYPE=");
		mystrncat( cgip->content, inheadp->content, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->content)); else putenv( cgip->content);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->length) ) {
		strcpy( cgip->length, "CONTENT_LENGTH=");
		mystrncat( cgip->length, inheadp->length, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->length)); else putenv( cgip->length);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->accept) ) {
		strcpy( cgip->http_accept, "HTTP_ACCEPT=");
		mystrncat( cgip->http_accept, inheadp->accept, ACCEPTLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_accept)); else putenv( cgip->http_accept);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->charset) ) {
		strcpy( cgip->http_charset, "HTTP_ACCEPT_CHARSET=");
		mystrncat( cgip->http_charset, inheadp->charset, ACCEPTLEN/4);
		if (pathway_server) cgi_putenv(strdup( cgip->http_charset)); else putenv( cgip->http_charset);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->lang) ) {
		strcpy( cgip->http_lang, "HTTP_ACCEPT_LANGUAGE=");
		mystrncat( cgip->http_lang, inheadp->lang, ACCEPTLEN/4);
		if (pathway_server) cgi_putenv(strdup( cgip->http_lang)); else putenv( cgip->http_lang);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->cookie) ) {
		strcpy( cgip->http_cookie, "HTTP_COOKIE=");
		mystrncat( cgip->http_cookie, inheadp->cookie, ACCEPTLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_cookie)); else putenv( cgip->http_cookie);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->range) ) {
		strcpy( cgip->range, "HTTP_RANGE=");
		mystrncat( cgip->range, inheadp->range, RANGELEN - TINYLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->range)); else putenv( cgip->range);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->referrer) ) {
		strcpy( cgip->http_referrer, "HTTP_REFERER=");
		mystrncat( cgip->http_referrer, inheadp->referrer, MIDLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_referrer)); else putenv( cgip->http_referrer);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->ua) ) {
		strcpy( cgip->http_ua, "HTTP_USER_AGENT=");
		mystrncat( cgip->http_ua, inheadp->ua, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_ua)); else putenv( cgip->http_ua);
	}
		DBGVAR("QUERY_STRING");

	if ( *(inheadp->from) ) {
		strcpy( cgip->http_from, "HTTP_FROM=");
		mystrncat( cgip->http_from, inheadp->from, SMALLLEN);
		if (pathway_server) cgi_putenv(strdup( cgip->http_from)); else putenv( cgip->http_from);
	}
		DBGVAR("QUERY_STRING");

	ip->status |= WN_CGI_SET;  /* mark environment as setup */
		DBGVAR("QUERY_STRING");
}

