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
//--------------------------------------------------------------------------
//
// Filename:  ddl2java.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "ddlread.h"
#include "ddlproc.h"
#include "itemlist.h"
#include "detailList.h"
#include "classgen.h"
#ifdef __TANDEM
#include <unistd.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


#define MODULE_ID "DDL2"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef _VPROC
void _VPROC(void) {};
#endif
#ifdef __cplusplus
}
#endif
int defaultLanguageType;
// Begin of Changes for Solution Number 10-030319-4961 
int stringInitializationType;
// End of Changes for Solution Number 10-030319-4961 
//--------------------------------------------------------------------------
// ddl2Java
//--------------------------------------------------------------------------
ddl2Java::ddl2Java()
{
   	verbose = false;
   	compile = false;

} // ddl2Java contructor

//--------------------------------------------------------------------------
// ddl2Java Destructor
//--------------------------------------------------------------------------

ddl2Java::~ddl2Java()
{

} // ddl2Java destructor

//--------------------------------------------------------------------------
void
ddl2Java ::initializeHdr(readDictRep *rph)
{
   rph->status = EC_OK;
} // initializeReplyHdr
//--------------------------------------------------------------------------
// getDDLInfo
//
// Get list of Records or Defs to generate
//--------------------------------------------------------------------------
bool ddl2Java::getDDLInfo(char *readDictRequest, itemInfo *itemList,
	char *packageName, char *sourcePath, char *encodingName,bool exp)
{

	int i, j;
	char moreFields = 'N';
   	bool foundError = false;
   	bool foundOneField = false;
   	readDefStructureReq 	read_struct_req;
   	readDictRep 			readDictReply;
   	readDefStructureRep 	*readDefStructureReply;
   	readDictReq *readDictPtr = (readDictReq *)readDictRequest;
   	classGenInfo cgInfo;
	detailListHolder dl;
	DETAIL_LIST *dlptr;
	int numEl = 0;

   	readDefStructureReply = (readDefStructureRep * ) malloc(sizeof (readDefStructureRep));
   	if(readDefStructureReply == NULL) {
   		printf("ddl2java - Error: Unable to obtain memory for ddl structure information.\n");
      	return false;
   	}

   	initializeHdr(&readDictReply);

   	memset(readDefStructureReply,' ',sizeof(readDefStructureRep));

   	classGen = new ClassGen();
	detailList = new DetailListProcessor();
   	/* Get list of RECs or DEFs */
   	ddlProcessor->ddl_read_ddl_defs(readDictRequest,(char *)&readDictReply);
   	if(readDictReply.status == EC_OK)  {
   		memset(&read_struct_req,' ', sizeof(readDefStructureReq));
      	read_struct_req.request_code = RC_READ_DDL_DEF_STRUCT;
      	memcpy(&read_struct_req.dict_subvol,readDictReply.ddl_subvol_expanded,HOST_SUBVOL_SIZE);
      	for(i = 0; i < readDictReply.num_returned; i++) {
      		read_struct_req.ddl_object_id = readDictReply.def_info[i].ddl_object_id;
      		read_struct_req.next_ddl_element_id = 0;
      		read_struct_req.num_wanted = MAX_DDL_ELEMENTS;
         	/* Get list of elements within a given REC or DEF */
      		ddlProcessor->ddl_read_ddl_elements((char *)&read_struct_req, (char *)readDefStructureReply);
			if((foundOneField == false) && (readDefStructureReply->num_returned == 0))
				return false;
        	for(j = 0; j < readDefStructureReply->num_returned; j ++) {
         		/* Create a list of all the elements within the given REC or DEF */
				foundOneField = true;
            	if(!(detailList->addToDetailList(readDefStructureReply->element_info[j],exp))) {
                  	foundError = true;
            	}
         	}
      		moreFields = readDefStructureReply->more_flag;
      		while( moreFields == 'Y') {
      			read_struct_req.next_ddl_element_id = readDefStructureReply->next_ddl_element_id;
            	memset(readDefStructureReply,' ', sizeof(readDefStructureRep));
         		ddlProcessor->ddl_read_ddl_elements((char *)&read_struct_req, (char *)readDefStructureReply);
            	for(j = 0; j < readDefStructureReply->num_returned; j ++) {
            		if(!(detailList->addToDetailList(readDefStructureReply->element_info[j],exp))) {
               			foundError = true;
               		}
            	}
         		moreFields = readDefStructureReply->more_flag;
      		}
         	if(foundError) {
         		free(readDefStructureReply);
            	delete classGen;
				delete detailList;
            	printf("ddl2java - Errors found in data types; ddl2java exiting ...\n");
            	return false;
         	}
         	/* Write the class for the given REC or DEF */
         	strcpy(cgInfo.ddlElName,itemList->ddlElementName);
         	strncpy(cgInfo.packageName, packageName,MAXPATHLEN);
         	strncpy(cgInfo.sourcePath, sourcePath, MAXPATHLEN);
         	strncpy(cgInfo.encodingName, encodingName,MAXPATHLEN);
         	cgInfo.languageType = itemList->languageType;
         	cgInfo.generatedClassType = itemList->generatedClassType;
         	cgInfo.verbose = verbose;
         	cgInfo.compile = compile;
         	cgInfo.charStringType = itemList->charStringType;
         	cgInfo.alignmentType = itemList->alignmentType;
			dl = detailList->getDetailList();
			dlptr = dl.head;
			numEl = dl.numElements;
			classGen->setDetailList(dlptr,numEl);
      		if(!classGen->generateClassFile(&cgInfo)) {
         		free(readDefStructureReply);
            	delete classGen;
           		return false;
      		}
      	}
   	} else {
   		free(readDefStructureReply);
      	delete classGen;
      	return false;
   	}
   	free(readDefStructureReply);
   	delete classGen;
   	return true;
}
//--------------------------------------------------------------------------
// processRequest
//
//  Do general setup and then process the user's request.
//--------------------------------------------------------------------------
bool ddl2Java::processRequest(char *generationInfo)
{
   generation_info *genInfo = 	(generation_info *)generationInfo;

   itemInfo itemList;
   readDictReq 			readDictRequest;

   if(genInfo->verbose == 1)
   		verbose = true;

   if(genInfo->compile == 1)
   		compile = true;
   if(verbose)
   		printf("Reading list of items for which classes will be generated.\n");

   if(!getListItem->openFile(genInfo->item_list_path)) {
   		return false;
   }
   while(getListItem->readItemInfo(&itemList, defaultLanguageType,genInfo->ddl_dict_path) == 0) {
   		memset(&readDictRequest,' ', sizeof(readDictReq));
      	strncpy(readDictRequest.dict_subvol, genInfo->ddl_dict_path,HOST_SUBVOL_SIZE);
   		switch(itemList.ddlElementType) {
      	case DDLREC:
           	strcpy(readDictRequest.next_ddl_ODF_key.def_name, itemList.ddlElementName);
            strcpy(readDictRequest.start_def_characters,itemList.ddlElementName);
            strcpy(readDictRequest.end_def_characters,itemList.ddlElementName);
            readDictRequest.request_code = RC_READ_ONE_REC;
            readDictRequest.num_wanted = 1;
            strncpy(readDictRequest.next_ddl_ODF_key.odf_id,"  ",2);
            if(!getDDLInfo((char *)&readDictRequest, &itemList,
				genInfo->package_name, genInfo->source_path, genInfo->encoding, genInfo->expand)) {
         	  	return false;
           	}
            break;
      	case DDLDEF:
         	strcpy(readDictRequest.next_ddl_ODF_key.def_name, itemList.ddlElementName);
            strcpy(readDictRequest.start_def_characters,itemList.ddlElementName);
            strcpy(readDictRequest.end_def_characters,itemList.ddlElementName);
            readDictRequest.request_code = RC_READ_ONE_DEF;
            readDictRequest.num_wanted = 1;
            strncpy(readDictRequest.next_ddl_ODF_key.odf_id,"  ",2);
            if(!getDDLInfo((char *)&readDictRequest, &itemList,
            	genInfo->package_name, genInfo->source_path, genInfo->encoding, genInfo->expand)) {
         	  	return false;
           	}
            break;
      default: printf("ddl2java - Error: Internal error at processRequest.\n");
      }
   }

  	return true;
} // processRequest
//---------------------------------------------------------------------
// usage
//---------------------------------------------------------------------
void ddl2Java ::usage(void)
{
	printf("Usage: ddl2java -[chvq] -dpath dictionaryPath -i itemList -p packagePath -l language(C or COBOL) -d sourcepath -e encoding\n");
   	return;
}
//---------------------------------------------------------------------
bool ddl2Java ::runProgram(char *genInfo)
{

	bool processDDLsuccessfully;

	ddlProcessor = new DDLProcessor();

   getListItem = new ItemList;

   processDDLsuccessfully = processRequest(genInfo);
   return processDDLsuccessfully;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main (int argc, char ** argv)
{
   	int i;
   	short rtn, status, len;
   	generation_info gen_info;
   	bool foundDictionary = false, foundPackage = false, foundItemList = false, foundSPath = false;
   	ddl2Java *ddl2java = new ddl2Java();

	char dictPath[MAX_GFILE_LEN];
   	defaultLanguageType = 0;
    memset(&gen_info,' ', sizeof(generation_info));
   	gen_info.verbose = 0;
	gen_info.expand = false;
   	strcpy(gen_info.source_path,".");
   	strcpy(gen_info.encoding,".");


   	printf("%s\n",BANNER);

   	for(i = 1; argc > i;) {
     	if(strcmp("-c", argv[i]) == 0) {
      	gen_info.compile = 1;
         i++;
    } 
	//changes begin for sol:10-110831-9468
	//-q option Creates fully qualified instance variable names in the generated class if an existing 
	//definition or record is used as the type of a field in another definition or record.
	else if(strcmp("-q", argv[i]) == 0) {
		gen_info.expand = true;
        i++;
    } 
	//changes end for sol:10-110831-9468
	else if(strcmp("-d", argv[i]) == 0) {
      	if(++i < argc) {
         	if((strncmp("-", argv[i],1) == 0) || (strncmp("/G",argv[i], 2) == 0)) {
            	printf("ddl2java - Error: valid source path must be specified with -d option.\n");
            	return(1);
            }
         	strncpy(gen_info.source_path, argv[i], MAXPATHLEN);
         	foundSPath = true;
         } else {
         	printf("ddl2java - Error: source path must be specified with -d option.\n");
            return(1);
         }
         i++;
    } else if(strcmp("-dpath", argv[i]) == 0) {
         if(++i < argc) {
            if(strncmp("-", argv[i],1) == 0) {
            	printf("ddl2java - Error: DDL dictionary path must be specified with -dpath option.\n");
            	return(1);
            }
         	strncpy(dictPath, argv[i], MAX_GFILE_LEN);
         	i++;
#ifdef __TANDEM
			rtn = PATHNAME_TO_FILENAME_(dictPath,gen_info.ddl_dict_path,
                sizeof(gen_info.ddl_dict_path),&len, &status);
#else
         	rtn = PATHNAME_TO_FILENAME__(dictPath,gen_info.ddl_dict_path,
            	sizeof(gen_info.ddl_dict_path),&len, &status, /* octal mask: */ 037000000000);
#endif
         	if((rtn == 0) && (gen_info.ddl_dict_path[0] != '0') &&
            	(status == 1)) {
            	gen_info.ddl_dict_path[len + 1] = '\0';
         	} else {
             	printf("ddl2java - Error: Invalid value (%s) specified for -dpath option.\n",
               	dictPath);
         	}
         	foundDictionary = true;
         } else {
         	printf("ddl2java - Error: DDL dictionary path must be specified with -dpath option.\n");
            return(1);
         }
    } else if(strcmp("-i", argv[i]) == 0) {
         if(++i < argc) {
         	if((strncmp("-", argv[i],1) == 0) || (strncmp("/G",argv[i],2) == 0)) {
            	printf("ddl2java - Error: valid item list file name must be specified with -i option.\n");
            	return(1);
            }
            len = strlen(argv[i]);
         	if(len > MAXPATHLEN) {
            	printf("ddl2java - Error: file name (%s) specified for -i option exceeds the maximum path length of 1024.\n",
               	argv[i]);
            	return(1);
         	} else {
         		strncpy(gen_info.item_list_path, argv[i], len );
            	i++;
         	}
         	foundItemList = true;
         } else {
         	printf("ddl2java - Error: item list file name must be specified with -i option.\n");
            return(1);
         }
    } else if(strcmp("-l", argv[i]) == 0) {
         if(++i < argc) {
            if(strncmp("-", argv[i],1) == 0) {
           	 	printf("ddl2java - Error: Either COBOL or C must be specified with the -l option.\n");
            	return(1);
            }
         	if(strncmp(argv[i],"COBOL", 5) == 0) {
            	defaultLanguageType = COBOL_LANG;
         	} else if (strncmp(argv[i],"C",1) == 0) {
            	defaultLanguageType = C_LANG;
         	} else {
            	printf("ddl2java - Error: Invalid value (%s) specified for -l option.\n", argv[i]);
               return(1);
         	}
            i++;
         } else {
         	printf("ddl2java - Error: Either COBOL or C must be specified with the -l option.\n");
            return(1);
         }
    }
// Begin of Changes for Solution Number 10-030319-4961 
//Option to initialize the string either by SPACE or Null.
else if(strcmp("-o", argv[i]) == 0) {
         if(++i < argc) {
            if(strncmp("-", argv[i],1) == 0) {
           	 	printf("ddl2java - Error: Either SPACE or NULL must be specified with the -o option.\n");
            	return(1);
            }
         	if(strncmp(argv[i],"SPACE", 5) == 0) {
            	stringInitializationType = SPACE_TYPE;
         	} else if (strncmp(argv[i],"NULL",4) == 0) {
            	stringInitializationType =NULL_TYPE;     
         	} else {
            	printf("ddl2java - Error: Invalid value (%s) specified for -o option.\n", argv[i]);
               return(1);
         	}
            i++;
         } else {
         	printf("ddl2java - Error: Either SPACE or NULL must be specified with the -o option.\n");
            return(1);
         }
      } 
// End of Changes for Solution Number 10-030319-4961 
 else if(strcmp("-p", argv[i]) == 0) {
      	if(++i < argc) {
            if((strncmp("-", argv[i],1) == 0) || (strncmp("/G", argv[i],2) == 0)){
            	printf("ddl2java - Error: valid package name must be specified with the -p option.\n");
            	return(1);
            }
      		len = strlen(argv[i]);
            if(len > MAXPATHLEN) {
            	printf("ddl2java - Error: package name (%s) specified for -p option exceeds the maximum path length of 1024.\n",
               	argv[i]);
               return(1);
            } else {
         		strcpy(gen_info.package_name, argv[i]);
            }
            foundPackage = true;
            i++;
         } else {
         	printf("ddl2java - Error: valid package name must be specified with the -p option.\n");
            return(1);
         }
    } else if(strcmp("-e", argv[i]) == 0) {
      	if(++i < argc) {
            if((strncmp("-", argv[i],1) == 0) ){
            	printf("ddl2java - Error: valid encoding property name must be specified with the -e option.\n");
            	return(1);
            }
      		len = strlen(argv[i]);
            if(len > MAXPATHLEN) {
            	printf("ddl2java - Error: encoding property name (%s) specified for -e option exceeds the maximum name length of 1024.\n",
               	argv[i]);
               return(1);
            } else {
         		strcpy(gen_info.encoding, argv[i]);
            }
            i++;
         } else {
         	printf("ddl2java - Error: valid encoding property name must be specified with the -e option.\n");
            return(1);
         }
    } else if(strcmp("-h", argv[i]) == 0) {
           	ddl2java->usage();
            return(1);
    } else if(strcmp("-v", argv[i]) == 0) {
           	gen_info.verbose = 1;
            i++;
    } else {
            printf("ddl2java - Error: unknown option -- %s\n",argv[i]);
         	ddl2java->usage();
            return(1);
      }
	}
   	if(!foundDictionary) {
   		printf("ddl2java - Error: the -dpath option is required and must identify the DDL dictionary.\n");
      	return(1);
   	}
   	if(!foundPackage) {
   		printf("ddl2java - Error: the -p option is required.\n");
      	return(1);
   	}
   	if(!foundItemList) {
   		printf("ddl2java - Error: the -i option is required.\n");
      	return(1);
   	}
   	if(defaultLanguageType == 0) {
   		defaultLanguageType = COBOL_LANG;
   	}
// Begin of Changes for Solution Number 10-030319-4961 
     if(stringInitializationType==0) {
                stringInitializationType = NULL_TYPE;
    } 
// End of Changes for Solution Number 10-030319-4961 

   	ddl2java->runProgram((char *)&gen_info);
   	exit(0);
      
} // main





// end ddl2java.cpp
