/*
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.

 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.

 * @ @ @ END COPYRIGHT @ @ @
 */


/*
 * Copyright 2004
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.

 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.

 * @ @ @ END COPYRIGHT @ @ @
 */


/*
 * @(#)jni_md.h	1.17 03/01/23
 *
 * Copyright 2003 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT 
#define JNIIMPORT
#define JNICALL

typedef int jint;
#ifdef _LP64 /* 64-bit Solaris */
typedef long jlong;
#else
typedef long long jlong;
#endif

typedef signed char jbyte;

#endif /* !_JAVASOFT_JNI_MD_H_ */
