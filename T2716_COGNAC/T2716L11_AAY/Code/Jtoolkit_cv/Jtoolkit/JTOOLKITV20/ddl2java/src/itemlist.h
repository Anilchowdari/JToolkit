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
// Filename:  itemList.h
//--------------------------------------------------------------------------
#ifndef __ITEMLIST__
#define __ITEMLIST__
#include "ddl2java.h"

class ItemList
{
public:
	ItemList();
	~ItemList();

   void closeFile();
   bool openFile(char *itemPath);
   int readItemInfo(itemInfo *listItem, int defaultLanguage, char *dictPath);

protected:

   char   	fileName[MAXPATHLEN];
   FILE		*fp;
   void		upShiftIt(char *original, char *upShiftedName);
};


#endif // __ItemList__

