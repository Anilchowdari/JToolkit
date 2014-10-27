/* ------------------------------------------------------------------------
* Filename:     GuardianException.java
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
 * Signals that a Guardian error has occurred.
 * <p>
 * With this exception one can get the Guardian error that occurred
 * as well as the name of the Guardian procedure that generated the error and
 * the name of the file that encountered the error.</p>
 *
 * @version 1.0, 8/21/2001
 * @since NSJ4.2
 */
public class GuardianException extends Exception {
   /**
    * The Guardian error that caused the exception.
    * @serial
    */
   private int _errorNum = 0; /*FEOK*/;
   /**
    * Procedure call which failed.
    * @serial
    */
   private String _proc; 
   /**
    * Name of file that was in error if the error is associated with a filename.
    * This will be an empty string if the error is not associated with a filename.
    * @serial
    */
   private String _fileName; 

   /**
    * Constructs a GuardianException with a file system error number,
    * the procedure which failed and the file it was performed on.  An error 
    * message string will be created based on the parameters passed in.
    *
    * @param error the file system error number
    * @param function the name of the procedure that got the error
    * @param filename the name of the file the I/O was performed on
    */
   public GuardianException (int error, String function, String filename) {
      super("File system error " + error + " using " + function + " on file " + filename);
      _errorNum = error;
      _proc = function;
      _fileName = filename;
   } /* end ctor */

   /**
    * Constructs a GuardianException with a file system error number and
    * the procedure which failed.    An error message string will be created 
    * based on the parameters passed in.  Use this constructor if the error
    * is not associated with a particular file otherwise use {@link 
    * #GuardianException(int, String, String)}.
    *
    * @param error the file system error number
    * @param function the name of the procedure that got the error
    */
   public GuardianException (int error, String function) {
      super("File system error " + error + " using " + function);
      _errorNum = error;
      _proc = function;
      _fileName = "";
   } /* end ctor */

   /**
    * Constructs a GuardianException with an error message string, the file 
    * system error number, the procedure which failed and the file it was 
    * performed on.
    *
    * @param msg the message String returned by getMessage
    * @param error the file system error number
    * @param function the name of the procedure that got the error
    * @param filename the name of the file the I/O was performed on
    */
   public GuardianException (String msg, int error, String function, String fileName) {
      super(msg);
      _errorNum = error;
      _proc = function;
      _fileName = fileName;
   } /* end ctor */

   /**
    * Constructs a GuardianException with an error message string, the file 
    * system error number and the procedure which failed.  Use this constructor
    * if the error is not associated with a particular file otherwise use {@link 
    * #GuardianException(String, int, String, String)}.
    *
    * @param msg the message String returned by getMessage
    * @param error the file system error number
    * @param function the name of the procedure that got the error
    */
   public GuardianException (String msg, int error, String function) {
      super(msg);
      _errorNum = error;
      _proc = function;
      _fileName = "";
   } /* end ctor */

   /**
    * Gets the file system error number associated with this exception.
    *
    * @return file system error number
    */
   public int getErrorNum() {
      return _errorNum;
   }

// This might be a future enhancement because there's no easy way to get
// this.  Guardian Programmers Guide suggests starting an ERROR process and
// doing IPC to get the error text (ugh!).  Would be tedious but easier to 
// create giant switch statement or perhaps enhance GError to have
// corresponding text for each error.  Tuxedo might have a file that has error
// text for each error.
   /*
    * Gets the text description of the file system error number.
    *
    * @return textual description of error number
    */
//   public String getErrorText() {
//      return _errorText;
//   }

   /**
    * Gets the name of the procedure associated with this exception.
    *
    * @return name of the procedure which failed
    */
   public String getProcName() {
      return _proc;
   }

   /**
    * Gets the name of the file that encountered this exception.  This will be
    * an empty string if the error is not associated with a particular file.
    *
    * @return file name in error
    */
   public String getFileName() {
      return _fileName;
   }

}  /* end class GuardianException */
