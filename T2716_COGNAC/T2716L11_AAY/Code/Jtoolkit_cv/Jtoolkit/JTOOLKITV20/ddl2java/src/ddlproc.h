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
// Filename:  ipcproc.h
//--------------------------------------------------------------------------
#ifndef __IPCPROCH__
#define __IPCPROCH__
#include "ddl2java.h"
#include "ddlread.h"
class ddl2Java;
class DDLProcessor
{
public:
	DDLProcessor();
   ~DDLProcessor();

	void ddl_read_ddl_defs(char * requestBuf, char * replyBuf);
	void ddl_read_ddl_elements(char * requestBuf, char * replyBuf);

};
#endif // __IPCPROCH__
