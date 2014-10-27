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


#ifndef __CGI_SIP
#define __CGI_SIP
#include "pmspi.h"
typedef struct generalFileInfo_ {
        char pathmonName[7];
        char serverClassName[18];
        short sernum;
} generalFileInfo_t;
generalFileInfo_t generalFileInfo;

typedef struct initialSipInfo_ {
	char pathmonName[7];
	short pmCpu;
	short pmBackUpCpu;
	char pmonSubVol[18];
	char logPath[256];
	char nodeName[9];
	int numSipSC;
	int currentServer;
	char hometerm[36];
        int cpuList;
} initialSipInfo_t;
initialSipInfo_t initialSipInfo;

typedef struct generalServerInfo_{
	char javaHome[256];
	char tcpipProcessName[36];
        char ptcpipkey[9];
	char classpath[4095];
	char serverArglist[256];
	char envVariables[4095];
	int numstatic;
        char className[256];
	char serverClassName[16];
        char serverClassRoot[14];
	short serverClassCpus;
        short cpuAssigned[CPUSPERNODE]; 
} generalServerInfo_t;
generalServerInfo_t serverInfo;

typedef struct PHandle_t {
	short data[10];
}PHandle_t;

#define TRUE 1
#define FALSE 0
#define CONFIGUREIT 100
#define RECONFIGUREIT 200
#define REVIEWIT 300
#define CONFIGUREONLY 400
#define RESTARTIT 500
#define CONFIGURENORUN 600
#define RECONFIGURE_NEWSERVER 700
#define RECONFIGUREONLY 800
#define DELETEIT 900
#define DELETE_SERVER 1000
#define DELETE_PWAY 1100
#define RECON_SERVER 1200
#define MAXSCS 4095
#endif
