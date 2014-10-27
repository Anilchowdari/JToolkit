#ifndef UNIGUARD_H
#define UNIGUARD_H


/**************************************************************************/

void filepathToGuardian (char *guardian, char * filepath);
#define MAXGUARDFILE 40
/******Changed declaration of sock to int 16/7/2000******/
int cgilib_tcpwait (int sock, long * tag, long timelimit);
/*******************************************************/
#endif
