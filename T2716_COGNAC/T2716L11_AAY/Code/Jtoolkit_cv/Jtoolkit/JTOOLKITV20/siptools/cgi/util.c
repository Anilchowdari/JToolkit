/*
    Wn: A Server for the HTTP
    File: wn/util.c
    Version 1.17.9

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
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <grp.h>
#include <errno.h>
#include "wn.h"

extern	char		*inet_ntoa();

extern void		www_unescape();

static struct in_addr		ip_address;

#ifdef RFC931_TIMEOUT
extern void		rfc931();
#ifdef SOCKADDR
	static struct sockaddr	*mysin,
				*remsin;
#else
	static struct sockaddr_in	*mysin,
					*remsin;
#endif
#endif

/***********************************************************************/

char * strlower (char * s)
	{
	int i;

	for (i=0;s[i];i++)
		s[i] = (char) tolower ((int)s[i]);
	return s;
	}

/*
 * get_remote_ip() gets IP address of client via getpeername call and
 * puts it in global variable remaddr.
 */

void get_remote_ip( )
{
	static struct sockaddr_in	saddr;
	int			size;

	size = sizeof(saddr);
	if ( getpeername(this_conp->sock, (struct sockaddr *) &saddr, (int *) &size)< 0){
		*remaddr = '\0';
		*remotehost = '\0';
		return;
	}

	ip_address = saddr.sin_addr;
	mystrncpy(remaddr, inet_ntoa(ip_address), 20);

#ifdef RFC931_TIMEOUT
	remsin = &saddr;
#endif
}



/*
 * get_remote_info() does DNS lookup of remotehost and places host name
 * in global variable remotehost.  
 * It also does RFC931 lookup if RFC931_TIMEOUT
 * is defined.  If possible the call to this function happens after the
 * entire transaction is complete so the user doesn't have to wait for these
 * lookups to happen.  This delay is not possible for CGI or when doing
 * authentication. (RFC931 stuff thanks to Chris Davis).
 * 
 */

void get_remote_info( )
{
#ifndef NO_DNS_HOSTNAMES
	char    dot_name[MAXHOSTNAMELEN + 1];
	struct hostent	*hostentp = NULL;

	if ( *remotehost )
		return;
	if ( !*remaddr ) {
		strcpy( remotehost, "unknown");
		return;
	}
	
	/* if (*remotehost) we have already got info; if (!*remaddr)
	   then there is no hope -- we can't even get IP address */

	hostentp = gethostbyaddr((char *) &ip_address.s_addr,
			sizeof (ip_address.s_addr), AF_INET);
	if (hostentp) {
		mystrncpy( remotehost, hostentp->h_name, MAXHOSTNAMELEN);

		/* Check that the name has this address listed. */
#ifndef CHECK_DNS_HOSTNAMES
		if ( (!dir_p->accessfile) || (!*dir_p->accessfile)) {
			strlower( remotehost);
			return;
		}
			/* Assume ok unless used for access control */
#endif /* CHECK_DNS_HOSTNAMES */
		if (strlen( remotehost) >= MAXHOSTNAMELEN) {
			hostentp = gethostbyname( remotehost);
		} else {
			sprintf(dot_name, "%s.", remotehost);
			hostentp = gethostbyname( dot_name);
		}

		if (hostentp) {
			register char **ap;
			for (ap = hostentp->h_addr_list; *ap; ap++) {
				if (!memcmp( (char *) &ip_address.s_addr,
						*ap, hostentp->h_length)) {
					/* Value in remotehost is ok.*/
					strlower( remotehost);
					return;
				}
			}
		}
		/* No good name found */
		*remotehost = '\0';
	}
#endif /* NO_DNS_HOSTNAMES */
	if ( !*remotehost )
		strcpy( remotehost, remaddr);
	if ( !*remotehost )
		strcpy( remotehost, "unknown");

}


#ifdef RFC931_TIMEOUT
void get_rfc931()
{
	if (remsin && mysin && (*this_conp->rfc931name == '\0'))
		rfc931(remsin, mysin, this_conp->rfc931name);
	signal( SIGALRM, SIG_DFL);
	alarm( 20);  	/* Give ourselves 20 seconds to get remote DNS data
			 * and write log entry.
			 */
}
#endif


/*
 * mystrncpy( s1, s2, n) is an strncpy() which guarantees a null
 * terminated string in s1.  At most (n-1) chars are copied.
 */

char * mystrncpy( char	*s1, char *s2, int	n)
{
	register char	*cp1,
			*cp2;
	cp1 = s1;
	cp2 = s2;
	n--;
	while ( *cp2 && (n > 0)) {
		n--;
		*cp1++ = *cp2++;
	}
	*cp1 = '\0';
	return s1;
}

/*
 * mystrncat( s1, s2, n) is an strncat() which guarantees a null
 * terminated string in s1.  At most (n-1) chars are appended.
 */

char * mystrncat( char	*s1, char *s2,  int	n)
{
	register char	*cp1,
			*cp2;
	cp1 = s1;
	cp2 = s2;
	n--;

	while ( *cp1)
		cp1++;

	while ( *cp2 && (n > 0)) {
		n--;
		*cp1++ = *cp2++;
	}
	*cp1 = '\0';
	return s1;
}

char * mymemcpy( char	*p1, char *p2, int n)
{
	if ( p1 == p2)
		return (p1);
	while ( n > 0 ) {
		n--;
		*p1++ = *p2++;
	}
	return (p1);
}

/*
 * chop( line)  Cut out CRLF at end of line, or just LF.  Return TRUE
 * if there is a LF at end, otherwise FALSE.
 */

int chop( char *line)
{
	register char	*cp;

	if ( *line == '\0')
		return FALSE;
	cp = line;
	while ( *cp )
		cp++;
	if ( *--cp == '\n') {
		*cp = '\0';
		if ( (cp > line) && *--cp == '\r')
			*cp = '\0';
		return TRUE;
	}
	return FALSE;
}


/*
 * int amperline( p1, p2)  Copy p2 to p1 until 
 * p2 is exhausted.  Encode '<', '>', and &. 
 */

int amperline ( char	*p1, char *p2)
{
	register char *cp;
	int found = FALSE;

	while ( *p2 ) {
		switch( *p2) {
		case '<':
			found = TRUE;
			strcpy( p1, "&lt;");
			p1 += 4;
			p2++;
			break;
		case '>':
			found = TRUE;
			strcpy( p1, "&gt;");
			p1 += 4;
			p2++;
			break;
		case '&':
			cp = p2;
			cp++;
			while ( isalnum( *cp))
				cp++;

			if ( *cp == ';') {
				*p1++ = *p2++;
			}
			else {
				found = TRUE;
				strcpy( p1, "&amp;");
				p1 += 5;
				p2++;
			}
			break;
		default:
			*p1++ = *p2++;
		}
	}
	*p1 = 0;
	return found;
}


/*
 * get_local_info() fills in hostname and port from the connected socket.
 */

void get_local_info( int	sockdes)
{
	int size;
	static struct sockaddr_in      saddr;
	struct hostent  *hostentp;

	size = sizeof(saddr);
	/*  sockdes is our our descriptor for the socket. */
	if ( getsockname( sockdes, (struct sockaddr *) &saddr, (int *) &size) < 0 ) {
		daemon_logerr( err_m[73], "", errno);
		errno = 0;
		return;
	}

		/* Remember our port number */
	port = ntohs(saddr.sin_port);
#ifdef RFC931_TIMEOUT
	mysin = &saddr;
#endif
	/* Remember our hostname (or at least dotted quad) */
	if ( *hostname) 
		return;

	mystrncpy( hostname, inet_ntoa(saddr.sin_addr), MAXHOSTNAMELEN);

}

#ifdef NEED_INITGROUPS
#ifdef STANDALONE

#ifndef NGROUPS_MAX
#define NGROUPS_MAX	(16)
#endif

int initgroups( char	*gp_name, gid_t	group_id)

{
	gid_t		groups[NGROUPS_MAX];
	struct group	*g;
	int		i;
	char		**names;

	groups[0] = group_id;

	for ( i = 1; i < NGROUPS_MAX; i++) {
		if ((g = getgrent()) == NULL)
			break;
		if (g->gr_gid == group_id)
			continue;

		for (names = g->gr_mem; *names != NULL; names++) {
		        if (!strcmp(*names, gp_name))
				groups[i] = g->gr_gid;
		}
	}

	return setgroups(i, groups);
}

#endif
#endif


/*
 *  Case insensitive comparison of first n chars of two strings
 */

int strncasecmp( char *s1, char *s2, int n)
{
	int	r;

	while ( *s1 && *s2 && ( n > 0)) {
		if ( (r = (tolower( *s1) - tolower( *s2))) != 0 )
			return r;
		s1++;
		s2++;
		n--;
	}
	return ( n == 0 ? 0 : *s1 - *s2);
}


/*
 *  Case insensitive comparison of two strings
 */

int strcasecmp( char *s1, char *s2)

{
	int	r;

	while ( *s1 && *s2 ) {
		if ( (r = (tolower( *s1) - tolower( *s2))) != 0 )
			return r;
		s1++;
		s2++;
	} 
	return ( *s1 - *s2);
}


#ifdef _TNS_X_TARGET
#else
#include <cextdecs.h>
unsigned int alarm (unsigned int seconds)
{
SETLOOPTIMER ((short)(100 * seconds));
}
#endif
