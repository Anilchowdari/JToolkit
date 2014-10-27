/*
    Wn: A Server for the HTTP
    File: wn/prequest.c
    Version 1.18.0

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
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "uniguard.h"
#include "wn.h"
#include <stdlib.h>
#include <cextdecs.h(DEBUG,FILE_GETINFOLISTBYNAME_,PATHNAME_TO_FILENAME_)>

extern char	*inet_ntoa();

#ifdef USE_VIRTUAL_HOSTS
extern char	*vhostlist[][3];
#endif

#ifndef S_IROTH
#define		S_IROTH	0000004	/* read permission, other */
#endif

extern void	tilde(),
		www_unescape();

static int	dedot();

static	void	parse_cgi(),
		path_security();


/*

 * Fill in the fields basename, filepath, cachepath, query, pathinfo,
 * type, length and mod_date of the struct pointed by ip and corresponding
 * to the requested item whose header is pointed to by ih.  Note that
 * cachepath and filepath are strings while relpath and basename are
 * only pointers into filepath.

 */

void parse_request(  Request	*ip, char	*url_path)
{
	register char	*cp;

	char    	path[MIDLEN];

	if ( (inheadp->protocol ==  HTTP1_1) &&	(!*inheadp->host_head) ) {
		/* Missing Host: header in 1.1 request is an error */
		senderr(CLIENT_ERR, err_m[15], "");
		return;
	}

	ip->type = RTYPE_UNCHECKED; /* we don't know type yet */

	/*
	 * Here are the initial steps in correct order:
	 *
	 * 1. Get hostname from "http://host/..." if it is there
	 *  and remove this part of url_path.  Call set_interface_root().
	 * 2. Find first ? and put everything after it in ip->query
	 * 3. Copy remainder to path
	 * 4. If path is only "/" make it DEFAULT_URI
	 * 4. Undo URL escapes on path ( www_unescsape)
	 * 6. Check if ip->rootdir should be changed and do it (tilde)
	 * 7. Parse for CGI setting PATH_INFO (parse_cgi)
	 * 8. Check for last ; or = and parse parameters
	 * 9. Dedot
	 * 10. Check path security (path_security)
	 * 11. Remove trailing slash if there.
	 * 12. Fill in filepath, and pointers relpath and basename
	 * 13. If it's an nph-CGI set ip->type = RTYPE_NPH_CGI
	 */

	if ( (strncasecmp( url_path, "http://", 7) == 0 ) ||
			(strncasecmp( url_path, "https://", 8) == 0 )) {
		cp = strchr( url_path, ':');
		url_path = cp + 3;

		cp = strchr( url_path, '/');
		if ( cp != NULL)
			*cp = '\0';
		mystrncpy( inheadp->host_head, url_path, MAXHOSTNAMELEN);
		if ( cp != NULL) {
			*cp = '/';
			url_path = cp;
		}
		else
			url_path = "/";
	}

	set_interface_root( );

	if ( ( cp = strchr( url_path, '?')) != NULL) {
		*cp++  = '\0';
		mystrncpy( ip->query, cp, MIDLEN);
		/* Decoding happens in cgi.c and csearch.c/check_query() */
		mystrncpy( path, url_path, MIDLEN);
		*--cp = '?';
	}
	else
		mystrncpy( path, url_path, MIDLEN);

	www_unescape( path, '+');

	if ( path[1] == '\0') {
		/* path = "/"" */
		strcpy( path , DEFAULT_URI);
		ip->filetype |= WN_DEFAULT_DOC;
	}

	if ( path[1] ==  ';') {
		/* path =  "/;something" */
		strcpy( path, DEFAULT_URI);
		ip->filetype |= WN_DEFAULT_DOC;
		mystrncat( path, &url_path[1], MIDLEN);
	}

	parse_cgi( ip, path);

	while (dedot( ip, path))
		;

	path_security( ip, path, FALSE);

	if ( ip->type == RTYPE_FINISHED)
		return;

	cp = path;
	/* move cp to end of path */
	while ( *cp)
		cp++;
	cp--;

	if ( *cp == '/') {
			/* Remove trailing '/'  if there */
		*cp = '\0';
	}


	/* Fill in filepath, and pointers relpath and basename */

	mystrncpy( ip->filepath, ip->rootdir, MAXDIRLEN);
	cp = ip->filepath;
	while ( *cp)
		cp++;
	ip->relpath = cp;
	mystrncpy( ip->relpath, path, MIDLEN - (MAXDIRLEN + TINYLEN + 4));

	cp = strrchr( ip->filepath, '/');
	ip->basename = ++cp;

#ifdef _NSKOSS
#else
   filepathToGuardian (ip->gname, ip->filepath);
#endif

	get_stat( ip);

   return;
}



/*
 * get_stat( ip)
 * Stats the file pointed to by ip->filepath.  If it fails assume file
 * does not exist.  If it is a directory set WN_DIR bit in ip->filetype
 * unless this is the default document, in which case log an error and
 * deny access. If not a directory make sure it is a regular file and
 * if so, get length and modification time and set the etag.
 */

void get_stat( Request	*ip)
{
	struct stat stat_buf;

#ifdef _NSKOSS
    if ((ip->filetype & WN_DEFAULT_DOC))
		return;

    if ( stat( ip->filepath, &stat_buf) != 0 ) {
        /* Might be redirect, or defdoc, just continue */
        *ip->length = '\0';
        ip->mod_time = 0;
        ip->status |= WN_CANT_STAT;
        return;
    }

    ip->mod_time = stat_buf.st_mtime;
    ip->datalen = (long) stat_buf.st_size;
    sprintf( ip->length, "%lu", ip->datalen);
    set_etag( &stat_buf);
                                            
#else
	short ret;
	short item_list[3] ={142,     /* length */
	                      41,    /* type = 0 = unstructured */
			      42,    /* filecode */
			    };
    short item_value[4];
    /* Some applications may not be document oriented */
    /* --- allow them not to need a home page          */
    if (!(ip->filetype & WN_DEFAULT_DOC)) {   

        char gname[40];   /* djc 24 oct 98 */
        if (rootdir[0] == '/') {
           short filelen, status;
           ret = PATHNAME_TO_FILENAME_ (ip->filepath, gname, sizeof(gname),
                                       &filelen, &status); 
           }
        else {
           strcpy (gname, ip->gname); 
           ret = 0;
            }
           
        if (ret == 0)
	        ret = FILE_GETINFOLISTBYNAME_(
	           		gname,          /* djc 24 oct 98 */
				(short) strlen(gname),
				item_list,
				3,
				item_value,
				sizeof (item_value));
      /*** Added  code to copy correct data length-24/7/2000****/	        
		if(ret==0){
                   FILE *fp;
		   int count = 0;
		   fp = fopen(gname,"r");
		   while(!feof(fp)){
            	      char c;
            	      c=(char)fgetc(fp);		   
	 	      if(c != '\n' && c != '\r')
			count++;
		   }
		   sprintf(ip->length,"%lu",count);
 		   stat_buf.st_size = count;		   
		}
	/*************************************************************/
		else {
		    /* Might be redirect, or defdoc, just continue */
		    char msg[128];
		    *ip->length = '\0';
		    ip->mod_time = 0;
		    ip->status |= WN_CANT_STAT;
/*     djc  June 15, 1998  ***************************************************
		    if (ret == 11) 
			    ip->type = RTYPE_DENIED;
		    else {
		           sprintf (msg, "%s (%s) - error=%hd",
				    ip->filepath, ip->gname, ret);
    	                   logerr( err_m[12], msg);
	 	    }
******************************************************************************/
		    return;
		    }
        	}
	
   	ip->mod_time = time(NULL);
	/* memcpy ((char*)&(ip->datalen), (char *)&(item_value[1]), 4);
   	sprintf( ip->length, "%lu",item_value[1]);
	*/
	stat_buf.st_mtime = time(NULL);
	stat_buf.st_ino = 0;
	/* stat_buf.st_size = item_value[0]; */
	set_etag( &stat_buf);
#endif

     ip->type = RTYPE_CGI;
	return;
	}


void set_etag( struct stat *sbp)
{
	sprintf( this_rp->etag, "%lx-%lx-%lx", (long) sbp->st_mtime,
				(long) sbp->st_ino, (long) sbp->st_size);
}

static void parse_cgi( Request	*ip, char	*path)
{
	if ( ip->type == RTYPE_FINISHED)
		return;

    ip->type = RTYPE_CGI;

    /******************  19/06/2000  *******************/	
    if(debug_log)
       write_debug(1, "Prequest : Request Type is CGI",0);
    /***************************************************/

	*(ip->pathinfo) = '\0';

}



/*
 * path_security( ip, path, is_pathinfo) generates an error if any bad
 * characters are found in the URI.  Allowable chars are any alpha-numeric or
 * '_', '-', '.', '+', '/', and '%'.  Also "../" is disallowed (the function
 * dedot() has already handled "/../" and "/./".  We also check that
 * path is either empty (referring to root) or begins with '/'.
 * Things are a little more lax for the PATHINFO part of a CGI URL.
 * At the moment this means that '=' is allowed.
 */

static void path_security( Request	*ip, char	*path, int	is_pathinfo)
{
	register char	*cp;
	char		buf[SMALLLEN];

	/* Security check */
	cp = path;
	if ( *cp && (*cp != '/') ) {
		senderr( CLIENT_ERR, err_m[59], path);
		return;
	}

	while ( *cp ) {
		if ( isalnum( *cp))
			cp++;
		else {
			switch( *cp) {
			case '.':
				if ( (cp[1] == '.') && (cp[2] == '/')) {
					logerr( err_m[21], ip->request);
					senderr( CLIENT_ERR, err_m[59], path);
					return;
				}
			case '/':
			case '-':
			case '+':
			case '_':
			case '%':
			case '$':
				cp++;
				break;
			/* Stuff allowed for pathinfo here */
			case '=':
				if ( is_pathinfo) {
					cp++;
					break;
				}
			default:
				sprintf( buf, err_m[20],	*cp);
				logerr( buf, ip->request);
				senderr( CLIENT_ERR, err_m[59], path);
				return;
			}
		}
	}
}


/*
 * dedot( ip, path) replaces the first "//", "/./", and "/xxxx/../"
 * in path with "/".  It returns TRUE if it found one of these
 * and FALSE if there were none of these in path.
 */

static int dedot( Request	*ip, char	*path)
{
	register char	*cp,
			*cp2;

	if ( ip->type == RTYPE_FINISHED)
		return FALSE;

	cp = path;
	while ( (cp = strchr( cp, '/')) != NULL) {
		cp++;
		if ( *cp == '/') {
			strcpy( cp, cp + 1);
			return TRUE;
		}
		if ( strncmp( cp, "./", 2) == 0 ) {
			strcpy( cp, cp + 2);
			return TRUE;
		}
		if ( strncmp( cp, "../", 3) == 0 ) {
			*--cp = '\0';
			if ( (cp2 = strrchr( path, '/')) == NULL) {
				logerr( err_m[21], ip->request);
				senderr( CLIENT_ERR, err_m[59], path);
				return FALSE;
		}
			strcpy( cp2, cp + 3);
			return TRUE;
		}
	}
	return FALSE;
}


/*
 * getfpath( path, fname, ip) takes filename in fname and translates
 * to complete path relative to system root.
 * If fname starts with '/' assume it is relative to system root,
 * if it starts with ~/ it is relative to WN root  otherwise
 * assume relative to current directory.
 */

int getfpath( char	*path, char *fname, Request	*ip)
{

#ifdef LIMIT_2_HIERARCHY
	return getfpath2( path, fname, ip->cachepath);
#else
	register char	*cp;

	if ( *fname == '/') {
		mystrncpy( path, fname, MIDLEN);
	}
	else if ( *fname == '~' && *(fname + 1) == '/') {
		mystrncpy( path, ip->rootdir, SMALLLEN);
		mystrncat( path, fname + 1, MIDLEN - SMALLLEN);
	}
	else {
		strcpy( path, ip->cachepath);
		if ( (cp = strrchr( path, '/')) == NULL) {
			logerr( err_m[37], path);
			return FALSE;
		}
		else
			strcpy( ++cp, fname);
	}				
	return TRUE;
#endif
}


/*
 * getfpath2( path, fname, cachepath) is like getfpath except it restricts
 * to WN hierarchy.  If fname starts with either '/' or '~/' then
 * it is assumed relative to WN root  otherwise it is assumed 
 * relative to current directory.
 */

int getfpath2( char	*path, char *fname, char *cachepath)
{

	register char	*cp;

	if ( *fname == '~')
		fname++;
	if ( *fname == '/') {
		mystrncpy( path, this_rp->rootdir, SMALLLEN);
		mystrncat( path, fname, MIDLEN - SMALLLEN);
	}
	else {
		strcpy( path, cachepath);
		if ( (cp = strrchr( path, '/')) == NULL) {
			logerr( err_m[37], path);
			return FALSE;
		}
		else 
			strcpy( ++cp, fname);
	}				
	if ( strstr( "../", path) != NULL) {
		logerr( err_m[21], path);
		return FALSE;
	}
	return TRUE;
}


void set_interface_root(void)
{
		mystrncpy( this_rp->rootdir, rootdir, SMALLLEN);
}
