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
// Filename:  ddl2java.h
//--------------------------------------------------------------------------
#ifndef __DDL2JAVAH__
#define __DDL2JAVAH__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __TANDEM
#include <tal.h>
#include <cextdecs.h>
#else
#ifndef ___cc_status_DEFINED
#define ___cc_status_DEFINED
typedef int _cc_status;
#endif /* ___cc_status_DEFINED */
#define CCE     1
#define _status_eq(x) ((x) == CCE)

#include "stubcli.h"
#include "llong.h"
#endif


#define true 1
#define false 0
#define MAX_PKGLEN 1024
#define MAXPATHLEN 1024
#define MAXNAMELEN 1024
#define MAX_GFILE_LEN 36
#define MAX_LINE_LEN 1024

#define C_LANG 1
#define COBOL_LANG 2
// Begin of Changes for Solution Number 10-030319-4961 
#define SPACE_TYPE 1
#define NULL_TYPE 0
// End of Changes for Solution Number 10-030319-4961 
#define CLASSRQ 1
#define CLASSRP 2
#define CLASSBOTH 3
#define INPUT 4
#define OUTPUT 5
#define IO 6
#define NULLTERMINATED 1
#define NON_NULLTERMINATED 2
#define DDLDEF 1
#define DDLREC 2


#include "ddl.h"
#include "utils.h"

/*#ifdef __TANDEM
extern int errno;
#endif*/
#define BANNER "ddl2java 3.0 (C)2000 Compaq (C)2003 Hewlett-Packard Development Company, L.P."


typedef struct _itemListInfo
{
	char	ddlDictPath[HOST_SUBVOL_SIZE];
   	char	ddlElementName[DDL_NAME_SIZE + 1];
   	int		ddlElementType;
   	int		languageType;
   	int		generatedClassType;
   	int		charStringType;
   	int		alignmentType;
} itemInfo;

typedef struct _gen_info
{
	char 						package_name[MAXPATHLEN];
   	char						ddl_dict_path[MAX_GFILE_LEN];
   	char						item_list_path[MAXPATHLEN];
   	char						source_path[MAXPATHLEN];
   	int							languageType;
   	int 						compile;
   	int							verbose;
   	char						encoding[MAXPATHLEN];
	bool						expand;// this field is added for providing commandline switch -q. ref solution:10-110831-9468
} generation_info;

typedef struct _classGenInfo
{
	char	ddlElName[DDL_NAME_SIZE + 1];
	char	packageName[MAXPATHLEN];
	char    sourcePath[MAXPATHLEN];
	char	encodingName[MAXPATHLEN];
	bool	verbose;
	bool 	compile;
	int		languageType;
	int 	generatedClassType;
	int		charStringType;
	int		alignmentType;
} classGenInfo;


typedef struct _ddl_struct_info
{
	int	total_num_fields;
   	readDefStructureRep *headPtr;
}ddl_struct_info;

extern int defaultLanguageType;

class DDLDictionary;
class DDLProcessor;
class ItemList;
class ClassGen;
class DetailListProcessor;

class ddl2Java
{
public:
 	ddl2Java();
   ~ddl2Java();
   void initializeHdr(readDictRep *rph);
   bool runProgram(char *generationInfo);
   void usage(void);

protected :
	bool processRequest(char *generationInfo);
    bool getDDLInfo(char *readDictRequest, itemInfo *itemList, char *packageName, char *sourcePath, char *encodingName, bool expand);

private:
   DDLProcessor			*ddlProcessor;
   ItemList				*getListItem;
   ClassGen				*classGen;
   DetailListProcessor  *detailList;
   bool					verbose;
   bool					compile;
};





#endif  // __DDL2JAVAH__

