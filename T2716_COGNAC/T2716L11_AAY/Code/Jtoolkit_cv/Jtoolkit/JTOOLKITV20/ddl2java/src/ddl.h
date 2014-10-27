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
// Filename:  ddlh.h
//--------------------------------------------------------------------------
#ifndef __DDLH__
#define __DDLH__

//--------------------------------------------------------------------------
// Note:  Must use Alignement = 1 when compiling
//   ogdbsvr (NSK)  Fieldalign shared2
//   TSOGGen (VC++) Project | Settings | C/C++ | Code Generation | Structure Alignment = 1 Byte
//--------------------------------------------------------------------------


#define CURRENT_VERSION				"DB10"

#define RC_COMPARE_DDL_MOD_DATES 'C1'
#define RC_READ_APPLICATION_SO_RECS	'R2'
#define RC_READ_DDL_DEF 			'R3'
#define RC_READ_DDL_DEF_STRUCT 	'R4'
#define RC_READ_ALL_DICT	'R5'
#define RC_READ_ALL_DEF	'R6'
#define RC_READ_ALL_REC 'R7'
#define RC_READ_ONE_REC 'R8'
#define RC_READ_ONE_DEF 'R9'

// Reply codes
#define EC_DDL_VERSION_NOT_SUPPORTED 'D1'
#define EC_DDL_TIMESTAMP_MISMATCH 'D2'
#define EC_HOST_FILE_ERROR 		'H1'
#define EC_NO_RECS_FOUND			'N5'
#define EC_OK					      'OK'
#define EC_REQUEST_TOO_SHORT		'R1'
#define EC_UNKNOWN_REQUEST 	 	'U2'
#define EC_UNSUPPORTED_VERSION	 'U5'

// size defines
#define CLIENT_LANG_SIZE	30
#define CONV_METHOD_SIZE	36
#define DDL_COMMENT_SIZE	134
#define DDL_VALUE_SIZE 134
#define DDL_NAME_SIZE	30
#define DDL_PIC_TEXT_SIZE	30
#define ERROR_TEXT_SIZE	80
#define HOST_FILE_SIZE  	36
#define HOST_SUBVOL_SIZE	28
#define SYSTEM_NAME_SIZE	8
#define VERSION_SIZE	4
#define SQL_LEN_SIZE 2
#define SQL_INDICATOR_SIZE 2

// data type defines
#define ALPHANUMERIC 0
#define NUM_STR_UNSIGNED 1
#define BINARY_16_SIGNED 2
#define BINARY_16_UNSIGNED 3
#define BINARY_32_SIGNED 4
#define BINARY_32_UNSIGNED 5
#define BINARY_64_SIGNED 6
#define FLOAT32 8
#define COMPLEX32_2 9
#define FLOAT64 10
#define NUM_STR_TRAIL_EMB_SIGN 12
#define NUM_STR_TRAIL_SEP_SIGN 13
#define NUM_STR_LEAD_EMB_SIGN 14
#define NUM_STR_LEAD_SEP_SIGN 15
#define LOGICAL_1 17
#define LOGICAL_2 19
#define LOGICAL_4 21
#define BINARY_8_SIGNED 22
#define BINARY_8_UNSIGNED 23
#define SQLVARCHAR 24
#define ENUMERATION 25
#define BITSIGNED 26
#define BITUNSIGNED 27
#define NATIONALSTRING 28
#define SQLDATETIMEYEAR 32
#define SQLDATETIMEMONTH 33
#define SQLDATETIMEYEARTOMONTH 34
#define SQLDATETIMEDAY 35
#define SQLDATETIMEMONTHTODAY 36
#define SQLDATETIMEYEARTODAY 37
#define SQLDATETIMEHOUR 38
#define SQLDATETIMEDAYTOHOUR 39
#define SQLDATETIMEMONTHTOHOUR 40
#define SQLDATETIMEYEARTOHOUR 41
#define SQLDATTIMEMINUTE 42
#define SQLDATETIMEHOURTOMINUTE 43
#define SQLDATETIMEDAYTOMINUTE 44
#define SQLDATETIMEMONTHTOMINUTE 45
#define SQLDATETIMEYEARTOMINUTE 46
#define SQLDATETIMESECOND 47
#define SQLDATETIMEMINUTETOSECOND 48
#define SQLDATETIMEHOURTOSECOND 49
#define SQLDATETIMEDAYTOSECOND 50
#define SQLDATETIMEMONTHTOSECOND 51
#define SQLDATETIMEYEARTOSECOND 52
#define SQLDATETIMEFRACTION 53
#define SQLDATETIMESECONDTOFRACTION 54
#define SQLDATETIMEMINUTETOFRACTION 55
#define SQLDATETIMEHOURTOFRACTION 56
#define SQLDATETIMEDAYTOFRACTION 57
#define SQLDATETIMEMONTHTOFRACTION 58
#define SQLDATETIMEYEARTOFRACTION 59
#define SQLINTERVALYEAR 60
#define SQLINTERVALMONTH 61
#define SQLINTERVALYEARTOMONTH 62
#define SQLINTERVALDAY 63
#define SQLINTERVALHOUR 64
#define SQLINTERVALDAYTOHOUR 65
#define SQLINTERVALMINUTE 66
#define SQLINTERVALHOURTOMINUTE 67
#define SQLINTERVALDAYTOMINUTE 68
#define SQLINTERVALSECOND 69
#define SQLINTERVALMINUTETOSECOND 70
#define SQLINTERVALHOURTOSECOND 71
#define SQLINTERVALDAYTOSECOND 72
#define SQLINTERVALFRACTION 73
#define SQLINTERVALSECONDTOFRACTION 74
#define SQLINTERVALMINUTETOFRACTION 75
#define SQLINTERVALHOURTOFRACTION 76
#define SQLINTERVALDAYTOFRACTION 77
// Fake data type for length and indicator fields that go with SQL data types
#define SQLLENGTHORINDICATOR 99

// Conversion Routine type Defines
#define TEXT_ROUTINE 1
#define NUMERIC_ROUTINE 2

// Maximum number in replies
#define MAX_DDL_IPCS	10
#define MAX_DDL_ELEMENTS	1000
#define NAMESIZE 310
#define PLISTSIZE 10

typedef char             	client_language_def[CLIENT_LANG_SIZE];
typedef char             	conversion_method_def[CONV_METHOD_SIZE];
typedef char             	ddl_comment_def[DDL_COMMENT_SIZE];
typedef unsigned short   	ddl_element_id_def;
typedef char             	ddl_name_def[DDL_NAME_SIZE];
typedef unsigned long    	ddl_object_id_def;
typedef char		   		ddl_ODF_id_def[2];
typedef char               	host_filename_def[HOST_FILE_SIZE];
typedef char               	subvol_name_def[HOST_SUBVOL_SIZE];

typedef struct __ddl_identifier
{
    ddl_object_id_def              ddl_object_id;
    ddl_element_id_def             ddl_element_id;
} ddl_identifier;

typedef struct __ddl_dict_information
{
   short                           number_elements;
   short                           ipc_maximum_length;
   subvol_name_def                 dict_host_subvol;
   ddl_name_def                    ipc_name;
} ddl_dict_information;

// Changed this structure to support DDL2
// Changed the occurs_max also to supportDDL2
typedef struct __ddl_element_dc
{
   short            	seq_num;
   short            	element_type;
   ddl_name_def        	ddl_element_name;
   short                level;
   int                  host_offset;
   int                  host_length;
   short                host_data_type;
   short                host_precision;
   short                host_scale;
   char                 filler_1[2];
   int                  occurs_max;
   short                occurs_depends_upon;
   short                element_redefined;
   char				   	sqlnullable;
   int  			   	sql_length;
   short                needs_discriminator;
   short	  		   	owns_discriminator;
   short			   	hasBeenInitialized;
} ddl_element_dc_def;

typedef struct __ddl_element
{
	ddl_element_dc_def				ddl_element_dc;
	ddl_name_def					source_def;
	char                            is_user_filler;
	char                            is_ddl_filler;
	char                            ddl_pic_text[DDL_PIC_TEXT_SIZE];
	ddl_comment_def                 ddl_comment;
   short                           	ddl_value_len;
   char                            	ddl_value_text[DDL_VALUE_SIZE];
} ddl_element_def;

typedef struct __ddl_ODF_key
{
	ddl_ODF_id_def	odf_id;
	ddl_name_def                    def_name;
}ddl_ODF_key_def;

typedef struct __ddl_ipc
{
	ddl_object_id_def				ddl_object_id;
	ddl_ODF_key_def					ddl_ODF_key;
} ddl_ipc_def;


typedef struct __read_dictionary_req
{
   int				               request_code;
   subvol_name_def                 dict_subvol;
   short                           num_wanted;
   ddl_name_def                    start_def_characters;
   ddl_name_def                    end_def_characters;
   ddl_ODF_key_def	next_ddl_ODF_key;
} readDictReq;

typedef struct __read_ddl_def_reply
{
   	int								status;
   	short							num_returned;
	char							more_flag;
 	char							filler_0;
   	ddl_ODF_key_def					next_ddl_ODF_key;
   	subvol_name_def					ddl_subvol_expanded;
   	ddl_ipc_def						def_info[MAX_DDL_IPCS];
} readDictRep;

typedef struct __read_ddl_def_struct_req
{
   int                 				request_code;
   subvol_name_def                 	dict_subvol;
   ddl_object_id_def				ddl_object_id;
   ddl_element_id_def              	next_ddl_element_id;
   short                           	num_wanted;
} readDefStructureReq;

typedef struct __read_ddl_def_struct_reply
{
   	int                  			status;
   	short                           num_returned;
	char 							more_flag;
	char							filler_0;
   	ddl_element_id_def              next_ddl_element_id;
   	ddl_element_def             	element_info[MAX_DDL_ELEMENTS];
} readDefStructureRep;

typedef struct __read_rdf_struct_reply
{
	int								status;
	ddl_object_id_def				ddl_object_id;
}readRdfStructureRep;

typedef struct _bigName
{
	int   currNameLen;
   	char *Name;
} bigName;

typedef struct _ddl_detail_info
{
   bigName									qualName;
   bigName									discriminator;
   char 									discriminatorValue[32];
   char										qualified_name_required_flag;
   char										parent_occurs_flag;
   ddl_element_def						    element_info;

} ddl_detail_info;

typedef struct detail_list_t DETAIL_LIST;

typedef struct detail_list_t
{
   DETAIL_LIST								*pItem;
   DETAIL_LIST								*nItem;
   DETAIL_LIST								*parent;
   ddl_detail_info							detail_info;
} ;
extern struct detail_list_t ddl_detail_info_struct;

typedef struct _parent_list
{
	bigName 	parentName;
   	int  		numParentOccurs;
   	int  		parentDataLen;
   	int			offset;
} parent_name_list;

typedef struct _full_parent_list
{
	parent_name_list	pList[PLISTSIZE];
}PNAME_LIST;

typedef struct _parent_len_list
{
	int parentLen[7];
}PLENGTH_LIST;
typedef struct _detailListStruct
{
	DETAIL_LIST						*head;
	int								numElements;
} detailListHolder;
#endif // ddl.h
