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
// Filename:  java1.h
//--------------------------------------------------------------------------
#ifndef __JAVA1__
#define __JAVA1__

struct java_reserved_words_t {
	char *word;
};
struct java_reserved_words_t java_reserved_words[] = {
	"abstract", "boolean", "break", "byte", "byvalue", "case", "cast",
   "catch", "char", "class", "const", "continue",
   "default", "do", "double", "else", "extends", "false", "final",
   "finally", "float", "for", "future", "generic",
   "goto", "if", "implements", "import", "inner", "instanceof", "int",
   "interface", "long", "native", "new", "null",
   "operator", "outer", "package", "private", "protected", "public", "rest",
   "return", "short", "static", "super", "switch",
   "synchronized", "this", "throw", "throws",
   "transient", "true", "try", "var", "void", "volatile", "while"
};
#define NUM_RESERVED_WORDS (sizeof(java_reserved_words)/sizeof(struct java_reserved_words_t))

#endif // java1.h


