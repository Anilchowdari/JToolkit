
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
// Filename:  keysfile.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "keysfile.h"

#define MODULE_ID "SOFI"

//--------------------------------------------------------------------------
// KeySequencedFile
//--------------------------------------------------------------------------
KeySequencedFile::KeySequencedFile()
{
	fnum = -1;
   filename[0] = 0;
} // KeySequencedFile
//--------------------------------------------------------------------------
// KeySequencedFile
//--------------------------------------------------------------------------
KeySequencedFile::~KeySequencedFile()
{
	closeFile();
} // KeySequencedFile  destructor

//--------------------------------------------------------------------------
// closeFile
//--------------------------------------------------------------------------
void
KeySequencedFile::closeFile()
{
	if(fnum > -1)
   {
#ifdef __TANDEM
	FILE_CLOSE_(fnum);
#else
   	FILE_CLOSE__(fnum, (short )0, /* octal mask: */ 020000000000);
#endif
   }

   filename[0] = 0;
   fnum = -1;
} // closeFile

//--------------------------------------------------------------------------
// keyposition
//--------------------------------------------------------------------------
short
KeySequencedFile::keyposition(char *keyval, short keyspec, short complen, short postype)
{
   _cc_status
   	cc;

	fileError = 0;
#ifdef __TANDEM
   cc = KEYPOSITIONX(fnum, keyval, keyspec, complen, postype);
   if(!_status_eq(cc))
   {
      FILEINFO(fnum, &fileError);
#else
   cc = KEYPOSITIONX_(fnum, keyval, keyspec, complen, postype, /* octal mask: */ 037000000000);
   if(!_status_eq(cc))
   {
      FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   	printf("ddl2java - Error: Keyposition error %d for file %s.\n",
                         fileError, filename);
   }
   return fileError;

} // keyposition

//--------------------------------------------------------------------------
// openFile
//--------------------------------------------------------------------------
short
KeySequencedFile::openFile(const char * fname)
{

   closeFile();
   fixFileName(fname, HOST_FILE_SIZE, filename);

#ifdef __TANDEM
   fileError = FILE_OPEN_(filename, strlen(filename), &fnum);
#else
   fileError = FILE_OPEN__(filename, strlen(filename), &fnum, (short )0, (short )0, (short )0, (short )0, (short )0, (short )0, (short )0, (short *)0, /* octal mask: */ 034000000000);
#endif
   if(fileError != 0)
   {
		printf("ddl2java - Error: Could not open %s: error %d.\n", filename, fileError);
   }
	return fileError;

} //  openFile

//--------------------------------------------------------------------------
// openFileReadOnly
//--------------------------------------------------------------------------
short
KeySequencedFile::openFileReadOnly(const char * fname)
{

   closeFile();
   fixFileName(fname, HOST_FILE_SIZE, filename);

#ifdef __TANDEM
   fileError = FILE_OPEN_(filename, strlen(filename), &fnum, READ_ONLY);
#else
   fileError = FILE_OPEN__(filename, strlen(filename), &fnum, READ_ONLY, (short )0, (short )0, (short )0, (short )0, (short )0, (short )0, (short *)0, /* octal mask: */ 036000000000);
#endif
   if(fileError != 0)
   {
		printf("ddl2java - Error: Could not open %s: error %d.\n", filename, fileError);
   }
	return fileError;

} //  openFileReadOnly

//--------------------------------------------------------------------------
// read
//--------------------------------------------------------------------------
short
KeySequencedFile::read(char *buf, short readcount, short *countread)
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = READX(fnum, buf, readcount, countread);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = READX_(fnum, buf, readcount, countread, (long )0, /* octal mask: */ 036000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   }
	return fileError;

} //  read

//--------------------------------------------------------------------------
// readlock
//--------------------------------------------------------------------------
short
KeySequencedFile::readlock(char *buf, short readcount, short *countread)
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = READLOCKX(fnum, buf, readcount, countread);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = READLOCKX_(fnum, buf, readcount, countread, (long )0, /* octal mask: */ 036000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   }
	return fileError;

} //  readlock

//--------------------------------------------------------------------------
// readupdatelock
//--------------------------------------------------------------------------
short
KeySequencedFile::readupdatelock(char *buf, short readcount, short *countread)
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = READUPDATELOCKX(fnum, buf, readcount, countread);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = READUPDATELOCKX_(fnum, buf, readcount, countread, (long )0, /* octal mask: */ 036000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   }
	return fileError;

} //  readupdatelock

//--------------------------------------------------------------------------
// write
//--------------------------------------------------------------------------
short
KeySequencedFile::write(char *buf, short writecount)
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = WRITEX(fnum, buf, writecount);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = WRITEX_(fnum, buf, writecount, (short *)0, (long )0, /* octal mask: */ 034000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   }
	return fileError;

} //  write

//--------------------------------------------------------------------------
// writeupdateunlock
//--------------------------------------------------------------------------
short
KeySequencedFile::writeupdateunlock(char *buf, short writecount)
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = WRITEUPDATEUNLOCKX(fnum, buf, writecount);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = WRITEUPDATEUNLOCKX_(fnum, buf, writecount, (short *)0, (long )0, /* octal mask: */ 034000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif
   }
	return fileError;

} //  writeupdateunlock

//--------------------------------------------------------------------------
// unlockrec
//--------------------------------------------------------------------------
void
KeySequencedFile::unlockrec()
{
   _cc_status
   	cc;
   fileError = 0;
#ifdef __TANDEM
   cc = UNLOCKREC(fnum);
   if(!_status_eq(cc))
   {
        FILEINFO(fnum, &fileError);
#else
   cc = UNLOCKREC_(fnum, (long )0, /* octal mask: */ 020000000000);
   if(!_status_eq(cc))
   {
   	FILEINFO_(fnum, &fileError, (short *)0, (short *)0, (short *)0, (short *)0, (long *)0, (long *)0, (short *)0, (short *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (char *)0, (short *)0, (long *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, (short *)0, /* octal mask: */ 030000000000);
#endif

   }

} //  unlockrec


// end keysfile.cpp
