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
// Filename:  classgen.h
//--------------------------------------------------------------------------
#ifndef __CLASSGEN__
#define __CLASSGEN__
#include "ddl2java.h"
#include "ddlread.h"
#include "ddl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


#define SET_METHOD 1
#define CLEAR_METHOD 2
#define GET_METHOD 3
#define CLEAR_ALL 4
#define DEFAULT_METHOD 5
#define DEFAULT_ALL 6
#define VALUE_STRING 1
#define NULL_STRING 2
#define CLSS 1
#define MTHD 2
#define UNINITIALIZED -1
#define SUBCLASSINITIALIZED 0
#define CLEARALLINITIALIZED 1

class ClassGen
{
public:
	ClassGen();
	~ClassGen();


   void 			checkParentOccurs(DETAIL_LIST *item);
   void 			createQualifiedName(DETAIL_LIST *item, int lowest_level_no);
   void				checkForQualifiedNames();
   bool				openClassFile();
   void				closeClassFile();
   void				writeClassConstructor(int classType);
   void				initializeSubClasses(DETAIL_LIST *item, int indentLevel,
   						int iternum, int classType);
   bool				generateClassFile(classGenInfo *cgInfo);
   bool				processRequest(int classType, bool verbose,
   						bool compile, bool isTsmp);
   bool				writeClassHdr(bool isTsmp, int classType);
   bool				writeInstanceVars(int classType);
   void				writeSubClass(DETAIL_LIST *item, int classType);
   bool				writeMethods(int classType);
   void				writeMethodOccurs(DETAIL_LIST *item, int classType);
   int				getParentNameList(DETAIL_LIST *item, PNAME_LIST *parentNameList);
   void				writeMethodText(DETAIL_LIST *item, int indexes,
   						char *occursName, int classType);
   void				determineText4ElemOccurs(DETAIL_LIST *item, int i, int classType);
   bool				determineText4Occurs(DETAIL_LIST *item, int indentLevel, int iternum, int classType);
   int				getParentName(DETAIL_LIST *item, char *fullName, PLENGTH_LIST *parentOccursList);
   void				writeRQConversionText(DETAIL_LIST *item, char *fieldName,
   						 char *indent, char *iterator, int classType);
   void				writeRPConversionText(DETAIL_LIST *item, char *fieldName,
   						 char *indent, char *iterator, int classType);
   bool				writeLoadUnLoad(int classType);
   void				fixUpRedefinedElement(DETAIL_LIST *item);
   DETAIL_LIST * 	getNextElement(DETAIL_LIST *element);
   DETAIL_LIST *  	getPrevElement(DETAIL_LIST *element);
   void           	expandSQLDataElements();
   void				generateMethodParentOccurs(int indexes, char *OccursName,
                     	char *methodName, char *fieldName, char *simpleName,
                     	char *returnType, char *indent, char *valueString1);
   void				checkDataType(char *indent, int dataType, int classType);
   void 			setDetailList(DETAIL_LIST *list, int numEl);
   void				writeForLoop(DETAIL_LIST *item, char *indent,
   						char *index, int classType);
   void				writeInitializeForLoop(DETAIL_LIST *item, char *indent, char *index);
   void				printRemoveDecimal(char *indent, char *fieldName, int classType);
   void			  	writeHashTable();
   void				writeInputConversionText(DETAIL_LIST *item, char *fieldName,
						char *indent, char *iterator, int classType);
   void				writeOutputConversionText(DETAIL_LIST *item, char *fieldName,
						char *indent, char *iterator, int classType);
   void				writeDoAllText(int classType);
   void 			writeCheckBufferSize(DETAIL_LIST *element,int dataType,char *indent);
   void  			writeElemNumericInstanceVar(int dataType,DETAIL_LIST *element);
   void 			writeOccursNumericInstanceVar(int dataType,DETAIL_LIST *element);
   void             writeInitialValue(int dataType, char *Str, DETAIL_LIST *element);

protected:

	FILE 	*pfp;
   	int 	foundOccurs;
   	char 	className[36];
   	char 	packageName[MAXPATHLEN + 1];
   	char 	sourcePath[MAXPATHLEN + 1];
   	char 	encodingName[MAXPATHLEN + 1];
   	char 	encodingCode[MAXPATHLEN + 80];
   	bool 	writing;
   	int 	languageType;
   	bool 	_indexDeclared;
   	bool 	_tmpDeclared;
   	bool 	verbose;
   	int 	methodType;
   	int 	charStringType;
	bool 	wroteConstructor;
	bool 	encodingSpecified;
	bool 	encodingString;
	int 	num_elements;

};


#endif // __ClassGen__
