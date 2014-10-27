/*
 * @(#)TsmpRequestException.java	1.0 98/03/06
 * 
 *  @@@ START COPYRIGHT @@@
 *
 *             Copyright 2002 Compaq Computer Corporation
 *             Protected as an unpublished work.
 * 
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
 *
 *  @@@ END COPYRIGHT @@@
 * 
 */

package com.tandem.tsmp;

/**
 * This exception indicates that an error has occurred while trying to 
 * convert data for a TsmpServerRequest. 
 *
 * @author  Steve Rispoli
 * @version 1.0, 03/06/98
 * @see     com.tandem.tsmp.TsmpServer
 * @see     com.tandem.tsmp.TsmpServerReply
 * @see     com.tandem.tsmp.TsmpGenericServerReply
 * @see     com.tandem.tsmp.TsmpServerRequest
 * @since   JDK1.1
 */

public
class TsmpRequestException extends TsmpServerException {

    /**
     * Constructs an <code>TsmpRequestException</code> with the specified
     * descriptive message, indicating for which instance variable conversion
     * failed and the type of conversion that was attempted.
     *
     * @param   instanceVariableName the name of the instance variable
     * @param   datatype a number indicating the data type
     * @param   msg the descriptive message
     * @since   JDK1.1
     */

    public TsmpRequestException(String instanceVariableName, int datatype, String msg) {
        super(msg);
        varName = instanceVariableName;
        dataType = datatype;
    }

    /**
     * Constructs an <code>TsmpRequestException</code> with the specified
     * descriptive message, defaulting the instance variable name to null and
     * the data type to 0.
     *
     * @param   msg the descriptive message
     * @since   JDK1.1
     */

    public TsmpRequestException(String msg) {
        super(msg);
        varName = null;
        dataType = 0;
    }

    /**
     * Returns the name of the instance variable for which the conversion failed.
     * @return the instance variable name as a String
     */

    public String getErrorInstance() {
        return varName;
    }

    /**
     * Produce the exception message.
     *
     * @return an error message
     */

    public String getMessage() {
        if (varName == null)
           return super.getMessage();
        else
           return super.getMessage() + " Tsmp conversion type is " + dataTypeToString(dataType) + "; Tsmp instance variable is " + getErrorInstance();
    }

    /**
     * Returns the integer data type as a String.
     *
     * @params dataType the data type reqpresented as an integer.
     * @return a String describing the dataType
     */
    private String dataTypeToString(int dataType) {

         switch (dataType) {
           case  1: return ("ALPHANUMERIC_STRING");
           case  2: return ("NUMERIC_STRING_UNSIGNED");
           case  3: return ("BINARY_16_SIGNED");
           case  4: return ("BINARY_16_UNSIGNED");
           case  5: return ("BINARY_32_SIGNED");
           case  6: return ("BINARY_32_UNSIGNED");
           case  7: return ("BINARY_64_SIGNED");
           case  8: return ("FLOAT_32");
           case  9: return ("FLOAT_64");
           case 10: return ("NUMERIC_STRING_TRAILING_IMBEDDED");
           case 11: return ("NUMERIC_STRING_TRAILING_SEPARATE");
           case 12: return ("NUMERIC_STRING_LEADING_IMBEDDED");
           case 13: return ("NUMERIC_STRING_LEADING_SEPARATE");
           case 14: return ("LOGICAL_1");
           case 15: return ("LOGICAL_2");
           case 16: return ("LOGICAL_4");
           case 17: return ("BINARY_8_SIGNED");
           case 18: return ("BINARY_8_UNSIGNED");
           case 19: return ("BIT_SIGNED");
           case 20: return ("BIT_UNSIGNED");
           default: return("");
         }
    }

    /**
     * varName current instance variable name
     * @since JDK1.1
     */

    private String varName;

    /**
     * dType DDL data type
     * @since JDK1.1
     */

    private int dataType;

}
