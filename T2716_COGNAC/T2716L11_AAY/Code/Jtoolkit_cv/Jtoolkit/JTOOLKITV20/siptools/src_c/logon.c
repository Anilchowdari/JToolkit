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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <cgilib.h>
#include "util.h"
#include "access.h"
#include "cgi_sip.h"


/************************************************************************/
int CGI_main (int argc, char *argv[])
{
	char * pServiceName;

	if (strcmp(getenv("REQUEST_METHOD"), "HEAD") == 0) {
    	CGI_printf ("Content-type: text/html\n\n");
    	return 0;
    }

	if (strcmp(getenv("REQUEST_METHOD"), "POST") != 0 &&
    	strcmp(getenv("REQUEST_METHOD"), "GET") != 0) {
    	char szMsg[1024];
    	sprintf (szMsg, "Unrecognized REQUEST_METHOD: %s \n",
                    getenv("REQUEST_METHOD"));
    	util_display_error_form (szMsg);
    	return 0;
	}

	if (getenv("ServiceName") == NULL) {
    	display_welcome_form ();
    	return 0;
	}

	pServiceName = util_form_entry_value ("ServiceName");

	if(strncmp(pServiceName,"home",4) == 0) {
		display_welcome_form();
		return 0;
	}

	if(strncmp(pServiceName,"configure",9) == 0) {
		display_config_info(pServiceName);
		return 0;
	}
	if(strcmp(pServiceName,"confhref") == 0) {
		display_got_config_href();
		return 0;
	}
	if(strcmp(pServiceName,"ViewConf") == 0) {
		display_config_info("reviewit");
		return 0;
	}
	if(strcmp(pServiceName,"ViewServ") == 0) {
		writeServerPage(REVIEWIT);
		return 0;
	}
	if(strcmp(pServiceName,"reviewhref") == 0) {
		display_got_reconfigure_href();
		return 0;
	}
	if(strcmp(pServiceName,"managehref") == 0) {
		display_got_manage_href();
		return 0;
	}
	if(strcmp(pServiceName,"reconfigure") == 0) {
		displayReconfigureDetail(pServiceName);
		return 0;
	}
	if(strcmp(pServiceName,"ReconChoice") == 0) {
		getReconChoice();
		return 0;
	}
	if (strncmp(pServiceName,"confPway",10) == 0) {
    	get_pathway_info();
    	return 0;
	}
	if(strcmp(pServiceName,"DoReConfigure") == 0) {
		do_reconfigure();
		return 0;
	}

	if (strncmp(pServiceName, "sipServers",11) == 0) {
    	configureServer();
    	return 0;
    }

	if (strncmp(pServiceName, "start", 5) == 0) {
    	display_start_info();
    	return 0;
    }
	if (strncmp(pServiceName, "changeStatus", 12) == 0) {
		getManagementRequest();
		return 0;
	}

	{
	char szMsg[1024];
	sprintf (szMsg, "Service ""%s"" is not supported by CGI program ""%s"" ",
                pServiceName, getenv("SCRIPT_NAME"));
	util_display_error_form (szMsg);
	}

	return 0;
}
