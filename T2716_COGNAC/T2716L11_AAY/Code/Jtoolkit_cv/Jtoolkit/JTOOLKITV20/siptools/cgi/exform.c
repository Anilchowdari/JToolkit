#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/*
    This is a simple little test program that demonstrates how to
    wrte a CGI routine as a Pathway Server Class.
*/
#include "cgilib.h"
extern char **environ;


int CGI_main(int argc,char *argv[])
{
static  int get=0;
static int post=0;
int         i=0;
char        buffer[4096];
char        *equalsign=NULL;
int         Test_Count=0;
    /*

            To retrieve the value of the following filled out form entry:

                <INPUT SIZE=30 NAME="First_Name" > <b>Your First Name</b> <br>"

            Use:

                getenv("your_prefix_First_Name");

        In the first example the environment variables will be made with
        the same names as the name protion of the name value pair.

        In the second example the names of all decoded from are prefixed with the prefix "your_prefix_".
        Using the prefix option can be useful if you expect duplication of names on your form with default
        CGI parameters.

    */


    /* Always print a header */
  CGI_printf("Content-type: text/html\r\n\r\n");

    /* This is a logical case on REQUEST_METHOD */

    if (!strcmp(getenv("REQUEST_METHOD"),"GET")){
        get++;
        CGI_printf("<html><title>Template CGI Demo Form</title>\n");
        CGI_printf("<h1>CGI Forms Demo</h1>\n");
        CGI_printf("<FORM METHOD=\"POST\" ACTION=\"%s\">\n",getenv("SCRIPT_NAME"));
        CGI_printf("This form and its associated application demonstrate decoding of form encoded data.<BR>\n");
        CGI_printf("<INPUT SIZE=30 NAME=\"First_Name\" > <b>Your First Name</b> <br>\n");
        CGI_printf("<INPUT SIZE=30 NAME=\"Last_Name\" > <b>Your Last Name</b> <br>\n");
        CGI_printf("The following entry will control the number of test lines that are printed in the response.<BR>\n");
        CGI_printf("<INPUT SIZE=6 NAME=\"Test_Count\" > <b>Test Line count</b> <br>\n");
        CGI_printf("<INPUT TYPE=\"submit\" VALUE=\"Send Message\"></form><br></html>\n\n");
    /* CASE=POST */
    } else if (!strcmp(getenv("REQUEST_METHOD"),"POST")){
        post++;
        CGI_printf("<title>CGI Demo Form</title>\n");
        CGI_printf("<h1>CGI Form Response</h1>\n");

        CGI_printf("Get count: %d<BR>Post count: %d<BR>\n",get,post);
        CGI_printf("<H2>Environment Variables</H2>\n");
        /* This loop reads through the environment variables and displays them */
        for (i=0;environ[i];i++) {
            strcpy(buffer,environ[i]);
            equalsign=strchr(buffer,'=');
            *equalsign=0;
            equalsign+=1;
            CGI_printf("<b>%s</b>  %s<BR>\n",buffer,equalsign);
        }
        if (getenv("Test_Count")!=NULL)
                Test_Count=atoi(getenv("Test_Count"));

        if (Test_Count){
            CGI_printf("<h2>Printing %d test lines.</h2>",Test_Count);
            for (i=1;i<=Test_Count;i++){
                CGI_printf("Test Line %d ....|...10....|...20....|...30....|...40....|...50<BR>",i);
            }
        }
    /* CASE=DEFAULT FALL THROUGH */
    } else {
        CGI_printf("Unrecognized method '%s'.\n", getenv("REQUEST_METHOD"));
    }
    return 0;
}


