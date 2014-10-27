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
// Filename:  utils.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "ddl.h"
#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

//--------------------------------------------------------------------------
#ifdef __TANDEM
 void convert3WordTS(long long *ts64, unsigned short *ts3Word )
#else
 void convert3WordTS(__int64 *ts64, unsigned short *ts3Word )
#endif
{
	union
   {
#ifdef __TANDEM
   	long long i64;
#else
		__int64 i64;
#endif
      unsigned short i16[4];
   } ts;

   ts.i64 = 0L;
   ts.i16[1] = ts3Word[0];
   ts.i16[2] = ts3Word[1];
   ts.i16[3] = ts3Word[2];
   *ts64 = ts.i64;
} // convert3WordTS

//--------------------------------------------------------------------------
void fileNameWithNull(char *fileNameWithBlanks, char* fileName)
{
   char
      *ptr;
   memcpy(fileName, fileNameWithBlanks, HOST_FILE_SIZE);
   fileName[HOST_FILE_SIZE] = 0;
   ptr = strchr(fileName, ' ');
   if(ptr != NULL)
   	*ptr = 0;

} // fileNameWithNull

//--------------------------------------------------------------------------
void fixDDLName(char *fileName, int fNameLen)
{
	int
   	i;
   for(i=0;i<fNameLen;i++)
   {
      if(fileName[i] == '-')
      	fileName[i] = '_';
      else
   		fileName[i] = tolower(fileName[i]);
   }

} // fixDDLName


//--------------------------------------------------------------------------
bool fixFileName(const char *fileName, int maxNewFileNameLen, char *newFileName)
{
   bool
   	result = true;
	short
   	flen;
#ifdef __TANDEM
   if(FILENAME_SCAN_((char *)fileName, strlen(fileName), &flen) != 0)
        return false;
   if(FILENAME_RESOLVE_((char *)fileName, flen, newFileName, maxNewFileNameLen,
                        &flen, 1) != 0)
#else
   if(FILENAME_SCAN__((char *)fileName, strlen(fileName), &flen, (short *)0, (short *)0, (short )0, /* octal mask: */ 034000000000) != 0)
   	return false;
   if(FILENAME_RESOLVE__((char *)fileName, flen,
   							newFileName, maxNewFileNameLen,
                        &flen,
                        1, (char *)0, (short )0, (char *)0, (short )0, (char *)0, (short )0, /* octal mask: */ 037400000000) != 0)
#endif
   	result = false;
   	newFileName[flen] = 0;
   	return result;
} // fixFileName

//--------------------------------------------------------------------------
//------------------------------------------------------------------------------
// writeNumericConvertMethod
//	This routine generates a method used to convert returned data to the
//  proper format.  This routine writes a java method to the generated class
//  that inserts decimal points and zero-padds if necessary.
//------------------------------------------------------------------------------
void writeNumericConvertMethod(char *indent, int dataType, FILE *pfp)
{
	char typeStr1[6];
   	char typeStr2[6];

   	if(dataType == BINARY_16_SIGNED) {
   		strcpy(typeStr1,"Short");
      	strcpy(typeStr2,"short");
   	}
   	if((dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) ) {
   		strcpy(typeStr1,"Integer");
      	strcpy(typeStr2,"int");
   	}
   	if((dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED)) {
   		strcpy(typeStr1,"Long");
      	strcpy(typeStr2,"long");
   	}

   	fprintf(pfp,"%sprotected String convert%sToString(%s value, int scale) {\n",
   		indent, typeStr1, typeStr2);
   	fprintf(pfp,"%s  boolean _isNegative = false;\n", indent);
   	fprintf(pfp,"%s  %s _positiveValue = 0;\n", indent, typeStr2);
   	fprintf(pfp,"%s  StringBuffer _sb = new StringBuffer(64);\n\n", indent);
   	fprintf(pfp,"%s	if(scale < 0) {\n", indent);
	fprintf(pfp,"%s 	  _sb.append(value);\n", indent);
   	fprintf(pfp,"%s     for(int _i = 0; _i < Math.abs(scale); _i++) { \n", indent);
   	fprintf(pfp,"%s       _sb.append(0);\n",indent);
   	fprintf(pfp,"%s     }\n",indent);
   	fprintf(pfp,"%s     _sb.append(\".\");\n", indent);
	fprintf(pfp,"%s     return(_sb.toString());\n", indent);
   	fprintf(pfp,"%s  }\n", indent);
   	fprintf(pfp,"%s  if(value < 0) {\n", indent);
   	fprintf(pfp,"%s    _isNegative = true;\n",indent);
   	fprintf(pfp,"%s    if(%s.toString(value).length() <= scale) {\n", indent, typeStr1);
   	fprintf(pfp,"%s	   _positiveValue = (%s) Math.abs(value); \n", indent, typeStr2);
   	fprintf(pfp,"%s  	 } else {\n",indent);
   	fprintf(pfp,"%s      _positiveValue = value;\n",indent);
   	fprintf(pfp,"%s    }\n", indent);
   	fprintf(pfp,"%s  } else {\n",indent);
   	fprintf(pfp,"%s    _positiveValue = value;\n", indent);
   	fprintf(pfp,"%s  }\n",indent);
	fprintf(pfp,"%s  if(%s.toString(_positiveValue).length() < scale) {\n",indent, typeStr1);
   	fprintf(pfp,"%s    if(_isNegative) { \n",indent);
   	fprintf(pfp,"%s   	  _sb.append(\"-.\");\n", indent);
   	fprintf(pfp,"%s    } else {\n",indent);
   	fprintf(pfp,"%s       _sb.append(\".\"); \n",indent);
   	fprintf(pfp,"%s    }\n", indent);
   	fprintf(pfp,"%s 	for(int _j = 0; _j < (scale - %s.toString(_positiveValue).length()); _j ++) {\n",
   		indent, typeStr1);
   	fprintf(pfp,"%s    _sb.append(0);\n", indent);
   	fprintf(pfp,"%s 	}\n", indent);
   	fprintf(pfp,"%s 	return( _sb.append(_positiveValue).toString());\n", indent);
   	fprintf(pfp,"%s  } else {\n",indent);
   	fprintf(pfp,"%s    String _str = %s.toString(_positiveValue);\n\n",indent, typeStr1);
	fprintf(pfp,"%s    String _tmp = _str.substring(0, (_str.length() - scale )) + \".\" + _str.substring((_str.length() - scale), _str.length());\n",indent);
	fprintf(pfp,"%s    return(_tmp);\n",indent);
   	fprintf(pfp,"%s  }\n", indent);
   	fprintf(pfp,"%s}\n\n", indent);

   	return;
}
//--------------------------------------------------------------------------
// mkdir
//		Makes a directory for the generated .java source file.
//--------------------------------------------------------------------------
bool mkdir(char *dirName, mode_t mode)
{
	char buf[MAXPATHLEN];
   	struct stat statbuf;

   if(stat(dirName, &statbuf) < 0) {
   		if(errno != ENOENT) {
      		printf("ddl2java - Error %d returned while trying to create .java file.\n", errno);
         	return false;
      	}
      	sprintf(buf,"mkdir %s 2>/dev/null", dirName);
      	system(buf);
      	if(stat(dirName, &statbuf) < 0) {
      		printf("ddl2java - Error unable to create directory %s for .java file.\n", dirName);
         	return false;
      	}
      	/*if(chmod(dirName, mode) != 0) {
      		printf("ddl2java - Error %d when attempting to chmod for directory %s.\n",errno, dirName);
         	return false;
      	} else {
      		return true;
      	} */
      	return true;
   } else {
   		if(!(statbuf.st_mode&S_IFDIR)) {
      		printf("ddl2java - Error %s specified for the source path is not a directory.\n",
           		dirName);
         	return false;
      	}
   }
   return true;
}
//----------------------------------------------------------------------------
// makeName
// 	mallocs memory for various name fields.
//----------------------------------------------------------------------------
char * makeName(int orgSize)
{
	char *newName;

   	newName = (char *)malloc(orgSize + sizeof(orgSize));
   	return(newName);
}
//-----------------------------------------------------------------------------------------
// upshiftFirstCharacter
//		Upshifts the first character in orgName and returns the new values in newName
//-----------------------------------------------------------------------------------------
void upshiftFirstCharacter(char *orgName, char *newName, int orgNameLen, int newNameLen)
{
    int ind;

    assert(newNameLen >= (orgNameLen + 1));
    for(ind = 0; ind < orgNameLen; ind++) {
    	if(ind == 0) {
      		newName[ind] = (char) toupper((int)orgName[ind]);
      	} else {
      		if(orgName[ind] == '\0') {
         		newName[ind] = '\0';
         		return;
         	}
      		if(orgName[ind] == ' ') {
         		newName[ind] = '\0';
            	return;
        	}
        	if(orgName[ind] == '-') {
				newName[ind] = '_';
			} else {
				if(isalpha(orgName[ind]))
      				newName[ind] = (char) tolower((int)orgName[ind]);
      			else
      				newName[ind] = orgName[ind];
			}

      	}
    }  // for end
    newName[ind + 1] = '\0';
    return;
}
//------------------------------------------------------------------------------
// validateDataType
//		Checks that the data type for a field is a supported data type
//------------------------------------------------------------------------------
bool validateDataType(short dataType)
{

	if ( (dataType < 0) || (dataType == 7)
   		|| (dataType == 11)
      	|| (dataType > SQLINTERVALDAYTOFRACTION) ) 	{
      	printf("ddl2java - Internal Error: Unknown data type (%d) in ddl dictionary.\n",
   			dataType);
   		return false;
   }

   if ( dataType == COMPLEX32_2)   {
		printf("ddl2java - Error: Unsupported data type Complex32^2.\n");
      	return false;
   }
   return (true);
}
//------------------------------------------------------------------------
// printInputConversionErrors
//		Prints the code concerning errors during conversion for the INPUT
//		class.
//------------------------------------------------------------------------
void printInputConversionErrors(FILE *pfp, char *fieldName, char *indent)
{
	fprintf(pfp,"%s  }catch (DataConversionException dce) {\n", indent);
	fprintf(pfp,"%s     _numConversionErrors++;\n",indent);
	fprintf(pfp,"%s     if(_allowErrors) {\n", indent);
	fprintf(pfp,"%s        _errorDetail.put(\"%s\",dce);\n",
	           		indent, fieldName);
	fprintf(pfp,"%s     } else { \n",indent);
	fprintf(pfp,"%s        throw dce;\n",indent);
	fprintf(pfp,"%s     }\n",indent);
    fprintf(pfp,"%s  }\n", indent);
    return;
}
//------------------------------------------------------------------------------
// printConversionException
//------------------------------------------------------------------------------
void printConversionException(FILE *pfp, char *indent, char *fieldName)
{
	fprintf(pfp,"%s     throw new TsmpRequestException(\"Conversion error occurred for %s\");\n",
      		indent,fieldName);

   	fprintf(pfp,"%s  }\n",indent);
   	fprintf(pfp,"%s}\n",indent);
   	fprintf(pfp,"%scatch(NumberFormatException e) {\n", indent);
   	fprintf(pfp,"%s  throw new TsmpRequestException(\"Conversion error: Either %s does not contain a parseable number or the value is out of range.\");\n",
           	indent, fieldName);
   	fprintf(pfp,"%s}\n", indent);
   	return;
}
//-----------------------------------------------------------------
// writeImports
//-----------------------------------------------------------------
void writeImports(FILE *pfp,bool isTsmp, char *packageName)
{

   	fprintf(pfp,"package %s;\n", packageName);
   	if(isTsmp)
   		fprintf(pfp,"import com.tandem.tsmp.*;\n");
   	fprintf(pfp,"import java.util.Vector;\n");
   	fprintf(pfp,"import java.util.Hashtable;\n");
   	fprintf(pfp,"import java.io.CharConversionException;\n");
   	if(!isTsmp) {
		fprintf(pfp,"import java.util.HashMap;\n");
		fprintf(pfp,"import java.util.MissingResourceException;\n");
		fprintf(pfp,"import com.tandem.ext.guardian.*;\n");
		fprintf(pfp,"import com.tandem.ext.util.*;\n");
		fprintf(pfp,"import java.math.BigDecimal;\n"); //Soln_10-130423-7115
	}
}
//------------------------------------------------------------------
// writeSpecialIOMethods
//------------------------------------------------------------------
void writeSpecialIOMethods(FILE *pfp, int classType)
{
	// getLength
	if(classType == INPUT || classType == OUTPUT || classType == IO) {
		fprintf(pfp,"\n");
		fprintf(pfp,"/**\nReturns the expected length of the byte array used by this class.\n");
		fprintf(pfp,"@return int expected length of byte array.\n*/\n");
		fprintf(pfp,"   public int getLength() {\n");
		fprintf(pfp,"      return _bufferLen;\n");
		fprintf(pfp,"   }\n");
	}

// Begin Changes for Solution Number 10-040917-9893 

	if(classType == INPUT || classType == IO) {

//End Changes for Solution Number 10-040917-9893

		// allowErrors
		fprintf(pfp,"/**\n");
		fprintf(pfp,"  Indicates whether DataConversion Exceptions are thrown during execution\n");
		fprintf(pfp,"  of the unmarshal method. Specifying false indicates that the first data\n");
		fprintf(pfp,"  conversion error encountered during execution of the unmarshal method, \n");
		fprintf(pfp,"  causes a DataConversionException to be thrown.  All subsequent data in \n");
		fprintf(pfp,"  the byte array supplied to the unmarshal method is NOT converted\n\n");
		fprintf(pfp,"  Specifying true indicates that DataConversionExceptions are ignored \n");
		fprintf(pfp,"  during execution of the unmarshal method.  umarshal will attempt to \n");
		fprintf(pfp,"  convert all data in the byte array.  To determine if errors have \n");
		fprintf(pfp,"  occurred during execution of unmarshal, the user can call \n");
		fprintf(pfp,"  getNumConversionFailures().  This method returns the number of fields \n");
		fprintf(pfp,"  that failed to convert.  If the call to getNumConversionFailures() \n");
		fprintf(pfp,"  returns a number greater than zero, a call to \n");
		fprintf(pfp,"  getConversionFailureDetails() returns a HashMap whose keys are the fields\n");
		fprintf(pfp,"  that failed to convert and whose value is the associated \n");
		fprintf(pfp,"  DataConversionException.\n");
		fprintf(pfp,"  @param boolean true to indicate that conversion errors are to be \n");
		fprintf(pfp,"  allowed; false otherwise.\n");
		fprintf(pfp,"  @see #getNumConversionFailures\n");
		fprintf(pfp,"  @see #getConversionFailureDetails\n");
		fprintf(pfp,"*/\n");
		fprintf(pfp,"    public void allowErrors(boolean value) {\n");
		fprintf(pfp,"       _allowErrors = value;\n");
		fprintf(pfp,"    }\n");
		// getNumConversionFailures
		fprintf(pfp,"/**\n");
		fprintf(pfp,"   Returns the number of fields that failed to convert during execution\n");
		fprintf(pfp,"   of the unmarshal method.\n");
		fprintf(pfp,"   @return the number of fields that failed to convert during the \n");
		fprintf(pfp,"   execution of unmarshal.  A returned value of zero indicates that\n");
		fprintf(pfp,"   all fields converted.\n");
		fprintf(pfp,"   @see #allowErrors\n");
		fprintf(pfp,"   @see #getConversionFailureDetails\n*/\n");
		fprintf(pfp,"    public int getNumConversionFailures() {\n");
		fprintf(pfp,"       return _numConversionErrors;\n");
		fprintf(pfp,"    }\n");
		// getConversionFailureDetails
		fprintf(pfp,"/**\n");
		fprintf(pfp,"    Returns a HashMap with information about fields that failed to \n");
		fprintf(pfp,"    convert during execution of the unmarshal method.  Keys are \n");
		fprintf(pfp,"    the fields that failed to convert and values are the associated\n");
		fprintf(pfp,"    DataConversionExceptions.\n\n");
		fprintf(pfp,"    @return HashMap containing details of data conversion failures.\n");
		fprintf(pfp,"    If no failures occurred the HashMap.isEmpty() method returns true.\n");
		fprintf(pfp,"    @see #getNumConversionFailures\n");
		fprintf(pfp,"    @see #allowErrors\n");
		fprintf(pfp,"*/\n");
		fprintf(pfp,"     public HashMap getConversionFailureDetails() {\n");
		fprintf(pfp,"        return _errorDetail;\n");
		fprintf(pfp,"     }\n");
	}
}
