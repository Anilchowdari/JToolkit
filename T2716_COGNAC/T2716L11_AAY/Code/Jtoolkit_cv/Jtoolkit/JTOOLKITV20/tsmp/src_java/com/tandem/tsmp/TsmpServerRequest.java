/*
 * @(#)TsmpServerRequest.java	1.0 98/03/06
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

import com.tandem.ext.util.NativeLoader;


/**
 * Abstract class for a TS/MP request.
 * <p>
 * ddl2java generates classes which subclass <CODE>TsmpServerRequest</CODE>.
 * A <CODE>TsmpServerRequest</CODE> is passed by the programmer as an argument 
 * to the <CODE>TsmpServer</CODE> service method. 
 *
 * @version 1.0
 * @author Steve Rispoli
 * @since JDK1.0
 * @see com.tandem.tsmp.TsmpServer
 * @see com.tandem.tsmp.TsmpServerReply
 * @see com.tandem.tsmp.TsmpGenericServerReply
 */

public abstract class TsmpServerRequest extends Object {

    /* load library of native methods */
    static { 	  
    		NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();		
	 }

    /**
     * Allocates a new TsmpServerRequest.
     */
    
    public TsmpServerRequest() { };

    /**
     * The byte array in which the request data is loaded.
     */
    protected byte _requestBuffer[];

    /**
     * The current offset into the byte array.
     */

    protected int _offset;

    /**
     * Loads converted request data from instance variables into a byte array.  
     *
     * @exception TsmpRequestException if an exception occurs during data
     * conversion.
     */

    public abstract void marshal() throws TsmpRequestException;

    /**
     * Sets the request buffer
     *
     * @param buffer the byte array used to initialize this buffer
     * 
     */

    public void setBuffer(byte buffer[]) {
        _requestBuffer = buffer;
    };

    /**
     * Returns the request buffer
     * 
     * @return the request buffer
     */

    protected byte[] getBuffer() {
        return _requestBuffer;
    };

    /**
     * Returns a new String object representing the value of the 
     * <CODE>TsmpServerRequest</CODE> 
     * or "null byte array" if the request buffer is empty.
     */

    public String toString() {
        String s;
             
        if (_requestBuffer == null ) {
           s = new String("null byte array");        
        } else {
           s = new String(_requestBuffer);
        }
        return (s);
    }

    /**
     * Converts a blank-padded string to a null-terminated string and places 
     * the contents into the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     * 
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaStrToCStr ( String inString, int offset, int fieldSize );

    /**
     * Converts a blank-padded string to a non null-terminated string 
     * and places the contents into the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     * 
     * @return 0 on success, 3 on failure
     */


    protected final native int JavaStrToCobolStr ( String inString, int offset, int fieldSize );

    /**
     * Converts an optionally signed decimal numeric string to a decimal
     * numeric string, with a specified format and scale. The converted
     * string is placed in the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     * @param nOutScale the scale to apply to the converted string
     * @param nOutType the format to apply to the converted string
     * 
     * @return 0 on success, non-zero on failure on failure
     */

    protected final native int JavaStrToStr16
    ( String inString, int offset, int fieldSize );

    /**
     * Puts a Java string into the request buffer as a double-byte
     * character string.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     * @param fieldSize the length of the field in the request buffer
     *
     * @return CVERR_OK on success, CVERR_PARAM on failure
     */ 

    protected final native int JavaStrToNumStr ( String inString, int offset, int fieldSize, int nOutScale, int nOutType );

    /**
     * Converts a one byte Java String to a one byte ASCII string 
     * and place it in the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaStrToBIN8 ( String inString, int offset );

    /**
     * Converts a one byte Java string to a 2 byte ASCII string for COBOL.
     * The contents of the string is stored in the first element and a blank
     * character is stored in the second. The converted value is placed in 
     * the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaStrToBIN8Cobol ( String inString, int offset );

    /**
     * Converts a Java short into an unsigned short and places the result
     * in the request buffer.
     *
     * @param shortValue the short value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaShortToBIN16 ( short shortValue, int offset);
    
    /**
     * Converts a Java integer into an unsigned short and places the result
     * in the request buffer.
     *
     * @param intValue the integer value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaIntToUNBIN16 ( int intValue, int offset);

    /**
     * Converts a Java int into an unsigned integer and places the result
     * in the request buffer.
     *
     * @param intValue the integer value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaIntToBIN32 ( int intValue, int offset );

    /**
     * Converts a Java long into an unsigned integer and places the result
     * in the request buffer.
     *
     * @param longValue the long value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaLongToUNBIN32 ( long longValue, int offset );

    /**
     * Converts a Java long into a 64 bit long long value and places the result
     * in the request buffer.
     *
     * @param longlongValue the long value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaLongToBIN64 ( long longlongValue, int offset );

    /**
     * Converts a one byte Java String to a one byte ASCII string 
     * and place it in the request buffer.
     *
     * @param inString a java String
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaStrToLogical1 ( String inString, int offset );

    /**
     * Converts a Java short into a twp byte ASCII string and places the result
     * in the request buffer.
     *
     * @param shortValue the short value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */
    protected final native int JavaShortToLogical2 ( short shortValue, int offset);

    /**
     * Converts a Java long into a four bytye ASCII string and places the 
     * result in the request buffer.
     *
     * @param longValue the short value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int JavaLongToLogical4 ( long longValue, int offset);

    /**
     * Converts a Java String representing a Tandem floating point
     * value into a Tandem float and places the result
     * in the request buffer.
     *
     * @param inString the String value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int StrToFloat ( String inString, int offset );

    /**
     * Converts a Java String representing a Tandem double 
     * value into a Tandem double and places the result
     * into the request buffer.
     *
     * @param inString the String value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int StrToDouble ( String inString, int offset );

    /**
     * Converts a signed Java bit string into a short and places the result
     * in the request buffer.
     *
     * @param value the value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int BITtoTdmShort ( int value, int offset );

    /**
     * Converts an unsigned Java bit string into a short and places the result
     * in the request buffer.
     *
     * @param value the value to convert
     * @param offset the location within the request buffer to place the
     * converted string
     *
     * @return 0 on success, 3 on failure
     */

    protected final native int UBITtoTdmShort ( int value, int offset );
}
