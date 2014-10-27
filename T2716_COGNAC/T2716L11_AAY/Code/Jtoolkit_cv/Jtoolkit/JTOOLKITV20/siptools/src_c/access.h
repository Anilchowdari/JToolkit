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



#ifndef __ACCESS
#define __ACCESS
#include <stdlib.h>
#include "cgi_sip.h"

/*********************************************************************
 * display_welcome_form
 *   Displays the first form seen by the user.
 ********************************************************************/
void display_welcome_form ();
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
void display_config_info(char *);
/*********************************************************************
 * display_start_info
 *	display information about the current status of all applications
 *  configured in a specified configuration directory.
 ********************************************************************/
void display_start_info();
/*******************************************************************
 * display_got_config_href
 *
 *  Displays the initial menu for the Configuration Tool
 *******************************************************************/
void display_got_config_href();
/*******************************************************************
 * display_got_reconfigure_href
 *
 *  Displays the initial menu for the Reconfiguration Tool
 *******************************************************************/
void display_got_reconfigure_href();
/*******************************************************************
 * display_got_manage_href
 *
 *  Displays the initial menu for the Management Tool
 *******************************************************************/
void display_got_manage_href();
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
void displayConfigurationFinished(int);
/*********************************************************************************
 * char * errno_errMsg(char *BeginText, int err, char *pathName)
 *	params:
  *		BeginText - some text that identifies what you were trying to do when the error happened
 *		err - the value of errno
 *		fileName - the name of the file on which the error occurred.
 *
 *	displays the appropriate error message.
 *********************************************************************************/
void errno_errMsg(char *BeginText, int err, char *pathName);
void displayReconfigureDetail();
void displayReconfigurePwayFinish();
void displayServerConfigedPage(int);
/************************************************************************************
 * writeServerPage
 *
 *	param - type one of CONFIGUREIT, RECONFIGUREIT,CONFIGUREONLY or RECONFIGUREONLY
 *
 *  displays the html need to configure a server.
 ***********************************************************************************/
void writeServerPage(int);
void writeStatusHeader(char *);
void writePathmonEntry(char *, short, short);

// Begin Changes for Solution Number 10-040512-5972 

/*******************************************************************************
 * writeServerEntry
 *   params -  serverClassName and count ie. server count 
 * display the Server that has been configured under the Pathmon
 * provides the link to review the configuration of the Server class
 *
 ******************************************************************************/
void writeServerEntry(char *serverClassName, short count);

/*******************************************************************************
 * writeCpuEntry
 *   params - serverClassName, status of the server class and the server count 
 * display the CPU and the status of the server class whether it is running or 
 * stopped.
 *
 ******************************************************************************/
void writeCpuEntry(char *serverClassName,short running,short count);

/*******************************************************************************
 * getCpuAll
 * params serverClassName, number of CPU's in which the Servers are configured 
 *
 ******************************************************************************/
void getCpuAll(char *serverClassName,short CPU);

/*******************************************************************************
 *  checkCpu_Up
 * param - CPU
 * return TRUE or FALSE
 * Check whether the CPU is UP or not
 *
 ******************************************************************************/
short checkCpu_Up(short CPU);

/*******************************************************************************
 * getCpuUpList
 * param - implicit parameter, initialSipInfo.cpuList  contains the CPU list that are up in the system and initialSipInfo.cpuUp  count of CPU that are up 
 * updates the number of CPU and the CPU list that are UP in the system 
 *
 ******************************************************************************/
void getCpuUpList();

// End Changes for Solution Number 10-040512-5972 


void writeNoServersEntry();
void writePathmonEndEntry(char *, short, short);
void finishStatusPage();
/*********************************************************************
 * void get_pathway_info()
 *
 *	retrieves the information needed to configure or reconfigure
 *  the pathway application.  Also determines what type of
 *  configuration is being requested.
 **********************************************************************/
void get_pathway_info();
/*********************************************************************
 *  getPathwayFields()
 *
 *	retrieves the fields from the form that contain the pathway
 *	configuration information.
 *********************************************************************/
int  getPathwayFields();

// Begin Changes for Solution Number 10-040512-5972

/**************************************************************************
 * getServerFields()
 * param n'th server  of the Server Class and the type
 *
 * 	retrieves information from the form about a server configuration
 *************************************************************************/
int getServerFields(short sernum,short type);


// End Changes for Solution Number 10-040512-5972

void configureServer();

/***********************************************************************************
* configureSipServer(short writeFile, short type,short entry)
 * 	params:
 *		writeFile - either TRUE or FALSE -indicates whether the configuration files
 *					should be written.  Currently, this is TRUE unless type is
 *					RESTARTIT.
 *  	type - one of the usual types.
 *
 *	This routine configures and starts a Sip Server; optionally, it also writes the
 *	configuration file information for the server.
 **************************************************************************************/
int configureSipServer(short writeFile,short type ,short entry);
void writeInitHiddenFields();
int writeInitialExports(char *);
void writeConfigDirError(char *);
int writeServerExports(char *);
short checkPathmonRunning(char *);
int readInitialExports(char *);
int readServerExports(char *, char *);
void getManagementRequest();
int stopServerClass(char *);

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
int doInitialConfiguration(short, short);
int changeServerStatus(int);
void do_reconfigure();
/*******************************************************************************
 * rewriteGeneralInfoFile
 * params:
 *         type - DELETE_SERVER or DELETE_PWAY or RECON_SERVER
 * Rewrite the Sip General Info file
 *******************************************************************************/
int rewriteGeneralInfoFile(short);
/**********************************************************************************
 * FILE *openGeneralInfoFile
 * 	param char* openMode
 *
 *	opens the .sipGeneralInfo file.  The procedure expects that the global variable
 *	confDir contains the name of the configuration directory.
 ***********************************************************************************/
FILE * openGeneralInfoFile(char *);
/***************************************************************************
 * writePathwayCommandsFile
 *	param:
 *		int type - type of operation; RECONFIGUREIT, RECONFIGUREONLY, CONFIGUREIT,
 *										CONFIGUREONLY
 *	writes the commands necessary to configure this application from the command line.
 **********************************************************************************/
void writePathwayCommandsFiles(int);
/*******************************************************************************
 * writePathmonObeyFile
 *
 * 	writes an obey file to start the PATHMON process from the command line.
 *******************************************************************************/
void writePathmonObeyFile();
int doesPMexist();
/*******************************************************************************
 * getReconChoice
 *
 *    get the reconfiguration option, the option is any of the ADD New Server to *    the Pathmon,Reconfigure Pathmon,Reconfigure Server class,Delete the Server
 *    Class and Delete the Pathmon
*******************************************************************************/
void getReconChoice();
int getType();
/*********************************************************************************
 * writeServerCommands
 *	param: char *fileName - the name of the file containing the serverclass
 *							configuration commands.  This file name has the form
 *							sipappl_<pathmonName>_<serverclassName>_conf
 ********************************************************************************/
void writeServerCommands(char *);
void deleteApplication();

// Begin Changes for Solution Number 10-040512-5972 
/*******************************************************************************
 * int deleteServer
 * params - ServerClass name  
 *          entry TRUE or FALSE
 * returns pmspi_ret if some error occurs
 * delete the Server Class and remove the entry from SIP General Info file if thentry is TRUE
 *
 *****************************************************************************/

int deleteServer(char *serverClassName,short entry,short option);

// End Changes for Solution Number 10-040512-5972

int setUpDefine();

// Begin Changes for Solution Number 10-040512-5972
/*******************************************************************************
 * int setUpDefineKey
 * param - PTCP/IP Filter key ,implicit parameter , maximum 8 characters, alphanumeric, first character must be a letter
 * returns pmspi_ret if some error occurs
 * add the define for the PTCP/IP Filter Key 
 *
 *****************************************************************************/

int setUpDefineKey();

// End Changes for Solution Number 10-040512-5972
void initPmSpi();

#define READIT 100
#define CLOSEIT 200
#define MAXINFOLINESIZE 100
#endif
