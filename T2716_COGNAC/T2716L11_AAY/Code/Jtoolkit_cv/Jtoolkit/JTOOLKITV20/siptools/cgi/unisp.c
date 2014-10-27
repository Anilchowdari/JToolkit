/* **************************************************************** **
 **
 **            This material is the proprietary property of
 **         and confidential to Tandem Computers Incorporated.
 **   Disclosure outside Tandem is prohibited except by license agreement
 **                  or other confidentiality agreement.
 **
 **     Copyright (c) 1991-1993 by Tandem Computers Incorporated
 **                      All Rights Reserved
 **
 ** **************************************************************** **
 **
 */

/* **************************************************************** **
 **
 **     Source Identification
 **
 ** **************************************************************** **
 */
#ifdef _NSKOSS
#else

#include <stdlib.h>   
#include <stdio.h>  
#include <string.h> 
#include <ctype.h>  
#include <sys/stat.h> 
#include <pwd.h>   

#include <cextdecs.h(                                    \
                   FILE_CLOSE_,                          \
                   FILE_GETINFO_,                        \
                   FILE_OPEN_,                           \
                   MYTERM,                               \
                   PROCESS_CREATE_,                      \
                   PROCESS_GETINFOLIST_,                 \
                   PROCESS_STOP_,                        \
                   PROCESSHANDLE_DECOMPOSE_,             \
                   WRITEX,                               \
                   VERIFYUSER                            \
                  )>  

#include <tal.h>       

#include "wn.h"
#include "unispawn.h"

/* **************************************************************** **
 **
 **     CONSTANTS               // include macros
 **
 ** **************************************************************** **
 */

#define MAXNAMESTRLEN 255
#define MAXVALSTRLEN  255
#define MAXLOCSYM     50

#define I_PFS   0
#define I_DEBUG 1
#define NUM_I   2

/* **************************************************************** **
 **
 **     DATA
 **
 ** **************************************************************** **
 */

/* **************************************************************** **
 **
 **      FUNCTION PROTOTYPES
 **
 ** **************************************************************** **
 */

void daemon_logerr(  char* msg, char* msg2, int error);


/* **************************************************************** **
 **
 **      Externally called function to create a Tandem process.
 **      argv[0] contains the object file path name
 **      argv[1] contains start of argument list
 **      attr    contains process attributes
 **
 **      returns: 0 = success, other = success
 **
 ** **************************************************************** **
 */
int unispawn (char **argv, char * sockargv)
{
        extern char     **environ;    /* Guardian environment */
        int     i;
        short   TsizeName, TsizeVal;
        short   Tparam_msg_length;
        param_msg_type Tparam_msg;
        startup_msg_type Tstartup_msg;
        assign_msg_type  Tassign_msg;
        unsigned char   *TpParam;
        struct {
                char    volume[8];
                char    subvolume[8];
                char    file[8];
        } TfileName;
        short len;
        short   Terror;
		short   TnpError;
		unsigned short   Tcpu;
		unsigned short   Tpin;
        short   TerrorDetail;
        short   TprocessHandle[10];
        char    TprocessDesc[33];
        short   TprocessDescLen;
        short   Tfnum;
        unsigned short  Tret_attr_list = 33;  /* library file */
        short   Tret_values_len;
        struct {
                short   bytelength;
                char    libName[40];
        } Tlibrary;
        short isAssign;


        /* Construct Guardian Startup message */
        memset ((char *) & Tstartup_msg, (int) ' ', sizeof(startup_msg_type));
        Terror = get_startup_msg (&Tstartup_msg, &len);
        if (Terror == -1)
            return 990;

        /* Build argument string of startup message */
        memset (Tstartup_msg.param, ' ', sizeof(Tstartup_msg.param));
        Tstartup_msg.param[0] = '\0';
        strcat (Tstartup_msg.param, sockargv);
        strcat (Tstartup_msg.param, " ");
        strcat (Tstartup_msg.param, "-server ");
        strcat (Tstartup_msg.param, "-nlb ");
        for (i = 1; argv[i]; i++) {
                if ((strlen(argv[i]) + 1)
                     > (sizeof(Tstartup_msg.param) - 
                        strlen(Tstartup_msg.param) - 3)) {
                        return 0;        /* failure */
                }
                strcat (Tstartup_msg.param, argv[i]);
                strcat (Tstartup_msg.param, " ");
        }
        Tstartup_msg.param[strlen(Tstartup_msg.param)+1] = '\0'; /* add an extra  null */

        /* Construct Guardian ASSIGN message */
        isAssign = 1;
        Terror = get_assign_msg_by_name ("STDERR", &Tassign_msg);
        if (Terror == -1) {
			isAssign = 0;
            }

        /* Construct Guardian Param message from environment variables */
        memset ((char *) & Tparam_msg, 0, sizeof(Tparam_msg));
        Tparam_msg.msg_code   = -3;
        Tparam_msg.num_params = 0;
        Tparam_msg_length = (short) sizeof(Tparam_msg.msg_code) +
            (short) sizeof(Tparam_msg.num_params);

        TpParam = (unsigned char *) Tparam_msg.parameters;
        for (i = 0; environ[i]; i++) {
			    char var[1024];
				char * name = var;
				char * val;
                /* Don't propagate the standard environment variables that are */
                /* automatically provided by the C runtime environment */
                if (memcmp((char *)environ[i], (char *) "STDIN", 5) == 0)
                        continue;
                if (memcmp((char *)environ[i], (char *) "STDOUT", 6) == 0)
                        continue;
                if (memcmp((char *)environ[i], (char *) "STDERR", 6) == 0)
                        continue;
                if (memcmp((char *)environ[i], (char *) "DEFAULTS", 8) == 0)
                        continue;
				strncpy (var, environ[i], sizeof (var)-1);
				var[sizeof(var)-1] = '\0';
				val = strchr (var, (int) '=')+1;
				*(val-1) = '\0';
                /****Type casted to avoid warning-26/7/2000******/
		TsizeName = (short)strlen(name);
                TsizeVal  = (short)strlen(val);
		/***********************************************/
                /*  Will the parameter fit into the message ? */
                if (((unsigned char *) &(Tparam_msg.parameters[1023])-(unsigned char *) TpParam + 1)
                     < (unsigned char) (TsizeName + 1 + TsizeVal + 1)) {
                        return 992;        /* failure */
                }
                /* Add variable to the PARAM message */
                *TpParam = (unsigned char) TsizeName;
                TpParam = TpParam + 1;
                strcpy ((char *) TpParam, name);
                TpParam = TpParam + TsizeName;
                *TpParam = (unsigned char) TsizeVal;
                TpParam = TpParam + 1;
                strcpy ((char *)TpParam, val);
                TpParam = TpParam + TsizeVal;
                Tparam_msg.num_params++;
                Tparam_msg_length = (short)(Tparam_msg_length + TsizeName + TsizeVal + 2);
        }

#ifdef DEBUG
        TpParam = (unsigned char *) Tparam_msg.parameters;
        for (i = 0; i < Tparam_msg.num_params; i++) {
                unsigned char   *TpNameLen;
                unsigned char   *TpValLen;
                char    Tname[MAXNAMESTRLEN+1];
                char    Tval [MAXVALSTRLEN+1];
                TpNameLen = TpParam;
                TpValLen  = TpParam + *TpNameLen + 1;
                memcpy (Tname, TpNameLen + 1, *TpNameLen);
                Tname[*TpNameLen] = '\0';
                memcpy (Tval,  TpValLen + 1, *TpValLen);
                Tval[*TpValLen] = '\0';
                fopen_std_file (2,0);
                fprintf (stderr, "\r\n(%d)%s=(%d)%s",
                    (int) *TpNameLen, Tname,
                    (int) *TpValLen,  Tval);
                TpParam = TpParam + *TpNameLen + *TpValLen + 2;
        }
        fprintf (stderr, "\r\n");
#endif

        /* Get the library name (to propagate to the new process) */
        /* If no library is returned, then Tlibrary.bytelength will be zero  */
        Terror   = PROCESS_GETINFOLIST_ (,        /* CPU */
        ,         /* PIN */
        ,         /* Node */
        ,         /* Node length */
        ,         /* Process handle */
        (short *) & Tret_attr_list,
            1,        /* attribute count */
        (short *) & Tlibrary,
            sizeof (Tlibrary) / 2, /* in short words */
        (short *) & Tret_values_len,
            (short *) & TerrorDetail);

        if (Terror) {
                return Terror;    /* failure */
        }

        /* Create the new process */
        TnpError = PROCESS_CREATE_ (argv[0],  /* program file  */
        (short)strlen(argv[0]), /* length */
        ,
        ,
/*        (char *)Tlibrary.libName,    */
/*        (short) Tlibrary.bytelength,  */  
        ,                /* swap file */
        ,                /* swap file length */
        ,                /* ext swap file */
        ,                /* ext swap file length */
        ,                /* priority */
        ,                /* processor */
        TprocessHandle,  /* process handle */
        &TerrorDetail,   /* error detail */
        2,               /* system generated name */
        ,                /* process name */
        ,                /* process name length */
        TprocessDesc,    /* process descriptor */
        sizeof(TprocessDesc), /* max length */
        &TprocessDescLen, /* actual length */
        ,                /* nowait tag */
        ,                /* home term */
        ,                /* home term len */
        ,                /* memory pages */
        ,                /* job id */
        6,               /* create options */
                         /*  6=propagate defines(always) */
        ,                /* defines */
        ,                /* defines len */
        ,            /* debug option */
                         /* PFS size */
        );


        if (TnpError) {
             daemon_logerr( err_m[116], "unispawn: PROCESS_CREATE_ failure", (int)TnpError);
                return TnpError;        /* failure */
			}

        /* Open the created process */
        Terror = FILE_OPEN_ (TprocessDesc,
            TprocessDescLen,
            &Tfnum);
        if (Terror) {
             daemon_logerr( err_m[116], "unispawn: FILE_OPEN_ failure", (int) Terror);
                PROCESS_STOP_ (TprocessHandle);
                return Terror;        /* failure */
        }

        /* Send environment messages to the new process */
        /* The order is STARTUP, then ASSIGN, then PARAM  */

        Terror = (short)WRITEX (Tfnum, (char *) & Tstartup_msg,sizeof(Tstartup_msg));
        Terror = FILE_GETINFO_ (Tfnum, &TerrorDetail);
        if (TerrorDetail == 0 || TerrorDetail == 70) {
            if (isAssign) {
		Terror = (short)WRITEX (Tfnum, (char *) & Tassign_msg, sizeof(Tassign_msg));
		Terror = FILE_GETINFO_ (Tfnum, &TerrorDetail);
                }
             }
        if ((TerrorDetail == 0 || TerrorDetail == 70) && Tparam_msg.num_params) {
                Terror = (short)WRITEX (Tfnum, (char *) & Tparam_msg, Tparam_msg_length);
                Terror = FILE_GETINFO_ (Tfnum, &TerrorDetail);
        }
        if (TerrorDetail != 0 && TerrorDetail != 70) {
             daemon_logerr( err_m[116], "unispawn: WRITEX failure", (int) TerrorDetail);
                PROCESS_STOP_ (TprocessHandle);
                return TerrorDetail;        /* failure */
        }

        /* Close the created process */
        FILE_CLOSE_ (Tfnum);

        /* Get CPU and pin) */
        Terror = PROCESSHANDLE_DECOMPOSE_ (TprocessHandle, 
                 (short *)&Tcpu, (short *)&Tpin);
        if (Terror) {
             daemon_logerr( err_m[116], "unispawn: PROCESSHANDLE_DECOMPOSE_ failure", (int) Terror);
                PROCESS_STOP_ (TprocessHandle);
                return Terror;        /* failure */
        }

        return 0;
}

#endif /* _NSKOSS */

/* **************************************************************** **
 **      END OF FILE                                                 **
 ** **************************************************************** **
 */
