/*---------------------------------------------------------------------------
 * Copyright 2004
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

#ifndef __UTILH
#define __UTILH

void util_display_error_form (char * pText);
void util_display_warn_form(char * pText);
char * util_form_entry_value (char * pName);
char * util_escape_html (char * pString);
char * util_format_money (long long * pAmount);
void util_display_error_begin_form();
void util_display_error_msg_form(char * pText);
void util_display_error_end_form();
char * removeTrailingCharacters(char *pName);
#endif
