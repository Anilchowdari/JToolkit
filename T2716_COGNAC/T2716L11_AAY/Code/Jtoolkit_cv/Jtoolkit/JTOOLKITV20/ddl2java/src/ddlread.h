//------------------------------------------------------------------------
//      "TANDEM CONFIDENTIAL: NEED TO KNOW ONLY
//
//   Copyright (C) 1997, Tandem Computers Incorporated
//      Protected as an unpublished work.
//         All rights reserved.
//
//   The computer program listings, specifications and documentation
//   herein are the property of Tandem Computers Incorporated or a third
//   party supplier and shall not be reproduced, copied, disclosed, or
//   used in whole or in part for any reason without the prior express
//   written permission of Tandem Computers Incorporated."
//--------------------------------------------------------------------------
//
// Filename:  ddlread.h
//--------------------------------------------------------------------------
#ifndef __DDLREAD__
#define __DDLREAD__
#include "keysfile.h"
#include "ddllib.h"
class DDLDictionaryReader
{
public:
	DDLDictionaryReader();
   	~DDLDictionaryReader();

   	short openAndCheckDDLVersion(char *DDLSubvol, char * fileInError);

   	short readDDLElements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRePPtr);
	short readDDLRecords(readDictReq *readDictReqPtr,
   			readDictRep *readDictRepPtr);
   	short readRdfFile(readDefStructureReq* readDefStructureReqPtr,
 
  					readRdfStructureRep* readRdfStructureRepPtr);
//changed to support  DDL2 for sol:10-100611-1034
private:					
	short readDDL2Elements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRePPtr);
	short readDDL1Elements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRePPtr);
protected:
   	short findCommentText(ddl_object_id_def otfObject, char* commentText);
   	short findValueText(ddl_object_id_def otfObject, char* valueText);
	short findPICText(ddl_object_id_def otfObject, char* picText);
   	short findSourceDef(ddl_object_id_def sourceDefId, char* sourceDefName);
 	short getOTFText(ddl_object_id_def otfObject, char* text, short maxLen,
						short *textLen);
private:
   	KeySequencedFile *OBLFile;
   	KeySequencedFile *ODFFile;
   	KeySequencedFile *OTFFile;
   	KeySequencedFile *RDFFile;
   	unsigned short 	ddlVersion;

};

#endif // __DDLREAD__
