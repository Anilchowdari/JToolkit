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
// Filename:  classgen.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "classgen.h"
#include "detailList.h"
#include "ddl.h"
#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "java1.h"

static DETAIL_LIST *_detail_list;

//--------------------------------------------------------------------------
// DetailListProcessor
//   Constructor for DetailListProcessor
//--------------------------------------------------------------------------
DetailListProcessor::DetailListProcessor()
{
	_detail_list = NULL;
   num_elements = 0;
   fillerCount = 0;
} // ClassGen
//--------------------------------------------------------------------------
// DetailListProcessor Destructor
//--------------------------------------------------------------------------
DetailListProcessor::~DetailListProcessor()
{

} // DetailListProcessor Destructor


//-----------------------------------------------------------------------
// addToDetailList
//		creates a linked list containing information about each field within
//		a DEF or RECORD.
//
//	side effects:
// 	1. Checks if the field name needs qualification.  If it does, this
//	   routine calls another routine to create the qualified name.
//-----------------------------------------------------------------------
bool DetailListProcessor::addToDetailList(ddl_element_def element,bool exp)
{
   	int i;
   	DETAIL_LIST *next, *list_item, *tail;
   	bool name_needs_qualification = false;
   	char fieldName[DDL_NAME_SIZE + 1];

   	if((element.ddl_element_dc.level == 66) ||
   		(element.ddl_element_dc.level == 88 ) ||
   		(element.is_ddl_filler == 'Y'))
      	return true;

   	next = (DETAIL_LIST *) malloc (sizeof(DETAIL_LIST));
   	if(next == NULL) {
   		printf("ddl2java - Error: Unable to obtain memory for detail information about Record or DEF\n");
      	return false;
   	}
   	next->detail_info.qualName.currNameLen = next->detail_info.discriminator.currNameLen = 0;
   	next->detail_info.qualName.Name = next->detail_info.discriminator.Name = NULL;

   	next->detail_info.qualified_name_required_flag = 'N';
   	next->detail_info.parent_occurs_flag = 'N';
   	memcpy(&next->detail_info.element_info, &element,sizeof(ddl_element_def));
   	num_elements ++;

   	/* Level 89 records don't have a data type field, so don't validate it */
   	if (element.ddl_element_dc.level != 89) {
       if(!validateDataType (next->detail_info.element_info.ddl_element_dc.host_data_type)) {
   	      return(false);
       }
   	}

   	memset(fieldName,' ', DDL_NAME_SIZE + 1);
   	for(i = 0; i < DDL_NAME_SIZE + 1; i++) {
   		if(next->detail_info.element_info.ddl_element_dc.ddl_element_name[i] == ' ') {
      		fieldName[i ] = '\0';
        	next->detail_info.element_info.ddl_element_dc.ddl_element_name[i] = '\0';
        	break;
      	} else {
      		fieldName[i] = next->detail_info.element_info.ddl_element_dc.ddl_element_name[i];
      	}
   	}

   	next->nItem = NULL;
   	next->detail_info.qualName.Name = makeName(NAMESIZE);
   	if(next->detail_info.qualName.Name == NULL) {
     	printf("ddl2java - Error: memory allocation error.\n");
      	return(false);
   	} else {
     	next->detail_info.qualName.currNameLen = NAMESIZE;
   	}
   	next->detail_info.element_info.ddl_element_dc.hasBeenInitialized = -1;
   	if(_detail_list == NULL) {
		strcpy(next->detail_info.qualName.Name,
		next->detail_info.element_info.ddl_element_dc.ddl_element_name);
      	next->detail_info.qualified_name_required_flag = 'N';
      	next->detail_info.parent_occurs_flag = 'N';
   	  	_detail_list = tail = next;
      	_detail_list->pItem = NULL;
      	return true;
   	}
   	list_item = _detail_list;
   	while(list_item != NULL) {
   		if(strcmp(fieldName,"filler") != 0) {
      		// Don't do check to see if name needs qualification if field name is filler.
   			if(strcmp(list_item->detail_info.element_info.ddl_element_dc.ddl_element_name, fieldName) == 0) {
      			name_needs_qualification = true;
				list_item->detail_info.qualified_name_required_flag = 'Y';

      		}
			//changes begin for sol:10-110831-9468 by Babu
			//The fix will generate fully qualified instance variable names even if 
			//there is an existing DDL definition or record is used as the type of a field in
			//another definition or record and there are not more than once reference to the existing definition or record.
			if (exp == true)
			if(list_item->detail_info.element_info.ddl_element_dc.level <= 1) {
				name_needs_qualification = true;
				list_item->detail_info.qualified_name_required_flag = 'Y';
			}
			//changes end for sol:10-110831-9468 by Babu
      	}
      	if(list_item->nItem == NULL) {
      		break;
      	}
      	list_item = list_item->nItem;
   	}
   	assert(list_item != NULL);
   	assert(list_item->nItem == NULL);
   	list_item->nItem = next;
   	next->pItem = list_item;
   	tail = next;

   	/* Check for occurring parents */
   	if(next->detail_info.element_info.ddl_element_dc.level <= 1) {
   		next->detail_info.parent_occurs_flag = 'N';
      	next->parent = NULL;
   	}

   	if(strcmp(next->detail_info.element_info.ddl_element_dc.ddl_element_name,"filler") == 0) {
	   	sprintf(next->detail_info.qualName.Name,"%s%d",
      	next->detail_info.element_info.ddl_element_dc.ddl_element_name, fillerCount);
	  	if(next->detail_info.element_info.ddl_element_dc.host_data_type != 0) {
      		next->detail_info.element_info.ddl_element_dc.host_data_type = 0;
      	}
      	fillerCount++;
      	return true;
   	}

	if((name_needs_qualification == false) ||
   		(next->detail_info.element_info.ddl_element_dc.element_type == 0)) {
      	next->detail_info.qualified_name_required_flag = 'N';
      	strcpy(next->detail_info.qualName.Name,
      	next->detail_info.element_info.ddl_element_dc.ddl_element_name);
      	check4JavaReservedWord(next);
      	return true;
   } else {
   		next->detail_info.qualified_name_required_flag = 'Y';
   }
   return true;
}
//----------------------------------------------------------------------------
// check4JavaReservedWord
//
//	Checks if a field name is a Java Reserved word.  If it is, this routine
//	prepends an underscore to the field name and moves the newly created name
//  to the qualifiedName.Name field of the list item.
//------------------------------------------------------------------------------
void
DetailListProcessor::check4JavaReservedWord(DETAIL_LIST *item)
{
	int i;
   	bool isReservedWord = false;
   	char tempName[MAXPATHLEN];


   	for(i = 0; i < NUM_RESERVED_WORDS; i++) {
   		if(strcmp(item->detail_info.qualName.Name, java_reserved_words[i].word) == 0 ) {
      		isReservedWord = true;
         	break;
      	}
   	}
   	if(isReservedWord) {
   		strcpy(tempName,item->detail_info.qualName.Name);
      	if((strlen(item->detail_info.qualName.Name) + 4) > item->detail_info.qualName.currNameLen) {
      		item->detail_info.qualName.Name = (char *) realloc(item->detail_info.qualName.Name,
           		item->detail_info.qualName.currNameLen + NAMESIZE);
         	if(item->detail_info.qualName.Name == NULL) {
           		printf("ddl2java - Fatal error: Unable to obtain memory for qualified names.\n");
        		printf("ddl2java exits\n");
        		exit(EXIT_FAILURE);
      		}
         	item->detail_info.qualName.currNameLen = item->detail_info.qualName.currNameLen + NAMESIZE;
      	}
      	sprintf(item->detail_info.qualName.Name,"_%s", tempName);
   }
   return;
}
//-----------------------------------------------------------------------
// Returns a pointer to the detail list also sets the number of
// elements in the list
//-----------------------------------------------------------------------
detailListHolder DetailListProcessor::getDetailList()
{
	detailListHolder  list;
	list.head = _detail_list;
	list.numElements = num_elements;
	return list;
}
