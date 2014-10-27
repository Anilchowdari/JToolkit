/*---------------------------------------------------------------------------
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/




#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <assert.h>
/* The D40 compilers require that external definitions are defined
	once and only once in an object module.
*/
#define _CGI_NO_EXTERNALS
#include <cgilib.h>
#include <cextdecs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include "util.h"
#include "access.h"
#include "cgi_sip.h"
#include "pmproc.h"
#include "pmspi.h"
#ifdef _VPROC
void _VPROC(void) {};
#endif
static char *errmsg;
char msg[500];
char confFileName[2096];
char confDir[1095];
char confFile[100];
char generalInfoFile[20];

// Begin Changes for Solution Number 10-040512-5972

short cpulists[CPUSPERNODE];

// to append  with the Server Class name 

char *append[CPUSPERNODE]={"","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

// End Changes for Solution Number 10-040512-5972


/*********************************************************************
 * void get_pathway_info()
 *
 *	retrieves the information needed to configure or reconfigure
 *  the pathway application.  Also determines what type of
 *  configuration is being requested.
 **********************************************************************/

void get_pathway_info ()
{
	struct stat statBuf;

// Begin Changes for Solution Number 10-040512-5972

   // int has been changed to short 

	short result;
	short type;
        FILE *pfp;

// End Changes for Solution Number 10-040512-5972

	int err;
	char checkFile[256];
	char cmd[256];

    type = getType();
	if(type == 0) {
		util_display_error_begin_form();
		strcpy(msg,"You must choose a configuration option; either 'Configure and Start' or 'Configure Pathway'");
		util_display_error_msg_form(msg);
		strcpy(msg,"Use the browser back key to return to the Configuration Tool and choose an option");
		util_display_error_msg_form(msg);
		util_display_error_end_form();
		return;
	}

	result = getPathwayFields();
	if(result == FALSE) {
		util_display_error_form("Error in entry fields");
		return;
	}

	if(stat(confDir, &statBuf) != 0) {
		err = errno;
		if(err == ENOENT) {
			err = mkdir(confDir, 0777);
			if(err == -1) {
				err = errno;
				errno_errMsg("Unable to create configuration directory<BR>",err,confDir);
				return;
			}
		} else {
			errno_errMsg("Unable to create configuration directory<BR>", err,confDir);
			return;
		}
	} else {
		if(!(statBuf.st_mode&S_IFDIR)) {
			util_display_error_form("The path provided for the configuration directory is a file name not a directory");
			return;
		}
		sprintf(checkFile,"%s/.zzTestSip",confDir);
		pfp = fopen(checkFile,"a+");
		if(pfp == NULL) {
			errno_errMsg("Unable to write to configuration directory<BR>",errno, confDir);
			return;
		} else {
			fclose(pfp);
			sprintf(cmd,"rm %s",checkFile);
			if(errno = system(cmd)) {
                           errno_errMsg("Unable to remove the checkFile",errno,checkFile);
                        }   
		}
	}
	if(stat(initialSipInfo.logPath, &statBuf) != 0) {
		if(errno == ENOENT) {
			err = mkdir(initialSipInfo.logPath, 0777);
			if(err == -1) {
				errno_errMsg("Unable to create logpath directory<BR>",errno,initialSipInfo.logPath);
				return;
			}
		} else {
			errno_errMsg("Unable to create logpath directory<BR>", err,initialSipInfo.logPath);
			return;
		}
	} else {
		if(!(statBuf.st_mode&S_IFDIR)) {
			util_display_error_form("The path provided for the logpath directory is a file name not a directory");
			return;
		}
	}
	initPmSpi();
	if(type == REVIEWIT) {
		writeServerPage(REVIEWIT);
		return;
	}
	if(type == DELETEIT) {
		deleteApplication();
	} else {
		result = doInitialConfiguration(TRUE, type);
		if(result != 0) {
			util_display_error_form(msg);
			return;
		}
	}
	return;
}
/*********************************************************************************
 * char * errno_errMsg(char *BeginText, int err, char *pathName)
 *	params:
 *		BeginText - some text that identifies what you were trying to do when the error happened
 *		err - the value of errno
 *		fileName - the name of the file on which the error occurred.
 *
 *	displays the appropriate error message.
 *********************************************************************************/
 void errno_errMsg(char *BeginText, int err, char *pathName)
 {
 	static char message[3000];

	switch(err) {
		case EACCES:
			sprintf(message,"%s %d: Access denied to %s",BeginText,err, pathName);
			util_display_error_form(message);
			return;
		case EFAULT:
			sprintf(message,"%s %d: Internal error occurred trying to access %s",BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case EFSBAD:
			sprintf(message,"%s %d: The fileset is corrupt for %s",BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case EIO:
			sprintf(message,"%s %d: An input or output error occurred when accessing %s",
				BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case ELOOP:
			sprintf(message,"%s %d: Too many symbolic links were encountered in the translating path %s",
				BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case ENAMETOOLONG:
			sprintf(message,"%s %d: A component of the pathname %s is too long",
				BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case ENOROOT:
			sprintf(message,"%s %d: The root of the fileset for %s is not in started state or is unavailable",
				BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case ENOTDIR:
			sprintf(message,"%s %d: A component of the file name %s is not a directory",
				BeginText, err, pathName);
			util_display_error_form(message);
			return;
		case ENFILE:
			sprintf(message,"%s %d: Too many files are open in the system", BeginText, err);
			util_display_error_form(message);
			return;
		case ENOMEM:
			sprintf(message,"%s %d: Insufficient space to allocate a buffer", BeginText, err);
			util_display_error_form(message);
			return;
		case EINVAL:
			sprintf(message,"%s %d: Internal error, Invalid function argument", BeginText, err);
			util_display_error_form(message);
			return;
		default:
			sprintf(message,"%s %d occurred when accessing %s", BeginText, err, pathName);
			util_display_error_form(message);
			return;
	}
}



/********************************************************************************
 * doInitialConfiguration
 *
 *	params:
 *		writeFiles - either TRUE or FALSE
 *		type -  one of CONFIGUREIT, RECONFIGUREIT, RESTARTIT, CONFIGUREONLY
 *				or RECONFIGUREONLY
 *
 *	Creates the pathway configuration
 ***********************************************************************************/
int doInitialConfiguration(short writeFiles, short type)
{
        FILE *pfp;
	short error_detail, error;
	int result = -1;
	short info, detail;
	char pathmonfname[36];
	short numServerClasses = 0;
	int status;
	char *ptr;

	if(type == RECONFIGUREIT || type == CONFIGUREIT || type == CONFIGUREONLY) {
		status = pmproc_running();
		if(status) {
			if(type == CONFIGUREIT) {
				sprintf(msg,"A process named %s is already running.  Either use the browser back button and choose another Pathmon Name or use the Reconfiguration Tool",
					initialSipInfo.pathmonName);
				return -1;
			} else {
				initPmSpi();
				/* It's running stop it so we can reconfigure */
				result = pmspi_shutdown(&error, &error_detail);
				if(result != 0) {
					errmsg = pmspi_errormsg(result,error,error_detail);
					strcpy(msg,errmsg);
					return -1;
				}
			}
		} else {
			if(type == CONFIGUREIT || type == CONFIGUREONLY) {
				status = doesPMexist();
				if(status) {
					sprintf(msg,"A process named %s is already configured.  Use the browser back button and choose another Pathmon Name or use the Reconfiguration Tool",
								initialSipInfo.pathmonName);
					return -1;
				}
			}
		}
	}

	if(type == RESTARTIT)
		type = CONFIGUREIT;

	if(type  == CONFIGUREONLY  || type == RECONFIGUREONLY) {
	} else {
		info = detail = 0;
		result = pmproc_create(pathmonfname, &info, &detail);
		if(result == 1) {
			numServerClasses = MAXSCS;
			initPmSpi();
			result = pmspi_coldstart((short)initialSipInfo.pmBackUpCpu,
				numServerClasses, &info, &detail);
			if(result != 0) {	/* Write error detail info */
				sprintf(msg,"Unable to start pathmon: error = %d; detail_error %d",
					info, detail);
				return -1;
			}
		} else {
			errmsg = pmproc_errormsg(info,detail);
			strcpy(msg,errmsg);
			return -1;
		}
	}
	if(writeFiles) {
		ptr = (char *)&initialSipInfo.pathmonName[1];
		/* change made for solution no. 10-070910-7331 */
		sprintf(confFile,"%s_pathway_cold.pwy",ptr);
		sprintf(confFileName,"%s/%s",confDir,confFile);
		initPmSpi();
		if(((pmspi_writePMInfo(confFileName,(short)initialSipInfo.pmBackUpCpu, numServerClasses)) == 1) &&
				((writeInitialExports(confDir)) == 1)) {
		} else {
			errno_errMsg("Unable to write configuration information",errno,"Pathway Configuration Files");
			return -1;;
		}
		writePathwayCommandsFiles(type);
		if(type == CONFIGUREIT || type == CONFIGUREONLY) {

                  // Begin Changes for Solution Number 10-040512-5972

                     /* Replacing writeGeneralInfo function to write into the SIP General Info file using the structure generalFileInfo */
 
                        strcpy(generalFileInfo.pathmonName,initialSipInfo.pathmonName);
                        strcpy(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY");
                        generalFileInfo.sernum = 0;
                        pfp = openGeneralInfoFile("ab");
                        if(pfp == NULL) {
                            util_display_error_form("Unable to open General Info File"); 
                            return -1;
                        }
                        fwrite(&generalFileInfo,sizeof(generalFileInfo),1,pfp);
                        fclose(pfp);

                  // End Changes for Solution Number 10-040512-5972

			initialSipInfo.currentServer = 1;
		}
		if(type == RECONFIGUREIT || type == RECONFIGUREONLY) {
			displayReconfigurePwayFinish(type);
		} else {
			if(initialSipInfo.numSipSC > 0)
				writeServerPage(type);
			else
				displayConfigurationFinished(type);

		}
	}
	return 0;
}

// Begin Changes for Solution Number 10-040512-5972

/**********************************************************************************
 * void configureServer
 *	extracts information from the screen then calls routines to configure distributor and
 *  the Sip Server.
 **********************************************************************************/
void configureServer()
{
	int error;
	char *mode = NULL;
	char *conf;
	short result;
        short i;
	short type = 0;
	short err1, err2;
	int addServerToExistingConf = 0;
	int status;
        short entry = FALSE ;
        short sernum = 0;
        short sclen = 0;
        char *sccpus;
        char *scpus;
        char *svrname;
        char *ptr;
        short display = FALSE;

// get the number of CPU's that are up and the CPU's that are up

         getCpuUpList();


// Check CPU is not null

         scpus = util_form_entry_value ("SCCPUS");

// Store original value of scpus as strtok puts null 
         sclen = strlen(scpus);
         sccpus = (char*)malloc(sclen + 1);
         memset(sccpus,'\0',sclen+1);
         strncpy(sccpus, scpus, sclen); 

       if (strcmp(sccpus,"")==0) {
                       strcpy(msg,"Data entry error: Invalid value for CPU");
                       util_display_error_form(msg);
                       return;
               }
// Check  Svrname is not null 

         svrname = util_form_entry_value ("SVRNAME");
         if (strcmp(svrname,"") == 0) {
                     strcpy(msg,"Data entry error: Server Class Name cannot be NULL");
                     util_display_error_form(msg);
                     return;
         }



// if CPU is all assign the CPU values from 0 to 15


         if (strcasecmp(sccpus,"all") == 0) {
            for(i = 0; i < CPUSPERNODE; i++) {
               cpulists[i] = i;
            } 
           sernum = CPUSPERNODE;
        }
         else {
           ptr = strtok(sccpus,",");
		while(ptr != NULL) {

// display err msg if CPU value is not a digit and doesn't lies between 0 to 15
                       for(i=0;i<strlen(ptr);i++) {
                       if( !isdigit((int)ptr[i])) {
                          strcpy(msg,"Data entry error: Enter only digits for CPU or  ALL to configure in all the CPU's");
                          util_display_error_form(msg);
                          return;
                       }
                     }
                     cpulists[sernum] = (short)atoi(ptr);

                 if( cpulists[sernum] < 0 || cpulists[sernum] > 15 ) {
                    strcpy(msg,"Data entry error: CPU value must be between 0 to 15");
                    util_display_error_form(msg);
                    return;
                }  
                     ptr = strtok(NULL,",");
                     sernum++;
		}
        }

     
generalFileInfo.sernum = sernum;

// configure as many severs as many CPU's 

           for(i=1;i<=sernum;i++) {

	        type = getType();

        if(type == 0) {
           util_display_error_form("Internal error; unable to determine mode");
           return;
        }


       
      /* making the entry as TRUE , to write the server class name and the number of servers in the SIP General Info file */

                if(i == 1)
                  entry = TRUE;
                else
                  entry = FALSE;

      /* making the display as TRUE to display the Reconfigured Info Page */

                if( (i == 2) && ( type == RECONFIGUREIT || type == RECONFIGUREONLY || addServerToExistingConf==1) ) {
                    displayServerConfigedPage(type);
                    display = TRUE;
                }


       	if(type == CONFIGUREIT) {
           mode = getenv("MODE");
	   if(mode != NULL) {
	     if(strcmp(mode,"RECONFIGURE_NEWSERVER") == 0)
		addServerToExistingConf = 1;
           }
           else {
            util_display_error_form("Internal Error : Unable to get the Mode");
            return;
          } 
	}


        if(getServerFields(i,type) == FALSE) {
                util_display_error_form("Error in data entry fields");
                return;
        }


	conf = util_form_entry_value("CONFIGDIR");

	if(conf == NULL) {
		strcpy(msg,"Unable to obtain configuration directory");
		util_display_error_form(msg);
		return;
	} else {
		strcpy(confDir,conf);
	}
	result = getPathwayFields();
	if(result == FALSE) {
		util_display_error_form("Internal error: Unable to obtain required information");
		return;
	}

	if(type == CONFIGUREONLY) {
		status = pmproc_running(initialSipInfo.pathmonName);
		if(!status) {
			type = CONFIGURENORUN;
		}
		mode = getenv("MODE");
		if(mode != NULL) {
			if(strcmp(mode,"RECONFIGURE_NEWSERVER") == 0)
				addServerToExistingConf = 1;
		}
                else {
                  util_display_error_form("Internal Error : Unable to get the Mode");
                  return;
                } 
	}
            
	if(type == CONFIGUREIT || type == RECONFIGUREIT || type == CONFIGUREONLY
		|| type == CONFIGURENORUN || type == RECONFIGUREONLY) {
		if(addServerToExistingConf == 1) {
			result = readServerExports(initialSipInfo.pathmonName,
							serverInfo.serverClassName);
			if(result != -1) {
				sprintf(msg,"A serverclass named %s already exists in this application; use the browser back button and choose another serverclass name",
					serverInfo.serverClassName);
				util_display_error_form(msg);
				return;
			}
		} else {
			if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
				result = readServerExports(initialSipInfo.pathmonName,
					serverInfo.serverClassName);
				if(result != -1) {
					sprintf(msg,"A serverclass named %s already exists in this application; choose another name or use the Reconfiguration Tool",
						serverInfo.serverClassName);
					util_display_error_form(msg);
					return;
				}
			}

		}

		if(addServerToExistingConf == 1) {
                        error = configureSipServer(TRUE, RECONFIGURE_NEWSERVER,entry);
		} else {
                          error = configureSipServer(TRUE, type,entry);
                         
		}

		if(error != 0) {
			util_display_error_form(msg);
			return;
		}
		if(type == CONFIGUREIT || type == RECONFIGUREIT) {
			initPmSpi();

// check whether the CPU is Up or not, if Up start the server

                        if(checkCpu_Up(serverInfo.serverClassCpus)) 
			error = pmspi_startsc(serverInfo.serverClassName,&err1,&err2);
			if(error != 0) {
				errmsg = pmspi_errormsg(error,err1,err2);
				util_display_error_form(errmsg);
			}
		}
	}
	if(type == RECONFIGUREIT || type == RECONFIGUREONLY || addServerToExistingConf == 1) {
		if(addServerToExistingConf == 1) {
			if(initialSipInfo.currentServer > initialSipInfo.numSipSC) {
				initialSipInfo.numSipSC++;
				if(writeInitialExports(confDir) == -1) {
					errno_errMsg("Server has been configured but unable to write server detail file",errno,"Server Exports");
					return;
				}
			}
		}
      }

}
	if(initialSipInfo.currentServer < initialSipInfo.numSipSC) {
		initialSipInfo.currentServer++;
		if(type == CONFIGURENORUN)
			type = RECONFIGURE_NEWSERVER;
		writeServerPage(type);
	} 

       if(!display) 
         if(type == RECONFIGUREIT || type == RECONFIGUREONLY || addServerToExistingConf==1) 
           displayServerConfigedPage(type);

       displayConfigurationFinished(type);
}

// End Changes for Solution Number 10-040512-5972

/************************************************************************************
 * void writeServerPage
 *
 *	param - type one of CONFIGUREIT, RECONFIGUREIT,CONFIGUREONLY or RECONFIGUREONLY
 *
 *  displays the html need to configure a server.
 ***********************************************************************************/
void writeServerPage(int type)
{
   	char m_TCPIP_ProcessName[37];
	char *svc, *pmon;
	char *numServers;
	char *currentServer;
	char *dirconf;
	int error, firstCpu = 0 ;
	char pathmonName[26];
	int newServer = 0;
	int status;
        int i,cpupairs = 0;
        char scCPUS[100];
        char tempNum[3];
   

	if(type == RECONFIGUREIT) {
		svc = util_form_entry_value("CHOICE");
		if(strcmp(svc,"ADDNEWSC") == 0) {
			/* See if pathmon is running for this application */
			status = pmproc_running();
			if(status)
				type = CONFIGUREIT;
			else
				type = CONFIGURENORUN;
			newServer = 1;
		}
		numServers = util_form_entry_value("SERVERCOUNT");
		if(numServers == NULL) {
			strcpy(msg,"Internal error: Unable to obtain current number of serverclasses");
			util_display_error_form(msg);
			return;
		} else {
			currentServer = util_form_entry_value("CURRENTSERVERNO");
			if(currentServer == NULL) {
				strcpy(msg,"Internal error: Unable to obtain current number of serverclasses");
				util_display_error_form(msg);
				return;
			}
			initialSipInfo.currentServer = atoi(numServers);
			if(newServer == 1) {
				initialSipInfo.currentServer ++;
				writeInitialExports(confDir);
			}
		}
	}
            // Begin Changes for Solution Number 10-040512-5972

             // no need to check for version name to assign transport name 

		strcpy(m_TCPIP_ProcessName , "$ZTC1J");

           // End Changes for Solution Number 10-040512-5972

	if(type == REVIEWIT || type == RECONFIGUREIT) {
		dirconf = util_form_entry_value("CONFIGDIR");
		if(dirconf == NULL) {
			strcpy(msg,"Internal error: Unable to obtain the name of the configuration directory");
			util_display_error_form(msg);
			return;
		} else {
			strcpy(confDir,dirconf);
		}
		pmon = util_form_entry_value("PMONNAME");
		if(pmon == NULL) {
			strcpy(msg,"Internal error: Pathmon name missing from View Request");
			util_display_error_form(msg);
			return;
		} else {
			strcpy(pathmonName,pmon);
		}
		if(type == REVIEWIT) {
			svc = util_form_entry_value("SVC");
		} else {
			svc = getenv("CHOICE");
                        // to get the Serverclass name from choice moving the pointer +7 ie. DELETE_ 
                        svc += 7;
		}
		if(svc == NULL) {
			strcpy(msg,"Internal error; ServerClass name missing from View Request");
			util_display_error_form(msg);
			return;
		} 
                
		error = readServerExports(pathmonName,svc);
		if(error == -1) {
			errno_errMsg("Unable to read server exports",errno,"Server export file");
			return;
		}
	}

	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><HEAD>\n");
	CGI_printf("<TITLE>sipconf2</TITLE>\n");
	CGI_printf("<SCRIPT Language=\"JavaScript\">\n");
	CGI_printf("function showHelp(msgindex) {\n");
	CGI_printf("nIndex = parseInt(msgindex, 10);\n");
	CGI_printf(" Win1 =\n");
	CGI_printf("open(\"\",\n");
	CGI_printf("			\"subWindow\",\n");
	CGI_printf("			\"toolbar=no,\" +\n");
	CGI_printf("			\"location=no,\" +\n");
	CGI_printf("			\"directories=no,\" +\n");
	CGI_printf("			\"status=no,\" +\n");
	CGI_printf("			\"menubar=no,\" +\n");
	CGI_printf("			\"scrollbars=yes,\" +\n");
	CGI_printf("			\"resizable=no,\" +\n");
	CGI_printf("			\"width=350,\" +\n");
	CGI_printf("			\"height=250\"\n");
	CGI_printf("	);\n");
	CGI_printf("if(nIndex == 1) {\n");
	CGI_printf("	var sText = \"The default value is the standard location of the Java Virtual Machine object file.  If you want to use another JVM, enter its absolute path(including the object file name).<P></BODY>\"\n");
	CGI_printf("    var sTitle = \"JVM Location\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 2) {\n");
	CGI_printf("	var sText = \"If you want to use a TCP/IP transport process other than the default process, enter the name of the process here. This name should be specified as $process_name.<P>\"\n");
	CGI_printf("    var sTitle = \"Transport Name\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 3) {\n");
	CGI_printf("	var sText = \"The absolute path to classes required by this SIP Server. Multiple paths should be separated by colons(:).<P>\"\n");
	CGI_printf("    var sTitle = \"Class Path\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 4) {\n");
	CGI_printf("	var sText1 = \"<P>Arguments passed to your Sip Server. It is not necessary to \"\n");
	CGI_printf("	var sText2 = \"escape printable characters in an argument list. If an argument includes \"\n");
	CGI_printf("    var sText3 = \"blanks, quotes, commas, or semicolons, enclose the entire argument in quotes. \"\n");
	CGI_printf("    var sText4 = \"For detailed information, refer to the discussion of SET SERVER in the NonStop TS/MP System Management Manual.</P>\"\n");
	CGI_printf("    var sText = sText1 + sText2 + sText3 + sText4\n");
	CGI_printf("    var sTitle = \"Arglist\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 5) {\n");
	CGI_printf("    var sText1 = \"<P>Environmental variables expected by your Sip Server. Multiple environmental \"\n");
	CGI_printf("    var sText2 = \"variables can be specified as a list of name value pairs separated by commas.&nbsp;\"\n");
	CGI_printf("    var sText3 = \"If a name or value includes blanks, quotes, commas, or semicolons, enclose \"\n");
	CGI_printf("    var sText4 = \"the entire argument in quotes. </P>\"\n");
	CGI_printf("    var sText5 = \"<P>Environmental variable names can't contain an equal sign(=) but an equal sign can be include in the value. \"\n");
	CGI_printf("    var sText6 = \"For detailed information, refer to the discussion of SET SERVER in the NonStop TS/MP System Management Manual.</P>\"\n");
	CGI_printf("	var sText = sText1 + sText2 + sText3 + sText4 + sText5 + sText6;\n");
	CGI_printf("    var sTitle = \"Environmental Variables\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 6) {\n");
	CGI_printf("	var sText = \"<P>This key provides a measure of security: only users who know the key can access round-robin on the port. It can be up to 8 alphanumeric characters, the first caharacter must be a letter.<P>\"\n");
	CGI_printf("    var sTitle = \"Filter Key\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 7) {\n");
	CGI_printf("	var sText = \"<P>The name must begin with a letter and must be no longer than 13 characters.<P>\"\n");
	CGI_printf("    var sTitle = \"ServerClass Name\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 8) {\n");
	CGI_printf("	var sText = \"<P>The cpus where you want your Sip Server to run. Cpus should be supplied as a comma separated list. By default, all cpus on the system can be used.<P>\"\n");
	CGI_printf("    var sTitle = \"ServerClass Cpus\";\n");
	CGI_printf("}\n");

	CGI_printf("	Win1.document.write(\"<HTML><BODY BGCOLOR='ffffcc'><H2>\" + sTitle + \"</H2><HR><P></BODY></HTML>\" + sText)\n");
	CGI_printf("	Win1.focus();\n");
	CGI_printf("	Win1.document.close()\n");
	CGI_printf("}\n");
	CGI_printf("</SCRIPT>\n");
	CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
	CGI_printf("function CheckData(form) {\n");
	CGI_printf("	var bdataEntryError = false;\n");
	CGI_printf("	var sErrorMsg = \"Invalid value specified for: \";\n");

	CGI_printf("	var sJhome = document.servconf.JAVAHOME.value;\n");
	CGI_printf("	var sTcpIP = document.servconf.TCPIPPROCESS.value;\n");
        CGI_printf("	var sPTcpIP = document.servconf.PTCPIPKEY.value;\n");
	CGI_printf("	var sCpath = document.servconf.CLASS_PATH.value;\n");
	CGI_printf("	var sCname = document.servconf.CLASS_NAME.value;\n");
	CGI_printf("	var sSvrname = document.servconf.SVRNAME.value\n");
	CGI_printf("	if (sJhome < 2) {\n");
	CGI_printf("     	sErrorMsg = sErrorMsg + \"JVM Location\";\n");
	CGI_printf("     	bdataEntryError = true;\n");
	CGI_printf("	}\n");

	CGI_printf("	if (sTcpIP < 2) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", TCP/IP Transport\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"TCP/IP Transport\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
        CGI_printf("	if (sPTcpIP < 1 || sPTcpIP > 8) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", PTCP/IP Key\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"PTCP/IP Key\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");

	CGI_printf("	if(sCpath < 2) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", User Classpath\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"User Classpath\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	if(sCname < 1) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Class name\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Class name\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	if(type != RECONFIGUREIT) {
		CGI_printf("	if(sSvrname < 1 || sSvrname > 13) {\n");
		CGI_printf("		if (bdataEntryError) {\n");
		CGI_printf("			sErrorMsg = sErrorMsg + \", ServerClass Name\";\n");
		CGI_printf("    	} else {\n");
		CGI_printf("     		sErrorMsg = sErrorMsg + \"ServerClass Name\";\n");
		CGI_printf("     		bdataEntryError = true;\n");
		CGI_printf("    	}\n");
		CGI_printf("	}\n");
	}
        CGI_printf("    }\n");
	CGI_printf("</SCRIPT>\n");
	CGI_printf("</HEAD>	<BODY>\n");
	if((type == CONFIGUREIT || type == CONFIGUREONLY) && newServer == 0 ) {
		CGI_printf("<H1><I><FONT COLOR=\"#0000FF\">SIP CONFIGURATION TOOL</FONT></I></H1>\n");
	}
	if(type == RECONFIGUREIT || newServer == 1) {
		CGI_printf("<H1><I><FONT COLOR=\"#0000FF\">SIP RECONFIGURATION TOOL</FONT></I></H1>\n");
	}
	if(type == REVIEWIT) {
		CGI_printf("<H1><I><FONT COLOR=\"#0000FF\">SIP VIEW TOOL</FONT></I></H1>\n");
	}
	CGI_printf("<CENTER><HR SIZE=5 NOSHADE WIDTH=\"100&#037\"></CENTER>\n");
	if(type != REVIEWIT)
		CGI_printf("<FORM name=\"servconf\" action=\"%s\" method=POST onsubmit=\"return CheckData(this.form)\">\n",getenv("SCRIPT_NAME"));
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<B><I>Scalable IP Server configuration information for  new server</I></B>\n");
	}
	if(type == RECONFIGUREIT) {

		CGI_printf("<B><I>Scalable IP Server information for serverclass %s</I></B>\n",serverInfo.serverClassName);
	}
	if(type == REVIEWIT) {
		CGI_printf("<B><I>Scalable IP Server information for server %s</I></B>\n",serverInfo.serverClassName);
	}
	CGI_printf("<BR>&nbsp;\n");
	CGI_printf("<TABLE BORDER COLS= 2 WIDTH=\"70&#037\" BGCOLOR=\"#CCCCCC\" ><TR>\n");
	CGI_printf("<TD BGCOLOR=\"#CCCCCC\"><B><FONT COLOR=\"#3333FF\">JVM Location:</FONT></B></TD>\n");

	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><INPUT type=text name=\"JAVAHOME\" size=50 value=\"/usr/tandem/java/bin/java\">\n");
	}
	if(type == RECONFIGUREIT) {
		CGI_printf("<TD><INPUT type=text name=\"JAVAHOME\" size=50 value=\"%s\">\n",
			serverInfo.javaHome);
	}
	if(type == REVIEWIT) {
		CGI_printf("<TD><B><FONT COLOR=\"#3333FF\">%s</B></FONT>\n",
			serverInfo.javaHome);
	}
	CGI_printf("<BR><B>The path to the JVM you want to use.<B>\n");
	CGI_printf("<BR><INPUT name=\"help1\" type=BUTTON value=\"More Info\" onclick=\"showHelp('1');\"></B>\n");
	CGI_printf("</TD>\n");
	CGI_printf("</TR>\n");

	CGI_printf("<TR>\n");
	CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">Transport Name:</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><INPUT name=\"TCPIPPROCESS\" size=20 value=\"%s\">\n",m_TCPIP_ProcessName);
	}
	if(type == RECONFIGUREIT) {
		CGI_printf("<TD><INPUT name=\"TCPIPPROCESS\" size=20 value=\"%s\">\n",serverInfo.tcpipProcessName);
	}
	if(type == REVIEWIT) {
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",serverInfo.tcpipProcessName);
	}
	CGI_printf("<BR><B>The name of the PTCP/IP or TCP/IPv6 process to be associated</B>\n");
        CGI_printf("<BR><B> with this Java Server.</B>\n");
	CGI_printf("<BR><INPUT name=\"help2\" type=BUTTON value=\"More Info\" onclick=\"showHelp('2');\"></B>\n");
	CGI_printf("</TD></TR><TR>\n");

        CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">PTCP/IP Filter Key</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Name:</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
        CGI_printf("<TD><INPUT name=\"PTCPIPKEY\" size=20 value=\"key\">\n");
	}
	if(type == RECONFIGUREIT) {
		CGI_printf("<TD><INPUT name=\"PTCPIPKEY\" size=20 value=\"%s\">\n",serverInfo.ptcpipkey);
	}
	if(type == REVIEWIT) {
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",serverInfo.ptcpipkey);
	}
	CGI_printf("<BR><B>The name of the PTCP/IP filter key to be associated with this Java Server.</B>\n");
	CGI_printf("<BR><INPUT name=\"help6\" type=BUTTON value=\"More Info\" onclick=\"showHelp('6');\"></B>\n");
	CGI_printf("</TD></TR><TR>\n");


	CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">User</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">classpath</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><INPUT type=text name=\"CLASS_PATH\" size=60 MAXSIZE=4095>\n");
	}
	if(type == RECONFIGUREIT) {
		CGI_printf("<TD><INPUT type=text name=\"CLASS_PATH\" size=180 MAXSIZE=4095 value=\"%s\">\n",
			serverInfo.classpath);
	}
	if(type == REVIEWIT) {
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",
			serverInfo.classpath);
	}
	CGI_printf("<BR><B>The classpath for classes required for this SIP Server.</B>\n");
	CGI_printf("<BR><INPUT name=\"help3\" type=BUTTON value=\"More Info\" onclick=\"showHelp('3');\"></B>\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR> <TD><B><FONT COLOR=\"#3366FF\">.class Name</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><INPUT type=text name=\"CLASS_NAME\" size=40>\n");
	}
	if(type == RECONFIGUREIT){
		CGI_printf("<TD><INPUT type=text name=\"CLASS_NAME\" size=40 value=\"%s\">\n",
			serverInfo.className);
	}
	if(type == REVIEWIT){
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",
			serverInfo.className);
	}
	CGI_printf("<BR><B>The name of the Java .class file containing the \"main\" for this SIP Server. </B>\n");
	CGI_printf("</TD></TR><TR>\n");


	CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">Arglist</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><TEXTAREA name=\"ARGLIST\" ROWS=0 COLS=60></TEXTAREA>");
	}
	if(type == RECONFIGUREIT) {
		CGI_printf("<TD><TEXTAREA name=\"ARGLIST\" ROWS=0 COLS=60>%s</TEXTAREA>",
			serverInfo.serverArglist);
	}
	if(type == REVIEWIT) {
		CGI_printf("<TD><b><FONT COLOR=\"#3366FF\"><TEXTAREA name=\"ARGLIST\" ROWS=0 COLS=60>%s</TEXTAREA></B></FONT>",
			serverInfo.serverArglist);
	}
	CGI_printf("<BR><B>Arguments passed directly to the JVM.<B>\n");
	CGI_printf("<BR><INPUT name=\"help4\" type=BUTTON value=\"More Info\" onclick=\"showHelp('4');\"></B>\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR> <TD><B><FONT COLOR=\"#3366FF\">ENV</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">variable(s)</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><TEXTAREA name=\"ENVVARS\" ROWS=1 COLS=60></TEXTAREA>");
	}
	if(type == RECONFIGUREIT){
		CGI_printf("<TD><TEXTAREA name=\"ENVVARS\" ROWS=1 COLS=60>%s</TEXTAREA>",
			serverInfo.envVariables);
	}
	if(type == REVIEWIT){
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\"><TEXTAREA name=\"ENVVARS\" ROWS=1 COLS=60>%s</TEXTAREA></B></FONT>",
			serverInfo.envVariables);
	}
	CGI_printf("<BR><B>Environmental variables needed by the SIP Server.</B>\n");
	CGI_printf("<BR><INPUT name=\"help5\" type=BUTTON value=\"More Info\" onclick=\"showHelp('5');\"></B>\n");
	CGI_printf("</TD>\n");


	CGI_printf("</TR><TR><TD><B><FONT COLOR=\"#3366FF\">ServerClass</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Cpus</FONT></B></TD>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><INPUT type=text name=\"SCCPUS\" size=32 value=\"all\">\n");
	}
	if(type == RECONFIGUREIT || type == REVIEWIT){
	    for(i = 0; i < 16; i++) {
			if(serverInfo.cpuAssigned[i] != -1)
				cpupairs++;
		}
		if(cpupairs == 16) {
			strcpy(scCPUS,"all");
		} else {
			firstCpu = 1;
			for(i = 0; i < 16; i++) {
				if(serverInfo.cpuAssigned[i] != -1) {
					if(firstCpu) {
						sprintf(scCPUS,"%d",serverInfo.cpuAssigned[i]);
						firstCpu = 0;
					} else {
						strcat(scCPUS,",");
						sprintf(tempNum,"%d",serverInfo.cpuAssigned[i]);
						strcat(scCPUS,tempNum);
					}
				}
			}
		}
	} 

        if(type == RECONFIGUREIT) {
                CGI_printf("<TD><INPUT type=text name=\"SCCPUS\" size=32 value=\"%s\">\n",scCPUS);
        }
        if(type == REVIEWIT) {
                CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",scCPUS);
        }
CGI_printf("<BR><B>The cpu where you want the SIP Servers to run.</B>\n");
	CGI_printf("<BR><INPUT name=\"help8\" type=BUTTON value=\"More Info\" onclick=\"showHelp('8');\"></B>\n");

CGI_printf("<TR>\n");


	if(type == CONFIGUREIT || type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">ServerClass Name Root </FONT></B></TD>\n");
		CGI_printf("<TD><INPUT type=text name=\"SVRNAME\" value=\"SIPSVC%d\" size=15>\n",
			initialSipInfo.currentServer);
		CGI_printf("<BR><B>A serverclass name.</B>\n"); 
		CGI_printf("<BR><INPUT name=\"help7\" type=BUTTON value=\"More Info\" onclick=\"showHelp('7');\"></B>\n");
		CGI_printf("</TD></TR><TR>\n");
	}


	CGI_printf("</TD></TR></TABLE><P>\n");

	CGI_printf("<INPUT TYPE=hidden NAME=\"ServiceName\" VALUE=\"sipServers\">\n");
	writeInitHiddenFields();

	if(type == CONFIGUREIT) {
		CGI_printf("<INPUT type=hidden name=\"PATHMONSTATE\" value=\"RUNNING\">\n");
		CGI_printf("<BR><BR><FONT COLOR=\"#3366FF\">Select one of the following configuration options:</FONT>\n");
		CGI_printf("<BR><B>Options: <BR><input type=RADIO value=\"CONFIGUREIT\" name=\"TYPE\">&nbsp;Configure and Start\n");
		CGI_printf("<BR><input type=RADIO value=\"CONFIGUREONLY\" name=\"TYPE\">&nbsp;Only Configure</B>\n");
		if(newServer == 1)
			CGI_printf("<BR><input type=hidden name=MODE value=\"RECONFIGURE_NEWSERVER\">\n");
                 else
                        CGI_printf("<BR><input type=hidden name=MODE value=\"CONFIGURE_SERVER\">\n"); 
	} else if(type == CONFIGUREONLY || type == CONFIGURENORUN) {
		CGI_printf("<BR><input type=hidden name=\"TYPE\" value=\"CONFIGUREONLY\">\n");
		if(newServer == 1)
			CGI_printf("<INPUT type=hidden name=MODE value=\"RECONFIGURE_NEWSERVER\">\n");
                else
                        CGI_printf("<BR><input type=hidden name=MODE value=\"CONFIGURE_SERVER\">\n"); 
	} else if (type == RECONFIGUREIT) {
		status = pmproc_running();
		if(status) {
			CGI_printf("<BR><B>Check \"Reconfigure\" if you have changed data in any of the fields above.\n");
			CGI_printf("<BR><BR>Check \"Reconfigure and Start ServerClass\" to reconfigure and restart the serverclass. \n");
			CGI_printf("<I>NOTE: </I>If the serverclass is running it will be stopped.\n");
			CGI_printf("<BR><BR><B>Options: <BR><input type=RADIO value=\"RECONFIGUREONLY\" name=\"TYPE\">&nbsp;Reconfigure Only\n");
			CGI_printf("<BR><input type=RADIO value=\"RECONFIGUREIT\" name=\"TYPE\">&nbsp;Reconfigure and Restart</B>\n");
		} else {
			CGI_printf("<INPUT type=hidden name=\"TYPE\" value=\"RECONFIGUREONLY\">\n");
		}

		CGI_printf("<INPUT type=hidden name=SVRNAME value=\"%s\">\n",serverInfo.serverClassName);
	} else if(type == REVIEWIT) {
		CGI_printf("<P><B><A HREF=\"%s?ServiceName=start&CONFIGDIR=%s\">Return to Management Tool</B></A>\n",
			getenv("SCRIPT_NAME"),confDir);
		CGI_printf("<BR><BR><A HREF=\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	if(type != REVIEWIT) {
		CGI_printf("<BR><INPUT type=submit name=\"continue\" value=\"Submit\"></FORM>\n");
		CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("</BODY>\n");
	CGI_printf("</HTML>\n");
	return;
}
/*******************************************************************
 * display_got_config_href
 *
 *  This displays the initial menu for the Configuration Tool
 *******************************************************************/
void display_got_config_href()
{

	char *conf;
	confDir[0] = '/0';
	conf = getenv("CONFIGDIR");
	if(conf != NULL) {
		strcpy(confDir,conf);
	}
	CGI_printf("Content-type: text/html\n\n");
   	CGI_printf("<title>sipconfiginfo</title></head><body>\n");
	CGI_printf("<center><h1><i><font color=\"#3333FF\">SIP (Scalable IP) Configuration Tool</font></i></h1></center>\n");
	CGI_printf("<center><hr SIZE=5 NOSHADE WIDTH=\"100&#037\"></center>\n");
	CGI_printf("<BR><FORM action=\"%s\" method=POST>\n",getenv("SCRIPT_NAME"));
	CGI_printf("<input type=hidden name=ServiceName value=configure>");
	CGI_printf("<input type=submit name=configure value=\"Configure new application\">\n");
	if(confDir[0] != '0') {
		CGI_printf("<input type=hidden name=CONFIGDIR value=%s>\n",
			confDir);
	}
	CGI_printf("</FORM></TD>\n");
	CGI_printf("<P>The SIP Configuration tool is used to configure a new TS/MP application for one or more SIP ");
	CGI_printf("servers. It provides the opportunity to either configure and start an ");
	CGI_printf("application or to simply configure an application and start it at a later ");
	CGI_printf("time. While using this tool, provide the information that ");
	CGI_printf("is needed to complete the configuration. ");
	CGI_printf("<p>These questions include the following general information: \n");
	CGI_printf("<ul><li>\n");
	CGI_printf("The name for the PATHMON process that will monitor ");
	CGI_printf("the TS/MP application.</li>\n");

	CGI_printf("<li>A Guardian volume and subvolume where TS/MP will write ");
	CGI_printf("an internal representation of the configuration and log status information.</li>\n");

	CGI_printf("<li>The name of a terminal which will act as the home terminal for the TS/MP ");
	CGI_printf("servers within the application.</li>\n");

	CGI_printf("<li>The path to the location where the log files for ");
	CGI_printf("the SIP servers to reside.</li>\n");
	CGI_printf("</ul>\n");
	CGI_printf("Provide specific information about the SIP server.&nbsp;");
	CGI_printf("This information includes:\n");
	CGI_printf("<ul>\n");
	CGI_printf("<li>The path to the version of the Java Virtual Machine (JVM) to be used with the SIP server.&nbsp; If ");
	CGI_printf("the JVM is not modified, use the default value.</li>\n");

	CGI_printf("<li>The CLASSPATH needed by the SIP server.</li>\n");

	CGI_printf("<li>The name of the .class file for the SIP server.</li>\n");

	CGI_printf("<li>Any arguments that should be passed to the SIP server.&nbsp; For instance, ");
	CGI_printf("if JDBC is used in the Application, include: <P><I>&nbsp&nbsp-D jdbc.drivers=com.tandem.sqlmp.SQLMPDriver</I></li> \n");

	CGI_printf("<li>Any environmental variables the SIP server requires.</li>\n");

	CGI_printf("<li>A name for the SIP serverclass.</li>\n");
	CGI_printf("</ul>\n");
	if(confDir[0] == '0') {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"));
	} else {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("</body></html>\n");
	return;
}
/*******************************************************************
 * display_got_reconfigure_href
 *
 *  This displays the initial menu for the Reconfiguration Tool
 *******************************************************************/
void display_got_reconfigure_href()
{
	char *conf;
	confDir[0] = '/0';
	conf = getenv("CONFIGDIR");
	if(conf != NULL) {
		strcpy(confDir,conf);
	}
	CGI_printf("Content-type: text/html\n\n");
   	CGI_printf("<title>sipconfiginfo</title></head><body>\n");
	CGI_printf("<center><h1><i><font color=\"#3333FF\">SIP Reconfiguration Tool</font></i></h1></center>\n");
	CGI_printf("<center><hr SIZE=5 NOSHADE WIDTH=\"100&#037\"></center>\n");
	CGI_printf("<FORM name=\"recon\" action=\"%s\" method=POST onsubmit=\"return confirmRecon(this.form)\">\n",getenv("SCRIPT_NAME"));
	CGI_printf("<input type=hidden name=ServiceName value=reconfigure>");
	if(confDir[0] == '0') {
		CGI_printf("Enter the path to your configuration directory: <input type=text name=CONFIGDIR>\n");
	} else {
		CGI_printf("Path to your configuration directory: <input type=text name=CONFIGDIR value =\"%s\" size=\"%d\">\n",
			confDir, (strlen(confDir) + 2));
	}
	CGI_printf("<BR>Enter the name of the pathmon for your application: <input type=text name=PMONNAME>\n");
	CGI_printf("<BR><input type=submit name=reconfigure value=\"Reconfigure an existing application\"></FORM>\n");
	CGI_printf("<center><hr SIZE=5 NOSHADE WIDTH=\"100&#037\"></center>\n");
	CGI_printf("<P>The SIP Reconfiguration tool is used to reconfigure \n");
	CGI_printf("an existing application.  To use this tool, you must supply the path to the configuration \n");
	CGI_printf("directory that was specified when the application was configured.  You must also supply the name \n");
	CGI_printf("of the Pathmon for the application.</P>\n");
	CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
	CGI_printf("function confirmRecon(form) {\n");
	CGI_printf("	var cName = document.recon.CONFIGDIR.value;\n");
	CGI_printf("	var pName = document.recon.PMONNAME.value;\n");
	CGI_printf("	var bdataEntryError = false;\n");
	CGI_printf("	var sErrMsg;\n");
	CGI_printf("	if (cName.length < 1) {\n");
	CGI_printf("    	sErrMsg = \"Please enter a value for the configuration directory. This is the directory where your application configuration files were written when the application was originally configured.\";\n");
	CGI_printf("    	bdataEntryError = true;\n");
	CGI_printf("	}\n");
	CGI_printf("	if (pName.length < 1) { \n");
	CGI_printf("		if(bdataEntryError) {\n");
	CGI_printf("    		sErrMsg = sErrMsg + \"  In Addition, enter a value for the Pathmon name for your application.\";\n");
	CGI_printf("		} else {\n");
	CGI_printf("    		sErrMsg = \"Enter a value for the Pathmon name for your application.\";\n");
	CGI_printf("			bdataEntryError = true;\n");
	CGI_printf("		}\n");
	CGI_printf("	}\n");
	CGI_printf("	if(bdataEntryError) {\n");
	CGI_printf("		alert(sErrMsg);\n");
	CGI_printf("		return false;\n");
	CGI_printf("	} else {\n");
	CGI_printf("		return true;\n");
	CGI_printf("	}\n");
	CGI_printf("}\n");
	CGI_printf("</SCRIPT>\n");
	if(confDir[0] == '0') {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"));
	} else {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("</body></html>\n");
	return;
}
/*******************************************************************
 * display_got_manage_href
 *
 *  Displays the initial menu for the Management Tool
 *******************************************************************/
void display_got_manage_href()
{

	char *conf;
	confDir[0] = '/0';
	conf = getenv("CONFIGDIR");
	if(conf != NULL) {
		strcpy(confDir,conf);
	}
	CGI_printf("Content-type: text/html\n\n");
   	CGI_printf("<title>sipconfiginfo</title></head><body>\n");
	CGI_printf("<center><h1><i><font color=\"#3333FF\">SIP Management Tool</font></i></h1></center>\n");
	CGI_printf("<center><hr SIZE=5 NOSHADE WIDTH=\"100&#037\"></center>\n");
	CGI_printf("<FORM name=manage action=\"%s\" method=POST onsubmit=\"return Use(this.form)\">\n",getenv("SCRIPT_NAME"));
	CGI_printf("<input type=hidden name=ServiceName value=start>\n");
	if(confDir[0] == '0') {
		CGI_printf("<BR>Enter the path to your configuration directory: <input type=text name=CONFIGDIR>\n");
	} else {
		CGI_printf("<BR>Path to your configuration directory: <input type=text name=CONFIGDIR value=\"%s\" size=\"%d\">\n",
			confDir,(strlen(confDir) + 2));
	}
	CGI_printf("<BR><input type=submit name=configure value=\"Manage an existing application\">\n");
	CGI_printf("</FORM>\n");
	CGI_printf("<P>The SIP management tool is used to:\n");
	CGI_printf("<ul><li>Obtain information about entities in an existing application.\n");
	CGI_printf("<li>Obtain the status of an existing application.\n");
	CGI_printf("<li>Start an already configured application.</li>\n");
	CGI_printf("<li>Stop an executing application.</li>\n");
	CGI_printf("<li>Start SIP serverclasses within an executing application.</li>\n");
	CGI_printf("<li>Stop SIP serverclasses within an executing application.</li>\n");
	CGI_printf("</ul>\n");
	CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
	CGI_printf("function Use(form) {\n");
	CGI_printf("var cName = document.manage.CONFIGDIR.value;\n");
	CGI_printf("if (cName.length < 3) {\n");
	CGI_printf("    alert(\"You must supply a value for the configuration directory.\")\n");
	CGI_printf("    return false\n");
	CGI_printf("}\n");
	CGI_printf("}\n");
	CGI_printf("</SCRIPT>\n");
	if(confDir[0] == '0') {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"));
	} else {
		CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("</body></html>\n");
	return;
}

/*********************************************************************
 * display_welcome_form
 *   Displays the first form seen by the user.
 ********************************************************************/
void display_welcome_form()
{
	char *conf;
	confDir[0] = '/0';
	conf = getenv("CONFIGDIR");
	if(conf != NULL) {
		strcpy(confDir,conf);
	}
	CGI_printf("Content-type: text/html\n\n");
   	CGI_printf("<title>sipintro</title></head><body>\n");
	CGI_printf("<center><h1><font color=\"#009900\">Scalable IP(SIP) TOOLS</font></h1></center>\n");
	CGI_printf("<center><font color=\"#009900\">Version 2.0</font><font color=\"#009900\"></font>\n");
	if(confDir[0] == '0') {
		CGI_printf("<p><A HREF=\"%s?ServiceName=confhref\">Configuration Tool</A>\n",
			getenv("SCRIPT_NAME"));
		CGI_printf("<BR><A HREF=\"%s?ServiceName=reviewhref\">Reconfiguration Tool</A>\n",
			getenv("SCRIPT_NAME"));
		CGI_printf("<BR><A HREF=\"%s?ServiceName=managehref\">Management Tool</A>\n",
			getenv("SCRIPT_NAME"));
	} else {
		CGI_printf("<p><A HREF=\"%s?ServiceName=confhref&CONFIGDIR=%s\">Configuration Tool</A>\n",
			getenv("SCRIPT_NAME"),confDir);
		CGI_printf("<BR><A HREF=\"%s?ServiceName=reviewhref&CONFIGDIR=%s\">Reconfiguration Tool</A>\n",
			getenv("SCRIPT_NAME"),confDir);
		CGI_printf("<BR><A HREF=\"%s?ServiceName=managehref&CONFIGDIR=%s\">Management Tool</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("</body></html>\n");
	return;
}

/********************************************************************
 * display_config_info
 *
 *	param: 	confType one of "configureit", "reviewit", or "reconfigureit"
 *
 *	Used to display configuration information for:
 *		an initial configuration (first time)
 *		a reconfiguration (reconfiguring existing app)
 *		or to review an existing configuration.
 ********************************************************************/

void display_config_info (char *confType)
{
	int type, error = 0;
	char *conf_fileDir;
	char *pmonName;
	char pathmonName[26];
	FILE *pfp;

	if(strcmp(confType,"configure") == 0)
		type = CONFIGUREIT;
	else if(strcmp(confType,"reconfigure") == 0) {
		type = RECONFIGUREIT;
	} else if(strcmp(confType,"reviewit") == 0) {
		type = REVIEWIT;
	}

	conf_fileDir = getenv("CONFIGDIR");
	if(conf_fileDir != NULL) {
		conf_fileDir = removeTrailingCharacters(conf_fileDir);
		strcpy(confDir, conf_fileDir);
	} else {
		confDir[0] = '\0';
	}

	if(type == RECONFIGUREIT || type == REVIEWIT) {
		if (conf_fileDir == NULL) {
			util_display_error_form("Internal error: can't obtain configuration file directory name");
			return;
		}
	}
	if(type == REVIEWIT) {
		pmonName = util_form_entry_value("PMONNAME");
		if(pmonName == NULL) {
			util_display_error_form("Internal error: can't obtain Pathmon name");
			return;
		} else {
			strcpy(pathmonName,pmonName);
		}

		pfp = openGeneralInfoFile("rb");
		if(pfp == NULL) {
			sprintf(msg,"Unable to open .sipGeneralInfo file in %s",confDir);
			errno_errMsg(msg,errno,".sipGeneralInfo");
			return;
		}
		initialSipInfo.pathmonName[0] = '\0';
		/* Begin Changes for Solution Number 10-050318-5738  */
                /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
		while( !feof(pfp))
		{
		 if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0)
		 {
			if(strcmp(generalFileInfo.pathmonName,pathmonName) == 0) {
				strcpy(initialSipInfo.pathmonName,pathmonName);
				break;
			}
		}
		}
		/* End Changes for Solution Number 10-050318-5738 */
		fclose(pfp);
		if(initialSipInfo.pathmonName[0] == '\0')
			error = -1;
		if(error == 0) {
			error = readInitialExports(initialSipInfo.pathmonName);
			if(error == -1) {
				errno_errMsg("Unable to read pathway exports",errno,"Pathway Export File");
				return;
		    }
		}
	}
	if(type == RECONFIGUREIT)
		getPathwayFields();

	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML>\n");
	CGI_printf("<HEAD>\n");
	CGI_printf("<title>sipconf</title>\n");
	CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
	CGI_printf("function CheckData(form) {\n");
	CGI_printf("	var bdataEntryError = false;\n");
	CGI_printf("	var sErrorMsg = \"Data entry error for: \";\n");
	if(type == CONFIGUREIT) {
		CGI_printf("	var sCdir = document.config.CONFIGDIR.value;\n");
	}
	CGI_printf("	var sPmon = document.config.PMONNAME.value;\n");
	CGI_printf("	var sLogpath = document.config.LOGPATH.value;\n");
	CGI_printf("	var sHomeTerm = document.config.HOMETERM.value;\n");
	CGI_printf("	var sNumSipSc = document.config.NUMSIPSC.value\n");
	CGI_printf("	var sGuardianVol = document.config.PMONSUBVOL.value\n");
	CGI_printf("	var sPrimCpu = document.config.PMPRIMCPU.value\n");
	CGI_printf("	var sBackCpu = document.config.PMBACKCPU.value\n");
	CGI_printf("	var sType = document.config.TYPE.value\n");

	if(type == CONFIGUREIT) {
		CGI_printf("	if (sCdir < 2) {\n");
		CGI_printf("     	sErrorMsg = sErrorMsg + \"Configuration Dir\";\n");
		CGI_printf("     	bdataEntryError = true;\n");
		CGI_printf("	}\n");
	}
	CGI_printf("	if(sPmon < 1) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", PathmonName\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Pathmon Name\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	var nPrimCpu = parseInt(sPrimCpu);\n");
	CGI_printf("	var nBackCpu = parseInt(sBackCpu);\n");
	CGI_printf("	if(nPrimCpu == nBackCpu) {\n");
	CGI_printf("		if(bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Pathmon Cpus (the primary cpu and the backup cpu cannot have the same value)\";\n");
	CGI_printf("		} else {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \"Pathmon Cpus (the primary cpu and the backup cpu cannot have the same value)\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("		}\n");
	CGI_printf("	}\n");
	CGI_printf("	if (sGuardianVol < 2) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Guardian SubVolume\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Guardian SubVolume\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	if (sLogpath < 2) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Logpath\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Logpath\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	nNumSipSc = parseInt (sNumSipSc, 10);\n");
	CGI_printf("	if (nNumSipSc > 4095 || nNumSipSc < 1) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Number of SIP ServerClasses (value must be > 0 and < 4096)\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Number of SIP Serverclasses (value must be > 0 and < 4096).\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	if (sType < 2) {\n");
	CGI_printf("		if (bdataEntryError) {\n");
	CGI_printf("			sErrorMsg = sErrorMsg + \", Options (you must check a configuration option)\";\n");
	CGI_printf("    	} else {\n");
	CGI_printf("     		sErrorMsg = sErrorMsg + \"Options (you must check a configuration option)\";\n");
	CGI_printf("     		bdataEntryError = true;\n");
	CGI_printf("    	}\n");
	CGI_printf("	}\n");
	CGI_printf("	if (bdataEntryError) {\n");
	CGI_printf("    	alert(sErrorMsg);\n");
	CGI_printf("    	return false;\n");
	CGI_printf("	} else {\n");
	CGI_printf("    	return true;\n");
	CGI_printf("	}\n");
	CGI_printf("}\n");
	CGI_printf("</SCRIPT>\n");
	CGI_printf("<SCRIPT Language=\"JavaScript\">\n");
	CGI_printf("function showHelp(msgindex) {\n");
	CGI_printf("nIndex = parseInt(msgindex, 10);\n");
	CGI_printf(" Win1 =\n");
	CGI_printf("open(\"\",\n");
	CGI_printf("			\"subWindow\",\n");
	CGI_printf("			\"toolbar=no,\" +\n");
	CGI_printf("			\"location=no,\" +\n");
	CGI_printf("			\"directories=no,\" +\n");
	CGI_printf("			\"status=no,\" +\n");
	CGI_printf("			\"menubar=no,\" +\n");
	CGI_printf("			\"scrollbars=yes,\" +\n");
	CGI_printf("			\"resizable=no,\" +\n");
	CGI_printf("			\"width=350,\" +\n");
	CGI_printf("			\"height=250\"\n");
	CGI_printf("	);\n");
	CGI_printf("if(nIndex == 1) {\n");
	CGI_printf("	var sText = \"The absolute path to a directory where configuration files will be written. If you supply the name of an existing directory, it must allow write access.  If you supply the name of a new directory, the Configuration Tool will attempt to create that directory.<P>\"\n");
	CGI_printf("    var sTitle = \"Configuration Directory\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 2) {\n");
	CGI_printf("	var sText = \"The pathmon name must begin with a $ and contain no more than 6 characters including the $.<P>\"\n");
	CGI_printf("    var sTitle = \"Pathmon Name\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 3) {\n");
	CGI_printf("	var sText = \"The name of a Guardian volume and subvolume where the TS/MP log files will be created. The format for this field is $volume.subvolume.Existing Volume name should be given. The maximum characters allowed for the SubVolume is 8. <P>\"\n");
	CGI_printf("    var sTitle = \"Guardian Volume/Subvolume\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 4) {\n");
	CGI_printf("	var sText = \"The absolute path to a directory where stderr and stdout will be redirected. If you supply the name of an existing directory, it must allow write access. If you supply the name of a new directory, the Configuration Tool will attempt to create that directory. The SIP will write status messages to a file in this directory. The file will have the subscript SIP.<P>\"\n");
	CGI_printf("    var sTitle = \"Log Path Directory\";\n");
	CGI_printf("}\n");
	CGI_printf("if(nIndex == 5) {\n");
	CGI_printf("	var sText = \"The format for this field is $process-name. If omitted, this parameter defaults to the home terminal of the cgi_sip process.<P>\"\n");
	CGI_printf("    var sTitle = \"HomeTerm\";\n");
	CGI_printf("}\n");
	CGI_printf("	Win1.document.write(\"<HTML><BODY BGCOLOR='ffffcc'><H2>\" + sTitle + \"</H2><HR><P></BODY></HTML>\" + sText)\n");
	CGI_printf("	Win1.focus();\n");
	CGI_printf("	Win1.document.close()\n");
	CGI_printf("}\n");
	CGI_printf("</SCRIPT>\n");

	CGI_printf("</HEAD>\n");
	CGI_printf("<BODY TEXT=\"#000000\" BGCOLOR=\"#FFFFFF\" LINK=\"#FF0000\" VLINK=\"#800080\" ALINK=\"#0000FF\">\n");
	CGI_printf("<H1>\n");
	if(type == CONFIGUREIT) {
		CGI_printf("<I><FONT COLOR=\"#3333FF\">SIP CONFIGURATION TOOL</FONT></I></H1>\n");
	} else if(type == RECONFIGUREIT) {
		CGI_printf("<I><FONT COLOR=\"#3333FF\">SIP RECONFIGURATION TOOL</FONT></I></H1>\n");
	} else if(type == REVIEWIT) {
		CGI_printf("<I><FONT COLOR=\"#3333FF\">SIP VIEW TOOL</FONT></I></H1>\n");
	}


	CGI_printf("<CENTER>\n");
	CGI_printf("<HR SIZE=5 NOSHADE WIDTH=\"100&#037;\"></CENTER>\n");
	if(type == CONFIGUREIT || type == RECONFIGUREIT)
		CGI_printf("<FORM name=\"config\" action=\"%s\" method=\"POST\" onsubmit=\"return CheckData(this.form)\">\n",getenv("SCRIPT_NAME"));
	if(type == CONFIGUREIT)
		CGI_printf("<BR><B><I>General configuration information</I></B><BR>&nbsp\n");
	else
		CGI_printf("<BR><B><I>Current configuration information<BR>(configuration directory %s, pathmon %s)</I></B><BR>&nbsp\n",
			confDir,initialSipInfo.pathmonName);

	CGI_printf("<TABLE BORDER COLS=2 WIDTH=\"60&#037;\" BGCOLOR=\"#CCCCCC\" >\n");
	if(type == CONFIGUREIT) {
		CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Configuration</FONT></B>\n");
		CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Dir:</FONT></B></TD>\n");
		if(confDir[0] == '0' || (strlen(confDir) < 2)) {
			CGI_printf("<TD><INPUT type=text name=\"CONFIGDIR\" size=50 maxlength=400>\n");
		} else {
			CGI_printf("<TD><INPUT type=text name=\"CONFIGDIR\" value=%s size=%d maxlength=400>\n",
				confDir,(strlen(confDir) + 2));
		}
		CGI_printf("<BR><B>The configuration directory.\n");
		CGI_printf("<BR><INPUT name=\"help1\" type=BUTTON value=\"More Info\" onclick=\"showHelp('1');\"></B>\n");
		CGI_printf("&nbsp;</TD>\n");
	} else {
		CGI_printf("<INPUT type=hidden name=\"CONFIGDIR\" value=\"%s\">\n",
			confDir);
	}
	if(type == CONFIGUREIT) {
		CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Pathmon</FONT></B>\n");
		CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Name:</FONT></B></TD>\n");
		CGI_printf("<TD><INPUT type=text name=\"PMONNAME\" size=7 value=\"$sip\" MIN=2 MAX=7>\n");
		CGI_printf("<BR><B>The name for the PATHMON process for this application.\n");
		CGI_printf("<BR><INPUT name=\"help2\" type=BUTTON value=\"More Info\" onclick=\"showHelp('2');\"></B>\n");
		CGI_printf("</TD>\n");
		CGI_printf("</TR>\n");
	} else {
		CGI_printf("<INPUT type=hidden name=\"PMONNAME\" value=%s>\n",initialSipInfo.pathmonName);
	}


	CGI_printf("<TR>\n");
	CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">Primary&nbsp;</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">cpu:</FONT></B></TD>\n");
	if (type == CONFIGUREIT)
		CGI_printf("<TD><SELECT name=\"PMPRIMCPU\"><OPTION value=\"0\">0<OPTION value=\"1\">1<OPTION value=\"2\">2<OPTION value=\"3\">3<OPTION value=\"4\">4<OPTION value=\"5\">5<OPTION value=\"6\">6<OPTION value=\"7\">7<OPTION value=\"8\">8<OPTION value=\"9\">9<OPTION value=\"10\">10<OPTION value=\"11\">11<OPTION value=\"12\">12<OPTION value=\"13\">13<OPTION value=\"14\">14<OPTION value=\"15\">15</SELECT>\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"PMPRIMCPU\" value=%d>\n",
			initialSipInfo.pmCpu);
	if(type == REVIEWIT)
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%d</B></FONT>\n",
			initialSipInfo.pmCpu);
	CGI_printf("<BR><B>The primary cpu where PATHMON will run.</B>&nbsp;\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Backup</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">cpu:</FONT></B></TD>\n");

	CGI_printf("<TD>\n");
	if(type == CONFIGUREIT)
		CGI_printf("<SELECT name=\"PMBACKCPU\"><OPTION value=\"1\">1<OPTION value=\"0\">0<OPTION value=\"2\">2<OPTION value=\"3\">3<OPTION value=\"4\">4<OPTION value=\"5\">5<OPTION value=\"6\">6<OPTION value=\"7\">7<OPTION value=\"8\">8<OPTION value=\"9\">9<OPTION value=\"10\">10<OPTION value=\"11\">11<OPTION value=\"12\">12<OPTION value=\"13\">13<OPTION value=\"14\">14<OPTION value=\"15\">15</SELECT>\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<INPUT type=text name=\"PMBACKCPU\" value=%d>\n",
			initialSipInfo.pmBackUpCpu);
	if(type == REVIEWIT)
		CGI_printf("<B><FONT COLOR=\"#3366FF\">%d</B></FONT>\n",
			initialSipInfo.pmBackUpCpu);

	CGI_printf("<BR><B>The backup cpu for the PATHMON process.</B>&nbsp;\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Guardian</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Volume/Subvolume:</FONT></B></TD>\n");

	if(type == CONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"PMONSUBVOL\" size=30 value=\"$system.sip\">\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"PMONSUBVOL\" size=30 value=\"%s\">\n",
			initialSipInfo.pmonSubVol);
	if(type == REVIEWIT)
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",
			initialSipInfo.pmonSubVol);
	CGI_printf("<BR><B>A volume and subvolume in the Guardian name space where TS/MP log files will be written.\n");
	CGI_printf("<BR><INPUT name=\"help3\" type=BUTTON value=\"More Info\" onclick=\"showHelp('3');\"></B>\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Log Path Directory:</FONT></B></TD>\n");
	if(type == CONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"LOGPATH\" size=50 maxlength=1022>\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"LOGPATH\" size=50 maxlength=1022 value=\"%s\">\n",
			initialSipInfo.logPath);
	if(type == REVIEWIT)
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",
			initialSipInfo.logPath);

	CGI_printf("<BR><B>The absolute path to a directory where the stderr and stdout files will be written.</B>&nbsp;\n");
	CGI_printf("<BR><INPUT name=\"help4\" type=BUTTON value=\"More Info\" onclick=\"showHelp('4');\"></B>\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Number of sets of</FONT></B>\n");
	CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">SIP Serverclasses:</FONT></B></TD>\n");

	if(type == CONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"NUMSIPSC\" size=3 value=1 MIN=1>\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"NUMSIPSC\" size=3 value=%d MIN=1>\n",
			initialSipInfo.numSipSC);
	if(type == REVIEWIT)
		CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%d</B></FONT>\n",
			initialSipInfo.numSipSC);
	CGI_printf("<BR><B>The number of different sets of servers that will be created for this");
	CGI_printf(" application. There will be a pool of servers for each set.&nbsp;</B>&nbsp;\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("<TR><TD><B><FONT COLOR=\"#3366FF\">Home Terminal:</FONT></B></TD>\n");
	if(type == CONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"HOMETERM\" size=30 >\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<TD><INPUT type=text name=\"HOMETERM\" size=30 value=\"%s\">\n",
			initialSipInfo.hometerm);
	if(type == REVIEWIT) {
		if(initialSipInfo.hometerm[0] != '\0')
			CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">%s</B></FONT>\n",
				initialSipInfo.hometerm);
		else
			CGI_printf("<TD><B><FONT COLOR=\"#3366FF\">Default Value</B></FONT>\n");
	}
	CGI_printf("<BR><B>The home terminal for the pathway system.</B>&nbsp;\n");
		CGI_printf("<BR><INPUT name=\"help5\" type=BUTTON value=\"More Info\" onclick=\"showHelp('5');\"></B>\n");
	CGI_printf("</TD></TR>\n");

	CGI_printf("</TABLE>\n");

	CGI_printf("<INPUT TYPE=hidden NAME=\"ServiceName\" VALUE=\"confPway\">\n");
	if(type == CONFIGUREIT) {
		CGI_printf("<BR><input type=hidden name=\"CURRENTSERVERNO\" value=0>\n");
		CGI_printf("<BR><B>Options: <BR><input type=RADIO value=\"CONFIGUREIT\" name=\"TYPE\">&nbsp;Configure and Start\n");
		CGI_printf("<BR><input type=RADIO value=\"CONFIGUREONLY\" name=\"TYPE\">&nbsp;Configure Only</B>\n");
		CGI_printf("<BR><INPUT type=submit name=\"continue\" value=\"Submit\">\n");

	}
	if(type == RECONFIGUREIT){
		CGI_printf("<BR><input type=hidden name=\"CURRENTSERVERNO\" value=%d>\n",
			initialSipInfo.currentServer);
		CGI_printf("<BR><B>Check \"Reconfigure and Start Pathway\" if you have changed any of the fields above. If the application is currently \n");
		CGI_printf("running, it will be shutdown. In addition, checking \"Reconfigure and Start Pathway\" will \n");
		CGI_printf("cause all configuration files to be rewritten.\n");
		CGI_printf("<BR><B>Check \"Reconfigure Only\" if you only want to rewrite the configuration files but do not \n");
		CGI_printf("<BR><B>want to shutdown the application.\n");
		CGI_printf("<BR><BR><B>Options: <BR><INPUT type=RADIO value=\"RECONFIGUREIT\" name=\"TYPE\">&nbsp;Reconfigure and Start Pathway\n");
		CGI_printf("<BR><INPUT type=RADIO value=\"RECONFIGUREONLY\" name=\"TYPE\">&nbsp;Reconfigure Only</B>\n");
		CGI_printf("<BR><INPUT type=submit name=\"submit\" value=\"Submit\"></BR></FORM>\n");
	}
	if(type == REVIEWIT) {
		CGI_printf("<P><B><A HREF=\"%s?ServiceName=start&CONFIGDIR=%s\">Return to Management Tool</A></B>\n",
			getenv("SCRIPT_NAME"),confDir);
	}
	CGI_printf("<BR><BR><A HREF=\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
			getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n\n");

	return;
}
/*********************************************************************
 * int getPathwayFields()
 *
 *	retrieves the fields from the form that contain the pathway
 *	configuration information.
 *********************************************************************/
int getPathwayFields()
{

	char *pmonName;
	char *logpath;
	char *pmonSubVol;
	char *pmonPrimCpu;
	char *pmonBackUpCpu;
        char pmonVolcpy[8];
        struct stat statBuf;
	char *numSipSC;
	char *hometerm;
	char *conf_fileDir;
	char *currentServer;
        char pmonVol[8];
        char *ptr;
	int i;
	short error, count, kind, entity_type;
	union optionBits_t{
		short options;
	   	struct f_b {
	 		unsigned int pad : 14;
			unsigned int subVolOK : 1;
			unsigned int restOfIt: 1;
	   	} flag_bits;
	} optionBits;

	initialSipInfo.pathmonName[0] = '\0';
	initialSipInfo.hometerm[0] = '\0';

	conf_fileDir = util_form_entry_value ("CONFIGDIR");
	if (conf_fileDir == NULL) {
			return FALSE;
	} else {
		strcpy(confDir,conf_fileDir);
		if(confDir[0] != '/') {
			strcpy(msg,"Data entry error: Configuration Directory must be an absolute path name");
			util_display_error_form(msg);
			return FALSE;
		}
	}

	pmonName = util_form_entry_value ("PMONNAME");
	if (pmonName == NULL) {
		return FALSE;
	} else {
		if(strlen(pmonName) > 6) {
			strcpy(msg,"Data entry error: Pathmon name must be no longer than 6 characters including the $");
			util_display_error_form(msg);
			return FALSE;
		}
		initialSipInfo.pathmonName[0] = pmonName[0];
		for(i = 1; i < strlen(pmonName); i++) {
			initialSipInfo.pathmonName[i] = tolower((int)pmonName[i]);
		}
		initialSipInfo.pathmonName[i] = '\0';
		if(initialSipInfo.pathmonName[0] != '$') {
			strcpy(msg,"Data entry error: Pathmon name must begin with a $");
			util_display_error_form(msg);
			return FALSE;
		}
		error = FILENAME_SCAN_(initialSipInfo.pathmonName,
				strlen(initialSipInfo.pathmonName),&count, &kind, &entity_type);
		if(error != 0 || entity_type != 0) {
			strcpy(msg,"Data entry error: name supplied for Pathmon name is not a valid process name");
			util_display_error_form(msg);
			return FALSE;
		}
	}

// Begin Changes for Solution Number 10-040512-5972

// Checking whether the CPU selected is UP or not. Displays appropriate message. 

         getCpuUpList();

	pmonPrimCpu = util_form_entry_value ("PMPRIMCPU");
	if (pmonPrimCpu == NULL)
    	return FALSE;
	else {
		initialSipInfo.pmCpu = atoi(pmonPrimCpu);
                if( ! checkCpu_Up(initialSipInfo.pmCpu)) {
                   strcpy(msg," Primary CPU is not up");   
                   util_display_error_form(msg);
                   return FALSE;
                }
            }
                   

	pmonBackUpCpu = util_form_entry_value("PMBACKCPU");
	if(pmonBackUpCpu == NULL)
		return FALSE;
	else {
   		initialSipInfo.pmBackUpCpu = atoi(pmonBackUpCpu);
                if( ! checkCpu_Up(initialSipInfo.pmBackUpCpu)) {
                  strcpy(msg," Backup CPU is not up");
                  util_display_error_form(msg); 
                }
             }
                  
                

	pmonSubVol = util_form_entry_value("PMONSUBVOL");
	if(pmonSubVol == NULL) {
		return FALSE;
	} else {
		strcpy(initialSipInfo.pmonSubVol,pmonSubVol);
		if(initialSipInfo.pmonSubVol[0] != '$') {
			strcpy(msg,"Data entry error: Guardian Vol/SubVol must be $volume.subvolume name");
			util_display_error_form(msg);
			return FALSE;
		}
		optionBits.flag_bits.pad = 0;
		optionBits.flag_bits.subVolOK = 1;
		optionBits.flag_bits.restOfIt = 0;
		error = FILENAME_SCAN_(initialSipInfo.pmonSubVol,
			strlen(initialSipInfo.pmonSubVol),&count, &kind, &entity_type,
				optionBits.options);
		if(error != 0 || count != strlen(initialSipInfo.pmonSubVol)) {
			strcpy(msg,"Data entry error: value supplied for Guardian Vol/SubVol is not a valid value");
			util_display_error_form(msg);
			return FALSE;
		}

/* Checking the Pathmon Volume provided is vaild or not */

           strcpy(pmonVol,pmonSubVol);
           ptr = strtok(pmonVol,".");
           ptr += 1;
           strcpy(pmonVolcpy,ptr);
           strcpy(pmonVol,"/G/");
           strcat(pmonVol,pmonVolcpy);
           if(stat(pmonVol, &statBuf) != 0) {
              if(errno == ENOENT) {
                 strcpy(msg,"Data entry error: value supplied for Guardian Volume is not valid");
                 util_display_error_form(msg);
                  return FALSE;
               }
           }
               
	}


// End Changes for Solution Number 10-040512-5972

	logpath = util_form_entry_value("LOGPATH");
	if(logpath == NULL) {
		return FALSE;
	 } else {
		strcpy(initialSipInfo.logPath,logpath);
		if(initialSipInfo.logPath[0] != '/') {
			strcpy(msg,"Data entry error: Logpath must be an absolute path name");
			util_display_error_form(msg);
			return FALSE;
		}
	}

	numSipSC = util_form_entry_value("NUMSIPSC");
	if(strcmp(numSipSC,"") == 0)  {
                strcpy(msg,"Data entry error: Number of Servers must be a number");
                util_display_error_form(msg);
		return FALSE;
	} else {
                initialSipInfo.numSipSC = atoi(numSipSC);
                for(i=0;i<strlen(numSipSC);i++) {
                 if(!isdigit((int)numSipSC[i])) {
                        strcpy(msg,"Data entry error: Number of Servers must be a number");
                        util_display_error_form(msg);
                        return FALSE;
                } 
              }
        }

	currentServer = util_form_entry_value("CURRENTSERVERNO");
	if(currentServer == NULL)
		return FALSE;
	else
		initialSipInfo.currentServer = atoi(currentServer);


	hometerm = util_form_entry_value("HOMETERM");
	if(hometerm == NULL)
		strcpy(initialSipInfo.hometerm,"none");
	else {
		strcpy(initialSipInfo.hometerm,hometerm);
		if(strcmp(initialSipInfo.hometerm,"none") != 0) {
			if(initialSipInfo.hometerm[0] == '\0') {
				strcpy(initialSipInfo.hometerm,"none");
			} else {
				if(initialSipInfo.hometerm[0] != '$') {
					strcpy(msg,"Data entry error: Hometerm must begin with a $");
					util_display_error_form(msg);
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}
/*********************************************************************/
void writeInitHiddenFields()
{
	CGI_printf("<INPUT TYPE=hidden NAME=\"PMONNAME\" VALUE=\"%s\">\n",
		initialSipInfo.pathmonName);

	CGI_printf("<INPUT TYPE=hidden NAME=\"PMPRIMCPU\" VALUE=\"%d\">\n",
		initialSipInfo.pmCpu);

	CGI_printf("<INPUT TYPE=hidden NAME=\"PMBACKCPU\" VALUE=\"%d\">\n",
		initialSipInfo.pmBackUpCpu);

	CGI_printf("<INPUT TYPE=hidden NAME=\"PMONSUBVOL\" VALUE=\"%s\">\n",
		initialSipInfo.pmonSubVol);

	CGI_printf("<INPUT TYPE=hidden NAME=\"LOGPATH\" VALUE=\"%s\">\n",
		initialSipInfo.logPath);

	CGI_printf("<INPUT TYPE=hidden NAME=\"NUMSIPSC\" VALUE=\"%d\">\n",
		initialSipInfo.numSipSC);

	CGI_printf("<INPUT TYPE=hidden NAME=\"NODENAME\" VALUE=\"%s\">\n",
		initialSipInfo.nodeName);

	CGI_printf("<INPUT TYPE=hidden NAME=\"CURRENTSERVERNO\" VALUE=\"%d\">\n",
		initialSipInfo.currentServer);

	CGI_printf("<INPUT TYPE=hidden NAME=\"HOMETERM\" VALUE=\"%s\">\n",
		initialSipInfo.hometerm);

	CGI_printf("<INPUT TYPE=hidden NAME=\"CONFIGDIR\" VALUE=\"%s\">\n",
		confDir);

	CGI_printf("<INPUT TYPE= hidden NAME=\"CONFFILE\" VALUE =\"%s\">\n",
		confFile);

	return;
}

// Begin Changes for Solution Number 10-040512-5972

/**************************************************************************
 * int getServerFields()
 *
 * 	retrieves information from the form about a server configuration
 *************************************************************************/
int getServerFields(short ssnum,short type)
{
        struct stat statBuf;
	char *nodename;
	char *javahome = NULL;
	char *tcpipProcess;
        char *ptcpipkey;
	char *classpath;
	char *servArglist;
	char *envvars;
	char *svrname = NULL;
        char servername[20];
	char *ptr;
	char *classname;
	char *currentServer;
	int i;
	char tempClassName[1095];

	if(getPathwayFields()== FALSE) {
		return FALSE;
	}

	nodename = util_form_entry_value ("NODENAME");
	if (nodename == NULL)
		return FALSE;
	else
		strcpy(initialSipInfo.nodeName,nodename);

	currentServer = util_form_entry_value("CURRENTSERVERNO");
	if(currentServer == NULL)
		return FALSE;
	else
		initialSipInfo.currentServer = (short) atoi(currentServer);


	javahome = util_form_entry_value ("JAVAHOME");
	if (javahome == NULL) {
		return FALSE;
	} else {
		strcpy(serverInfo.javaHome,javahome);
		if(serverInfo.javaHome[0] != '/') {
			strcpy(msg,"Data entry error: Java location contain an absolute path to the JVM");
			util_display_error_form(msg);
			return FALSE;
		}
                if(stat(javahome, &statBuf) != 0 ) {
                  if(errno == ENOENT) { 
                      strcpy(msg,"JavaHome : No such File");
                       util_display_error_form(msg);
                       return FALSE;
                  }
               }
	}

	tcpipProcess = util_form_entry_value ("TCPIPPROCESS");
	if (tcpipProcess == NULL) {
		return FALSE;
	} else {
		strcpy(serverInfo.tcpipProcessName,tcpipProcess);
		if(serverInfo.tcpipProcessName[0] != '$') {
			strcpy(msg,"Data entry error: Invalid value specified for tcpip process name");
			util_display_error_form(msg);
			return FALSE;
		}
    }

// check for valid key value

        ptcpipkey = util_form_entry_value ("PTCPIPKEY");
	if (ptcpipkey == NULL) {
		return FALSE;
	} else {
		strcpy(serverInfo.ptcpipkey,ptcpipkey);
                if(strlen(ptcpipkey) < 1 || strlen(ptcpipkey) > 8) {
                   strcpy(msg,"Data entry error: Number of characters should notexceed 8");
                  util_display_error_form(msg);
                   return FALSE;
               }
               if(isdigit((int)serverInfo.ptcpipkey[0])) {
                  strcpy(msg,"Data entry error:Invalid value specified for ptcpipkey");
                  util_display_error_form(msg);
                  return FALSE;

               } 
	  }

	classname = util_form_entry_value("CLASS_NAME");
	if(classname == NULL)
		return FALSE;
	else {
		strcpy(tempClassName,classname);
		ptr = strtok(tempClassName,".");
		ptr += strlen(tempClassName) + 1;
		if(strcmp(ptr,"class") == 0) {
			strcpy(serverInfo.className,tempClassName);
		} else {
			strcpy(serverInfo.className,classname);
		}
	}

	classpath = util_form_entry_value ("CLASS_PATH");
	if (classpath == NULL) {
		return FALSE;
	} else {
		strcpy(serverInfo.classpath,classpath);
		if(serverInfo.classpath[0] != '/') {
			strcpy(msg,"Data entry error: Class Path must be an absolute path name");
			util_display_error_form(msg);
			return FALSE;
		}
	}

	envvars = util_form_entry_value ("ENVVARS");
	if (envvars == NULL)
		strcpy(serverInfo.envVariables,"none");
	else {
		strcpy(serverInfo.envVariables,envvars);
		if(serverInfo.envVariables[0] == '\0')
			strcpy(serverInfo.envVariables,"none");
	}


	servArglist = util_form_entry_value("ARGLIST");
	if(servArglist == NULL)
		strcpy(serverInfo.serverArglist,"none");
	else {
		strcpy(serverInfo.serverArglist,servArglist);
		if(serverInfo.serverArglist[0] == '\0')
			strcpy(serverInfo.serverArglist,"none");
	}



		serverInfo.numstatic = 1; 


	svrname = util_form_entry_value ("SVRNAME");
	if (svrname == NULL) {
		return FALSE;
	} else {
		if(strlen(svrname) > 13) {
			strcpy(msg,"Data entry error: Name specified for serverclass name is more than 13 characters long.");
			util_display_error_form(msg);
			return FALSE;
		}

strcpy(servername,svrname);
for(i=0; i < strlen(servername); i++) {
serverInfo.serverClassRoot[i] = tolower((int)servername[i]);
}
serverInfo.serverClassRoot[i] = '\0';


/* appendig the server class name with charaters "01" to "15" accordingly to the server name */

strcat(servername,append[ssnum-1]);
		for(i=0; i < strlen(servername); i++) {
			serverInfo.serverClassName[i] = tolower((int)servername[i]);
		}

		serverInfo.serverClassName[i] = '\0';
	}
               serverInfo.serverClassCpus=cpulists[ssnum-1];
	return TRUE;
}

// End Changes for Solution Number 10-040512-5972


// Begin Changes for Solution Number 10-040512-5972

/***********************************************************************************
* configureSipServer(int writeFile, int type,int entry)
 * 	params:
 *		writeFile - either TRUE or FALSE -indicates whether the configuration files
 *					should be written.  Currently, this is TRUE unless type is
 *					RESTARTIT.
 *  	type - one of the usual types.
 *      entry - TRUE or FALSE indicates whether the entry has to be made in SIP General Info File
 *
 *	This routine configures and starts a Sip Server; optionally, it also writes the
 *	configuration file information for the server.
 **************************************************************************************/
int configureSipServer(short writeFile, short type,short entry)
{
        FILE *pfp;
	serverClassInfo_t scInfo;
	int error = 0;
	struct arglist_t arglist;
	struct env_t  envlist;
	char serverArglist[4095];
	char envVariables[4095];
	int i, len;
	short info = 0;
	short detail = 0;
         int noServer;
	int pmspi_ret = 0;
	char *ptr2;
	char *ptr;
	char *argPtr;
	char *envPtr;
	short status;
	int holdType = -1;

       

	initPmSpi();
	strcpy(scInfo.sc_name,serverInfo.serverClassName);
	strcpy(scInfo.gsubvol,initialSipInfo.pmonSubVol);
	strcpy(scInfo.logpath, initialSipInfo.logPath);
	scInfo.cpulist = serverInfo.serverClassCpus;

        
	strcpy(scInfo.cwddir, initialSipInfo.logPath);
	sprintf(scInfo.progname,"%s", serverInfo.javaHome);
	/* make the arglist*/
	arglist.len = 0;
	argPtr = (char *)&arglist.data[0];
	len = (short) sprintf(argPtr,"%s",serverInfo.className) + 1;
	arglist.len += len;
	argPtr += len;
	if(strcmp(serverInfo.serverArglist,"none") != 0) {
		strcpy(serverArglist,serverInfo.serverArglist);
		ptr2 = strtok(serverArglist,",");
		for(i = 0; ptr2 != NULL; i++) {
			len = sprintf(argPtr,"%s",ptr2) + 1;
			arglist.len += len;
			argPtr += len;
			ptr2 = strtok(NULL,",");
		}
	}
	envPtr = (char *)&envlist.data[0];
	envlist.len = 0;
	len = (short) sprintf(envPtr,"CLASSPATH=%s:",
		serverInfo.classpath) + 1;
	envlist.len += len;
	envPtr += len;
	if(strcmp(serverInfo.envVariables,"none") != 0) {
		strcpy(envVariables,serverInfo.envVariables);
		ptr2 = strtok(envVariables,",");
		for(i = 0; ptr2 != NULL; i++) {
			len = (short) sprintf(envPtr,"%s",ptr2) + 1;
			envlist.len += len;
			envPtr += len;
			ptr2 = strtok(NULL,",");
		}
	}
 
	if(type == RECONFIGURE_NEWSERVER) {
		status = pmproc_running();
		if(!status) {
			holdType = RECONFIGURE_NEWSERVER;
			type = CONFIGURENORUN;
		}
	}
	if(type == CONFIGUREIT || type == RECONFIGUREIT || type == CONFIGUREONLY ||
		type == RESTARTIT || type == RECONFIGURE_NEWSERVER) { 

                pmspi_ret = pmspi_addsc(&scInfo,&arglist,&envlist,&info,&detail); 
                if(pmspi_ret != 0) {

			errmsg = pmspi_errormsg(pmspi_ret,info,detail);
			strcpy(msg,errmsg);
			return pmspi_ret;
		}
		/* Add define for tcp/ip*/
             
		error = setUpDefine();
	    if(error != 0) {
		strcpy(msg,"Unable to set up tcp/ip define for server");
		return error;
            }

                /* Add define for Ptcp/ip FILTER KEY*/

                error = setUpDefineKey();

                if(error != 0) {
                  strcpy(msg,"Unable to set up  Filter key define for server");
                  return error;
                }


	}
	if(holdType != -1)
		type = holdType;

	if(writeFile) {
		ptr = (char *)&initialSipInfo.pathmonName[1];
		/* change made for solution no. 10-070910-7331 */
		sprintf(confFile,"%s_%s_pathway_server.pwy", ptr, serverInfo.serverClassRoot);
		sprintf(confFileName,"%s/%s",confDir,confFile);
		pmspi_ret = pmspi_writeServerConfig(confFileName,&scInfo,&arglist,&envlist);
		if(pmspi_ret == -1) {
			errno_errMsg("Unable to write server configuration file",errno,confFileName);
			return pmspi_ret;
		}
               noServer = generalFileInfo.sernum; 
		if(type == CONFIGUREIT || type == CONFIGURENORUN || type == CONFIGUREONLY) {
                        if(entry) 
			  writeServerCommands(confFile);
            } else
                        if(entry) {
			  writePathwayCommandsFiles(type);
                          writeServerCommands(confFile);
                        }
                generalFileInfo.sernum = noServer;

		error = writeServerExports(confDir);
		if(error != 0) {
			errno_errMsg("Unable to write server exports",errno,"Server Export File");
			return error;
		}
                if(entry == TRUE)
		if(type == CONFIGUREIT || type == CONFIGURENORUN || type == CONFIGUREONLY ) { 
                        strcpy(generalFileInfo.pathmonName,initialSipInfo.pathmonName);
                        strcpy(generalFileInfo.serverClassName,scInfo.sc_name);
                        pfp = openGeneralInfoFile("ab");
                        if(pfp == NULL) {
                            errno_errMsg(msg,errno,"sipGeneralInfo");  
                            return -1;
                        }
                        fwrite(&generalFileInfo,sizeof(generalFileInfo),1,pfp);
                        fclose(pfp);
                }
                if(entry == TRUE)
		if(type == RECONFIGURE_NEWSERVER || type == RECONFIGUREIT || type == RECONFIGUREONLY ) {
        strcpy(generalFileInfo.pathmonName,initialSipInfo.pathmonName);
        strcpy(generalFileInfo.serverClassName,scInfo.sc_name); 
			error = rewriteGeneralInfoFile(RECON_SERVER);
			if(error == -1) {
				errno_errMsg(msg,errno,"sipGeneralInfo");
				return error;
			}
		} 
	}
        
	return pmspi_ret;
}

// End Changes for Solution Number 10-040512-5972

/******************************************************************************/
int writeInitialExports(char *Path)
{
	FILE *pfp;
	char hiddenFileName[256];
	char *ptr;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	sprintf(hiddenFileName,"%s/.sipappl.conf.%s",Path,ptr);

	pfp = fopen(hiddenFileName,"w");
	if(pfp == NULL)
		return -1;
	fprintf(pfp,"export CONFILE=%s\n",confFile);
	fprintf(pfp,"export CONFIGDIR=%s\n",confDir);
	fprintf(pfp,"export PMONNAME=\\%s\n", initialSipInfo.pathmonName);
	fprintf(pfp,"export PMPRIMCPU=%d\n", initialSipInfo.pmCpu);
	fprintf(pfp,"export PMBACKCPU=%d\n", initialSipInfo.pmBackUpCpu);
	fprintf(pfp,"export LOGPATH=%s\n", initialSipInfo.logPath);
	fprintf(pfp,"export NUMSIPSC=%d\n", initialSipInfo.numSipSC);
	fprintf(pfp,"export NODENAME=%s\n", initialSipInfo.nodeName);
	if(strcmp(initialSipInfo.hometerm,"none") == 0) {
		fprintf(pfp,"export HOMETERM=%s\n", initialSipInfo.hometerm);
	} else {
		fprintf(pfp,"export HOMETERM=\\%s\n", initialSipInfo.hometerm);
	}
	fprintf(pfp,"export PMONSUBVOL=\\%s\n", initialSipInfo.pmonSubVol);
	fprintf(pfp,"export CURRENTSERVERNO=%d\n",initialSipInfo.currentServer);
	fclose(pfp);
	return 1;
}
/***************************************************************************/
void writeConfigDirError(char *msg)
{
	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML>\n");
	CGI_printf("%s\n", msg);
	CGI_printf("Please provide an absolute path to which this tool will write your configuration data:&nbsp;");
	CGI_printf("<FORM action=\"%s\" method=POST><input type=hidden name=ServiceName value=initConf><input type=text name=CONFIGDIR>\n",getenv("SCRIPT_NAME"));
	writeInitHiddenFields();
	CGI_printf("<p><input type=submit name=configure value=\"Continue Configuration\"></FORM>\n");
	CGI_printf("</BODY></HTML>\n\n");
	return;
}
/***********************************************************************************/
// Begin Changes for Solution Number 10-040512-5972

int writeServerExports(char *Path)
{
	FILE *pfp;
	char hiddenFileName[256];
	char *ptr;
        char *svrname;
        char servername[16];
        int i;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	sprintf(hiddenFileName,"%s/.sipappl.conf.%s.%s",
		Path,ptr, serverInfo.serverClassName);

	pfp = fopen(hiddenFileName,"w");
	if(pfp == NULL)
		return -1;

      svrname = util_form_entry_value ("SVRNAME");
      for(i=0;i<strlen(svrname);i++) {
         servername[i] = tolower((int)svrname[i]);
     }
          servername[i] = '\0';
     strcpy(svrname,servername);

//  Writing the CPU info to the Server Class file for reviewing the initial config

      if(strcmp(serverInfo.serverClassName,svrname)==0)
         fprintf(pfp,"export SER_CPU=%s\n",util_form_entry_value ("SCCPUS"));


        fprintf(pfp,"export SCCPUS=%d\n",serverInfo.serverClassCpus);
	fprintf(pfp,"export JAVAHOME=%s\n",serverInfo.javaHome);
	fprintf(pfp,"export TCPIPPROCESS=%s\n",serverInfo.tcpipProcessName);
        fprintf(pfp,"export PTCPIPKEY=%s\n",serverInfo.ptcpipkey);
	fprintf(pfp,"export CLASS_NAME=%s\n",serverInfo.className);
	fprintf(pfp,"export CLASS_PATH=%s\n",serverInfo.classpath);
	fprintf(pfp,"export ENVVARS=%s\n",serverInfo.envVariables);
	fprintf(pfp,"export ARGLIST=%s\n",serverInfo.serverArglist);
	fprintf(pfp,"export NUMSTATIC=%d\n",serverInfo.numstatic);
	fprintf(pfp,"export SRVNAME=%s\n",serverInfo.serverClassName);

	fclose(pfp);

	return 0;
}


// End Changes for Solution Number 10-040512-5972


// Begin Changes for Solution Number 10-040512-5972

/*********************************************************************
 * display_start_info
 *	display information about the current status of all applications
 *  configured in a specified configuration directory.
 ********************************************************************/
void display_start_info()
{
	char *conf_fileDir;
	FILE *pfp;
	char lastPathmonName[16];
        char lastserverClassName[25];
        char servername[18];
	char szMsg[1024];
	short error;
	int status = 1;
        int gotOneLine = 0;
	short serverCount = 0;
	short pathmonCount = 0;
	int wrotePathmonHeader = 0;
	int wroteStatusHeader = 0;
	short freezestate, running, info, detail;
	int pmspi_ret;
        int i;


	conf_fileDir = util_form_entry_value ("CONFIGDIR");
	if (conf_fileDir == NULL)
		return;
	else
		strcpy(confDir,conf_fileDir);

	lastPathmonName[0] = '\0';
        lastserverClassName[0] = '\0';
	pfp = openGeneralInfoFile("rb");
	if(pfp == NULL) {
		sprintf(msg,"Unable to open .sipGeneralInfo file in %s",confDir);
		errno_errMsg(msg,errno,".sipGeneralInfo");
		return;
	}
	/* Begin Changes for Solution Number 10-050318-5738  */
        /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
        while( !feof(pfp))
        {
         if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0)
         {
          gotOneLine = 1;
          strcpy(initialSipInfo.pathmonName,generalFileInfo.pathmonName);
          if(lastPathmonName[0] == '\0') {
             strcpy(lastPathmonName,generalFileInfo.pathmonName);
             pathmonCount++;
          } else {
                  if(strcmp(lastPathmonName,generalFileInfo.pathmonName) != 0) {
                      wrotePathmonHeader = 0;
                      strcpy(lastPathmonName,generalFileInfo.pathmonName);
                      if(serverCount == 0) {
                          writeNoServersEntry();
                      }
                      writePathmonEndEntry(generalFileInfo.pathmonName,pathmonCount,serverCount);
                      pathmonCount++;
                      serverCount = 0;
                   }
           }
           if(wroteStatusHeader == 0) {
                        /* Got a least one pathmon, serverclass combination, so
write form */
                        writeStatusHeader(confDir);
                        wroteStatusHeader = 1;
                }
                initPmSpi();
                status = pmproc_running();
                if(wrotePathmonHeader == 0) {
                        writePathmonEntry(generalFileInfo.pathmonName,status, pathmonCount);
                        wrotePathmonHeader = 1;
                }
                if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
                    continue;
                serverCount++;
                if(status == 0) {
                        writeServerEntry(generalFileInfo.serverClassName,serverCount);
                 // writing the status for the first CPU

                       writeCpuEntry(generalFileInfo.serverClassName, status,serverCount);
                   
                } else {
                        pmspi_ret = pmspi_statussc(generalFileInfo.serverClassName,&freezestate,&error, &info, &detail,&running);
                                                /* An error 1035 means that the
server is not configured; this is ok because they may have only configured it bu
t not started it */
                        if(pmspi_ret == 1035) {
                                running = 0;
                                pmspi_ret = 0;
                        }
                        if(pmspi_ret != 0) {
                                errmsg = pmspi_errormsg(pmspi_ret,info,detail);
                                util_display_error_form(errmsg);
                                return;
                        }

                        writeServerEntry(generalFileInfo.serverClassName,serverCount);
                        writeCpuEntry(generalFileInfo.serverClassName, running,serverCount);
                              
                }
                for(i=1;i<generalFileInfo.sernum;i++) {

// writing the status of the server class in each CPU

                  strcpy(servername,generalFileInfo.serverClassName);
                  strcat(servername,append[i]);
                  serverCount++;
                  if(status == 0 ) {
                     writeCpuEntry(servername,status,serverCount);
                  } else {
                     pmspi_ret = pmspi_statussc(servername,&freezestate, &error, &info, &detail,&running);
                   if(pmspi_ret == 1035) {
                      running = 0;
                      pmspi_ret = 0;
                   }
                   if(pmspi_ret != 0) {
                      errmsg = pmspi_errormsg(pmspi_ret,info,detail);
                      util_display_error_form(errmsg);
                      return;
                  }
                writeCpuEntry(servername, running,serverCount);
                }                      
             }
// writing the Start All and Stop All option for the Server class

               getCpuAll(generalFileInfo.serverClassName,generalFileInfo.sernum);
                
        }                
      }
      /* End Changes for Solution Number 10-050318-5738 */
            
	if(wrotePathmonHeader == 1) {
		if(serverCount == 0) {
			writeNoServersEntry();
		}
		writePathmonEndEntry(generalFileInfo.pathmonName,pathmonCount,serverCount);
		wrotePathmonHeader = 0;
	}

	if(gotOneLine == 1) {
		finishStatusPage(pathmonCount,serverCount);
	} else {
		sprintf(szMsg,"No SIP configuration files exist in %s",confDir);
		util_display_error_form(szMsg);
		fclose(pfp);
		return;
	}
	fclose(pfp);
	return;
}

// End Changes for Solution Number 10-040512-5972

/*************************************************************************/
void writeStatusHeader(char *confDir)
{
	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><HEAD>\n");
        CGI_printf("<TITLE>sipstatus</TITLE>\n");
        CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
        CGI_printf("function StartAll(form,checkname,count) {\n");
        CGI_printf("var name = new Array(\"\",\"01\",\"02\",\"03\",\"04\",\"05\",\"06\",\"07\",\"08\",\"09\",\"10\",\"11\",\"12\",\"13\",\"14\",\"15\");");
        CGI_printf("\n var data = \"Start\" ;");
        CGI_printf(" var check = new String(checkname);");
        CGI_printf("\n var checkn = check.substring(0,check.length-5);");
        CGI_printf("\n for (i=0;i<count;i++) {\n");
        CGI_printf("checkboxname = checkn+name[i];");
        CGI_printf(" box = eval(\"form.\"+checkboxname);");
        CGI_printf(" if(box.value == data) { \n");
        CGI_printf(" box.checked = true; } \n"); 
        CGI_printf(" else { \n");
        CGI_printf(" box.checked = false; } \n");
        CGI_printf("}\n");
        CGI_printf(" box = eval(\"form.\"+checkn+\"Stop\"); ");
        CGI_printf(" box.checked = false; ");
        CGI_printf("}\n");
        CGI_printf("</SCRIPT>\n");
        CGI_printf("<P><SCRIPT LANGUAGE=\"JavaScript\">\n");
        CGI_printf("function StopAll(form,checkname,count,pmon) {\n");
        CGI_printf("var name = new Array(\"\",\"01\",\"02\",\"03\",\"04\",\"05\",\"06\",\"07\",\"08\",\"09\",\"10\",\"11\",\"12\",\"13\",\"14\",\"15\");");
        CGI_printf("\n var data = \"Stop\" ;");
        CGI_printf(" var check = new String(checkname);");
        CGI_printf("\n var checkn = check.substring(0,check.length-4);");
        CGI_printf(" box = eval(\"form.\"+checkn+\"Start\"); ");
        CGI_printf(" box.checked = false; ");
        CGI_printf("\n for (i=0;i<count;i++) {\n");
        CGI_printf("checkboxname = checkn+name[i];");
        CGI_printf(" box = eval(\"form.\"+checkboxname);");
        CGI_printf(" if(box.value == data) { \n");
        CGI_printf(" box.checked = true; } \n");
        CGI_printf(" else { \n");
        CGI_printf("box.checked = false; } \n");
        CGI_printf("}\n");
        CGI_printf(" box = eval(\"form.\"+checkn+\"Start\"); ");
        CGI_printf(" box.checked = false; ");
        CGI_printf("}\n");
        CGI_printf("</SCRIPT>\n");
	CGI_printf("</HEAD><BODY>\n");

	CGI_printf("<CENTER><H1><I><FONT COLOR=\"#3333FF\">SIP Management Tool</FONT></I></H1></CENTER>\n");

	CGI_printf("<CENTER><HR SIZE=5 NOSHADE WIDTH=\"100&#037;\"></CENTER>\n");

	CGI_printf("<CENTER><H3>Applications Configured in %s</H3></CENTER>\n",confDir);
	CGI_printf("<CENTER><TABLE BORDER WIDTH=\"80&#037;\">\n");

	CGI_printf("<TR><TD ALIGN=CENTER VALIGN=CENTER><CENTER><B><FONT COLOR=\"#3333FF\">Pathmon Name</FONT></B></CENTER>\n");
	CGI_printf("</TD><TD ALIGN=CENTER><B><FONT COLOR=\"#3333FF\">Current Status</FONT></B></TD>\n");

	CGI_printf("<TD ALIGN=CENTER><B><FONT COLOR=\"#3333FF\">Action</FONT></B></TD></TR>\n");
	return;
}
/*************************************************************************/
void writePathmonEntry(char *pathmonName, short running, short pathmonCount)
{
	char status[20];
	char checkBoxText[20];
	char pathmonEnvName[20];
	char *ptr;

	if(running) {
		strcpy(status,"Running");
		strcpy(checkBoxText,"Stop");
	} else {
		strcpy(status,"Stopped");
		strcpy(checkBoxText,"Start");
	}
	strcpy(pathmonEnvName,"PATHMONNAME");
	ptr = (char *)&initialSipInfo.pathmonName[1];

	CGI_printf("<TR><TD><B><A HREF=\"%s?ServiceName=ViewConf&PMONNAME=%s&CONFIGDIR=%s\">%s</B><FORM name=manage action=%s method=POST><INPUT type=hidden name=\"%s\" value=\"%s\"></TD><TD><I>%s</I></TD>",
		getenv("SCRIPT_NAME"),pathmonName,confDir,pathmonName,getenv("SCRIPT_NAME"),pathmonEnvName,pathmonName,status);

	CGI_printf("<TD><INPUT type=checkbox name=PCHECK_%s value=\"%s\">%s</TD></TR><TR><TD>&nbsp;</TD>\n",
		ptr, checkBoxText, checkBoxText);

	CGI_printf("<TD><TABLE WIDTH=\"100&#37;\"><TR><TD ALIGN=LEFT><B><FONT COLOR=\"#3333FF\">ServerClassName</FONT></B></TD>\n");
        CGI_printf("<TD ALIGN=LEFT><B><FONT COLOR=\"#3333FF\">CPU</FONT></B></TD>\n");
	CGI_printf("<TD ALIGN=LEFT><B><FONT COLOR=\"#3333FF\">Status</FONT></B></TD>\n");

	CGI_printf("<TD><B><FONT COLOR=\"#3333FF\">Action</FONT></B></TD></TR>\n");

	return;
}
/*************************************************************************/
// Begin Changes for Solution Number 10-040512-5972

void writeServerEntry(char *serverClassName, short count)
{
	char serverClassEnv[20];
	char *ptr;
	ptr = (char *)&initialSipInfo.pathmonName[1];
	sprintf(serverClassEnv,"SCNAME_%d",count);
	CGI_printf("<TR><TD><B><A HREF=\"%s?ServiceName=ViewServ&TYPE=REVIEWIT&PMONNAME=%s&SVC=%s&CONFIGDIR=%s\">%s</B></TD></TR>\n",getenv("SCRIPT_NAME"),initialSipInfo.pathmonName,serverClassName,confDir,serverClassName);

	return;
}

/****************************************************************/

void getCpuAll(char *serverClassName,short count)
{
   char *ptr;
    ptr = (char *)&initialSipInfo.pathmonName[1];
   

   CGI_printf("<TR><TD></TD><TD><INPUT name=%s%sStart type=checkbox value=%d onClick=\"if (this.checked) return StartAll(this.form,this.name,this.value)\"\n>Start All</TD><TD><INPUT name=%s%sStop type=checkbox value=%d onClick=\"if (this.checked) return StopAll(this.form,this.name,this.value)\"\n>Stop All</TD></TR>",ptr,serverClassName,count,ptr,serverClassName,count);
return;
}
/****************************************************************/
void writeCpuEntry(char *serverClassName, short running,short count)
{
        char status[8];
        char checkBoxText[6];
        char serverClassEnv[20];
        char hiddenFileName[256];
        char checkBoxName[21];
        char *ptr;
        FILE *pfp;
        int CPU;
        char *ptr1;
        char line[5095];

        if(running) {
                strcpy(status,"Running");
                strcpy(checkBoxText,"Stop");
        } else {
                strcpy(status,"Stopped");
                strcpy(checkBoxText,"Start");
        }

        ptr = (char *)&initialSipInfo.pathmonName[1];
        sprintf(checkBoxName,"%s%s",ptr,serverClassName);
        sprintf(hiddenFileName,"%s/.sipappl.conf.%s.%s",confDir,ptr,serverClassName);
        pfp = fopen(hiddenFileName,"r");
         if(pfp == NULL) {
           util_display_error_form("Unable to open hidden file ");
           return;
         }

// get the CPU in which the Server Class is running
        
        while(fgets(line,sizeof(line),pfp)) {
          ptr1 = &line[0];
          ptr1 += 7;
          if(strncmp(ptr1,"SCCPUS",6) == 0) { 
              ptr1 +=7; 
              CPU = atoi(ptr1);
              break;
          }
        }
         sprintf(serverClassEnv,"SCNAME_%d",count);
               
        CGI_printf("<TR><TD></TD><TD>%d</TD><TD><I>%s</I></TD><TD><INPUT name=%s type=checkbox value=\"%s\">%s</TD><TD><INPUT type=hidden name=%s value=\"%s\"></TD></TR>\n",CPU,status,checkBoxName,checkBoxText,checkBoxText,serverClassEnv,serverClassName);
        fclose(pfp);
        return;
}


// End Changes for Solution Number 10-040512-5972

/*************************************************************************/
void writeNoServersEntry()
{
	char *ptr;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	CGI_printf("<TR><TD><B>No serverclasses configured</B></TD><TD>&nbsp;</TD><TD>&nbsp</TD></TR>\n");
	return;
}
/*************************************************************************/
void writePathmonEndEntry(char *pathmonName,short pathmonCount, short serverCount)
{
	char submitName[20];
	sprintf(submitName,"submit_%d",pathmonCount);

	CGI_printf("</TR></TABLE></TD><TD>&nbsp;<INPUT type=hidden name=\"ServiceName\" value=\"changeStatus\">\n");
	CGI_printf("<INPUT type=hidden name=\"PATHMONCOUNT\" value=%d>\n",
		pathmonCount);
	CGI_printf("<INPUT type=hidden name=\"SERVERCOUNT\" value=%d>\n",
		serverCount);
	CGI_printf("<INPUT type=hidden name=\"CONFIGDIR\" value=\"%s\">\n",
		confDir);
	CGI_printf("</TD></TR><TR>\n");
	CGI_printf("<TD>&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT type=submit name=\"%s\" value=\"Submit\"></FORM></TD>\n",
		submitName);
	CGI_printf("</TR>\n");
	return;
}
/*************************************************************************/
void finishStatusPage()
{
	CGI_printf("</TABLE></CENTER>\n");
	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n");
	return;

}
/**********************************************************************/
// Begin Changes for Solution Number 10-040512-5972

void getManagementRequest()
{
	char *pmon;
	char *confdir;
	char *serverClassName;
	char *pCheckBox;
	char *serverCounts;
	char *ptr;
	char pcheckValue[46];
	char serverEnvName[20];
	int serverCount, i;
	int error, pmspi_ret;
	short info,detail;
	int fileError;

	pmon = util_form_entry_value("PATHMONNAME");
	if(pmon != NULL) {
		strcpy(initialSipInfo.pathmonName,pmon);
	} else {
		util_display_error_form("Internal error: unable to obtain Pathmon Name");
		return;
	}
	ptr = (char *)&initialSipInfo.pathmonName[1];
	confdir = util_form_entry_value("CONFIGDIR");
	if(confdir != NULL) {
		strcpy(confDir,confdir);
	} else {
		util_display_error_form("Internal error: unable to obtain the configuration directory");
		return;
	}
	serverCounts = util_form_entry_value("SERVERCOUNT");
	if(serverCounts == NULL) {
		util_display_error_form("Internal error: unable to obtain SERVERCOUNT environmental variable");
		return;
	} else {
		serverCount = atoi(serverCounts);
	}
	error = readInitialExports(initialSipInfo.pathmonName);
	if(error == -1) {
		errno_errMsg("Unable to read pathway information",errno,"Pathway export file");
		return;
	}
	initPmSpi();
	sprintf(pcheckValue,"PCHECK_%s",ptr);
	pCheckBox = getenv(pcheckValue);
	if(pCheckBox != NULL) {
		if(strcmp(pCheckBox,"Stop") == 0) {
			if(serverCount > 0) {
				for(i = 1; i <= serverCount; i++) {
					sprintf(serverEnvName,"SCNAME_%d",i);
					serverClassName = util_form_entry_value(serverEnvName);
					if(serverClassName == NULL) {
						util_display_error_form("Internal error: unable to obtain serverclass name");
						return;
					}
					fileError = readServerExports(initialSipInfo.pathmonName,serverClassName);
					if(fileError == -1) {
						errno_errMsg("Unable to read server exports file",errno,"Server exports file");
						return;
					}

					pmspi_ret = stopServerClass(serverClassName);
					if(pmspi_ret != 0) {
						util_display_error_form(errmsg);
						return;
					}
				}
			}
			pmspi_ret = pmspi_shutdown(&info,&detail);
			if(pmspi_ret != 0) {
				errmsg = pmspi_errormsg(pmspi_ret,info,detail);
				util_display_error_form(errmsg);
				return;
			}
		} else {
			error = doInitialConfiguration(FALSE, RESTARTIT);
			if(error != 0) {
				util_display_error_form(msg);
				return;
			}
			if(serverCount > 0) {
				error = changeServerStatus(serverCount);
				if (error != 0) {
                                        util_display_error_form(errmsg);
					return;
                               }
			}
		}
	} else {
		if(serverCount > 0) {
			error = changeServerStatus(serverCount);
			if(error != 0) {
				util_display_error_form(errmsg);
				return;
			}
		}
	}
	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><BODY>\n");
	CGI_printf("<CENTER><H1>Management Request Completed Successfully</H1></CENTER>\n");
	CGI_printf("<HR SIZE=5 NOSHADE WIDTH=\"100&#037\">\n");
	CGI_printf("<P><B><A HREF=\"%s?ServiceName=start&CONFIGDIR=%s\">Return to Management Tool</B>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n\n");

	return;
}


// End Changes for Solution Number 10-040512-5972

/**************************************************************************************/
int changeServerStatus(int serverCount)
{
	int i;
	char *serverClassName;
	char *serverCheckBox;
	char serverEnvName[20];
	char serverCheckBoxName[22];
	int error, pmspi_ret;
	short info,detail;
	char *ptr;
	int readError;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	for(i = 1; i <= serverCount; i++) {
		sprintf(serverEnvName,"SCNAME_%d",i);
		serverClassName = util_form_entry_value(serverEnvName);
		if(serverClassName == NULL) {
			util_display_error_form("Internal error: unable to obtain serverclass name");
			return -1;
		}
		sprintf(serverCheckBoxName,"%s%s",ptr,serverClassName);
		serverCheckBox = getenv(serverCheckBoxName);
		if(serverCheckBox == NULL)
			continue;
		serverCheckBox = removeTrailingCharacters(serverCheckBox);
		/* This shouldn't happen cause it couldn't be running if the Pathmon wasn't running */
		if(strcmp(serverCheckBox,"Stop") == 0) {
			readError = readServerExports(initialSipInfo.pathmonName,serverClassName);
			if(readError == -1) {
				errno_errMsg("Unable to read server export file",errno,"Server Export file");
				return -1;
			}
			initPmSpi();
			pmspi_ret = stopServerClass(serverClassName);
			if(pmspi_ret != 0) {
				return -1;
			}
		} else {
			error = readServerExports(initialSipInfo.pathmonName,
					serverClassName);
			if(error != 0) {
				strcpy(msg,"Unable to find the hidden server configuration file");
				errmsg = (char *)&msg[0];
				return -1;
			}
			/* First try to start it */
			error = pmspi_startsc(serverInfo.serverClassName,&info,&detail);
			if(error == ZPWY_ERR_PM_NAMENOTDEFINED) {

				error = configureSipServer(FALSE, RESTARTIT,FALSE);
				if(error != 0) {
					return -1;
				}
				error = pmspi_startsc(serverInfo.serverClassName,&info,&detail);
				if(error != 0) {
					errmsg = pmspi_errormsg(error,info,detail);
					return -1;
				}
			} else if (error != 0) {
				errmsg = pmspi_errormsg(error, info,detail);
				return -1;
			}

		}
	}
    return 0;

}
/******************************************************************/
int readInitialExports(char *pathmonName)
{
	char hiddenFileName[256];
	char pmonName[16];
	char line[200];
	char envString[200];
	char envName[40];
	char envValue[100];
	char tempValue[100];
	char *ptr, *ptr1,*ptr2,*ptr3;
	FILE *pfp;

	strcpy(pmonName,pathmonName);
	ptr = (char *)&pmonName[1];
	sprintf(hiddenFileName,"%s/.sipappl.conf.%s",
		confDir,ptr);

	pfp = fopen(hiddenFileName,"r");
	if(pfp == NULL)
		return -1;
	while(fgets(line,sizeof(line),pfp)) {
		ptr1 = strtok(line," ");
		ptr1 = strtok(NULL," ");
		strcpy(envString,ptr1);
		ptr2 = strtok(envString,"=");
		strcpy(envName,envString);
		ptr2 = strtok(NULL,"=");
		strcpy(envValue,ptr2);
		ptr2 = strtok(envValue,"\n");

		if(strcmp(envName,"PMPRIMCPU") == 0) {
			initialSipInfo.pmCpu = atoi(envValue);
		}
		if(strcmp(envName,"PMBACKCPU") == 0) {
			initialSipInfo.pmBackUpCpu = atoi(envValue);
		}
		if(strcmp(envName,"LOGPATH") == 0) {
			strcpy(initialSipInfo.logPath, envValue);
		}
		if(strcmp(envName,"NUMSIPSC") == 0) {
			initialSipInfo.numSipSC = atoi(envValue);
		}
		if(strcmp(envName,"NODENAME") == 0) {
			strcpy(initialSipInfo.nodeName,envValue);
		}
		if(strcmp(envName,"HOMETERM") == 0) {
			strcpy(tempValue,envValue);
			if(strcmp(tempValue,"none") != 0) {
				ptr3 = (char *)&tempValue[1];
				strcpy(initialSipInfo.hometerm,ptr3);
			} else {
				strcpy(initialSipInfo.hometerm,tempValue);
			}
		}
		if(strcmp(envName,"PMONSUBVOL") == 0) {
			strcpy(tempValue,envValue);
			ptr3 = (char *)&tempValue[1];
			strcpy(initialSipInfo.pmonSubVol,ptr3);
		}
		if(strcmp(envName,"CURRENTSERVERNO") == 0) {
			initialSipInfo.currentServer = atoi(envValue);
		}
	}
	fclose(pfp);
	return 0;
}
/******************************************************************/
int readServerExports(char *pathmonName, char *serverClassName)
{
	char hiddenFileName[256];
	char pmonName[16];
	char line[5095];
	char envString[5096];
	char envName[100];
	char envValue[4095];
	char *ptr, *ptr1,*ptr2;
        char *qptr1, *qptr2;
	FILE *pfp;
	int i, j = 0;
	int len = 0;
	strcpy(pmonName,pathmonName);
	ptr = (char *)&pmonName[1];
	sprintf(hiddenFileName,"%s/.sipappl.conf.%s.%s",
		confDir,ptr,serverClassName);

	pfp = fopen(hiddenFileName,"r");
	if(pfp == NULL)
		return -1;

	while(fgets(line,sizeof(line),pfp)) {
		ptr1 = strtok(line," ");
		len = strlen(ptr1);
		ptr2 = ptr1 + len + 1;
               qptr1 = strchr(ptr2,'"');
              if(qptr1 != NULL) {
                  qptr1++;
                  qptr2 = strchr(qptr1,'"');
                  if(qptr2 != NULL) {
                  qptr2++;
                  qptr2 = '\0';
                  ptr1 = ptr2;
              } else {
                ptr1 = strtok(NULL, " ");
                }
             } else {
               ptr1 = strtok(NULL," ");
               } 
		strcpy(envString,ptr1);
		ptr2 = strtok(envString,"=");
		strcpy(envName,envString);
		ptr2 += strlen(envName) + 1;
		strcpy(envValue,ptr2);
		ptr2 = strtok(envValue,"\n");
		if(strcmp(envName,"JAVAHOME") == 0) {
			strcpy(serverInfo.javaHome,envValue);
		}
		if(strcmp(envName,"TCPIPPROCESS") == 0) {
			strcpy(serverInfo.tcpipProcessName,envValue);
		}
                if(strcmp(envName,"PTCPIPKEY") == 0) {
			strcpy(serverInfo.ptcpipkey,envValue);
		}
		if(strcmp(envName,"CLASS_NAME") == 0) {
			strcpy(serverInfo.className, envValue);
		}
		if(strcmp(envName,"CLASS_PATH") == 0) {
			strcpy(serverInfo.classpath,envValue);
		}
		if(strcmp(envName,"ARGLIST") == 0) {
			strcpy(serverInfo.serverArglist,envValue);
		}
		if(strcmp(envName,"ENVVARS") == 0) {
			strcpy(serverInfo.envVariables,envValue);
		}
		if(strcmp(envName,"SRVNAME") == 0) {
			if(strcmp(envValue,serverClassName) != 0) {
				return -1;
			} else {
				strcpy(serverInfo.serverClassName,serverClassName);
			}
		}

               if(strcmp(envName,"SER_CPU") == 0) {

                 if ( strcasecmp(envValue,"all") == 0 ) {
           	   for(i = 0; i < 16; i++) {
			serverInfo.cpuAssigned[i] = 1;
		   }
		} else {
	            for(i = 0; i < 16; i++) {
			serverInfo.cpuAssigned[i] = -1;
		    }
		    ptr = strtok(envValue,",");
                    i = 0;
		    while(ptr != NULL) {
			j = (short)atoi(ptr);
			serverInfo.cpuAssigned[i++] = j;
			ptr = strtok(NULL,",");
		     }
		}
               }
	}
	fclose(pfp);
	return 0;
}
/************************************************************************************/
int stopServerClass(char *serverClassName)
{
	int pmspi_ret;
	short freezestate, error, info, detail,running;
	int i = 0;

	initPmSpi();
	pmspi_ret = pmspi_statussc(serverClassName,&freezestate, &error, &info, &detail, &running);
	if(pmspi_ret != 0)
		return 0;
	if(freezestate == ZPWY_VAL_THAWED) {
		while(i < 2) {
			pmspi_ret = pmspi_freezesc(serverClassName,&info,&detail);
			if(pmspi_ret != 0 || pmspi_ret != ZPWY_ERR_PM_FREEZEINPROGRESS) {
				/* Try sleeping for a second up to three seconds */
				sleep(2);
				i++;
			} else {
				break;
			}
		}
		if(pmspi_ret != 0) {
			errmsg = pmspi_errormsg(pmspi_ret,info,detail);
			return pmspi_ret;
		}
		pmspi_ret = pmspi_stopsc(serverClassName,&info,&detail);
		if(pmspi_ret != 0) {
			errmsg = pmspi_errormsg(pmspi_ret,info,detail);
			return pmspi_ret;
		}
		pmspi_ret = pmspi_thawsc(serverClassName,&info,&detail);
		if(pmspi_ret != 0) {
			errmsg = pmspi_errormsg(pmspi_ret, info, detail);
			return pmspi_ret;
		}
	} else {
		if(running == 1) {
			pmspi_ret = pmspi_stopsc(serverClassName,&info,&detail);
			if(pmspi_ret != 0) {
				errmsg = pmspi_errormsg(pmspi_ret,info,detail);
				return pmspi_ret;
			}
		}
	}
	return pmspi_ret;
}

/*********************************************************************************/
void do_reconfigure(void)
{
	int i, j = 0;
	char *conf_fileDir;
	char *pmonName;
	char pathmonName[26];
	char nextline[500];
	char templine[200];
	int serverCount = 0;
	char *numServers;
	char *scName;
	char *pmonCheckBox;
	char *scCheckBox;
	char serverClassName[16];
	char serverClassEnvName[10];
	char scCheckBoxName[20];
	int reconfigurePathway = 0;
	int reconfigureServer = 0;
	int foundFirstServer = 0;

	nextline[0] = '\0';
	j = 0;
	conf_fileDir = util_form_entry_value ("CONFIGDIR");
	if (conf_fileDir == NULL) {
		util_display_error_form("Internal error: can't obtain configuration file directory name");
		return;
	} else {
		strcpy(confDir,conf_fileDir);
	}
	pmonName = util_form_entry_value("PMONNAME");
	if(pmonName == NULL) {
		util_display_error_form("Internal error: can't obtain Pathmon name");
		return;
	} else {
		strcpy(pathmonName,pmonName);
		strcpy(initialSipInfo.pathmonName,pathmonName);
	}
	pmonCheckBox = getenv("PATHWAY");
	if(pmonCheckBox != NULL)
		reconfigurePathway = 1;
	numServers = util_form_entry_value("SERVERCOUNT");
	if(numServers != NULL) {
		serverCount = atoi(numServers);
	}
	if(serverCount != 0) {
		for(i = 1; i <= serverCount;i++) {
			sprintf(serverClassEnvName,"SCNAME_%d",i);
			scName = util_form_entry_value(serverClassEnvName);
			if(scName == NULL) {
				strcpy(msg,"Internal error: unable to obtain serverclass name");
				util_display_error_form("msg");
				return;
			}
			sprintf(scCheckBoxName,"cb_%s",serverClassName);
			scCheckBox = getenv(scCheckBoxName);
			if(scCheckBox != NULL) {
				reconfigureServer = 1;
				if(reconfigurePathway == 0 && foundFirstServer == 0) {
					strcpy(serverInfo.serverClassName,serverClassName);
					foundFirstServer = 1;
				} else {
					sprintf(templine,"<INPUT type=hidden name=SCNAME_%d value=%s>",
						j,serverClassName);
					strcat(nextline,templine);
					j++;
				}
			}
		}
	}
	if(reconfigurePathway == 1) {
		display_config_info("reconfigure");
		return;
	} else if (reconfigureServer == 1) {
		writeServerPage(RECONFIGUREIT);
		return;
	}
	strcpy(msg,"No action requested");
	util_display_error_form(msg);
	return;
}
/**********************************************************************************
 * FILE *openGeneralInfoFile
 * 	param char* openMode
 *
 *	opens the .sipGeneralInfo file.  The procedure expects that the global variable
 *	confDir contains the name of the configuration directory.
 ***********************************************************************************/
FILE * openGeneralInfoFile(char *openMode)
{
	char generalInfoFileName[4095];
	FILE *pfp;

	strcpy(generalInfoFile,".sipGeneralInfo");
	sprintf(generalInfoFileName,"%s/%s",confDir,generalInfoFile);

	pfp = fopen(generalInfoFileName,openMode);
	return pfp;
}
/******************************************************************************/
int rewriteGeneralInfoFile(short type)
{
        FILE *pfp1;
        FILE *pfp2;
        char cmd[256];
        generalFileInfo_t readFileInfo;
        char tempGeneralInfoFileName[256];
        char generalInfoFileName[256];

        sprintf(generalInfoFileName,"%s/.sipGeneralInfo",confDir);
        sprintf(tempGeneralInfoFileName,"%s/.tempsipGenralInfo",confDir);

        pfp2 = fopen(tempGeneralInfoFileName,"wb");
        if(pfp2 == NULL) {
          strcpy(msg,"Internal Error: unable to rewrite .sipGeneralInfoFile");
          return -1;
        }

        pfp1 = openGeneralInfoFile("rb");
        if(pfp1 == NULL) {
          fclose(pfp2);
          strcpy(msg,"Internal Error: unable to rewrite .sipGeneralInfoFile");
          return -1;
        }
        switch(type) {
          case DELETE_SERVER :
		/* Begin Changes for Solution Number 10-050318-5738  */
               /* while(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != feof(pfp1)) { */
                while( !feof(pfp1))
                {
                 if(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != 0)
                 {
                 if(strcmp(readFileInfo.pathmonName,generalFileInfo.pathmonName) == 0 ){
              
                   if(strcmp(readFileInfo.serverClassName,generalFileInfo.serverClassName) == 0) 
                     continue;
                 }
                fwrite(&readFileInfo,sizeof(readFileInfo),1,pfp2);
              }
	     }
	     /* End Changes for Solution Number 10-050318-5738 */
               break;

          case DELETE_PWAY :
		/* Begin Changes for Solution Number 10-050318-5738  */
               /* while(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != feof(pfp1)) { */
	       while( !feof(pfp1))
               {
                 if(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != 0)
                 {
                 if(strcmp(readFileInfo.pathmonName,generalFileInfo.pathmonName)== 0 )
                 continue; 
                fwrite(&readFileInfo,sizeof(readFileInfo),1,pfp2);
               }
	      }
		/* End Changes for Solution Number 10-050318-5738 */
               break;
          case RECON_SERVER :
		/* Begin Changes for Solution Number 10-050318-5738  */
                /* while(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != feof(pfp1)) { */
	       while( !feof(pfp1))
               {
                 if(fread(&readFileInfo,sizeof(readFileInfo),1,pfp1) != 0)
                 {
                 if(strcmp(readFileInfo.pathmonName,generalFileInfo.pathmonName)== 0 ) 
                 if(strncmp(readFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0) {
                   fwrite(&readFileInfo,sizeof(readFileInfo),1,pfp2);
                   fwrite(&generalFileInfo,sizeof(generalFileInfo),1,pfp2);
                   continue;
               }
                 fwrite(&readFileInfo,sizeof(readFileInfo),1,pfp2);
               }         
              }
	      /* End Changes for Solution Number 10-050318-5738 */
                 break;
          default :
            strcpy(msg,"Not a valid type to Rewrite the File");
            fclose(pfp1);
            fclose(pfp2);
            return -1; 
         
         } 

         fclose(pfp1);
         fclose(pfp2);
         sprintf(cmd,"mv %s %s",tempGeneralInfoFileName, generalInfoFileName); 
         if(system(cmd)) {
              strcpy(msg,"Internal error: Unable to replace .sipGeneralInfo file");
              return -1;
        }
return 0;

}
            
/***************************************************************************
 * writePathwayCommandsFile
 *	param:
 *		int type - type of operation; RECONFIGUREIT, RECONFIGUREONLY, CONFIGUREIT,
 *										CONFIGUREONLY
 *	writes the commands necessary to configure this application from the command line.
 **********************************************************************************/
void writePathwayCommandsFiles(int type)
{
	char commandFile[256];
	char *ptr;
	FILE *pfp;
	char *cmd;
	char serverCommandsFile[256];
	char obeyFile[256];
	char pathwayConfFile[256];

	ptr = (char *)&initialSipInfo.pathmonName[1];
	/* changes for solution no. 10-070910-7331: file naming conventions changed */
	sprintf(commandFile,"%s/%s_start",confDir,ptr);
	sprintf(obeyFile,"%s/%s_pwystart.tacl",confDir,ptr);
	sprintf(pathwayConfFile,"%s_pathway_cold.pwy",ptr);

	/* Write commands to start application */
	pfp = fopen(commandFile,"w");
	if(pfp == NULL) {
          sprintf(msg,"Unable to open command file in %s",confDir);
          errno_errMsg(msg,errno,".sipGeneralInfo");
          return;
        }
        else {     
		fprintf(pfp,"gtacl -p fup purge \\%s.%so !\n",initialSipInfo.pmonSubVol,ptr);
		fprintf(pfp,"gtacl -p edit <%s\n",obeyFile);
		fprintf(pfp,"gtacl -c 'obey %s.%so'\n",initialSipInfo.pmonSubVol, ptr);
		fprintf(pfp,"gtacl -p pathcom \\%s < %s/%s\n",
			initialSipInfo.pathmonName,confDir,pathwayConfFile);
		fclose(pfp);
	}
	if(type == RECONFIGUREIT || type == RECONFIGUREONLY || type == RECONFIGURE_NEWSERVER) {
		pfp = openGeneralInfoFile("r");
		if(pfp == NULL) {
			sprintf(msg,"Unable to open .sipGeneralInfo file in %s",confDir);
			errno_errMsg(msg,errno,".sipGeneralInfo");
			return;
		}
		/* Begin Changes for Solution Number 10-050318-5738  */
                /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
		while( !feof(pfp))
                {
                 if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0 )
                 {
			if(strcmp(generalFileInfo.pathmonName,initialSipInfo.pathmonName) == 0) {
				if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
					continue;
					/* change made for solution no. 10-070910-7331 */
				sprintf(serverCommandsFile,"%s_%s_pathway_server.pwy",ptr, generalFileInfo.serverClassName);
				writeServerCommands(serverCommandsFile);
			}
		}
	       }
	       /* End Changes for Solution Number 10-050318-5738 */
		fclose(pfp);
	}


	cmd = malloc(strlen(commandFile) + 9);
	if(cmd != NULL) {
           sprintf(cmd,"chmod +x %s",commandFile);
	   if( errno = system(cmd) ) {
             errno_errMsg("Unable to change the mode of CommandFile",errno,commandFile);
           }
	}
	free(cmd);
	writePathmonObeyFile();

	return;
}
/*******************************************************************************
 * writePathmonObeyFile
 *
 * 	writes an obey file to start the PATHMON process.
 *******************************************************************************/
 void writePathmonObeyFile()
 {
 	char obeyFile[256];
	char *ptr;
	FILE *pfp2;
	char pathctlFileName[9];

	/* Write obey file to actually start pathway */
	ptr = (char *)&initialSipInfo.pathmonName[1];
	sprintf(obeyFile,"%s/%s_pwystart.tacl",confDir,ptr);
	pfp2 = fopen(obeyFile,"w");
        if(pfp2 == NULL) {
          sprintf(msg,"Unable to open command file in %s",confDir);
          errno_errMsg(msg,errno,".sipGeneralInfo");
          return;
        }
	else {
		fprintf(pfp2,"get %s.%so!\n", initialSipInfo.pmonSubVol, ptr);
		fprintf(pfp2,"a\n");
		fprintf(pfp2,"volume %s\n",initialSipInfo.pmonSubVol);
		if((strlen(initialSipInfo.pathmonName) - 1) <= 5) {
			sprintf(pathctlFileName,"%sctl",ptr);
		} else {
			sprintf(pathctlFileName,"%scl", ptr);
		}
		fprintf(pfp2,"assign pathctl,%s\n",pathctlFileName);
		fprintf(pfp2,"pathmon/name %s, cpu %d, out $NULL, nowait/\n",
			initialSipInfo.pathmonName, initialSipInfo.pmCpu);
		fprintf(pfp2,"//\n");
		fprintf(pfp2,"exit\n");
		fclose(pfp2);
	}
	return;
}
/*********************************************************************************
 * writeServerCommands
 *	param: char *fileName - the name of the file containing the serverclass
 *							configuration commands.  This file name has the form
 *							sipappl_<pathmonName>_<serverclassName>_conf
 ********************************************************************************/
void writeServerCommands(char *fileName)
{
	char commandFile[2000];
	char *ptr;
	FILE *pfp;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	/* change made for solution 10-070910-7331 */
	sprintf(commandFile,"%s/%s_start",confDir,ptr);
	/* Write commands to start server */
	pfp = fopen(commandFile,"a");
	if(pfp != NULL) {
		fprintf(pfp,"gtacl -p pathcom \\%s < %s\n",
			initialSipInfo.pathmonName,fileName);
		fclose(pfp);
               return;
	}
        else {
           sprintf(msg,"Unable to write Server commands in %s",confDir);
           util_display_error_form(msg);
           return;
       }

}
/***********************************************************************
 * displayConfigurationFinished
 *  param type : CONFIGUREIT, RECONFIGUREIT, REVIEWIT, CONFIGUREONLY
 *
 *		Displays a page indicating that the requested activity is
 *		complete.  The activity could be configuring and starting
 *		a new application, only configuring a new application,
 *		reconfiguring an existing application, or viewing an
 *		existing application.
 **********************************************************************/
void displayConfigurationFinished(int type)
{
	char *ptr;
	FILE *pfp;
	int numServers = 0;

	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><HEAD><TITLE>lastpage</TITLE></HEAD>\n");
	CGI_printf("<BODY>\n");
	if(type == CONFIGUREIT || type == CONFIGUREONLY) {
		CGI_printf("<CENTER><H1>Configuration Complete</H1></CENTER>\n");
	} else if (type == RECONFIGUREIT) {
		CGI_printf("<CENTER><H1>Reconfiguration Complete</H1></CENTER>\n");
	} else if(type == REVIEWIT) {
		CGI_printf("<CENTER><H1>Review Complete</H1></CENTER>\n");
	}

	CGI_printf("<HR SIZE=5 NOSHADE WIDTH=\"100&#037\">\n");
	if(type == CONFIGUREIT)
		CGI_printf("<BR>The application is configured and running.\n");
	if(type == RECONFIGUREIT)
		CGI_printf("<BR>The application is reconfigured and running.\n");

	CGI_printf("<BR><BR>Pathmon: %s\n",initialSipInfo.pathmonName);
	CGI_printf("<BR>ServerClasses: \n");

	pfp = openGeneralInfoFile("rb");
                if(pfp == NULL) {
                        sprintf(msg,"Unable to open .sipGeneralInfo file in %s",confDir);
                        errno_errMsg(msg,errno,".sipGeneralInfo");
                        return;
                }


            /* Begin Changes for Solution Number 10-050318-5738  */		
            /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
            while( !feof(pfp))
            {
              if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0 ) 
	      {
               if(strcmp(generalFileInfo.pathmonName,initialSipInfo.pathmonName) == 0) {
                 if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
                  continue;
                 else {
                       CGI_printf("&nbsp;%s",generalFileInfo.serverClassName);
                       numServers++;
                 }
        }
      }
    }
    /* End Changes for Solution Number 10-050318-5738 */
	if(numServers == 0)
		CGI_printf("&nbsp;No serverclasses configured");
	CGI_printf("<BR>\n");
	fclose(pfp);

	ptr = (char *)&initialSipInfo.pathmonName[1];
	if(type != REVIEWIT) {
		CGI_printf("<P>If you want to modify your configuration or move it to another system, \n");
		CGI_printf("you can edit the configuration files found in %s.&nbsp; These files are \n",
			confDir);
		CGI_printf("named :\n");
		CGI_printf("<BR>&nbsp;\n");
		CGI_printf("<UL>\n");
		CGI_printf("<LI>\n");
		/* change made for solution no. 10-070910-7331 */
		CGI_printf("%s_pathway_cold.pwy - which contains commands used to configure TS/MP.\n",
			ptr);
		if(numServers > 0 )
		/* change made for solution no. 10-070910-7331 */
			CGI_printf("<LI>%s_\"serverClassName\"_pathway_server.pwy - which contains commands to configure the appropriate serverclass.\n",
				ptr);

		CGI_printf("<LI>\n");
		/* change made for solution no. 10-070910-7331 */
		CGI_printf("%s_pwystart.tacl - which contains commands to create the PATHMON process for \n",
			ptr);
		CGI_printf("the application</LI>\n");

		CGI_printf("<LI>\n");
		/* change made for solution no. 10-070910-7331 */
		CGI_printf("%s_start - which contains commands to configure and start the \n",
			ptr);
		CGI_printf("application if this tool is unavailable</LI>\n");
		CGI_printf("</UL>\n");
		CGI_printf("Use the browser print function to print this page for later reference. \n");
	}
	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n");
	return;
}
/************************************************************************************/
int doesPMexist()
{

	FILE *pfp;
	char *ptr;
	char currentPathmonName[26];
	char line[500];

	pfp = openGeneralInfoFile("r");
	if(pfp == NULL) {
		return FALSE;
	}
	while(fgets(line,sizeof(line),pfp)) {
		ptr = strtok(line,",");
		strcpy(currentPathmonName,line);
		if(strcmp(currentPathmonName,initialSipInfo.pathmonName) == 0) {
			fclose(pfp);
			return TRUE;
		}
	}
	fclose(pfp);
	return FALSE;
}
/******************************************************************************/
void displayReconfigureDetail()
{
	char *conf_fileDir;
	char *pmonName;
	char pathmonName[26];
        char lastserverClassName[25];
	int foundPathmon = 0;
	int serverCount = 0;
	FILE *pfp;
	int error;
        lastserverClassName[0] = '\0';
        

	conf_fileDir = util_form_entry_value ("CONFIGDIR");
	if (conf_fileDir == NULL) {
		util_display_error_form("Internal error: can't obtain configuration file directory name");
		return;
	} else {
		strcpy(confDir,conf_fileDir);
	}
	pmonName = util_form_entry_value("PMONNAME");
	if(pmonName == NULL) {
		util_display_error_form("Internal error: can't obtain Pathmon name");
		return;
	} else {
		strcpy(pathmonName,pmonName);
	}
	pfp = openGeneralInfoFile("r");
	if(pfp == NULL) {
		errno_errMsg("Unable to open .sipGeneralInfo file",errno,".sipGeneralInfo");
		return;
	}
	initialSipInfo.pathmonName[0] = '\0';
	/* Begin Changes for Solution Number 10-050318-5738  */
        /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
        while( !feof(pfp))
        {
          if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0) 
          {
		if(strcmp(generalFileInfo.pathmonName,pathmonName) == 0) {
			if(foundPathmon == 0) {
				CGI_printf("Content-type: text/html\n\n");
				CGI_printf("<HTML><HEAD> <TITLE>siprecon2</TITLE></HEAD><BODY>\n");
				CGI_printf("<CENTER><H1>\n");
				CGI_printf("<FONT COLOR=\"#3366FF\">SIP Reconfiguration Tool</FONT></H1></CENTER>\n");

				CGI_printf("<CENTER><HR SIZE=5 NOSHADE WIDTH=\"100&#037\">\n");
				CGI_printf("<BR><B><FONT COLOR=\"#3366FF\">Reconfiguration Options for %s:</FONT></B>\n",pathmonName);
				CGI_printf("<BR><FORM name=\"reconf\" action=\"%s\" method=POST>\n",getenv("SCRIPT_NAME"));
				CGI_printf("<SELECT name=\"CHOICE\"><OPTION value=PWAY>Reconfigure Pathway");
				CGI_printf("<OPTION value=DELETE_PWAY>Delete Entire Application");
			}
			strcpy(initialSipInfo.pathmonName,pathmonName);
			foundPathmon = 1;
			if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
		           continue;
                     serverCount++;
			CGI_printf("<OPTION value=%s>Reconfigure %s",generalFileInfo.serverClassName,generalFileInfo.serverClassName);
			CGI_printf("<OPTION value=DELETE_%s>Delete %s",generalFileInfo.serverClassName,generalFileInfo.serverClassName);
	}
      }
     }
     /* End Changes for Solution Number 10-050318-5738 */
	if(foundPathmon == 0) {
		sprintf(msg,"A pathmon named %s is not configured in %s.",pathmonName, confDir);
		util_display_error_form(msg);
		return;
	}
	CGI_printf("<OPTION value=ADDNEWSC>Add new ServerClass</SELECT>\n");
	error = readInitialExports(initialSipInfo.pathmonName);
	if(error == -1) {
		errno_errMsg("Unable to read pathway export file",errno,"Pathway export file");
		return;
	}
	writeInitHiddenFields();
	CGI_printf("<INPUT type=hidden name=ServiceName value=ReconChoice>\n");
	CGI_printf("<INPUT type=hidden name=PMONNAME value=%s>\n",initialSipInfo.pathmonName);
	CGI_printf("<INPUT type=hidden name=CONFIGDIR value=%s>\n",confDir);
	CGI_printf("<INPUT type=hidden name=SERVERCOUNT value=%d\n",serverCount);
	CGI_printf("<BR><BR><INPUT type=submit name=reconfigure value=Reconfigure></FORM>\n");
	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</CENTER>\n");
	CGI_printf("</BODY></HTML>\n");
	return;
}
/**************************************************************************************/
void getReconChoice()
{
	char *choice;
        char cmd[256];
        char copyGeneralinfo[2050];
        FILE *pfp;
        char lastserverClassName[16];
        char servername[16];
        char pathmonName[7];
        int result;
        int i;
        int entry = TRUE;

        getPathwayFields();
        sprintf(cmd,"cp %s/.sipGeneralInfo %s/.sipGeneralInfoCopy",confDir,confDir);
        if(system(cmd)) {
           util_display_error_form("Unable to copy .sip GeneralInfo file");
           return;
        }

        strcpy(pathmonName,initialSipInfo.pathmonName);
        sprintf(copyGeneralinfo,"%s/.sipGeneralInfoCopy",confDir);
        pfp = fopen(copyGeneralinfo,"r");
        if(pfp == NULL) {
          util_display_error_form("Unable to open Copy of GenerealInfo File");
          return;
        }
        
	choice = util_form_entry_value ("CHOICE");
	if(choice == NULL) {
		util_display_error_form("Internal error: can't obtain reconfiguration choice");
                fclose(pfp);
		return;
	}
         
// Reconfgiure the Pathway

	if(strcmp(choice,"PWAY") == 0) {
		display_config_info("reconfigure");
	} 

// Delete the Pathway
        else if(strcmp(choice,"DELETE_PWAY") == 0) {  
		deleteApplication();
	} 

// Delete the Server Class
        else if(strncmp(choice,"DELETE",6) == 0 ) { 
             choice += strlen("DELETE_");  
             /* Begin Changes for Solution Number 10-050318-5738  */		
             /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp) ) { */
            while( !feof(pfp))
            {
             if( fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0 )
             {
              if(strcmp(generalFileInfo.pathmonName,pathmonName) == 0) {
                 if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)  
                   continue;
                 else if(strcmp(generalFileInfo.serverClassName,choice)==0) {
                   for(i=0;i<generalFileInfo.sernum;i++) {

                      strcpy(servername,generalFileInfo.serverClassName);
                      strcpy(choice,"DELETE_");
                      strcat(servername,append[i]);
                      strcat(choice,servername);

                      result = deleteServer(choice,entry,1);
                      if(result != 0) {
                        strcat(msg,"Error in deleting the Server class");
                        util_display_error_form(msg);
                        fclose(pfp);
                        return;
                      }

                      entry = FALSE;
                 }
            }       
          }
 
         }
        }
        /* End Changes for Solution Number 10-050318-5738 */
             displayServerConfigedPage(DELETEIT);
          }

// Add new Server Class to the Pathmon
          else if(strcmp(choice,"ADDNEWSC")==0) 
                writeServerPage(RECONFIGUREIT);

// Reconfgiure the Server
          else { 
	     /* Begin Changes for Solution Number 10-050318-5738  */
             /* while( fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
             while( !feof(pfp) )
             {
              if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0)
              {
              if(strcmp(generalFileInfo.pathmonName,pathmonName) == 0) {
                 if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
                   continue;
                 else if(strcmp(generalFileInfo.serverClassName,choice)==0) {
                   strcpy(lastserverClassName,choice);
                   for(i=0;i<generalFileInfo.sernum;i++) {
                      strcpy(servername,generalFileInfo.serverClassName);
                      strcpy(choice,"DELETE_");
                      strcat(servername,append[i]);
                      strcat(choice,servername);
                      result = deleteServer(choice,entry,0);
                      if(result != 0) {
                        strcat(msg,"  Error while trying to Reconfigure the Server Class");
                        util_display_error_form(msg);
                        fclose(pfp);
                        return;
                     } 
                      entry = FALSE;
 
                   }
                }
           }

         }
        }
         /* End Changes for Solution Number 10-050318-5738 */
                strcpy(choice,"DELETE_");
                strcat(choice,lastserverClassName);
		writeServerPage(RECONFIGUREIT);
	}
        fclose(pfp);
        sprintf(cmd,"rm %s",copyGeneralinfo);
        if( system(cmd) ) {
            util_display_error_form("Unable to remove the copy of GeneralInfo file");
        }

	return;
}
/****************************************************************************/

void displayReconfigurePwayFinish(int type)
{
	char *ptr;
	ptr = (char *)&initialSipInfo.pathmonName[1];

	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><HEAD><TITLE>lastpage</TITLE></HEAD>\n");
	CGI_printf("<BODY>\n");
	if(type != DELETEIT)
		CGI_printf("<CENTER><H1>Reconfiguration of Pathway Complete</H1></CENTER>\n");
	else
		CGI_printf("<CENTER><H1>Deletion of Application Complete</H1></CENTER>\n");

	CGI_printf("<HR SIZE=5 NOSHADE WIDTH=\"100&#037\">\n");
	if(type == RECONFIGUREIT) {
		CGI_printf("<BR>Pathmon is configured and running.\n");
		CGI_printf("<P>Use the <A HREF=\"%s?ServiceName=start&CONFIGDIR=%s\">Management Tool to start the associated serverclasses.</A></P>\n",
			getenv("SCRIPT_NAME"),serverInfo.serverClassName,confDir);
	}
	if(type != DELETEIT) {
	/* change made for solution no. 10-070910-7331 */
		CGI_printf("<P>A new pathway configuration file has been created called %s_pathway_cold.pwy. This file\n",ptr);
		CGI_printf("can be found in %s.</P>\n",confDir);
		CGI_printf("<P><B><A HREF=\"%s?ServiceName=reconfigure&PMONNAME=%s&CONFIGDIR=%s\">Reconfigure more objects in this application</A></B>\n",
			getenv("SCRIPT_NAME"),initialSipInfo.pathmonName,confDir);
	} else {
		CGI_printf("<P>The original configuration files can still be found in %s\n",
			confDir);
	}
	CGI_printf("<P><A HREF=\"%s?ServiceName=reviewhref&CONFIGDIR=%s\">Return to Reconfiguration Tool Main Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n");
	return;
}
/****************************************************************************/
void displayServerConfigedPage(int type)
{
	char *ptr;

	ptr = (char *)&initialSipInfo.pathmonName[1];
	CGI_printf("Content-type: text/html\n\n");
	CGI_printf("<HTML><HEAD><TITLE>lastpage</TITLE></HEAD>\n");
	CGI_printf("<BODY>\n");
	if(type == RECONFIGUREIT || type == RECONFIGUREONLY) {
		CGI_printf("<CENTER><H1>Reconfiguration of ServerClass Complete</H1></CENTER>\n");
	} else if(type == DELETEIT) {
		CGI_printf("<CENTER><H1>Deletion of ServerClass Complete</H1></CENTER>\n");
	} else {
		CGI_printf("<CENTER><H1>Addition of new ServerClass Complete</H1></CENTER>\n");
	}

	CGI_printf("<HR SIZE=5 NOSHADE WIDTH=\"100&#037\">\n");
	if(type == RECONFIGUREIT || type == CONFIGUREIT) {
		CGI_printf("<BR>ServerClass %s is configured and running.\n", serverInfo.serverClassName);
	}
	if(type == RECONFIGUREONLY || type == CONFIGUREONLY) {
		CGI_printf("<BR>ServerClass %s is configured.  Use the <A HREF=\"%s?ServiceName=start&CONFIGDIR=%s\">Management Tool</A> to start this serverclass.\n",
			getenv("SCRIPT_NAME"),serverInfo.serverClassName,confDir);
	}
	CGI_printf("<P><B><A HREF=\"%s?ServiceName=reconfigure&PMONNAME=%s&CONFIGDIR=%s\">Reconfigure more objects in this application</B></A>\n",
		getenv("SCRIPT_NAME"),initialSipInfo.pathmonName,confDir);

	CGI_printf("<P><A HREF=\"%s?ServiceName=reviewhref&CONFIGDIR=%s\">Return to Reconfiguration Tool Main Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);

	CGI_printf("<P><A HREF =\"%s?ServiceName=home&CONFIGDIR=%s\">Return to SIP Home Page</A>\n",
		getenv("SCRIPT_NAME"),confDir);
	CGI_printf("</BODY></HTML>\n");
	return;
}
/************************************************************************************/
int getType()
{
	char *mode;


	mode = getenv("TYPE");
	if(mode != NULL) {
		if(strcmp(mode,"CONFIGUREIT") == 0) {
			return CONFIGUREIT;
		}
		if(strcmp(mode,"CONFIGUREONLY") == 0) {
			return CONFIGUREONLY;
		}
		if(strcmp(mode,"RECONFIGUREIT")== 0) {
			return RECONFIGUREIT;
		}
		if(strcmp(mode,"RECONFIGUREONLY") == 0) {
			return RECONFIGUREONLY;
		}
		if(strcmp(mode,"REVIEWIT")== 0) {
			return REVIEWIT;
		}
		if(strcmp(mode,"DELETEIT") == 0) {
			return DELETEIT;
		}
	}
	return 0;
}
/************************************************************/
void deleteApplication()
{
	short error_detail, error;
	int result = -1;
	int status;
	FILE *pfp;
	char *ptr;
	char pathmonName[16];
	char cmd[256];

	strcpy(pathmonName,initialSipInfo.pathmonName);
	ptr = (char *)&pathmonName[1];

	status = pmproc_running();
	if(status) {
		/* It's running stop it so we can delete it */
		initPmSpi();
		result = pmspi_shutdown(&error, &error_detail);
		if(result != 0) {
			errmsg = pmspi_errormsg(result,error,error_detail);
			strcpy(msg,errmsg);
util_display_error_begin_form();
                        util_display_error_msg_form(msg);
util_display_error_end_form();
			return;
		}
	}
	sprintf(cmd,"rm %s/.sipappl.conf.%s",confDir,ptr);
	if( system(cmd) ) {
            util_display_error_form("Unable to remove the Pathomn Conf file");
        }
	pfp = openGeneralInfoFile("r");
        if(pfp == NULL) {
                util_display_error_form("Unable to open .sipGeneralInfo file");
                return;
        }
	/* Begin Changes for Solution Number 10-050318-5738  */
        /* while(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != feof(pfp)) { */
        while( !feof(pfp))
        {
          if(fread(&generalFileInfo,sizeof(generalFileInfo),1,pfp) != 0)
          {
		if(strcmp(initialSipInfo.pathmonName,generalFileInfo.pathmonName) == 0) {
			if(strncmp(generalFileInfo.serverClassName,"PATHMON_ONLY_ENTRY",17) == 0)
				continue;
			sprintf(cmd,"rm %s/.sipappl.conf.%s.%s", confDir,ptr,generalFileInfo.serverClassName);
			if(system(cmd)) {
                           util_display_error_form("Unable to remove the conf file");
                        }
		}
	}
       }
       /* End Changes for Solution Number 10-050318-5738 */
	fclose(pfp);
        strcpy(generalFileInfo.pathmonName,initialSipInfo.pathmonName);
        strcpy(generalFileInfo.serverClassName,"NULL");
	if(rewriteGeneralInfoFile(DELETE_PWAY) == -1){
		errno_errMsg(msg,errno,".sipGeneralInfo");
		return;
	}
	displayReconfigurePwayFinish(DELETEIT);
	return;
}
/**********************************************************************/
// Begin Changes for Solution Number 10-040512-5972

int deleteServer(char *serverClassName, short entry,short option)
{
        int result = -1;
        char *ptr;
        char *ptr2;
        char cmd[256];
        int pmspi_ret = 0;
        short detail, info;

        ptr = (char *)&initialSipInfo.pathmonName[1];
        ptr2 = serverClassName;
        ptr2 += strlen("DELETE_");

        initPmSpi();
        result = stopServerClass(ptr2);

        if(result != 0) {
          util_display_error_form(errmsg);
          return -1;
        }

        pmspi_ret = pmspi_deletesc(ptr2,&info,&detail);
                        if(pmspi_ret != 0) {
                                errmsg = pmspi_errormsg(pmspi_ret,info,detail);
                                strcpy(msg,errmsg);
                                return pmspi_ret;
                        }

        
        if(option || !entry) { 
           sprintf(cmd,"rm %s/.sipappl.conf.%s.%s",confDir,ptr,ptr2);
           if( errno = system(cmd) ) 
               errno_errMsg("Unable to remove the hidden file os ServerClass",errno,"Conf file");
        } 

        if(initialSipInfo.numSipSC > 0) {
                initialSipInfo.numSipSC--;
                if(writeInitialExports(confDir) == -1){
                        errno_errMsg("Unable to rewrite pathway configuration details",errno,"Initial Export File");
                }
        }

        // remove the entry from the Sip General Info File as the server class is deleted from the Pathmon

        if(entry) 
          if(rewriteGeneralInfoFile(DELETE_SERVER)== -1) {
                errno_errMsg(msg,errno,".sipGeneralInfo");
                return -1;
        }

        return 0;
}
// End Changes for Solution Number 10-040512-5972

/************************************************************************/
int setUpDefine()
{
	short oldValue;
	short checksum;
	short error;
	char attrName[16];
	char attrValue[25];
	char defineName[25];
	char buffer[475];
	short len, detail, info;
	union optionBits_t{
		short options;
	   	struct f_b {
		 	unsigned int pad : 15;
	     	unsigned int defineType : 1;
	   	} flag_bits;
	} optionBits;
	struct defineItem_t defineList;
	int pmspi_ret;

	error = DEFINEMODE(1,&oldValue);
	if(error != 0)
		return error;

	memset(attrName,' ',sizeof(attrName));
	memcpy(attrName,"CLASS",5);
	memcpy(attrValue,"MAP",3);
	len = 3;

	error = DEFINESETATTR(attrName, attrValue,len);
	if(error != 0)
		return error;
	memset(attrName,' ',sizeof(attrName));
	memcpy(attrName,"FILE",4);
	sprintf(attrValue,"%s.%s",initialSipInfo.nodeName,serverInfo.tcpipProcessName);
	strcpy(attrValue,serverInfo.tcpipProcessName);
	len = strlen(attrValue);
	error = DEFINESETATTR(attrName, attrValue,len);
	if(error != 0)
		return error;
	error = DEFINEVALIDATEWORK(&checksum);
	if(error != 0)
		return error;

	memset(defineName,' ',sizeof(defineName));
	memcpy(defineName,"=TCPIP^PROCESS^NAME",19);
	optionBits.flag_bits.pad = 0;
	optionBits.flag_bits.defineType = 1;
	error = DEFINESAVE(defineName,(short *)&buffer,sizeof(buffer),&len,optionBits.options);
        if(error != 0)
               return error;
	defineList.len = len;
	memcpy(defineList.data,buffer,len);
	pmspi_ret = pmspi_addDefine(serverInfo.serverClassName,"=TCPIP^PROCESS^NAME",
				&defineList, &info, &detail);
	if(pmspi_ret != 0) {
		errmsg = pmspi_errormsg(pmspi_ret,info,detail);
		return pmspi_ret;
	}

	return error;
}


/************************************************************************/
// Begin Changes for Solution Number 10-040512-5972

// add define for PTCP/IP Filter key 

int setUpDefineKey()
{
        short oldValue;
        short checksum;
        short error;
        char attrName[16];
        char attrValue[35];
        char defineName[25];
        char buffer[475];
        short len,detail, info;
        union optionBits_t{
                short options;
                struct f_b {
                        unsigned int pad : 15;
                unsigned int defineType : 1;
                } flag_bits;
        } optionBits;
        struct defineItem_t defineList;
        int pmspi_ret;

        error = DEFINEMODE(1,&oldValue);
        if(error != 0)
        {
                return error;
        }

        memset(attrName,' ',sizeof(attrName));
        memcpy(attrName,"CLASS",5);
        memcpy(attrValue,"MAP",3);

        error = DEFINESETATTR(attrName, attrValue,3);
        if(error != 0)
        {
                return error;
        }
        memset(attrName,' ',sizeof(attrName));
        memcpy(attrName,"FILE",4);

         sprintf(attrValue,"%s.%s.%s",initialSipInfo.nodeName,initialSipInfo.pmonSubVol,serverInfo.ptcpipkey);

        error = DEFINESETATTR(attrName, attrValue,strlen(attrValue));
        if(error != 0)
        {
                return error;
        }
        error = DEFINEVALIDATEWORK(&checksum);
        if(error != 0)
        {
                return error;
        }
        memset(defineName,' ',sizeof(defineName));
        memcpy(defineName,"=PTCPIP^FILTER^KEY",18);
        optionBits.flag_bits.pad = 0;

        optionBits.flag_bits.defineType = 1;
        error = DEFINESAVE(defineName,(short *)&buffer,sizeof(buffer),&len,optionBits.options);
        defineList.len = len;
        memcpy(defineList.data,buffer,len);
        pmspi_ret = pmspi_addDefine(serverInfo.serverClassName,"=PTCPIP^FILTER^KEY",&defineList, &info, &detail);
        if(pmspi_ret != 0) {
                errmsg = pmspi_errormsg(pmspi_ret,info,detail);
                return pmspi_ret;
        }

        return error;
}
// End Changes for Solution Number 10-040512-5972

/*********************************************************************************/
void initPmSpi()
{
	PHandle_t phandle;
	short len;

	pathmon_T.fnum = -1;
	PROCESSHANDLE_GETMINE_(phandle.data);
	PROCESSHANDLE_DECOMPOSE_(phandle.data,,,,initialSipInfo.nodeName,MAXNODENAMELEN,&len);
	initialSipInfo.nodeName[len] = '\0';

	pmspi_pm(initialSipInfo.pathmonName, initialSipInfo.nodeName);
	return;
}
/*****************************************************************************/

// Begin Changes for Solution Number 10-040512-5972
// Check whether the CPU  is Up or not 

short checkCpu_Up(short Cpu)
{

     if(((initialSipInfo.cpuList >> (15-Cpu)) & 1)==1)
        return TRUE;

     return FALSE;
}
/****************************************************************************
 * get the Number of CPUs that are Up and the CPU list that are Up.
 ****************************************************************************/

void getCpuUpList()
{
        long status;

        status=PROCESSORSTATUS();
        initialSipInfo.cpuList = 0x0000FFFF & status;
        return ;
}


// End Changes for Solution Number 10-040512-5972  


