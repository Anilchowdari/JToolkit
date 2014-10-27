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
// Filename:  detailList.h
//--------------------------------------------------------------------------
#ifndef __DETAILLIST__
#define __DETAILLIST__
#include "ddl2java.h"
#include "ddlread.h"
#include "ddl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

class DetailListProcessor
{
public:
	DetailListProcessor();
	~DetailListProcessor();

	bool addToDetailList(ddl_element_def element, bool exp);
	void check4JavaReservedWord(DETAIL_LIST *item);
	detailListHolder getDetailList();

protected:
	int 	num_elements;
	int 	fillerCount;
};
#endif __DETAILLIST__
