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

/* General helper routines  */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
/* The D40 compilers require that external definitions are defined
	once and only once in an object module.
*/
#define _CGI_NO_EXTERNALS
#include <cgilib.h>

#include "util.h"
#if !defined(MAXNAMELEN)
#if defined(NAME_MAX)
#define MAXNAMELEN NAME_MAX
#else
#define MAXNAMELEN 1024
#endif
#endif

/******************************************************************/

void util_display_error_form (char * pText)
{
CGI_printf ("Content-type: text/html\n\n");
CGI_printf ("<title>Message</title>\n");
CGI_printf ("<h1>Message</h1>\n");
CGI_printf ("<h2>%s</h2>\n", pText);
CGI_printf ("</html>\n");
return;
}
/******************************************************************/
void util_display_warn_form(char *pText)
{
CGI_printf ("<title>Message</title>\n");
CGI_printf ("<h1>Warning</h1>\n");
CGI_printf ("<h2>%s</h2>\n", pText);
return;
}

void util_display_error_begin_form ()
{
CGI_printf ("Content-type: text/html\n\n");
CGI_printf ("<title>Message</title>\n");
return;
}
/******************************************************************/

void util_display_error_msg_form (char * pText)
{
CGI_printf ("<h2>%s</h2>\n", pText);
return;
}
/******************************************************************/

void util_display_error_end_form ()
{
CGI_printf ("</html>\n");
return;
}

/******************************************************************/
/* Get the value associated with a name in a CGI request */

char * util_form_entry_value (char * pName)
{
char * pTemp;
char *ptr;
int ch, i;

	pTemp = getenv(pName);
	if (pTemp == NULL) {
    	char szMsg[1024];
    	sprintf (szMsg, "Could not find form field name: %s", pName);
    	util_display_error_form (szMsg);
    	return NULL;
    }
	ptr = pTemp;
	for(i = 0; i < strlen(pTemp); i++) {
		ch = *ptr;
		if((ch == 13) || (ch == 10)) {
			*ptr = '\0';
			break;
		}
		ptr++;
	}
	return pTemp;
}
/************************************************************************************/
char * removeTrailingCharacters(char *pName)
{
	char *ptr;
	int ch, i;

	ptr = pName;
	for(i = 0; i < strlen(pName); i++) {
		ch = *ptr;
		if((ch == 13) || (ch == 10)) {
			*ptr = '\0';
			break;
		}
		ptr++;
	}
	return pName;
}

/********************************************************************/
/* Certain characters must be escaped if they appear in HTML - so */
/* data coming from a database must be escaped to remove the */
/* special characters */

char * util_escape_html (char * pString)
{
char * pIn;
static char szBuf[4096];
char szTemp[2];

/* This is not very efficient but it gives the general idea. */
/* It's possible that characters above decimal code 127 should */
/* also be escaped - beware Europeans!  */
szBuf[0] = '\0';
for (pIn=pString; *pIn; pIn++)
    switch (*pIn) {
       case '<':
          strcat (szBuf, "&lt;");
          break;
       case '>':
          strcat (szBuf, "&gt;");
          break;
       case '&':
          strcat (szBuf, "&amp;");
          break;
       case '\"':
          strcat (szBuf, "&quot;");
          break;
       default:
          szTemp[0] = *pIn;
          szTemp[1] = '\0';
          strcat (szBuf, szTemp);
          break;
       }
return szBuf;
}
/********************************************************************/

char * util_format_money (long long * pAmount)
{
double dAmount;
static char szBuf[64];

dAmount = (double) *pAmount;
dAmount = dAmount/100;
sprintf (szBuf, "%.2f", dAmount);
return szBuf;
}
