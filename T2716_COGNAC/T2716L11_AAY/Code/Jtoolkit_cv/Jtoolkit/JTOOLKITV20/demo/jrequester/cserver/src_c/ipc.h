/* SCHEMA PRODUCED DATE - TIME : 3/30/2011 - 22:34:24 */
/* SCHEMA PRODUCED DATE - TIME : 3/30/2011 - 22:38:23 */
#pragma section input
/* Definition INPUT created on 03/30/2011 at 22:38 */
#pragma fieldalign shared2 __input
typedef struct __input
{
   char                            firstname[30];
   char                            lastname[30];
   unsigned short                  age;
   short                           empid;
   char                            operation;
} input_def;
#define input_def_Size 65
#pragma section output
/* Definition OUTPUT created on 03/30/2011 at 22:38 */
#include <tnsint.h>
#pragma fieldalign shared2 __output
typedef struct __output
{
   char                            message[255];
   __int32_t                       retcode;
} output_def;
#define output_def_Size 260
