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
// Filename:  classgen.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "classgen.h"
#include "ddl.h"
#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
// Begin of Changes for Solution Number 10-030319-4961  
extern int stringInitializationType;
// End of Changes for Solution Number 10-030319-4961  
static DETAIL_LIST *_detail_list;

//--------------------------------------------------------------------------
// ClassGen
//   Constructor for ClassGen
//--------------------------------------------------------------------------
ClassGen::ClassGen()
{
	_detail_list = NULL;
    className[0] = 0;
   	_indexDeclared = false;
   	_tmpDeclared = false;
   	verbose = false;
   	num_elements = 0;
} // ClassGen
//--------------------------------------------------------------------------
// ClassGen Destructor
//--------------------------------------------------------------------------
ClassGen::~ClassGen()
{
	DETAIL_LIST *element, *temp;

   	element = _detail_list;

   	while(element != NULL) {
   		assert(element == _detail_list);
   		temp = element;
   		_detail_list = element = element->nItem;
    	if(temp->detail_info.qualName.Name != NULL) {
      		free(temp->detail_info.qualName.Name);
    	}
   		free(temp);
   	}

} // ClassGen Destructor
//-----------------------------------------------------------------------
// setDetailList
// Sets a pointer to the head of the DetailList and also sets
// the number of elements in the list
//-----------------------------------------------------------------------
void ClassGen::setDetailList(DETAIL_LIST *list, int numEl)
{
	_detail_list = list;
	num_elements = numEl;
}

//-----------------------------------------------------------------------
//
//     For SQL data types must generate multiple elements per "field".
//      --VARCHAR has a length, must generate for field_len.
//      --Any SQL datatype can have a null indicator so must generate
//        for field_indicator.
//     This function will insert elements into the detail list for these
//     extra fields, as needed.
//
//-----------------------------------------------------------------------
void ClassGen::expandSQLDataElements()
{
	DETAIL_LIST *current,*previous,*insertI,*insertV;

   	char *suffixLen = "_len";
   	char *suffixIndicator = "_indicator";
   	//change code
   	int  sqlFieldLength;

	current = _detail_list;
   	/* Go through whole element list, looking for SQL data types */
   	while(current != NULL) {
      	current->detail_info.element_info.ddl_element_dc.sql_length = 0;
      	sqlFieldLength =
              current->detail_info.element_info.ddl_element_dc.host_length;
      	previous = current->pItem;
      	/*  Any SQL data type can have a null indicator */
      	if (current->detail_info.element_info.ddl_element_dc.sqlnullable == 'Y')
      	{
          	insertI = (DETAIL_LIST *) malloc (sizeof(DETAIL_LIST));
          	if (insertI == NULL)
          	{
            	printf("ddl2java - Fatal error: Unable to obtain memory.\n");
            	printf("ddl2java exits\n");
            	exit(EXIT_FAILURE);
          	}
   	  		num_elements++;

          	current->detail_info.element_info.ddl_element_dc.sql_length = sqlFieldLength;
          	memcpy (insertI, current, sizeof(DETAIL_LIST));
          	insertI->nItem = current;
          	insertI->detail_info.qualName.Name
              	= makeName( current->detail_info.qualName.currNameLen
                                   + strlen (suffixIndicator) + 1);
          	if (insertI->detail_info.qualName.Name == NULL)
          	{
            	printf("ddl2java - Fatal error: Unable to obtain memory.\n");
            	printf("ddl2java exits\n");
            	exit(EXIT_FAILURE);
          	}
          	strcpy (insertI->detail_info.qualName.Name, current->detail_info.qualName.Name);
          	strcat (insertI->detail_info.qualName.Name, suffixIndicator);
          	insertI->detail_info.qualName.currNameLen += strlen (suffixIndicator);
          	insertI->detail_info.element_info.ddl_element_dc.sql_length =
              	sqlFieldLength;
          	insertI->detail_info.element_info.ddl_element_dc.host_length = SQL_INDICATOR_SIZE;
          	insertI->detail_info.element_info.ddl_element_dc.host_data_type = SQLLENGTHORINDICATOR;
          	if (previous != NULL)
              	previous->nItem = insertI;
          	current->pItem = insertI;
          	current->detail_info.element_info.ddl_element_dc.host_offset
              	+= SQL_INDICATOR_SIZE;
          	current->detail_info.element_info.ddl_element_dc.host_length
              	-= SQL_INDICATOR_SIZE;
          	previous = insertI;
      }

      /* SQL VARCHAR data type has a length field */
      if ( current->detail_info.element_info.ddl_element_dc.host_data_type
                  == SQLVARCHAR )  {
      		insertV = (DETAIL_LIST *) malloc (sizeof(DETAIL_LIST));
          	if (insertV == NULL)   {
          		printf("ddl2java - Fatal error: Unable to obtain memory.\n");
            	printf("ddl2java exits\n");
            	exit(EXIT_FAILURE);
          	}
   	  		num_elements++;

          	current->detail_info.element_info.ddl_element_dc.sql_length = sqlFieldLength;
          	memcpy (insertV, current, sizeof(DETAIL_LIST));
          	insertV->nItem = current;
          	insertV->detail_info.qualName.Name
              = makeName( current->detail_info.qualName.currNameLen
                                   + strlen (suffixLen) + 1);
          	if (insertV->detail_info.qualName.Name == NULL) {
            	printf("ddl2java - Fatal error: Unable to obtain memory.\n");
            	printf("ddl2java exits\n");
            	exit(EXIT_FAILURE);
          	}
          	strcpy (insertV->detail_info.qualName.Name, current->detail_info.qualName.Name);
          	strcat (insertV->detail_info.qualName.Name, suffixLen);
          	insertV->detail_info.qualName.currNameLen += strlen (suffixLen);
          	insertV->detail_info.element_info.ddl_element_dc.sql_length =
              	sqlFieldLength;
          	insertV->detail_info.element_info.ddl_element_dc.host_length = SQL_LEN_SIZE;
          	insertV->detail_info.element_info.ddl_element_dc.host_data_type = SQLLENGTHORINDICATOR;
          	if (previous != NULL)
              	previous->nItem = insertV;
          	current->pItem = insertV;
          	current->detail_info.element_info.ddl_element_dc.host_offset
              	+= SQL_LEN_SIZE;
          	current->detail_info.element_info.ddl_element_dc.host_length
              -= SQL_LEN_SIZE;
      }

      previous = current;
	  current = current->nItem;

	}  /* end while */

}
//---------------------------------------------------------
// Traverses the detail list looking for fields whose names
// need qualification.  When such a field is found, this
// routine calls createQualifiedName.  This routine also
// checks to see if the parent of the field occurs.
//-----------------------------------------------------------
void ClassGen::checkForQualifiedNames()
{
	DETAIL_LIST *element,*item;

	element = _detail_list;
   	while(element != NULL) {
		if(element->detail_info.qualified_name_required_flag == 'Y') {
   		   /* Create the qualfied name for this field */
   		   createQualifiedName(element,element->detail_info.element_info.ddl_element_dc.level);
   		}
		element = element->nItem;
	}
	element = _detail_list;
	while(element != NULL) {
		if(element->detail_info.element_info.ddl_element_dc.element_redefined > 0) {
         	element->detail_info.element_info.ddl_element_dc.element_redefined = 0;
		}
      	element->detail_info.element_info.ddl_element_dc.needs_discriminator = -1;
      	element->detail_info.element_info.ddl_element_dc.owns_discriminator = -1;
		element = element->nItem;
	}
	element = _detail_list;
	while(element != NULL) {
		if(element->detail_info.element_info.ddl_element_dc.level > 1) {
			checkParentOccurs(element);
		}
		element = element->nItem;
	}
}

//------------------------------------------------------------------------------
// checkParentOccurs
//
//		This routine checks for occurring parents for an elementary
//      field.
//------------------------------------------------------------------------------
void
ClassGen::checkParentOccurs(DETAIL_LIST *item)
{
	DETAIL_LIST *list_item;
   	int orig_level_no, lowest_level_no;
   	char occurs_flag = 'N';

   	orig_level_no = lowest_level_no = item->detail_info.element_info.ddl_element_dc.level;

   	list_item = item->pItem;

   	while(list_item != NULL) {
   		/* It's at the same level */
   		if(list_item->detail_info.element_info.ddl_element_dc.level == lowest_level_no) {
      		list_item = list_item->pItem;
         	continue;
      	}
      	/* It's a child of an item at the same level */
      	if(list_item->detail_info.element_info.ddl_element_dc.level > lowest_level_no) {
      		list_item = list_item->pItem;
         	continue;
      	}
      	/* It's a group item and also the highest parent of
      	   all possible parents */
      	if((list_item->detail_info.element_info.ddl_element_dc.level == 0) ||
      		(list_item->detail_info.element_info.ddl_element_dc.level == 1)) {
         	if(list_item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
         		item->detail_info.parent_occurs_flag = 'Y';
            	item->parent = list_item;
            	return;
         	} else {
         		item->parent = list_item;
            	item->detail_info.parent_occurs_flag = occurs_flag;
         		return;
         	}
      	}
      	/* It occurs more than once */
      	if(list_item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
      		occurs_flag = 'Y';
      	}
      	lowest_level_no = list_item->detail_info.element_info.ddl_element_dc.level;
      	list_item = list_item->pItem;
   	}
   	item->detail_info.parent_occurs_flag = 'N';
   	item->parent = NULL;
   	return;
}

//----------------------------------------------------------------------------------------
// createQualifiedName
//
//		Creates a qualified name by prepending parent names.
//----------------------------------------------------------------------------------------
void
ClassGen::createQualifiedName(DETAIL_LIST *item, int item_level_no)
{

	DETAIL_LIST *parent,*list_item;
   	list_item = item->pItem;
   	int current_level = item_level_no;
   	struct parents{
   		char parentNames[30];
   	} pNames[10];
   	int i =0,j;

   	for(i = 0; i < 10; i++) {
   		pNames[i].parentNames[0] = '\0';
   	}
   	i = 0;
   	parent = NULL;
   	while(list_item != NULL) {
   		// at the same level
   		if(list_item->detail_info.element_info.ddl_element_dc.level ==  current_level) {
        	list_item = list_item->pItem;
      	} else {
			// Highest level field in the def or record
      		if(list_item->detail_info.element_info.ddl_element_dc.level == 0) {
           		break;
         	}
         	if(list_item->detail_info.element_info.ddl_element_dc.level < current_level) {
         		parent = list_item;
            	strcpy(pNames[i].parentNames,parent->detail_info.element_info.ddl_element_dc.ddl_element_name);
				if(list_item->detail_info.qualified_name_required_flag == 'N')
					break;
           		current_level = list_item->detail_info.element_info.ddl_element_dc.level;
         		i++;
            	if(i == 10) {
            		printf("ddl2java - Error: Over 10 names required for qualification.\n");
               		printf("ddl2java - unable to complete generation.\n");
               		printf("ddl2java exits\n");
               		exit(EXIT_FAILURE);
            	}
         	}
         	list_item = list_item->pItem;
      	}
   }
   if(parent == NULL) {
   		strcpy(item->detail_info.qualName.Name,
      		item->detail_info.element_info.ddl_element_dc.ddl_element_name);
		return;
   }

   if(parent != NULL) {
   		if(item->detail_info.qualName.Name == NULL) {
      		item->detail_info.qualName.Name = makeName(NAMESIZE);
         	if(item->detail_info.qualName.Name == NULL) {
         		printf("ddl2java - Fatal error: Unable to obtain memory for qualified names.\n");
            	printf("ddl2java exits\n");
            	exit(EXIT_FAILURE);
         	}
         	item->detail_info.qualName.currNameLen = NAMESIZE;
      	}
      	memset(item->detail_info.qualName.Name,' ',item->detail_info.qualName.currNameLen);
      	item->detail_info.qualName.Name[0] = '\0';
      	current_level = parent->detail_info.element_info.ddl_element_dc.level;
      	j = i;
   	  	while(j >= 0) {
	  		if(pNames[j].parentNames[0] == NULL) {
				j--;
				continue;
		}
      	if((strlen(item->detail_info.qualName.Name) +
          strlen(pNames[j].parentNames)) > item->detail_info.qualName.currNameLen) {
     	  	item->detail_info.qualName.Name = (char *) realloc(item->detail_info.qualName.Name,
               	item->detail_info.qualName.currNameLen + NAMESIZE);
            if(item->detail_info.qualName.Name == NULL) {
               	printf("ddl2java - Fatal error: Unable to obtain memory for qualified names.\n");
           		printf("ddl2java exits\n");
           		exit(EXIT_FAILURE);
         	}
            item->detail_info.qualName.currNameLen = item->detail_info.qualName.currNameLen + NAMESIZE;
        }
        strcat(item->detail_info.qualName.Name,pNames[j].parentNames);
        strcat(item->detail_info.qualName.Name,"_");
        j--;
     }
     if((strlen(item->detail_info.qualName.Name) +
      	strlen(item->detail_info.element_info.ddl_element_dc.ddl_element_name)) >
     		item->detail_info.qualName.currNameLen) {
       	item->detail_info.qualName.Name = (char *) realloc(item->detail_info.qualName.Name,
       	item->detail_info.qualName.currNameLen + NAMESIZE);
       	if(item->detail_info.qualName.Name == NULL) {
       		printf("ddl2java - Fatal error: Unable to obtain memory for qualified names.\n");
        	printf("ddl2java exits\n");
        	exit(EXIT_FAILURE);
        }
        item->detail_info.qualName.currNameLen = item->detail_info.qualName.currNameLen + NAMESIZE;
     }
     strcat(item->detail_info.qualName.Name, item->detail_info.element_info.ddl_element_dc.ddl_element_name);
     item->detail_info.qualified_name_required_flag = 'Y';
  }

  return;
}
//-----------------------------------------------------------------------------------------
// getNextElement
//	If given NULL for a parameter, this routine returns the top of the list;
//  otherwise, it returns the next item in the list.
//-----------------------------------------------------------------------------------------
DETAIL_LIST *
ClassGen::getNextElement(DETAIL_LIST *element)
{
	if(element == NULL) {
   		return _detail_list;
   	}
   	return element->nItem;
}
//------------------------------------------------------------------------------
// getPrevElement
//		Returns the previous item in the list.
//------------------------------------------------------------------------------
DETAIL_LIST *
ClassGen::getPrevElement(DETAIL_LIST *element)
{
	if(element == NULL) {
   		return NULL;
   	}
   	return element->pItem;
}

//--------------------------------------------------------------------------
// openClassFile
//		Opens the .java file
//--------------------------------------------------------------------------
bool
ClassGen::openClassFile()
{
	char fileName[MAXPATHLEN +  42];
   	struct stat statBuf;

   	if(strcmp(sourcePath,".") != 0) {

   		if(stat(sourcePath, &statBuf) != 0) {
   			if(!mkdir(sourcePath, 0777)) {
      			return false;
      		}
   		} else {
   			if(!(statBuf.st_mode&S_IFDIR)) {
      			printf("ddl2java - Error: unable to write .java file. %s is not a directory.\n",
         		sourcePath);
         		return false;
      		}
   		}
      	sprintf(fileName,"%s/%s.java", sourcePath, className);
   	} else {
   		sprintf(fileName,"%s.java",className);
   	}

   	if(verbose)
   		printf("Creating .java file: %s.\n", fileName);

   	pfp = fopen(fileName,"w+t");
   	if(pfp == NULL) {
   		printf("ddl2java - Error: Unable to create file %s\n",fileName);
      	return false;
   	}
   	return true;
}

//--------------------------------------------------------------------------
// closeClassFile
//		Closes the .java file
//---------------------------------------------------------------------------
void
ClassGen::closeClassFile()
{
   fclose(pfp);
}
//--------------------------------------------------------------------------
// generateClassFile
//		Calls the routines necessary to generate the .java source file.
//--------------------------------------------------------------------------
bool
ClassGen::generateClassFile(classGenInfo *cgInfo)
{
   	DETAIL_LIST *element;
   	char tempClassName[31];
   	int i;
   	bool rtnValue;
   	bool isTsmp;

   	if(cgInfo->verbose)
   		verbose = true;

   	strcpy(encodingName, cgInfo->encodingName);
   	encodingSpecified = true;
   	if(strcmp(encodingName,".") == 0) {
		encodingSpecified = false;
   	}
   	checkForQualifiedNames();

   	expandSQLDataElements();

   	element = getNextElement(NULL);

   	if(verbose)
   		printf("Generating .java source file for %s.\n",
      		element->detail_info.qualName.Name);

   	upshiftFirstCharacter(cgInfo->ddlElName, tempClassName,
      	((int) strlen(cgInfo->ddlElName) + 1), (int) sizeof(tempClassName));

   	languageType = cgInfo->languageType;
   	charStringType = cgInfo->charStringType;

   	strcpy(packageName, cgInfo->packageName);
   	for(i = 0; i < MAXPATHLEN; i++) {
	   if(packageName[i] != ' ') {
      		continue;
      	} else {
      		packageName[i] = '\0';
         	break;
      	}
   	}
   	strcpy(sourcePath, cgInfo->sourcePath);

   	switch (cgInfo->generatedClassType) {
	case CLASSRQ:
	case CLASSBOTH:
		isTsmp = true;
		sprintf(className,"RQ%s",tempClassName);
   		rtnValue = processRequest(cgInfo->generatedClassType,
   			verbose, cgInfo->compile, isTsmp);
   		if(cgInfo->generatedClassType == CLASSRQ) {
			break;
		}
	case CLASSRP:
		isTsmp = true;
   		sprintf(className,"RP%s",tempClassName);
   		rtnValue = processRequest(CLASSRP,
   			verbose, cgInfo->compile, isTsmp);
   		break;
    case INPUT:
    case IO:
    	isTsmp = false;
       	if(cgInfo->generatedClassType == INPUT)
    		sprintf(className,"I_%s",tempClassName);
    	else
    		sprintf(className,"IO_%s",tempClassName);
    	rtnValue = processRequest(cgInfo->generatedClassType,
    		verbose, cgInfo->compile, isTsmp);
   		break;
	case OUTPUT:
		isTsmp = false;
		sprintf(className,"O_%s",tempClassName);
		rtnValue = processRequest(cgInfo->generatedClassType,
			verbose, cgInfo->compile, isTsmp);
		break;
	}
   	return rtnValue;

}

bool ClassGen::processRequest(int classType, bool verbose,
			bool compile, bool isTsmp)
{
	char fileName[MAXPATHLEN +  42];
	char buf[MAXPATHLEN];
	int tempclassType;

  	if(!openClassFile()) {
   		return false;
  	}
  	writeImports(pfp, isTsmp, packageName);

  	if(verbose)
   		printf("Generating general class information ...\n");

  	if(!writeClassHdr(isTsmp,classType)) {
   		printf("ddl2java - Error: Can't write request class header\n");
      	return false;
   	}
    if(verbose)
      	printf("Generating instance variables ...\n");

    if(!writeInstanceVars(classType)) {
     	printf("ddl2java - Error: Can't write instance variables for %s\n", className);
    }

    if(verbose)
      	printf("Generating set methods ... \n");

    if(classType == CLASSRQ || classType == OUTPUT || classType == IO) {
    	methodType = SET_METHOD;
    	if(!writeMethods(classType)) {
   			printf("ddl2java - Error: Can't write set methods for class %s\n", className);
      		return false;
   		}
	}
   	if(verbose)
   		printf("Generating clear methods ... \n");

   	if(classType == CLASSRQ || classType == OUTPUT || classType == IO || classType == INPUT ||
   		classType == CLASSRP) {
    	methodType = CLEAR_METHOD;
   		if(!writeMethods(classType)) {
   			printf("ddl2java - Error: Can't write clear methods for class %s\n", className);
      		return false;
   		}
   		methodType = CLEAR_ALL;
		fprintf(pfp,"/** Used to clear the values of all instance variables in this class.\n*/\n");
		fprintf(pfp,"  public void clear_All() {\n");
   		writeDoAllText(classType);
		fprintf(pfp,"  }\n");
	}

	if(verbose)
	   	printf("Generating get methods ... \n");

	if(classType == CLASSRP || classType == INPUT || classType == IO) {
    	methodType = GET_METHOD;
    	if(!writeMethods(classType)) {
			printf("ddl2java - Error: Can't write get methods for class %s\n", className);
			return false;
		}
	}

	if(classType == IO || classType == CLASSBOTH) {
		if(classType == CLASSBOTH)
			tempclassType = CLASSRQ;
		else
			tempclassType = INPUT;
	   	if(!writeLoadUnLoad(tempclassType)) {
   			printf("ddl2java - Error: Can't write load request for class %s\n", className);
      		return false;
   		}

   		if(classType == CLASSBOTH) {
   			tempclassType = CLASSRP;
   		} else {
   			tempclassType = OUTPUT;
		}
   		if(!writeLoadUnLoad(tempclassType)) {
			printf("ddl2java - Error: Can't write load request for class %s\n", className);
			return false;
   		}
   		fprintf(pfp,"  }\n");
	} else {
		if(!writeLoadUnLoad(classType)) {
				printf("ddl2java - Error: Can't write load request for class %s\n", className);
				return false;
   		}
   		fprintf(pfp,"  }\n");
	}

    closeClassFile();
    // Compile class
   	if(compile) {
   		if(strcmp(sourcePath,".") != 0) {
    		sprintf(fileName,"%s/%s.java", sourcePath, className);
   		} else {
   			sprintf(fileName,"%s.java",className);
   		}
   		printf("ddl2java - compiling %s...\n",fileName);
    	sprintf(buf,"javac -d . %s", fileName);
    	system(buf);
   	}
   	return true;
}
//---------------------------------------------------------------------------
//  writeClassHdr
// 	Writes the header information for the generated classes
//---------------------------------------------------------------------------
bool
ClassGen:: writeClassHdr(bool isTsmp, int classType)
{
	DETAIL_LIST *element;
   	char serviceClassName[100];
   	struct tm *localtimep;
   	time_t now;
   	int length;

   	if(isTsmp) {
	   	if(classType == CLASSRQ) {
   			strcpy(serviceClassName, "TsmpServerRequest");
   		} else {
   			strcpy(serviceClassName, "TsmpServerReply");
   		}
	}

   	element = getNextElement(NULL);

   	assert(element->detail_info.element_info.ddl_element_dc.level == 0);

   	// Write JavaDoc general class info
   	now = time(NULL);
   	localtimep = localtime(&now);
   	fprintf(pfp,"/**\n%s class was generated for DDL Rec/Def: %s on: %d/%d/%2d at: %d:%d:%d.\n",
     	className, element->detail_info.qualName.Name, (localtimep->tm_mon + 1), localtimep->tm_mday,
       localtimep->tm_year % 100, localtimep->tm_hour, localtimep->tm_min,
       localtimep->tm_sec);

    if(isTsmp) {
   		if(classType == CLASSRQ) {
     		fprintf(pfp,"A <CODE>TsmpServerRequest</CODE> is \n");
      		fprintf(pfp,"  passed to  a <CODE>TsmpServer</CODE> which, in turn, is used to send a service request\n");
      		fprintf(pfp,"  to a TS/MP service.*/\n");
   		} else {
     		fprintf(pfp,"A<CODE>TsmpServerReply</CODE> is used to receive the reply from the TS/MP service.*/\n");
   		}
   		fprintf(pfp,"\n");
   		fprintf(pfp,"  public class %s extends %s {\n\n", className, serviceClassName);
   		if(classType == CLASSRQ) {
     		fprintf(pfp,"\n    public byte _requestBuffer [] = new byte [%d];\n",
        		element->detail_info.element_info.ddl_element_dc.host_length);
   		} else {
     		length = element->detail_info.element_info.ddl_element_dc.host_length + 1;
     		fprintf(pfp,"    public int _replyLength = %d;\n",length);
			fprintf(pfp,"    public Hashtable replyDataTypes = new Hashtable(%d);\n",
				num_elements);
   		}
   		return true;
	}

	if(classType == IO) {
		fprintf(pfp,"This is an INPUT/OUTPUT class which can be used to read and write data.*/\n");
		fprintf(pfp,"\n");
		fprintf(pfp,"  public class %s implements com.tandem.ext.guardian.GuardianInput, \n", className);
		fprintf(pfp,"            com.tandem.ext.guardian.GuardianOutput {\n");
	} else if (classType == INPUT) {
		fprintf(pfp,"This is an INPUT class which is typically used to read data.*/\n");
		fprintf(pfp,"\n");
		fprintf(pfp,"  public class %s implements com.tandem.ext.guardian.GuardianInput {\n", className);
	} else if (classType == OUTPUT) {
		fprintf(pfp,"This is an OUTPUT class which is typically used to write data.*/\n");
		fprintf(pfp,"\n");
		fprintf(pfp,"  public class %s implements com.tandem.ext.guardian.GuardianOutput {\n", className);
	}



	if(classType == IO || classType == INPUT) {
		fprintf(pfp,"    protected int _numConversionErrors;\n");
		fprintf(pfp,"    protected boolean _allowErrors = false;\n");
		fprintf(pfp,"    protected HashMap _errorDetail;\n");
    }

   if(classType == IO || classType == OUTPUT || classType == INPUT) {
		fprintf(pfp,"\n    protected int _bufferLen = %d;\n",
				element->detail_info.element_info.ddl_element_dc.host_length);
   }
   return true;
}

//--------------------------------------------------------------------------
// writeInstanceVars
// 	Uses the information in the linked list to write instance variables for
// 	each element in the DDL def or record.
//--------------------------------------------------------------------------
bool ClassGen::writeInstanceVars(int classType)
{
	char upShiftName[1026];
   	DETAIL_LIST *element;
   	bool needSubClasses = false;
   	int i;
	int dataType;

   	wroteConstructor = false;

   	element = getNextElement(NULL);
   	if((element->detail_info.element_info.ddl_element_dc.level == 0) &&
   		(element->detail_info.element_info.ddl_element_dc.host_data_type == 0)){
   		// get past the def/record name stuff
   		element = getNextElement(element);
   	}

   	while(element != NULL) {
		dataType = element->detail_info.element_info.ddl_element_dc.host_data_type;
    	if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
      		(element->detail_info.element_info.ddl_element_dc.level != 0)) {
      		// group name
         	if(element->detail_info.element_info.ddl_element_dc.occurs_max <= 1) {
         		element = getNextElement(element);
      			continue;
         	} else {
         		foundOccurs = 1;
         	}
      	}// end if element_type == 0 and level !=0

      	// 89-level's are unique, not related to other elements, so handle here.
      	if (element->detail_info.element_info.ddl_element_dc.level == 89) {
			if(classType == INPUT || classType == OUTPUT || classType == IO) {
        		fprintf(pfp,"    public static short %s = (short) %s;\n",
                    element->detail_info.qualName.Name,
                    element->detail_info.element_info.ddl_value_text);
			} else {
				fprintf(pfp,"    public static String %s = Short.toString((short)%s);\n",
					 element->detail_info.qualName.Name,
                    	element->detail_info.element_info.ddl_value_text);
			}
            element = getNextElement(element);
      		continue;
      	}

      	if(element->detail_info.parent_occurs_flag == 'N') {
        	if(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
           		if(element->detail_info.element_info.ddl_element_dc.element_type == 0) {
           			upshiftFirstCharacter(element->detail_info.qualName.Name, upShiftName,
              			(int)strlen(element->detail_info.qualName.Name) + 1, (int) sizeof(upShiftName));
        			fprintf(pfp,"    %s_class %s[] = {", upShiftName, element->detail_info.qualName.Name);
               		for(i = 0; i <= (element->detail_info.element_info.ddl_element_dc.occurs_max - 2); i++)
               		{
                    	fprintf(pfp,"new %s_class(),\n",upShiftName);
               		}
               		fprintf(pfp,"new %s_class()};\n",upShiftName);

               		foundOccurs = 1;
               		needSubClasses = true;
            	} else {
					if(classType == INPUT || classType == OUTPUT || classType == IO) {
						writeOccursNumericInstanceVar(dataType,element);
					} else {
						fprintf(pfp,"    public String %s[] = {", element->detail_info.qualName.Name);
						for(i = 0; i <= (element->detail_info.element_info.ddl_element_dc.occurs_max - 2); i ++)
						{
							fprintf(pfp,"new String(),\n");
						}
                  		fprintf(pfp,"new String()};\n");
					}
           		}
        	} else {
				if(classType == INPUT || classType == OUTPUT || classType == IO) {
					writeElemNumericInstanceVar(dataType,element);
				} else {
					fprintf(pfp,"    public String %s;\n",
					   	element->detail_info.qualName.Name);
        		}
			}
     	}
     	element = getNextElement(element);
   	}
   	if(needSubClasses) {
   		element = getNextElement(NULL);
      	while(element != NULL) {
      		if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
      			(element->detail_info.parent_occurs_flag == 'N') &&
            	(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) &&
            	(element->detail_info.element_info.ddl_element_dc.level != 0)) {
           		writeSubClass(element, classType);
      		}
      		element = getNextElement(element);
      	}
   	}

   	if(wroteConstructor == false) {
   		writeClassConstructor(classType);
   		wroteConstructor = true;
   	}
   	if(needSubClasses) {
   		element = getNextElement(NULL);
		while(element != NULL) {
			if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
				(element->detail_info.parent_occurs_flag == 'N') &&
		     	(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) &&
            	(element->detail_info.element_info.ddl_element_dc.level != 0)) {
				if(element->detail_info.element_info.ddl_element_dc.hasBeenInitialized == UNINITIALIZED) {
					initializeSubClasses(element, 0, 0, classType);
					element->detail_info.element_info.ddl_element_dc.hasBeenInitialized = SUBCLASSINITIALIZED;
				}
			}
	    	element = getNextElement(element);
		}
   	}
   	if(wroteConstructor) {
   		if(classType == CLASSRP)
   			writeHashTable();
   		fprintf(pfp,"   }\n");
   		if(classType == INPUT || classType == OUTPUT || classType == IO) {
			writeSpecialIOMethods(pfp, classType);
		}

   	}
   	return true;
}
//-----------------------------------------------------------------------------------
// writeClassConstructor
// writes the beginning part of the constructor for the class.
//-------------------------------------------------------------------------------
void ClassGen::writeClassConstructor(int classType)
{
	DETAIL_LIST *element;
	int length;

	element = getNextElement(NULL);

	assert(element->detail_info.element_info.ddl_element_dc.level == 0);
	fprintf(pfp,"\n");
	fprintf(pfp,"/**\n%s Constructor\n*/\n", className);
	fprintf(pfp,"   public %s() {\n",className);
	if(classType == CLASSRP) {
   	  	length = element->detail_info.element_info.ddl_element_dc.host_length + 1;
    	fprintf(pfp,"        setReplyLength(%d);\n",length);
   	}
   	if(classType == INPUT || classType == IO) {
		fprintf(pfp,"        _errorDetail = new HashMap();\n");
	}
	return;
}
//-------------------------------------------------------------------------
// writeHashTable
//-------------------------------------------------------------------------
void ClassGen::writeHashTable()
{
	DETAIL_LIST *element;
	int length;

	element = getNextElement(NULL);
	if(element->detail_info.element_info.ddl_element_dc.level == 0)
		element = getNextElement(element);

	while(element != NULL) {
	  	if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
	       	(element->detail_info.element_info.ddl_element_dc.level != 0)) {
	       	// group name
	        element = getNextElement(element);
	       	continue;
	     } else {
     		fprintf(pfp,
                "        replyDataTypes.put(\"%s\",new String(\"%d,%d\"));\n",
				element->detail_info.qualName.Name,
                (element->detail_info.element_info.ddl_element_dc.host_data_type + 1),
                element->detail_info.element_info.ddl_element_dc.host_length);
		}
		element = getNextElement(element);
	}
	return;
}
//--------------------------------------------------------------------------
// initializeSubClasses
// 	initializes the variables in subclasses
//--------------------------------------------------------------------------
void
ClassGen::initializeSubClasses(DETAIL_LIST *item, int indentLevel, int iternum,
	int classType)
{
    DETAIL_LIST *element, *childElement;
   	char itemName[8 * 1024];
    int i, next_level, j, k, n, numParents;
    PLENGTH_LIST parentLengthList;
    static char *iterator [] = { "_i0", "_i1", "_i2", "_i3", "_i4", "_i5",
      									  "_i6"};
    static char *indent [] = {"      ",
      						  "        ",
                              "          ",
                              "            ",
                              "              ",
                              "                ",
                              "                  ",
                              "                    ",
                              "                      ",
                              "                        ",
                              "                          ",
                              "                            "};

    i = indentLevel;
   	n = iternum;
	int dataType;

	if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
	   	writeInitializeForLoop(item, indent[i], iterator[n]);
		i++;
		n++;
    }

   	element = getNextElement(item);
   	next_level = element->detail_info.element_info.ddl_element_dc.level;


   	while(element!= NULL) {
		dataType = element->detail_info.element_info.ddl_element_dc.host_data_type;
   		if((element != item) &&
      		(element->detail_info.element_info.ddl_element_dc.level ==
         		item->detail_info.element_info.ddl_element_dc.level)) {
         	break;
      	}
	  	if(element->detail_info.element_info.ddl_element_dc.level == next_level) {
	    // Elementary item
	    	if(element->detail_info.element_info.ddl_element_dc.element_type == 1) {
	        	if(element->detail_info.element_info.ddl_element_dc.occurs_max == 1) {
            		// doesn't occur
               		memset(itemName,'\0',sizeof(itemName));
            		numParents = getParentName(element, itemName, &parentLengthList);
					if(numParents == 7) numParents --;
            		if(numParents == -1) {
               			return;
            		} else {
						if(classType == INPUT || classType == OUTPUT || classType == IO) {
						// dataType is not numeric
							switch(dataType) {
							case BINARY_16_SIGNED:
							case SQLLENGTHORINDICATOR:
							case ENUMERATION:
							case BINARY_16_UNSIGNED:
							case BINARY_32_SIGNED:
							case BINARY_32_UNSIGNED:
							case BINARY_64_SIGNED:
							case FLOAT32:
							case LOGICAL_1:
							case LOGICAL_2:
							case LOGICAL_4:
							case BITSIGNED:
							case BITUNSIGNED:
								break;
								/*For Soln: 10-120828-4530 */
							case NUM_STR_UNSIGNED:
								fprintf(pfp,"%s%s = \"0\";\n",indent[i], itemName);
								break;
								/*For Soln: 10-120828-4530 */
							default:
							    /*changes Begin for the sol 10-120723-4191*/
								switch(stringInitializationType) { 
									case NULL_TYPE:
										fprintf(pfp,"%s%s = null;\n",indent[i], itemName);
										break;
									case SPACE_TYPE:
										fprintf(pfp,"%s%s = \" \";\n",indent[i], itemName);
										break;
							  }
							  /*changes end for the sol 10-120723-4191*/
							}
						} else {
                       		fprintf(pfp,"%s%s = null;\n",indent[i], itemName);
						}
						element->detail_info.element_info.ddl_element_dc.hasBeenInitialized++;
               		}
         	} else {
         		// Elementary item that occurs
               writeInitializeForLoop(element, indent[i], iterator[n]);
               memset(itemName,'\0',sizeof(itemName));
               numParents = getParentName(element,itemName, &parentLengthList);
               if(numParents == -1 ) {
               		return;
               } else {
			   		if(classType == INPUT || classType == OUTPUT || classType == IO) {
						// dataType is not numeric
						switch(dataType) {
						case BINARY_16_SIGNED:
						case SQLLENGTHORINDICATOR:
						case ENUMERATION:
						case BINARY_16_UNSIGNED:
						case BINARY_32_SIGNED:
						case BINARY_32_UNSIGNED:
						case BINARY_64_SIGNED:
						case FLOAT32:
						case LOGICAL_1:
						case LOGICAL_2:
						case LOGICAL_4:
						case BITSIGNED:
						case BITUNSIGNED:
							break;
						/*For Soln: 10-120828-4530 */
						case NUM_STR_UNSIGNED:
								fprintf(pfp,"%s%s = \"0\";\n",indent[i+1], itemName);
								break;
						/*For Soln: 10-120828-4530 */
						default:
						    /*changes Begin for the sol 10-120723-4191*/
							switch(stringInitializationType) { 
									case NULL_TYPE:
										fprintf(pfp,"%s%s = null;\n",indent[i+1], itemName);
										break;
									case SPACE_TYPE:
										fprintf(pfp,"%s%s = \" \";\n",indent[i+1], itemName);
										break;
							}
							/*changes end for the sol 10-120723-4191*/
						}
					} else {
                   		fprintf(pfp,"%s%s = null; \n",indent[i + 1], itemName);
					}
					element->detail_info.element_info.ddl_element_dc.hasBeenInitialized++;
               }
               fprintf(pfp,"%s}\n",indent[i]);
            }
            element = getNextElement(element);
            continue;
         }
         // Group item
         (void)initializeSubClasses(element, i, n, classType);
         element->detail_info.element_info.ddl_element_dc.hasBeenInitialized++;
         childElement = element->nItem;
         while(childElement->detail_info.element_info.ddl_element_dc.level > next_level) {
         	childElement = childElement->nItem;
            if(childElement == NULL) {
            	break;
            }
         }
         element = childElement;
         continue;
      }
      element = getNextElement(element);
   }
   if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
   		fprintf(pfp,"%s}\n",indent[indentLevel]);
   }
   return;
}

//--------------------------------------------------------------------------
// writeSubClass
// 	writes inner classes for elements that occur in the DDL.  This routine
//  calls itself recursively until the intance variables for all elements
//  within an occurring group have been generated.
//--------------------------------------------------------------------------
void
ClassGen::writeSubClass(DETAIL_LIST *item, int classType)
{
	DETAIL_LIST *element;
   	char upShiftName[1026];
   	int next_level,current_level, j;
   	int hasSubClasses = 0;
   	char *C_indent[] = 	{" ",
   						 "    ",
                         "      ",
                         "        ",
                         "          ",
                         "            ",
                         "              ",
                         "                ",
                         "                  ",
                         "                    ",
                         "                      ",
                         "                        ",
                         "                          "};

   	char *indent;
	int dataType;

   	current_level = item->detail_info.element_info.ddl_element_dc.level;
   	next_level = item->detail_info.element_info.ddl_element_dc.level + 1;

   	upshiftFirstCharacter(item->detail_info.qualName.Name, upShiftName,
   		(int) strlen(item->detail_info.qualName.Name)+ 1,
               	(int) sizeof(upShiftName));

   	indent = C_indent[current_level];
   	fprintf(pfp,"%sclass %s_class {\n",indent, upShiftName);

   	indent = C_indent[next_level];
   	element = item->nItem;

   	while(element != NULL) {
		dataType = element->detail_info.element_info.ddl_element_dc.host_data_type;
   		if((element != item) &&
         	(element->detail_info.element_info.ddl_element_dc.level ==
         	 item->detail_info.element_info.ddl_element_dc.level)) {
        	break;
      	}

      	if(element->detail_info.element_info.ddl_element_dc.level == next_level){
      		// Elementary item
      		if((element->detail_info.element_info.ddl_element_dc.occurs_max <= 1) &&
         		(element->detail_info.element_info.ddl_element_dc.element_type != 0)) {
				if(classType == INPUT || classType == OUTPUT || classType == IO)
				{
					/* Added for the solution 10-120524-3305*/
					fprintf(pfp,"%s",indent);
					/** changes end **/
					writeElemNumericInstanceVar(dataType, element);
				}
				else
         			fprintf(pfp,"%sString %s;\n",indent, element->detail_info.qualName.Name);
         	}
         	// Elementary item that occurs
         	if((element->detail_info.element_info.ddl_element_dc.element_type != 0) &&
         		(element->detail_info.element_info.ddl_element_dc.occurs_max > 1)) {
				if(classType == INPUT || classType == OUTPUT || classType == IO) {
					writeOccursNumericInstanceVar(dataType,element);
				} else {
					fprintf(pfp,"%sString %s[] = {", indent, element->detail_info.qualName.Name);
					for(j = 0; j < element->detail_info.element_info.ddl_element_dc.occurs_max; j++){
						if(j <= (element->detail_info.element_info.ddl_element_dc.occurs_max - 2)) {
					       	fprintf(pfp,"new String(),\n");
					    } else {
					    	fprintf(pfp,"new String()};\n");
					    }
               		}
				}
         	}
         	// Group item that occurs
         	if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
           		(element->detail_info.element_info.ddl_element_dc.occurs_max > 1)) {
            	upshiftFirstCharacter(element->detail_info.qualName.Name, upShiftName,
               	 	(int) strlen(element->detail_info.qualName.Name) + 1,
               		(int) sizeof(upShiftName));
              	fprintf(pfp,"%s%s_class %s[] = {new %s_class(),\n",indent, upShiftName, element->detail_info.qualName.Name,
               				upShiftName);
               	for(j = 0; j < (element->detail_info.element_info.ddl_element_dc.occurs_max -1); j++) {
               		if(j == (element->detail_info.element_info.ddl_element_dc.occurs_max - 2)) {
               			fprintf(pfp,"%s%snew %s_class()};\n",indent,indent,upShiftName);
                  	} else {
                  		fprintf(pfp,"%s%snew %s_class(),\n",indent,indent,upShiftName);
                  	}
               	}
               	hasSubClasses = 1;
         	}
         	// Group item that doesn't occur
         	if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
         		(element->detail_info.element_info.ddl_element_dc.occurs_max <= 1)) {
            	upshiftFirstCharacter(element->detail_info.qualName.Name, upShiftName,
               	 	(int) strlen(element->detail_info.qualName.Name) + 1,
               		(int) sizeof(upShiftName));
              	fprintf(pfp,"%s%s_class %s = new %s_class();\n",indent, upShiftName,
               		element->detail_info.qualName.Name, upShiftName);
			
			/*Added for solution 10-120524-3305*/
			hasSubClasses = 1;
         	}
			/* Commented for solution 10-120524-3305*/
         	/*hasSubClasses = 1;*/  
			/**** changes end ***/
      	}
      	element = getNextElement(element);
   	}

   	if(hasSubClasses == 1) {
   		element = item->nItem;
      	// find the  inner subClasses and print them
      	while(element != NULL) {
      		if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
         		(element->detail_info.element_info.ddl_element_dc.level == next_level)) {
             	writeSubClass(element, classType);
         	}
			
		/*************Added for solution 10-120524-3305 *********/
		if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
         	(element->detail_info.element_info.ddl_element_dc.level <= current_level)) {
		        break;
		}
		/*** changes end ***/
		
         	element = getNextElement(element);
      	}
   	}
   	indent = C_indent[current_level];
   	fprintf(pfp,"%s}\n",indent);

   	return;
}
//--------------------------------------------------------------------------
// writeMethods
//		This is the main routine which writes the set, get, and clear methods
//    for each generated class.
//--------------------------------------------------------------------------
bool
ClassGen::writeMethods(int classType)
{
	int length;
	DETAIL_LIST *element;
   	char *indent = "    ";
   	bool writeConvertLong = false;
   	bool writeConvertShort = false;
   	bool writeConvertInt = false;

	fprintf(pfp,"\n");

   	element = getNextElement(NULL);

   	if((classType == CLASSRP) && (!wroteConstructor)) {
   		fprintf(pfp,"%spublic %s() {\n", indent, className);
      	length = element->detail_info.element_info.ddl_element_dc.host_length + 1;
      	fprintf(pfp,"%s  setReplyLength(%d);\n", indent,length);
      	fprintf(pfp,"%s}\n", indent);
      	wroteConstructor = true;
   	}
   	if(methodType == GET_METHOD) {
		if(classType == CLASSRP) {
	 		fprintf(pfp,"/**\nConstruct a new TsmpServerReply by converting the reply buffer of the specified TsmpGenericServer.\n*/\n");
	 		fprintf(pfp,"%spublic %s(TsmpGenericServerReply g) throws TsmpReplyException {\n",indent, className);
	   		fprintf(pfp,"%s  byte[] _replyBuffer = g.getBuffer();\n", indent);
	   		fprintf(pfp,"%s  setBuffer(_replyBuffer);\n", indent);
	   		fprintf(pfp,"%s  unmarshal();\n", indent);
	   		fprintf(pfp,"%s}\n\n",indent);
	 	}
	}

   	while(element != NULL) {
   		// Group doesn't need a method, neither does 89-level..
      	if(((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
      		(element->detail_info.element_info.ddl_element_dc.level != 0))
         	|| (element->detail_info.element_info.ddl_element_dc.level == 89)){
         	element = getNextElement(element);
      		continue;
      	}
      	if(classType == CLASSRP || classType == INPUT || classType == IO) {
      		if((element->detail_info.element_info.ddl_element_dc.host_data_type == BINARY_16_SIGNED) &&
      			(element->detail_info.element_info.ddl_element_dc.host_scale != 0)) {
      			writeConvertShort = true;
   			} else if(((element->detail_info.element_info.ddl_element_dc.host_data_type == BINARY_16_UNSIGNED) ||
      			(element->detail_info.element_info.ddl_element_dc.host_data_type == BINARY_32_SIGNED)) &&
      			(element->detail_info.element_info.ddl_element_dc.host_scale != 0)) {
      			writeConvertInt = true;
   			} else if (((element->detail_info.element_info.ddl_element_dc.host_data_type == BINARY_32_UNSIGNED) ||
      			(element->detail_info.element_info.ddl_element_dc.host_data_type == BINARY_64_SIGNED)) &&
      			(element->detail_info.element_info.ddl_element_dc.host_scale != 0)) {
      			writeConvertLong = true;
   			}
      	}
      	// Class (Record or Def Name) RP Constructor
   		if(element->detail_info.element_info.ddl_element_dc.level == 0) {
           	if(element->detail_info.element_info.ddl_element_dc.host_data_type != 0) {
        			writeMethodText(element, 1, NULL, classType);
        	}
      	} else {

         	if(element->detail_info.parent_occurs_flag == 'N') {
         		writeMethodText(element, 1, NULL, classType);
         	} else {
         		writeMethodOccurs(element, classType);
         	}
      	}
      	element = getNextElement(element);
   	}
   	if(classType == CLASSRP && methodType != CLEAR_METHOD) {
   		if(writeConvertShort) writeNumericConvertMethod(indent, BINARY_16_SIGNED, pfp);
   		if(writeConvertInt) writeNumericConvertMethod(indent, BINARY_32_SIGNED,pfp);
   		if(writeConvertLong) writeNumericConvertMethod(indent, BINARY_64_SIGNED,pfp);
    }
    return true;
}
//------------------------------------------------------------------
// writeDoAllText
//
void ClassGen::writeDoAllText(int classType)
{
	DETAIL_LIST *element;
	char *indent = "      ";
	char initString[10];
	int dataType;

	element = getNextElement(NULL);
	while(element != NULL) {
		dataType = element->detail_info.element_info.ddl_element_dc.host_data_type;
		if(dataType == ENUMERATION) {
			element = getNextElement(element);
			continue;
		}
		if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
			(element->detail_info.parent_occurs_flag == 'N') &&
	     	(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) &&
	       	(element->detail_info.element_info.ddl_element_dc.level != 0)) {
			if(element->detail_info.element_info.ddl_element_dc.hasBeenInitialized == SUBCLASSINITIALIZED) {
				initializeSubClasses(element, 0, 0, classType);
			}
		} else {
			if((element->detail_info.element_info.ddl_element_dc.level != 0) &&
				(element->detail_info.element_info.ddl_element_dc.hasBeenInitialized != CLEARALLINITIALIZED)) {
				if(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
			      	/* Occurring item */
				   	fprintf(pfp,"%sfor(int _i = 0; _i < %d; _i++) {\n",
				           	indent,element->detail_info.element_info.ddl_element_dc.occurs_max);
				    if(classType == INPUT || classType == OUTPUT ||
				    	classType == IO) {
							writeInitialValue(dataType,initString, element);
				   	} else {
						strcpy(initString,"null");
					}
					fprintf(pfp,"%s  %s[_i] = %s;\n",
						indent, element->detail_info.qualName.Name,
				       		initString);
				   	fprintf(pfp,"%s}\n",indent);
		        } else  {
					if((element->detail_info.element_info.ddl_element_dc.element_type != 0) &&
					  (element->detail_info.element_info.ddl_element_dc.hasBeenInitialized < CLEARALLINITIALIZED)) {
			     		/* Single item (no occurs) */
			     		if(classType == INPUT || classType == OUTPUT ||
						    	classType == IO) {
							writeInitialValue(dataType,initString, element);
					   	} else {
							strcpy(initString,"null");
						}
			        	fprintf(pfp,"%s%s = %s;\n",indent,
			        		element->detail_info.qualName.Name, initString);
					}
	        	}
			}
		}
	   	element = getNextElement(element);
	}
}


//--------------------------------------------------------------------------
// writeMethodOccurs
//  	Determines the text to be used for the set, get, clear method names
//      for ocurring fields and writes it to the .java file.
//--------------------------------------------------------------------------
void
ClassGen::writeMethodOccurs(DETAIL_LIST *item, int classType)
{
	PNAME_LIST *parentNameList;
   	char occursName[7 * 1025];
   	char numStr[3];
   	int i, j, k, indexes;

   	parentNameList = (PNAME_LIST *) malloc(sizeof(PNAME_LIST));
   	if(parentNameList == NULL) {
   		return;
   	}
   	for(j = 0; j < PLISTSIZE; j++) {
     	parentNameList->pList[j].parentName.Name = makeName(NAMESIZE);
      	if(parentNameList->pList[j].parentName.Name == NULL) {
        	printf("ddl2java - Fatal error: Unable to obtain memory for parent name list.\n");
         	printf("ddl2java exits.\n");
         	exit(EXIT_FAILURE);
      	}
      	parentNameList->pList[j].parentName.currNameLen = NAMESIZE;
	  	memset(parentNameList->pList[j].parentName.Name,'\0',NAMESIZE);
	  	parentNameList->pList[j].numParentOccurs = 0;
	  	parentNameList->pList[j].parentDataLen = 0;
	  	parentNameList->pList[j].offset = -1;
   	}

   	i = getParentNameList(item,parentNameList);
   	assert(parentNameList->pList[0].parentName.currNameLen != 0);
   	k = 0;
   	memset(occursName,'\0', sizeof(occursName));
   	indexes = 0;
   	for(j = i; j >= 0; j --) {
   		if(*parentNameList->pList[j].parentName.Name == '\0') {
      		continue;
      	}
      	if(occursName[0] != '\0') {
      		strcat(occursName,".");
   		}
      	strcat(occursName,parentNameList->pList[j].parentName.Name);
      	if(parentNameList->pList[j].numParentOccurs > 1) {
      		strcat(occursName,"[");
      		strcat(occursName,"_i");
      		sprintf(numStr,"%d",k);
      		strcat(occursName,numStr);
      		k++;
      		strcat(occursName,"]");
         	indexes++;
      	}
   	}
   	strcat(occursName,".");
   	strcat(occursName,item->detail_info.qualName.Name);
   	if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
   		strcat(occursName,"[");
      	strcat(occursName,"_i");
     	sprintf(numStr,"%d",k);
      	strcat(occursName,numStr);
      	strcat(occursName,"]");
      	indexes++;
   	}


   	writeMethodText(item, indexes, occursName, classType);
   	for(j = 0; j < PLISTSIZE; j++) {
   		free(parentNameList->pList[j].parentName.Name);
   	}
   	free(parentNameList);
   	return;
}
//--------------------------------------------------------------------------
// getParentNameList
//		Gets a list of parent names for the DDL item.
//--------------------------------------------------------------------------
int
ClassGen::getParentNameList(DETAIL_LIST *item, PNAME_LIST *parentNameList)
{
	DETAIL_LIST *element;
	int foundOccursItem = 0;
   	int level_no, i;
	i = 0;

   	element = getPrevElement(item);
   	level_no = item->detail_info.element_info.ddl_element_dc.level;

   	while(element != NULL) {
   		if(element->detail_info.element_info.ddl_element_dc.level == 1) {
      		if(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
            	if(parentNameList->pList[i].parentName.currNameLen < strlen(element->detail_info.qualName.Name)) {
            		parentNameList->pList[i].parentName.Name = (char *)realloc(parentNameList->pList[i].parentName.Name,
               		parentNameList->pList[i].parentName.currNameLen + strlen(element->detail_info.qualName.Name));
               	if(parentNameList->pList[i].parentName.Name == NULL) {
               		printf("ddl2java - Fatal error: Unable to obtain memory for parent names.\n");
                  	printf("ddl2java exits.\n");
                  	exit(EXIT_FAILURE);
               	}
               	parentNameList->pList[i].parentName.currNameLen = parentNameList->pList[i].parentName.currNameLen +
               	strlen(element->detail_info.qualName.Name);
            }
            strcpy((char *)parentNameList->pList[i].parentName.Name,element->detail_info.qualName.Name);
            parentNameList->pList[i].numParentOccurs = element->detail_info.element_info.ddl_element_dc.occurs_max;
            parentNameList->pList[i].parentDataLen = element->detail_info.element_info.ddl_element_dc.host_length;
            parentNameList->pList[i].offset = element->detail_info.element_info.ddl_element_dc.host_offset;
         }
         break;
	}
    if ((element->detail_info.element_info.ddl_element_dc.level == level_no)||
    	(element->detail_info.element_info.ddl_element_dc.level > level_no)) {
    	element = getPrevElement(element);
        continue;
    }
	if(foundOccursItem == 1 &&  element->detail_info.element_info.ddl_element_dc.occurs_max == 1   &&
	     element->detail_info.parent_occurs_flag == 'N') {
	     break;
    }
    if((element->detail_info.element_info.ddl_element_dc.level < level_no) &&
      	(element->detail_info.element_info.ddl_element_dc.element_type == 0)) {
	    if(parentNameList->pList[i].parentName.currNameLen < strlen(element->detail_info.qualName.Name)) {
           		parentNameList->pList[i].parentName.Name = (char *)realloc(parentNameList->pList[i].parentName.Name,
              	parentNameList->pList[i].parentName.currNameLen + strlen(element->detail_info.qualName.Name));
            	if(parentNameList->pList[i].parentName.Name == NULL) {
              		printf("ddl2java - Fatal error: Unable to obtain memory for parent names.\n");
               		printf("ddl2java exits.\n");
               		exit(EXIT_FAILURE);
				}
            	parentNameList->pList[i].parentName.currNameLen = parentNameList->pList[i].parentName.currNameLen +
               	strlen(element->detail_info.qualName.Name);
         }
         strcpy((char *)parentNameList->pList[i].parentName.Name,
         element->detail_info.qualName.Name);
         parentNameList->pList[i].numParentOccurs = element->detail_info.element_info.ddl_element_dc.occurs_max;
         parentNameList->pList[i].parentDataLen = element->detail_info.element_info.ddl_element_dc.host_length;
         parentNameList->pList[i].offset = element->detail_info.element_info.ddl_element_dc.host_offset;
         i++;
         if(i > 10) {
         	printf("ddl2java - Error: too many parents for %s\n",
            	element->detail_info.qualName.Name);
            exit(1);
         }
         assert(i < 10);
         level_no = element->detail_info.element_info.ddl_element_dc.level;
      }
      if(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
         foundOccursItem = 1;
      }
      element = getPrevElement(element);
      if(foundOccursItem == 1 &&  element->detail_info.element_info.ddl_element_dc.occurs_max == 1   &&
        	element->detail_info.parent_occurs_flag == 'N') {
         	break;
      }
   }
   return i;
}
//------------------------------------------------------------------------------
// generateMethodParentOccurs
//		generates the java method for an element with an occuring parent
//------------------------------------------------------------------------------
void
ClassGen::generateMethodParentOccurs(int indexes, char *occursName, char *methodName,
                         char *fieldName, char *simpleName, char *returnType,
                         char *indent, char *valueString1)
{
	int j;
   	char thisOccursName[7 * 1025];
   	char *ptr;

   	/* Generate Javadoc text */
   	strcpy(thisOccursName, occursName);
   	ptr = strtok(thisOccursName,"[");

   	if(indexes > 0) {
   		for (j = 1; j <= indexes; j++) {
      		fprintf(pfp,"@param _i%d  index which identifies the occurrence of %s for which you want to %s %s.\n",
                  	(j - 1), ptr, methodName, simpleName);
         	ptr = strtok(NULL,".");
         	ptr = strtok(NULL,"[");
      	}
   	} else {
      	fprintf(pfp,"@param _i%d  index which identifies the occurrence of %s for which you want to %s %s\n",
           	indexes, ptr, simpleName);
   	}

   	/*  Generate method(s)  */
   	fprintf(pfp,"*/\n");
   	if(indexes > 0) {
       	if(verbose)
           	printf("    public %s %s%s(%s%s",
                  returnType, methodName, fieldName, valueString1,
                  (strlen(valueString1)? ",":""));
       	fprintf(pfp,"%spublic %s %s%s(%s%s\n",
           	indent, returnType, methodName, fieldName, valueString1,
                  (strlen(valueString1)? ",":""));
       	for(j = 1; j < indexes; j++) {
           	if(verbose)
               	printf("int _i%d,", (j -1));
           	fprintf(pfp,"                                  int _i%d,\n",(j - 1));
       	}
       	if(verbose)
           	printf("int _i%d)\n", (j -1));
       	fprintf(pfp,"                                  int _i%d) {\n",(j - 1));
    } else {
       	if(verbose)
           printf("    public %s %s%s(int _i%d)\n",
               	returnType, methodName, fieldName, indexes);
       	fprintf(pfp,"%spublic %s %s%s(int _i%d) {\n",
           indent, returnType, methodName, fieldName, indexes);
   }
}

//--------------------------------------------------------------------------
// writeMethodText
//		Actually prints the method text.
//--------------------------------------------------------------------------
void
ClassGen::writeMethodText(DETAIL_LIST *item, int indexes,
	char *occursName, int classType)
{
   char *indent = "    ";
   char methodName[10];
   char valueString1[20];
   char valueString2[25];
   char returnType[10];
   char fieldName[1025];
   bool flag = false;
   int dataType = item->detail_info.element_info.ddl_element_dc.host_data_type;


   switch(methodType) {
   	case SET_METHOD:
            strcpy(methodName,"set");
            if(classType == INPUT || classType == OUTPUT
            	|| classType == IO) {
				//soln_10-130423-7115
				if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	     				&& (item->detail_info.element_info.ddl_element_dc.host_scale > 0) ){					
					strcpy(valueString1,"BigDecimal value");			
				}
				//soln_10-130423-7115
				else{		  
				switch(dataType) {
				case BINARY_16_SIGNED:
				case LOGICAL_2:
				case SQLLENGTHORINDICATOR:
				case ENUMERATION:
					strcpy(valueString1,"short value");
					break;
				case BINARY_16_UNSIGNED:
				case BINARY_32_SIGNED:
				case LOGICAL_4:
				case BITSIGNED:
				case BITUNSIGNED:
					strcpy(valueString1,"int value");
					break;
				case BINARY_32_UNSIGNED:
				case BINARY_64_SIGNED:
					strcpy(valueString1,"long value");
					break;
				case FLOAT32:
					strcpy(valueString1,"float value");
					break;
				case FLOAT64:
					strcpy(valueString1,"double value");
					break;
				case LOGICAL_1:
					strcpy(valueString1,"byte value");
					break;
				default:
					strcpy(valueString1,"String value");
					break;
				}
				}
			} else {
				strcpy(valueString1, "String value");
			}
			strcpy(valueString2, "value;");
			
            strcpy(returnType, "void");
            break;
   	case CLEAR_METHOD:
            strcpy(methodName,"clear");
            strcpy(valueString1,"");
   	      	strcpy(returnType, "void");
   	      	if(classType == INPUT || classType == OUTPUT
   	      		|| classType == IO) {
			
				//soln_10-130423-7115
				if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	    				 && (item->detail_info.element_info.ddl_element_dc.host_scale > 0) ){					 
					strcpy(valueString2,"null;");
				}
				else{
				//soln_10-130423-7115					
				switch(dataType) {
				case BINARY_16_SIGNED:
				case LOGICAL_2:
				case SQLLENGTHORINDICATOR:
				case ENUMERATION:
					strcpy(valueString2,"(short)0;");
					break;
				case BINARY_16_UNSIGNED:
				case BINARY_32_SIGNED:
				case LOGICAL_4:
				case BITSIGNED:
				case BITUNSIGNED:
					strcpy(valueString2,"(int)0;");
					break;
				case BINARY_32_UNSIGNED:
				case BINARY_64_SIGNED:
					strcpy(valueString2,"(long)0;");
					break;
				case FLOAT32:
					strcpy(valueString2,"(float)0;");
					break;
				case FLOAT64:
					strcpy(valueString2,"(double)0;");
					break;
				case LOGICAL_1:
					strcpy(valueString2,"(byte)0;");
					break;
				default:
					strcpy(valueString2,"null;");
				}
				}
			} else {
				strcpy(valueString2,"null;");
			}
   		  	break;
   	case GET_METHOD:
            strcpy(methodName,"get");
            strcpy(valueString1,"");
   	      	strcpy(valueString2,"");
   	      	if(classType == INPUT || classType == OUTPUT || classType == IO) {
				//soln_10-130423-7115
				if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	    				 && (item->detail_info.element_info.ddl_element_dc.host_scale > 0) ){					 
					strcpy(returnType,"BigDecimal");
				}
				else {
				//soln_10-130423-7115				
				switch(dataType) {
				case BINARY_16_SIGNED:
				case LOGICAL_2:
				case SQLLENGTHORINDICATOR:
				case ENUMERATION:
					strcpy(returnType,"short");
					break;
				case BINARY_16_UNSIGNED:
				case BINARY_32_SIGNED:
				case LOGICAL_4:
				case BITSIGNED:
				case BITUNSIGNED:
					strcpy(returnType,"int");
					break;
				case BINARY_32_UNSIGNED:
				case BINARY_64_SIGNED:
					strcpy(returnType,"long");
					break;
				case FLOAT32:
					strcpy(returnType,"float");
					break;
				case FLOAT64:
					strcpy(returnType,"double");
					break;
				case LOGICAL_1:
					strcpy(returnType,"byte");
					break;
				default:
            		strcpy(returnType, "String");
					}
				}
			} else {
				strcpy(returnType,"String");
			}
   		  	break;
   	default: printf("ddl2java - ERROR: Internal error; invalid method name \n");
      			return;
   }

   memset(fieldName,'\0',sizeof(fieldName));
   upshiftFirstCharacter(item->detail_info.qualName.Name, fieldName,
                        (int) strlen (item->detail_info.qualName.Name) + 1,
         	            (int) sizeof(fieldName));

   if(verbose)
   		printf("  for %s.\n", item->detail_info.qualName.Name);

   switch(methodType) {
   case SET_METHOD:
      	/* Heading comment in code */
      	fprintf(pfp,"/**\nUsed to set a value for %s.\n",
            	item->detail_info.qualName.Name);
      	fprintf(pfp,"@param value  the value to be set.\n");

     	if(item->detail_info.parent_occurs_flag == 'Y') {
      		/* Occurring parent */
           	generateMethodParentOccurs(indexes, occursName, methodName, fieldName,
            	item->detail_info.qualName.Name, returnType, indent, valueString1);
         	fprintf(pfp,"%s  %s = %s\n",indent, occursName, valueString2);
      	} else {
      		if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
      			/* Occurring item */
           		if(verbose)
            		printf("    public void %s%s(%s, int _i0)\n",
               			methodName, fieldName, valueString1);
            	fprintf(pfp,"@param _i0  the index for %s.\n*/\n", item->detail_info.qualName.Name);
            	fprintf(pfp,"%spublic void %s%s(%s, int _i0) {\n",
              		indent, methodName, fieldName, valueString1);
            	fprintf(pfp,"%s  %s[_i0] = %s\n", indent, item->detail_info.qualName.Name, valueString2);
      		} else {
      			/* Single item (no occurs) */
      	   		if(verbose) {
      				printf("   public void %s%s(%s)\n",
               			methodName, fieldName, valueString1);
            	}
         		fprintf(pfp,"*/\n");
         		fprintf(pfp,"%spublic void %s%s(%s) {\n",indent,
                      methodName, fieldName, valueString1);
            	fprintf(pfp,"%s  %s = %s\n",indent, item->detail_info.qualName.Name, valueString2);
      		}
      	}
      	break;

   case CLEAR_METHOD:
      /* Heading comment in code */
   		fprintf(pfp,"/**\nUsed to clear any existing values for %s.\n",
      		item->detail_info.qualName.Name);

   		if(item->detail_info.parent_occurs_flag == 'Y') {
      		/* Occurring parent */
            generateMethodParentOccurs(indexes, occursName, methodName, fieldName,
            	item->detail_info.qualName.Name, returnType, indent, valueString1);
         	fprintf(pfp,"%s  %s = %s\n",indent, occursName, valueString2);
       	}  else {
      		if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
      			/* Occurring item */
                fprintf(pfp,"@param _i  the index for %s.\n*/\n", item->detail_info.qualName.Name);
          		fprintf(pfp,"%spublic void %s%s(int _i) {\n",
              		indent, methodName, fieldName, valueString1);
           		fprintf(pfp,"%s  %s[_i] = %s\n", indent, item->detail_info.qualName.Name, valueString2);
       		} else  {
      			/* Single item (no occurs) */
          		if(verbose) {
      				printf("   public void %s%s(%s)\n",
               			methodName, fieldName, valueString1);
          		}
          		fprintf(pfp,"*/\n");
          		fprintf(pfp,"%spublic void %s%s(%s) {\n",
            		indent, methodName, fieldName, valueString1);
           		fprintf(pfp,"%s    %s = %s\n",indent, item->detail_info.qualName.Name, valueString2);
       	   }
      	}
      	break;


   case GET_METHOD:
      	/* Nice little comment heading */
   		if(verbose) {
      		printf("Generating %s%s the get method for %s.\n",
         		methodName, fieldName, item->detail_info.qualName.Name);
      	}
   		fprintf(pfp,"/**\nUsed to obtain the value of %s.\n",
          	item->detail_info.qualName.Name);
      	fprintf(pfp,"@return the value for %s.\n", item->detail_info.qualName.Name);

   		if(item->detail_info.parent_occurs_flag == 'Y') {
      		/* Occurring parent */
            generateMethodParentOccurs(indexes, occursName, methodName, fieldName,
            	item->detail_info.qualName.Name, returnType, indent, valueString1);
          	fprintf(pfp,"%s  return(%s);\n",indent, occursName);
      	} else {
      		if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
      			/* Occurring item */
       	    	fprintf(pfp,"@param _i  the index for %s.\n", item->detail_info.qualName.Name);
          		fprintf(pfp,"@return the value of %s.\n", item->detail_info.qualName.Name);
          		fprintf(pfp,"*/\n");
          		fprintf(pfp,"%spublic %s %s%s(int _i) {\n",
              	    indent, returnType, methodName, fieldName);
          		fprintf(pfp,"%s  return(%s[_i]);\n", indent, item->detail_info.qualName.Name);
      		} else  {
      			/* Single item (no occurs) */
          		if(verbose) {
              		printf("   public String %s%s()\n",
               			methodName, fieldName);
          		}
    			fprintf(pfp,"*/\n");
          		fprintf(pfp,"%spublic %s %s%s() {\n",indent, returnType,
             		methodName, fieldName);
          		fprintf(pfp,"%s  return(%s);\n",indent, item->detail_info.qualName.Name);
      		}
   		}
   		break;

   }  /* end of switch */

   if(methodType != CLEAR_ALL)
   		fprintf(pfp,"%s}\n",indent);  /* line feed after each complete method */

   return;
}
//--------------------------------------------------------------------------
// writeLoadUnLoad
// 	Writes the marshal/unmarshal code to the .java file.
//--------------------------------------------------------------------------
bool
ClassGen::writeLoadUnLoad(int classType)
{
   DETAIL_LIST *element;
   int i = 0;
   static char *indent [] = {"    ",
     						 "      ",
   							 "        ",
                             "          ",
                             "            ",
                             "              ",
                             "                ",
                             "                  ",
                             "                    ",
                             "                      ",
                             "                        ",
                             "                          ",
                             "                            "};

   char methodName[30];
   char bufType[10];

   encodingString = false;

   if(classType == CLASSRQ || classType == OUTPUT) {
   		strcpy(methodName,"marshal");
      	strcpy(bufType,"Req");
   } else {
   		strcpy(methodName,"unmarshal");
      	strcpy(bufType,"Reply");
   }

   switch(classType) {
	   	case CLASSRQ:
			fprintf(pfp,"\n%spublic void %s()\n", indent[i], methodName);
      		fprintf(pfp,"%sthrows TsmpRequestException, NumberFormatException {\n", indent[i + 1]);
      		fprintf(pfp,"\n%sint _offset;\n", indent[i + 1]);
      		break;
		case OUTPUT:
			fprintf(pfp,"/**\nMoves and converts the data from the instance variables to a byte array\n");
			fprintf(pfp,"@return a byte array containing the converted data.\n");
			fprintf(pfp,"@exception com.tandem.ext.util.DataConversionException if an instance\n");
			fprintf(pfp,"variable fails to convert successfully.\n");
			fprintf(pfp,"**/\n");
			fprintf(pfp,"\n%spublic byte[] %s()\n", indent[i], methodName);
			fprintf(pfp,"%sthrows com.tandem.ext.util.DataConversionException, NumberFormatException {\n\n", indent[i + 2]);
			fprintf(pfp,"%sbyte [] _buffer = new byte[_bufferLen];\n",indent[i+2]);
			fprintf(pfp,"%s%s(_buffer);\n",indent[i+2], methodName);
			fprintf(pfp,"%sreturn _buffer;\n", indent[i+2]);
			fprintf(pfp,"%s}\n\n",indent[i]);
			fprintf(pfp,"/**\nMoves and converts the data from the instance variables to a byte array\n");
			fprintf(pfp,"@param _buffer the byte array to which the data is to be moved.\n");
		  	fprintf(pfp,"@return a byte array containing the converted data.\n");
		  	fprintf(pfp,"@exception com.tandem.guardian.DataConversionException if the length of the\n");
		  	fprintf(pfp,"supplied byte array is not greater than or equal to the minimum required\n");
		  	fprintf(pfp,"length or if an instance variable fails to convert successfully.\n");
	    	fprintf(pfp,"**/\n");
			fprintf(pfp,"\n%spublic byte[] %s(byte [] _buffer)\n", indent[i], methodName);
			fprintf(pfp,"%sthrows com.tandem.ext.util.DataConversionException {\n", indent[i + 2]);
			fprintf(pfp,"%sif(_buffer.length < _bufferLen) \n",
				indent[i+1]);
			fprintf(pfp,"%s   throw new DataConversionException(DataConversionException.ConvError.RANGE,\n",
							indent[i+1]);
			fprintf(pfp,"          \"Buffer length is less than minimum required buffer length\");\n",
				indent[i+1]);
			fprintf(pfp,"\n%sint _offset;\n", indent[i + 1]);
			break;
  	   	case CLASSRP:
   			fprintf(pfp,"%spublic void %s() \n",
      			indent[i], methodName);
      		fprintf(pfp,"\n%sthrows TsmpReplyException, NumberFormatException {\n", indent[i + 1]);
      		fprintf(pfp,"\n%sint _offset;\n", indent[i + 1]);
      		fprintf(pfp,"%sboolean _conversionError = false;\n",indent[i + 1]);
   			fprintf(pfp,"%sVector _unConvertedFields = new Vector();\n",indent[i + 1]);
      		break;
		case INPUT:
			fprintf(pfp,"/**\nMoves the data from the supplied byte array to the instance variables.\n");
			fprintf(pfp,"@param _buffer a byte array which contains the data to be moved. \n");
			fprintf(pfp,"@param _count the number of bytes to be converted.\n");
			fprintf(pfp,"@exception com.tandem.ext.util.DataConversionException if \n");
			fprintf(pfp,"a field fails to convert correctly and allowErrors has been set to false; or\n");
			fprintf(pfp,"allowErrors is set to true and _count < getLength().\n");
			fprintf(pfp,"@see #allowErrors\n");
	    	fprintf(pfp,"**/\n");
			fprintf(pfp,"%spublic void %s(byte [] _buffer, int _count) \n",
				indent[i], methodName);
			fprintf(pfp,"%sthrows com.tandem.ext.util.DataConversionException {\n",
				indent[i+1]);
			fprintf(pfp,"\n%sint _offset;\n", indent[i + 1]);
			fprintf(pfp,"%sint _fieldSize;\n", indent[i + 1]);
			fprintf(pfp,"%s_errorDetail.clear();\n", indent[i + 1]);
			fprintf(pfp,"%s_numConversionErrors = 0;\n", indent[i + 1]);
			fprintf(pfp,"%sif(!_allowErrors && _count < _bufferLen) {\n",indent[i + 1]);
			fprintf(pfp,"%s    _numConversionErrors++;\n", indent[i + 1]);
			fprintf(pfp,"%s    throw new DataConversionException(DataConversionException.ConvError.PARAM,\n",
				indent[i + 1]);
			fprintf(pfp,"%s      \"The value supplied for the _count variable is less than expected (see getLength())\");\n",
				indent[i + 1]);
			fprintf(pfp,"%s}\n",indent[i + 1]);
			fprintf(pfp,"%sif(_allowErrors && _count < _bufferLen) {\n", indent[i + 1]);
			fprintf(pfp,"%s    DataConversionException _bfe = new DataConversionException(DataConversionException.ConvError.PARAM,\n",
					indent[i + 1]);
			fprintf(pfp,"%s    \"The value supplied for the _count variable is less than expected (see  getLength())\");\n",
				indent[i + 1]);
			fprintf(pfp,"%s    _numConversionErrors++;\n", indent[i + 1]);
			fprintf(pfp,"%s    _errorDetail.put(_buffer,_bfe);\n",indent[i + 1]);
			fprintf(pfp,"%s}\n",indent[i + 1]);
			break;
		default: printf("ddl2java internal error; invalid classType %d passed to writeLoadUnLoad\n",classType);
   }

   element = getNextElement(NULL);


   if (encodingSpecified) {
      	fprintf(pfp,"%sString encodeSetting = new String();\n",indent[i + 1]);
      // Begin changes for Solution Number 10-071112-8842: now ddl2java accepts whatever encoding value is specified
		
		fprintf(pfp,"\n%sString defaultEncoding = \"%s\";\n", indent[i+1], encodingName);

	// end changes for Solution Number 10-071112-8842

              fprintf(pfp,"%sif(defaultEncoding == null){ \n", indent[i + 1] );
      	switch(classType) {
      	case CLASSRP:
      		fprintf(pfp,"%s throw new TsmpReplyException(\"Property not found for character encoding default : %s\");}\n",
      			indent[i + 1], encodingName);
      		break;
      	case CLASSRQ:
      	  	fprintf(pfp,"%s  throw new TsmpRequestException(\"Property not found for character encoding default : %s\");}\n",
      	  		indent[i + 1], encodingName);
      	  	break;
      	case OUTPUT:
      	case INPUT:
			fprintf(pfp,"%s throw new MissingResourceException(\"Property not found for character encoding default : %s\",\n",
				indent[i+1]);
			fprintf(pfp,"%s    this.getClass().getName(),\"%s\");\n", indent[i + 1], encodingName);
			fprintf(pfp,"%s}\n",indent[i+1]);
			break;
		default: printf("Internal error: ddl2java invalid classType %s passed to writeLoadUnLoad.\n",
			classType);
   		}
   }
   fprintf(pfp,"\n");

   _tmpDeclared = false;

   while(element != NULL) {
   		// highest level element (Def or Record Name)
      	// 89-level element can be skipped
   		if (((element->detail_info.element_info.ddl_element_dc.level == 0) &&
      		(element->detail_info.element_info.ddl_element_dc.host_data_type == 0))
         	|| (element->detail_info.element_info.ddl_element_dc.level == 89)) {
        	 element = getNextElement(element);
         	continue;
      	}
      	if(element->detail_info.parent_occurs_flag == 'N') {
         	// Group element that does not occur and that has no parent that occurs
      		if((element->detail_info.element_info.ddl_element_dc.element_type == 0) &&
         		(element->detail_info.element_info.ddl_element_dc.occurs_max == 1)) {
           		element = getNextElement(element);
            	continue;
         	}

   			if (encodingSpecified) {
	    		sprintf(encodingCode,"encodeSetting = System.getProperty(\"%s.encoding\",defaultEncoding);",
				element->detail_info.element_info.ddl_element_dc.ddl_element_name);
				encodingString = true;
			}
    		// Group element that occurs
    		if(element->detail_info.element_info.ddl_element_dc.element_type == 0) {
    			determineText4Occurs(element, 0, 0, classType);
    		} else {
       			// Elementary element that occurs
       			if(element->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
           			determineText4ElemOccurs(element, (i + 1), classType);
        		} else {
           			// Elementary element that doesn't occur and for which no parent occurs
            		fprintf(pfp,"\n%s_offset = %d;\n", indent[i + 1],
           				element->detail_info.element_info.ddl_element_dc.host_offset);
					if(classType == INPUT) {
						writeCheckBufferSize(element,
							element->detail_info.element_info.ddl_element_dc.host_data_type,
								indent[i + 1]);
					}
            		if(classType == CLASSRQ || classType == OUTPUT) {
            			checkDataType(indent[i + 1],
                  			element->detail_info.element_info.ddl_element_dc.host_data_type,
                  				classType);
                  		if(classType == CLASSRQ)
               				fprintf(pfp,"%sif(%s != null) {\n",indent[i + 1], element->detail_info.qualName.Name);
               			if(classType == CLASSRQ)
                			writeRQConversionText(element, element->detail_info.qualName.Name, indent[i + 2], NULL, classType);
                		else
                			writeOutputConversionText(element,element->detail_info.qualName.Name, indent[i + 2], NULL, classType);
                		if(classType == CLASSRQ)
                			fprintf(pfp,"%s}\n",indent[i + 1]);
            		} else {
						if(classType == CLASSRP) {
               				writeRPConversionText(element, element->detail_info.qualName.Name, indent[i + 1], NULL, classType);
               			} else {
							checkDataType(indent[i + 1],
								element->detail_info.element_info.ddl_element_dc.host_data_type,
									classType);
               				writeInputConversionText(element, element->detail_info.qualName.Name, indent[i + 1], NULL, classType);
						}

            		}
        		}
    		}
   		}
		element = getNextElement(element);
   }

   switch(classType) {
	case CLASSRQ:
   		fprintf(pfp,"%ssetBuffer(_requestBuffer);\n", indent[i + 1]);
   	  	fprintf(pfp,"%sreturn;\n",indent[i + 1]);
   	  	break;
   	case OUTPUT:
   		fprintf(pfp,"%sreturn _buffer;\n",indent[i + 1]);
		break;
    case CLASSRP:
		fprintf(pfp,"%sif(_conversionError) {\n",indent[i + 1]);
      	fprintf(pfp,"%sthrow new TsmpReplyException(\"%s - WARNING conversion errors occurred for the following fields\",_unConvertedFields);\n",
      		indent[i + 2],className);
		fprintf(pfp,"%s}\n",indent[i + 1]);
   }
   fprintf(pfp,"%s}\n",indent[i]);
   return true;
}
//------------------------------------------------------------------------------
// checkDataType
//		Determines the data type of a numeric element.
//------------------------------------------------------------------------------
void
ClassGen::checkDataType(char *indent, int dataType, int classType)
{
	if(classType == CLASSRQ || classType == CLASSRP) {
		switch(dataType) {
   		case BINARY_16_SIGNED:
   		case BINARY_16_UNSIGNED:
   		case BINARY_32_SIGNED:
   		case BINARY_32_UNSIGNED:
   		case BINARY_64_SIGNED:
   			if(!_indexDeclared) {
      			fprintf(pfp,"%sint _index;\n",indent );
         		_indexDeclared = true;
      		}
      		if(!_tmpDeclared) {
      			fprintf(pfp,"%sString _tmp;\n", indent);
         		_tmpDeclared = true;
      		}
      		return;
   		default: return;
   		}
	} else {
		if(classType == INPUT) {
			if(dataType == FLOAT32 || dataType == FLOAT64) {
				if(!_indexDeclared) {
					fprintf(pfp,"%sint _index;\n",indent );
					_indexDeclared = true;
				}
				if(!_tmpDeclared) {
					fprintf(pfp,"%sString _tmp;\n", indent);
					_tmpDeclared = true;
      			}
			}
		}
	}
	return;
}


//------------------------------------------------------------------------------
// determineText4ElemOccurs
//------------------------------------------------------------------------------
void
ClassGen::determineText4ElemOccurs(DETAIL_LIST *item,int i, int classType)
{  static char *indent [] = {"    ",
									  "      ",
   								  "        ",
                             "          ",
                             "            ",
                             "              ",
                             "                ",
                             "                  ",
                             "                    ",
                             "                      ",
                             "                        ",
                             "                          ",
                             "                            "};
   int j = i;


   if(classType == CLASSRQ || classType == OUTPUT) {
   		checkDataType(indent[j],
      		item->detail_info.element_info.ddl_element_dc.host_data_type,
      			classType);
   }
   writeForLoop(item, indent[j], "_i0", classType);
   fprintf(pfp,"\n%s_offset = %d + (_i0 * %d);\n",
   		indent[j + 1],item->detail_info.element_info.ddl_element_dc.host_offset,
            (item->detail_info.element_info.ddl_element_dc.sql_length?
             item->detail_info.element_info.ddl_element_dc.sql_length:
      		 item->detail_info.element_info.ddl_element_dc.host_length));
   if(classType == INPUT) {
   		writeCheckBufferSize(item,
			item->detail_info.element_info.ddl_element_dc.host_data_type,
			indent[i + 1]);
   }
   if(classType == CLASSRQ || classType == OUTPUT) {
	    if(classType == CLASSRQ)
   			fprintf(pfp,"%sif(%s[_i0] != null) {\n",indent[j + 1],item->detail_info.qualName.Name);
   		if(classType == CLASSRQ)
   			writeRQConversionText(item, item->detail_info.qualName.Name,indent[j + 2],"_i0", classType);
   		else
   			writeOutputConversionText(item, item->detail_info.qualName.Name,indent[j + 2],"_i0", classType);
   		if(classType == CLASSRQ)
      		fprintf(pfp,"%s}\n", indent[j + 1]);
   } else {
	   	if(classType == CLASSRP)
      		writeRPConversionText(item, item->detail_info.qualName.Name,indent[j + 1], "_i0", classType);
      	else
      		writeInputConversionText(item, item->detail_info.qualName.Name,indent[j + 1], "_i0", classType);
   }
   fprintf(pfp,"%s}\n",indent[i]);
   return;
}
//------------------------------------------------------------------------------
// writeForLoop
//		writes a for loop, if necessary for occurring items in the marshal/unmarshal
//		code.
//------------------------------------------------------------------------------
void
ClassGen::writeForLoop(DETAIL_LIST *item, char *indent, char *index,
	int classType)
{

	DETAIL_LIST *element;
   char dependsName[1024];

   dependsName[0] = '0';
   int dependsDataType;

   if(item->detail_info.element_info.ddl_element_dc.occurs_depends_upon != 0) {
   	element = getNextElement(NULL);

   	while(element != NULL) {
   		if(element->detail_info.element_info.ddl_element_dc.seq_num ==
      		item->detail_info.element_info.ddl_element_dc.occurs_depends_upon) {
      		strcpy(dependsName, element->detail_info.qualName.Name);
      		dependsDataType = element->detail_info.element_info.ddl_element_dc.host_data_type;
         	break;
      	}
      	element = getNextElement(element);
   	}

   	if(dependsName[0] != '0') {
		if(classType == INPUT || classType == OUTPUT || classType == IO) {
			if(dependsDataType == BINARY_16_SIGNED ||
				dependsDataType == BINARY_16_UNSIGNED ||
		 		dependsDataType == BINARY_32_SIGNED ||
		 		dependsDataType == BINARY_32_UNSIGNED ||
		 		dependsDataType == BINARY_64_SIGNED ) {
				fprintf(pfp,"%sfor(int %s = 0; %s < %s; %s++) { \n",
      				indent,index, index, dependsName, index);
      			return;
			}
		}
   		fprintf(pfp,"%sfor(int %s = 0; %s < Long.valueOf(%s).longValue(); %s++) { \n",
      		indent,index, index, dependsName, index);
      	return;
   	} else {
      	printf("ddl2java - WARNING unable to determine depending on object for %s.\n",
        		item->detail_info.qualName.Name);
      	printf("ddl2java - continuing using default values.\n");
   	}
   }
   fprintf(pfp,"%sfor(int %s = 0; %s < %d; %s++) {\n",
   		indent,
         index,
         index,
         item->detail_info.element_info.ddl_element_dc.occurs_max,
         index);
   return;
}
//------------------------------------------------------------------------------
// writeInitializeForLoop
//		writes a for loop, if necessary for occurring items in the initialization
//		code.
//------------------------------------------------------------------------------
void
ClassGen::writeInitializeForLoop(DETAIL_LIST *item, char *indent, char *index)
{

	DETAIL_LIST *element;

	//Changed it to int
	//short occurs_max;
	int  occurs_max;

	occurs_max = item->detail_info.element_info.ddl_element_dc.occurs_max;

    fprintf(pfp,"%sfor(int %s = 0; %s < %d; %s++) {\n",
   		indent, index, index, occurs_max, index);
   return;
}
//------------------------------------------------------------------------------
// determineText4Occurs
//------------------------------------------------------------------------------
bool
ClassGen::determineText4Occurs(DETAIL_LIST *item, int indentLevel,
	int iternum, int classType)
{
   DETAIL_LIST *element, *childElement;
   char itemName[8 * 1024];
   int i, next_level, j, k, n, numParents;
   PLENGTH_LIST parentLengthList;
   static char *iterator [] = { "_i0", "_i1", "_i2", "_i3", "_i4", "_i5",
   									  "_i6"};
   static char *indent [] = {"      ",
   							 "        ",
                             "          ",
                             "            ",
                             "              ",
                             "                ",
                             "                  ",
                             "                    ",
                             "                      ",
                             "                        ",
                             "                          ",
                             "                            "};

   i = indentLevel;
   n = iternum;

   if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
    	writeForLoop(item, indent[i], iterator[n], classType);
   		i++;
		n++;
   }
   element = getNextElement(item);
   next_level = element->detail_info.element_info.ddl_element_dc.level;


   while(element!= NULL) {
   	if((element != item) &&
      	(element->detail_info.element_info.ddl_element_dc.level ==
         	item->detail_info.element_info.ddl_element_dc.level)) {
         break;
      }
   	if(element->detail_info.element_info.ddl_element_dc.level == next_level) {
      	// Elementary item
         if(element->detail_info.element_info.ddl_element_dc.element_type == 1) {
      		if(element->detail_info.element_info.ddl_element_dc.occurs_max == 1) {
            	// doesn't occur
               	memset(itemName,'\0',sizeof(itemName));
            	numParents = getParentName(element, itemName, &parentLengthList);
				if(numParents == 7) numParents --;
            	if(numParents == -1) {
               		return false;
            	} else {
                  	fprintf(pfp,"\n%s_offset = %d", indent[i],
                  		element->detail_info.element_info.ddl_element_dc.host_offset);
			       	k = 0;
                  	for(j = numParents; j >= 0; j --) {
                  		if(parentLengthList.parentLen[j] > 0) {
                     		fprintf(pfp," + (%s * %d)",
                     			iterator[k], parentLengthList.parentLen[j]);
                        	k++;
                     	}
                  	}
                  	fprintf(pfp,";\n");
					if(classType == INPUT) {
						writeCheckBufferSize(element,
							element->detail_info.element_info.ddl_element_dc.host_data_type,
							indent[i + 1]);
					}
               		if(classType == CLASSRQ || classType == OUTPUT) {
                  		checkDataType(indent[i],
                     		element->detail_info.element_info.ddl_element_dc.host_data_type,
                     			classType);
                     	if(classType == CLASSRQ)
                    		fprintf(pfp,"%sif(%s != null) {\n",indent[i], itemName);
                    	if(classType == CLASSRQ)
               				writeRQConversionText(element, itemName, indent[i + 1], NULL, classType);
               			else
               				writeOutputConversionText(element, itemName, indent[i + 1], NULL, classType);
               			if(classType == CLASSRQ)
                    		fprintf(pfp,"%s}\n",indent[i]);
					} else {
					/* Soln. 10-130614-7736 */
				checkDataType(indent[i], element->detail_info.element_info.ddl_element_dc.host_data_type,
                     			classType);
				/* End of Soln. 10-130614-7736 */	
				if(classType == CLASSRP)
                  			writeRPConversionText(element, itemName, indent[i], NULL, classType);
                  		else
                  			writeInputConversionText(element, itemName, indent[i], NULL, classType);
                	}
      	     	}
         	} else {
         		// Elementary item that occurs
               	checkDataType(indent[i],
               	element->detail_info.element_info.ddl_element_dc.host_data_type,
               		classType);
                writeForLoop(element, indent[i], iterator[n], classType);
               	memset(itemName,'\0',sizeof(itemName));
               	numParents = getParentName(element,itemName, &parentLengthList);
               	if(numParents == -1 ) {
               		return false;
               	} else {
               		fprintf(pfp,"\n%s_offset = %d", indent[i + 1],	element->detail_info.element_info.ddl_element_dc.host_offset);
                  	k = 0;
                  	for(j = numParents; j >= 0; j --) {
                  		if(parentLengthList.parentLen[j] > 0) {
                  			fprintf(pfp," + (%s * %d)",
                     			iterator[k], parentLengthList.parentLen[j]);
                        	k++;
                     	}
                  	}
                  	fprintf(pfp," + (%s * %d)", iterator[k],
                         (element->detail_info.element_info.ddl_element_dc.sql_length?
                          element->detail_info.element_info.ddl_element_dc.sql_length:
      		              element->detail_info.element_info.ddl_element_dc.host_length));
                  	fprintf(pfp,";\n");
					if(classType == INPUT) {
						writeCheckBufferSize(element,
							element->detail_info.element_info.ddl_element_dc.host_data_type,
							indent[i + 1]);
						}
                 	if(classType == CLASSRQ || classType == OUTPUT) {
						if(classType == CLASSRQ)
                  			fprintf(pfp,"%sif(%s != null) {\n",indent[i + 1], itemName);
                  		if(classType == CLASSRQ)
               				writeRQConversionText(element, itemName, indent[i + 2], NULL, classType);
               			else
               				writeOutputConversionText(element, itemName, indent[i + 2], NULL, classType);
               			if(classType == CLASSRQ)
                     		fprintf(pfp,"%s}\n", indent[i + 1]);
                  	} else {
						if(classType == CLASSRP)
                  			writeRPConversionText(element, itemName, indent[i + 1], NULL, classType);
                  		else
                  			writeInputConversionText(element, itemName, indent[i + 1], NULL, classType);
                  	}
               	}
               	fprintf(pfp,"%s}\n",indent[i]);
			}
            element = getNextElement(element);
            continue;
         }
         // Group item
         (void)determineText4Occurs(element, i, n, classType);
         childElement = element->nItem;
         while(childElement->detail_info.element_info.ddl_element_dc.level > next_level) {
         	childElement = childElement->nItem;
            if(childElement == NULL) {
            	break;
            }
         }
         element = childElement;
         continue;
      }
      element = getNextElement(element);
   }
   if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
   		fprintf(pfp,"%s}\n",indent[indentLevel]);
   }
   return true;
}
//------------------------------------------------------------------------------
// getParentName
//		Determines the name used for the marshal/unmarshal code if a qualified name
//		must be used.
//------------------------------------------------------------------------------
int
ClassGen::getParentName(DETAIL_LIST *item, char *fullName,PLENGTH_LIST *parentLengthList)
{
	PNAME_LIST *parentNameList;
   	char tmp[1280];
   	int numParents ,j, k, indexes = 0;
   	static char *iterator [] = { "_i0", "_i1", "_i2", "_i3", "_i4", "_i5",
   									  "_i6"};

   	parentNameList = (PNAME_LIST *) malloc(sizeof(PNAME_LIST));
   	if(parentNameList == NULL) {
   		printf("ddl2java - Error: Unable to obtain memory for parent name list\n");
      	return -1;
   	}
   	for(j = 0; j < PLISTSIZE; j++) {
   		parentNameList->pList[j].parentName.Name = makeName(NAMESIZE);
      	if(parentNameList->pList[j].parentName.Name == NULL) {
        	printf("ddl2java - Fatal error: Unable to obtain memory for parent name list.\n");
         	printf("ddl2java exits.\n");
         	exit(EXIT_FAILURE);
      	}
      	parentNameList->pList[j].parentName.currNameLen = NAMESIZE;
      	memset(parentNameList->pList[j].parentName.Name,'\0', NAMESIZE);
	  	parentNameList->pList[j].numParentOccurs = 0;
	  	parentNameList->pList[j].parentDataLen = 0;
	  	parentNameList->pList[j].offset = -1;

   	}

   	numParents = getParentNameList(item,parentNameList);
   	assert(parentNameList->pList[0].parentName.Name != NULL);

   	for (k = 0; k < 7; k ++ ) {
   		parentLengthList->parentLen[k] = -1;
   	}

   	for(j = numParents; j >= 0; j -- ) {
   		if(*parentNameList->pList[j].parentName.Name == '\0') {
      		continue;
      	}
      	if(parentNameList->pList[j].numParentOccurs > 1) {
      		parentLengthList->parentLen[j] = parentNameList->pList[j].parentDataLen;
      		sprintf(tmp,"%s[%s].",parentNameList->pList[j].parentName.Name,iterator[indexes]);
         	indexes++;
      	} else {
      		sprintf(tmp,"%s.",parentNameList->pList[j].parentName.Name);
      	}
      	strcat(fullName,tmp);
   	}
   	if(item->detail_info.element_info.ddl_element_dc.occurs_max > 1) {
   		sprintf(tmp,"%s[%s]",item->detail_info.qualName.Name,
      	iterator[indexes]);
   	} else {
   		sprintf(tmp,"%s",item->detail_info.qualName.Name);
   	}
   	strcat(fullName,tmp);

   	for(j = 0; j < PLISTSIZE; j++) {
   		free(parentNameList->pList[j].parentName.Name);
   	}
   	free(parentNameList);

   	return numParents;
}
//------------------------------------------------------------------------------
// writeRQConversionText
//		Writes the code that calls the native methods for the tsmpRequest guy.
//------------------------------------------------------------------------------
void
ClassGen::writeRQConversionText(DETAIL_LIST *item, char *fieldName,
	char *indent, char *iterator, int classType)
{
	char bufferName[15];
	short data_type = 0;

	//change code
	int length = 0;
	short scale = 0;
	char exceptionName[25];

	length = item->detail_info.element_info.ddl_element_dc.host_length;
	data_type = item->detail_info.element_info.ddl_element_dc.host_data_type;
	scale = item->detail_info.element_info.ddl_element_dc.host_scale;

	strcpy(bufferName,"_requestBuffer");
	strcpy(exceptionName,"TsmpRequestException");

	if(iterator != NULL) {
    	strcat(fieldName,"[");
        strcat(fieldName,iterator);
        strcat(fieldName,"]");
    }

   	switch(data_type) {
        case SQLVARCHAR:
	 	case SQLDATETIMEYEAR:
	 	case SQLDATETIMEMONTH:
	 	case SQLDATETIMEYEARTOMONTH:
	 	case SQLDATETIMEDAY:
	 	case SQLDATETIMEMONTHTODAY:
	 	case SQLDATETIMEYEARTODAY:
	 	case SQLDATETIMEHOUR:
	 	case SQLDATETIMEDAYTOHOUR:
	 	case SQLDATETIMEMONTHTOHOUR:
	 	case SQLDATETIMEYEARTOHOUR:
	 	case SQLDATTIMEMINUTE:
	 	case SQLDATETIMEHOURTOMINUTE:
	 	case SQLDATETIMEDAYTOMINUTE:
	 	case SQLDATETIMEMONTHTOMINUTE:
	 	case SQLDATETIMEYEARTOMINUTE:
	 	case SQLDATETIMESECOND:
	 	case SQLDATETIMEMINUTETOSECOND:
	 	case SQLDATETIMEHOURTOSECOND:
	 	case SQLDATETIMEDAYTOSECOND:
	 	case SQLDATETIMEMONTHTOSECOND:
	 	case SQLDATETIMEYEARTOSECOND:
	 	case SQLDATETIMEFRACTION:
	 	case SQLDATETIMESECONDTOFRACTION:
	 	case SQLDATETIMEMINUTETOFRACTION:
	 	case SQLDATETIMEHOURTOFRACTION:
	 	case SQLDATETIMEDAYTOFRACTION:
	 	case SQLDATETIMEMONTHTOFRACTION:
	 	case SQLDATETIMEYEARTOFRACTION:
	 	case SQLINTERVALYEAR:
	 	case SQLINTERVALMONTH:
	 	case SQLINTERVALYEARTOMONTH:
	 	case SQLINTERVALDAY:
	 	case SQLINTERVALHOUR:
	 	case SQLINTERVALDAYTOHOUR:
	 	case SQLINTERVALMINUTE:
	 	case SQLINTERVALHOURTOMINUTE:
	 	case SQLINTERVALDAYTOMINUTE:
	 	case SQLINTERVALSECOND:
	 	case SQLINTERVALMINUTETOSECOND:
	 	case SQLINTERVALHOURTOSECOND:
	 	case SQLINTERVALDAYTOSECOND:
	 	case SQLINTERVALFRACTION:
	 	case SQLINTERVALSECONDTOFRACTION:
	 	case SQLINTERVALMINUTETOFRACTION:
   	 	case SQLINTERVALHOURTOFRACTION:
	 	case SQLINTERVALDAYTOFRACTION:
        case ALPHANUMERIC:
        case NATIONALSTRING:

			if (encodingString) {
				fprintf(pfp,"%s%s\n", indent, encodingCode);
    			encodingString = false;
			}
        	if (encodingSpecified) {
            	fprintf(pfp,"%s try {\n", indent);
 	    		fprintf(pfp,"%sbyte tempBytes[]=%s.getBytes(encodeSetting);\n",
       			indent, fieldName);
			} else {
            	fprintf(pfp,"%s try {\n", indent);
 	    		fprintf(pfp,"%sbyte tempBytes[]=%s.getBytes();\n",
       				indent, fieldName);
			}
            fprintf(pfp,"%sif (tempBytes.length > %d) throw new CharConversionException();\n",
				indent, ((charStringType == NULLTERMINATED) &&
				(languageType == C_LANG))?(length - 1):length);
       		if((charStringType == NULLTERMINATED) && (languageType == C_LANG)) {
            	fprintf(pfp,"%sfor(int _i=_offset; _i<(_offset + %d); _i++) \n%s  %s[_i]= (byte )0;\n",
					indent, length, indent, bufferName);
			}  else {
            	fprintf(pfp,"%sfor(int _i=_offset; _i<(_offset + %d); _i++) %s    %s[_i]=(byte)\' \';\n",
					indent, length, indent, bufferName);
			}
            fprintf(pfp,"%sfor(int _i=_offset, _j=0; _j<tempBytes.length; _i++, _j++) \n%s    %s[_i]=tempBytes[_j];\n",
				indent, indent, bufferName);
            fprintf(pfp,"%s     }catch (Exception e) {\n", indent);
        	if (encodingSpecified) {
				fprintf(pfp,"%s  throw new %s(\"Exception \" + e.toString() + \" Conversion error occurred for %s with encoding \" + encodeSetting);\n",
					indent,exceptionName,fieldName);
			} else {
				fprintf(pfp,"%s  throw new %s(\"Exception \" + e.toString() + \" Conversion error occurred for %s \");\n",
					indent,exceptionName,fieldName);
			}

	    	fprintf(pfp,"%s}\n", indent);
	    	return;

         case NUM_STR_UNSIGNED:
         	fprintf(pfp,"%sif(JavaStrToNumStr(%s, _offset, %d, %d, 16) != 0) {\n",
               	indent, fieldName, length, scale);
            break;
         case BINARY_16_SIGNED:
         	fprintf(pfp,"%stry {\n", indent);
            if(scale != 0) {
            	printRemoveDecimal(indent,fieldName, classType);
               	fprintf(pfp,"%s  if(JavaShortToBIN16(Short.valueOf(_tmp).shortValue(), _offset) != 0) {\n",
            		indent);
               	printConversionException(pfp,indent, fieldName);
            } else {
               fprintf(pfp,"%s  if(JavaShortToBIN16(Short.valueOf(%s).shortValue(), _offset) != 0) {\n",
            		indent, fieldName);
               printConversionException(pfp,indent, fieldName);
            }
            return;
         case BINARY_16_UNSIGNED:
         	if(languageType == C_LANG) {
         		fprintf(pfp,"%stry {\n", indent);
            	if(scale != 0) {
            		printRemoveDecimal(indent,fieldName, classType);
               		fprintf(pfp,"%s  if(JavaIntToUNBIN16(Integer.valueOf(_tmp).intValue(), _offset) != 0) {\n",
            			indent);
               		printConversionException(pfp,indent, fieldName);
            	} else {
               		fprintf(pfp,"%s  if(JavaIntToUNBIN16(Integer.valueOf(%s).intValue(), _offset) != 0) {\n",
            			indent,fieldName);
               		printConversionException(pfp,indent, fieldName);
            	}
            } else {
               	fprintf(pfp,"%stry {\n", indent);
            	if(scale != 0) {
            		printRemoveDecimal(indent,fieldName, classType);
               		fprintf(pfp,"%s  if(JavaShortToBIN16(Short.valueOf(_tmp).shortValue(), _offset) != 0) {\n",
            			indent);
               		printConversionException(pfp,indent, fieldName);
            	} else {
               		fprintf(pfp,"%s  if(JavaShortToBIN16(Short.valueOf(%s).shortValue(), _offset) != 0) {\n",
            			indent, fieldName);
               		printConversionException(pfp,indent, fieldName);
            	}
            }
            return;
         case BINARY_32_SIGNED:
         	fprintf(pfp,"%stry {\n", indent);
            if(scale != 0) {
            	printRemoveDecimal(indent, fieldName, classType);
               	fprintf(pfp,"%s  if(JavaIntToBIN32(Integer.valueOf(_tmp).intValue(), _offset) != 0) {\n",
            		indent);
               	printConversionException(pfp,indent, fieldName);
            } else {
               	fprintf(pfp,"%s  if(JavaIntToBIN32(Integer.valueOf(%s).intValue(), _offset)!= 0) {\n",
            		indent, fieldName);
               	printConversionException(pfp,indent, fieldName);
            }
            return;
         case BINARY_32_UNSIGNED:
         	if(languageType == C_LANG) {
         		fprintf(pfp,"%stry {\n", indent);
            	if(scale != 0) {
            		printRemoveDecimal(indent, fieldName, classType);
               		fprintf(pfp,"%s  if(JavaLongToUNBIN32(Long.valueOf(_tmp).longValue(), _offset) != 0) {\n",
            			indent);
               		printConversionException(pfp,indent, fieldName);
            	} else {
               		fprintf(pfp,"%s  if(JavaLongToUNBIN32(Long.valueOf(%s).longValue(), _offset)!= 0) {\n",
            			indent, fieldName);
               		printConversionException(pfp,indent, fieldName);
            	}
            } else {
               	fprintf(pfp,"%stry {\n", indent);
            	if(scale != 0) {
            		printRemoveDecimal(indent, fieldName, classType);
               		fprintf(pfp,"%s  if(JavaIntToBIN32(Integer.valueOf(_tmp).intValue(), _offset) != 0) {\n",
            			indent);
               		printConversionException(pfp,indent, fieldName);
            	} else {
               		fprintf(pfp,"%s  if(JavaIntToBIN32(Integer.valueOf(%s).intValue(), _offset)!= 0) {\n",
            			indent, fieldName);
               		printConversionException(pfp,indent, fieldName);
            	}
            }
            return;
         case BINARY_64_SIGNED:
         	fprintf(pfp,"%stry {\n", indent);
            if(scale != 0) {
               	printRemoveDecimal(indent,fieldName, classType);
            	fprintf(pfp,"%s  if(JavaLongToBIN64(Long.valueOf(_tmp).longValue(), _offset) != 0) {\n",
            		indent);
               	printConversionException(pfp,indent, fieldName);
            } else {
            	fprintf(pfp,"%s  if(JavaLongToBIN64(Long.valueOf(%s.trim()).longValue(), _offset) != 0) {\n",
            		indent, fieldName);
               	printConversionException(pfp,indent, fieldName);
         	}
            return;
         case FLOAT32:
            fprintf(pfp,"%stry {\n", indent);
            fprintf(pfp,"%s  if(StrToFloat( %s, _offset) != 0) {\n",
            	indent, fieldName);
            printConversionException(pfp,indent, fieldName);
            return;
         case FLOAT64:
         	fprintf(pfp,"%stry {\n", indent);
            fprintf(pfp,"%s  if(StrToDouble( %s, _offset) != 0) {\n",
            	indent, fieldName);
            printConversionException(pfp,indent, fieldName);
            return;
         case NUM_STR_TRAIL_EMB_SIGN:
         	fprintf(pfp,"%sif(JavaStrToNumStr( %s, _offset,%d, %d, 15) != 0) {\n",
            	indent,	fieldName, length, scale);
         	break;
         case NUM_STR_TRAIL_SEP_SIGN:
         	fprintf(pfp,"%sif(JavaStrToNumStr( %s, _offset,%d, %d, 13) != 0) {\n",
            	indent, fieldName, length, scale);
         	break;
         case NUM_STR_LEAD_EMB_SIGN:
            fprintf(pfp,"%sif(JavaStrToNumStr(%s, _offset,%d, %d, 14) != 0) {\n",
            	indent, fieldName, length, scale);
         	break;
         case NUM_STR_LEAD_SEP_SIGN:
         	fprintf(pfp,"%sif(JavaStrToNumStr( %s, _offset,%d, %d, 12) != 0) {\n",
            	indent, fieldName, length, scale);
         	break;
         case LOGICAL_1:
         	fprintf(pfp,"%sif(JavaStrToLogical1( %s, _offset) != 0) {\n",
            	indent,  fieldName);
            break;
         case LOGICAL_2:
            fprintf(pfp,"%stry {\n", indent);
         	fprintf(pfp,"%s  if(JavaShortToLogical2(Short.valueOf(%s.trim()).shortValue(), _offset) != 0) {\n",
            	indent, fieldName);
            printConversionException(pfp,indent, fieldName);
            return;
         case LOGICAL_4:
            fprintf(pfp,"%stry {\n", indent);
         	fprintf(pfp,"%s  if(JavaLongToLogical4(Long.valueOf(%s.trim()).longValue(), _offset) != 0) {\n",
            	indent, fieldName);
            printConversionException(pfp,indent, fieldName);
            return;
         case BINARY_8_SIGNED:
         case BINARY_8_UNSIGNED:
         	if(languageType == C_LANG) {
            	fprintf(pfp,"%sif(JavaStrToBIN8(%s, _offset) != 0) {\n",
               		indent,  fieldName);
            }
            if(languageType == COBOL_LANG) {
            	fprintf(pfp,"%sif(JavaStrToBIN8Cobol(%s, _offset) != 0) {\n",
               	indent,  fieldName, length);
            }
            break;
	 	case ENUMERATION:
	 	case SQLLENGTHORINDICATOR:
		 	fprintf(pfp,"%stry {\n", indent);
		    fprintf(pfp,"%s  if(JavaShortToBIN16(Short.valueOf(%s).shortValue(), _offset) != 0) {\n",
		          indent, fieldName);
		    printConversionException(pfp,indent, fieldName);
		    return;

         case BITSIGNED:
            fprintf(pfp,"%stry {\n", indent);
         	fprintf(pfp,"%s  if(BITtoTdmShort(Integer.valueOf(%s.trim()).intValue(), _offset) != 0) { \n",
            	indent, fieldName);
            printConversionException(pfp,indent, fieldName);
            return;
         case BITUNSIGNED:
           	fprintf(pfp,"%stry {\n", indent);
           	fprintf(pfp,"%sif(UBITtoTdmShort(Integer.valueOf(%s.trim()).intValue(), _offset) != 0) { \n",
           		indent, fieldName);
           	printConversionException(pfp,indent, fieldName);
           	return;
         default: printf("Unsupported data type %d\n", item->detail_info.element_info.ddl_element_dc.host_data_type);
      }
      fprintf(pfp,"%s  throw new %s(\"Conversion error occurred for %s\");\n",
      		indent, exceptionName, fieldName);
      fprintf(pfp,"%s}\n", indent);
	return;
}
//------------------------------------------------------------------------------
// printRemoveDecimal
//		Writes code to the .java file that removes a decimal point before calling
//		the native conversion routine for marshal.
//------------------------------------------------------------------------------
void
ClassGen::printRemoveDecimal(char *indent, char *fieldName, int classType)
{
	fprintf(pfp,"%s  _index = %s.indexOf(\".\");\n",indent,fieldName);
   	fprintf(pfp,"%s  if(_index == -1)\n", indent);
   	if(classType == CLASSRQ || classType == CLASSRP || classType == CLASSBOTH)
   		fprintf(pfp,"%s    throw new TsmpRequestException(\"Conversion error - missing decimial point - for %s.\");\n",
     		indent, fieldName);
    else
    	fprintf(pfp,"%s    throw new DataConversionException(DataConversionException.ConvError.SYNTAX,\"Conversion error - missing decimial point - for %s.\");\n",
     		indent, fieldName);
   	fprintf(pfp,"%s  _tmp = %s.substring(0,_index) + %s.substring((_index + 1),%s.length());\n",
     	indent, fieldName, fieldName, fieldName);
   	return;
}

//------------------------------------------------------------------------------
// writeRPConversionText
//		Writes the code that calls the native conversion routine for the tsmpReply.
//------------------------------------------------------------------------------
void
ClassGen::writeRPConversionText(DETAIL_LIST *item, char *fieldName,
	char *indent, char *iterator, int classType)
{
	char tempFieldName[1024];
	char bufferName[15];
	short scale = 0;
	//change code
	int length = 0;
	short seq_num = 0;
	short data_type = 0;

	scale = item->detail_info.element_info.ddl_element_dc.host_scale;
	length = item->detail_info.element_info.ddl_element_dc.host_length;
	seq_num = item->detail_info.element_info.ddl_element_dc.seq_num;
	data_type = item->detail_info.element_info.ddl_element_dc.host_data_type;

	strcpy(tempFieldName,fieldName);
	if(iterator != NULL) {
    	strcat(fieldName,"[");
       	strcat(fieldName,iterator);
       	strcat(fieldName,"]");
    }
    if(classType == CLASSRP)
		strcpy(bufferName,"_replyBuffer");

	switch(data_type)
    {
		case SQLVARCHAR:
		case SQLDATETIMEYEAR:
   		case SQLDATETIMEMONTH:
		case SQLDATETIMEYEARTOMONTH:
   		case SQLDATETIMEDAY:
   		case SQLDATETIMEMONTHTODAY:
   		case SQLDATETIMEYEARTODAY:
   		case SQLDATETIMEHOUR:
   		case SQLDATETIMEDAYTOHOUR:
   		case SQLDATETIMEMONTHTOHOUR:
   		case SQLDATETIMEYEARTOHOUR:
   		case SQLDATTIMEMINUTE:
   		case SQLDATETIMEHOURTOMINUTE:
   		case SQLDATETIMEDAYTOMINUTE:
   		case SQLDATETIMEMONTHTOMINUTE:
   		case SQLDATETIMEYEARTOMINUTE:
   		case SQLDATETIMESECOND:
   		case SQLDATETIMEMINUTETOSECOND:
   		case SQLDATETIMEHOURTOSECOND:
   		case SQLDATETIMEDAYTOSECOND:
   		case SQLDATETIMEMONTHTOSECOND:
   		case SQLDATETIMEYEARTOSECOND:
   		case SQLDATETIMEFRACTION:
   		case SQLDATETIMESECONDTOFRACTION:
   		case SQLDATETIMEMINUTETOFRACTION:
   		case SQLDATETIMEHOURTOFRACTION:
   		case SQLDATETIMEDAYTOFRACTION:
   		case SQLDATETIMEMONTHTOFRACTION:
   		case SQLDATETIMEYEARTOFRACTION:
   		case SQLINTERVALYEAR:
   		case SQLINTERVALMONTH:
   		case SQLINTERVALYEARTOMONTH:
   		case SQLINTERVALDAY:
   		case SQLINTERVALHOUR:
   		case SQLINTERVALDAYTOHOUR:
   		case SQLINTERVALMINUTE:
   		case SQLINTERVALHOURTOMINUTE:
   		case SQLINTERVALDAYTOMINUTE:
   		case SQLINTERVALSECOND:
   		case SQLINTERVALMINUTETOSECOND:
   		case SQLINTERVALHOURTOSECOND:
   		case SQLINTERVALDAYTOSECOND:
   		case SQLINTERVALFRACTION:
   		case SQLINTERVALSECONDTOFRACTION:
   		case SQLINTERVALMINUTETOFRACTION:
   		case SQLINTERVALHOURTOFRACTION:
		case SQLINTERVALDAYTOFRACTION:
      	case ALPHANUMERIC:
      	case NATIONALSTRING:

        	if (encodingString) {
            	fprintf(pfp,"%s%s\n", indent, encodingCode);
	    		encodingString = false;
			}

        	fprintf(pfp,"%s try {\n", indent);
        	if (encodingSpecified) {
            	fprintf(pfp,"%s%s = new String(%s,_offset,%d,encodeSetting);\n",
            		indent, fieldName, bufferName,length);
			} else {
            	fprintf(pfp,"%s%s = new String(%s,_offset,%d);\n",
            		indent, fieldName, bufferName, length);
			}
        	fprintf(pfp,"%s}catch (Exception e) {\n", indent);
        	fprintf(pfp,"%s  _conversionError = true;\n",indent);
        	fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
           		indent, fieldName);
        	fprintf(pfp,"%s}\n", indent);
        	break;

        case NUM_STR_UNSIGNED:
            fprintf(pfp,"%s%s = NumStrToJavaStr(_offset, %d, %d, 16);\n",
            	indent, fieldName, length, scale);
            fprintf(pfp,"%sif(%s == null) {\n",indent,fieldName);
            fprintf(pfp,"%s  _conversionError = true;\n",indent);
            fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
            	indent, fieldName);
            fprintf(pfp,"%s}\n", indent);
            break;
         case BINARY_16_SIGNED:
         	if(scale != 0) {
				fprintf(pfp,"%sshort _num_%d = BIN16ToJavaShort(_offset);\n",
            	  indent, seq_num);
			   	fprintf(pfp,"%s%s = convertShortToString(_num_%d,%d);\n",
               		indent, fieldName, seq_num, scale);
            } else {
               fprintf(pfp,"%s%s = Short.toString(BIN16ToJavaShort(_offset));\n",
            	   indent, fieldName);
            }
            break;
         case BINARY_16_UNSIGNED:
         	if(scale != 0) {
            	fprintf(pfp,"%sint _num_%d = UNBIN16ToJavaInt(_offset);\n",
            		indent, seq_num);
            	fprintf(pfp,"%s%s = convertIntegerToString(_num_%d, %d);\n",
               		indent, fieldName, seq_num, scale);
            } else {
            	fprintf(pfp,"%s%s = Integer.toString(UNBIN16ToJavaInt(_offset));\n",
            		indent, fieldName);
            }
            break;
         case BINARY_32_SIGNED:
         	if(scale != 0) {
            	fprintf(pfp,"%sint _num_%d = BIN32ToJavaInt(_offset);\n",
               		indent, seq_num);
               	fprintf(pfp,"%s%s = convertIntegerToString(_num_%d, %d);\n",
               		indent, fieldName, seq_num, scale);
            } else {
            	fprintf(pfp,"%s%s = Integer.toString(BIN32ToJavaInt(_offset));\n",
            		indent, fieldName);
            }
            break;
         case BINARY_32_UNSIGNED:
         	if(scale != 0) {
            	fprintf(pfp,"%slong _num_%d = UNBIN32ToJavaLong(_offset);\n",
            		indent, seq_num);
            	fprintf(pfp,"%s%s = convertLongToString(_num_%d, %d);\n",
               		indent, fieldName, seq_num, scale);
            } else {
            	fprintf(pfp,"%s%s = Long.toString(UNBIN32ToJavaLong(_offset));\n",
            		indent, fieldName);
            }
            break;
         case BINARY_64_SIGNED:
         	if(scale != 0) {
				fprintf(pfp,"%slong _num_%d = BIN64ToJavaLong( _offset);\n",
               		indent, seq_num);
               	fprintf(pfp,"%s%s = convertLongToString(_num_%d, %d);\n",
               		indent, fieldName, seq_num, scale);
            } else {
          		fprintf(pfp,"%s%s = Long.toString(BIN64ToJavaLong(_offset)); \n",
            		indent, fieldName);
            }
            break;
         case FLOAT32:
         	fprintf(pfp,"%s%s = Float32ToStr(_offset); \n",
            	indent, fieldName);
            break;
         case FLOAT64:
    		fprintf(pfp,"%s%s = Float64ToStr(_offset); \n",
            	indent,  fieldName);
            break;
         case NUM_STR_TRAIL_EMB_SIGN:
         	fprintf(pfp,"%s%s = NumStrToJavaStr(_offset, %d,(short) %d,(short) 15); \n",
            	indent, fieldName, length, scale);
            fprintf(pfp,"%sif(%s == null) {\n",indent, fieldName);
            fprintf(pfp,"%s  _conversionError = true;\n",indent);
            fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
            	indent, fieldName);
            fprintf(pfp,"%s}\n", indent);
            break;
         case NUM_STR_TRAIL_SEP_SIGN:
         	fprintf(pfp,"%s%s = NumStrToJavaStr(_offset, %d,(short) %d,(short) 13); \n",
            	indent, fieldName, length, scale);
            fprintf(pfp,"%sif(%s == null) {\n",indent, fieldName);
            fprintf(pfp,"%s  _conversionError = true;\n",indent);
            fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
            	indent, fieldName);
            fprintf(pfp,"%s}\n", indent);
            break;
         case NUM_STR_LEAD_EMB_SIGN:
         	fprintf(pfp,"%s%s = NumStrToJavaStr(_offset, %d,(short) %d,(short) 14); \n",
            	indent, fieldName,length, scale);
            fprintf(pfp,"%sif(%s == null) {\n",indent, fieldName);
            fprintf(pfp,"%s  _conversionError = true;\n",indent);
            fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
            	indent, fieldName);
            fprintf(pfp,"%s}\n", indent);
            break;
         case NUM_STR_LEAD_SEP_SIGN:
       		fprintf(pfp,"%s%s = NumStrToJavaStr(_offset, %d,(short) %d,(short) 12); \n",
            	indent, fieldName, length,scale);
            fprintf(pfp,"%sif(%s == null) {\n",indent, fieldName);
            fprintf(pfp,"%s  _conversionError = true;\n",indent);
            fprintf(pfp,"%s  _unConvertedFields.addElement(\"%s\");\n",
            	indent, fieldName);
            fprintf(pfp,"%s}\n", indent);
            break;
         case LOGICAL_1:
         	fprintf(pfp,"%s%s = Logical1ToJavaStr(_offset); \n",
            	indent, fieldName);
            break;
         case LOGICAL_2:
      		fprintf(pfp,"%s%s = Integer.toString(Logical2ToJavaInt(_offset)); \n",
            	indent, fieldName);
            break;
         case LOGICAL_4:
   			fprintf(pfp,"%s%s = Long.toString(Logical4ToJavaLong(_offset)); \n",
            	indent, fieldName);
            break;
         case BINARY_8_SIGNED:
         case BINARY_8_UNSIGNED:
         	fprintf(pfp,"%s%s = BIN8ToJavaStr(_offset); \n",
              	indent, fieldName);
            break;
		 case ENUMERATION:
       	 case SQLLENGTHORINDICATOR:
		 	fprintf(pfp,"%s%s = Short.toString(BIN16ToJavaShort(_offset));\n",
            	   indent, fieldName);
			break;
          case BITSIGNED:
         	printf("ddl2java - WARNING: Unsupported data type in element %s; generating default conversion.\n",
            	item->detail_info.element_info.ddl_element_dc.ddl_element_name);
            fprintf(pfp,"%s%s = Short.toString(BIN16ToJavaShort(_offset));\n",
            	   indent, fieldName);
         	break;
          case BITUNSIGNED:
         	printf("ddl2java - WARNING: Unsupported data type in element %s; generating default conversion.\n",
            	item->detail_info.element_info.ddl_element_dc.ddl_element_name);
         	fprintf(pfp,"%s%s = Integer.toString(UNBIN16ToJavaInt(_offset));\n",
            		indent, fieldName);
            break;
          default: printf("Unsupported data type in element %s\n", item->detail_info.element_info.ddl_element_dc.ddl_element_name);
      }
      return;
}
//------------------------------------------------------------------------------
// writeOutputConversionText
//		Writes the code that calls the data conversion routines
//		for the generated Output class.
//------------------------------------------------------------------------------
void
ClassGen::writeOutputConversionText(DETAIL_LIST *item, char *fieldName,
	char *indent, char *iterator, int classType)
{
	char bufferName[15];
	short data_type = 0;
	//change code
	int length = 0;
	short scale = 0;
	int fieldNameLen = 0;
	char exceptionName[25];

	length = item->detail_info.element_info.ddl_element_dc.host_length;
	data_type = item->detail_info.element_info.ddl_element_dc.host_data_type;
	scale = item->detail_info.element_info.ddl_element_dc.host_scale;

	strcpy(bufferName,"_buffer");
	strcpy(exceptionName,"DataConversionException");

	if(iterator != NULL) {
		fieldNameLen = strlen(fieldName);
		/* The following is true if this is an IO guy */
		if(fieldName[fieldNameLen -1] != ']') {
    		strcat(fieldName,"[");
        	strcat(fieldName,iterator);
        	strcat(fieldName,"]");
		}
    }
        //soln_10-130423-7115
        if (( (data_type == BINARY_16_SIGNED) || (data_type == BINARY_16_UNSIGNED) || (data_type == BINARY_32_SIGNED) || (data_type == BINARY_32_UNSIGNED) || (data_type == BINARY_64_SIGNED))
	     && (item->detail_info.element_info.ddl_element_dc.host_scale > 0) ){
				fprintf(pfp,"%sDataConversion.BigDecimalToBIN(%s, %s, _offset, %d, %d, %d);\n",
 	          	indent, bufferName, fieldName, data_type, length, scale);
				return;
		}
		//soln_10-130423-7115
   	switch(data_type) {
        case SQLVARCHAR:
	 	case SQLDATETIMEYEAR:
	 	case SQLDATETIMEMONTH:
	 	case SQLDATETIMEYEARTOMONTH:
	 	case SQLDATETIMEDAY:
	 	case SQLDATETIMEMONTHTODAY:
	 	case SQLDATETIMEYEARTODAY:
	 	case SQLDATETIMEHOUR:
	 	case SQLDATETIMEDAYTOHOUR:
	 	case SQLDATETIMEMONTHTOHOUR:
	 	case SQLDATETIMEYEARTOHOUR:
	 	case SQLDATTIMEMINUTE:
	 	case SQLDATETIMEHOURTOMINUTE:
	 	case SQLDATETIMEDAYTOMINUTE:
	 	case SQLDATETIMEMONTHTOMINUTE:
	 	case SQLDATETIMEYEARTOMINUTE:
	 	case SQLDATETIMESECOND:
	 	case SQLDATETIMEMINUTETOSECOND:
	 	case SQLDATETIMEHOURTOSECOND:
	 	case SQLDATETIMEDAYTOSECOND:
	 	case SQLDATETIMEMONTHTOSECOND:
	 	case SQLDATETIMEYEARTOSECOND:
	 	case SQLDATETIMEFRACTION:
	 	case SQLDATETIMESECONDTOFRACTION:
	 	case SQLDATETIMEMINUTETOFRACTION:
	 	case SQLDATETIMEHOURTOFRACTION:
	 	case SQLDATETIMEDAYTOFRACTION:
	 	case SQLDATETIMEMONTHTOFRACTION:
	 	case SQLDATETIMEYEARTOFRACTION:
	 	case SQLINTERVALYEAR:
	 	case SQLINTERVALMONTH:
	 	case SQLINTERVALYEARTOMONTH:
	 	case SQLINTERVALDAY:
	 	case SQLINTERVALHOUR:
	 	case SQLINTERVALDAYTOHOUR:
	 	case SQLINTERVALMINUTE:
	 	case SQLINTERVALHOURTOMINUTE:
	 	case SQLINTERVALDAYTOMINUTE:
	 	case SQLINTERVALSECOND:
	 	case SQLINTERVALMINUTETOSECOND:
	 	case SQLINTERVALHOURTOSECOND:
	 	case SQLINTERVALDAYTOSECOND:
	 	case SQLINTERVALFRACTION:
	 	case SQLINTERVALSECONDTOFRACTION:
	 	case SQLINTERVALMINUTETOFRACTION:
   	 	case SQLINTERVALHOURTOFRACTION:
	 	case SQLINTERVALDAYTOFRACTION:
        case ALPHANUMERIC:
        case NATIONALSTRING:
			fprintf(pfp,"%sif(%s != null) {\n",indent, fieldName);
			if (encodingString) {
				fprintf(pfp,"%s%s\n", indent, encodingCode);
    			encodingString = false;
			}
        	if (encodingSpecified) {
				if(charStringType == NULLTERMINATED)
         			fprintf(pfp,"%sDataConversion.JavaStrToCStr(%s,%s,_offset,%d, encodeSetting);\n",
       					indent,bufferName, fieldName,length);
       			else
       				fprintf(pfp,"%sDataConversion.JavaStrToCobolStr(%s,%s,_offset,%d, encodeSetting);\n",
       					indent,bufferName, fieldName,length);
			} else {
         		if(charStringType == NULLTERMINATED)
 	    			fprintf(pfp,"%sDataConversion.JavaStrToCStr(%s,%s,_offset,%d);\n",
       					indent, bufferName, fieldName, length);
       			else
       				fprintf(pfp,"%sDataConversion.JavaStrToCobolStr(%s,%s,_offset,%d);\n",
       					indent, bufferName, fieldName, length);
			}
			fprintf(pfp,"%s}\n",indent);
            return;
         case NUM_STR_UNSIGNED:
			fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
         	fprintf(pfp,"  %sDataConversion.JavaStrToNumStr(%s,%s, _offset, %d, %d, 16);\n",
               	indent, bufferName, fieldName, length, scale);
            break;
         case BINARY_16_SIGNED:
         	fprintf(pfp,"%sDataConversion.JavaShortToBIN16(%s,%s,_offset);\n",
         		indent, bufferName, fieldName);
            return;
         case BINARY_16_UNSIGNED:
         	fprintf(pfp,"%sDataConversion.JavaIntToUNBIN16(%s,%s,_offset);\n",
         		indent, bufferName, fieldName);
            return;
         case BINARY_32_SIGNED:
         	fprintf(pfp,"%sDataConversion.JavaIntToBIN32(%s, %s, _offset);\n",
         		indent, bufferName, fieldName);
            return;
         case BINARY_32_UNSIGNED:
         	fprintf(pfp,"%sDataConversion.JavaLongToUNBIN32(%s, %s, _offset);\n",
         		indent, bufferName, fieldName);
            return;
         case BINARY_64_SIGNED:
         	fprintf(pfp,"%sDataConversion.JavaLongToBIN64(%s, %s, _offset);\n",
         		indent, bufferName, fieldName);
            return;
         case FLOAT32:
            fprintf(pfp,"%sDataConversion.StrToFloat(%s, String.valueOf(%s), _offset);\n",
 	          	indent, bufferName, fieldName);
            return;
         case FLOAT64:
            fprintf(pfp,"%sDataConversion.StrToDouble(%s, String.valueOf(%s), _offset);\n",
            	indent, bufferName, fieldName);
            return;
         case NUM_STR_TRAIL_EMB_SIGN:
         	fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
         	fprintf(pfp,"%sDataConversion.JavaStrToNumStr(%s, %s, _offset,%d, %d, 15);\n",
            	indent,	bufferName, fieldName, length, scale);
            return;
         case NUM_STR_TRAIL_SEP_SIGN:
         	fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
         	fprintf(pfp,"%sDataConversion.JavaStrToNumStr(%s, %s, _offset,%d, %d, 13);\n",
            	indent, bufferName, fieldName, length, scale);
           	return;
         case NUM_STR_LEAD_EMB_SIGN:
         	fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
            fprintf(pfp,"%sDataConversion.JavaStrToNumStr(%s, %s, _offset,%d, %d, 14);\n",
            	indent, bufferName, fieldName, length, scale);
           	return;
         case NUM_STR_LEAD_SEP_SIGN:
         	fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
         	fprintf(pfp,"%sDataConversion.JavaStrToNumStr(%s, %s, _offset,%d, %d, 12);\n",
            	indent, bufferName, fieldName, length, scale);
           	return;
         case LOGICAL_1:
         	fprintf(pfp,"%s%s[_offset] = %s;\n",
            	indent, bufferName, fieldName);
            return;
         case LOGICAL_2:
           	fprintf(pfp,"%sDataConversion.JavaShortToLogical2(%s, %s, _offset);\n",
            	indent, bufferName, fieldName);
            return;
         case LOGICAL_4:
          	fprintf(pfp,"%sDataConversion.JavaIntToLogical4(%s, %s, _offset);\n",
            	indent, bufferName, fieldName);
            return;
         case BINARY_8_SIGNED:
         case BINARY_8_UNSIGNED:
         	fprintf(pfp,"%sif(%s != null) \n",indent, fieldName);
         	if(languageType == C_LANG) {
            	fprintf(pfp,"%sDataConversion.JavaStrToBIN8(%s, %s, _offset);\n",
               		indent,  bufferName, fieldName);
            }
            if(languageType == COBOL_LANG) {
            	fprintf(pfp,"%sDataConversion.JavaStrToBIN8Cobol(%s, %s, _offset);\n",
               	indent,  bufferName, fieldName, length);
            }
            return;
	 	case ENUMERATION:
	 	case SQLLENGTHORINDICATOR:
		    fprintf(pfp,"%sDataConversion.JavaShortToBIN16(%s, %s, _offset);\n",
		          indent, bufferName, fieldName);
		    return;
        case BITSIGNED:
           	fprintf(pfp,"%sDataConversion.BITToTdmShort(%s, %s, _offset);\n",
            	indent, bufferName, fieldName);
            return;
        case BITUNSIGNED:
           	fprintf(pfp,"%sDataConversion.UBITToTdmShort(%s, %s, _offset); \n",
           		indent, bufferName, fieldName);
           	return;
         default: printf("Unsupported data type %d\n", item->detail_info.element_info.ddl_element_dc.host_data_type);
      }
	return;
}
//------------------------------------------------------------------------------
// writeInputConversionText
//		Writes the code that calls the data conversion routine for input data.
//------------------------------------------------------------------------------
void
ClassGen::writeInputConversionText(DETAIL_LIST *item, char *fieldName,
	char *indent, char *iterator, int classType)
{
	char tempFieldName[1024];
	char bufferName[15];
	short scale = 0;
	//change code
	int length = 0;
	short data_type = 0;

	scale = item->detail_info.element_info.ddl_element_dc.host_scale;
	length = item->detail_info.element_info.ddl_element_dc.host_length;
	data_type = item->detail_info.element_info.ddl_element_dc.host_data_type;

	strcpy(tempFieldName,fieldName);
	if(iterator != NULL) {
    	strcat(fieldName,"[");
       	strcat(fieldName,iterator);
       	strcat(fieldName,"]");
    }
    strcpy(bufferName,"_buffer");
    
    //soln_10-130423-7115
    if (( (data_type == BINARY_16_SIGNED) || (data_type == BINARY_16_UNSIGNED) || (data_type == BINARY_32_SIGNED) || (data_type == BINARY_32_UNSIGNED) || (data_type == BINARY_64_SIGNED))
	     && (item->detail_info.element_info.ddl_element_dc.host_scale > 0)){	     	
		 fprintf(pfp,"%s  try {\n", indent);
		 fprintf(pfp,"%s      %s = DataConversion.BINToJavaBigDecimal(%s, _offset, %d, %d, %d);\n",
         		indent, fieldName, bufferName, data_type, length, scale);
         printInputConversionErrors(pfp,fieldName,indent);
	     return;
	    }
     //soln_10-130423-7115

	switch(data_type)
    {
		case SQLVARCHAR:
		case SQLDATETIMEYEAR:
   		case SQLDATETIMEMONTH:
		case SQLDATETIMEYEARTOMONTH:
   		case SQLDATETIMEDAY:
   		case SQLDATETIMEMONTHTODAY:
   		case SQLDATETIMEYEARTODAY:
   		case SQLDATETIMEHOUR:
   		case SQLDATETIMEDAYTOHOUR:
   		case SQLDATETIMEMONTHTOHOUR:
   		case SQLDATETIMEYEARTOHOUR:
   		case SQLDATTIMEMINUTE:
   		case SQLDATETIMEHOURTOMINUTE:
   		case SQLDATETIMEDAYTOMINUTE:
   		case SQLDATETIMEMONTHTOMINUTE:
   		case SQLDATETIMEYEARTOMINUTE:
   		case SQLDATETIMESECOND:
   		case SQLDATETIMEMINUTETOSECOND:
   		case SQLDATETIMEHOURTOSECOND:
   		case SQLDATETIMEDAYTOSECOND:
   		case SQLDATETIMEMONTHTOSECOND:
   		case SQLDATETIMEYEARTOSECOND:
   		case SQLDATETIMEFRACTION:
   		case SQLDATETIMESECONDTOFRACTION:
   		case SQLDATETIMEMINUTETOFRACTION:
   		case SQLDATETIMEHOURTOFRACTION:
   		case SQLDATETIMEDAYTOFRACTION:
   		case SQLDATETIMEMONTHTOFRACTION:
   		case SQLDATETIMEYEARTOFRACTION:
   		case SQLINTERVALYEAR:
   		case SQLINTERVALMONTH:
   		case SQLINTERVALYEARTOMONTH:
   		case SQLINTERVALDAY:
   		case SQLINTERVALHOUR:
   		case SQLINTERVALDAYTOHOUR:
   		case SQLINTERVALMINUTE:
   		case SQLINTERVALHOURTOMINUTE:
   		case SQLINTERVALDAYTOMINUTE:
   		case SQLINTERVALSECOND:
   		case SQLINTERVALMINUTETOSECOND:
   		case SQLINTERVALHOURTOSECOND:
   		case SQLINTERVALDAYTOSECOND:
   		case SQLINTERVALFRACTION:
   		case SQLINTERVALSECONDTOFRACTION:
   		case SQLINTERVALMINUTETOFRACTION:
   		case SQLINTERVALHOURTOFRACTION:
		case SQLINTERVALDAYTOFRACTION:
      	case ALPHANUMERIC:
      	case NATIONALSTRING:

        	if (encodingString) {
            	fprintf(pfp,"%s%s\n", indent, encodingCode);
	    		encodingString = false;
			}
			fprintf(pfp,"%s  try {\n", indent);
           	if (encodingSpecified) {
				if(charStringType == NULLTERMINATED) {
					if(data_type == ALPHANUMERIC || data_type == NATIONALSTRING || data_type == SQLVARCHAR)
            			fprintf(pfp,"%s     %s = DataConversion.CStrToJavaStr(%s,_offset,_fieldSize,encodeSetting);\n",
            				indent, fieldName, bufferName);
					else
						fprintf(pfp,"%s     %s = DataConversion.CStrToJavaStr(%s,_offset,%d,encodeSettng);\n",
							indent, fieldName, bufferName, length);
            	} else {
					if(data_type == ALPHANUMERIC || data_type == NATIONALSTRING || data_type == SQLVARCHAR)
            			fprintf(pfp,"%s     %s = DataConversion.CobolStrToJavaStr(%s,_offset,_fieldSize,encodeSetting);\n",
            				indent, fieldName, bufferName);
					else
						fprintf(pfp,"%s     %s = DataConversion.CobolStrToJavaStr(%s,_offset,%d,encodeSetting);\n",
            				indent, fieldName, bufferName, length);
				}
			} else {
				if(charStringType == NULLTERMINATED) {
					if(data_type == ALPHANUMERIC || data_type == NATIONALSTRING || data_type == SQLVARCHAR)
				   		fprintf(pfp,"%s     %s = DataConversion.CStrToJavaStr(%s,_offset,_fieldSize);\n",
				   			indent, fieldName, bufferName);
					else
						fprintf(pfp,"%s     %s = DataConversion.CStrToJavaStr(%s,_offset,%d);\n",
				   			indent, fieldName, bufferName, length);
				} else {
					if(data_type == ALPHANUMERIC || data_type == NATIONALSTRING || data_type == SQLVARCHAR)
				    	fprintf(pfp,"%s     %s = DataConversion.CobolStrToJavaStr(%s,_offset,_fieldSize);\n",
            				indent, fieldName, bufferName);
					else
						fprintf(pfp,"%s     %s = DataConversion.CobolStrToJavaStr(%s,_offset,%d);\n",
            				indent, fieldName, bufferName, length);
				}
          	}
          	printInputConversionErrors(pfp,fieldName,indent);
           	break;
        case NUM_STR_UNSIGNED:
        	fprintf(pfp,"%s  try {\n", indent);
            fprintf(pfp,"%s     %s = DataConversion.NumStrToJavaStr(%s,_offset,%d, %d, 16);\n",
            	indent, fieldName, bufferName, length, scale);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_16_SIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s      %s = DataConversion.BIN16ToJavaShort(%s, _offset);\n",
         		indent, fieldName, bufferName);
         	printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_16_UNSIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s      %s = DataConversion.UNBIN16ToJavaInt(%s, _offset);\n",
         		indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_32_SIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s      %s = DataConversion.BIN32ToJavaInt(%s, _offset);\n",
         		indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_32_UNSIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s      %s = DataConversion.UNBIN32ToJavaLong(%s, _offset);\n",
         		indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_64_SIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s      %s = DataConversion.BIN64ToJavaLong(%s, _offset);\n",
         		indent, fieldName, bufferName);
         	printInputConversionErrors(pfp,fieldName,indent);
            break;
         case FLOAT32:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s         _tmp = DataConversion.Float32ToStr(%s,_offset); \n",
            	indent, bufferName);
            fprintf(pfp,"%s         %s = Float.valueOf(_tmp).floatValue();\n",
            	indent, fieldName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case FLOAT64:
         	fprintf(pfp,"%s  try {\n", indent);
    		fprintf(pfp,"%s          _tmp = DataConversion.Float64ToStr(%s,_offset); \n",
            	indent, bufferName);
            fprintf(pfp,"%s          %s = Double.valueOf(_tmp).doubleValue();\n",
            	indent, fieldName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case NUM_STR_TRAIL_EMB_SIGN:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s          %s = DataConversion.NumStrToJavaStr(%s,_offset, %d,(short) %d,(short) 15); \n",
            	indent, fieldName, bufferName, length, scale);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case NUM_STR_TRAIL_SEP_SIGN:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s          %s = DataConversion.NumStrToJavaStr(%s,_offset, %d,(short) %d,(short) 13); \n",
            	indent, fieldName, bufferName, length, scale);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case NUM_STR_LEAD_EMB_SIGN:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s          %s = DataConversion.NumStrToJavaStr(%s,_offset, %d,(short) %d,(short) 14); \n",
            	indent, fieldName,bufferName,length, scale);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case NUM_STR_LEAD_SEP_SIGN:
         	fprintf(pfp,"%s  try {\n", indent);
       		fprintf(pfp,"%s          %s = DataConversion.NumStrToJavaStr(%s,_offset, %d,(short) %d,(short) 12); \n",
            	indent, fieldName, bufferName, length,scale);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case LOGICAL_1:
         	fprintf(pfp,"%s          %s = %s[_offset]; \n",
            	indent, fieldName, bufferName);
            break;
         case LOGICAL_2:
         	fprintf(pfp,"%s  try {\n", indent);
      		fprintf(pfp,"%s          %s = DataConversion.Logical2ToJavaShort(%s,_offset); \n",
            	indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case LOGICAL_4:
         	fprintf(pfp,"%s  try {\n", indent);
   			fprintf(pfp,"%s          %s = DataConversion.Logical4ToJavaInt(%s,_offset); \n",
            	indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
         case BINARY_8_SIGNED:
         case BINARY_8_UNSIGNED:
         	fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s         %s = DataConversion.BIN8ToJavaStr(%s,_offset); \n",
              	indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
		 case ENUMERATION:
       	 case SQLLENGTHORINDICATOR:
       	 	fprintf(pfp,"%s  try {\n", indent);
		 	fprintf(pfp,"%s          %s = DataConversion.BIN16ToJavaShort(%s,_offset);\n",
            	   indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
			break;
          case BITSIGNED:
         	printf("ddl2java - WARNING: Unsupported data type in element %s; generating default conversion.\n",
            	item->detail_info.element_info.ddl_element_dc.ddl_element_name);
            fprintf(pfp,"%s  try {\n", indent);
            fprintf(pfp,"%s          %s = DataConversion.TdmShortToBIT(%s,_offset);\n",
            	   indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
         	break;
          case BITUNSIGNED:
         	printf("ddl2java - WARNING: Unsupported data type in element %s; generating default conversion.\n",
            	item->detail_info.element_info.ddl_element_dc.ddl_element_name);
            fprintf(pfp,"%s  try {\n", indent);
         	fprintf(pfp,"%s%s = DataConversion.TdmShortToUBIT(%s,_offset);\n",
            		indent, fieldName, bufferName);
            printInputConversionErrors(pfp,fieldName,indent);
            break;
          default: printf("Unsupported data type in element %s\n", item->detail_info.element_info.ddl_element_dc.ddl_element_name);
      }
      return;
}
void ClassGen::writeCheckBufferSize(DETAIL_LIST *element,int dataType,char *indent)
{
	switch(dataType) {
		case SQLVARCHAR:
		case ALPHANUMERIC:
      	case NATIONALSTRING:
			fprintf(pfp,"%sif(_offset >= _count) return;\n", indent);
			fprintf(pfp,"%s_fieldSize = Math.min(%d,_count - _offset);\n", indent,
				element->detail_info.element_info.ddl_element_dc.host_length);
			break;
		default:
			fprintf(pfp,"%sif(_offset + %d > _count) return;\n", indent,
				element->detail_info.element_info.ddl_element_dc.host_length);
	}
	return;
}
//
// Writes the text for an elementary data item
//
void ClassGen::writeElemNumericInstanceVar(int dataType,DETAIL_LIST *element) {
    //soln_10-130423-7115
	if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	     && (element->detail_info.element_info.ddl_element_dc.host_scale > 0)){	     
	     fprintf(pfp,"    public BigDecimal %s;\n", element->detail_info.qualName.Name);
	     return;
	}
	//soln_10-130423-7115
	switch(dataType) {
		case BINARY_16_SIGNED:
		case LOGICAL_2:
		case SQLLENGTHORINDICATOR:
		case ENUMERATION:
			fprintf(pfp,"    public short %s;\n", element->detail_info.qualName.Name);
			break;
		case BINARY_16_UNSIGNED:
		case BINARY_32_SIGNED:
		case LOGICAL_4:
		case BITSIGNED:
		case BITUNSIGNED:
			fprintf(pfp,"    public int %s;\n", element->detail_info.qualName.Name);
			break;
		case BINARY_32_UNSIGNED:
		case BINARY_64_SIGNED:
			fprintf(pfp,"    public long %s;\n", element->detail_info.qualName.Name);
			break;
		case FLOAT32:
			fprintf(pfp,"    public float %s;\n", element->detail_info.qualName.Name);
			break;
		case FLOAT64:
			fprintf(pfp,"    public double %s;\n", element->detail_info.qualName.Name);
			break;
		case LOGICAL_1:
			fprintf(pfp,"    public byte %s;\n", element->detail_info.qualName.Name);
			break;
		default:
	    	fprintf(pfp,"    public String %s;\n",
	           	element->detail_info.qualName.Name);
    }
}
void ClassGen::writeOccursNumericInstanceVar(int dataType, DETAIL_LIST *element) {
	int i;
	
	//soln_10-130423-7115
	if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	     && (element->detail_info.element_info.ddl_element_dc.host_scale > 0)){	     
	     fprintf(pfp,"    public BigDecimal %s[] = new BigDecimal[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);	
	    }
	//soln_10-130423-7115
    else{
	switch(dataType) {
		case BINARY_16_SIGNED:
		case LOGICAL_2:
		case SQLLENGTHORINDICATOR:
		case ENUMERATION:
			fprintf(pfp,"    public short %s[] = new short[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		case BINARY_16_UNSIGNED:
		case BINARY_32_SIGNED:
		case LOGICAL_4:
		case BITSIGNED:
		case BITUNSIGNED:
			fprintf(pfp,"    public int %s[] = new int[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		case BINARY_32_UNSIGNED:
		case BINARY_64_SIGNED:
			fprintf(pfp,"    public long %s[] = new long[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		case FLOAT32:
			fprintf(pfp,"    public float %s[] = new float[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		case FLOAT64:
			fprintf(pfp,"    public double %s[] = new double[%d];\n", element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		case LOGICAL_1:
			fprintf(pfp,"    public byte %s[] = new byte[%d];\n",element->detail_info.qualName.Name,
				element->detail_info.element_info.ddl_element_dc.occurs_max);
			break;
		default:
			fprintf(pfp,"    public String %s[] = {", element->detail_info.qualName.Name);
            for(i = 0; i <= (element->detail_info.element_info.ddl_element_dc.occurs_max - 2); i ++)
            {
            	fprintf(pfp,"new String(),\n");
			}
            fprintf(pfp,"new String()};\n");
	}
}
}
//pabi
//writeInitialValue now has a third parameter to collect the scale value of BigDecimal datatypes
void ClassGen::writeInitialValue(int dataType, char *Str, DETAIL_LIST *element) {
        
	//soln_10-130423-7115
	if (( (dataType == BINARY_16_SIGNED) || (dataType == BINARY_16_UNSIGNED) || (dataType == BINARY_32_SIGNED) || (dataType == BINARY_32_UNSIGNED) || (dataType == BINARY_64_SIGNED))
	     && (element->detail_info.element_info.ddl_element_dc.host_scale > 0)){
	     strcpy(Str,"null");
	    return;
	   }
	//soln_10-130423-7115
	
	switch(dataType) {
		case BINARY_16_SIGNED:
		case LOGICAL_2:
		case SQLLENGTHORINDICATOR:
		case ENUMERATION:
			strcpy(Str,"(short)0");
			break;
		case BINARY_16_UNSIGNED:
		case BINARY_32_SIGNED:
		case LOGICAL_4:
		case BITSIGNED:
		case BITUNSIGNED:
			strcpy(Str,"(int)0");
			break;
		case BINARY_32_UNSIGNED:
		case BINARY_64_SIGNED:
			strcpy(Str,"(long)0");
			break;
		case FLOAT32:
			strcpy(Str,"(float)0");
			break;
		case FLOAT64:
			strcpy(Str,"(double)0");
			break;
		case LOGICAL_1:
			strcpy(Str,"(byte)0");
			break;
		/*For Soln: 10-120828-4530 */
		case NUM_STR_UNSIGNED:
		    strcpy(Str,"\"0\"");
			break;
		/*For Soln: 10-120828-4530 */
		default:
// Begin of Changes for Solution Number 10-030319-4961 
//Initializing the string by SPACE/NULL depending on the option provided.
      //strcpy(Str,"null");
      switch(stringInitializationType) {
                   case NULL_TYPE:
                       strcpy(Str,"null");
                       break;
                   case SPACE_TYPE:
                       strcpy(Str,"\" \"");
                       break;
                           }	
// End of Changes for Solution Number 10-030319-4961 
	}
}
// ClassGen.cpp

