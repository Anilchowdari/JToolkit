/*
 * @(#)TsmpServerReply.java	1.0 98/03/06
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
 * Abstract class for a Tsmp reply.
 * <p>
 * <CODE>ddl2java</CODE> generates classes which subclass <CODE>TsmpServerReply</CODE>.
 * A <CODE>TsmpServerReply</CODE> is passed by the programmer as an argument 
 * to the <CODE>TsmpServer</CODE> <CODE>service</CODE> method. 
 *
 * @version 1.0
 * @author Steve Rispoli
 * @since JDK1.1
 * @see com.tandem.tsmp.TsmpServer
 * @see com.tandem.tsmp.TsmpServerRequest
 * @see com.tandem.tsmp.TsmpGenericServerReply
 */

public abstract class TsmpServerReply extends Object {

    /* load library of native methods */
    static { 	  
    
    		NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();	
	 }

    /**
     * Allocates a new TsmpServerReply.
     */
     
    protected TsmpServerReply() { }

    /**
     * The byte array containing the reply data.
     */
   
    protected byte _replyBuffer[];

    /**
     * The current offset into the byte array.
     */

    protected int _offset;

    /**
     * The maximum expected length of the reply
     */

    protected int _replyLength;

    /**
     * The actual length of the reply
     */

    protected int _actualReplyLength;

    /**
     * Indicates a alphanumeric string data type.
     */

    static final int ALPHANUMERIC_STRING = 1;
    
    /**
     * Indicates a numeric unsigned string data type 
     */ 

    static final int NUMERIC_STRING_UNSIGNED = 2;

    /**
     * Indicates a BINARY 16 SIGNED data type.
     */

    public static final int BINARY_16_SIGNED = 3;

    /**
     * Indicates a BINARY 16 UNSIGNED data type.
     */

    public static final int BINARY_16_UNSIGNED = 4;

    /**
     * Indicates a BINARY 32 SIGNED data type.
     */

    public static final int BINARY_32_SIGNED = 5;

    /**
     * Indicates a BINARY 32 UNSIGNED data type.
     */

    public static final int BINARY_32_UNSIGNED = 6;

    /**
     * Indicates a BINARY 64 SIGNED data type.
     */

    public static final int BINARY_64_SIGNED = 7;

    /**
     * Indicates a FLOAT 32 data type.
     */

    static final int FLOAT_32 = 8;

    /**
     * Indicates a FLOAT 64 data type.
     */

    static final int FLOAT_64 = 9;

    /**
     * Indicates a numeric string trailing embedded sign data type.
     * See the Data Definition Language Reference manual for
     * more information about numeric strings with embedded
     * trailing signs.
     */
    static final int NUMSTR_TRAILING_EMBEDDED = 15;
    /**
     * Indicates a numeric string trailing separate sign data type.
     * See the Data Definition Language Reference manual for
     * more information about numeric strings with separate
     * trailing signs.
     */
    static final int NUMSTR_TRAILING_SEPARATE = 13;
    /**
     * Indicates a numeric string leading embedded sign data type.
     * See the Data Definition Language Reference manual for
     * more information about numeric strings with embedded
     * leading signs.
     */
    static final int NUMSTR_LEADING_EMBEDDED = 14;
    /**
     * Indicates a numeric string leading separate sign data type.
     * See the Data Definition Language Reference manual for
     * more information about numeric strings with separate
     * leading signs.
     */
    static final int NUMSTR_LEADING_SEPARATE = 12;
    /**
     * Indicates an unsigned numeric string data type.
     * See the Data Definition Language Reference manual for
     * more information about unsigned numeric strings.
     */
    /**
     * Indicates an unsigned numeric string data type.
     * See the Data Definition Language Reference manual for
     * more information about unsigned numeric strings.
     */
    static final int NUMSTR_UNSIGNED = 16;

    /**
     * Indicates a LOGICAL 1 data type.
     */

    static final int LOGICAL_1 = 14;

    /**
     * Indicates a LOGICAL 2 data type.
     */

    static final int LOGICAL_2 = 15;

    /**
     * Indicates a LOGICAL 4 data type.
     */

    static final int LOGICAL_4 = 16;

    /**
     * Indicates a BINARY 8 SIGNED data type.
     */

    public static final int BINARY_8_SIGNED = 17;

    /**
     * Indicates a BINARY 8 UNSIGNED data type.
     */

    public static final int BINARY_8_UNSIGNED = 18;

    /**
     * Indicates a BIT SIGNED data type.
     */

    static final int BIT_SIGNED = 19;

    /**
     * Indicates a BIT UNSIGNED data type.
     */

    static final int BIT_UNSIGNED = 20;

    /**
     * Moves and converts the data in the reply buffer into the appropriate
     * instance variables.
     * @exception TsmpReplyException if there is an converting the reply data
     */

    protected abstract void unmarshal() throws TsmpReplyException;

    /**
     * Copies the given byte array into the reply buffer 
     * 
     * @param buffer the byte array to copy into the reply buffer
     */

    protected void setBuffer(byte[] buffer) {
        _replyBuffer = buffer;
    }

    /**
     * Sets the maximum expected reply length.
     * 
     * @param replyLength the maximum expected length of the reply.
     */

    protected void setReplyLength(int replyLength) {
        _replyLength = replyLength;
    }

    /**
     * Sets the actual reply length.
     *
     * @param actualReplyLength the actual length of the reply.
     */

    void setActualReplyLength(int actualReplyLength) {
        _actualReplyLength = actualReplyLength;
    }

    /**
     * Returns the reply buffer
     * 
     * @return the reply buffer
     */

    public byte[] getBuffer() {
        return _replyBuffer;
    }

    /**
     * Gets the maximum expected reply length.
     * 
     * @return the maximum expected length of the reply.
     */

    public int getReplyLength() {
        return _replyLength;
    }

    /**
     * Gets the actual reply length.
     *
     * @return the  actual length of the reply.
     */

    public int getActualReplyLength() {
        return _actualReplyLength;
    }

    /**
     * Returns a new String object representing the value of the byte array 
     * or "null byte array" if the byte array is empty.
     *
     * @return the contents of the reply buffer as a string
     */

    public String toString() {
        String s;
             
        if (_replyBuffer == null ) {
           s = new String("null byte array");        
        } else {
           s = new String(_replyBuffer);
        }
        return (s);
    }

    /**
     * Convert a fixed-length, decimal numeric string with a specified
     * format and scale to an optionally signed decimal numeric string.
     *
     * @param offset the location in the reply buffer to extract the value
     * @param fieldSize the length of the value to extract
     * @param nInScale the scale to apply to the converted value 
     * @param nInTyp the type to apply to the converted value
     *
     * @return the converted String
     */
    protected final native String NumStrToJavaStr ( int offset, int fieldSize, int nInScale, int nInType );

    /**
     * Converts a string to a Java String
     *
     * @param offset the location in the reply buffer to extract the value
     * @param fieldSize the length of the value to extract
     *
     * @return the converted String
     */

    protected final native String StrToJavaStr ( int offset, int fieldSize );

    /**
     * Converts a character into a Java String
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     */

    protected final native String StrToJavaStr16 ( int offset, int fieldSize );

    /**
     * Puts a string of double-byte characters into a Java String
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     */ 

    protected final native String BIN8ToJavaStr ( int offset );

    /** 
     * Converts a signed short into a Java short
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted short
     */

    protected final native short BIN16ToJavaShort ( int offset );

    /** 
     * Converts a unsigned short into a Java int  
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted int
     */

    protected final native int UNBIN16ToJavaInt ( int offset );

    /** 
     * Converts a signed integer into a Java int 
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted int 
     */

    protected final native int BIN32ToJavaInt ( int offset );

    /** 
     * Converts a unsigned integer into a Java long
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted long
     */

    protected final native long UNBIN32ToJavaLong ( int offset );

    /** 
     * Converts a 64 bit long long into a Java long
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted long
     */

    protected final native long BIN64ToJavaLong ( int offset );

    /** 
     * Converts a Tandem float into a Java String representing a 
     * Tandem float.
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     */

    protected final native String Float32ToStr ( int offset );

    /** 
     * Converts a Tandem double into a Java String representing 
     * Tandem double.
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     */

    protected final native String Float64ToStr ( int offset );

    /** 
     * Converts a signed character into a Java String
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     */

    protected final native String Logical1ToJavaStr ( int offset );

    /** 
     * Converts a unsigned character string of two bytes into a Java int
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted int
     */

    protected final native int Logical2ToJavaInt ( int offset );

    /** 
     * Converts a unsigned character string of four bytes into a Java long
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted long
     */

    protected final native long Logical4ToJavaLong ( int offset );

    /** 
     * Converts a unsigned char into a Java int
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted int
     */
    
    protected final native int TdmShortToBIT ( int offset );

    /** 
     * Converts a unsigned char into a Java int
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted int
     */

    protected final native int TdmShortToUBIT ( int offset );

}
