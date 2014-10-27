/* -------------------------------------------------------------------------
*
* You are granted a limited copyright to modify and use this sample
* code for your internal purposes only. THIS SOFTWARE IS PROVIDED "AS-
* IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. IN NO EVENT SHALL THE
* HEWLETT-PACKARD COMPANY OR ITS SUBSIDIARIES BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE INCURRED BY YOU OR ANY THIRD PARTY IN CONNECTION WITH THE
* USE OF THIS SOFTWARE.
*
* -------------------------------------------------------------------------
*/                                                                            


/* SCHEMA PRODUCED DATE - TIME : 5/21/98 13:16:37 */
#pragma section employee_def
/* Definition EMPLOYEE-DEF version 2 updated on 05/21/98 at 12:00 */
#pragma fieldalign shared2 __employee_def
typedef struct __employee_def
{
   char                            employee_number[4];
   struct
   {
      char                            first[20];
      char                            last[30];
      char                            middle;
   } empname;
   char                            regnum[2];
   char                            branchnum[2];
} employee_def_def;
#pragma section employee_add
/* Definition EMPLOYEE-ADD created on 05/21/98 at 12:00 */
#pragma fieldalign shared2 __employee_add
typedef struct __employee_add
{
   char                            request_code[2];
   struct
   {
      char                            employee_number[4];
      struct
      {
         char                            first[20];
         char                            last[30];
         char                            middle;
      } empname;
      char                            regnum[2];
      char                            branchnum[2];
   } employee_info;
} employee_add_def;
#pragma section employee_request
/* Definition EMPLOYEE-REQUEST version 3 updated on 05/21/98 at 12:00 */
#pragma fieldalign shared2 __employee_request
typedef struct __employee_request
{
   char                            request_code[2];
   char                            employee_number[4];
} employee_request_def;
#pragma section employee_reply
/* Definition EMPLOYEE-REPLY created on 05/21/98 at 12:00 */
#pragma fieldalign shared2 __employee_reply
typedef struct __employee_reply
{
   char                            reply_code[2];
   struct
   {
      char                            employee_number[4];
      struct
      {
         char                            first[20];
         char                            last[30];
         char                            middle;
      } empname;
      char                            regnum[2];
      char                            branchnum[2];
   } employee_info;
} employee_reply_def;
#pragma section employee
/* Record EMPLOYEE created on 05/21/98 at 12:00 */
#pragma fieldalign shared2 __employee
typedef struct __employee
{
   struct
   {
      char                            employee_number[4];
      struct
      {
         char                            first[20];
         char                            last[30];
         char                            middle;
      } empname;
      char                            regnum[2];
      char                            branchnum[2];
   } employ;
} employee_def;
/* SCHEMA PRODUCED DATE - TIME : 5/21/98 15:07:12 */
#pragma section error_reply
/* Definition ERROR-REPLY created on 04/03/98 at 14:31 */
#pragma fieldalign shared2 __error_reply
typedef struct __error_reply
{
   char                            reply_code[2];
   char                            error_text[60];
} error_reply_def;
