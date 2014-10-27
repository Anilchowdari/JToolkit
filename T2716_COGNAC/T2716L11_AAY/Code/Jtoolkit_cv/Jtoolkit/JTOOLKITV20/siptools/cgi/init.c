/*
    Wn: A Server for the HTTP
    File: wn/init.c
    Version 1.18.0
    
    Copyright (C) 1995-7  <by John Franks>

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
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <cextdecs.h(ABEND,SETMODE,FILE_CLOSE_,DEFINEINFO )>
#ifdef   _NSKOSS
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#else
#endif
#include "wn.h"
#include "version.h"
#include "err.h"
#include "content.h"

#define _CGI_NO_EXTERNALS
#include "cgilib.h"

/* this may be needed if unistd.h isn't included... */
#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifdef RFC931_TIMEOUT
extern void	get_rfc931();
#endif

#ifdef NO_VOID_PTR
#define	void	int
#endif

#ifndef BSD_LIKE
extern long	timezone;
#endif

extern char	*optarg;

extern int	optind;

#define		WN_COMMON	(0)
#define		WN_VERBOSE	(1)
#define		NCSA_LOG	(2)

#ifdef VERBOSELOG
static int	log_type = WN_VERBOSE;
#else
static int	log_type = WN_COMMON;
#endif


static void	dump_log();

char	rootdir[SMALLLEN],
	wnlogfile[SMALLLEN],
    tcpipProcess[SMALLLEN],
	errlogfile[SMALLLEN],
    argvzero[SMALLLEN],
	pid_file[SMALLLEN],
	cfname[SMALLLEN],
	empty[1],	/* safe place to park pointers */
	hostname[MAXHOSTNAMELEN],
	remotehost[MAXHOSTNAMELEN],
	remaddr[20],
	**mtypelist = NULL,
	**suflist = NULL;

char DEFAULT_URI[1024] =	"/homepage";

int	admin_mode = FALSE,
	debug_log =  FALSE,
        length	= 0;
unsigned	serv_perm = 0,
		interface_num,
		acache_id,
		cache_id;

#define		LNOLOG	(0)
#define		LFILE	(1)
#define		LSYSLOG	(2)

static int	log_func = LNOLOG;

extern char ** environ;

int auto_form_decoding;
int authenticate_users;
int nologbanner;
char snw2addr[64];
int daemon_init;
int server_mode;
char pathmon[32] = "";
char serverClass[32] = "";
int pathway_server = 0;
int transaction_timeout = TRANSACTION_TIMEOUT;

#ifndef NO_FLOCK
static void	locklog(),
		unlocklog();
#endif

void CGI_internal_init(Request *ip);
static char * getDefineFile (char * defname);

static void	log_logfile(),
		start_log();
int open_wnlog(void);
void close_wnlog(void);


static FILE	*logfp = NULL,
		*errlogfp = NULL;

#ifdef _NSKOSS
char executablePath[PATH_MAX] = "";
#else
char executablePath[40] = "";
#endif

static void   append_gmtoff();
static void   CommandError(char *err); 


/****************************************************************************
   This function displays command error and exits 
****************************************************************************/
static void   CommandError(char *err)
{

    #ifndef _NSKOSS
    	    fopen_std_file (1,0);    /* open stdout */
    #endif

        fprintf (stdout, err);
	exit(1);

}
/***************************************************************************/



void wn_init( int	argc, char	*argv[])
{
	int	i;
	int usageError = 0;
        char pstr[20];
 
#ifdef _NSKOSS
    strcpy (executablePath, argv[0]);
#else
    strcpy (executablePath, strchr(argv[0],(int)'$'));
#endif
	daemon_init=0;

	empty[0] = '\0';
	port = DEFAULT_PORT;
	mystrncpy (wnlogfile, WN_LOGFILE, SMALLLEN);
	mystrncpy (errlogfile, WN_ERRLOGFILE, SMALLLEN);
    argvzero[0] = '\0';
	if ( *wnlogfile)
		log_func = LFILE;

#ifdef _NSKOSS
    getcwd (rootdir, sizeof(rootdir)-1);
#else
	strcpy (rootdir, strchr(getenv("DEFAULTS"),(int)'$'));
#endif

	mystrncpy( hostname, WN_HOSTNAME, MAXHOSTNAMELEN);

    auto_form_decoding = 1;
    authenticate_users = 1;
    nologbanner = 0;
    snw2addr[0] = '\0';
    server_mode = 0;
    pathway_server = 0;
    tcpipProcess[0] = '\0';
    pathmon[0] = '\0';
    serverClass[0] = '\0';

	for (i=1; i<argc;) {
		char *p;
		char *pn;

		p = argv[i];
		if (i+1 < argc)
		    pn= argv[i+1];
	        else
        	    pn = NULL;


        if (i==1 && isdigit(*p)) {   /* look for 'accept_nw2' parameter */
            char * pd;
            int periods = 0;
            for (pd=p; ; ) {
                 pd=strchr(pd,'.');
                 if (pd) {
                     periods++;
                     pd++;
                     }
                 else
                     break;
                 }
             if (periods == 4) 
	     {
                     strcpy (snw2addr, p);
                     i+=1;
                     server_mode = 1;
                     continue;
             }                                                             
             else 
	     {
                         usageError = 2;
                         break;
             }
           }   /* look for accept_nw2 parameter */


      else /*  if not first time (i.e, if i > 1 ) */
      {
         /* check for syntax of command line arguements              */
         /************************************************************/
            if( !((strcasecmp(p,"-server") == 0) ||
            (strcasecmp(p,"-homepage")==0) || (strcasecmp(p,"-naf")==0)
            || (strcasecmp(p,"-noa")==0) || (strcasecmp(p,"-nlb")==0) )) 
	    {

               if( pn && strcasecmp(pn,"-server") != 0 )
                 if(pn[0] == '-' && strcasecmp(p,"-f")!=0 ) 
                 {
                      usageError = 1;
                      break;
                 }
            }
        /************************************************************/    


		if (strcasecmp(p, "-server") == 0 ) {
			server_mode = 1;
            i+=1;
			continue;
			}
		else 
		if (strcmp(p, "-p") == 0 || strcmp(p, "-P") == 0) 
		{
		    if (pn) 
		    {
			if( strcmp(pn, "0") == 0 )
			{
				port           = 0;
				pathway_server = 1;
				server_mode    = 1;
			}
			else
			{
				port = atoi (pn);
                		if (port == 0) 
				     CommandError(" Invalid port number entered ");
			}
	               	i+=2;
			continue;
		    }
            else {
		        usageError = 1;
	     	    break;
			    }
			}
		else
		if (strcmp(p, "-r") == 0 || strcmp(p, "-R") == 0) {
		 if (pn) 
		 {
        	 	mystrncpy( rootdir, pn, SMALLLEN);
                 	i+=2;
		 	continue;
		 }
            else {
		        usageError = 1;
	     	    break;
			    }
			}
		else
		if (strcmp(p, "-t") == 0 || strcmp(p, "-T") == 0) {
	  	 if (pn) 
		 {
			if( pn[0] != '$' )        	
				CommandError(" Invalid TcpIp Process name ");

			mystrncpy( tcpipProcess, pn, SMALLLEN);
                	i+=2;
			continue;
 		 }
            else {
		        usageError = 1;
	     	    break;
			    }
			}
		else
		if (strcmp(p, "-l") == 0 || strcmp(p, "-L") == 0) {
			if (pn) 
			{
			        mystrncpy( wnlogfile, pn, SMALLLEN);
		                log_func  = LFILE;
			/*  Merging -d and -l options  */
			/***************   20/07/2000 ***********************/
				debug_log = TRUE;
			/****************************************************/
                		i+=2;
				continue;
			}
            else {
		        usageError = 1;
	     	    break;
			    }
			}
		else
		if (strcmp(p, "-e") == 0 || strcmp(p, "-E") == 0) {
			if (pn) {
				mystrncpy( errlogfile, pn, SMALLLEN);
                                i+=2;
				continue;
				}
			}
		else
		if (strcmp(p, "-f") == 0 || strcmp(p, "-F") == 0)
		{
		        /************************************************/
			if (pn && pn[0] != '-' ) 
			/************************************************/
			{
                                mystrncpy( argvzero, pn, SMALLLEN);
                                i+=2;
				continue;
			}
                        else 
			{
				mystrncpy( argvzero, argv[0], SMALLLEN);
                                i+=1;
				continue;
                        }
		}

		else

		if (strcasecmp(p, "-timeout") == 0 ) {
			if (pn) 
			{
			  transaction_timeout = atoi (pn);

			  if( transaction_timeout == 0 )
				CommandError(" Invalid value entered for timeout option ");

			  i+=2;
		   	  continue;
			}
            else {
		        usageError = 1;
	     	    break;
			    }
			}
		else
        if (strcasecmp (p, "-pathmon") == 0) {
           int n, c;
           char node[8]=""; 
           char process[8]="";
           if (pn) {
               c=0; n=0;
               i+=2;
               c = sscanf (pn, "\\%7[a-zA-Z0-9].$%5[a-zA-Z0-9]%n", node, process, &n);
               if (c < 2 ||  n == 0) {
                    node[0] ='\0';
                    c=0;n=0;
                    c = sscanf (pn, "$%5[a-zA-Z0-9]%n", process, &n);
                    if (c < 1 || n == 0) {
		                usageError = 1;
	     	            break;
			            }
                    else {
                       sprintf (pathmon,"$%s", process);
                       continue;
                        }
                    }
               else {
                   sprintf (pathmon,"\\%s.$%s", node, process);
                   continue;
                   }   
            }
            else {
		        usageError = 1;
	     	    break;
			    }
			}
        else
        if (strcasecmp(p, "-homepage") == 0) {
            if (pn) {
               i += 2;
               if (*pn != '/')
                   strcpy (DEFAULT_URI, "/");
               else
                   strcpy (DEFAULT_URI, "");
               strcat (DEFAULT_URI, pn);
                }
            else {
                usageError = 1;
                break;
                }
            }
        else   
		if (strcasecmp(p, "-naf") == 0 ) {
            auto_form_decoding = 0;
            i+=1;
			continue;
			}
		else
		if (strcasecmp(p, "-noa") == 0 ) {
            authenticate_users = 0;
            i+=1;
			continue;
			}
		else
		if (strcasecmp(p, "-nlb") == 0) {
            nologbanner  = 1;
            i+=1;
			continue;
			}
		else {
		    usageError = 1;
	     	break;
			}
         } /*  else */

       } /*  end of for loop */

    if (usageError) {
#ifdef _NSKOSS
#else
        fopen_std_file (1,0);    /* open stdout */
#endif
        fprintf (stdout, "Usage: appname -p port -r root_directory (Begin with / to use OSS files) \n");
        fprintf (stdout, "       -naf no_auto_form_decoding \n");
        fprintf (stdout, "       -noa  -t tcp_ip_process (e.g. ztc1)\n");
        fprintf (stdout, "       -homepage homepage (e.g. index)\n");
        fprintf (stdout, "       -timeout seconds (to process one request - default infinite)\n");
        fprintf (stdout, "       -l request_log -e error_log \n");
        fprintf (stdout, "       -f [argv[0]] (to process an application from a command prompt) \n");
        fprintf (stdout, "       -p 0 (run under Pathway - requests received on $receive) \n");
        fprintf (stdout, "       -pathmon pmon (route requests to serverclass sc-port-<port> \n");
        fprintf (stdout, "       running under named pathmon process. <port> is from -p option.) \n");
        fprintf (stdout, "       Pathway mode listener: appname -p 8080 -pathmon \\node.$pm \n");
        fprintf (stdout, "       Pathway mode server:   appname -p 0 (use maxlinks=1; linkdepth=1)\n");
        fprintf (stdout, "       Or use MINICGI_PATHMON define, param, or environment variable, and \n");
        fprintf (stdout, "       MINICGI_PORT param, or environment variable. \n");
        fprintf (stdout, "       MINICGI_DEBUG param, or environment variable forces break into debugger. \n");
        fprintf (stdout, "       (Note - use ^ in place of _ in params.) \n");
        fprintf (stdout, "(noa = no user authentication; )\n");
        if (usageError == 2) 
             fprintf (stdout, "Internally generated argv passed incorrectly\n");
        exit (1);
        }
	
    if (!*errlogfile)
        strcpy (errlogfile, "zerrlog");
	
    if (strcasecmp(wnlogfile, "stdout") == 0)
#ifdef _NSKOSS
		strcpy (wnlogfile, "");
#else
		strcpy (wnlogfile, strchr(getenv("STDOUT"),(int)'$'));
#endif
    if (strcasecmp(errlogfile, "stdout") == 0)
#ifdef _NSKOSS
		strcpy (errlogfile, "");
#else
		strcpy (errlogfile, strchr(getenv("STDOUT"),(int)'$'));
#endif
    
    if (argvzero[0] == '\0') {
       char *p;
       p = getenv ("MINICGI_GENERIC");
       if (p == NULL)
           p = getenv ("MINICGI^GENERIC");
       if (p)
           if (*p == 0 || *p == '1' || strcasecmp(p,"on") == 0 ) 
		       mystrncpy( argvzero, argv[0], SMALLLEN);
           else 
		       mystrncpy( argvzero, p, SMALLLEN);
        }
           
    if (argvzero[0]) {
        auto_form_decoding = 0;
        server_mode = 0;
        }
        
    if (tcpipProcess[0] == '\0') 
        strcpy(tcpipProcess, getDefineFile ("=TCPIP^PROCESS^NAME"));
    if (tcpipProcess[0] == '\0') {
	    char * p;
	    p = getenv ("TCPIP^PROCESS^NAME");
	    if (!p) {
		    p = getenv ("TCPIP_PROCESS_NAME");
		    }
	    if (p)
	     {
	       strcpy (tcpipProcess, p);
	       if( p[0] != '$')
		 CommandError("Detected Invaid TcpIp Processname from param settings\n");
	     }
	    }
    

    if (pathmon[0] == '\0') 
        strcpy(pathmon, getDefineFile ("=MINICGI_PATHMON"));
    if (pathmon[0] == '\0') 
        strcpy(pathmon, getDefineFile ("=MINICGI^PATHMON"));

/********************** 16/06/2000 *********************************
** Validation of pathmon process from obtained from param settings */

    pstr[0] = '\0';
    if (pathmon[0] == '\0') 
    {
        if(getenv("MINICGI_PATHMON"))
           strcpy(pstr,"MINICGI_PATHMON");
       else if (getenv("MINICGI^PATHMON"))
           strcpy(pstr,"MINICGI^PATHMON");

       if(pstr[0])
       {
	   strncpy (pathmon, getenv(pstr), sizeof(pathmon)); 
           if( pathmon[0] != '$')
               CommandError(" Detected Invalid Pathmon process from param\n");
       }
    }

    pathmon[sizeof(pathmon)-1] = '\0';

    pstr[0] = '\0';
    if(getenv("MINICGI_PORT"))
        strcpy(pstr,"MINICGI_PORT");
    else if (getenv("MINICGI^PORT"))
        strcpy(pstr,"MINICGI^PORT");

    if(pstr[0])
    {
         port = atoi(getenv(pstr));    
         if( port == 0)
	   CommandError("Detected Invalid port number from param settings\n");
    } 	  

/************************************************************************/

    if (pathmon[0]) {
        sprintf (serverClass, "sc-port-%lu", (unsigned long) port);
        }
    
    /* Alias Guardian style param names to conventional web style */
    /* Don't bother if we are a listener who will do pathsends    */

    if ((server_mode == 0 || pathway_server==1) && pathmon[0] == '\0') {  
        int i;
        for (i=0;environ[i];i++) {
            char * p; 
            char * pc=NULL; 
            char * pe=NULL;
            p = environ[i];
            if (p==NULL) 
                {char *pxyz;pxyz=p;printf ("NULL***********************");}
            pc = strchr(p,'^');
            if (pc)
                pe = strchr(pc+1,'=');
            if (pc && pe) {
                char * e;
                int j;
                e = malloc (strlen(p)+2);
                strcpy (e, p);
                for (j=0;e[j] && e[j]!='=';j++) 
                    if (e[j] == '^') 
                        e[j] = '_';
                putenv (e);                        
                free (e);
                }
            }
        }

    if (argvzero[0]) {
        char * argv[2];
#ifdef _NSKOSS
#else
        fopen_std_file (0,0);    /* open stdin */
        fopen_std_file (1,0);    /* open stdout */
        fopen_std_file (2,0);    /* open stderr */
#endif
        if (getenv ("REQUEST_METHOD") == NULL) 
            putenv("REQUEST_METHOD=GET");
        if (getenv ("CONTENT_TYPE") == NULL) 
            putenv("CONTENT_TYPE=text/html");
        if (getenv ("SCRIPT_NAME") == NULL) {
            char temp[SMALLLEN+64];
            strcpy (temp, "SCRIPT_NAME=");
            strcat (temp, argvzero); 
            putenv(temp);
            }
        CGI_internal_init(NULL);
        (void) CGI_initialize();
        argv[0] = argvzero;
        argv[1] = NULL;
        (void) CGI_main (1, argv);
        (void) CGI_terminate();          
        exit (0);
        }


    if (server_mode == 0) {
#ifdef _NSKOSS
        open_wnlog();
        if (log_func == LFILE)
    		chmod (wnlogfile, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
  		chmod (errlogfile, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
        close_wnlog ();
#else
        open_wnlog();
#ifdef _TNS_X_TARGET
        if (log_func == LFILE && logfp)
         /****Type casted to avoid warning-26/7/2000******/   
	    SETMODE ((short)gfileno(logfp), 1, 02022, 0);
        if (errlogfp)
            SETMODE ((short)gfileno(errlogfp), 1, 02022, 0);
 	/*************************************************/
#else
        if (log_func == LFILE && logfp)
            SETMODE ((short)fileno(logfp), 1, 02022, 0);
        if (errlogfp)
            SETMODE ((short)fileno(errlogfp), 1, 02022, 0);
#endif
        close_wnlog ();
#endif
        }
        
        

	start_log( FALSE);

}


/***************************************************************************/

void close_wnlog ()
	{
	if (logfp && *wnlogfile) {
		fclose (logfp);
        logfp = NULL;
        }
	if (errlogfp && *errlogfile) {
		fclose (errlogfp);
        errlogfp = NULL;
        }

	return;
	}


/****************************************************************************/


int open_wnlog(void)
	{
    char gnameLog[MAXGUARDFILE];
    char gnameErr[MAXGUARDFILE];

#ifdef _NSKOSS
	if (log_func == LFILE)
	    if (*wnlogfile)
			if ( (logfp = fopen ( wnlogfile, "a")) == NULL ) {
			fopen_std_file (1,0);
				fprintf( stdout, "Can't open logfile:%s -errno=%d\r\n", wnlogfile,errno);

				return 1;
			    }
			else
				;
		else
			logfp = stdout;

	if (*errlogfile) 
	    if ( (errlogfp = fopen( errlogfile, "a")) == NULL ) {
			fopen_std_file (1,0);
			 fprintf( stdout, "Can't open error logfile: %s -errno=%d\r\n", 
	                 errlogfile, errno);
	         close_wnlog();
		     return 1;
	         }
		else
			;
	else
		errlogfp = stdout;
#else
    filepathToGuardian (gnameLog, wnlogfile) ;
    filepathToGuardian (gnameErr, errlogfile) ;

     	 if (log_func == LFILE)
		if ( (logfp = fopen ( gnameLog, "a")) == NULL ) {
            fopen_std_file (1,0);    /* open stdout */
			fprintf( stdout, "Can't open logfile: %s (%s) -errno=%d\r\n", 
			         wnlogfile, gnameLog, errno);
			return 1;
		    }

    if ( (errlogfp = fopen( gnameErr, "a")) == NULL ) {
         fopen_std_file (1,0);    /* open stdout */
		 fprintf( stdout, "Can't open error logfile: %s (%s) -errno=%d\r\n", 
                 errlogfile, gnameErr, errno);
         close_wnlog();
	     return 1;
         }
#endif
    return 0;
	}

/****************************************************************************/

void logerr( char	*msg, char *msg2)
{

#ifndef NO_FLOCK
	struct flock	lck;
#endif

	time_t	clock;
	struct tm *ltm;

	char	*rhost,
		status[TINYLEN],
		date[TINYLEN];


    if (open_wnlog () != 0)
        return;

	if ( msg2 == NULL)
		msg2 = "";

	get_remote_info( );


	time(&clock);
	ltm = (struct tm *) localtime(&clock);
	strftime( date, TINYLEN, "%d/%h/%Y:%T", ltm);
	append_gmtoff( date, ltm);

	if ( outheadp && (*outheadp->status)) {
		mystrncpy( status, outheadp->status, 4);
	}
	else
		strcpy( status, "500");

	rhost = ( *remotehost ? remotehost : remaddr);
	rhost = ( *rhost ? rhost : "unknown");

	fprintf( errlogfp, "%s - - [%s] \"%s\" %s -", rhost,
				date, this_rp->request, status);

	if ( log_type == WN_VERBOSE ) {
		fprintf( errlogfp, " <(%d/%d) %s: %s>",
			this_conp->pid, this_conp->trans_cnt, msg, msg2);
	}
	fprintf( errlogfp, "\r\n");
	(void) fflush( errlogfp);

    close_wnlog();

}

void daemon_logerr( char	*msg, char * msg2, int	error)
{

	time_t	clock;
	struct tm *ltm;

	char	date[TINYLEN];

    if (open_wnlog () != 0)
        return;

	time(&clock);
	ltm = (struct tm *) localtime(&clock);
	strftime( date, TINYLEN, "%d/%h/%Y:%T", ltm);
	append_gmtoff( date, ltm);

	fprintf(errlogfp, "none - - [%s] \"none\" 500 0",  date);
	fprintf( errlogfp, " <%s %.100s: errno=%d> <> <> <>",
					msg, msg2, error);

	fprintf( errlogfp, "\r\n");
	(void) fflush( errlogfp);

	close_wnlog();
}


void writelog( Request	*ip, char	*msg, char *msg2)
{
	if (log_func == LFILE)
		log_logfile( ip, msg, msg2);
}


/* Write debug messages into the log file. */

void write_debug( int	n, char	*msg, char *msg2)

/*ignored for now, could become the debug level later*/
{
    if (open_wnlog () != 0)
        return;
	fprintf(logfp, "%s %s\r\n", msg, msg2);
	(void) fflush( logfp);
	close_wnlog();
}


static void log_logfile( Request	*ip, char	*msg,char  *msg2)
{
	time_t	clock;
	struct tm *ltm;

	register char	*cp,
			*cp2;

	char	bytes[TINYLEN],
		status[TINYLEN],
		date[TINYLEN],
		lbuf[BIGLEN + SMALLLEN];

	if ( outheadp && (*outheadp->status)) {
		mystrncpy( status, outheadp->status, 4);
	}
	else
		strcpy( status, "200");

	sprintf( bytes,"%ld", ip->logcount);

	time(&clock);
	ltm = (struct tm *) localtime(&clock);
	strftime( date, TINYLEN, "%d/%h/%Y:%T", ltm);
	append_gmtoff( date, ltm);

	cp = cp2 = this_conp->log_ptr;

	sprintf( lbuf, "[%s] \"%s\" %s %s",
				date, ip->request, status, bytes);
	cp2 += strlen( lbuf);
	if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
		dump_log();
		cp = cp2 = this_conp->logbuf;
	}
	strcpy( cp , lbuf);
	cp = cp2;

	if ( log_type == WN_VERBOSE ) {
		sprintf( lbuf, " <(%d/%d) %s: %s>",
			this_conp->pid, this_conp->trans_cnt, msg, msg2);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}

 		
		strcpy( cp, lbuf);
		cp = cp2;

		sprintf( lbuf, " <%s>", inheadp->ua);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}

		strcpy( cp , lbuf);
		cp = cp2;

		sprintf( lbuf, " <%s>", inheadp->referrer);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}
		strcpy( cp , lbuf);
		cp = cp2;

		sprintf( lbuf, " <%s>", inheadp->cookie);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + (LOGBUFLEN - 32)) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}
		strcpy( cp , lbuf);
		cp = cp2;

#ifdef USE_VIRTUAL_HOSTS
		sprintf( cp, " <%d>", interface_num);
		cp += strlen( cp);
#endif
	}
	else if ( log_type == NCSA_LOG) {
		sprintf( lbuf, " \"%s\"", inheadp->referrer);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}
		strcpy( cp , lbuf);
		cp = cp2;

		sprintf( lbuf, " \"%s\"", inheadp->ua);
		cp2 += strlen( lbuf);
		if ( cp2 >= this_conp->logbuf + LOGBUFLEN) {
			dump_log();
			cp = cp2 = this_conp->logbuf;
		}
		strcpy( cp , lbuf);
		cp = cp2;
	}

#ifdef _TNS_X_TARGET
	*cp++ =  '\r\n'; cp++;
#else
	*cp++ =  '\r'; *cp++ = '\n';
#endif
	*cp =  '\0';
	this_conp->log_ptr = cp;
}



static void append_gmtoff( char	*date, struct tm *ltm)
{
	register char	*cp;
	long		tz;
	char 		sign;

#if defined(BSD_LIKE) && ! defined(_TANDEM_SOURCE)
	tz = ltm->tm_gmtoff;
#else
	tz = - timezone;
	if( ltm->tm_isdst)
		tz += 3600;
#endif
	sign = ( tz > 0 ? '+' : '-');
	tz = ( tz > 0 ? tz :  -tz);
	cp = date;
	while ( *cp)
		cp++;
	
	sprintf( cp, " %c%02ld%02ld", sign, tz/3600, tz % 3600);
}

static void start_log( int	restart)
{
	time_t	clock;
	struct tm *ltm;
	char	*cp,
		startdate[TINYLEN];

	if ( log_func != LFILE)
		return;
    if (nologbanner || pathway_server || server_mode)
        return;

    if (open_wnlog () != 0)
        return;
	
	time(&clock);
	ltm = (struct tm *) localtime(&clock);
	strftime( startdate, TINYLEN, "%d/%h/%Y:%T", ltm);
	cp = ( restart ? "Restarting" : "Starting");
	fprintf(logfp, "\r\n%s: %s %s at port %d\r\n", 
			startdate, cp, VERSION, port);

	(void) fflush(logfp);
	close_wnlog ();
}


void
wn_abort( )
{

	this_rp->type = RTYPE_FINISHED;
	this_rp->status |= (WN_ABORTED + WN_ERROR);
	
}

void reinit (void) {
    
    }

void wn_exit( int	status)
{
	if (server_mode == 1) {
        if (status != 10)
		    flush_outbuf();
        FILE_CLOSE_ ((short)this_conp->sock);
        }

    alarm(0);

    if (status == 0) {
        if (pathway_server == 1 && pathway_reply_needed) {
            reinit();
            longjmp(jump_environment, 1);
            }

        exit(0);
        }
    else {
	    dump_log();
        if (server_mode == 1) 
            CGI_connection_abort (); /* djc  10/17/97 */
        if (pathway_server == 1 && pathway_reply_needed) {
            longjmp(jump_environment, 2);
            }
        if (pathway_server)
            ABEND();
        }
    
	exit(0);
}

static void dump_log()
{
	register char	*cp,
			*cp2;

	char		*rfc931p,
			*authname,
			*rhost,
			logline[MIDLEN];

	if ( this_conp->logbuf == this_conp->log_ptr)
		return;

    if (open_wnlog () != 0)
        return;

	*(this_conp->log_ptr) = '\0';

	get_remote_info( );

#ifdef RFC931_TIMEOUT
	get_rfc931();
#endif

	rfc931p = ( *this_conp->rfc931name ? this_conp->rfc931name : "-");
	authname = ( *this_rp->authuser ? this_rp->authuser : "-");

	rhost = ( *remotehost ? remotehost : remaddr);
	rhost = ( *rhost ? rhost : "unknown");

	/*  fseek( logfp, 0L, 2); */

	cp = this_conp->logbuf;
	cp2 = logline;

	while ( *cp && ( cp2 < (logline + MIDLEN))) {
		*cp2 = *cp;
		if ( *cp2 == '\n') {
			*++cp2 = '\0';
			fprintf( logfp, "%s %s %s %s", rhost, rfc931p,
					authname, logline);
			cp2 = logline;
			cp++;
			continue;
		}
		cp2++;
		cp++;
	}
	(void) fflush( logfp);
	this_conp->log_ptr = this_conp->logbuf;

    close_wnlog ();

}

static char * getDefineFile (char * defname) {
int i;
short defineError;
char defineName[24];
char defineClass[16];
char defineAttribute[16];
char defineValue[64];
short defineValueLen;
static char fileName[64];

    strcpy (defineName, defname);
    /****Type casted to avoid warning-26/7/2000******/
    for (i=(int)strlen(defineName);i<sizeof(defineName);i++)
    /************************************************/
        defineName[i] = ' ';
    defineError = DEFINEINFO (defineName, defineClass, defineAttribute, defineValue,
                              (short) sizeof (defineValue), &defineValueLen);
    if (defineError == 0) {
        memcpy (fileName, defineValue, defineValueLen);
        fileName[defineValueLen] = '\0';
        }
    else {
        fileName[0]='\0';
        }

return fileName;
}

