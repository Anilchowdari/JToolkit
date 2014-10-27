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
#include <spthread.h>

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "com_tandem_ext_enscribe_EnscribeFile.h"
#include "com_tandem_ext_enscribe_EnscribeFileSearch.h"


#include "com_tandem_ext_enscribe_EnscribeFileException.h"

#include "enscribejni.h"
#include "jEcmaFuncs.h"
#include "error.h"
#include <cextdecs.h>

short jEDebugFlag = 0;
short jEDebugBufferCnt = 0; /* if > 0, the number of bytes to dump */

void jEFormatHexOutput(unsigned int Offset, unsigned char *In, char *Out,
		int Length);
void jEthrowOutOfMem(JNIEnv *env);
/* ----------------------------------------------------------------------------
 Prints a buffer
 *----------------------------------------------------------------------------- */
void jEDumpBuffer(const char* idString, const char* buf, int bufLen) {
	int numToDump = bufLen;
	int NumWholeLines = numToDump / 16;
	int i, PartialLineLength = numToDump % 16;
	const char *lpBuffer;
	char wbuf[256];
	unsigned int Offset = 0;

	if (numToDump > jEDebugBufferCnt) {
		numToDump = jEDebugBufferCnt;
		NumWholeLines = numToDump / 16;
		PartialLineLength = numToDump % 16;
	}
	printf("%s: dumping %d bytes\n", idString, numToDump);

	lpBuffer = buf;
	printf("\n");
	for (i = 0; i < NumWholeLines; i++) {
		jEFormatHexOutput(Offset, (unsigned char*)lpBuffer, wbuf, 16);
		lpBuffer += 16;
		Offset += 16;
		printf("%s\n", wbuf);
	}

	if (PartialLineLength) {
		jEFormatHexOutput(Offset, (unsigned char*)lpBuffer, wbuf,
				PartialLineLength);
		lpBuffer += PartialLineLength;
		printf("%s\n", wbuf);
	}
	printf("\n");

} /* jEDumpBuffer */

/* ----------------------------------------------------------------------------
 Formats Hex Output
 * ---------------------------------------------------------------------------- */
void jEFormatHexOutput(unsigned int Offset, unsigned char *In, char *Out,
		int Length) {
	int i;
	unsigned char *InBase = In;
	static char HexChars[] = "0123456789ABCDEF";

	/* format offset to 4 hex digits */
	*Out++ = HexChars[Offset >> 12];
	*Out++ = HexChars[(Offset & 0X0FFF) >> 8];
	*Out++ = HexChars[(Offset & 0X00FF) >> 4];
	*Out++ = HexChars[ Offset & 0X000F];
	*Out++ = ' ';
	*Out++ = ' ';

	/* convert the input byte to hex */
	for (i = 0; i < Length; i++) {
		*Out++ = HexChars[(*In) >> 4];
		*Out++ = HexChars[(*In++) & 0X0F];
		*Out++ = ' ';
	}

	*Out++ = ' ';

	for (i = 0; i < (16-Length); i++) /* Align the ascii to 16 data bytes */
	{
		*Out++ = ' ';
		*Out++ = ' ';
		*Out++ = ' ';
	}

	/* output the original data */
	In = InBase;
	for (i = 0; i < Length; i++) {
		if (isprint( *In) )
			*Out++ = *In++;
		else {
			*Out++ = '.';
			++In;
		}
	}
	*Out = '\0';

	return;

} /* end of jEFormatHexOutput */

/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
char * jEnextTok(char **nextPtr, int chr) {
	char *currentPtr = *nextPtr, *ptr;
	if (currentPtr == NULL)
		return NULL;
	ptr = strchr(currentPtr, chr);
	if (ptr == NULL) {
		*nextPtr = NULL;
		return NULL;
	}
	*nextPtr = ptr + 1;
	*ptr = 0; /* null stopper  */
	return currentPtr;
} /* jEnextTok  */

/* ----------------------------------------------------------------------------
 Decodes the string received from the Java side that contains the
 file creation attributes
 * ---------------------------------------------------------------------------- */
void jEDecodeInfoString(const char *creationAttributes, short *numAttributes,
		short *itemArray, short *attributeArray, short *attributeArrayLen) {

	short dateTime[8], akAttribute, val, opt, *sptr = attributeArray;
	long long jts;
	long lval;
	char *caBuf, *nextPtr, *captr;
	char partialKey[300], /* max keylen is 255 */
	*cptr;
	int i, j, totalLen, numPartitions = 0, numAltkeys = 0, numAltkeyFiles = 0,
			partitalKeyLen = 0;

	union {
		int i;
		unsigned short s[2];
	} uns;

	caBuf = (char*)malloc(strlen(creationAttributes) + 1);

	memcpy(caBuf, creationAttributes, strlen(creationAttributes) + 1);

	captr = nextPtr = caBuf;
	captr = jEnextTok(&nextPtr, ','); /* swallow <xC */

	while (*captr != NULL) {
		captr = jEnextTok(&nextPtr, ',');
		if (*captr == NULL)
			break;
		opt = atoi(captr);
		/*printf("opt= %d ", opt);*/
		if (opt == OV_LAST_OPTVAL)
			break;
		itemArray[(*numAttributes)++] = opt;
		captr = jEnextTok(&nextPtr, ',');
		switch (opt) {
		case OV_FILE_TYPE: /*41 */
		case OV_FILE_CODE: /*42 */
		case OV_KEY_LENGTH: /*46 */
		case OV_MAXIMUM_EXTENTS: /*52 */
		case OV_ODD_UNSTRUCTURED_FLAG: /*65 */
		case OV_AUDITED_FLAG: /*66 */
		case OV_AUDIT_COMPRESSION_FLAG: /*67 */
		case OV_DATA_COMPRESSION_FLAG: /*68 */
		case OV_INDEX_COMPRESS_FLAG: /*69 */
		case OV_REFRESH_EOF_FLAG: /*70 */
		case OV_WRITE_THROUGH_FLAG: /*72 */
		case OV_VERIFY_WRITES_FLAG: /*73 */
		case OV_SERIAL_WRITES_FLAG: /*74 */
		case OV_NUMBER_OF_PARTITIONS: /*90 */
		case OV_PARTITION_PARTIAL_KEY_LENGTH: /*94 */
		case OV_NUMBER_OF_ALT_KEYS: /*100 */
		case OV_NUMBER_OF_ALT_KEY_FILES: /*102 */
		case OV_FILE_FORMAT: /*195 */

			val = atoi(captr);
			switch (opt) /* need some of the values for later */
			{
			case OV_NUMBER_OF_PARTITIONS:
				numPartitions = val;
				break;
			case OV_NUMBER_OF_ALT_KEYS:
				numAltkeys = val;
				break;
			case OV_NUMBER_OF_ALT_KEY_FILES:
				numAltkeyFiles = val;
				break;
			case OV_PARTITION_PARTIAL_KEY_LENGTH:
				partitalKeyLen = val;
				break;
			default:
				break;
			}
			/*printf("  short= %d\n", val);*/
			*sptr = val;
			sptr++;
			break;

		case OV_LOGICAL_RECORD_LENGTH: /*196 */
		case OV_BLOCK_LENGTH: /*197 */
		case OV_KEY_OFFSET: /*198 */
			lval = atol(captr);
			memcpy(sptr, &lval, 4);
			sptr+= 2;
			break;

		case OV_PRIMARY_EXTENT_SIZE: /*199 */
		case OV_SECONDARY_EXTENT_SIZE: /*200 */
			uns.i = atoi(captr);
			/*printf("  ext= %u\n", uns.i);*/
			memcpy(sptr, &uns.i, 4);
			sptr+= 2;
			break;

		case OV_EXPIRATION_TIME_GMT: /*57 */
			dateTime[0] = atoi(captr);
			captr = jEnextTok(&nextPtr, ',');
			dateTime[1] = atoi(captr);
			captr = jEnextTok(&nextPtr, ',');
			dateTime[2] = atoi(captr);
			captr = jEnextTok(&nextPtr, ',');
			dateTime[3] = atoi(captr);
			captr = jEnextTok(&nextPtr, ',');
			dateTime[4] = atoi(captr);
			captr = jEnextTok(&nextPtr, ',');
			dateTime[5] = atoi(captr);
			dateTime[6] = 0;
			dateTime[7] = 0;
			/*printf("  date= %d %d %d %d %d %d\n",
			 dateTime[0],
			 dateTime[1],
			 dateTime[2],
			 dateTime[3],
			 dateTime[4],
			 dateTime[5]); */
			jts = COMPUTETIMESTAMP(dateTime);
			memcpy(sptr, &jts, 8);
			sptr += 4;
			break;

			/* Sol: 10-030723-8110 Begin Changes */
		case OV_PARTITION_EXTENTS: /*97 */
			for (i=0; i<numPartitions; i++) {
				lval = atol(captr);
				memcpy(sptr, &lval, 4);
				sptr+=2;
				captr = jEnextTok(&nextPtr, ',');
				lval = atol(captr);
				memcpy(sptr, &lval, 4);
				sptr+=2;
				/* Sol: 10-030723-8110 End Changes */
				/*printf("  part exts= %u %u\n", sptr[-1], sptr[0]);*/
				if (i<numPartitions-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			break;

		case OV_PARTITION_VOLUME_NAME_LENS: /*91 */
			for (i=0; i<numPartitions; i++) {
				val = atoi(captr);
				*sptr = val;
				sptr++;
				/*printf("  part vol lens= %d\n", val);*/
				if (i<numPartitions-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			break;

		case OV_PARTITION_VOLUME_NAMES: /*92 */
			totalLen = 0;
			cptr = (char*)sptr;
			for (i=0; i<numPartitions; i++) {
				memcpy(&cptr[totalLen], captr, strlen(captr));
				totalLen += strlen(captr);
				/*printf("  part vol= %s\n", captr);*/

				if (i<numPartitions-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			sptr += (totalLen + 1) / 2;
			break;

		case OV_PARTITION_PARTIAL_KEY_VALS: /*95 */
			totalLen = 0;
			cptr = (char*)sptr;
			for (i=0; i<numAltkeyFiles; i++) {
				for (j=0; j<partitalKeyLen; j++) {
					val = atoi(captr);
					partialKey[j] = val;
					if (i<numAltkeyFiles-1 && j < partitalKeyLen -1) /* don't get next after last one */
						captr = jEnextTok(&nextPtr, ',');
				}

				memcpy(&cptr[totalLen], partialKey, partitalKeyLen);
				totalLen += partitalKeyLen;
			}
			sptr += (totalLen + 1) / 2;
			break;

		case OV_ALT_KEY_DESCRIPTORS: /*101 */
			for (i=0; i<numAltkeys; i++) {
				/* key specifier */
				if (captr[0] == '"' && captr[3] == '"' && strlen(captr) == 4) /* it it alphanumeric */
				{
					memcpy(sptr, &captr[1], 2);
				} else {
					val = atoi(captr);
					*sptr = val;
				}
				sptr++;
				captr = jEnextTok(&nextPtr, ',');

				/* key-len */
				val = atoi(captr);
				*sptr = val;
				sptr++;
				captr = jEnextTok(&nextPtr, ',');

				/* key-offset */
				val = atoi(captr);
				*sptr = val;
				sptr++;
				captr = jEnextTok(&nextPtr, ',');

				/* key-filenum */
				val = atoi(captr);
				*sptr = val;
				sptr++;
				captr = jEnextTok(&nextPtr, ',');

				/* null-value */
				val = atoi(captr);
				*sptr = val;
				sptr++;
				captr = jEnextTok(&nextPtr, ',');

				akAttribute = 0;
				/* null value is specified */
				val = atoi(captr);
				if (val != 0)
					akAttribute |= 0x8000; /* bit 0 */
				captr = jEnextTok(&nextPtr, ',');

				/* key is unique */
				val = atoi(captr);
				if (val != 0)
					akAttribute |= 0x4000; /* bit 1 */
				captr = jEnextTok(&nextPtr, ',');

				/* automatic updating cannot be performed */
				val = atoi(captr);
				if (val != 0)
					akAttribute |= 0x2000; /* bit 2 */
				captr = jEnextTok(&nextPtr, ',');

				/* are ordered by the sequence in which those records were inserted */
				val = atoi(captr);
				if (val != 0)
					akAttribute |= 0x1000; /* bit 3 */

				*sptr = akAttribute;
				/*printf("  akd= %d %d %d %d %d %d\n",
				 sptr[-5], sptr[-4], sptr[-3],
				 sptr[-2], sptr[-1], sptr[0]);*/
				sptr++;
				if (i<numAltkeys-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			break;

		case OV_ALT_KEY_FILE_NAME_LENS: /*103 */
			for (i=0; i<numAltkeyFiles; i++) {
				val = atoi(captr);
				*sptr = val;
				/*printf("  akf lens= %d\n", val);*/
				sptr++;
				if (i<numAltkeyFiles-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			break;

		case OV_ALT_KEY_FILE_NAMES: /*104 */
			totalLen = 0;
			cptr = (char*)sptr;
			for (i=0; i<numAltkeyFiles; i++) {
				memcpy(&cptr[totalLen], captr, strlen(captr));
				totalLen += strlen(captr);
				/*printf("  ak file= %s\n", captr);*/

				if (i<numAltkeyFiles-1) /* don't get next after last one */
					captr = jEnextTok(&nextPtr, ',');
			}
			sptr += (totalLen + 1) / 2;
			break;

		case OV_LAST_OPTVAL:
			*captr = NULL; /* to stop while */
			break;

		default:
			/* shouldn't get here */
			if (jEDebugFlag) {
				i = captr - caBuf;
				printf("jEDecodeInfoString unknown option: %d at index %d\n",
						opt, i);
			}
			break;
		}
	}
	*attributeArrayLen = (sptr - attributeArray) * 2;
	free(caBuf);
} /* jEDecodeInfoString */

/* ----------------------------------------------------------------------------
 Calls the GUARDIAN routine FILE_ALTERLIST_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileAlter(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jshort jpartOnly,
		/*in*/jstring jcreationAttributes)

{
	jboolean
	bfileNameIsCopy,
	bcreationAttributesIsCopy;
	const char
	*creationAttributes,
	*fileName;

	short
	i,
	error = 0,
	errorItem,
	filenameLen,
	numAttributes = 0,
	itemArray[100],
	attributeArrayLen = 0,
	attributeArray[5000];

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);
	creationAttributes = (*env)->GetStringUTFChars(env, jcreationAttributes, &bcreationAttributesIsCopy);

	jEDecodeInfoString(creationAttributes,
			&numAttributes, itemArray, attributeArray,
			&attributeArrayLen);

	/* ready to alter the file */
	filenameLen = strlen(fileName);
	error = FILE_ALTERLIST_ ( (char *)fileName, filenameLen,
			itemArray,
			numAttributes,
			attributeArray,
			attributeArrayLen,
			jpartOnly,
			&errorItem);
	if(jEDebugFlag)
	{
		printf("GfileAlter: error=%d errorItem=%d numAttributes=%d\n",
				error, errorItem, numAttributes);
		printf("%s\n", creationAttributes);
		for(i=0;i<numAttributes;i++)
		printf("%d ", itemArray[i]);
		printf("\n");
		for(i=0;i<(attributeArrayLen / 2);i++)
		printf("%d ", attributeArray[i]);
		printf("\n");
	}

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	(*env)->ReleaseStringUTFChars(env, jcreationAttributes, creationAttributes);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GfileAlter */

/* ----------------------------------------------------------------------------
 Calls CONTROL
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gcontrol(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jshort joperation,
		/*in*/jshort jparam)
{
	short error;
	long dummy;

	long
	tag = jfileNum;

	CONTROL(jfileNum, joperation, jparam, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gcontrol */

/* ----------------------------------------------------------------------------
 Calls FILE_CLOSE_ and unregisters the io handler
 * ---------------------------------------------------------------------------- */
JNIEXPORT void JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileClose(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum)
{
	long tag = jfileNum;
	short error;

	/*
	 * Before calling close, check for any outstanding IO.
	 * If IO is outstanding on jfileNum, cancel the IO
	 * because although the file system will cancel any outstanding
	 * IO on a close, it will not wake up any thread waiting for
	 * that IO to complete.
	 */

	JEnscribeCancelAnyOutstandingReq(jfileNum, tag);

	error = FILE_CLOSE_(jfileNum);
	if(jEDebugFlag)
	{
		printf("GfileClose: error=%d fnum=%d\n",
				error, jfileNum);
	}

	spt_unregFile(jfileNum);

} /* Java_com_tandem_enscribe_EnscribeFile_GfileClose */

/* ----------------------------------------------------------------------------
 Calls FILE_CREATELIST_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileCreate(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jstring jcreationAttributes)

{
	jboolean
	bfileNameIsCopy,
	bcreationAttributesIsCopy;
	const char
	*creationAttributes,
	*fileName;

	short
	i,
	error = 0,
	errorItem,
	filenameLen,
	numAttributes = 0,
	itemArray[100],
	attributeArrayLen = 0,
	attributeArray[5000];
	char
	thisFileName[255];

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);
	creationAttributes = (*env)->GetStringUTFChars(env, jcreationAttributes, &bcreationAttributesIsCopy);

	jEDecodeInfoString(creationAttributes,
			&numAttributes, itemArray, attributeArray,
			&attributeArrayLen);

	/* ready to create the file */
	strcpy(thisFileName, fileName);
	filenameLen = strlen(thisFileName);
	error = FILE_CREATELIST_ ( thisFileName, 255, &filenameLen,
			itemArray,
			numAttributes,
			attributeArray,
			attributeArrayLen,
			&errorItem);
	if(jEDebugFlag)
	{
		printf("GfileCreate: error=%d errorItem=%d numAttributes=%d\n",
				error, errorItem, numAttributes);
		printf("%s\n", creationAttributes);
		for(i=0;i<numAttributes;i++)
		printf("%d ", itemArray[i]);
		printf("\n");
		for(i=0;i<(attributeArrayLen / 2);i++)
		printf("%d ", attributeArray[i]);
		printf("\n");
	}

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	(*env)->ReleaseStringUTFChars(env, jcreationAttributes, creationAttributes);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GfileCreate */

/* ----------------------------------------------------------------------------
 Calls FILE_CREATELIST_ for a temporary file
 * ---------------------------------------------------------------------------- */
JNIEXPORT jstring JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileCreateTemp(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jvolName,
		/*in*/jstring jcreationAttributes,
		/*out*/jarray jerrorArray)

{
	jboolean
	bvolNameIsCopy,
	bcreationAttributesIsCopy,
	berrorArrayIsCopy;
	jshort
	*errorArray;
	const char
	*creationAttributes,
	*volName;

	short
	i,
	error = 0,
	errorItem,
	filenameLen,
	numAttributes = 0,
	itemArray[100],
	attributeArrayLen = 0,
	attributeArray[5000];
	char
	thisFileName[255];
	jstring
	resultString = NULL;

	errorArray = (*env)->GetShortArrayElements(env, jerrorArray, &berrorArrayIsCopy);
	volName = (*env)->GetStringUTFChars(env, jvolName, &bvolNameIsCopy);
	creationAttributes = (*env)->GetStringUTFChars(env, jcreationAttributes, &bcreationAttributesIsCopy);

	jEDecodeInfoString(creationAttributes,
			&numAttributes, itemArray, attributeArray,
			&attributeArrayLen);

	/* ready to create the file */
	strcpy(thisFileName, volName);
	filenameLen = strlen(thisFileName);
	error = FILE_CREATELIST_ ( thisFileName, 255, &filenameLen,
			itemArray,
			numAttributes,
			attributeArray,
			attributeArrayLen,
			&errorItem);
	if(jEDebugFlag)
	{
		printf("GfileCreateTemp: error=%d errorItem=%d numAttributes=%d\n",
				error, errorItem, numAttributes);
		printf("%s\n", creationAttributes);
		for(i=0;i<numAttributes;i++)
		printf("%d ", itemArray[i]);
		printf("\n");
		for(i=0;i<(attributeArrayLen / 2);i++)
		printf("%d ", attributeArray[i]);
		printf("\n");
	}
	if(error == 0)
	{
		thisFileName[filenameLen] = 0;
		resultString = (*env)->NewStringUTF(env, thisFileName);
	}

	errorArray[0] = error;
	(*env)->ReleaseStringUTFChars(env, jvolName, volName);
	(*env)->ReleaseStringUTFChars(env, jcreationAttributes, creationAttributes);
	(*env)->ReleaseShortArrayElements(env, jerrorArray, errorArray, 0); /* save changes */
	return resultString;
} /* Java_com_tandem_enscribe_EnscribeFile_GfileCreateTemp */
/* ----------------------------------------------------------------------------
 Calls PATHNAME_TO_FILENAME to obtain the Guardian name from an
 oss name
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GgetGuardianName(
		JNIEnv *env, jobject thisEnscribeFile, /*in*/jstring jfileName,
		/*in*/jboolean jfileNameIsOSS) {
	short error, flags, fileNameLen, typeInfo[5];
	char gFileName[100];
	const char *fileName;
	jboolean bfileNameIsCopy;
	jstring guardianName;
	jfieldID gNameID;
	jclass clazz;

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);

	error = PATHNAME_TO_FILENAME_((char *)fileName, gFileName, 100, &fileNameLen, &flags);
	if(error != 0)
	{
		goto bailout;
	}
	gFileName[fileNameLen] = 0;
	clazz = (*env)->GetObjectClass(env, thisEnscribeFile);
	gNameID = (*env)->GetFieldID(env, clazz,"guardianName_","Ljava/lang/String;");
	guardianName = (*env)->NewStringUTF(env,gFileName);
	(*env)->SetObjectField(env, thisEnscribeFile,gNameID,guardianName);

	bailout:

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GgetGuardianName */

/* ----------------------------------------------------------------------------
 Calls FILE_GETINFOBYNAME_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileExists(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jboolean jfileNameIsOSS)
{
	jboolean
	bfileNameIsCopy;
	short
	error,
	flags,
	fileNameLen,
	typeInfo[5];
	const char
	*fileName;
	char
	gFileName[100];

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);
	if(jfileNameIsOSS)
	{
		error = PATHNAME_TO_FILENAME_((char *)fileName, gFileName, 100, &fileNameLen, &flags);
		if(error != 0)
		{
			goto bailout;
		}
		gFileName[fileNameLen] = 0;
	}
	else
	{
		strcpy(gFileName, fileName);
	}

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);

	error = FILE_GETINFOBYNAME_(gFileName, strlen(gFileName), typeInfo, , 0,0,
			&flags);

	bailout:
	if(jEDebugFlag)
	{
		if(jfileNameIsOSS)
		printf("GfileExists: error=%d ossFileName=%s guardianName=%s\n",
				error, fileName, gFileName);
		else
		printf("GfileExists: error=%d guardianName=%s\n",
				error, gFileName);
	}
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GfileExists */

/* ----------------------------------------------------------------------------
 Calls FILE_OPEN and registers the io handler
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileOpen(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jboolean jfileNameIsOSS,
		/*out*/jarray jfileInfo,
		/*in*/jshort jaccess,
		/*in*/jshort jexclusion,
		/*in*/jshort jsyncDepth,
		/*in*/jshort jseqBlockBufferLength,
		/*in*/jboolean junstructuredAccess,
		/*in*/jboolean jnoOpenTimeUpdate,
		/*in*/jboolean juse64BitPrimaryKeyValues)
{

	jboolean
	bfileNameIsCopy,
	bfileInfoArrayIsCopy;
	jshort
	*fileInfoArray;

	spt_error_t spte;
	long dummy;
	short
	typeInfo[5],
	iList[4],
	fnum,
	error;
	unsigned short options;
	const char *fileName;
	union {
		int value;
		struct {
			unsigned int dummy: 31;
			unsigned int use64BitPrimaryKeyValues: 1;
		}flags;
	}elections;

	long rslt, tag;

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);
	fileInfoArray = (*env)->GetShortArrayElements(env, jfileInfo, &bfileInfoArrayIsCopy);

	options = 0x4000; /* bit 1 - nowait open */

	if(jfileNameIsOSS)
	options |= 0x20; /* bit 10, oss pathname  */
	if(junstructuredAccess)
	options |= 0x8000; /* bit 0  */
	if(jnoOpenTimeUpdate)
	options |= 0x2000; /* bit 2   */

	elections.value = 0;
	if(juse64BitPrimaryKeyValues)
	elections.flags.use64BitPrimaryKeyValues = 1;

	error = FILE_OPEN_ ( (char*)fileName, strlen(fileName),
			&fnum,
			jaccess,
			jexclusion,
			1, /* always with nowait */
			jsyncDepth,
			options,
			0,
			jseqBlockBufferLength,,elections.value);

	if(error != 0)
	{
		goto bailout;
	}

	/* register with SPT that we have this file open */
	spte = spt_regFileIOHandler(fnum, &JEnscribe_callback);

	/* If spte not SPT_SUCCESS then the file is already
	 registered or an error occured during register.
	 Return FEINUSE to the caller. */

	if(spte != SPT_SUCCESS)
	return FEINUSE;

	tag = (long)fnum;

	/* Need to use SPT thread wait for open to complete  */

	error = JEnscribeWaitForCompletion(fnum, tag, &dummy);

	/* Error 3(unable to open alternate key file and 4(unable to open
	 * partition) are just warnings.  Error 2 however is a bad guy.
	 */
	if(error == 0 || error == FEPARTFAIL || error == FEKEYFAIL) {

		FILE_GETINFO_ ( fnum, ,,,,typeInfo);

		fileInfoArray[FI_FILENUM_IDX] = fnum;
		fileInfoArray[FI_DEVTYPE_IDX] = typeInfo[0]; /* deviceType_  */
		fileInfoArray[FI_DEVSUBTYPE_IDX] = typeInfo[1]; /* deviceSubtype_   */

		if(typeInfo[0] == 3 ) /* disc file */
		{
			fileInfoArray[FI_FILETYPE_IDX] = typeInfo[3]; /* fileType_ */
			fileInfoArray[FI_FILECODE_IDX] = typeInfo[4]; /* fileCode_ */
			/* find out the maximum record size for this file   */
			if(typeInfo[3]> 0 && !junstructuredAccess) /* fileType_ is structured   */
			iList[0] = 43; /* logical record size */
			else
			iList[0] = 34; /* physical record size  */

			FILE_GETINFOLIST_(fnum, iList, 1, iList, 2);
			fileInfoArray[FI_RECSIZE_IDX] = iList[0];
		}
		else
		{
			fileInfoArray[FI_FILETYPE_IDX] = fileInfoArray[FI_FILECODE_IDX] = -1;
			fileInfoArray[FI_RECSIZE_IDX] = 32000;
		}
	}

	bailout:
	if(jEDebugFlag)
	{
		printf("GfileOpen: error=%d fileName=%s fnum=%d access=%d exclusion=%d\n",
				error, fileName, fnum, jaccess, jexclusion);
		printf("  syncDepth=%d options=%d seqBlockBufferLength=%d\n",
				jsyncDepth, options, jseqBlockBufferLength);
		if(error == 0)
		{
			printf("  devtype=%d subtype=%d fileType=%d fileCode=%d\n",
					typeInfo[0], typeInfo[1], typeInfo[3], typeInfo[4]);
			if(typeInfo[3]> 0 && !junstructuredAccess)
			printf("  recLen=%d\n", fileInfoArray[FI_RECSIZE_IDX]);
		}
	}

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	/* it was a copy, we have to copy changes back */
	if(error == 0 || error == FEPARTFAIL || error == FEKEYFAIL)
	(*env)->ReleaseShortArrayElements(env, jfileInfo, fileInfoArray, 0); /* copy data over   */

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GfileOpen  */

/* ----------------------------------------------------------------------------
 Calls FILE_PURGE_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfilePurge(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jboolean jfileNameIsOSS)
{
	jboolean
	bfileNameIsCopy;

	short
	error = 0;
	const char
	*fileName;

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);

	if(jfileNameIsOSS)
	{
		if(remove(fileName) != 0)
		error = errno;
	}
	else
	{
		error = FILE_PURGE_ ( (char *)fileName, strlen(fileName));
	}

	if(jEDebugFlag)
	{
		printf("GfilePurge: error=%d fileName=%s\n",
				error, fileName);
	}
	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GfilePurge */

/* ----------------------------------------------------------------------------
 Calls rename_guardian
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GfileRename(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jfileName,
		/*in*/jstring jnewFileName,
		/*in*/jboolean jfileNameIsOSS)
{
	jboolean
	bfileNameIsCopy,
	bnewFileNameIsCopy;

	short
	error = 0;
	const char
	*fileName,
	*newFileName;

	fileName = (*env)->GetStringUTFChars(env, jfileName, &bfileNameIsCopy);
	newFileName = (*env)->GetStringUTFChars(env, jnewFileName, &bnewFileNameIsCopy);

	if(rename_guardian(fileName, newFileName) != 0)
	error = errno;

	if(jEDebugFlag)
	{
		printf("GfileRename: error=%d oldFileName=%s newFileName=%s\n",
				error, fileName, newFileName);
	}

	(*env)->ReleaseStringUTFChars(env, jfileName, fileName);
	(*env)->ReleaseStringUTFChars(env, jnewFileName, newFileName);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_GfileRename */

/* ----------------------------------------------------------------------------
 Calls FILE_SETKEY_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gkeyposition(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jboolean jisKeySpecifierNumeric,
		/*in*/jstring jkeySpecifier,
		/*in*/jshort jkeySpecifierNumeric,
		/*in*/jarray jkeyValue,
		/*in*/jshort jcompareLength,
		/*in*/jshort jkeyLength,
		/*in*/jshort jmode,
		/*in*/jboolean jpositionToNextRecord,
		/*in*/jboolean jreadReverse,
		/*in*/jboolean jpositionToLastRecord)
{
	jboolean
	bkeySpecifierCopy = false,
	bkeyValueArrayIsCopy;
	short
	ikeySpec,
	keylen,
	compareLength,
	error;
	union {
		short positionMode;
		struct {
			unsigned int positionToNextRecordBit: 1;
			unsigned int readReverseBit: 1;
			unsigned int positionToLastRecordBit: 1;
		}flags;
	}posMode;

	jbyte *keyValue;
	const char
	*keySpecifier;
	union
	{
		char ks[2];
		short iks;
	}ksConvert;
	keyValue = (*env)->GetByteArrayElements(env, jkeyValue, &bkeyValueArrayIsCopy);

	if(!jisKeySpecifierNumeric)
	{
		keySpecifier = (*env)->GetStringUTFChars(env, jkeySpecifier, &bkeySpecifierCopy);
		memcpy(ksConvert.ks, keySpecifier, 2);
		ikeySpec = ksConvert.iks;
		if(bkeySpecifierCopy)
		(*env)->ReleaseStringUTFChars(env, jkeySpecifier, keySpecifier);
	}
	else
	ikeySpec = jkeySpecifierNumeric;

	keylen = jkeyLength;
	compareLength = jcompareLength;

	posMode.positionMode = 0;

	if(jpositionToNextRecord)
	posMode.flags.positionToNextRecordBit = 1; /* bit 0  */
	if(jreadReverse)
	posMode.flags.readReverseBit = 1; /* bit 1  */
	if(jpositionToLastRecord)
	posMode.flags.positionToLastRecordBit = 1; /* bit 2  */

	FILE_SETKEY_ (jfileNum, (char *)keyValue, keylen, ikeySpec,jmode, posMode.positionMode, compareLength);

	FILE_GETINFO_(jfileNum, &error);

	if(jEDebugFlag)
	{
		printf("Gkeyposition: error=%d fnum=%d keyspec=%d keyLength=%d mode=%d\n",
				error, jfileNum, ikeySpec, jkeyLength, posMode);
		jEDumpBuffer("key value", (const char*)keyValue, strlen((const char*)keyValue));
	}

	(*env)->ReleaseByteArrayElements(env, jkeyValue, keyValue, JNI_ABORT); /* no need to save changes */

	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_Gkeyposition */

/* ----------------------------------------------------------------------------
 Calls LOCKFILE
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Glockfile(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum)
{
	short error;
	long dummy;
	long tag = jfileNum;

	LOCKFILE ( jfileNum, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}
	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Glockfile */

/* ----------------------------------------------------------------------------
 Calls LOCKREC
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Glockrec(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum)
{
	short error;
	long dummy;
	long tag = jfileNum;

	LOCKREC ( jfileNum, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}
	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Glockrec */

/* ----------------------------------------------------------------------------
 Calls FILE_SETPOSITION
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gposition(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jlong jrecordSpecifier)
{
	short error = 0;
	long long recordSpecifier = jrecordSpecifier;

	FILE_SETPOSITION_(jfileNum, recordSpecifier);

	FILE_GETINFO_(jfileNum, &error);
	if(jEDebugFlag)
	{
		printf("Gposition: error=%d fnum=%d recordSpecifier=%d\n",
				error, jfileNum, jrecordSpecifier);
	}

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gposition */

/* ----------------------------------------------------------------------------
 Makes all READ calls
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gread(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jreadType,
		/*in*/jshort jfileNum,
		/*out*/jarray jbuf,
		/*in*/jint jmaxReadCount,
		/*out*/jarray jcountRead)
{
	jboolean
	bbufArrayIsCopy,
	bcountReadArrayIsCopy;
	jint
	*countReadArray;
	long
	tag = jfileNum;
	short error;
	int inBufMaxSize;
	long countRead = 0;

	unsigned short maxReadCount;

	jbyte
	*inBuf;

	inBuf = (*env)->GetByteArrayElements(env, jbuf, &bbufArrayIsCopy);
	inBufMaxSize = (*env)->GetArrayLength(env, jbuf);

	countReadArray = (*env)->GetIntArrayElements(env, jcountRead, &bcountReadArrayIsCopy);

	maxReadCount = jmaxReadCount;
	if(inBufMaxSize < maxReadCount)
	maxReadCount = inBufMaxSize;
	countReadArray[0] = 0;

	switch(jreadType)
	{
		default:
		case READ_TYPE_READ:
		READX ( jfileNum,
				(char*)inBuf, maxReadCount,, tag);

		break;

		case READ_TYPE_READLOCK:
		READLOCKX ( jfileNum,
				(char*)inBuf, maxReadCount,, tag);

		break;

		case READ_TYPE_READUPDATE:
		READUPDATEX ( jfileNum,
				(char*)inBuf, maxReadCount,, tag);

		break;

		case READ_TYPE_READUPDATELOCK:
		READUPDATELOCKX ( jfileNum,
				(char*)inBuf, maxReadCount,, tag);

		break;
	}
	FILE_GETINFO_(jfileNum, &error);

	/* Errors under 10 and number FEDUPALTKEY are warnings for read */

	if(error < 10 || error == FEDUPALTKEY)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &countRead);

		if(error == 0)
		countReadArray[0] = countRead;
	}

	if(jEDebugFlag)
	{
		printf("Gread: type=%d error=%d fnum=%d maxRead=%d  countRead=%d\n",
				jreadType, error, jfileNum, maxReadCount, countReadArray[0]);
		if(error == 0)
		jEDumpBuffer("Read buffer", (const char*)inBuf, countReadArray[0]);
	}
	(*env)->ReleaseIntArrayElements(env, jcountRead, countReadArray, 0); /* save changes */

	(*env)->ReleaseByteArrayElements(env, jbuf, inBuf, 0); /* save changes */

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gread */

/* ----------------------------------------------------------------------------
 calls FILE_RENAME_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GrenameOpenFile(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jstring jnewFileName)
{
	jboolean
	bnewFileNameIsCopy;
	short
	error = 0;
	const char
	*newFileName;
	newFileName = (*env)->GetStringUTFChars(env, jnewFileName, &bnewFileNameIsCopy);

	error = FILE_RENAME_(jfileNum, (char *)newFileName, strlen(newFileName));
	if(jEDebugFlag)
	{
		printf("GrenameOpenFile: error=%d fnum=%d newFileName=%s\n",
				error, jfileNum, newFileName);
	}

	(*env)->ReleaseStringUTFChars(env, jnewFileName, newFileName);
	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GrenameOpenFile */

/* ----------------------------------------------------------------------------
 Calls FILE_RESTOREPOSTION_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GrestorePosition(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jarray jposData,
		/*in*/short jsaveSize)
{
	jboolean
	bposArrayIsCopy;
	short
	error;

	jbyte
	*saveBuf;

	saveBuf = (*env)->GetByteArrayElements(env, jposData, &bposArrayIsCopy);

	FILE_RESTOREPOSITION_( jfileNum, (short*)saveBuf, jsaveSize);

	FILE_GETINFO_(jfileNum, &error);
	if(jEDebugFlag)
	{
		printf("GrestorePosition: error=%d fnum=%d\n",
				error, jfileNum);
		jEDumpBuffer("Restore buffer", (const char*)saveBuf, jsaveSize);
	}

	(*env)->ReleaseByteArrayElements(env, jposData, saveBuf, JNI_ABORT); /* no need to save changes */

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GrestorePosition */

/* ----------------------------------------------------------------------------
 Calls FILE_SAVEPOSITION_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GsavePosition(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*out*/jarray jposData,
		/*out*/jarray jsaveSize)
{
	jboolean
	bposArrayIsCopy,
	bsaveCountArrayIsCopy;
	jshort
	*saveCountArray;

	short
	// Begin of Changes for Solution Number 10-060501-6193
	// isaveBuf[150],
	isaveBuf[300],
	// End of Changes for Solution Number 10-060501-6193 
	error,
	maxSaveCount,
	saveCount = 0;

	jbyte
	*saveBuf;

	saveBuf = (*env)->GetByteArrayElements(env, jposData, &bposArrayIsCopy);
	maxSaveCount = (*env)->GetArrayLength(env, jposData);
	saveCountArray = (*env)->GetShortArrayElements(env, jsaveSize, &bsaveCountArrayIsCopy);

	FILE_SAVEPOSITION_ ( jfileNum, isaveBuf,(short)sizeof(isaveBuf), &saveCount);

	FILE_GETINFO_(jfileNum, &error);
	if(error == 0)
	{
		if(maxSaveCount> saveCountArray[0])
		maxSaveCount = saveCountArray[0];
		saveCount = 2 * saveCount; /* convert words to bytes */
		if(saveCount> maxSaveCount)
		saveCount = maxSaveCount;
		memcpy(saveBuf, isaveBuf, saveCount);
		saveCountArray[0] = saveCount;
	}
	if(jEDebugFlag)
	{
		printf("GsavePosition: error=%d fnum=%d maxSaveCount=%d  saveCount=%d\n",
				error, jfileNum, maxSaveCount, saveCount);
		jEDumpBuffer("Save buffer", (const char*)isaveBuf, saveCount);
	}

	(*env)->ReleaseShortArrayElements(env, jsaveSize, saveCountArray, 0); /* save changes */

	(*env)->ReleaseByteArrayElements(env, jposData, saveBuf, 0); /* save changes */

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GsavePosition */

/* ----------------------------------------------------------------------------
 Calls FILE_GETINFO_ to obtain information about file ownership
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GsetFileOwnership(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jshort jgroup,
		/*in*/jshort juserid)
{

	short error;
	long dummy;

	unsigned short
	opt;
	long
	tag = jfileNum;

	opt = jgroup << 8 | juserid;

	SETMODENOWAIT ( jfileNum , 2, opt, , , tag);
	FILE_GETINFO_(jfileNum, &error);

	if(jEDebugFlag)
	{
		printf("GsetFileOwnership: error= %d fnum=%d group=%d userid=%d\n",
				error, jfileNum, jgroup, juserid);
	}
	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GsetFileOwnership */

/* ----------------------------------------------------------------------------
 Calls FILE_GETINFO_ to obtain information about file security
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GsetFileSecurity(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jshort jreadOpt,
		/*in*/jshort jwriteOpt,
		/*in*/jshort jexecuteOpt,
		/*in*/jshort jpurgeOpt,
		/*in*/jboolean jclearOnPurge,
		/*in*/jboolean jprogid)
{

	short error;
	long dummy;
	unsigned short
	security = 0;

	long
	tag = jfileNum;

	if(jprogid)
	security = 0x8000; /* bit 0 */
	if(jclearOnPurge)
	security |= 0x4000; /* bit 1 */

	security |= jreadOpt << 9 | jwriteOpt << 6 | jexecuteOpt << 3 | jpurgeOpt;

	SETMODENOWAIT ( jfileNum , 1, security, ,, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	if(jEDebugFlag)
	{
		printf("GsetFileSecurity: error= %d fnum=%d rwep=%d%d%d%d progid=%d clearonpurge=%d\n",
				jfileNum, error, jreadOpt, jwriteOpt, jexecuteOpt, jpurgeOpt,
				jprogid, jclearOnPurge);
		printf("  security=%u\n", security);
	}
	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_GsetFileSecurity */

/* ----------------------------------------------------------------------------
 Calls SETMODE
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gsetmode(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jshort jfunction,
		/*in*/jshort jparam1,
		/*in*/jboolean jparam1Omitted,
		/*in*/jshort jparam2,
		/*in*/jboolean jparam2Omitted,
		/*out*/jarray jlastParams)
{
	jboolean
	blpArrayIsCopy;
	short error;
	long dummy;

	long
	tag = jfileNum;

	jshort *lastParamsArray;

	lastParamsArray = (*env)->GetShortArrayElements(env, jlastParams, &blpArrayIsCopy);

	if(jparam1Omitted && jparam2Omitted)
	SETMODENOWAIT ( jfileNum , jfunction, , ,
			lastParamsArray, tag);
	else
	if(jparam2Omitted)
	SETMODENOWAIT ( jfileNum , jfunction, jparam1, ,
			lastParamsArray, tag);
	else
	SETMODENOWAIT ( jfileNum , jfunction, jparam1, jparam2,
			lastParamsArray, tag);

	FILE_GETINFO_(jfileNum, &error);

	if(jEDebugFlag)
	{
		if(jparam1Omitted && jparam2Omitted)
		printf("Gsetmode: error= %d fnum=%d function=%d\n",
				error, jfileNum, jfunction);
		else
		if(jparam2Omitted)
		printf("Gsetmode: error= %d fnum=%d function=%d param1=%d last=%d\n",
				error, jfileNum, jfunction, jparam1, lastParamsArray[0]);
		else
		printf("Gsetmode: error= %d fnum=%d function=%d param1=%d param2=%d  last=%d %d\n",
				error, jfileNum, jfunction, jparam1, jparam2, lastParamsArray[0],
				lastParamsArray[1]);
	}
	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	(*env)->ReleaseShortArrayElements(env, jlastParams, lastParamsArray, 0); /* save changes */
	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gsetmode */

/* ----------------------------------------------------------------------------
 Calls UNLOCKFILE
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gunlockfile(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum)
{
	short error;
	long dummy;
	long
	tag = jfileNum;

	UNLOCKFILE ( jfileNum, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gunlockfile */

/* ----------------------------------------------------------------------------
 Calls UNLOCKREC
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gunlockrec(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum)
{
	short error;
	long dummy;

	long
	tag = jfileNum;

	UNLOCKREC ( jfileNum, tag);
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &dummy);
	}

	return error;

} /* Java_com_tandem_enscribe_EnscribeFile_Gunlockrec */

/* ----------------------------------------------------------------------------
 Calls WRITEX
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gwrite(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jwriteType,
		/*in*/jshort jfileNum,
		/*in*/jarray jbuf,
		/*in*/jint jwriteCount,
		/*out*/jarray jcountWritten)
{

	jboolean
	bbufArrayIsCopy,
	bcountWrittenArrayIsCopy;
	jint
	*countWrittenArray;
	long
	tag = jfileNum;
	short error;
	long countWritten = 0;
	unsigned short
	writeCount;

	jbyte *outBuf;

	outBuf = (*env)->GetByteArrayElements(env, jbuf, &bbufArrayIsCopy);
	countWrittenArray = (*env)->GetIntArrayElements(env, jcountWritten, &bcountWrittenArrayIsCopy);

	writeCount = jwriteCount;
	countWrittenArray[0] = 0;
	switch(jwriteType)
	{
		default:
		case WRITE_TYPE_WRITE:
		WRITEX ( jfileNum,
				(char*)outBuf, writeCount,, tag);

		break;

		case WRITE_TYPE_WRITEUPDATE:
		WRITEUPDATEX ( jfileNum,
				(char*)outBuf, writeCount,, tag);

		break;

		case WRITE_TYPE_WRITEUPDATEUNLOCK:
		WRITEUPDATEUNLOCKX ( jfileNum,
				(char*)outBuf, writeCount,, tag);

		break;
	}
	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &countWritten);

		if(error == 0)
		countWrittenArray[0] = countWritten;
	}

	if(jEDebugFlag)
	{
		printf("Gwrite: type=%d error=%d fnum=%d writeLen=%d  countWritten=%d\n",
				jwriteType, error, jfileNum, writeCount, countWrittenArray[0]);
		jEDumpBuffer("Write buffer", (const char*)outBuf, writeCount);
	}
	(*env)->ReleaseIntArrayElements(env, jcountWritten, countWrittenArray, 0); /* save changes */

	(*env)->ReleaseByteArrayElements(env, jbuf, outBuf, JNI_ABORT); /* don't need changes */

	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_Gwrite  */

/* ----------------------------------------------------------------------------
 Calls WRITEREAD - currently not used
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_Gwriteread(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jfileNum,
		/*in*/jarray jwritebuf,
		/*in*/jshort jwriteCount,
		/*out*/jarray jreadbuf,
		/*in*/jshort jmaxReadCount,
		/*out*/jarray jcountRead)
{

	jboolean
	bwritebufArrayIsCopy,
	breadbufArrayIsCopy,
	bcountReadArrayIsCopy;
	jint
	*countReadArray;
	long
	tag = jfileNum;
	short
	error,
	writebufLen,
	readbufLen,
	combufSize;
	long countRead = 0;

	jbyte
	*writeBuf,
	*readBuf;
	char
	*combuf;

	writeBuf = (*env)->GetByteArrayElements(env, jwritebuf, &bwritebufArrayIsCopy);
	readBuf = (*env)->GetByteArrayElements(env, jreadbuf, &breadbufArrayIsCopy);
	countReadArray = (*env)->GetIntArrayElements(env, jcountRead, &bcountReadArrayIsCopy);

	writebufLen = (*env)->GetArrayLength(env, jwritebuf);
	readbufLen = (*env)->GetArrayLength(env, jreadbuf);

	combufSize = writebufLen;
	if(jmaxReadCount> readbufLen)
	jmaxReadCount = readbufLen;
	if(readbufLen> combufSize)
	combufSize = readbufLen;

	combuf = (char*)malloc(combufSize);

	memcpy(combuf, writeBuf, writebufLen);

	WRITEREADX(jfileNum, combuf, writebufLen, jmaxReadCount,, tag);

	FILE_GETINFO_(jfileNum, &error);

	if(error == 0)
	{
		error = JEnscribeWaitForCompletion(jfileNum, tag, &countRead);

		if(error == 0)
		{
			countReadArray[0] = countRead;
			memcpy(readBuf, combuf, countRead);
		}
	}

	if(jEDebugFlag)
	{
		printf("Gwriteread: error=%d fnum=%d writeLen=%d maxRead=%d readCount=%d\n",
				error, jfileNum, writebufLen, jmaxReadCount, countRead);
		jEDumpBuffer("Write buffer", (const char*)writeBuf, writebufLen);
		if(error == 0)
		jEDumpBuffer("Read buffer", (const char*)combuf, countRead);
	}

	(*env)->ReleaseIntArrayElements(env, jcountRead, countReadArray, 0); /* save changes */

	(*env)->ReleaseByteArrayElements(env, jreadbuf, readBuf, 0); /* save changes */

	(*env)->ReleaseByteArrayElements(env, jwritebuf, writeBuf, JNI_ABORT); /* don't need changes */

	free(combuf);
	return error;
} /* Java_com_tandem_enscribe_EnscribeFile_Gwriteread */

/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
JNIEXPORT void JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GDebugFlag(
		JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jboolean jflag,
		/*in*/jshort jbufferCnt)
{
	jEDebugFlag = jflag;
	jEDebugBufferCnt = jbufferCnt;
} /* Java_com_tandem_enscribe_EnscribeFile_GDebugFlag */

/* ---------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */
JNIEXPORT jboolean JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GgetDebugFlag(
		JNIEnv *env,
		jobject thisEnscribeFile)
{
	return jEDebugFlag;
} /* Java_com_tandem_enscribe_EnscribeFile_GgetDebugFlag */

/* ----------------------------------------------------------------------------
 Calls FILENAME_FINDFINISH_
 * ---------------------------------------------------------------------------- */
JNIEXPORT void JNICALL Java_com_tandem_ext_enscribe_EnscribeFileSearch_GfilenameFindFinish
(JNIEnv *env,
		jobject thisEnscribeFile,
		jshort jsearchID)

{
	FILENAME_FINDFINISH_(jsearchID);
} /* Java_com_tandem_enscribe_EnscribeFileSearch_GfilenameFindFinish */

/* ----------------------------------------------------------------------------
 Calls FILENAME_FINDNEXT_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jstring JNICALL Java_com_tandem_ext_enscribe_EnscribeFileSearch_GfilenameFindNext
(JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jshort jsearchID,
		/*out*/jarray jerrorArray)

{
	jboolean
	berrorArrayIsCopy;
	jshort
	*errorArray;
	short
	error = 0,
	fileNameLen;

	jstring
	resultString = NULL;

	char
	thisFileName[255];

	errorArray = (*env)->GetShortArrayElements(env, jerrorArray, &berrorArrayIsCopy);

	error = FILENAME_FINDNEXT_(jsearchID,
			thisFileName, 255,
			&fileNameLen);
	if(error == 0)
	{
		thisFileName[fileNameLen] = 0;

		resultString = (*env)->NewStringUTF(env, thisFileName);
	}

	errorArray[0] = error;
	(*env)->ReleaseShortArrayElements(env, jerrorArray, errorArray, 0); /* save changes */

	return resultString;
} /* Java_com_tandem_enscribe_EnscribeFileSearch_GfilenameFindNext */

/* ----------------------------------------------------------------------------
 Calls FILENAME_FINDSTART_
 * ---------------------------------------------------------------------------- */
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFileSearch_GfilenameFindStart
(JNIEnv *env,
		jobject thisEnscribeFile,
		/*in*/jstring jsearchPattern,
		/*in*/jshort jresolveLevel,
		/*in*/jstring jstartFileName,
		/*in*/jboolean juseStartFileName,
		/*in*/jboolean jskipStartFileName,
		/*out*/jarray jerrorArray)

{
	jboolean
	bsearchPatternIsCopy,
	bstartFileNameIsCopy = false,
	berrorArrayIsCopy;
	jshort
	*errorArray;
	short
	searchID,
	options = 0,
	error = 0;

	const char
	*searchPattern,
	*startFileName;

	searchPattern = (*env)->GetStringUTFChars(env, jsearchPattern, &bsearchPatternIsCopy);
	errorArray = (*env)->GetShortArrayElements(env, jerrorArray, &berrorArrayIsCopy);

	if(juseStartFileName)
	{
		// get the String startFilename if only startFileName option is set
		startFileName = (*env)->GetStringUTFChars(env, jstartFileName, &bstartFileNameIsCopy);
		if(jskipStartFileName)
		options = 1; /* bit 15 */
		error = FILENAME_FINDSTART_(&searchID,
				(char *)searchPattern, strlen(searchPattern),
				jresolveLevel,
				/*device-type*/,
				/*device-subtype*/,
				options,
				(char *)startFileName, strlen(startFileName));
	}
	else
	error = FILENAME_FINDSTART_(&searchID,
			(char *)searchPattern, strlen(searchPattern),
			jresolveLevel);

	errorArray[0] = error;
	(*env)->ReleaseShortArrayElements(env, jerrorArray, errorArray, 0); /* save changes */
	(*env)->ReleaseStringUTFChars(env, jsearchPattern, searchPattern);

	// Begin Changes for Solution Number 10-040420-5227

	// release the String startFileName if only StartFileName option is set

	if(juseStartFileName)
	(*env)->ReleaseStringUTFChars(env, jstartFileName, startFileName);

	//End Changes for Solution Number 10-040420-5227

	return searchID;
} /* Java_com_tandem_enscribe_EnscribeFileSearch_GfilenameFindStart */
/*-------------------------------------------------------------------------------
 Calls a routine in jEcmaFuncs which cancels the last active operation
 *-------------------------------------------------------------------------------*/
JNIEXPORT jshort JNICALL Java_com_tandem_ext_enscribe_EnscribeFile_GcancelReq(
		JNIEnv *env, jobject thisEnscribeFile,/*in*/jshort jfileNum)
{
	short error;
	long tag = jfileNum;

	error = JEnscribeCancelReq(jfileNum,tag);
	return error;
}

// Begin changes for solution number 10-060921-9232

/* ----------------------------------------------------------------------------
 Calls the GUARDIAN routines FILENAME_SCAN_ and FILENAME_RESOLVE
 * ---------------------------------------------------------------------------- */
JNIEXPORT jint JNICALL Java_com_tandem_ext_enscribe_FileNameConverter_jniToFullyQualifiedFileName
(JNIEnv *env, jobject thisFilenameResolver, /*in*/jbyteArray partialFileName,
		/*out*/jbyteArray fullName) {
	
    int ret = 0;
    int inLength = 0;
    short outLength = 0;


    jbyte jpartialFileName[MAX_LENGTH];
    jbyte jfullFileName[MAX_LENGTH];

    memset(jpartialFileName, 0, MAX_LENGTH);
    memset(jfullFileName, 0, MAX_LENGTH);
    
    inLength = (*env)->GetArrayLength(env, partialFileName);
    (*env)->GetByteArrayRegion(env, partialFileName, 0, inLength, jpartialFileName);

    ret = FILENAME_SCAN_( (char*) jpartialFileName,
	        (short) inLength,
	        /* count */,
            /* kind */,
            /* level */,
            /* options */);

    if (ret != 0)
	     return ret;

    ret = FILENAME_RESOLVE_( (char*) jpartialFileName,
            (short) inLength,
            (char *)jfullFileName,
            MAX_LENGTH,
            &outLength,
            /* options */,
            /* override name */,
            /* length */,
            /* search */,
            /* length */,
            /* defaults */,
           /* length */);

    if (ret != 0)
	     return ret;

    (*env) ->SetByteArrayRegion(env, fullName, 0, outLength, jfullFileName);

    return ret;
}

/* ----------------------------------------------------------------------------
 Calls the GUARDIAN routine OLDFILENAME_TO_FILENAME
 * ---------------------------------------------------------------------------- */

JNIEXPORT jint JNICALL Java_com_tandem_ext_enscribe_FileNameConverter_jniToDSeriesFileName
(JNIEnv *env, jobject thisFilenameResolver, /*in*/jbyteArray oldFileName,
		/*out*/jbyteArray fileName) {

	int ret = 0;
	int inLength = 0;
    short outLength = 0;

    jbyte joldFileName[CSERIES_LENGTH];
    jbyte jfileName[MAX_LENGTH];
    
    memset(joldFileName, 0, CSERIES_LENGTH);
    memset(jfileName, 0, MAX_LENGTH);

    inLength = (*env)->GetArrayLength(env, oldFileName);
    (*env)->GetByteArrayRegion(env, oldFileName, 0, inLength, joldFileName);

    ret = OLDFILENAME_TO_FILENAME_((short *)joldFileName,
            (char *)jfileName,
            MAX_LENGTH,
            &outLength);

    if (ret != 0)
	     return ret;

    (*env) ->SetByteArrayRegion(env, fileName, 0, outLength, jfileName);

    return ret;
}

// End changes for solution number 10-060921-9232

/*-----------------------------------------------------------------------------
 Throws an OutOfMemoryError
 *-----------------------------------------------------------------------------*/
void jEthrowOutOfMem(JNIEnv *env) {
	jclass clazz;
	char *msg = "Unable to obtain for java extensions for enscribe";

	clazz = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
	if (clazz == 0)
		return;
	(*env)->ThrowNew(env, clazz, msg);
	return;
}

