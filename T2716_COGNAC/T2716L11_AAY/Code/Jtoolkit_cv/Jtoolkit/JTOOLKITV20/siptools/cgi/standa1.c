/*
    Wn: A Server for the HTTP
    File: wn/standalone.c
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <setjmp.h>
#include <cextdecs.h(FILE_OPEN_                     \
                     ,FILE_CLOSE_                   \
                     ,SETMODE                       \
                     ,DELAY                         \
                     ,DEBUG                         \
                     ,AWAITIOX                      \
                     ,FILE_GETINFO_                 \
                     ,READUPDATEX                   \
                     ,FILE_GETRECEIVEINFO_          \
                     ,REPLYX                        \
                     ,SERVERCLASS_SEND_             \
                     ,SERVERCLASS_SEND_INFO_        \
                     ,DEFINEINFO                    \
                     ,ADDRESS_DELIMIT_              \
                     ,ABEND                         \
                    )>

#ifdef _NSKOSS
#include <spawn.h>
#include <tdmext.h>
extern char ** environ;
#else
#include "unispawn.h"
#endif

#define _CGI_NO_EXTERNALS
#include "cgilib.h"

#include "uniguard.h"
#include "wn.h"

#define QUEBACKLOG      (1024)

extern int      daemon_init;
extern char tcpipProcess[SMALLLEN];
jmp_buf jump_environment;

extern FILE     *logfp;

static int      need_mime();

void do_connection (int sock);

void do_server_mode (char * snw2addr, char * inet_name);
static char * pathsendErrorText (short ps_error);
int pathsendSocket (char * snw2addr, char * inet_name);
static int pathsendDrainCompletions(short filenum, long * tag, long timelimit) ;

int initgroups(char *gp_name, gid_t group_id);

static char     *mtypebase = NULL;
static short ps_scopnum = -2;
int pathway_reply_needed;


/******************************************************************/


void do_standalone(char *argv[])
{
        char    mbuf[2*SMALLLEN];
#ifdef _TNS_X_TARGET
        int     sockdes;
    int sd;
#else
        short   sockdes;
    short       sd;
#endif
    int len,
                kav = 1,
                nagle = 1,
                on = TRUE;
    unsigned long listenRetryCount;
    /**** Changed short to int to avoid warning 26/7/2000****/
    int ret;
    /********************************************************/
    int error;
    long tag;
    char sockargv[64];

        struct linger ling;
        struct sockaddr_in      sa_server,
                                sa_client;


/* ----------------------------------------------------------------------------- */

if (server_mode && pathway_server == 1) {                 /* get requests from $receive */
    static short firstTime=1;
    static short open_count = 0;
    static short recvfile;
    static short error;
    static short last_error;
    static short receiveinfo[17];
    static short count_transferred;
    static short reply_count;
    static long  tag;
    static char reqbuf[256];    /* request message buffer */
    static char * tcpproc;
    static char snw2addr[32];
    static int ret = 0;
    static short replyx_error = 0;
    static int retsj = 0;

    if (firstTime==1) {
        firstTime = 0;
        /*  Open $receive */
        error   = FILE_OPEN_ ("$RECEIVE"
                      ,(short) strlen("$RECEIVE")
                      ,&recvfile
                      ,       /* access */
                      ,       /* exclusion */
                      ,       /* nowait depth */
                      ,1);    /* receive depth */
        if (recvfile == -1) {
                    daemon_logerr( err_m[117], "Error opening $RECEIVE", (int) error);
            exit(1);
            }
        }

     /***** Add message to debug log - 5/6/2000 ********/
     if(debug_log)
        write_debug( 1, "Standalone:","Under Pathmon");
    /***************************************************/

    /* Process requests */
    for (;;) {
        READUPDATEX (recvfile,
                     (char *) reqbuf,
                     (short) sizeof (reqbuf),
                     &count_transferred);
        error = FILE_GETINFO_(recvfile, &last_error);
        FILE_GETRECEIVEINFO_ ((short *) &receiveinfo);
        if (last_error == 6)  {            /* system message */
            short sys_msg;
            memcpy ((char *)&sys_msg, reqbuf, 2);
            switch (sys_msg) {
                case -103:
                    open_count++;
                    break;
                case -104:
                    open_count--;
                    break;
                }
            REPLYX (reqbuf, receiveinfo[1]);
            if (sys_msg == -104 && open_count == 0) {  /* last close message */
                (void) CGI_terminate();
                exit(0);    /* out of the for loop reading $receive */
                }
            else
                continue;                  /* ignore other system messages */
            }
        if (last_error != 0) {
            daemon_logerr( err_m[117], "Error returned from READUPDATEX of $RECEIVE",
                     (int) last_error);
                        (void) CGI_terminate();
            exit (1);
            }

        pathway_reply_needed = 1;

        tcpproc = strchr (reqbuf,(int)'/');
        if (tcpproc) {
            int len;
            len = tcpproc - reqbuf;
            memcpy (snw2addr, reqbuf, len);
            snw2addr[len] = 0;
            tcpproc++;
            }
        else {
            strcpy (snw2addr, reqbuf);
            tcpproc = tcpipProcess;
            }

        alarm(0); /* unset alarm */

        retsj = 0;
        retsj = setjmp(jump_environment);
        if (retsj == 0)
            do_server_mode (snw2addr,tcpproc);

        pathway_reply_needed = 0;
        alarm(0); /* unset alarm */

        /* reply to the requester  */
        if (retsj != 1)
            replyx_error = 300; /* force pathway to disable the server */
        else
            replyx_error = 0;
        reply_count = 0;
        REPLYX ((char *) &reply_count, reply_count,,,replyx_error);
        error = FILE_GETINFO_(recvfile, &last_error);
        if (last_error != 0) {
           daemon_logerr( err_m[117], "REPLYX returned error", (int) last_error);
                        (void) CGI_terminate();
            exit (1);
            }
        if (retsj != 1) {  /* reply before abending on non-recoverable errors */
                        (void) CGI_terminate();
            ABEND();
            }
        }
    }

/* ----------------------------------------------------------------------------- */

/***** Change condition to disallow TCP/IP Listner to enter here - 7/6/2000 ********/
if (server_mode && pathway_server == 0 && pathmon[0]==0) {
    /*  we're a server, not a listener */

    /***** Add message to debug log - 5/6/2000 ********/
    if(debug_log)
        write_debug( 1, "Standalone:","Under Server");
    /**************************************************/

    do_server_mode(snw2addr,tcpipProcess);
    exit(0);
    }

/* ----------------------------------------------------------------------------- */

if (server_mode && pathmon[0] != 0) {
    /*  we're a server,  but have a pathmon configured, so hand off the request */
    /*  This mode allow the TCP/IP listner to be used                           */
        int ret;


        char *tstr;
        char address[32];

     /***** Fixed bug in address field to remove 'd'-09/06/2000 ********/
        address[0] = '\0';
        tstr = strstr(snw2addr,"d");
        if (tstr){
           int i,j=0;
           for (i=0;i<=strlen(snw2addr);i++){
                if(snw2addr[i] !='d')
                        address[j]=snw2addr[i];
                else {
                        address[j]=snw2addr[i+1];
                        i++;
                }
                j++;
                }
           address[j]='\0';
        }

     /**************************************************/
     /***** Add message to debug log - 5/6/2000 ********/
        if(debug_log)
                write_debug( 1, "Standalone:","Listner under Pathmon");
     /**************************************************/

        ret = pathsendSocket (address, tcpipProcess);
        exit(0);
        }

/* ----------------------------------------------------------------------------- */

/* We're a listener */

#define LISTEN_RETRY_MSG_MAX_COUNT 128
#define LISTEN_RETRY_DELAY_SECONDS 5
#define LISTEN_RETRY_MSG_INTERVAL_SECONDS 3600
#define LISTEN_RETRY_MSG_INIT_BURST 32
#define LISTEN_RETRY_MSG_EXPRESSION(count) \
          (daemon_init==0 || \
           count < LISTEN_RETRY_MSG_INIT_BURST || \
          (count%(LISTEN_RETRY_MSG_INTERVAL_SECONDS/LISTEN_RETRY_DELAY_SECONDS)==0))
#define LISTEN_RECOVER(s) { long tag=1; if (daemon_init==0) exit(2); \
                            shutdown_nw(s,2,tag); cgilib_tcpwait (s, &tag, 500); \
                            FILE_CLOSE_((short) s); DELAY(500); continue; }

if (tcpipProcess[0] != '\0') {
        socket_set_inet_name (tcpipProcess);
        }

for (listenRetryCount = 0; ;listenRetryCount++) {

        if ((sd = socket_nw (AF_INET,SOCK_STREAM,SOCK_STREAM,0200|017,0)) == -1) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[22], "socket_nw: listen", errno);
                LISTEN_RECOVER(sd);
        }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[22], "socket_nw: listen; awaitio", error);
                LISTEN_RECOVER(sd);
        }

    SETMODE ((short) sd, (short) 30, (short) 1, (short) 0);

        ling.l_onoff = ling.l_linger = 0;

#ifndef NO_LINGER
        if ( setsockopt_nw( sd, SOL_SOCKET, SO_LINGER, (char *) &ling,
                                            sizeof (ling),1) == -1) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[23], "setsockopt_nw: listen; linger", errno);
                LISTEN_RECOVER(sd);
        }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[23], "setsockopt_nw: listen; linger; awaitio", error);
                LISTEN_RECOVER(sd);
        }
#endif

        if ( (setsockopt_nw ( sd, SOL_SOCKET, SO_REUSEADDR,
                                        (char *) &on, sizeof( on),1)) == -1) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[24], "setsockopt_nw: listen; reuseaddr", errno);
                LISTEN_RECOVER(sd);
        }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[24], "setsockopt_nw: listen; reuseaddr; awaitio", error);
                LISTEN_RECOVER(sd);
        }


#ifndef DO_NAGLE
        if ( setsockopt_nw(sd, IPPROTO_TCP, TCP_NODELAY,
                                (char*)&nagle, sizeof(nagle),1)) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[105], "setsockopt_nw: listen; nagle", errno);
                /* Not fatal */
        }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[105], "setsockopt_nw: listen; nagle; awaitio", error);
        }
#endif /* DO_NAGLE */

        if ( (setsockopt_nw( sd, SOL_SOCKET, SO_KEEPALIVE,
                                        (char *) &kav, sizeof( kav),1)) == -1) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[98], "setsockopt_nw: listen; keepalive", errno);
                LISTEN_RECOVER(sd);
        }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[98], "setsockopt_nw: listen; keepalive; awaitio", error);
                LISTEN_RECOVER(sd);
        }

        memset((char *)&sa_server, 0, sizeof( sa_server));
        sa_server.sin_family = AF_INET;
        sa_server.sin_port = htons( port);

        {
                struct hostent *hptr;

                if ( *hostname && ((hptr = gethostbyname(hostname)) != NULL)) {
                        bcopy(hptr->h_addr,
                        (char *) &(sa_server.sin_addr.s_addr), hptr->h_length);
                }
                else {
                        if ( *hostname) {
                                sprintf( mbuf, err_m[91], hostname);
                                strcat( mbuf, err_m[92]);
                                daemon_logerr( mbuf, "gethostbyname: listen", errno);
                if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                            daemon_logerr( err_m[25], "bind_nw: listen",  errno);
                        LISTEN_RECOVER(sd);
                        }
                        sa_server.sin_addr.s_addr = htonl( INADDR_ANY);
                }
        }

        if( bind_nw( sd, (struct sockaddr *) &sa_server,
                        sizeof(sa_server),1) == -1) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[25], "bind_nw: listen",  errno);
                LISTEN_RECOVER(sd);
            }
    error = cgilib_tcpwait (sd, &tag, -1);
    if (error != 0) {
        if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                    daemon_logerr( err_m[25], "bind_nw: listen; awaitio", error);
                LISTEN_RECOVER(sd);
            }

    daemon_init=1;

    for (;;) {

            listen( sd, QUEBACKLOG);

         len = sizeof(sa_client);
         if ( (sockdes = accept_nw( sd, (struct sockaddr *) &sa_client,
                                                                  &len,1)) == -1 ) {
                    if ( errno == EINTR)
                continue;
            else {
                if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                                daemon_logerr( err_m[28], "listen: accept_nw", errno);
                break;  /*  to recover listen */
                }
            }

        if (pathmon[0]) {
            tag = 0;
            error = pathsendDrainCompletions(-1, &tag, -1);
            if (tag != 1) {  /* not a socket completion */
                if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                     daemon_logerr( err_m[28], "listen: pathsendDrainCompletions", error);
                break;   /* the only possibility for pathsend is an error exit */
            }        /*  to recover listen */

     /***** Add message to debug log - 5/6/2000 ********/
            if(debug_log && error ==0)
                write_debug( 1, "Standalone:","Pathway Server started");
            }
     /**************************************************/

        else {
            error = cgilib_tcpwait (sd, &tag, -1);

     /***** Add message to debug log - 5/6/2000 ********/
            if(debug_log && error == 0)
                write_debug( 1, "Standalone:","Connected to server");
             }
     /**************************************************/

        if (error != 0) {
            if (error == EINTR)
                continue;
            else {
                if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                            daemon_logerr( err_m[28], "listen: accept_nw; awaitio", error);
                        break;    /*  to recover listen */
                    }
            }

        listenRetryCount = 0;

        sprintf (sockargv, "%hu.%s",
                           sa_client.sin_port,
                           inet_ntoa (sa_client.sin_addr));

        if (pathmon[0]) {
            int ret;
            ret = pathsendSocket (sockargv, tcpipProcess);
            if (ret == 1)
                continue;
            else  {
                if (LISTEN_RETRY_MSG_EXPRESSION(listenRetryCount))
                                daemon_logerr( err_m[28], "listen: pathsendSocket", ret);
                break;
                }
            }

#ifdef _NSKOSS
                {
                pid_t pid;
                char * ts_argv[51];
                int i,j;
                struct inheritance inherit;

                memset ((char *)&inherit, 0, sizeof(inherit));
                ts_argv[0] = argv[0];
                ts_argv[1] = sockargv;
                ts_argv[2] = "-server";
                ts_argv[3] = "-nlb";

                for (i=1, j=4; argv[i] && j<50;)
                        ts_argv[j++] = argv[i++];
                ts_argv[j] = NULL;


        pid = tdm_spawn (argv[0],
                                         0,                /* fd count */
                                             NULL,
                                                 &inherit,  /* signal inheritance */
                                                 ts_argv,
                                                 environ,
                                                 NULL,
                                                 NULL);
            if (pid == -1) {
                    daemon_logerr (err_m[29],
                             "Process creation or initialization failure.", (int) errno);
            exit (1);
            }
                }
#else
        ret = unispawn (argv, sockargv);
            if (ret != 0) {
                    daemon_logerr (err_m[29],
                             "Process creation or initialization failure.", (int) ret);
            exit (1);
            }
#endif
        }


    LISTEN_RECOVER (sd);
    }

}

/* ----------------------------------------------------------------------------- */


void do_server_mode (char * snw2addr, char * inet_name) {
        char    mbuf[2*SMALLLEN];

#ifdef _TNS_X_TARGET
        int     sockdes;
    int sd;
#else
        short   sockdes;
    short       sd;
#endif
    int len,
                kav = 1,
                nagle = 1,
                on = TRUE,
        listenRetryCount;
        short ret;
    int error;
    long tag;

        struct linger ling;
        struct sockaddr_in      sa_server,
                                sa_client;

        if (inet_name[0] != '\0') {
                socket_set_inet_name (inet_name);
                }

        sscanf (snw2addr, "%hu.", &sa_client.sin_port);
        sa_client.sin_family = AF_INET;
        sa_client.sin_addr.s_addr = inet_addr (strchr(snw2addr, '.')+1);

        sockdes = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (sockdes == -1) {
                    daemon_logerr( err_m[22], "socket: server", errno);
                    exit(2);
            }

            if ( (setsockopt( sockdes, SOL_SOCKET, SO_KEEPALIVE,
                                        (char *) &kav, sizeof( kav))) == -1) {
                    daemon_logerr( err_m[98], "setsockopt: server; keepalive", errno);
                    exit(2);
                }

        for (;;) {
            error = accept_nw2 (sockdes, (struct sockaddr *)&sa_client, 1);
            if (error == -1 && errno == EINTR) {
                continue;
                }
            break;
            }
        if (error == -1) {
            daemon_logerr( err_m[28], "accept_nw2: server", errno);
            if (pathway_server) {
            /***Type casted sockdes to short to avoid warning-26/7/2000*****/
                FILE_CLOSE_((short)sockdes);
            /*******************************************************/
                longjmp(jump_environment,1);
                }
            exit(2);
            }


        get_local_info( sockdes);

        do_connection(sockdes);
        /***Type casted sockdes to short to avoid warning-26/7/2000*****/
        FILE_CLOSE_((short)sockdes);
        /******************************************************/
                if (pathway_server)
                        longjmp(jump_environment,1);

        return;
        }

/* ----------------------------------------------------------------------------- */

int pathsendSocket (char * snw2addr, char * inet_name) {
    short ps_error;
    short ps_pathsend_error;
    short ps_file_system_error;
    short ps_actual_reply_len;
    long ps_timeout = -1;   /* infinite wait */
    short flags = 1;
    int sendLength;
    char * reqbuf;

    reqbuf = malloc(64);
    strcpy (reqbuf, snw2addr);
    if (inet_name[0]) {
        strcat (reqbuf, "/");
        strcat (reqbuf, inet_name);
        }

        /* Send the message to the server class */
        ps_error = SERVERCLASS_SEND_ (
                        (char *) pathmon,
                        (short) strlen (pathmon),
                        (char *) serverClass,
                        (short) strlen (serverClass),
                        (char *) reqbuf,
   (short) (strlen (reqbuf)+1),
     (short) 0,
                        (short *) &ps_actual_reply_len,
                        (long) ps_timeout,
                        (short) flags,
                        (short *) &ps_scopnum,
                        (long)  reqbuf
                        );

 /***** Add message to debug log - 5/6/2000 ********/
        if (debug_log && ps_error ==0){
                write_debug(1, "Standalone: ","Serverclass send executed");
        }
        /**************************************************/

        ps_pathsend_error=0;
        ps_file_system_error=0;
        ps_error = SERVERCLASS_SEND_INFO_ (
                        (short *) &ps_pathsend_error,
                        (short *) &ps_file_system_error);


        if (ps_pathsend_error == 0) {
            return 1;
            }
        else {
                    daemon_logerr( err_m[117], pathsendErrorText(ps_pathsend_error), (int) ps_file_system_error);
            return 0;
            }

    }

/*****************************************************************************/

static int pathsendDrainCompletions(short filenum, long * tag, long timelimit)
{
short ps_error;
short ps_pathsend_error;
short ps_file_system_error;
 /***Changed error_num type to int avoid warning-26/7/2000*****/
int error_num=0;
/*************************************************************/
short addr_err=0;
short latest_ps_error = 0;

if (filenum == -2)   /* uninitialized ps_scopnum */
    return 0;

for (;;) {
    *tag = 0;
    /****Type casted filenum to avoid warning 26/7/2000*****/
    error_num = cgilib_tcpwait((int)filenum, tag, timelimit);
    /*******************************************************/
    if (*tag == 1)     /*  socket completion */
        return error_num;

    if (error_num == 26)
        break;

    if (error_num == 40)
        break;

    if (*tag > 0) {
            addr_err = ADDRESS_DELIMIT_(*tag);
            if (addr_err == 0)
                    free ((char *) (*tag));
            else {
            char msg[128];
            sprintf (msg, "Error calling ADDRESS_DELIMIT_ (%hd) on PATHSEND tag (%ld) awaitio error (%hd)",
                          addr_err, *tag, error_num);
                    daemon_logerr( err_m[117], msg , 0 );
            *tag = 0;
            }
        }

    if (error_num == 0) {
        continue;
        }

    if (error_num == 233) {
        ps_pathsend_error=0;
        ps_file_system_error=0;
        ps_error = SERVERCLASS_SEND_INFO_ (
                        (short *) &ps_pathsend_error,
                        (short *) &ps_file_system_error);
        if (ps_file_system_error == 300)    /* server will kill itself */
            continue;
                daemon_logerr( err_m[117], pathsendErrorText(ps_pathsend_error), (int) ps_file_system_error);
        latest_ps_error = ps_pathsend_error;
        break;
        }
       /*****Removed type cast of error_num-26/7/2000*********/
        daemon_logerr( err_m[117], "Error calling AWAITIOX on PATHSEND", error_num);
      /******************************************************/
    return error_num;
    }

return latest_ps_error;
}


/***********************************************************************/

static char * pathsendErrorText (short ps_error)
{
switch (ps_error) {
   case  900: return ("Invalid server class name.");
   case  901: return ("Invalid Pathmon process name.");
   case  902: return ("Error with Pathmon connection (eg. Open, I/O, etc.).");
   case  903: return ("Unknown message received from PATHMON.");
   case  904: return ("Error with Server Link connection (eg. Open, I/O, etc. ).");
   case  905: return ("No Server Available.");
   case  906: return ("the user called SC_SEND_INFO before ever calling SC_SEND_.");
   case  907: return ("The caller is using an extended segment id that is out of range.");
   case  908: return ("The caller supplied a reference parameter which is an extended address, but doesn't have an extended segment in use.");
   case  909: return ("The caller set bits in the flags parameter that are reserved and must be 0.");
   case  910: return ("A required parameter was not supplied.");
   case  911: return ("One of the buffer length parameters is invalid.");
   case  912: return ("A reference parameter is out of bounds.");
   case  913: return ("The Server Class is Frozen.");
   case  914: return ("PATHMON does not recognize Server Class name.");
   case  915: return ("Send denied because Pathmon is shutting down.");
   case  916: return ("Send denied by PATHMON because of Server creation failure.");
   case  917: return ("The Tmf Transaction mode of the Send does not match that of the ServerClass (eg.  Requester Send has a TransId and the ServerClass is configured with TMF OFF).");
   case  918: return ("Send operation aborted. See accompanying Guardian error for more information.");
   case  919: return ("The caller supplied an invalid timeout value.");
   case  920: return ("The caller's PFS segment could not be accessed.");
   case  921: return ("The maximum number of Pathmons allowed has been exceeded.");
   case  922: return ("The maximum number of server classes allowed has been exceeded.");
   case  923: return ("The maximum number of allowed server links has been exceeded.");
   case  924: return ("The maximum number of allowed send requests has been exceeded.");
   case  925: return ("The maximum number of allowed requesters has been exceeded.");
   case  947: return ("Error with LINMMON connection (eg. Open, I/O, etc. ).");
   default: {static char szMsg[32]; sprintf (szMsg, "Pathsend error: %hd.", ps_error); return szMsg;}
   }

}

/*******************************************************************************/



