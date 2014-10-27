/*
    Wn: A Server for the HTTP
    File: wn/chkauth.c
    Version 1.17.9

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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cextdecs.h(USER_AUTHENTICATE_)>

#include "wn.h"
#include "auth.h"

int checkUser (char * user, char * password, char * msg); 

static void	sendauth(),
		decode64();

static int	send_noauth();

static
WN_CONST
short int tr[128]={
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,
    28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    -1,-1,-1,-1,-1
};



static void decode64( char	*bufcoded, char *out)
{

	register char	*in;
	char		buf[SMALLLEN + TINYLEN];
    
	
	while( isspace(*bufcoded))
		bufcoded++;

	mystrncpy( buf, bufcoded, SMALLLEN);
	in = buf;

	while( *in && (tr[*in &= 0177] >= 0))
		in++;
	*in++ = 0;
	*in++ = 0;
	*in++ = 0;
	*in = 0;
    
	in = buf;
    
	while ( in[3] ) {
        	*out++ = (unsigned char) (tr[in[0]] << 2 | tr[in[1]] >> 4);
	        *out++ = (unsigned char) (tr[in[1]] << 4 | tr[in[2]] >> 2);
        	*out++ = (unsigned char) (tr[in[2]] << 6 | tr[in[3]]);
		in += 4;
	}
	
    	if ( in[2] ) {
        	*out++ = (unsigned char) (tr[in[0]] << 2 | tr[in[1]] >> 4);
	        *out++ = (unsigned char) (tr[in[1]] << 4 | tr[in[2]] >> 2);
        	*out++ = (unsigned char) (tr[in[2]] << 6);
	}
	else if ( in[1]) {
        	*out++ = (unsigned char) (tr[in[0]] << 2 | tr[in[1]] >> 4);
	        *out++ = (unsigned char) (tr[in[1]] << 4);
	}
	else if ( in[0] ) {
        	*out++ = (unsigned char) (tr[in[0]] << 2);
	}
	*out = '\0';
}


/*
 * chkauth( ip) check whether authorization is in use and whether the
 * client is authenticated.
 */

int chkauth( Request	*ip)
{
	register char	*cp,
			*cp2;

    char * password;

	char	*authheadp = NULL,
		buf[SMALLLEN];
    char msg[256];
	int	result;
    static char logonVar[] = "MINICGI_FORCE_LOGON=";
    static char logoffVar[] = "MINICGI_FORCE_LOGOFF=1";
    static char forceUserId[128];
    char * pLogon;
    char * pLogoff;

    if (authenticate_users == 0)
    {

        /*************   06/06/2000  **********************/ 
        if(debug_log)
	      write_debug(1, "chkauth "," Authentication not required ");
	/*************               **********************/
        return TRUE;
    }

	/*************   06/06/2000  **********************/ 
        if(debug_log)
	      write_debug(1, "chkauth "," Authentication required ");
	/*************               **********************/ 
	if (ip->query) {
		pLogoff = strstr(ip->query, logoffVar);
		if (pLogoff) {
			if (pLogoff == ip->query || 
				(pLogoff != ip->query && *(pLogoff-1) == '&')) {
				pLogoff += sizeof(logoffVar)-1;
				if (*pLogoff == 0 || *pLogoff == '&') { 
/***7/7/2000 - Changed code to process MINICGI_FORCE_LOGOFF correctly****/   
                             sendauth( ip, "1", autherr_m[17]);
/************************************************************************/
					return FALSE;
					}
				}
			}
		}

	if ( *(inheadp->authorization)) {

		cp = inheadp->authorization;

		if ( strncasecmp( cp, "Basic", 5) == 0) {
			strcpy( buf, "Basic ");
			cp += 5;
			cp2 = buf + 6;
			decode64( cp, cp2);
			mystrncpy( ip->authuser, cp2, SMALLLEN);
			if ( ( cp = strchr( ip->authuser, ':')) != NULL)
				*cp = '\0';
            		password = cp+1;
			authheadp = buf;
		}
		else {
			mystrncpy( buf, inheadp->authorization, SMALLLEN);
			cp = buf;
			while( *cp && !isspace( *cp))
				cp++;
			*cp = '\0';
			senderr( SERV_ERR, autherr_m[9], buf);

		/*************   06/06/2000  **********************/ 
			/* Incorrect or unknown Autherization type  */
		if(debug_log)
  	              write_debug(1,"chkauth", "Incorrect or unknown Autherization type ");
		/*************   06/06/2000  **********************/ 

			wn_exit( 0);
		}


		if ( authheadp == NULL) {
			sendauth( ip, "-s false", autherr_m[10]);

			/*************   06/06/2000  **********************/ 
			/*  No Authorization header  */
			if(debug_log)
			    write_debug(1,"chkauth", "No Autherization header");
			/*************   06/06/2000   **********************/ 

			wn_exit( 0); 
		}

        if (ip->query) {
            pLogon = strstr(ip->query, logonVar);
            if (pLogon) {
                if (pLogon == ip->query || 
                    (pLogon != ip->query && *(pLogon-1) == '&')) {
                    int count;
                    pLogon += sizeof (logonVar)-1;
                    count = sscanf (pLogon,"%63[^&]", forceUserId);
                    if (count == 1) {
                        www_unescape (forceUserId, ' ');
                        if (ip->authuser[0]) {
                            if (strcmp(ip->authuser, forceUserId) != 0) {
                                char msg[128];
                                sprintf (msg, "Logon required to user id %s. "
                                         ,forceUserId);
                                sendauth( ip, "-s false", msg);
                                return (FALSE);
                                }
                            }
                        }
                    }
                }
            }

        result = checkUser (ip->authuser, password, msg);
		switch (result) {
		case 0:
			/*************   06/06/2000  **********************/ 
			if(debug_log)
			    write_debug(1,"chkauth","Authorization successful");
			/*************   06/06/2000   **********************/ 

			return TRUE;

		case AUTH_DENIED:

			/*************   06/06/2000   **********************/
			if(debug_log) 
			     write_debug(1,"chkauth","Authorization denied ");
			/*************   06/06/2000  **********************/ 
			
			sendauth( ip, "-s false", msg);
			/*  sendauth( ip, "-s false", autherr_m[13]); */
			return FALSE;

		default:
			/*************   06/06/2000 **********************/
			if(debug_log)	 
			    write_debug(1,"chkauth","Authentication module failed");
			/*************   06/06/2000  **********************/ 

			sprintf( buf, "%s %d", autherr_m[0], result);
			logerr( err_m[42], buf);
			sendauth( ip, "-s false", buf);
			return FALSE;
		}
	}
	sendauth( ip, "-s false", "");
	return FALSE;
}


static void sendauth( Request	*ip, char	*noncearg, char *logmsg)
{
	char   buf[MIDLEN];

       strcpy( outheadp->status, "401 Unauthorized");
       sprintf( outheadp->list,
                      "WWW-Authenticate: Basic realm=\"%.200s\"\r\n","/");
      ip->encoding =  NULL;
        ip->mod_time = 0;  
        ip->content_type = "text/html";

/***7/7/2000 - Changed code to process MINICGI_FORCE_LOGOFF correctly****/   
        if(strcmp(noncearg,"1")) {
	   sprintf( buf, "<head>\n<title>%s</title>\n</head>\n<body>\n",
			autherr_m[1]);
	   sprintf( buf + strlen(buf), "<h2>%s</h2>\n", autherr_m[1]);
        }
        else
        {
          sprintf( buf, "<head>\n<title>%s</title>\n</head>\n<body>\n",
                        autherr_m[14]);
          sprintf( buf + strlen(buf), "<h2>%s</h2>\n", autherr_m[14]);         
        }
/***************************************************************************/	
        
        sprintf( buf + strlen(buf), "%s\n%s\n </body>\n", 
			 logmsg, SERVER_LOGO);
	/****Type casted to avoid warning-26/7/2000******/
	ip->datalen = (long)strlen( buf);
	/************************************************/
	sprintf( ip->length, "%d", ip->datalen);

	http_prolog( );
	send_text_line(buf);

	writelog( ip, log_m[1], logmsg);
	return;
}


/*****************************************************************************/

int checkUser (char * user, char * password, char * msg)
    {
    long long dialog_id;
    short ua_status;
    char displayText[256];
    short displayTextLen;
    short rc;
    
    if (*password == '\0') {
        strcpy (msg, "Password is required.");
        return 1;
        }
        
    dialog_id = 0;
    rc = USER_AUTHENTICATE_ (user,
                            (short) strlen (user),
                            (short) 0x0181,
                            &dialog_id,   /* dialog id */
                            &ua_status,
                            ,             /* status flags */
                            displayText,
                            (short) sizeof (displayText),
                            &displayTextLen);
    if (rc != 70) {
        if (msg)  {
            displayText[displayTextLen] = '\0';
            strcpy (msg, displayText);
            }
        return 1;
        }
        
    rc = USER_AUTHENTICATE_ (password,
                            (short) strlen (password),
                            (short) 0x0081, 
                            &dialog_id,   /* dialog id */
                            &ua_status,
                            ,             /* status flags */
                            displayText,
                            (short) sizeof (displayText),
                            &displayTextLen);
    if (rc != 0) {
        if (msg)  {
            displayText[displayTextLen] = '\0';
            strcpy (msg, displayText);
            }
        return 1;
        }

    return 0;
}    
