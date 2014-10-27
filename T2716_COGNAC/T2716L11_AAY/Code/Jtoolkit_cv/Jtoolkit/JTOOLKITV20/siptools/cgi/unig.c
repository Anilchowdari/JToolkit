
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <cextdecs.h(                                    \
                   DEBUG                                 \
                  ,AWAITIOX                              \
                  ,FILEINFO                              \
                  )>  

#include <tal.h>

#include "uniguard.h"

/**************************************************************************/
/**** Changed sock type to int to avoid warning-26/7/2000****/
int cgilib_tcpwait (int sock, long * tag, long timelimit)
/************************************************************/	
{
	_cc_status c_status; 
    short tmpFilenum;
    long tmpTag;
    short ret;
    /****Type casted to avoid warning-26/7/2000******/
    tmpFilenum = (short)sock;
   /************************************************/
    tmpTag = 0;
    ret = 0;
    c_status = AWAITIOX (&tmpFilenum,,,&tmpTag,timelimit);  
    if (_status_eq(c_status))
        *tag = tmpTag;
    else {
        FILEINFO (tmpFilenum, &ret);
        }
    return ret;
    }                      

/**************************************************************************/

void filepathToGuardian (char *guardian, char * filepath)
{
	int i, j;

	for (i=0, j=0; filepath[i] && i< MAXGUARDFILE-1; i++) {
		if (filepath[i] == '/') {
            if (i==0 || filepath[i+1] == '\0')
                ;
            else
			    guardian[j++] = '.';		

            continue;
            }
        
        guardian[j++] = filepath[i];
        }
    
    guardian[j] = '\0';
		
return;
}
