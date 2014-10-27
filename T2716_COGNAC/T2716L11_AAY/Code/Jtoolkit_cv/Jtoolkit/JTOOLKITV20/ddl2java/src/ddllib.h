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
// Filename:  ddllib.h
//--------------------------------------------------------------------------
#ifndef __DDLLIBH__
#define __DDLLIBH__

#define  MINDDLVERSION	6
#ifdef _TNS_X_TARGET
#define  MAXDDLVERSION  9
#else 
#define  MAXDDLVERSION  7
#endif

#define  DDLDICTDDF ".DICTDDF"
#define  DDLDICTOBL ".DICTOBL"
#define  DDLDICTODF ".DICTODF"
#define  DDLDICTOTF ".DICTOTF"
#define  DDLDICTRDF ".DICTRDF"

#define DDLODFALTKEY 'ID'

typedef char                       file_name_def[34];
typedef char                       device_name[16];
typedef unsigned short             timestamp_def[3];
typedef char                       ascii_switch_def;
typedef char                       name_def[30];
typedef char                       sgname_def[15];
typedef unsigned long              object_number_def;

typedef struct
   {
   object_number_def               text_id;
   unsigned short                  line_number;
   } otf_identifier_def;

typedef struct {
	object_number_def			   object;
} rdf_identifier_def;

typedef struct
   {
   char                            obj_type[2];
   name_def                        name;
   } object_identifier_def;


typedef unsigned                   userid_def;

typedef struct
   {
   char                            group_name[8];
   char                            user_name[8];
   } userid_name_def;

typedef struct
   {
   object_number_def               object;
   short                           element;
   } field_def;

typedef short                      null_value_def;
typedef unsigned long              text_id_def;
typedef unsigned long              qualifier_id_def;


typedef struct
   {
   object_number_def               next_obj;
   text_id_def                     next_text_id;
   unsigned short                  version;
   userid_name_def                 creator_userid;
   qualifier_id_def                next_qual_id;
   } DDF_DEF;

typedef object_identifier_def      ODF_ALTKEY;

typedef struct
   {
   object_number_def               object;
   object_identifier_def           identifier;
   unsigned short                  version;
   timestamp_def                   date_created;
   userid_name_def                 creator_userid;
   timestamp_def                   date_modified;
   userid_name_def                 modifier_userid;
   text_id_def                     comment_text;
   } ODF_DEF;

typedef object_number_def          OBL_GENKEY;

typedef struct
   {
   field_def                       identifier;
   short                           level;
   name_def                        local_name;
   text_id_def                     comment_text;
   text_id_def                     value_text;
   text_id_def                     as_text;
   text_id_def                     heading_text;
   text_id_def                     display_text;
   text_id_def                     picture_text;
   text_id_def                     help_text;
   text_id_def                     must_be_text;
   text_id_def                     edit_pic_text;
   char                            tacl_type[2];
   object_number_def               source_def;
   short                           element_redefined;
   short                           obj_class;
   short                           structure;
   short                           element_size;
   short                           scale;
   short                           offset;
   short                           occurs_min;
   short                           occurs_max;
   object_number_def               occurs_min_object;
   object_number_def               occurs_max_object;
   short                           occurs_element;
   short                           starting;
   short                           ending;
   short                           talbound;
   null_value_def                  null_value;
   object_number_def               null_value_object;
   null_value_def                  spi_null_value;
   object_number_def               spi_null_value_object;
   ascii_switch_def                null_value_specified;
   ascii_switch_def                spi_null_value_specified;
   ascii_switch_def                upshift;
   ascii_switch_def                user_defined_filler;
   ascii_switch_def                group_comp;
   ascii_switch_def                source_def_flag;
   ascii_switch_def                novalue;
   ascii_switch_def                tacl_inherited;
   ascii_switch_def                null_inherited;
   ascii_switch_def                spi_null_inherited;
   ascii_switch_def                upshift_inherited;
   ascii_switch_def                usage_is_index;
   long                            bit_length;
   short                           field_align;
   short                           bit_offset;
   object_number_def               enum_def;
   short                           pascalbound;
   name_def                        index_name;
   ascii_switch_def                external_specified;
   ascii_switch_def                justify;
   ascii_switch_def                justify_inherited;
   ascii_switch_def                sqlnullable_specified;
   short                           interval_leading_precision;
   short                           datetime_fraction_precision;
   ascii_switch_def                sqlnullable_flag;
   char                            filler[5];
   } OBL_DEF;
typedef OBL_DEF *POBL_DEF;

//This structure added to support the DDL2 for sol:10-100611-1034
//Changed the filler also

typedef struct
   {
   field_def                       identifier;
   short                           level;
   name_def                        local_name;
   text_id_def                     comment_text;
   text_id_def                     value_text;
   text_id_def                     as_text;
   text_id_def                     heading_text;
   text_id_def                     display_text;
   text_id_def                     picture_text;
   text_id_def                     help_text;
   text_id_def                     must_be_text;
   text_id_def                     edit_pic_text;
   char                            tacl_type[2];
   object_number_def               source_def;
   short                           element_redefined;
   short                           obj_class;
   short                           structure;
   int                             element_size;
   short                           scale;
   int                             offset;
   int                             occurs_min;
   int                             occurs_max;
   object_number_def               occurs_min_object;
   object_number_def               occurs_max_object;
   short                           occurs_element;
   short                           starting;
   short                           ending;
   short                           talbound;
   null_value_def                  null_value;
   object_number_def               null_value_object;
   null_value_def                  spi_null_value;
   object_number_def               spi_null_value_object;
   ascii_switch_def                null_value_specified;
   ascii_switch_def                spi_null_value_specified;
   ascii_switch_def                upshift;
   ascii_switch_def                user_defined_filler;
   ascii_switch_def                group_comp;
   ascii_switch_def                source_def_flag;
   ascii_switch_def                novalue;
   ascii_switch_def                tacl_inherited;
   ascii_switch_def                null_inherited;
   ascii_switch_def                spi_null_inherited;
   ascii_switch_def                upshift_inherited;
   ascii_switch_def                usage_is_index;
   long                            bit_length;
   short                           field_align;
   short                           bit_offset;
   object_number_def               enum_def;
   short                           pascalbound;
   name_def                        index_name;
   ascii_switch_def                external_specified;
   ascii_switch_def                justify;
   ascii_switch_def                justify_inherited;
   ascii_switch_def                sqlnullable_specified;
   short                           interval_leading_precision;
   short                           datetime_fraction_precision;
   ascii_switch_def                sqlnullable_flag;
   char                            filler[2];
   } OBL_DEF_V2;
typedef OBL_DEF_V2 *POBL_DEF_V2;

typedef struct
   {
   otf_identifier_def              identifier;
   object_number_def               constant_id;
   unsigned short                  text_len;
   char                            text_type;
   char                            text_line[132];
   } OTF_DEF;
typedef OTF_DEF *POTF_DEF;


typedef struct
   {
   otf_identifier_def              identifier;
   object_number_def               constant_id;
   char                            constant_type_str[2];
   char                            constant_type_explicit;
   char                            locale_name[16];
   char                            fillbyte;
   object_number_def               ln_constant;
   unsigned short                  text_len;
   char                            text_type;
   char                            text_line[132];
   } OTFD30_DEF;
typedef OTFD30_DEF *POTFD30_DEF;

typedef struct __rdf_v3
{
   object_number_def               object;
   object_number_def               def_number;
   short                           record_length;
   file_name_def                   file_name;
   char                            file_type;
   char                            file_duration;
   unsigned short                  file_code;
   object_number_def               file_code_object;
   short                           primary_extent_size;
   object_number_def               primary_extent_object;
   short                           secondary_extent_size;
   object_number_def               secondary_extent_object;
   ascii_switch_def                refresh;
   ascii_switch_def                audit;
   union
   {
      short                           block_size;
      short                           buffer_size;
   } u_block_size;
   object_number_def               block_size_object;
   object_number_def               buffer_size_object;
   ascii_switch_def                icompress;
   ascii_switch_def                dcompress;
   short                           maxextents;
   object_number_def               maxextents_object;
   char                            buffered;
   ascii_switch_def                audit_compress;
   ascii_switch_def                verified_writes;
   ascii_switch_def                serial_writes;
   ascii_switch_def                odd_unstructured;
} RDF_DEF;


#endif

