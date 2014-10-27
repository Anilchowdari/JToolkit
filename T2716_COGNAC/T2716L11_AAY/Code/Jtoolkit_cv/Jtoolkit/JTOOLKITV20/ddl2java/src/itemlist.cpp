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
// Filename:  itemList.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "itemlist.h"
#include "ddl.h"
#include <stdio.h>
#include <ctype.h>
struct detail_list_t ddl_detail_info_struct;
//--------------------------------------------------------------------------
// ItemList
//--------------------------------------------------------------------------
ItemList::ItemList()
{
	fileName[0] = 0;
} // ItemList
//---------------------------------------------------------------------------
// ItemList Destructor
//----------------------------------------------------------------------------
ItemList::~ItemList()
{
	closeFile();
} // ItemList destructor
//----------------------------------------------------------------------------
// closeFile
//----------------------------------------------------------------------------
void
ItemList::closeFile()
{
	if(fileName[0] != 0) {
   	fclose(fp);
   }
   fileName[0] = 0;
} // closeFile
//----------------------------------------------------------------------------
// openFile
//----------------------------------------------------------------------------
bool
ItemList::openFile(char *itemPath)
{

	int i;

	strncpy(fileName, itemPath, MAXPATHLEN);
   for(i=0; i < MAXPATHLEN; i++) {
     	if(fileName[i] != ' ') {
        	continue;
      } else {
        	fileName[i] = '\0';
         break;
      }
   }


   if((fp = fopen(fileName,"r")) == NULL) {
   	printf("ddl2java - Error: Can't open item list file %s\n",fileName);
      return false;
   } else {
   	return true;
   }
}
//-------------------------------------------------------------------------
// upShiftAll
//-------------------------------------------------------------------------
void
ItemList::upShiftIt(char *original, char *upShiftedName)
{
	int i;

   for(i = 0; i < ((int)strlen(original) + 1); i++) {
   	if((original[i] == '\0')|| (original[i] == '\n')) {
      	upShiftedName[i] = '\0';
      	break;
      }
   	upShiftedName[i] = toupper((int) original[i]);

   }
}

//-------------------------------------------------------------------------
// readItemInfo
//-------------------------------------------------------------------------
int
ItemList::readItemInfo(itemInfo *listItem, int defaultLanguage, char *dictPath)
{
 	char line[MAX_LINE_LEN];
   char *gotLine;
   char *ptr1, *ptr2;
   char temp[31];
   char ddlType[31];
   char ddlName[DDL_NAME_SIZE + 1];
   char tempName[DDL_NAME_SIZE + 1];
   char classType[5], lang[6], charStringType[3];
   bool gotGoodLine = false;

   classType[0] = 0;
   lang[0] = 0;
   charStringType[0] = 0;

   gotLine = fgets(line, sizeof(line), fp);
   while(gotLine != NULL) {
   	if(line[0] == '\n') {
      	gotLine = fgets(line, sizeof(line), fp);
      	continue;
      } else {
      	gotGoodLine = true;
      	break;
      }
   }
   if(!gotGoodLine) {
     	return 1;
   }

   ptr1 = strtok(line,",");
   if(ptr1 != NULL) {
     	(void) sscanf(line, "%s %s", temp, tempName);
   }
   upShiftIt(tempName, ddlName);
   upShiftIt(temp, ddlType);

   ptr2 = strtok(NULL,",");

   while(ptr2 != NULL) {
   		memset(temp,'\0',(int) sizeof(temp));
   		upShiftIt(ptr2,temp);
     	if((strncmp(temp, "R", 1) == 0) || (strncmp(temp,"B",1) == 0)){
        	strcpy(classType, temp);
		} else if((strncmp(temp, "I",1) == 0) || (strncmp(temp,"O",1) == 0)){
			strcpy(classType, temp);
     	} else if(strncmp(temp, "C", 1) == 0) {
        	strcpy(lang, temp);
      	} else if(strncmp(temp, "N", 1) == 0) {
         strcpy( charStringType,temp);
      	} else {
      		printf("ddl2java - Error: Invalid value: %s for DDL Element: %s\n",
         		temp, tempName);
         	return -1;
      	}
      	ptr2 = strtok(NULL,",");
   }
   strcpy(listItem->ddlElementName, ddlName);

   if(strncmp(ddlType, "DEF",3) == 0) {
   	listItem->ddlElementType = DDLDEF;
   } else if(strncmp(ddlType, "REC", 3) == 0) {
      listItem->ddlElementType = DDLREC;
   } else {
      printf("ddl2java - Error: Invalid DDL element type value: %s specified for %s\n",
      	ddlType, tempName);
      return -1;
   }

   if(classType[0] != 0) {
   		if(strcmp(classType,"RQ") == 0) {
      		listItem->generatedClassType = CLASSRQ;
      		printf("ddl2java - deprecated classType RQ specified\n");
      	} else if(strcmp(classType, "RP") == 0) {
      		listItem->generatedClassType = CLASSRP;
      		printf("ddl2java - deprecated classType RP specified\n");
      	} else if(strcmp(classType, "BOTH") == 0) {
      		listItem->generatedClassType = CLASSBOTH;
      		printf("ddl2java - deprecated classType BOTH specified\n");
		} else if(strcmp(classType,"INPUT") == 0) {
			listItem->generatedClassType = INPUT;
		} else if(strcmp(classType,"OUTPUT") == 0) {
			listItem->generatedClassType = OUTPUT;
		} else if(strcmp(classType, "IO") == 0) {
			listItem->generatedClassType = IO;
      	} else {
      		printf("ddl2java - Error: Invalid classType: %s specified for %s.\n",
         	classType, tempName);
         	return -1;
      	}
   } else {
   	printf("ddl2java - Error: No classType specified for %s.\n",tempName);
      return -1;
   }
   if(lang[0] != 0) {
   	if(strcmp(lang, "COBOL") == 0) {
      	listItem->languageType =  COBOL_LANG;
      } else if(strcmp(lang,"C") == 0) {
      	listItem->languageType = C_LANG;
      } else {
      	printf("ddl2java - Error: Invalid language type: %s specified for %s.\n", lang, tempName);
         return -1;
      }
   } else {
   	listItem->languageType = defaultLanguage;
   }

   if(charStringType[0] != 0) {
   	if(strcmp(charStringType,"NT") == 0) {
      	listItem->charStringType = NULLTERMINATED;
      } else if(strcmp(charStringType,"NN") == 0) {
      	listItem->charStringType = NON_NULLTERMINATED;
      } else {
      	printf("ddl2java - Error: Invalid value specifed for character string type: %s for %s.\n",
         	charStringType, tempName);
         return -1;
      }
   } else {
   	listItem->charStringType = NON_NULLTERMINATED;
   }
   strncpy(listItem->ddlDictPath, dictPath, HOST_SUBVOL_SIZE);
   return 0;
}


