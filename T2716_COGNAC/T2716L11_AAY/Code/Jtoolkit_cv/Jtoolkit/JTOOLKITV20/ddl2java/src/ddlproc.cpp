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
// Filename:  ddlproc.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "ddlread.h"
#include "ddlproc.h"

//--------------------------------------------------------------------------
// DDLProcessor
//--------------------------------------------------------------------------
DDLProcessor::DDLProcessor()
{

} // DDLProcessor

//--------------------------------------------------------------------------
// DDLProcessor
//--------------------------------------------------------------------------
DDLProcessor::~DDLProcessor()
{

} // DDLProcessor  destructor

//---------------------------------------------------------------------------
void DDLProcessor::ddl_read_ddl_defs(char * requestBuf, char * replyBuf)
{
   	readDictReq 	*readDictReqPtr = (readDictReq*)requestBuf;
   	readDictRep 	*readDictRepPtr = (readDictRep*)replyBuf;
   	DDLDictionaryReader    	ddlReader;
   	char    	DDLSubvol[37], 	fileName[37];
   	short error = 0;

   	memcpy(DDLSubvol, readDictReqPtr->dict_subvol, sizeof(readDictReqPtr->dict_subvol));
   	DDLSubvol[36] = 0;
   	strtok(DDLSubvol," ");  // put null at first blank
   	error = ddlReader.openAndCheckDDLVersion(DDLSubvol, fileName);

   	if(error != 0)  {
      	if(error == EC_DDL_VERSION_NOT_SUPPORTED)  {
      		readDictRepPtr->status = EC_DDL_VERSION_NOT_SUPPORTED;
      	} else {
      		readDictRepPtr->status = EC_HOST_FILE_ERROR;
      	}
    	return;
   	}
   	error = ddlReader.readDDLRecords(readDictReqPtr, readDictRepPtr);
   	if(error == 0) {
   	  	readDictRepPtr->status = EC_OK;
   	} else {
	   	readDictRepPtr->status = EC_HOST_FILE_ERROR;
	}
   	return;
} // ddl_read_ddl_defs

//---------------------------------------------------------------------------
void DDLProcessor::ddl_read_ddl_elements(char * requestBuf, char * replyBuf)
{
   	readDefStructureReq *readDefStructureReqPtr = (readDefStructureReq*)requestBuf;
   	readDefStructureRep *readDefStructureRepPtr = (readDefStructureRep*)replyBuf;
   	readRdfStructureRep *readRdfStructureRepPtr;
   	DDLDictionaryReader ddlReader;
   	char    	DDLSubvol[37], 	fileName[37];
   	short   	error;

   	memcpy(DDLSubvol, readDefStructureReqPtr->dict_subvol, sizeof(readDefStructureReqPtr->dict_subvol));
   	DDLSubvol[36] = 0;
   	strtok(DDLSubvol," ");  // put null at first blank
   	error = ddlReader.openAndCheckDDLVersion(DDLSubvol, fileName);

   	if(error != 0)   {
      	if(error == EC_DDL_VERSION_NOT_SUPPORTED) {
      		readDefStructureRepPtr->status = EC_DDL_VERSION_NOT_SUPPORTED;
      	} else {
      		readDefStructureRepPtr->status = EC_HOST_FILE_ERROR;
        }
    	return;
   	}
   	error = ddlReader.readDDLElements(readDefStructureReqPtr, readDefStructureRepPtr);
   	if(error == 1) {
		readRdfStructureRepPtr = (readRdfStructureRep * ) malloc(sizeof (readRdfStructureRep));
		if(readRdfStructureRepPtr == NULL) {
   			printf("ddl2java - Error: Unable to obtain memory for ddl structure information.\n");
   			readDefStructureRepPtr->status = EC_HOST_FILE_ERROR;
      		return;
   		}
		error = ddlReader.readRdfFile(readDefStructureReqPtr, readRdfStructureRepPtr);
		if(error == 0) {
			readDefStructureReqPtr->ddl_object_id = readRdfStructureRepPtr->ddl_object_id;
			readDefStructureReqPtr->next_ddl_element_id = 0;
			error = ddlReader.readDDLElements(readDefStructureReqPtr,
				readDefStructureRepPtr);
		}
	}
   	if(error == 0)  {
   		readDefStructureRepPtr->status = EC_OK;
   	} else {
	   	readDefStructureRepPtr->status = EC_HOST_FILE_ERROR;
	}
   	return;
} // ddl_read_ddl_elements
//------------------------------------------------------------------------------------
// ddlproc.cpp
