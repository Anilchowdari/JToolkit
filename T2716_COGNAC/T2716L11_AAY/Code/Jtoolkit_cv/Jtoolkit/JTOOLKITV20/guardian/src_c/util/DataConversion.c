/* ------------------------------------------------------------------------
* Filename:     DataConversion.c
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2001
*   Compaq Computer Corporation
*      Protected as an unpublished work.
*         All rights reserved.
*
*   The computer program listings, specifications, and documentation
*   herein are the property of Compaq Computer Corporation or a
*   third party supplier and shall not be reproduced, copied,
*   disclosed, or used in whole or in part for any reason without
*   the prior express written permission of Compaq Computer
*   Corporation.
*
*  @@@ END COPYRIGHT @@@
*
* ------------------------------------------------------------------------ */

/*****************************************************************************
    Data Conversion Functions

This module provides support for converting to and from Tandem C and Cobol
datatypes, and Java datatypes. The functions in this module are
written in C to be called from Java as native methods. They are
written to the Java 1.2 JNI specification.

This file was copied from the convert.c that is part of the tsmp package and
then modified.

******************************************************************************/

#include "com_tandem_ext_util_DataConversion.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "kfpconv.h"  //Solution Number: 10-060905-8795

/*****************************************************************************
        Put float value into request buffer at offset .
        Tandem format float string is converted to a float
        value.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           void
*****************************************************************************/

JNIEXPORT void JNICALL Java_com_tandem_ext_util_DataConversion_StrToFloatNative
  (JNIEnv *env, jclass jcls, jbyteArray buf, jstring inString, jint offset)
{
        typedef union {
            unsigned char           byte [4];
            float                   word;
        } SHORT;


        SHORT value;
        char **none = 0;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Convert value to bytes */
        value.word = (float)strtod(str, none);

        /* Move Value into request buffer */
        (*env)->SetByteArrayRegion(env, buf, offset, 4, (jbyte*) value.byte);
        (*env)->ReleaseStringUTFChars(env, inString, str);
}

/*****************************************************************************
        Put double value into request buffer at offset .
        Tandem format double string is converted to a double
        value.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           void
*****************************************************************************/

JNIEXPORT void JNICALL Java_com_tandem_ext_util_DataConversion_StrToDoubleNative
  (JNIEnv *env, jclass jcls, jbyteArray buf, jstring instring, jint offset)
{
        typedef union {
            unsigned char           byte [8];
            double                  word;
        } SHORT;


        SHORT value;
        char **none = 0;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, instring, 0);

        /* Convert value to bytes */
        value.word = strtod(str, none);

        /* Move Value into request buffer */
        (*env)->SetByteArrayRegion(env, buf, offset, 8, (jbyte*) value.byte);
        (*env)->ReleaseStringUTFChars(env, instring, str);
}

/*****************************************************************************
        Takes Float32 out of reply buf and converts to Java  float.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           jstring: Converted value as a Tandem float string.

*****************************************************************************/


JNIEXPORT jstring JNICALL Java_com_tandem_ext_util_DataConversion_Float32ToStrNative(JNIEnv * env, jclass jcls, jbyteArray buf, jint offset)
{
typedef union {
        unsigned char           byte [4]; //Solution Number: 10-060905-8795
        float                   word;
} SHORT;

        SHORT value;
        char str[30];
        int ret;

        /* Move value from Reply buffer  */
        (*env)->GetByteArrayRegion(env, buf, offset, 8, (jbyte*) value.byte);
        ret = sprintf(str,"%.7G", value.word);
        if (ret <0) {
         str[0] = 0;
         strcat(str, "NaN");
        }

        return (*env)->NewStringUTF(env, (const char *)&str);
}

/*****************************************************************************
        Takes Float64 out of reply buf and converts to Java double

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           jstring: Converted value as a Tandem double string.

*****************************************************************************/
JNIEXPORT jstring JNICALL Java_com_tandem_ext_util_DataConversion_Float64ToStrNative (JNIEnv * env, jclass jcls, jbyteArray buf, jint offset)
{
typedef union {
        unsigned char           byte [8];  //Solution Number: 10-060905-8795
        double                  word;
} SHORT;


        SHORT value;        char str[30];
        int ret;

        /* Move value from Reply buffer  */
        (*env)->GetByteArrayRegion(env, buf, offset, 16, (jbyte*) value.byte);
        ret = sprintf(str,"%.17G", value.word);
        if (ret <0) {
         str[0] = 0;
         strcat(str, "NaN");
        }

        return (*env)->NewStringUTF(env, (const char *)&str);
}

/* Begin changes for Solution Number: 10-060905-8795 */

/*****************************************************************************
        Put 32-bit TNS Float value into request buffer at offset.
        32-bit IEEE Float string is converted to a 32-bit TNS Float.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           void

*****************************************************************************/
JNIEXPORT void JNICALL Java_com_tandem_ext_util_DataConversion_StrIEEEToTNSFloat32Native
(JNIEnv *env, jclass jcls, jbyteArray buf, jstring inString, jint offset)
/*  
(JNIEnv *, jclass, jbyteArray, jstring, jint)
*/
{
        typedef union {
            unsigned char           byte [4];
            NSK_float_tns32         word;
        } SHORT;
        NSK_float_ieee32 ieee_num;


        SHORT value;
        char **none = 0;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Convert value to bytes */
        ieee_num.u.f32 = (float)strtod(str, none);
        // convert ieee float value to tns float
        NSK_FLOAT_IEEE32_TO_TNS32_(&ieee_num, &value.word );

        /* Move Value into request buffer */
        (*env)->SetByteArrayRegion(env, buf, offset, 4, (jbyte*) value.byte);
        (*env)->ReleaseStringUTFChars(env, inString, str);
}

/*****************************************************************************
        Put 64-bit TNS Float value into request buffer at offset.
        64-bit IEEE Float string is converted to a 64-bit TNS Float.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jstring inString    Input value to be converted.
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           void
*****************************************************************************/
JNIEXPORT void JNICALL Java_com_tandem_ext_util_DataConversion_StrIEEEToTNSFloat64Native
(JNIEnv *env, jclass jcls, jbyteArray buf, jstring inString, jint offset)
/*
(JNIEnv *, jclass, jbyteArray, jstring, jint)
*/
{
        typedef union {
            unsigned char           byte [8];
            NSK_float_tns64         word;
        } SHORT;
        NSK_float_ieee64 ieee_num;


        SHORT value;
        char **none = 0;

        /* Get input string, change Java 16 bit unicode to 8 bit Ascii */
        const char *str = (*env)->GetStringUTFChars(env, inString, 0);

        /* Convert value to bytes */
        ieee_num.u.f64 = strtod(str, none);
        // convert ieee float value to tns float
        NSK_FLOAT_IEEE64_TO_TNS64_(&ieee_num, &value.word );

        /* Move Value into request buffer */
        (*env)->SetByteArrayRegion(env, buf, offset, 8, (jbyte*) value.byte);
        (*env)->ReleaseStringUTFChars(env, inString, str);
}

/*****************************************************************************
        Takes 32-bit TNS float out of reply buf and converts to java 32-bit IEEE float.
        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           jstring: Converted value as a TNS Float32 string.

*****************************************************************************/
JNIEXPORT jfloat JNICALL Java_com_tandem_ext_util_DataConversion_Float32TNSToIEEENative
(JNIEnv * env, jclass jcls, jbyteArray buf, jint offset)
{
        typedef union {
            unsigned char           byte [4];
            NSK_float_tns32         word;
        } SHORT;
        NSK_float_ieee32 ieee_num;

        SHORT value;

        /* Move value from Reply buffer  */
        (*env)->GetByteArrayRegion(env, buf, offset, 4, (jbyte*) value.byte);
        NSK_FLOAT_TNS32_TO_IEEE32_(&value.word, &ieee_num);
        return ieee_num.u.f32;
}

/*****************************************************************************
        Takes 64-bit TNS float out of reply buf and converts to Java 64-bit IEEE float.

        INPUTS:
           JNIEnv *env         JNI interface pointer.
           jclass jcls         native method pointer.
           jbyteArray buf      buffer to place output value
           jint offset         Offset at which to place converted value in reque
st buffer.

        RETURNS:
           jstring: Converted value as a Tandem double string.

*****************************************************************************/
JNIEXPORT jdouble JNICALL Java_com_tandem_ext_util_DataConversion_Float64TNSToIEEENative
(JNIEnv * env, jclass jcls, jbyteArray buf, jint offset)
{
        typedef union {
            unsigned char           byte [8];
            NSK_float_tns64         word;
        } SHORT;
        NSK_float_ieee64 ieee_num;

        SHORT value;

        /* Move value from Reply buffer  */
        (*env)->GetByteArrayRegion(env, buf, offset, 8, (jbyte*) value.byte);
        NSK_FLOAT_TNS64_TO_IEEE64_(&value.word, &ieee_num);
        return ieee_num.u.f64;
}

/* End Changes for Solution Number: 10-060905-8795 */