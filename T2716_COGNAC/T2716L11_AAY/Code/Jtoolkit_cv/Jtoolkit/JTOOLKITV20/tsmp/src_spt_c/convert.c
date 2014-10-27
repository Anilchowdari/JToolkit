/*
 * @(#)convert.c
 *
 *  @@@ START COPYRIGHT @@@
 *
 *             Copyright 2002 Compaq Computer Corporation
 *             Protected as an unpublished work.
 * 
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
 *
 *  @@@ END COPYRIGHT @@@
 *
 */
 
/*****************************************************************************

     Jpathsend Data Conversion Functions

This module provides support for converting to and from Tandem C and Cobol
datatypes, and Java datatypes. The functions in this module are
written in C to be called from Java as native methods. They are
wriiten to the Java 1.2 JNI specification.

3/02/1998 - First version

*****************************************************************************/

#include "com_tandem_tsmp_TsmpServerReply.h"
#include "com_tandem_tsmp_TsmpServerRequest.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "limits.h"
#include "errno.h"

/* Define misc. useful constants */
#define null 0
#define false 0
#define true 1
#define decimalPt  '.'

/* Define possiable error constants */
#define CVERR_OK 0        /* Number converted sucsessfuly */
#define CVERR_RANGE 1     /* Conversion of numeric data causes overflow */
#define CVERR_SYNTAX 2    /* Error while parsing numeric string */
#define CVERR_PARAM 3     /* Error with input parm to conversion function */
#define CVERR_TRUNC 4     /* Converted data is truncated to fit output field length */
#define CVERR_INTERN 5    /* Internal error */

/* Enumerated type for Numeric string Trailing/Leading
   Imbedded/Separate Sign
*/
#define TDMTYPE_LSSNUM 12 /* Leading Separate Sign */
#define TDMTYPE_TSSNUM 13 /* Trailing Separate Sign */
#define TDMTYPE_LESNUM 14 /* Leading Imbedded Sign */
#define TDMTYPE_TESNUM 15 /* Trailing Imbedded Sign */
#define TDMTYPE_UNSNUM 16 /* Unsigned */

/* return minimum of value1 or value2 */
int min (int value1, int value2) {

  return (value1 < value2 ) ? value1 : value2;

};

/*****************************************************************************

This procedure is used to get a pointer to the instance
of the request buffer for obj class.

*****************************************************************************/
jbyte *getReqBuf( JNIEnv *env, jobject obj, jobject *reqBufferObj) {

  jfieldID bufferFieldID;
  jclass javaClass;

  javaClass = (*env)->GetObjectClass(env, obj);
  bufferFieldID = (*env)->GetFieldID(env, javaClass, "_requestBuffer", "[B");
  *reqBufferObj = (*env)->GetObjectField(env, obj ,bufferFieldID);
  return (*env)->GetByteArrayElements(env, *reqBufferObj, 0);

} ;

/***************************************************************************** 

This function releases the request buffer and copies data back to Java
buffer.

*****************************************************************************/
void releaseReqBuf(JNIEnv *env, jobject reqBufferObj, jbyte *reqBufferPtr) {
  (*env)->ReleaseByteArrayElements(env, reqBufferObj, reqBufferPtr, 0);
};


/*****************************************************************************

This procedure is used to get a pointer to the instance
of the reply buffer for obj class.

*****************************************************************************/
jbyte *getReplyBuf( JNIEnv *env, jobject obj, jobject *replyBufferObj) {

  jfieldID bufferFieldID;
  jclass javaClass;

  javaClass = (*env)->GetObjectClass(env, obj);
  bufferFieldID = (*env)->GetFieldID(env, javaClass, "_replyBuffer", "[B");
  *replyBufferObj = (*env)->GetObjectField(env, obj ,bufferFieldID);
  return (*env)->GetByteArrayElements(env, *replyBufferObj, 0);

} ;

/*****************************************************************************

This function releases reply buffer. Data is NOT copied back
because it is not changed by Reply conversion functions.
Converted values are returned by function.
*****************************************************************************/
void releaseReplyBuf(JNIEnv *env, jobject replyBufferObj, jbyte *replyBufferPtr) {
  (*env)->ReleaseByteArrayElements(env, replyBufferObj, replyBufferPtr, JNI_ABORT);
};


/*****************************************************************************

        Convert Java 16 bit unicode to 8 bit Ascii C string.

        Null byte is added and value is copied to request buffer at offset. Function
        checks that string will fit in fieldSize but assumes there is fieldSize room
        for the string in request buffer.	

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size converted value should fit into.

        RETURNS:  
           CVERR_PARAM is returned if the output field length is invalid.
           CVERR_OK is returned if no error.


*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToCStr ( JNIEnv *env, jobject obj, jstring inString, jint offset, jint fieldSize  )
{
        jobject reqBufferObj;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);	
 
        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Make sure string will fit in field size */
        size_t len = strlen(str);
	if ( len == 0 )
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return ( CVERR_PARAM );
        }
        if (len +1 > fieldSize) 
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return ( CVERR_PARAM );
        }

        /* Copy string to request buffer at offset, this is C string so add null */
        memcpy(buf + offset, str, len +1);

        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);

        return ( CVERR_OK );
}

/*****************************************************************************

        Convert Java 16 bit unicode to 8 bit Ascii Cobol string.

        Value is copied to request buffer at offset.
        Function checks that string will fit in fieldSize but
        assumes there is fieldSize room for the string in request buffer.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size converted value should fit into.

        RETURNS:  
           CVERR_PARAM is returned if the output field length is invalid.
           CVERR_OK is returned if no error.

*****************************************************************************/
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToCobolStr ( JNIEnv *env, jobject obj, jstring inString, jint offset, jint fieldSize )
{

        char *strPtr;
	jobject reqBufferObj;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);	

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Make sure string will fit in field size */
        size_t len = strlen(str);
	if ( len == 0 ) 
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return ( CVERR_PARAM );
        }
        if (len > fieldSize) 
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return ( CVERR_PARAM );
        }
	strPtr = (char *) malloc(fieldSize +1);
        if(strPtr == null)
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return (CVERR_PARAM);
        }
        memset(strPtr,' ',fieldSize +1);
        memcpy(strPtr,str,len);

        /* Copy string to request buffer at offset, do not copy null  */
	memcpy(buf + offset ,strPtr, fieldSize); 
        
        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);
	free(strPtr);
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
		
        return ( CVERR_OK );	

}

/*****************************************************************************

        Convert Java 16 bit unicode to Cobol national string or C char array.
	(Since Java strings have no null terminator, no special C null 
	terminated array conversion is needed.)

        Value is copied to request buffer at offset.
        Function checks that string will fit in fieldSize but
        assumes there is fieldSize room for the string in request buffer.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size (in bytes) converted value should fit into.

        RETURNS:  
           CVERR_PARAM is returned if the output field length is invalid.
           CVERR_OK is returned if no error.

*****************************************************************************/
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToStr16 ( JNIEnv *env, jobject obj, jstring inString, jint offset, jint fieldSize )
{

        char *strPtr;
	jobject reqBufferObj;
        jsize len;

        /* Get input string, in Java 16 bit unicode */
        const jchar *str = (*env)->GetStringChars(env, inString, 0);	

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Get place to put string */
	strPtr = (char *) malloc(fieldSize +1);
        if(strPtr == null) 
        {
           (*env)->ReleaseStringChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           return (CVERR_PARAM);
        }

        /* Make sure string will fit in field size */
        len = (*env)->GetStringLength(env, inString);
	if ( len == 0 ) 
        {
           (*env)->ReleaseStringChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           free(strPtr);
           return ( CVERR_PARAM );
        }
        if ((len*2) > fieldSize) 
        {
           (*env)->ReleaseStringChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf); 
           free(strPtr);
           return ( CVERR_PARAM );
        }

        memset(strPtr,' ',fieldSize +1);
        memcpy(strPtr,str,fieldSize);

        /* Copy string to request buffer at offset, do not copy null  */
	memcpy(buf + offset ,strPtr, fieldSize); 
        
        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringChars(env, inString, str);
	free(strPtr);
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
		
        return ( CVERR_OK );	

}

/*****************************************************************************
      Convert an optionally signed decimal numeric string to a decimal numeric string
      with a specified format and scale.  The input string may not contain leading or
      trailing blanks or a terminating null; it may contain a leading sign specifier,
      leading zeros, and/or a decimal point.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size converted value should fit into.
           jint nOutScale      Scale to use.
           jint nOutType      Enumerated type for Numeric string Trailing/Leading  Imbedded/Separate Sign


        RETURNS:
           CVERR_PARAM is returned if the output field length is invalid.
           CVERR_OK is returned if no error.

*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToNumStr ( JNIEnv *env, jobject obj, jstring inString, jint offset, jint fieldSize, jint nOutScale, jint nOutType )
{
	jbyte				*lpInDig, *lpOutDig, *lpSave;
	int					negative;
        jbyte* lpIn;
        jbyte* lpOutStart;
        unsigned int nInSize, nOutSize;
        jobject reqBufferObj;

        const char *str = (*env)->GetStringUTFChars(env, inString, 0);	
        jbyte *lpOut = getReqBuf(env, obj, &reqBufferObj);
        lpOutStart = lpOut;
        lpOut = lpOut + offset;
        lpIn = (jbyte *)str;
        nInSize = strlen((const char *)lpIn);
        nOutSize = fieldSize;

	if ( nInSize == 0 )
        {
          (*env)->ReleaseStringUTFChars(env, inString, str);

	 /* Begin Changes for Solution 10-050222-4864 */
         /* releaseReqBuf(env, reqBufferObj, lpOut); */
	    releaseReqBuf(env, reqBufferObj, lpOutStart);
         /* End Changes for Solution 10-050222-4864 */

          return ( CVERR_SYNTAX );
        } 
	negative = ( *lpIn == '-' );
	if ( *lpIn == '-' || *lpIn == '+' ) lpIn++, nInSize--;

	if ( nInSize == 0 || nInSize == 1 && *lpIn == decimalPt )
        {
          (*env)->ReleaseStringUTFChars(env, inString, str);

	  /* Begin Changes for Solution 10-050222-4864 */
          /* releaseReqBuf(env, reqBufferObj, lpOut); */
             releaseReqBuf(env, reqBufferObj, lpOutStart);
          /* End Changes for Solution 10-050222-4864 */

          return ( CVERR_SYNTAX );
        }
	memset( lpOut, '0', (size_t) nOutSize );

	switch ( nOutType )
	{
	case TDMTYPE_LSSNUM:
		if ( nOutSize < 2 ) 
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_PARAM );
                }
		*lpOut++ = (jbyte)( negative ? '-' : '+' ), nOutSize--;
		break;

	case TDMTYPE_TSSNUM:
		if ( nOutSize < 2 ) 
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_PARAM );
                }
		lpOut [ --nOutSize ] = (jbyte)( negative ? '-' : '+' );
		break;

	case TDMTYPE_LESNUM:
		if ( nOutSize < 1 )
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_PARAM );
                } 
		if ( negative ) lpOut [0] |= 0x80;
		break;

	case TDMTYPE_TESNUM:
		if ( nOutSize < 1 )
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_PARAM );
                } 
		if ( negative ) lpOut [ nOutSize - 1 ] |= 0x80;
		break;

	case TDMTYPE_UNSNUM:
		if ( nOutSize < 1 )
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_PARAM );
                }
		if ( negative )
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_RANGE );
                }
		break;

	default:
                /* Begin Changes for Solution 10-050222-4864 */
		/* return ( CVERR_PARAM ); */

                (*env)->ReleaseStringUTFChars(env, inString, str);

                 releaseReqBuf(env, reqBufferObj, lpOutStart);
                 /* End Changes for Solution 10-050222-4864 */

                return ( CVERR_PARAM );
 }

	/*
		Skip leading zeros and scan digits in the integer part of the number.  When the
		last integer digit is scanned, move the significant digits into the integer field
		of the result, checking for overflow first.
	*/

/* Looks like this is skiping nulls not spaces ?? */
	while ( nInSize > 0 && *lpIn == '0' ) lpIn++, nInSize--;
	lpSave = lpIn;
	while ( nInSize > 0 && isdigit ( *lpIn ) ) lpIn++, nInSize--;

	lpOutDig = lpOut + (signed) ( nOutSize - nOutScale );
	lpInDig = lpIn;
	if ( nOutScale < 0 ) lpOutDig += nOutScale, lpInDig += nOutScale;

	if ( lpOutDig - lpOut < lpInDig - lpSave )
        {
            (*env)->ReleaseStringUTFChars(env, inString, str);

            /* Begin Changes for Solution 10-050222-4864 */ 
            /* releaseReqBuf(env, reqBufferObj, lpOut); */
               releaseReqBuf(env, reqBufferObj, lpOutStart);
            /* End Changes for Solution 10-050222-4864 */

            return ( CVERR_RANGE );
        }
	while ( lpInDig > lpSave ) *--lpOutDig |= *--lpInDig;

	if ( nInSize > 0 && *lpIn == decimalPt )
	{
		signed short		scale;

		/*
			The number has a fraction part.  Skip leading zeros and scan digits in this
			part.  When the last digit is scanned, move the significant digits into the
			fraction field of the result, checking for overflow first.  Discard any input
			digits of lower order than the output scale, or use zeros for output digits
			of lower order than the input scale.
		*/

		lpInDig = ++lpIn, nInSize--;
/* Looks like this is skiping nulls not spaces ?? */
		while ( nInSize > 0 && *lpIn == '0' ) lpIn++, nInSize--;
		lpSave = lpIn;
		while ( nInSize > 0 && isdigit ( *lpIn ) ) lpIn++, nInSize--;

		scale = (short) ( lpIn - lpInDig );

		lpInDig = lpIn;
		lpOutDig = lpOut + nOutSize;
		if ( scale > nOutScale ) lpInDig -= scale - nOutScale;
		else
		if ( nOutScale > scale ) lpOutDig -= nOutScale - scale;

		if ( lpOutDig - lpOut < lpInDig - lpSave )
                {
                   (*env)->ReleaseStringUTFChars(env, inString, str);

                   /* Begin Changes for Solution 10-050222-4864 */
                   /* releaseReqBuf(env, reqBufferObj, lpOut); */
                      releaseReqBuf(env, reqBufferObj, lpOutStart);
                   /* End Changes for Solution 10-050222-4864 */

                   return ( CVERR_RANGE );
                } 

		while ( lpInDig > lpSave ) *--lpOutDig |= *--lpInDig;
	}

        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */

        releaseReqBuf(env, reqBufferObj, lpOutStart);

	return ( nInSize > 0 ? CVERR_SYNTAX : CVERR_OK );
}

/*****************************************************************************

        Convert Java 16 bit unicode to 8 bit Ascii. String should be
        only one byte long.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:  
           CVERR_PARAM is returned if the length is greater than one byte.
           CVERR_OK is returned if no error.

*****************************************************************************/
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToBIN8 ( JNIEnv *env, jobject obj, jstring inString, jint offset )
{
        jobject reqBufferObj;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);	

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Make sure string is only one byte */
        size_t len = strlen(str);
	if ( len !=1 )
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf);
           return (CVERR_PARAM);
        }

        /* Move byte into buffer at offset */
	buf[offset] = (jbyte)str[0]; 
        
        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
		
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Convert Java 16 bit unicode to 8 bit Ascii for Cobol.  A two byte string
        is built for Cobol. Input String should be only one byte long. A space is
        added in second byte for cobol.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_PARAM is returned if the length is greater than one byte.
           CVERR_OK is returned if no error.

*****************************************************************************/
        
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToBIN8Cobol ( JNIEnv *env, jobject obj, jstring inString, jint offset )
{
        jobject reqBufferObj;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);	

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Make sure string is only one byte */
        size_t len = strlen(str);
	if ( len !=1 )
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf);
           return (CVERR_PARAM);
        }
        /* Move byte into bufer at offset and add space in second byte */
        buf[offset] = (jbyte)*str;
        buf[offset +1] = ' ';
                
         /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
		
        return ( CVERR_OK );
   

}

/*****************************************************************************

        Put 16 bit signed value into request buffer at offset.
        String value was converted to a short and explicit decimal
        points are removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jshort shortValue   Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.


*****************************************************************************/
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaShortToBIN16 ( JNIEnv *env, jobject obj,  jshort shortValue, jint offset )
{

typedef union {
	unsigned char			byte [2];
	unsigned short			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Convert value to bytes */
        value.word = shortValue;
        
        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 2);       
        		
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************

        Put 16 bit unsigned value into request buffer at offset.
        String value was converted to a short and explicit decimal
        points are removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint intValue       Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
           CVERR_RANGE if the integer is greater than the largest unsigned short


*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaIntToUNBIN16 ( JNIEnv *env, jobject obj,  jint intValue, jint offset )
{

typedef union {
	unsigned char			byte [2];
	unsigned short			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        if (intValue > USHRT_MAX)
        {
           releaseReqBuf(env, reqBufferObj, buf);
           return (CVERR_RANGE);
        }

        /* Convert value to bytes */
        value.word = intValue;
        
        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 2);       
        		
       /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Put 32 bit signed value into request buffer at offset.
        String value was converted to a short and explicit decimal
        points are removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint intValue       Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaIntToBIN32 ( JNIEnv *env, jobject obj, jint intValue, jint offset )
{

typedef union {
	unsigned char			byte [4];
	unsigned int			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Convert value to bytes */
        value.word = intValue;
        
        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 4);       
        		
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Put 32 bit unsigned value into request buffer at offset.
        String value was converted to a short and explicit decimal
        points are removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jlong longValue     Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
           CVERR_RANGE if the long is greater than the largest unsigned int
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaLongToUNBIN32 ( JNIEnv *env, jobject obj, jlong longValue, jint offset )
{

typedef union {
	unsigned char			byte [4];
	unsigned int			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);
        if (longValue > UINT_MAX)
        {
           releaseReqBuf(env, reqBufferObj, buf);
           return (CVERR_RANGE);
        }
        value.word = longValue;
        
        memcpy(buf + offset, value.byte, 4);       
        		
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Put 64 bit signed value into request buffer at offset.
        String value was converted to a short and explicit decimals
        were removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
            CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaLongToBIN64 ( JNIEnv *env, jobject obj, jlong longlongValue, jint offset )

{

typedef union {
        unsigned char               byte [8];
        long long                    word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Convert value to bytes */
        value.word = longlongValue;

        /* Move Value into request buffer */
        memcpy(buf + offset , value.byte, 8);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );


}

/*****************************************************************************

        Convert Java 16 bit unicode to 8 bit Ascii. String should be
        only one byte long.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_PARAM is returned if the length is not one byte.
           CVERR_OK is returned if no error.

*****************************************************************************/
jint Java_com_tandem_tsmp_TsmpServerRequest_JavaStrToLogical1 ( JNIEnv *env, jobject obj, jstring inString, jint offset )
{
        jobject reqBufferObj;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Make sure string is only one byte */
        size_t len = strlen(str);
        if ( len !=1 )
        {
           (*env)->ReleaseStringUTFChars(env, inString, str);
           releaseReqBuf(env, reqBufferObj, buf);
           return (CVERR_PARAM);
        }

        /* Move byte into buffer at offset */
        buf[offset] = (jbyte)str[0];

        /* Tell java we no longer need to access to copy of input string */
        (*env)->ReleaseStringUTFChars(env, inString, str);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);

        return ( CVERR_OK );
  

}


/*****************************************************************************
        Put 16 bit value into request buffer at offset.
        String value was converted to a short and explicit decimals
        were removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaShortToLogical2 ( JNIEnv *env, jobject obj, jshort shortValue, jint offset )
{

typedef union {
        unsigned char                   byte [2];
        unsigned short                  word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Convert value to bytes */
        value.word = shortValue;

        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 2);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
  

}

/*****************************************************************************
        Put 32 bit signed value into request buffer at offset.
        String value was converted to a short and explicit decimals
        were removed on Java side.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_JavaLongToLogical4 ( JNIEnv *env, jobject obj, jlong longValue, jint offset )
{

typedef union {
        unsigned char           byte [4];
        long                    word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Convert value to bytes */
        value.word = (long) longValue;

        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 4);

        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
  

}

/*****************************************************************************
        Put float value into request buffer at offset .
        Tandem format float string is converted to a float
        value.
        
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
            CVERR_OK is returned if no error.
            CVERR_RANGE if no conversion or over/under flow. 
            
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_StrToFloat (JNIEnv *env, jobject obj, jstring inString, jint offset )
{

typedef union {
	unsigned char		byte [4];
	float			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;
        char **none = null;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Convert value to bytes */
        value.word = (float)strtod(str, none);

        /* Let strtod return the largest or smallest value now. */
#if 0
        /* Check for error conditions */
        if (errno == EINVAL || errno == ERANGE ){ 
          (*env)->ReleaseStringUTFChars(env, inString, str);
          releaseReqBuf(env, reqBufferObj, buf);
          return CVERR_RANGE;
        }
#endif

        /* Move Value into request buffer */
        memcpy(buf + offset , value.byte, 4);       
        (*env)->ReleaseStringUTFChars(env, inString, str);
        		
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Put float64 (double) value into request buffer at offset .
        Tandem format double string is converted to a double
        value.
        
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_OK is returned if no error.
           CVERR_RANGE if no conversion or over/under flow.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_StrToDouble (JNIEnv *env, jobject obj, jstring inString, jint offset )
{

typedef union {
	unsigned char		byte [8];
	double			word;
} SHORT;


        SHORT value;
        jobject reqBufferObj;
        char **none = null;

        /* Get request buffer for this class instance */
        jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Convert value to bytes */
        value.word = strtod(str, none);

        /* Let strtod return the largest or smallest value now. */
#if 0
        /* Check for error conditions */
        if (errno == EINVAL || errno == ERANGE ){
          (*env)->ReleaseStringUTFChars(env, inString, str);
          releaseReqBuf(env, reqBufferObj, buf);
          return CVERR_RANGE;
        }
#endif
        
        /* Move Value into request buffer */
        memcpy(buf + offset, value.byte, 8);       
        (*env)->ReleaseStringUTFChars(env, inString, str);
        		
        /* JNI sees copy of Java request buffer, release and have Java copy
           the values back */
        releaseReqBuf(env, reqBufferObj, buf);
        return ( CVERR_OK );
   

}

/*****************************************************************************
        Put BITvalue into request buffer at offset.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint BITvalue       Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_PARAM is returned if BITvalue > 32767 or  BITvalue < 0
           CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_BITtoTdmShort (JNIEnv *env, jobject obj, jint BITvalue, jint offset )
{

typedef union {
        unsigned char          byte [2];
        short                    word;
} SHORT;


      SHORT value;
      jobject reqBufferObj;

      /* Get request buffer for this class instance */
      jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

      /*  Make sure BITvalue is in right range */
      if (BITvalue > 32767 || BITvalue < 0)
      {
         releaseReqBuf(env, reqBufferObj, buf);
         return (CVERR_PARAM);
      }

      /* Convert value to bytes */
      value.word = (short)BITvalue;
 
      /* Move Value into request buffer */
      memcpy( buf + offset ,value.byte, 2);      

      /* JNI sees copy of Java request buffer, release and have Java copy
         the values back */
      releaseReqBuf(env, reqBufferObj, buf);
      return ( CVERR_OK );
}

/*****************************************************************************
        Put Unsigned BITvalue into request buffer at offset.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint BITvalue       Input value to be converted.
           jint offset         Offset at which to place converted value in request buffer.

        RETURNS:
           CVERR_PARAM is returned if BITvalue > 65535 or  BITvalue < 0
           CVERR_OK is returned if no error.
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerRequest_UBITtoTdmShort (JNIEnv *env, jobject obj, jint BITvalue, jint offset )
{

typedef union {
        unsigned char          byte [2];
        unsigned short           word;
} SHORT;


      SHORT value;
      jobject reqBufferObj;
      /* Get request buffer for this class instance */
      jbyte *buf = getReqBuf(env, obj, &reqBufferObj);

      /*  Make sure BITvalue is in right range */
      if (BITvalue > 65535 || BITvalue < 0)
      {
         releaseReqBuf(env, reqBufferObj, buf);
         return (CVERR_PARAM);
      }

      /* Convert value to bytes */
      value.word = (short)BITvalue;
 
      /* Move Value into request buffer */
      memcpy(buf + offset, value.byte , 2);      

      /* JNI sees copy of Java request buffer, release and have Java copy
         the values back */
      releaseReqBuf(env, reqBufferObj, buf);
      return ( CVERR_OK );
}


/*****************************************************************************
        Convert a fixed-length, decimal numeric string with a specified format and scale
        to an optionally signed decimal numeric string.  The input string may not contain
        leading or trailing blanks or a terminating null; it may contain leading zeros
        and, depending on the format, a sign specifier.  

         INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to value out of  reply buffer.
           jint fieldSize      Field size of value in reply buffer.
           jint nInScale      Scale to use.
           jint nInType      Enumerated type for Numeric string Trailing/Leading Imbedded/Separate Sign

        RETURNS:
           jstring : Value from reply buffer converted to a Java string or null string if error.
           
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_NumStrToJavaStr ( JNIEnv *env, jobject obj, jint offset, jint fieldSize, jint nInScale, jint nInType )
{
        jobject replyBufferObj;

        jbyte				*lpEmbSign = NULL;
	unsigned short			nWorkSize;
        size_t  nInSize;

        jbyte szWorkBuff[256];
        jbyte *npWorkBuff;
        jbyte *lpInSav;

        jbyte *lpIn  = getReplyBuf(env, obj, &replyBufferObj);
        lpInSav = lpIn;
        lpIn = lpIn + offset;
        npWorkBuff = szWorkBuff;
        nInSize = fieldSize;

        switch ( nInType )
	{
	case TDMTYPE_LSSNUM:
	 	if ( nInSize < 2 ) goto HandleErrors;  /* return ( CVERR_PARAM );*/
		if ( lpIn [0] != '-' && lpIn [0] != '+' ) goto HandleErrors; /*return ( CVERR_SYNTAX ); */
		if ( lpIn [0] == '-' ) *npWorkBuff++ = '-';
		lpIn++, nInSize--;
		break;

	case TDMTYPE_TSSNUM:
		if ( nInSize < 2 ) goto HandleErrors;  /* return ( CVERR_PARAM ); */
		if ( lpIn [ nInSize - 1 ] != '-' && lpIn [ nInSize - 1 ] != '+' )
			goto HandleErrors; /*return ( CVERR_SYNTAX ); */
		if ( lpIn [ nInSize - 1 ] == '-' ) *npWorkBuff++ = '-';
		nInSize--;
		break;

	case TDMTYPE_LESNUM:
		if ( nInSize < 1 ) goto HandleErrors; /* return ( CVERR_PARAM ); */
		lpEmbSign = lpIn;
		if ( *lpEmbSign & 0x80 ) *npWorkBuff++ = '-', *lpEmbSign &= ~ 0x80; 
		break;

	case TDMTYPE_TESNUM:
		if ( nInSize < 1 ) goto HandleErrors; /* return ( CVERR_PARAM );  */
		lpEmbSign = lpIn + nInSize - 1;
		if ( *lpEmbSign & 0x80 ) *npWorkBuff++ = '-', *lpEmbSign &= ~ 0x80;
		break;

	case TDMTYPE_UNSNUM:
		if ( nInSize < 1 ) goto HandleErrors; /* return ( CVERR_PARAM ); */
		break;

	default:
		goto HandleErrors; /* return ( CVERR_PARAM ); */
                ;
	}

	if ( nInScale >= (signed) nInSize )
	{
		/* The number has only a fraction part, and may require extra leading zeros. */

		if ( nInScale > (signed short) sizeof szWorkBuff - 2 )
		{
			if ( lpEmbSign && szWorkBuff [0] == '-' ) *lpEmbSign |= 0x80;
			goto HandleErrors; /* return ( CVERR_INTERN ); */
		};

		*npWorkBuff++ = decimalPt;

		memset ( npWorkBuff, '0', (size_t) ( nInScale - nInSize ) );
		npWorkBuff += nInScale - nInSize;

		memcpy( npWorkBuff, lpIn, (size_t) nInSize );
		npWorkBuff += nInSize;
	}
	else
	if ( nInScale > 0 )
	{
		/* The number has both an integer part and a fraction part.  Trim leading zeros
		   from the integer part, leaving at least one digit. */

		if ( nInSize > (short) sizeof szWorkBuff - 2 )
		{
			if ( lpEmbSign && szWorkBuff [0] == '-' ) *lpEmbSign |= 0x80;
			goto HandleErrors; /* return ( CVERR_INTERN ); */
		}

		while ( nInSize > (unsigned) nInScale && *lpIn == '0' ) lpIn++, nInSize--;

		memcpy( npWorkBuff, lpIn, (size_t) ( nInSize - nInScale ) );
		npWorkBuff += nInSize - nInScale;

		*npWorkBuff++ = decimalPt;

		memcpy( npWorkBuff, lpIn + ( nInSize - nInScale ), (size_t) nInScale );
		npWorkBuff += nInScale;
	}
	else
	{
		/* 'nInScale' is zero or negative.  The number has no fraction part, and may
		   require extra trailing zeros to make it an integer.  Trim leading zeros from
		   the integer part, leaving at least one digit.  Add trailing zeros if the
		   number is not zero. */

		if ( nInSize - nInScale > (short) sizeof szWorkBuff - 2 )
		{
			 if ( lpEmbSign && szWorkBuff [0] == '-' ) *lpEmbSign |= 0x80;
			goto HandleErrors; /* return ( CVERR_INTERN ); */
		}

		while ( nInSize > 1 && *lpIn == '0' ) lpIn++, nInSize--;

		 memcpy( npWorkBuff, lpIn, (size_t) nInSize );

		npWorkBuff += nInSize;

		if ( nInScale < 0 && ( nInSize > 1 || *lpIn != '0' ) )
		{
		memset( npWorkBuff, '0', (size_t) ( - nInScale ) );
			npWorkBuff -= nInScale;
		}
	}

	if ( lpEmbSign && szWorkBuff [0] == '-' ) *lpEmbSign |= 0x80;

	nWorkSize = (unsigned short) ( npWorkBuff - szWorkBuff );

        /* turn into C string for NewString*/
        szWorkBuff[nWorkSize] = '\0';
        releaseReplyBuf(env, replyBufferObj, lpInSav);
        return (*env)->NewStringUTF(env, (const char *)szWorkBuff );

   HandleErrors:
        releaseReplyBuf(env, replyBufferObj, lpInSav);
        return NULL;
}

/*****************************************************************************
        Takes string out of reply buf and converts to Java string.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size of value in reply buffer to be converted.
           
 
        RETURNS:
           jstring: Converted value as a Java string or null string if error.
          
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_StrToJavaStr ( JNIEnv *env, jobject obj, jint offset, jint fieldSize ) {
        jobject replyBufferObj;
        char* copyField;
        jstring javaString;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Turn into C string for NewStringUTF */
        copyField = calloc(1, fieldSize + 1);
        /* Check to see if there was space */
        if (copyField == NULL) {
         javaString = NULL;
        } else {
        /* Make a Java String  */
         strncat(copyField,((char *)buf + offset),fieldSize);
         javaString  = (*env)->NewStringUTF(env, (const char *)copyField );
         free(copyField);
        };

	/* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return javaString;
               
}

/*****************************************************************************
        Puts double byte character string from a reply buf into a Java string.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to place converted value in request buffer.
           jint fieldSize      Field size of value in reply buffer to be converted.
           
 
        RETURNS:
           jstring: Converted value as a Java string or null string if error.
          
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_StrToJavaStr16 ( JNIEnv *env, jobject obj, jint offset, jint fieldSize ) {
        jobject replyBufferObj;
        jchar* copyField;
        jstring javaString;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Make a Java String  */
        copyField = calloc(1, fieldSize + 2);
        /* Check to see if there was space */
        if (copyField == NULL) {
         javaString = NULL;
        } else {
	 memcpy((char *)copyField,((char *)buf + offset),fieldSize);
         memset((char *)(copyField + fieldSize), 0, 2);
         javaString  = (*env)->NewString(env, copyField, fieldSize/2);
         free(copyField);
        };

	/* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return javaString;
               
}

/*****************************************************************************
        Takes BIN8 out of reply buf and converts to Java string.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to place converted value in request buffer.
                      
 
        RETURNS:
           jstring: Converted value as a Java string.
          
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_BIN8ToJavaStr ( JNIEnv *env, jobject obj, jint offset )
{
        jobject replyBufferObj;
        char str[2]; 

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Turn into C string for NewStringUTF */
        str[0] = buf[offset];
        str[1] = '\0';
        
        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return (*env)->NewStringUTF(env, (const char *)&str); 

}
/*****************************************************************************
        Takes BIN16 out of reply buf and converts to Java short.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset          Offset at which to take reply value from.
                      
 
        RETURNS:
           jshort: Converted value as a Java short.
          
*****************************************************************************/

jshort Java_com_tandem_tsmp_TsmpServerReply_BIN16ToJavaShort (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
	unsigned char			byte [2];
	short    			word;
} SHORT;


        SHORT value;
        jobject replyBufferObj;
        
        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        // Begin Changes for Solution Number 10-040505-5748
         
        if(!buf)
          (*env)->FatalError(env ,"getReplyBuf failed");

        // End Changes for Solution Number 10-040505-5748
                        
        /* Move value from Reply buffer  */
        memcpy( value.byte,  buf + offset, 2);

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return  value.word;	
   

}
/*****************************************************************************
        Takes Unsigned BIN16 out of reply buf and converts to Java int.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset          Offset at which to take reply value from.
                      
 
        RETURNS:
           jint: Converted value as a Java int.
          
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerReply_UNBIN16ToJavaInt (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
        unsigned char                 byte [2];
        unsigned short                word;
} SHORT;


       SHORT value;
       jobject replyBufferObj;

       /* Get reply buffer for this class instance */
       jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

       /* Move value from Reply buffer  */
       memcpy( value.byte,  buf + offset, 2);

       /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
       releaseReplyBuf(env, replyBufferObj, buf);
       return  value.word;
  

}

/*****************************************************************************
        Takes  BIN32 out of reply buf and converts to Java int.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jint: Converted value as a Java int.
          
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerReply_BIN32ToJavaInt (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
	unsigned char			byte [4];
	int      			word;
} SHORT;

         
        SHORT value;
        jobject replyBufferObj;
        
        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);
                        
        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 4);       

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return value.word;	
 
}

/*****************************************************************************
        Takes  unsigned BIN32 out of reply buf and converts to Java long.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jlong: Converted value as a Java long.
          
*****************************************************************************/

jlong Java_com_tandem_tsmp_TsmpServerReply_UNBIN32ToJavaLong (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
        unsigned char                   byte [4];
        unsigned int                    word;
} SHORT;

   
        SHORT value;
        jobject replyBufferObj;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 4);

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return value.word;

}

/*****************************************************************************
        Takes  BIN64 out of reply buf and converts to Java long.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jlong: Converted value as a Java long.
          
*****************************************************************************/

jlong Java_com_tandem_tsmp_TsmpServerReply_BIN64ToJavaLong (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
        unsigned char                   byte [8];
        long long                       word;
} SHORT;


        SHORT value;
        jobject replyBufferObj;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 8);

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return value.word;

}

/*****************************************************************************
        Takes Logical1 out of reply buf and converts to Java string.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jstring: Converted value as a Java string.
          
*****************************************************************************/
jstring Java_com_tandem_tsmp_TsmpServerReply_Logical1ToJavaStr ( JNIEnv *env, jobject obj, jint offset )
{
        jobject replyBufferObj;
        char str[2];

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Turn into C string for NewStringUTF */
        str[0] = buf[offset];
        str[1] = '\0';

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return (*env)->NewStringUTF(env, (const char *)&str);

}

/*****************************************************************************
        Takes Logical2 out of reply buf and converts to Java int.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jint: Converted value as a Java int.
          
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerReply_Logical2ToJavaInt (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
        unsigned char                   byte [2];
        unsigned short                  word;
} SHORT;


        SHORT value;
        jobject replyBufferObj;
   
        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);
   
        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 2);

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return  value.word;
  

}

/*****************************************************************************
        Takes Logical4 out of reply buf and converts to Java long.
       
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jlong: Converted value as a Java long.
          
*****************************************************************************/

jlong Java_com_tandem_tsmp_TsmpServerReply_Logical4ToJavaLong (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
        unsigned char                   byte [4];
        unsigned int                    word;
} SHORT;

   
        SHORT value;
        jobject replyBufferObj;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 4);
 
        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return value.word;

}

/*****************************************************************************
        Takes Float32 out of reply buf and converts to Java  float.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jstring: Converted value as a Tandem float string.
          
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_Float32ToStr (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
	unsigned char		byte [8];
	float			word;
} SHORT;

        SHORT value;
        jobject replyBufferObj;
        char str[30];
        int ret;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);
                        
        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 8);       
        ret = sprintf(str,"%.7G", value.word);
        if (ret <0) {
         str[0] = 0;
         strcat(str, "NaN");
        }
        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return (*env)->NewStringUTF(env, (const char *)&str);
 
}

/*****************************************************************************
        Takes Float64 out of reply buf and converts to Tandem double string.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jstring: Converted value as a Java double.
          
*****************************************************************************/

jstring Java_com_tandem_tsmp_TsmpServerReply_Float64ToStr (JNIEnv *env, jobject obj, jint offset )
{

typedef union {
	unsigned char		byte [16];
	double			word;
} SHORT;
 

        SHORT value;
        jobject replyBufferObj;
        char str[30];
        int ret;

        /* Get reply buffer for this class instance */
        jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);
                        
        /* Move value from Reply buffer  */
        memcpy( value.byte, buf + offset, 16);       
        ret = sprintf(str,"%.17G", value.word);
        if (ret <0) {
         str[0] = 0;  
         strcat(str, "NaN");
        }

        /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
        releaseReplyBuf(env, replyBufferObj, buf);
        return (*env)->NewStringUTF(env, (const char *)&str);

}

/*****************************************************************************
        Takes short out of reply buf and converts to Java  int.
        
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jint: Converted value as a Java int.
          
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerReply_TdmShortToBIT (JNIEnv *env, jobject obj, jint offset )
{
typedef union {
	unsigned char			byte [2];
	unsigned short			word;
} SHORT;

      SHORT value;
      jobject replyBufferObj;

      /* Get reply buffer for this class instance */
      jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);
 
      /* Move value from Reply buffer  */
      memcpy(value.byte, buf + offset, 2);      

      /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
      releaseReplyBuf(env, replyBufferObj, buf);
      return value.word;

}

/*****************************************************************************
        Takes short out of reply buf and converts to Java  int.
        
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jobject obj         native method pointer.
           jint offset         Offset at which to take reply value from.
                      
 
        RETURNS:
           jint: Converted value as a Java int.
          
*****************************************************************************/

jint Java_com_tandem_tsmp_TsmpServerReply_TdmShortToUBIT (JNIEnv *env, jobject obj, jint offset )
{
typedef union {
	unsigned char			byte [2];
	unsigned short			word;
} SHORT;

      SHORT value;
      jobject replyBufferObj;

      /* Get reply buffer for this class instance */
      jbyte *buf = getReplyBuf(env, obj, &replyBufferObj);

      /* Move value from Reply buffer  */
      memcpy(value.byte, buf + offset, 2);
  
      /* JNI sees copy of Java reply buffer, tell VM we do not need it anymore */
      releaseReplyBuf(env, replyBufferObj, buf);
      return  value.word;
   
}
