/*---------------------------------------------------------------------------
 * Copyright 2003
 * Hewlett-Packard Development Company, L.P.
 * Copyright 2002 Compaq Computer Corporation
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 *-------------------------------------------------------------------------*/  

#include <sys/types.h>

#include <pthread.h>

#include <string.h>
#include <stdio.h>

#include "com_tandem_ext_enscribe_EnscribeFileAttributes.h"

#include "enscribejni.h"
#include "jEcmaFuncs.h"
#include <cextdecs.h>

extern short jEDebugFlag;

/* ADDED for soln# 10-111122-0670*/
#define NON_SQL_FILES 0

#define INFORESULTSSIZE 3000
#define INFOSTRINGSIZE 25000 /* need room to turn numbers into ascii values */

/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
void jEBuildInfoString(char *infoString, int *infoStringLen,
					    short* infoList, short numItems,
					    short *infoResults, short infoResultsLen,
						short *numPartitions, short *numAltKeys,
						short *numAltKeyFiles,
						short bExpectingVariableData)
{

	char
		valueString[1000],
		addString[2000],
		*cptr = (char*)infoResults;
	int
		i,
		j,
		item,
		bytesConsumed,
		*iptr;
	unsigned int *uiptr;
	long long
		*lptr;
	short
		dateTime[8],
		valueLen,
		currentKeyLen = 0,
		primaryKeyLen = 0,
		*sptr = infoResults;

	typedef struct attributebitfields_
	{
		unsigned short bit0 : 1;
		unsigned short bit1 : 1;
		unsigned short bit2 : 1;
		unsigned short bit3 : 1;
		unsigned short bit4_15 : 11;
	} attributebitfields;


	union
	{
		short	i;
		attributebitfields a;
	} attribute;

	union
	{
		short			s;
		unsigned short us;
		unsigned char  c[2];
	} guardianOwner;

	for(item=0;item<numItems;item++)
	{
		/* printf("%d %d\n", infoList[item], *sptr); */

		addString[0] = 0;
		switch(infoList[item])
		{
			/* 2 byte (short) values */
			case OV_ERROR_DETAIL: /*8 */
			case OV_PARTITION_IN_ERROR: /*9 */
			case OV_KEY_IN_ERROR: /*10 */
			case OV_CURRENT_KEY_SPECIFIER: /*13 */
			case OV_CURRENT_KEY_LENGTH: /*203 */
			case OV_CURRENT_PRIMARY_KEY_LENGTH: /*205 */
			case OV_OPEN_ACCESS_MODE: /*21 */
			case OV_OPEN_EXCLUSION_MODE: /*22 */
			case OV_PHYSICAL_RECORD_LENGTH: /*34 */
			case OV_FILE_TYPE: /*41 */
			case OV_FILE_CODE: /*42 */
			case OV_KEY_LENGTH: /*46 */
			case OV_LOCK_KEY_LENGTH: /*47 */
			case OV_MAXIMUM_EXTENTS: /*52 */
			case OV_ALLOCATED_EXTENTS: /*53 */
			case OV_SAFEGUARD_PROTECTED: /*59 */
			case OV_PROGID_FLAG: /*60 */
			case OV_CLEAR_ON_PURGE_FLAG: /*61 */
			case OV_LICENSED_FILE_FLAG: /*63 */
			case OV_ODD_UNSTRUCTURED_FLAG: /*65 */
			case OV_AUDITED_FLAG: /*66 */
			case OV_AUDIT_COMPRESSION_FLAG: /*67 */
			case OV_DATA_COMPRESSION_FLAG: /*68 */
			case OV_INDEX_COMPRESS_FLAG: /*69 */
			case OV_REFRESH_EOF_FLAG: /*70 */
			case OV_WRITE_THROUGH_FLAG: /*72 */
			case OV_VERIFY_WRITES_FLAG: /*73 */
			case OV_SERIAL_WRITES_FLAG: /*74 */
			case OV_FILE_IS_OPEN_FLAG: /*75 */
			case OV_CRASH_OPEN_FLAG: /*76 */
			case OV_ROLLFORWARD_NEEDED_FLAG: /*77 */
			case OV_BROKEN_FLAG: /*78 */
			case OV_CORRUPT_FLAG: /*79 */
			case OV_SECONDARY_PARTITION_FLAG: /*80 */
			case OV_INDEX_LEVELS: /*81 */
			case OV_SEQUENTIAL_BLOCK_BUFFERING_FLAG: /*116 */
			case OV_FILE_FORMAT: /*195*/
				switch (infoList[item]) /* we need to return several values */
				{
					case OV_CURRENT_KEY_LENGTH: /*14 */
						currentKeyLen = sptr[0];
						break;
					case OV_CURRENT_PRIMARY_KEY_LENGTH: /*16 */
						primaryKeyLen = sptr[0];
						break;
					default:
						break;
				}
				sprintf(addString, "%d,%d,", infoList[item], sptr[0]);
				sptr++;
			break;

			/* 4 byte fields */
			case OV_LOGICAL_RECORD_LENGTH: /*43 */
			case OV_BLOCK_LENGTH: /*44 */
			case OV_KEY_OFFSET: /*45 */
			case OV_OSS_OWNER_GROUP_ID: /*164 */
			case OV_OSS_ACCESS_PERMISSIONS: /*165 */
			case OV_OSS_OWNER: /*167 */
				iptr = (int*)&sptr[0];
				sprintf(addString, "%d,%d,", infoList[item], *iptr);
				sptr += 2;
			break;

			case OV_CURRENT_KEY_VALUE: /*15 */
				/* we have already obtained the length */
				cptr = (char*)&sptr[0];
				sprintf(addString, "%d,", OV_CURRENT_KEY_VALUE);
				for(j=0;j<currentKeyLen;j++)
				{
					sprintf(valueString, "%d,", cptr[j]);
					strcat(addString, valueString);
				}
				sptr += (currentKeyLen + 1) / 2;
				break;

			case OV_CURRENT_PRIMARY_KEY_VALUE: /*17 */
				/* we have already obtained the length */
				cptr = (char*)&sptr[0];
				sprintf(addString, "%d,", OV_CURRENT_PRIMARY_KEY_VALUE);
				for(j=0;j<primaryKeyLen;j++)
				{
					sprintf(valueString, "%d,", cptr[j]);
					strcat(addString, valueString);
				}
				sptr += (primaryKeyLen + 1) / 2;
				break;

			case OV_PRIMARY_EXTENT_SIZE: /*199 */
			case OV_SECONDARY_EXTENT_SIZE: /*200 */
				uiptr = (unsigned int *)&sptr[0];
				sprintf(addString, "%d,%u,", infoList[item], *uiptr);
				sptr += 2;
				break;

			case OV_GUARDIAN_FILE_OWNER: /*58 */
				guardianOwner.s = sptr[0];
				sprintf(addString, "%d,%d,%d,", OV_GUARDIAN_FILE_OWNER,
					guardianOwner.c[0], guardianOwner.c[1]);
				sptr++;
				break;

			case OV_SECURITY_STRING: /*62 */
				cptr = (char*)&sptr[0];
				for(i=0;i<4;i++)
				{
					switch(cptr[i])
					{
						case 0: /* any local ID */
							valueString[i] = 'A';
							break;
						case 1: /* member of ownerÆs group (local) */
							valueString[i] = 'G';
							break;
						case 2: /* owner (local) */
							valueString[i] = 'O';
							break;
						case 04: /* any network user (local or remote) */
							valueString[i] = 'N';
							break;
						case 5: /* member of ownerÆs community */
							valueString[i] = 'C';
							break;
						case 6: /* local or remote user having same ID as owner */
							valueString[i] = 'U';
							break;
						case 7: /* local super ID only */
							valueString[i] = 'S';
							break;
						default:
							valueString[i] = ' ';
							break;
					}
				}
				valueString[4] = 0;
				sprintf(addString, "%d,\"%s\",", infoList[item], valueString);
				sptr += 2;
				break;

			case OV_NUMBER_OF_PARTITIONS: /*90 */
				*numPartitions = sptr[0];
				sptr++;
				if(bExpectingVariableData)
				{
					sprintf(addString, "%d,%d,", infoList[item], *numPartitions);
                       /* Sol: 10-030723-8110 Begin Changes */
					/* OV_PARTITION_EXTENTS 97*/
					for(i=0;i<*numPartitions;i++)
					{
                                            uiptr = (unsigned int*)&sptr[0];
                                            sprintf(valueString, "%u,", *uiptr);
                                            sptr += 2;
                                            strcat(addString, valueString);

                                            uiptr = (unsigned int*)&sptr[0];
                                            sprintf(valueString, "%u,", *uiptr);
                                            sptr += 2;
                                            strcat(addString, valueString);
					}
					item++; /* OV_PARTITION_EXTENTS 97*/
                       /* Sol: 10-030723-8110 End Changes */     
					bytesConsumed = 0;
					/* there are numPartitions shorts before the array of names */
					cptr = (char*)&sptr[*numPartitions];
					for(i=0;i<*numPartitions;i++)
					{
						valueLen = sptr[i];
						bytesConsumed += valueLen;
						memcpy(valueString, cptr, valueLen);
						valueString[valueLen] = 0;
						cptr += valueLen;
						strcat(addString, valueString);
						strcat(addString, ",");
					}
					sptr += (bytesConsumed + 1)/2 + *numPartitions;
					item += 2; /* past OV_PARTITION_VOLUME_NAME_LENS:  92
								OV_PARTITION_VOLUME_NAMES:  93 */
				}
				break;

			case OV_PARTITION_PARTIAL_KEY_LENGTH: /*94 */
				item++; /* skip past key vals */
				valueLen = sptr[0];
				sptr++;
				if(valueLen > 0)
				{
					sprintf(addString, "%d,%d,",
									OV_PARTITION_PARTIAL_KEY_LENGTH,
									valueLen);
					cptr = (char*) sptr;
					bytesConsumed = 0;
					for(i=0;i<*numPartitions;i++)
					{
						bytesConsumed += valueLen;
						/* key might not be alphanumeric, return as
					       decimal values for each byte */
						for(j=0;j<valueLen;j++)
						{
							sprintf(valueString, "%d,", cptr[j]);
							strcat(addString, valueString);
						}
						cptr += valueLen;
					}
					sptr += (bytesConsumed + 1)/2;
				}
				break;

			case OV_NUMBER_OF_ALT_KEYS: /*100 */
				*numAltKeys = sptr[0];
				sptr++;
				if(bExpectingVariableData)
				{
					sprintf(addString, "%d,%d,", infoList[item], *numAltKeys);
					for(i=0;i<*numAltKeys;i++)
					{
						attribute.i = sptr[5];
						sprintf(valueString, "%d,%d,%d,%d,%d,%d,%d,%d,%d,",
									sptr[0], /* key specifier 2 bytes */
									sptr[1], /* key len 2 bytes */
									sptr[2], /* key offset 2 bytes */
									sptr[3], /* key alt file number 2 bytes */
									sptr[4], /* null value 2 bytes */
									attribute.a.bit0, /* attributes <0> = 1  a null value is specified. */
									attribute.a.bit1, /* attributes <1> = 1  the key is unique. */
									attribute.a.bit2, /* attributes <2> = 1  that automatic updating cannot be performed on this key. */
									attribute.a.bit3);/* attributes <3> = 0  that alternate-key records with
													duplicate key values are ordered by the value
													of the primary-key field. This attribute has
													meaning only for alternate keys that allow
													duplicates.
													= 1  that alternate-key records with
													duplicate key values are ordered by the
													sequence. */
						sptr +=6;
						strcat(addString, valueString);
					}
					item++;
				}

			break;

			case OV_NUMBER_OF_ALT_KEY_FILES: /*102 */
				*numAltKeyFiles = sptr[0];
				sptr++;
				if(bExpectingVariableData)
				{
					sprintf(addString, "%d,%d,", infoList[item], *numAltKeyFiles);
					item += 2; /* past OV_ALT_KEY_FILE_NAME_LENS and OV_ALT_KEY_FILE_NAMES */
					bytesConsumed = 0;
					/* there are numAltKeyFiles shorts before the array of names */
					cptr = (char*)&sptr[*numAltKeyFiles];
					for(i=0;i<*numAltKeyFiles;i++)
					{
						valueLen = sptr[i];
						bytesConsumed += valueLen;
						memcpy(valueString, cptr, valueLen);
						valueString[valueLen] = 0;
						cptr += valueLen;
						strcat(addString, valueString);
						strcat(addString, ",");
					}
					sptr += (bytesConsumed + 1)/2 + *numAltKeyFiles;
				}

			break;


			case OV_LAST_OPEN_TIME: /*117 */
			case OV_EXPIRATION_TIME: /*118 */
			case OV_CREATION_TIME: /*119 */
			case OV_MODIFICATION_TIME: /*145 */
				lptr = (long long*)sptr;
				if(lptr > 0)
				{
					INTERPRETTIMESTAMP ( *lptr, dateTime);
					sprintf(addString, "%d,%d,%d,%d,%d,%d,%d,%d,", infoList[item],
						dateTime[0],
						dateTime[1],
						dateTime[2],
						dateTime[3],
						dateTime[4],
						dateTime[5],
						dateTime[6]);
				}
				sptr += 4;
			break;


             /* Sol: 10-030703-7581 added OV_AGGREGATE_EOF and 
                     OV_AGGREGATE_MAX_FILE_SIZE */
                        case OV_AGGREGATE_EOF: /*191 */
                        case OV_AGGREGATE_MAX_FILE_SIZE: /*192 */
			case OV_NEXT_RECORD_POINTER: /*201 */
			case OV_CURRENT_RECORD_POINTER: /*202 */
				lptr = (long long*)sptr;
				sprintf(addString, "%d,%Ld,",infoList[item], *lptr);
				sptr += 4;
				break;

			default:
				/* should never get here */
				if(jEDebugFlag)
				{
					printf("jEBuildInfoString unknown option: %d\n", infoList[item]);
				}
			break;
		}
		strcat(&infoString[*infoStringLen], addString);
		*infoStringLen += strlen(addString);
	}
} /* jEBuildInfoString */

/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
JNIEXPORT jstring JNICALL Java_com_tandem_ext_enscribe_EnscribeFileAttributes_GgetFileInfoDetails(
 				JNIEnv *env,
				jobject thisEnscribeFile,
				/*in*/ jshort jfileNum,
				/*in*/ jstring jfileName,
				/*in*/ jboolean jusingFileName,
				/*in*/ jboolean jossFormatFileName,
				/*out*/ jarray jerrorArray)
{
	jboolean
		bfileNameIsCopy,
		berrorArrayIsCopy;
	jshort
		*errorArray;

	short
		typeInfo[5],
		infoResults[INFORESULTSSIZE],
		infoWanted[206],
		numWanted = 0,
		fileNameLen,
		flags,
		infoResultsLen,
		errorItem,
		error = 0,
		numPartitions = 0,
		numAltKeys = 0,
		numAltKeyFiles = 0;

	int
		i,
		infoStringLen;
	const char
  		*fileName;
	char
		infoString[INFOSTRINGSIZE],
		wbuf[100],
		gFileName[100];
	jstring
		resultString = NULL;
	jboolean
		fileIsOSS = false;

	errorArray = (*env)->GetShortArrayElements(env, jerrorArray, &berrorArrayIsCopy);
	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);

	errorArray[0] = errorArray[1] = 0;
	if(!jusingFileName)
	{
		error = FILE_GETINFO_ ( jfileNum, ,,,,typeInfo, &flags);
		/* typeInfo[3] is fileType_ */

	}
	else
	{
		/* by file name */
		if(jossFormatFileName)
		{
			error = PATHNAME_TO_FILENAME_(fileName, gFileName, 100, &fileNameLen, &flags);
			if(error != 0)
			{
				if(jEDebugFlag)
				{
					printf("GgetFileInfoDetails PATHNAME_TO_FILENAME_: error=%d \n", error);
				}
				goto bailout;
			}
			gFileName[fileNameLen] = 0;
		}
		else
		{
			strcpy(gFileName, fileName);
		}
		error = FILE_GETINFOBYNAME_(gFileName, strlen(gFileName), typeInfo, , 0,0,
			 &flags);
		/* at this point, gFileName has the Guardian format filename  */
	}
	/* we don't need it anymore  */
	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);

	if(error != 0)
	{
		if(jEDebugFlag)
		{
			printf("GgetFileInfoDetails: error=%d\n", error);
		}
		goto bailout;
	}

	if((flags & 1))
		fileIsOSS = true;
	if(typeInfo[0] != DEVICETYPE_DISC)
	{
		error = ERR_NOT_DISC_FILE;
		goto bailout;
	}
	/* now we know what type of file it is and whether we need more information  */
	infoWanted[numWanted++] = OV_PHYSICAL_RECORD_LENGTH;
	infoWanted[numWanted++] = OV_FILE_TYPE;
	infoWanted[numWanted++] = OV_FILE_CODE;
	infoWanted[numWanted++] = OV_BLOCK_LENGTH;
	infoWanted[numWanted++] = OV_PRIMARY_EXTENT_SIZE;
	infoWanted[numWanted++] = OV_SECONDARY_EXTENT_SIZE;
	infoWanted[numWanted++] = OV_MAXIMUM_EXTENTS;
	infoWanted[numWanted++] = OV_ALLOCATED_EXTENTS;
	if(!fileIsOSS)
		infoWanted[numWanted++] = OV_GUARDIAN_FILE_OWNER;
	infoWanted[numWanted++] = OV_SAFEGUARD_PROTECTED;
	infoWanted[numWanted++] = OV_CLEAR_ON_PURGE_FLAG;
	if(!fileIsOSS)
		infoWanted[numWanted++] = OV_SECURITY_STRING;
	if(typeInfo[3] == FILETYPE_UNSTRUCTURED)
		infoWanted[numWanted++] = OV_ODD_UNSTRUCTURED_FLAG;
	infoWanted[numWanted++] = OV_AUDITED_FLAG;
	infoWanted[numWanted++] = OV_AUDIT_COMPRESSION_FLAG;
	infoWanted[numWanted++] = OV_REFRESH_EOF_FLAG;
	infoWanted[numWanted++] = OV_WRITE_THROUGH_FLAG;
	infoWanted[numWanted++] = OV_VERIFY_WRITES_FLAG;
	infoWanted[numWanted++] = OV_SERIAL_WRITES_FLAG;
	infoWanted[numWanted++] = OV_FILE_IS_OPEN_FLAG;
	infoWanted[numWanted++] = OV_CRASH_OPEN_FLAG;
	infoWanted[numWanted++] = OV_ROLLFORWARD_NEEDED_FLAG;
	infoWanted[numWanted++] = OV_BROKEN_FLAG;
	infoWanted[numWanted++] = OV_CORRUPT_FLAG;
	infoWanted[numWanted++] = OV_SECONDARY_PARTITION_FLAG;
	infoWanted[numWanted++] = OV_NUMBER_OF_PARTITIONS;
	/* infoWanted[numWanted++] = OV_SEQUENTIAL_BLOCK_BUFFERING_FLAG;   */
	infoWanted[numWanted++] = OV_LAST_OPEN_TIME;
	infoWanted[numWanted++] = OV_EXPIRATION_TIME;
	infoWanted[numWanted++] = OV_CREATION_TIME;
	infoWanted[numWanted++] = OV_MODIFICATION_TIME;
	infoWanted[numWanted++] = OV_FILE_FORMAT;

       /* Sol: 10-030703-7581 Added OV_AGGREGATE_EOF and
               OV_AGGREGATE_MAX_FILE_SIZE in the list of items */
        infoWanted[numWanted++] = OV_AGGREGATE_EOF;
        infoWanted[numWanted++] = OV_AGGREGATE_MAX_FILE_SIZE;

	if(fileIsOSS)
	{
		infoWanted[numWanted++] = OV_OSS_OWNER_GROUP_ID;
		infoWanted[numWanted++] = OV_OSS_ACCESS_PERMISSIONS;
		infoWanted[numWanted++] = OV_OSS_OWNER;
	}

	if(typeInfo[3] != FILETYPE_UNSTRUCTURED) /* file is structured  */
	{
		infoWanted[numWanted++] = OV_LOGICAL_RECORD_LENGTH;

		 /** ADDED for soln# 10-111122-0670 ***
		 *** These two attributes are not part of SQL files  ***
		 *** so a condition checking is introduced **/
		if (typeInfo[2] == NON_SQL_FILES)
		{
			infoWanted[numWanted++] = OV_NUMBER_OF_ALT_KEYS;
			infoWanted[numWanted++] = OV_NUMBER_OF_ALT_KEY_FILES;
		}
		if(typeInfo[3] == FILETYPE_KEY_SEQUENCED)
		{
			infoWanted[numWanted++] = OV_KEY_LENGTH;
			infoWanted[numWanted++] = OV_KEY_OFFSET;
			infoWanted[numWanted++] = OV_LOCK_KEY_LENGTH;
			infoWanted[numWanted++] = OV_INDEX_COMPRESS_FLAG;
			infoWanted[numWanted++] = OV_DATA_COMPRESSION_FLAG;
			infoWanted[numWanted++] = OV_INDEX_LEVELS;

		}
	}
	else
	{
		infoWanted[numWanted++] = OV_PROGID_FLAG;
	}


	if(!jusingFileName) /* the file is open   */
	{

		error  = FILE_GETINFOLIST_ ( jfileNum,
							infoWanted,
							numWanted,
							infoResults,
							INFORESULTSSIZE * 2,
							&infoResultsLen,
							&errorItem);
	}
	else
	{
		error  = FILE_GETINFOLISTBYNAME_ ( gFileName, strlen(gFileName),
							infoWanted,
							numWanted,
							infoResults,
							INFORESULTSSIZE * 2,
							&infoResultsLen,
							&errorItem);
	}

	if(error != 0)
	{
		if(jEDebugFlag)
		{
			printf("GgetFileInfoDetails-1: error=%d errorItem=%d\n", error, errorItem);
			if(errorItem > -1)
				printf("  Error item value = %d\n", infoWanted[errorItem]);
			for(i=0;i<numWanted;i++)
				printf("%d,", infoWanted[i]);
			printf("\n");
		}
		goto bailout;
	}

	/* we don't expect to get any errors as we knew what to ask for  */
	strcpy(infoString, "<FI,");
	if(fileIsOSS)
	{
		sprintf(wbuf, "%d,1,", OV_IS_OSS_FILE);
		strcat(infoString, wbuf);
	}
	infoStringLen = strlen(infoString);
	jEBuildInfoString(infoString, &infoStringLen, infoWanted, numWanted, infoResults,
				infoResultsLen, &numPartitions,  &numAltKeys, &numAltKeyFiles,
				/*didn't get variable data yet*/ false);


	numWanted = 0;
	/* if there are partitions, we need to get them  */
	if(numPartitions > 0)
	{
		infoWanted[numWanted++] = OV_NUMBER_OF_PARTITIONS;
		infoWanted[numWanted++] = OV_PARTITION_EXTENTS;
		infoWanted[numWanted++] = OV_PARTITION_VOLUME_NAME_LENS;
		infoWanted[numWanted++] = OV_PARTITION_VOLUME_NAMES;

		if(typeInfo[3] == FILETYPE_KEY_SEQUENCED)
		{
			infoWanted[numWanted++] = OV_PARTITION_PARTIAL_KEY_LENGTH;
			infoWanted[numWanted++] = OV_PARTITION_PARTIAL_KEY_VALS;
		}
	}
	/* if there are alternate keys, we need to get them */
	if(numAltKeys > 0)
	{
		infoWanted[numWanted++] = OV_NUMBER_OF_ALT_KEYS;
		infoWanted[numWanted++] = OV_ALT_KEY_DESCRIPTORS;
	}
	if(numAltKeyFiles > 0)
	{
		infoWanted[numWanted++] = OV_NUMBER_OF_ALT_KEY_FILES;
		infoWanted[numWanted++] = OV_ALT_KEY_FILE_NAME_LENS;
		infoWanted[numWanted++] = OV_ALT_KEY_FILE_NAMES;
	}
	if(numWanted > 0)
	{
		if(!jusingFileName) /* the file is open */
		{

			error  = FILE_GETINFOLIST_ ( jfileNum,
								infoWanted,
								numWanted,
								infoResults,
								INFORESULTSSIZE * 2,
								&infoResultsLen,
								&errorItem);
		}
		else
		{
			error  = FILE_GETINFOLISTBYNAME_ ( gFileName, strlen(gFileName),
								infoWanted,
								numWanted,
								infoResults,
								INFORESULTSSIZE * 2,
								&infoResultsLen,
								&errorItem);
		}
	}
	if(error != 0)
	{
		if(jEDebugFlag)
		{
			printf("GgetFileInfoDetails-2: error=%d errorItem=%d\n", error, errorItem);
			if(errorItem > -1)
				printf("  Error item value = %d\n", infoWanted[errorItem]);
			for(i=0;i<numWanted;i++)
				printf("%d,", infoWanted[i]);
			printf("\n");
		}
		goto bailout;
	}
	/* process results   */
	/* append results to infoString  */
	jEBuildInfoString(infoString, &infoStringLen, infoWanted, numWanted, infoResults,
				infoResultsLen, &numPartitions,  &numAltKeys, &numAltKeyFiles,
				/*have variable data now*/ true);

	sprintf(wbuf, "%d,FI>,", OV_LAST_OPTVAL);
	strcat(infoString, wbuf);
	if(jEDebugFlag)
	{
		printf("GgetFileInfoDetails-3: %s\n", infoString);
	}

	/* create a java String and return it   */

	 resultString = (*env)->NewStringUTF(env, infoString);

bailout:
	errorArray[0] = error;
	(*env)->ReleaseShortArrayElements(env, jerrorArray, errorArray, 0); /* save changes */
	return resultString;
} /* Java_com_tandem_enscribe_EnscribeFileAttr_GgetFileInfoDetails */


/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
JNIEXPORT jstring JNICALL Java_com_tandem_ext_enscribe_EnscribeLastCallStatus_GgetCurrentFileInfo(
 				JNIEnv *env,
				jobject thisEnscribeFile,
				/*in*/ jshort jfileNum,
				/*in*/ jshort jfileType,
				/*in*/ jboolean jossFormatFileName,
				/*out*/ jarray jerrorArray)
{
	jboolean
		berrorArrayIsCopy;
	jshort
		*errorArray;

	short
		infoResults[INFORESULTSSIZE],
		infoWanted[200],
		numWanted = 0,
		infoResultsLen,
		errorItem,
		error = 0,
		numPartitions = 0,
		numAltKeys = 0,
		numAltKeyFiles = 0;

	int
		i,
		infoStringLen;
	char
		infoString[INFOSTRINGSIZE],
		wbuf[100];
	jstring
		resultString = NULL;

	errorArray = (*env)->GetShortArrayElements(env, jerrorArray, &berrorArrayIsCopy);

	errorArray[0] = errorArray[1] = 0;

	/* now we know what type of file it is and whether we need more information  */
	infoWanted[numWanted++] = OV_ERROR_DETAIL;
	infoWanted[numWanted++] = OV_PARTITION_IN_ERROR;
	infoWanted[numWanted++] = OV_OPEN_ACCESS_MODE;
	infoWanted[numWanted++] = OV_OPEN_EXCLUSION_MODE;


	if(jfileType != FILETYPE_UNSTRUCTURED) /* file is structured  */
	{
		infoWanted[numWanted++] = OV_KEY_IN_ERROR;
		infoWanted[numWanted++] = OV_CURRENT_KEY_SPECIFIER;
		infoWanted[numWanted++] = OV_CURRENT_KEY_LENGTH;
		infoWanted[numWanted++] = OV_CURRENT_KEY_VALUE;
		if(jfileType == FILETYPE_KEY_SEQUENCED)
		{
			infoWanted[numWanted++] = OV_CURRENT_PRIMARY_KEY_LENGTH;
			infoWanted[numWanted++] = OV_CURRENT_PRIMARY_KEY_VALUE;
		}
	}
	if(jfileType == FILETYPE_RELATIVE || jfileType == FILETYPE_ENTRY_SEQUENCED) {
		infoWanted[numWanted++] = OV_NEXT_RECORD_POINTER;
		infoWanted[numWanted++] = OV_CURRENT_RECORD_POINTER;
	}

	error  = FILE_GETINFOLIST_ ( jfileNum,
						infoWanted,
						numWanted,
						infoResults,
						INFORESULTSSIZE * 2,
						&infoResultsLen,
						&errorItem);

	if(error != 0)
	{
		if(jEDebugFlag)
		{
			printf("GgetCurrentFileInfo: error=%d errorItem=%d\n", error, errorItem);
			if(errorItem > -1)
				printf("  Error item value = %d\n", infoWanted[errorItem]);
			for(i=0;i<numWanted;i++)
				printf("%d,", infoWanted[i]);
			printf("\n");
		}
		goto bailout;
	}
	/* we don't expect to get any errors as we knew what to ask for  */
	strcpy(infoString, "<FI,");
	infoStringLen = strlen(infoString);
	jEBuildInfoString(infoString, &infoStringLen, infoWanted, numWanted, infoResults,
				infoResultsLen, &numPartitions,  &numAltKeys, &numAltKeyFiles,
				false);


	sprintf(wbuf, "%d,FI>,", OV_LAST_OPTVAL);
	strcat(infoString, wbuf);
	if(jEDebugFlag)
	{
		printf("GgetCurrentFileInfo-2: %s\n", infoString);
	}

	/* create a java String and return it   */

	 resultString = (*env)->NewStringUTF(env, infoString);

bailout:
	errorArray[0] = error;
	(*env)->ReleaseShortArrayElements(env, jerrorArray, errorArray, 0); /* save changes */
	return resultString;
} /* Java_com_tandem_enscribe_EnscribeFileAttr_GgetCurrentFileInfo */


