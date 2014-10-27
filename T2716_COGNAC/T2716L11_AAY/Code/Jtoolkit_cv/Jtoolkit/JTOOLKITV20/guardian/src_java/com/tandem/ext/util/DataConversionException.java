/* ------------------------------------------------------------------------
* Filename:     DataConversionException.java
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

package com.tandem.ext.util;

/**
 * Signals that a data conversion exception of some sort has occurred.
 *
 * @version 1.0, 8/21/2001
 */
public class DataConversionException extends Exception {

   /**
    * Cause of the conversion error, one of RANGE, SYNTAX or PARAM.
    * @serial
    */
   // made convError transient Ref. Solution# 10-030930-0064
   private transient ConvError convError;

   /* typesafe enum */
   /** Used to indicate the kind of data conversion error */
   public static class ConvError {
      //TBD: when _error is final the compiler complains the first time it tries to
      // compile and then the compile works.
      private /*final*/ String _error;
      private ConvError(String error) {
         _error = error;
      } // end ctor
      /** Returns a string representation of the conversion error. */
      public String toString() {
         return _error;
      }
      /** Indicates conversion of numeric data caused overflow */
      public static final ConvError RANGE = new ConvError("Numeric data out of range");
      /** Indicates an error while parsing a numeric string*/
      public static final ConvError SYNTAX = new ConvError("Syntax error while parsing");
      /** Indicates invalid input parameter to conversion method */
      public static final ConvError PARAM = new ConvError("Invalid input parameter");
   } // end class ConvError

   /**
    * Constructs a DataConversionException with a conversion error.
    * Create a DataConversionException something like this:
    * <p>
    * new DataConversionException(DataConversionException.ConvError.RANGE);
    *
    * @param error the kind of conversion error
    */
   public DataConversionException (ConvError error) {
      super("Data conversion failed with error: " + error);
      convError = error;
   } /* end ctor */
   

   /**
    * Constructs a DataConversionException with a conversion error and a
    * detailed message.
    *
    * @param error the kind of conversion error
    * @param msg the detailed message
    */
   public DataConversionException (ConvError error, String msg) {
      super(msg);
      convError = error;
   } /* end ctor */
   
   /**
    * Gets the kind of conversion error associated with this exception. one of:
    * <ul>
    * <li><code>{@link DataConversionException.ConvError#RANGE}</code>
    * <li><code>{@link DataConversionException.ConvError#SYNTAX}</code>
    * <li><code>{@link DataConversionException.ConvError#PARAM}</code>
    * </ul>
    *
    * @return the kind of conversion error
    */
   public ConvError getConversionError() {
      return convError;
   }

}  /* end class DataConversionException */
