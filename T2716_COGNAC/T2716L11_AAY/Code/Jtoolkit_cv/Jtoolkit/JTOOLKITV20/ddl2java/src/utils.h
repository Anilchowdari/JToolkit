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
// Filename:  utils.h
//--------------------------------------------------------------------------
#ifndef __UTILSH__
#define __UTILSH__

void convert3WordTS(long long *ts64, unsigned short *ts3Word );
void fileNameWithNull(char *fileNameWithBlanks, char* fileName);
void fixDDLName(char *fileName, int fNameLen);
bool fixFileName(const char *fileName, int maxNewFileNameLen, char *newFileName);
void writeNumericConvertMethod(char *indent, int dataType, FILE *pfp);
bool mkdir(char *dirName, mode_t mode);
char * makeName(int orgSize);
void upshiftFirstCharacter(char *orgName, char *newName, int orgNameLen, int newNameLen);
bool validateDataType(short dataType);
void printInputConversionErrors(FILE *pfp, char *fieldName, char *indent);
void printConversionException(FILE *pfp, char *indent, char *fieldName);
void writeImports(FILE *pfp,bool isTsmp, char *packageName);
void writeSpecialIOMethods(FILE *pfp, int classType);
#endif // __UTILSH__

