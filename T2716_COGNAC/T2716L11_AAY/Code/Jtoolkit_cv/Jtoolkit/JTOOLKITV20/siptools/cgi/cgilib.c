#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "wn.h"
#include "version.h"
#include "cgi.h"

#define _CGI_NO_EXTERNALS
#include "cgilib.h"

char	*get_input();
int wn_getc();
static char * strdup (char *src) ;

extern char argvzero[SMALLLEN];
extern char ** environ;

int cgi_errno;
static int captureDone = 0;
static int noOutput = 1;
static long contentLength=0;
static long bytesRemaining=0;
static char ** cgivars = NULL;
static char *printBuf = NULL;
#define MAX_CHARS_IN_ONE_CGI_PRINTF 64000

#define ENV_UNUSED_ENTRIES 512
static char ** origEnviron ;
static int unusedEnvEntries = ENV_UNUSED_ENTRIES;

/***************************************************************************/

void debugEnviron(char * tag) 
{
int i;
char buf[128];

if (!debug_log)
    return;

sprintf (buf,"environ=%lu; origEnviron=%lu; unusedEnvEntries=%d",
             (unsigned long) environ,
             (unsigned long) origEnviron,
             unusedEnvEntries);
write_debug (1, tag, buf);


write_debug (1, tag, "ENVIRON............................................");
if ( environ)
    for (i=0; environ[i]; i++) {
        write_debug (1, tag, environ[i]);
        }

write_debug (1, tag, "ORIGENVIRON............................................");
if ( origEnviron)
    for (i=0; origEnviron[i]; i++) {
        write_debug (1, tag, origEnviron[i]);
        }

return;
}





/***************************************************************************/

int setupCgiEnv (void) {
    int i;
        
    debugEnviron("setupCgiEnv entry") ;

	origEnviron = environ;

    /* create new environment */
	for (i=0;origEnviron[i];i++) 
		;
	unusedEnvEntries = ENV_UNUSED_ENTRIES;
    environ = malloc (sizeof(char*)*(i+2+unusedEnvEntries));
	environ[0] = NULL;
        
    /* duplicate inherited environment */
	for (i=0;origEnviron[i];i++) 
		 environ[i] = strdup (origEnviron[i]);
	environ[i] = NULL;

    debugEnviron("setupCgiEnv exit") ;

    return 1;
    }
    
    
/***************************************************************************/

int cleanupCgiEnv (void) {
    int i;

    debugEnviron("cleanupCgiEnv entry") ;

	for (i=0; environ[i]; i++) {
		free (environ[i]);
		environ[i]=NULL;
		}
    free (environ);

    environ = origEnviron;
	
    debugEnviron("cleanupCgiEnv exit") ;

    return 1;
    }


/***************************************************************************/

int cgi_putenv (char *var) {
    int i,j; 
	int match = -1;

    match = -1;
    for (i=0; environ[i] && match == -1; i++) {
        char * envp = environ[i];
        for (j=0;;j++) {
           char e = envp[j];
           char v = var[j];
           if (e=='\0' || v=='\0') 
               break;
           if (e=='=' && v=='=') {
               match=i;
               break;        
               }
           if (e != v)
               break;
           }
        }

    if (match != -1) {
       free (environ[match]);
       environ[match] = var;
       }
    else {
       environ[i] = var;
       environ[i+1] = NULL;
       unusedEnvEntries--;
       if (unusedEnvEntries < 1) {
           unusedEnvEntries = ENV_UNUSED_ENTRIES;
           environ = realloc (environ,
                     (sizeof(char*)*(i+2+unusedEnvEntries)));
           }
       } 

    return 1;
    }

/***************************************************************************/

static void cgi_error (char * msg)
	{
	CGI_puts ("</head></table></form><p><h3>CGI error: ");
	CGI_puts (msg);
	CGI_puts ("</h3></body></html>");
	}

/*************************************************************************/
/**                                                                     **/
/**     getcgivars.c-- routine to read CGI input variables into an      **/
/**         array of strings.                                           **/
/**                                                                     **/
/**     The x2c() and unescape_url() routines were lifted directly      **/
/**     from NCSA's sample program util.c, packaged with their HTTPD.   **/
/**                                                                     **/
/*************************************************************************/

static char * strdup (char *src) 
	{
	size_t size;
    char * dest;
	size = strlen (src);
	dest=(char *) malloc(size+2); 
	strcpy(dest,src);
	return dest;
	}

/***************************************************************************/

/** Convert a two-char hex string into the char it represents **/
char x2c(char *what) {
   register char digit;
/*** Type casted to avoid warning- 26/7/2000***********/
   digit = (char) (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
/******************************************************/   
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
   return(digit);
}

/** Reduce any %xx escape sequences to the characters they represent **/
void unescape_url(char *url) {
    register int i,j;

    for(i=0,j=0; url[j]; ++i,++j) {
        if((url[i] = url[j]) == '%') {
            url[i] = x2c(&url[j+1]) ;
            j+= 2 ;
        }
    }
    url[i] = '\0' ;
}


/** Read the CGI input and place all name/val pairs into list.        **/
/** Returns list containing name1, value1, name2, value2, ... , NULL  **/
int util_getcgivars(void) {
    int i ;
    char *request_method ;
    int content_length;
    char *cgiinput ;
    char **pairlist ;
    int paircount ;
    char *nvpair ;
    char *eqpos ;
	char * qstring;

    cgiinput = NULL;

    /** Depending on the request method, read all CGI input into cgiinput **/
    /** (really should produce HTML error messages, instead of exit()ing) **/
    request_method= getenv("REQUEST_METHOD") ;
    if (!strcmp(request_method, "GET")) {
        qstring= (getenv("QUERY_STRING")) ;
		if (qstring == NULL)
			return 2;      /* no variables */
		if (*qstring == '\0')
			return 2;      /* no variables */
		cgiinput = strdup (qstring);
        }
    else if (!strcmp(request_method, "POST")) {
        if ( strcasecmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
            cgi_error ("getcgivars(): Unsupported Content-Type.\n") ;
            return 0;
        }
        if ( !(content_length = atoi(getenv("CONTENT_LENGTH"))) ) {
            cgi_error ("getcgivars(): No Content-Length was sent with the POST request.\n") ;
            return 0 ;
        }
		if (content_length <= 1)
			return 2;      /* no variables */
        if ( !(cgiinput= (char *) malloc(content_length+1)) ) {
            cgi_error ("getcgivars(): Could not malloc for cgiinput.\n") ;
            return 0 ;
        }
        if (!CGI_fread(cgiinput, content_length, 1, stdin)) {
            cgi_error ("Couldn't read CGI input from STDIN.\n") ;
            return 0 ;
        }
        cgiinput[content_length]='\0' ;
        if (cgiinput[content_length-1] == '\n' ||
            cgiinput[content_length-1] == '\r')
            cgiinput[content_length-1] = '\0';
        if (cgiinput[content_length-2] == '\n' ||
            cgiinput[content_length-2] == '\r')
            cgiinput[content_length-2] = '\0';
    }
    else {
        cgi_error ("getcgivars(): unsupported REQUEST_METHOD\n") ;
        return 0 ;
    }

    /** Change all plusses back to spaces **/
    for(i=0; cgiinput[i]; i++) if(cgiinput[i] == '+') cgiinput[i] = ' ' ;

    /** First, split on "&" to extract the name-value pairs into pairlist **/
    pairlist= (char **) malloc(256*sizeof(char **)) ;
    paircount= 0 ;
    nvpair= strtok(cgiinput, "&") ;
    while (nvpair) {
        pairlist[paircount++]= strdup(nvpair) ;
        if (!(paircount%256))
            pairlist= (char **) realloc(pairlist,(paircount+256)*sizeof(char **)) ;
        nvpair= strtok(NULL, "&") ;
    }
    pairlist[paircount]= 0 ;    /* terminate the list with NULL */

    /** Free old pairs (if any)  **/
    if (cgivars != NULL) {
        for (i=0;cgivars[i];i++)
            free (cgivars[i]);
        free (cgivars);
        }

    /** Then, from the list of pairs, extract the names and values **/
    cgivars= (char **) malloc((paircount*2+1)*sizeof(char **)) ;
    for (i= 0; i<paircount; i++) {
        if (eqpos=strchr(pairlist[i], '=')) {
            *eqpos= '\0' ;
            unescape_url(cgivars[i*2+1]= strdup(eqpos+1)) ;
        } else {
            unescape_url(cgivars[i*2+1]= strdup("")) ;
        }
        unescape_url(cgivars[i*2]= strdup(pairlist[i])) ;
    }
    cgivars[paircount*2]= 0 ;   /* terminate the list with NULL */
    
    /** Free anything that needs to be freed **/
    free(cgiinput);

    for (i=0;;i++)
		if (pairlist[i] != 0)
	        free(pairlist[i]);
		else
			break;

    free(pairlist) ;

    /** Return success **/
    return 1 ;
    
}

/***************** end of the getcgivars() module ********************/

char *util_getvar(char * varname)
{
int i;

for (i=0; cgivars[i]; i=i+2)
     if (strcmp(cgivars[i], varname) == 0)
         return cgivars[i+1];
return NULL;
}

/***********************************************************************/

void doHttpHeaders (const char * buf)
    {

    if (noOutput) {
       char temp[16];
       memcpy (temp, buf, 5);
       temp[5]='\0';
       /****Changed code to send HTTP headers-20/7/2000***/
	/* if (strcasecmp (temp, "HTTP/") != 0) 
           CGI_puts ("HTTP/1.0 200 \r\n"); */
      /***************************************************/
       noOutput = 0;
       http_prolog();
       }
    return;
    }


/************************************************************************/

void CGI_internal_term(void)
	{
    if (pathway_server)
        cleanupCgiEnv ();

	return;
    }
   
/************************************************************************/

void CGI_internal_init( Request   *ip)
	{
   char * cl;

    captureDone = 0;
    noOutput = 1;

   if (printBuf == NULL)
       printBuf = malloc (MAX_CHARS_IN_ONE_CGI_PRINTF);

    if (pathway_server)
        setupCgiEnv ();

    if (ip)
       cgi_env( ip, FALSE);  /* Full CGI environment */

   cl = getenv("CONTENT_LENGTH");
	contentLength = 0;
   if (cl)
   	sscanf (cl, "%ld", &contentLength);
   bytesRemaining = contentLength;

   if (auto_form_decoding)
	   CGI_Capture();

   return;
   }


/***********************************************************************/

void CGI_Capture(void)
	{
	char *var;
	size_t len;
	int i, j;
	int ret;
    int varCount = 0;
    if (captureDone)
        return;

    debugEnviron("CGI_Capture entry") ;

    captureDone = 1;

	ret = util_getcgivars();

	if (ret == 1) {
        /*  djc: untested code to workaround a putenv bug .......
          tpr t970217 0918 11066  */
        /* bubble sort in ascending order */
        int change = 1;
        while (change) {
            change = 0;
            for (i=0; cgivars[i] && cgivars[i+2]; i=i+2) {
                if (strcmp(cgivars[i], cgivars[i+2]) > 0) {
                    char * tmp;
                    tmp = cgivars[i];
                    cgivars[i]=cgivars[i+2];
                    cgivars[i+2] = tmp;
                    tmp = cgivars[i+1];
                    cgivars[i+1]=cgivars[i+3];
                    cgivars[i+3] = tmp;
                    change = 1;
                    }
                 }
             }
        /* .......................................   */

		for (i=0; cgivars[i]; i=i+2) {
			len = strlen(cgivars[i]) + strlen (cgivars[i+1]) + 8;
			var = malloc (len);
			sprintf (var,"%s=%s",cgivars[i],cgivars[i+1]);
            if (pathway_server) {
                cgi_putenv (var); 
                }
            else {
                putenv(var);
			    free (var);
                }
			} 
		}

    debugEnviron("CGI_Capture exit") ;

	return;
	}

/***********************************************************************/


int CGI_puts (const char * string)
	{
    cgi_errno = 0;
    
    if (argvzero[0])
        return fputs (string, stdout);

    doHttpHeaders (string);

	send_text_line ((char *) string); 

    if (errno == 0)
	/***Type casted to avoid warning-26/7/2000****/    
	return (int)strlen(string);
	/********************************************/
    else {
        errno = cgi_errno;
        return EOF;
        } 
   }

/*********************************************************************/

int CGI_getc(FILE * stream)
   {
   int c;
   cgi_errno = 0;
   
   if (argvzero[0])
       return getc (stream);

   c = wn_getc();
   if (c == EOF)
      errno = cgi_errno;
      
   return c;
   }

/***********************************************************************/

size_t CGI_fread (void *buf, size_t size,size_t num_items,FILE *stream)
	{
   size_t i;
   size_t countRead;
   char * b;

   if (argvzero[0])
        return fread (buf, size,num_items,stream);

   b = (char *) buf;
   if (num_items <= 0)
   	return 0;
      
   countRead = 0;
   for (i=0;i<(size*num_items);i++) {
       int c;
       if (bytesRemaining == 0)
			  break;
       c = wn_getc();
       if (c == EOF)
           break;
       b[i] = (unsigned char) c;
       countRead++;
       bytesRemaining--;
   	}
    return countRead/size; 
   }

/************************************************************************/

int CGI_printf(const char *format, ...)
	{
	va_list marker;
	int count = 0;

	va_start (marker, format);

    /*  There must be a better way of doing this  */
	count = vsprintf (printBuf, format, marker);
	if (count >= MAX_CHARS_IN_ONE_CGI_PRINTF)
		cgi_error ("****** Too much text in one CGI_printf ******");

    if (argvzero[0])
        return fputs (printBuf, stdout);

	count = CGI_puts (printBuf);
	return count;
	}

/**************************************************************************/

int CGI_fflush(FILE * stream)
	{
    if (argvzero[0])
        return fflush (stream);

	cgi_errno = 0;
	flush_outbuf();
	if (cgi_errno != 0) {
	    return EOF;
        }
	return 0;
	}

/**************************************************************************/

void ErrorAbort (void)
	{
    if (argvzero[0])
        exit(0);

    flush_outbuf();
    exit (1);

	return;
	}

/***************************************************************************/

size_t CGI_fwrite(const void *buffer,size_t size,size_t num_items,FILE *stream)
	{
    size_t countWritten;

    if (argvzero[0])
        return fwrite(buffer,size,num_items,stream);

    if (num_items <= 0)
   	    return 0;

    doHttpHeaders (buffer);

    cgi_errno = 0;
    countWritten = size*num_items;
    /****Type casted to avoid warning-26/7/2000******/
    send_out_mem ((char *) buffer, (int)countWritten);
   /***********************************************/
    if (cgi_errno != 0) {
        errno = cgi_errno;
        return 0;
        }
        
    return countWritten/size;
	}


/***************************************************************************/

int CGI_set_fflush_timer(int seconds)
	{
	return 0;   /* not implemented */
    }

