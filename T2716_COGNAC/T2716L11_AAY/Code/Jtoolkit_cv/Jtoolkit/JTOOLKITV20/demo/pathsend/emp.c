/* -------------------------------------------------------------------------
*
* You are granted a limited copyright to modify and use this sample
* code for your internal purposes only. THIS SOFTWARE IS PROVIDED "AS-
* IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. IN NO EVENT SHALL THE
* HEWLETT-PACKARD COMPANY OR ITS SUBSIDIARIES BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE INCURRED BY YOU OR ANY THIRD PARTY IN CONNECTION WITH THE
* USE OF THIS SOFTWARE.
* 
* -------------------------------------------------------------------------   
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <cextdecs.h>
#include <tal.h>
#include "emp.h"

short fnum;
char filename[37];
/* Prototypes for internal functions */
void add_emp (char * reqbuf, char ** repbuf, short * reply_count);
void get_emp (char * reqbuf, char ** repbuf, short * reply_count);
void del_emp (char * reqbuf, char ** repbuf, short * reply_count);
short open_file(void);
short keyposition(char *keyval, short keyspec, short complen, short postype);
short writeFile(char *buf, short writecount);
short writeUpdateFile(char *buf, short writecount);
short read(char *buf, short readcount, short *countread);
void closeFile(void);
/***********************************************************************/
int main (int argc, char *argv[])
{
	short open_count = 0;
	short error;
        short gerror;
	short last_error;
	short recvfile;
	short receiveinfo[17];
        short diaginfo;
        short diag;
	short count_transferred;
	short reply_count;
	char request_code[3];
	char reqbuf[8096];    /* request message buffer */
	char * repbuf;        /* pointer to reply buffer */
        fnum = -1;
	/*  Open $receive */
	error   = FILE_OPEN_ ("$RECEIVE"
       	    ,(short) strlen("$RECEIVE")
            ,&recvfile
            , /* access */
            , /* exclusion */
            , /* nowait depth */
            ,1);    /* receive depth */
	if (recvfile == -1) {
    	    fprintf (stderr, "Error opening $RECEIVE: %hd\n", error);
    	    exit(1);
    	}              
	/* Process requests */
	for (;;) {
             READUPDATEX (recvfile,
                  (char *) reqbuf,
                  (short) sizeof (reqbuf),
                  &count_transferred);
	    error = FILE_GETINFO_(recvfile, &last_error);
	    FILE_GETRECEIVEINFO_ ((short *) &receiveinfo,&diaginfo);
            diag  = (short) (12 & diaginfo);
	    if (last_error == 6)  {            /* system message */
	        short sys_msg;
	        memcpy ((char *)&sys_msg, reqbuf, 2);
		switch (sys_msg) {
	            case -103:
	           	open_count++;
		        break;
		    case -104:
	 	        open_count--;
		        break;            
		}         
		REPLYX (reqbuf, receiveinfo[1]);
	        if (sys_msg == -104 && open_count == 0)   
		    /* last close message */
		    break;    
		    /* out of the for loop reading $receive */
	        else
	            continue;
                    /* ignore other system messages */
	        }
	        if (last_error != 0) {
	            fprintf (stderr, 
	               	"Error %hd returned from READUPDATEX of $RECEIVE<br>\n",
		           last_error);
		    exit (1);
	        }
		strncpy(request_code, reqbuf, 2);
		if(strncmp(request_code, "01", 2) == 0) {
		    get_emp(reqbuf, &repbuf, &reply_count);
		} else if(strncmp(request_code, "02",2) == 0) {
		    add_emp(reqbuf, &repbuf, &reply_count);
		} else if(strncmp(request_code, "03",2) == 0){
		    del_emp(reqbuf, &repbuf, &reply_count);
		} else {
		    fprintf(stderr,
			"Invalid request received from client\n");
		    exit(1);
		}
     
                switch(diag)
                {
                    case 0:  /* context free call */
                             gerror=0;
                             break;
                    case 4:  /* new dialog */
                             gerror=70;  /* making the dialog to continue */
                             break;
                    case 8:  /* existing dialog */
                             gerror=0;   /* making the dialog to end */
                             break;
                    default:   
                             strcpy (repbuf,"ERROR"); 
                             reply_count = (short)strlen(repbuf);
                             gerror =0; 
                             break; 
                }
    	    	/* reply to the requester */
    		REPLYX ((char *) repbuf, reply_count,,,gerror);
	        error = FILE_GETINFO_(recvfile, &last_error);
		if (last_error != 0) {
		    fprintf (stderr, "REPLYX returned error %hd<br>\n", last_error);
		    exit (1);
	        }
	}    
	error = FILE_CLOSE_ (recvfile);
	closeFile();

	if (error != 0) 
	    fprintf (stderr, "Error %hd on FILE_CLOSE_ of $RECEIVE<br>\n", error);
	return 0;
	
}
/***********************************************************************/
void add_emp (char * reqbuf, char ** repbuf, short * reply_count)
{
	employee_add_def *req = (employee_add_def *) reqbuf;
	employee_reply_def rep;
	employee_def_def emp_rec;
	error_reply_def err;
	char error_text[60];
	short fileErr;
	
	if(fnum == -1) {
	  fileErr = open_file();
	  if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to open employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	  }
	}
		
	memset(&rep,' ', sizeof(rep));
	
	
	memset(&emp_rec,' ', sizeof(emp_rec));
	memmove(&emp_rec.employee_number, req->employee_info.employee_number, sizeof(emp_rec.employee_number));
	memmove(&emp_rec.empname.first,req->employee_info.empname.first,
		sizeof(emp_rec.empname.first));
	emp_rec.empname.middle = req->employee_info.empname.middle;
	memmove(&emp_rec.empname.last, req->employee_info.empname.last,
		sizeof(emp_rec.empname.last));
	memmove(&emp_rec.regnum, req->employee_info.regnum, sizeof(emp_rec.regnum));
	memmove(&emp_rec.branchnum, req->employee_info.branchnum,
	  sizeof(emp_rec.branchnum));

	fileErr = writeFile((char *)&emp_rec, (sizeof(emp_rec) - 1));
	if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to write to employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	}
	memset(&rep,' ', sizeof(rep));
	memmove(&rep.reply_code,"02",2);	
	memmove(&rep.employee_info.employee_number, emp_rec.employee_number, sizeof(rep.employee_info.employee_number));
	memmove(&rep.employee_info.empname.first,req->employee_info.empname.first,
		sizeof(rep.employee_info.empname.first));
	rep.employee_info.empname.middle = req->employee_info.empname.middle;
	memmove(&rep.employee_info.empname.last, req->employee_info.empname.last,
		sizeof(rep.employee_info.empname.last));
	memmove(&rep.employee_info.regnum, req->employee_info.regnum,
		 sizeof(rep.employee_info.regnum));
	memmove(&rep.employee_info.branchnum, req->employee_info.branchnum,
	  sizeof(rep.employee_info.branchnum));
	
	*repbuf = (char *) &rep;
	*reply_count = (short) sizeof(rep);
	return;
}
/***********************************************************************/
void get_emp (char * reqbuf, char ** repbuf, short * reply_count)
{
	employee_request_def *req = (employee_request_def *) reqbuf;
	employee_reply_def rep;
	employee_def_def emp_rec;
	error_reply_def err;
	short countRead;
	char error_text[60];
	short fileErr;
	
	if(fnum == -1) {
	  fileErr = open_file();
	  if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to open employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	  }
	}

	memset(&emp_rec,' ', sizeof(emp_rec));
	memmove(&emp_rec.employee_number, req->employee_number, sizeof(emp_rec.employee_number));
	 fileErr = keyposition(emp_rec.employee_number,0,0,0);
        if(fileErr != 0) {
            memset(&err,' ', sizeof(err));
            memmove(err.reply_code,"00",sizeof(err.reply_code));
            sprintf(error_text,"Unable to keyposition in employ file; error : %d", fileErr);
            memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
            return;
        }

		
	fileErr = read((char *)&emp_rec, sizeof(emp_rec), &countRead);
	if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to read from employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	}
	memset(&rep,' ', sizeof(rep));
	memmove(rep.reply_code,"01",sizeof(rep.reply_code));	
	memmove(&rep.employee_info.employee_number, emp_rec.employee_number, sizeof(rep.employee_info.employee_number));
	memmove(&rep.employee_info.empname.first,emp_rec.empname.first,
		sizeof(rep.employee_info.empname.first));
	rep.employee_info.empname.middle = emp_rec.empname.middle;
	memmove(&rep.employee_info.empname.last, emp_rec.empname.last,
		sizeof(rep.employee_info.empname.last));
	memmove(&rep.employee_info.regnum, emp_rec.regnum, sizeof(rep.employee_info.regnum));
	memmove(&rep.employee_info.branchnum, emp_rec.branchnum,
	  sizeof(rep.employee_info.branchnum));
	
	*repbuf = (char *) &rep;
	*reply_count = (short) sizeof(rep);
	return;
}
/***********************************************************************/
void del_emp (char * reqbuf, char ** repbuf, short * reply_count)
{
	employee_request_def *emp = (employee_request_def *) reqbuf;
	employee_def_def emp_rec;
	error_reply_def err;
	short countRead;
	char error_text[60];
	char sEmpNum[5];
	short fileErr;
	
	if(fnum == -1) {
	  fileErr = open_file();
	  if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to open employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	  }
	}

	memset(&emp_rec,' ', sizeof(emp_rec));
	memmove(&emp_rec.employee_number, emp->employee_number, sizeof(emp_rec.employee_number));
	
	fileErr = keyposition(emp_rec.employee_number,0,0,0);
	if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to keyposition in employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	}
	fileErr = read((char *)&emp_rec, sizeof(emp_rec), &countRead);
	if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to read employ file prior to delete; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	}
	
	fileErr = writeUpdateFile((char *)&emp_rec, 0);
	if(fileErr != 0) {
	    memset(&err,' ', sizeof(err));
	    memmove(err.reply_code,"00",sizeof(err.reply_code));
	    sprintf(error_text,"Unable to delete from employ file; error : %d", fileErr);
	    memmove(err.error_text,error_text,strlen(error_text));
            *repbuf = (char *) &err;
            *reply_count = (short) sizeof(err);
	    return;
	}

	memset(&err,' ', sizeof(err));
	memmove(err.reply_code,"03",sizeof(err.reply_code));
	memset(&error_text,' ', sizeof(error_text));
	memmove(sEmpNum,emp->employee_number,sizeof(emp->employee_number));
        sEmpNum[4] = '\0';
	sprintf(error_text,"Deleted employee %s", sEmpNum);
	memmove(err.error_text,error_text,strlen(error_text));
        *repbuf = (char *) &err;
        *reply_count = (short) sizeof(err);
	return;
	
}

/********************************************************************************/
short open_file(void)
{
   char fname[37];
   short flen;
   short err;
	
   memset(fname,' ', sizeof(fname));
   memmove(fname,"employ", 6);
   err = FILENAME_SCAN_((char *)fname, (short) strlen(fname), &flen);

   if(err != 0) 
   	return(err);

   err = FILENAME_RESOLVE_((char *)fname, flen, filename, 36, &flen, 1);

   if(err != 0)
   	return(err);

   filename[flen] = 0;
 
   err = FILE_OPEN_(filename, (short) strlen(filename), &fnum);

   return(err);

} /*  openFile */
/****************************************************************************/
short keyposition(char *keyval, short keyspec, short complen, short postype)
{
   _cc_status cc;

   short fileError = 0;

   cc = KEYPOSITIONX(fnum, keyval, keyspec, ,);
   if(!_status_eq(cc))
   {
     FILEINFO(fnum, &fileError);
   }
   return fileError;

} /* keyposition */
/*****************************************************************************/
short writeUpdateFile(char *buf, short writecount)
{
    _cc_status cc;
    short fileError = 0;
    
    cc = LOCKREC(fnum);
    if(!_status_eq(cc)) {
	FILEINFO(fnum, &fileError);
	return(fileError);
    }
    cc = WRITEUPDATEX(fnum, buf, writecount);
    if(!_status_eq(cc))
    {
	FILEINFO(fnum, &fileError);
	return(fileError);
    }
    cc = UNLOCKFILE(fnum);
    if(!_status_eq(cc))
    {
	FILEINFO(fnum, &fileError);
    }
    return(fileError);
} /* writeUpdateFile */

/****************************************************************************/
short writeFile(char *buf, short writecount)
{
   _cc_status cc;
   short fileError = 0;
   cc = WRITEX(fnum, buf, writecount);
   if(!_status_eq(cc))
   {
     FILEINFO(fnum, &fileError);
   }
   return fileError;

} /*  write */
/*****************************************************************************/
void closeFile(void)
{
   if(fnum > -1)
   {
   	FILE_CLOSE_(fnum);
   }
   
   fnum = -1;
   filename[0] = '0';
} /* closeFile */
/******************************************************************************/
short read(char *buf, short readcount, short *countread)
{
   _cc_status cc;
   short fileError = 0;

   cc = READX(fnum, buf, readcount, countread);
   if(!_status_eq(cc))
   {
   	FILEINFO(fnum, &fileError);
   }
   return fileError;

} /*  read  */

