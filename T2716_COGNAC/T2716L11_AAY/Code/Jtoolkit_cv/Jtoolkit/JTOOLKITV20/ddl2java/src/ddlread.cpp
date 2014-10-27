//--------------------------------------------------------------------------
// Copyright 2003
// Hewlett-Packard Development Company, L.P.  
// Copyright 2002 Compaq Computer Corporation
// Protected as an unpublished work.
// All rights reserved.
//
// The computer program listings, specifications and
// documentation herein are the property of Compaq Computer
// Corporation and successor entities such as Hewlett-Packard
// Development Company, L.P., or a third party supplier and
// shall not be reproduced, copied, disclosed, or used in whole
// or in part for any reason without the prior express written
// permission of Hewlett-Packard Development Company, L.P.             
//
//--------------------------------------------------------------------------
//
//
// Filename:  ddlread.cpp
//--------------------------------------------------------------------------
#include "ddl2java.h"
#include "ddlread.h"
#ifndef __TANDEM
#include "ddldef.h"
#include "ddldef1.h"
#include <winsock2.h>
#include "ddl.h"
#endif

//--------------------------------------------------------------------------
// DDLDictionaryReader
//--------------------------------------------------------------------------
DDLDictionaryReader::DDLDictionaryReader()
{
   OBLFile = NULL;
   ODFFile = NULL;
   OTFFile = NULL;
} // DDLDictionaryReader

//--------------------------------------------------------------------------
// DDLDictionaryReader
//--------------------------------------------------------------------------
DDLDictionaryReader::~DDLDictionaryReader()
{
   if(OBLFile != NULL)
   		delete OBLFile;
   if(ODFFile != NULL)
   		delete ODFFile;
   if(OTFFile != NULL)
   		delete OTFFile;

} // DDLDictionaryReader  destructor
//--------------------------------------------------------------------------
// findCommentText
//--------------------------------------------------------------------------
short DDLDictionaryReader::findCommentText(ddl_object_id_def otfObject,
	char* commentText)
{
	short
   	textLen,
   	error;
   	error = getOTFText(otfObject, commentText, DDL_COMMENT_SIZE - 1, &textLen);
   	commentText[textLen] = 0; // null terminated.  Field is wide enough for the null.
   	return error;
} // findCommentText

//--------------------------------------------------------------------------
// findValueText
//--------------------------------------------------------------------------
short DDLDictionaryReader::findValueText(ddl_object_id_def otfObject,
	char* valueText)
{
	short
   	valueLen,
   	error;
   	error = getOTFText(otfObject, valueText, DDL_VALUE_SIZE - 1, &valueLen);
   	valueText[valueLen] = 0; // null terminated.  Field is wide enough for the null.
   	return error;
} // findValueText

//--------------------------------------------------------------------------
// findPICText
//--------------------------------------------------------------------------
short DDLDictionaryReader::findPICText(ddl_object_id_def otfObject,
	char* picText)
{
   	short
   	textLen,
   	error;
   	error = getOTFText(otfObject, picText, DDL_PIC_TEXT_SIZE, &textLen);
   	if(error != 0)
   		return error;

   	if(textLen == DDL_PIC_TEXT_SIZE)  {
   		memcpy(&picText[DDL_PIC_TEXT_SIZE-4], "...", 3);
   	}
   	return 0;
} // findPICText

//--------------------------------------------------------------------------
// findSourceDef
//--------------------------------------------------------------------------
short
DDLDictionaryReader::findSourceDef(ddl_object_id_def sourceDefId, char* sourceDefName)
{
	short countRead, error;
   	char  keyspec[4];
    ODF_DEF 	odf_rec;

   	memcpy(keyspec, &sourceDefId,  4);
   	error = ODFFile->keyposition(keyspec, 0, 4, KEYPOS_EXACT);
   	if(error != 0)  {
    	printf("ddl2java - Error: keyposition error %d occurred trying to find source def.\n",
      		error);
   		return error;
   	}
	error = ODFFile->read((char*)&odf_rec, sizeof(ODF_DEF), &countRead);
	if(error != 0 && error != FILEERROR_EOF) {
    	printf("ddl2java - Error: read error %d occurred trying to find source def.\n", error);
      	return error;
   	}
   	if(error == FILEERROR_EOF) {
    	printf("ddl2java - Error: Source def not found.\n");
      	return 0; // just going to leave it blank
   	}
   	// have the record
   	memcpy(sourceDefName, odf_rec.identifier.name, DDL_NAME_SIZE);
   	return 0;
} // findSourceDef

//--------------------------------------------------------------------------
// getOTFText
//--------------------------------------------------------------------------
short DDLDictionaryReader::getOTFText(ddl_object_id_def otfObject,
	char* text, short maxLen, short *textLen)
{
	OTFD30_DEF    	otfD30_rec;
 	OTF_DEF 	   	otfPreD30_rec;
   	short    		error, countRead;
   	char 		   	*ptr;

	error = OTFFile->keyposition((char*)&otfObject, 0, 4, KEYPOS_GENERIC);

   	if(error != 0)  {
   		printf("ddl2java - Error: keyposition error %d on OTF file.\n", error);
    	return error;
   	}

   	if(ddlVersion == 6)  {
	   	error = OTFFile->read((char*)&otfPreD30_rec, sizeof(OTF_DEF), &countRead);
      	if(error != 0)  {
			*textLen = 0;
         	return error;
      	}
      	// have a record
      	ptr = otfPreD30_rec.text_line;
      	*textLen = otfPreD30_rec.text_len;
   	} else  {
	   	error = OTFFile->read((char*)&otfD30_rec, sizeof(OTFD30_DEF),
	   		&countRead);
	    if(error != 0)  {
	  		*textLen = 0;
        	return error;
      	}
      	// have a record
      	ptr = otfD30_rec.text_line;
      	*textLen = otfD30_rec.text_len;
   }

   if(*textLen > maxLen)
   		*textLen = maxLen;
   memcpy(text, ptr, *textLen );
   return 0;
} // getOTFText

//--------------------------------------------------------------------------
// openAndCheckDDLVersion
//--------------------------------------------------------------------------
short DDLDictionaryReader::openAndCheckDDLVersion(char *DDLSubvol,
	char * fileInError)
{
	short error;
   	char DICTFileName[37];
   	DDF_DEF ddf_rec;
   	short countread;
   	KeySequencedFile DDFFile;

   	strcpy(DICTFileName, DDLSubvol);
   	strcat(DICTFileName,DDLDICTDDF);
   	strcpy(fileInError, DICTFileName);
   	error = DDFFile.openFileReadOnly(DICTFileName);

   	if(error != 0) {
   		printf("ddl2java - Error: Open error %d on DICTDDF.\n", error);
   		return error;
   	}
	error =  DDFFile.read((char*)&ddf_rec, sizeof(DDF_DEF), &countread);

   	DDFFile.closeFile();
   	if(error != 0) {
   		printf("ddl2java - Error: Read error %d on DICTDDF.\n", error);
   		return error;
   	}
   	ddlVersion = ddf_rec.version;
   	if(ddlVersion < MINDDLVERSION || ddlVersion > MAXDDLVERSION) {
   		printf("ddl2java - Error: The version %d of the dictionary is unsupported.\n",
      	ddlVersion);
      return EC_DDL_VERSION_NOT_SUPPORTED;
   	}

  	// open other files
   	strcpy(DICTFileName, DDLSubvol);
   	strcat(DICTFileName,DDLDICTOBL);
   	strcpy(fileInError, DICTFileName);
   	OBLFile = new KeySequencedFile();
   	error = OBLFile->openFileReadOnly(DICTFileName);
   	if(error != 0)  {
   		printf("ddl2java - Error: Open error %d on DICTOBL.\n", error);
   		return error;
   	}

   	strcpy(DICTFileName, DDLSubvol);
   	strcat(DICTFileName,DDLDICTODF);
   	strcpy(fileInError, DICTFileName);
   	ODFFile = new KeySequencedFile();
   	error = ODFFile->openFileReadOnly(DICTFileName);
   	if(error != 0) {
   		printf("ddl2java - Error: Open error %d on DICTODF.\n", error);
   		return error;
   	}

   	strcpy(DICTFileName, DDLSubvol);
   	strcat(DICTFileName,DDLDICTOTF);
   	strcpy(fileInError, DICTFileName);
   	OTFFile = new KeySequencedFile();
   	error = OTFFile->openFileReadOnly(DICTFileName);
   	if(error != 0)  {
   		printf("ddl2java - Error: open error %d on DICTOTF.\n", error);
      	return error;
   	}
   	strcpy(DICTFileName, DDLSubvol);
   	strcat(DICTFileName,DDLDICTRDF);
   	strcpy(fileInError, DICTFileName);
	RDFFile = new KeySequencedFile();
	error = RDFFile->openFileReadOnly(DICTFileName);


 	return 0;
} // openAndCheckDDLVersion

//--------------------------------------------------------------------------
// readDDLElements
//--------------------------------------------------------------------------
short DDLDictionaryReader::readDDLElements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRepPtr)
{
	// keyposition generic using the IPC Object id given
   	// read all the records.
//changed to support  DDL2 for sol:10-100611-1034
	if(ddlVersion==9)
	{
		//OBL_DEF_V2 obl_rec;
		return readDDL2Elements( readDefStructureReqPtr,readDefStructureRepPtr);
	}
	else
	{
		//OBL_DEF obl_rec;
		return readDDL1Elements( readDefStructureReqPtr,readDefStructureRepPtr);
	}
   	//return 0;
} //  readDDLElements

//#################################

short DDLDictionaryReader::readDDL2Elements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRepPtr)
{
	// keyposition generic using the IPC Object id given
   	// read all the records.

   	bool all_done = false;
   	bool first_read = true;
   	short numRecs = 0,  countRead, keyPosLen, error;
   	ddl_identifier 	ddlElementId;
	OBL_DEF_V2 obl_rec;
	
	ddlElementId.ddl_object_id = readDefStructureReqPtr->ddl_object_id;
   	ddlElementId.ddl_element_id = readDefStructureReqPtr->next_ddl_element_id;
   	if(ddlElementId.ddl_element_id == 0)
   		keyPosLen = sizeof(ddl_object_id_def);
   	else  {
   		keyPosLen = sizeof(ddl_identifier);
      	keyPosLen = keyPosLen + 256 * sizeof(ddl_object_id_def);
   	}

   	error = OBLFile->keyposition((char*)&ddlElementId,
               							0, keyPosLen, KEYPOS_GENERIC);
   	if(error != 0)  {
    	printf("ddl2java - Internal Error: %d occurred while trying to position to record/def.\n",
      		error);
      	printf("ddl2java - Error ocurred on DICTOBL file");
        readDefStructureRepPtr->status = EC_HOST_FILE_ERROR;
       	return error;
   	}
   	readDefStructureRepPtr->more_flag = 'N';
   	readDefStructureRepPtr->next_ddl_element_id = 0;

   	while (!all_done)  {

   		error = OBLFile->read((char*)&obl_rec, sizeof(OBL_DEF_V2), &countRead);

      	if(error > 1)  {
         	printf("ddl2java - Internal Error: %d occurring reading DICTOBL file.\n",
         		error);
         	/* We don't need to swap bytes but changed code to simple assignment */
         	readDefStructureRepPtr->status  = EC_HOST_FILE_ERROR;
            return error;
      	}
      	if(error == FILEERROR_EOF && first_read)
      		return error;

      	first_read = false;
      	if(error == FILEERROR_EOF)
			break;

      	// we read one more to check if more
      	if(numRecs == MAX_DDL_ELEMENTS || numRecs >= readDefStructureReqPtr->num_wanted ) {
      		readDefStructureRepPtr->more_flag = 'Y';
         	readDefStructureRepPtr->next_ddl_element_id = obl_rec.identifier.element;
         	break;
      	}
      	// this is a record we want
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.seq_num
      		= obl_rec.identifier.element;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.element_type
      		= obl_rec.obj_class;
      	memcpy(readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.ddl_element_name,
      		obl_rec.local_name, DDL_NAME_SIZE);

      	fixDDLName(readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.ddl_element_name,
                 DDL_NAME_SIZE);
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.level
      		= obl_rec.level;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_offset
      		= obl_rec.offset;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_length
      		= obl_rec.element_size;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_data_type
      		= obl_rec.structure;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_precision
      		= obl_rec.interval_leading_precision;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_scale
      		= obl_rec.scale;
		
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.occurs_max
      		= obl_rec.occurs_max;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.occurs_depends_upon
      		= obl_rec.occurs_element;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.element_redefined
      		= obl_rec.element_redefined;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.sqlnullable
      		= obl_rec.sqlnullable_flag;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.needs_discriminator
      		= -1;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.owns_discriminator
      		= -1;
      	readDefStructureRepPtr->element_info[numRecs].is_user_filler
      		= obl_rec.user_defined_filler;
      	readDefStructureRepPtr->element_info[numRecs].is_ddl_filler
      		= 'N';
      	if(obl_rec.user_defined_filler != 'Y' ) {
      		if(strncmp(obl_rec.local_name, "FILLER ", 7) == 0)
      			readDefStructureRepPtr->element_info[numRecs].is_ddl_filler = 'Y';
      	}
      	//
      	memset(readDefStructureRepPtr->element_info[numRecs].ddl_pic_text, ' ', DDL_PIC_TEXT_SIZE);
      	memset(readDefStructureRepPtr->element_info[numRecs].ddl_comment, ' ' , DDL_COMMENT_SIZE );
      	memset(readDefStructureRepPtr->element_info[numRecs].source_def, ' ' , DDL_NAME_SIZE );
      	if(obl_rec.picture_text != 0)  {
      		error = findPICText(obl_rec.picture_text, readDefStructureRepPtr->element_info[numRecs].ddl_pic_text);
      	}
      	if(!error && obl_rec.comment_text != 0)  {
      		error = findCommentText(obl_rec.comment_text, readDefStructureRepPtr->element_info[numRecs].ddl_comment);
      	}
      	if(!error && obl_rec.source_def != 0)  {
      		error = findSourceDef( obl_rec.source_def, readDefStructureRepPtr->element_info[numRecs].source_def);
      	}
      	if(!error && obl_rec.value_text != 0) {
         	error = findValueText(obl_rec.value_text, readDefStructureRepPtr->element_info[numRecs].ddl_value_text);
      	}
      	if(error)  {
         	printf("ddl2java - Internal Error: %d occurred while reading DICTOTF.\n",
         		error);
         	return error;
      	}
      	numRecs++;
	}

   	readDefStructureRepPtr->num_returned = numRecs;

   	if(numRecs == 0) {
   		readDefStructureRepPtr->status = EC_NO_RECS_FOUND;
   		printf("ddl2java - Error: Unable to find DDL record/def.\n");
   	}
   	return 0;
} //  readDDLElements



//##################################

short DDLDictionaryReader::readDDL1Elements(readDefStructureReq* readDefStructureReqPtr,
   					readDefStructureRep* readDefStructureRepPtr)
{
	// keyposition generic using the IPC Object id given
   	// read all the records.

   	bool all_done = false;
   	bool first_read = true;
   	short numRecs = 0,  countRead, keyPosLen, error;
   	ddl_identifier 	ddlElementId;
	OBL_DEF obl_rec;
	
    ddlElementId.ddl_object_id = readDefStructureReqPtr->ddl_object_id;
   	ddlElementId.ddl_element_id = readDefStructureReqPtr->next_ddl_element_id;
   	if(ddlElementId.ddl_element_id == 0)
   		keyPosLen = sizeof(ddl_object_id_def);
   	else  {
   		keyPosLen = sizeof(ddl_identifier);
      	keyPosLen = keyPosLen + 256 * sizeof(ddl_object_id_def);
   	}

   	error = OBLFile->keyposition((char*)&ddlElementId,
               							0, keyPosLen, KEYPOS_GENERIC);
   	if(error != 0)  {
    	printf("ddl2java - Internal Error: %d occurred while trying to position to record/def.\n",
      		error);
      	printf("ddl2java - Error ocurred on DICTOBL file");
        readDefStructureRepPtr->status = EC_HOST_FILE_ERROR;
       	return error;
   	}
   	readDefStructureRepPtr->more_flag = 'N';
   	readDefStructureRepPtr->next_ddl_element_id = 0;

   	while (!all_done)  {

   		error = OBLFile->read((char*)&obl_rec, sizeof(OBL_DEF), &countRead);

      	if(error > 1)  {
         	printf("ddl2java - Internal Error: %d occurring reading DICTOBL file.\n",
         		error);
         	/* We don't need to swap bytes but changed code to simple assignment */
         	readDefStructureRepPtr->status  = EC_HOST_FILE_ERROR;
            return error;
      	}
      	if(error == FILEERROR_EOF && first_read)
      		return error;

      	first_read = false;
      	if(error == FILEERROR_EOF)
			break;

      	// we read one more to check if more
      	if(numRecs == MAX_DDL_ELEMENTS || numRecs >= readDefStructureReqPtr->num_wanted ) {
      		readDefStructureRepPtr->more_flag = 'Y';
         	readDefStructureRepPtr->next_ddl_element_id = obl_rec.identifier.element;
         	break;
      	}
      	// this is a record we want
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.seq_num
      		= obl_rec.identifier.element;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.element_type
      		= obl_rec.obj_class;
      	memcpy(readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.ddl_element_name,
      		obl_rec.local_name, DDL_NAME_SIZE);

      	fixDDLName(readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.ddl_element_name,
                 DDL_NAME_SIZE);
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.level
      		= obl_rec.level;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_offset
      		= obl_rec.offset;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_length
      		= obl_rec.element_size;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_data_type
      		= obl_rec.structure;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_precision
      		= obl_rec.interval_leading_precision;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.host_scale
      		= obl_rec.scale;
		
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.occurs_max
      		= obl_rec.occurs_max;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.occurs_depends_upon
      		= obl_rec.occurs_element;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.element_redefined
      		= obl_rec.element_redefined;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.sqlnullable
      		= obl_rec.sqlnullable_flag;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.needs_discriminator
      		= -1;
      	readDefStructureRepPtr->element_info[numRecs].ddl_element_dc.owns_discriminator
      		= -1;
      	readDefStructureRepPtr->element_info[numRecs].is_user_filler
      		= obl_rec.user_defined_filler;
      	readDefStructureRepPtr->element_info[numRecs].is_ddl_filler
      		= 'N';
      	if(obl_rec.user_defined_filler != 'Y' ) {
      		if(strncmp(obl_rec.local_name, "FILLER ", 7) == 0)
      			readDefStructureRepPtr->element_info[numRecs].is_ddl_filler = 'Y';
      	}
      	//
      	memset(readDefStructureRepPtr->element_info[numRecs].ddl_pic_text, ' ', DDL_PIC_TEXT_SIZE);
      	memset(readDefStructureRepPtr->element_info[numRecs].ddl_comment, ' ' , DDL_COMMENT_SIZE );
      	memset(readDefStructureRepPtr->element_info[numRecs].source_def, ' ' , DDL_NAME_SIZE );
      	if(obl_rec.picture_text != 0)  {
      		error = findPICText(obl_rec.picture_text, readDefStructureRepPtr->element_info[numRecs].ddl_pic_text);
      	}
      	if(!error && obl_rec.comment_text != 0)  {
      		error = findCommentText(obl_rec.comment_text, readDefStructureRepPtr->element_info[numRecs].ddl_comment);
      	}
      	if(!error && obl_rec.source_def != 0)  {
      		error = findSourceDef( obl_rec.source_def, readDefStructureRepPtr->element_info[numRecs].source_def);
      	}
      	if(!error && obl_rec.value_text != 0) {
         	error = findValueText(obl_rec.value_text, readDefStructureRepPtr->element_info[numRecs].ddl_value_text);
      	}
      	if(error)  {
         	printf("ddl2java - Internal Error: %d occurred while reading DICTOTF.\n",
         		error);
         	return error;
      	}
      	numRecs++;
	}

   	readDefStructureRepPtr->num_returned = numRecs;

   	if(numRecs == 0) {
   		readDefStructureRepPtr->status = EC_NO_RECS_FOUND;
   		printf("ddl2java - Error: Unable to find DDL record/def.\n");
   	}
   	return 0;
} //  readDDLElements


//--------------------------------------------------------------------------
// readDDLRecords
//--------------------------------------------------------------------------
short
DDLDictionaryReader::readDDLRecords(readDictReq *readDictReqPtr,
   			readDictRep *readDictRepPtr)
{
	// keyposition approx to start
   	// 	 if initial read, use "ID" + start_def_characters
   	//   	else use last object id
   	// read thru all "ID" records stopping when record > end_def_characters
   	// When end of "ID" records hit, skip into "RD" with "RD" + start_def_characters
   	// read thru all "RD" records stopping when record > end_def_characters

   	bool 	all_done = false;
   	short   	numRecs = 0, numCharsEndIPC,  numCharsStartIPC,  numCharsItem,
      			countRead,   i, error;
   	char    	*ptr;
   	char		endIPC[ DDL_NAME_SIZE + 1];
   	char		startIPC[ DDL_NAME_SIZE + 1];
   	char     ddlSubvol[HOST_FILE_SIZE+1];
   	char		keyspec[sizeof(ddl_name_def) + 3];
   	ODF_DEF  odf_rec;


   	ptr = ODFFile->getFileName();

   	ddlSubvol[HOST_FILE_SIZE] = 0;
   	strcpy(ddlSubvol, ptr);
   	ptr = strrchr(ddlSubvol,'.');
   	*ptr = 0;
   	memset(readDictRepPtr->ddl_subvol_expanded, ' ', HOST_SUBVOL_SIZE);
   	memcpy(readDictRepPtr->ddl_subvol_expanded, ddlSubvol, strlen(ddlSubvol));
   	memcpy(endIPC, readDictReqPtr->end_def_characters, DDL_NAME_SIZE);
   	endIPC[DDL_NAME_SIZE] = 0;
   	numCharsEndIPC = DDL_NAME_SIZE;

   	for(i=DDL_NAME_SIZE-1;i>=0;i--) {
   		if(endIPC[i] != ' ')  {
      		numCharsEndIPC = i + 1;
         	break;
      	}
   	}
   	if(endIPC[0] == ' ')  {
	  	endIPC[0] = 'Z';
      	numCharsEndIPC = 1;
   	}
   	memcpy(startIPC, readDictReqPtr->start_def_characters, DDL_NAME_SIZE);
   	startIPC[DDL_NAME_SIZE] = 0;
   	numCharsStartIPC = DDL_NAME_SIZE;

   	for(i=DDL_NAME_SIZE-1;i>=0;i--) {
   		if(startIPC[i] != ' ')  {
      		numCharsStartIPC = i + 1;
         	break;
      	}
   	}
   	if(startIPC[0] == ' ') {
   		startIPC[0] = 'A';
      	numCharsStartIPC = 1;
   	}
   	// we are treating the odf_id as a two byte character field
   	if(memcmp(readDictReqPtr->next_ddl_ODF_key.odf_id, "  " , 2) != 0)  // reading more
   	{
      	memcpy(keyspec,readDictReqPtr->next_ddl_ODF_key.odf_id, 2);
		memcpy(&keyspec[2],readDictReqPtr->next_ddl_ODF_key.def_name,
      		sizeof(ddl_name_def));
      	keyspec[sizeof(ddl_name_def) + 2] = 0;
   	} else  { // we are just starting
        memcpy(keyspec,"ID", 2);
    	memcpy(&keyspec[2],readDictReqPtr->start_def_characters,
      		sizeof(ddl_name_def));
      	keyspec[sizeof(ddl_name_def) + 2] = 0;
   	}

   	error = ODFFile->keyposition(keyspec, DDLODFALTKEY,
   		sizeof(ddl_name_def) + 2 ,  KEYPOS_APPROX);

   	if(error != 0) {
    	printf("ddl2java - Error: Unable to find record/def; keyposition error: %d.\n",
      		error);
      	readDictRepPtr->status = error;
   		return error;
   	}
   	readDictRepPtr->more_flag = 'N';
   	memset(readDictRepPtr->next_ddl_ODF_key.odf_id, ' ', 2);
   	memset(readDictRepPtr->next_ddl_ODF_key.def_name, ' ', DDL_NAME_SIZE);

   	while (!all_done)  {
	   	error = ODFFile->read((char*)&odf_rec, sizeof(ODF_DEF), &countRead);

      	if(error > 1)  {
         	printf("ddl2java - Internal Error on ODF file: read error %d.\n", error);
         	readDictRepPtr->status = error;
           	return error;
      	}
      	if(error == FILEERROR_EOF)
      		break;

      	// we read one more to check if more
      	if(numRecs == MAX_DDL_IPCS || numRecs >= readDictReqPtr->num_wanted )       {
      		readDictRepPtr->more_flag = 'Y';
         	memcpy(readDictRepPtr->next_ddl_ODF_key.odf_id, odf_rec.identifier.obj_type, 2);
         	memcpy(readDictRepPtr->next_ddl_ODF_key.def_name, odf_rec.identifier.name, DDL_NAME_SIZE);
         	break;
      	}
      	// see if record is after the end_def_characters
	  	for(i=DDL_NAME_SIZE-1;i>=0;i--)  {
	     		if(odf_rec.identifier.name[i] != ' ') {
	           		numCharsItem = i + 1; /* Sol: 10-030604-6884 */
	           		break;
	        	}
		}
                /* Sol:  10-030604-6884 Changed If condition */
      		if(memcmp(odf_rec.identifier.name, endIPC, numCharsItem) > 0)  {
      		if(strncmp(odf_rec.identifier.obj_type, "ID", 2) == 0) {
         		// reposition into "RD" records and continue
            	memcpy(keyspec,"RD", 2);
            	memcpy(&keyspec[2],readDictReqPtr->start_def_characters,
                  sizeof(ddl_name_def));
            	keyspec[sizeof(ddl_name_def) + 2] = 0;
            	error = ODFFile->keyposition(keyspec,
						DDLODFALTKEY, sizeof(ddl_name_def) + 2 ,
                        	KEYPOS_APPROX);
            	if(error != 0)  {
               		printf("ddl2java - Internal error on ODF file: keyposition error %d.\n", error);
               		readDictRepPtr->status = error;
               		return error;
            	}
            	continue;
         	}
         	break; // all done
      	}
      	// see if it is before the start chars
        /* Sol:10-030604-6884 Changed numCharsStartIPC to numCharsStartIPC-1 */
      	if(memcmp(odf_rec.identifier.name, startIPC, numCharsStartIPC-1) < 0)
      		continue;
      	// this may be a record we want
      	readDictRepPtr->def_info[numRecs].ddl_object_id = odf_rec.object;
      	memcpy(readDictRepPtr->def_info[numRecs].ddl_ODF_key.odf_id,
      			odf_rec.identifier.obj_type, 2);
      	memcpy(readDictRepPtr->def_info[numRecs].ddl_ODF_key.def_name ,
      			odf_rec.identifier.name, DDL_NAME_SIZE);
      	fixDDLName(readDictRepPtr->def_info[numRecs].ddl_ODF_key.def_name,
                 DDL_NAME_SIZE);
      	numRecs++;
   	}
    readDictRepPtr->num_returned = numRecs;
    if(numRecs == 0) {
   		readDictRepPtr->status = EC_NO_RECS_FOUND;
   		printf("ddl2java - Error: no matching DDL records/defs found.\n");
   	}

  	return 0;

} // readDDLRecords
//--------------------------------------------------------------------------
// readDDLElements
//--------------------------------------------------------------------------
short DDLDictionaryReader::readRdfFile(readDefStructureReq* readDefStructureReqPtr,
   					readRdfStructureRep* readRdfStructureRepPtr)
{
	// keyposition generic using the IPC Object id given

   	short countRead, keyPosLen, error;
   	rdf_identifier_def 	rdf_identifier;

   	RDF_DEF    	rdf_rec;

    rdf_identifier.object = readDefStructureReqPtr->ddl_object_id;
   	keyPosLen = sizeof(ddl_object_id_def);

   	error = RDFFile->keyposition((char*)&rdf_identifier,
               							0, keyPosLen, KEYPOS_EXACT);
   	if(error != 0)  {
    	printf("ddl2java - Internal Error: %d occurred while trying to position to record/def.\n",
      		error);
      	printf("ddl2java - Error ocurred on DICTRDF file");
        readRdfStructureRepPtr->status = EC_HOST_FILE_ERROR;
       	return error;
   	}

   	error = RDFFile->read((char*)&rdf_rec, sizeof(RDF_DEF), &countRead);
   	if(error > 1)  {
	   	printf("ddl2java - Internal Error on RDF file: read error %d.\n", error);
	   	readRdfStructureRepPtr->status = error;
	   	return error;
    }
    readRdfStructureRepPtr->status = error;
    readRdfStructureRepPtr->ddl_object_id = rdf_rec.def_number;
    return error;
}

// end ddlread.cpp
