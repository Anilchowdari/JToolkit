//--------------------------------------------------------------------------
//   Copyright 2002 Compaq Computer Corporation
//            Protected as an unpublished work.
//
//The computer program listings, specifications and documentation contained
//herein are the property of Compaq Computer Corporation or its supplier
//and may not be reproduced, copied, disclosed, or used in whole or part for
//any reason without the prior express written permission of Compaq Computer
//Corporation.
//--------------------------------------------------------------------------
//
//
// Filename:  keysfile.h
//--------------------------------------------------------------------------
#ifndef __KEYSFILE__
#define __KEYSFILE__

#define READ_ONLY 1

#define KEYPOS_APPROX 0
#define KEYPOS_GENERIC 1
#define KEYPOS_EXACT 2
#define KEYPOS_SKIP_THIS_REC 8000H

#define FILETYPE_KEY 3
#define FILETYPE_AUDIT 2

#define FILEERROR_EOF 1
#define FILEERROR_DUP_RECORD 10
#define FILEERROR_REC_NOT_FOUND 11
#define FILEERROR_FILE_NOT_FOUND 11

class KeySequencedFile
{
public:
	KeySequencedFile();
	~KeySequencedFile();

   void closeFile();
   short deleterec();
   short getfileError() { return fileError; }
   char * getFileName() { return filename; }
   short keyposition(char *keyval, short keyspec, short complen, short postype);
   short openFile(const char * fname);
   short openFileReadOnly(const char * fname);
   short read(char *buf, short readcount, short *countread);
   short readlock(char *buf, short readcount, short *countread);
   short readupdate(char *buf, short readcount, short *countread);
   short readupdatelock(char *buf, short readcount, short *countread);
   void	unlockrec();
   short write(char *buf, short writecount);
   short writeupdateunlock(char *buf, short writecount);

protected:

	char   	filename[HOST_FILE_SIZE+1];
   short		fnum;
   short		fileError;
};


#endif // __KEYSFILE__
