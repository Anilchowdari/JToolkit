/* ------------------------------------------------------------------------
* Filename:     GuardianWarning.java
*
*  @@@ START COPYRIGHT @@@
*
*   Copyright 2001
*   Compaq Computer Corporation
*      Protected as an unpublished work.
*         All rights reserved.
*
*   The computer program listings, specifications, and documentation
*   herein are the property of Compaq Computer Corporation or a
*   third party supplier and shall not be reproduced, copied,
*   disclosed, or used in whole or in part for any reason without
*   the prior express written permission of Compaq Computer
*   Corporation.
*
*  @@@ END COPYRIGHT @@@
*
* ------------------------------------------------------------------------ */

package com.tandem.ext.guardian;

/**
 * An exception that provides information on Guardian I/O warnings. 
 * The meaning of a particular warning number depends on the Guardian procedure
 * called.  Sometimes warnings indicate the I/O request was not performed whereas
 * at other times the request was successfully performed.  See the Guardian
 * Procedures Calls Reference Manual and the Guardian Procedure Errors and
 * Messages Manual for more information.
 * <p>
 * With this exception one can get the Guardian file system warning that occurred
 * as well as the name of the Guardian procedure that generated the warning and
 * the name of the file that encountered the warning.</p>
 *
 * @version 1.0, 10/19/2001
 * @since NSJ4.2
 */
public class GuardianWarning extends GuardianException {

   /**
    * Constructs a GuardianWarning with a file system warning number,
    * the procedure which got the warning and the file it was performed on.  An error
    * message string will be created based on the parameters passed in.
    *
    * @param warning the file system warning number
    * @param function the name of the procedure that got the warning
    * @param filename the name of the file the I/O was performed on
    */
   public GuardianWarning (int warning, String function, String filename) {
      super(warning, function, filename);
   } /* end ctor */
   
   /**
    * Constructs a GuardianWarning with a file system warning number and
    * the procedure which got the warning.  An error message string will be created based on the 
    * parameters passed in.  Use this constructor if the warning is not associated
    * with a particular file otherwise use {@link #GuardianWarning(int,String,String)}.
    *
    * @param warning the file system warning number
    * @param function the name of the procedure that got the warning
    */
   public GuardianWarning (int warning, String function) {
      super(warning, function);
   } /* end ctor */

   /**
    * Constructs a GuardianWarning with an error message string, the file 
    * system warning number, the procedure which got the warning and the file
    * it was performed on.
    *
    * @param msg the message String returned by {@link #getMessage}
    * @param warning the file system warning number
    * @param function the name of the procedure that got the warning
    * @param filename the name of the file the I/O was performed on
    */
   public GuardianWarning (String msg, int warning, String function, String filename) {
      super(msg, warning, function, filename);
   } /* end ctor */

   /**
    * Constructs a GuardianWarning with an error message string, the file 
    * system warning number and the procedure which got the warning.  Use this
    *  constructor if the warning is not associated
    * with a particular file otherwise use {@link #GuardianWarning(String, int,String,String)}.
    *
    * @param msg the message String returned by getMessage
    * @param warning the file system warning number
    * @param function the name of the procedure that got the warning
    */
   public GuardianWarning (String msg, int warning, String function) {
      super(msg, warning, function);
   } /* end ctor */

}  /* end class GuardianWarning */
