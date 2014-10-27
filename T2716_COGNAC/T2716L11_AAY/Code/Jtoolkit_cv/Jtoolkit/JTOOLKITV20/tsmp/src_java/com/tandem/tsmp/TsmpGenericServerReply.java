/*
 * @(#)TsmpGenericServerReply.java	1.0 98/03/06
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
 * Final class for a TsmpGenericServerReply.  A TsmpGenericServerReply
 * is used to get an arbitrary reply from a legacy server.  Methods are
 * supplied to interogate the reply code from the server.  Based on the
 * reply codes value, reply classes subclassed from TsmpServerReply can
 * then be used to unload the reply data into the appropriate instance
 * variables of the given class.
 *
 * @version 1.0
 * @author Steve Rispoli
 * @since JDK1.1
 * @see com.tandem.tsmp.TsmpServer
 * @see com.tandem.tsmp.TsmpServerReply
 * @see com.tandem.tsmp.TsmpServerRequest
 */

public final class TsmpGenericServerReply extends TsmpServerReply {

    /**
     * Allocates a new TsmpGenericServerReply with a default value of 32000
     * for the maximum expected server reply length.
     */
     
    public TsmpGenericServerReply() { 
        this._replyLength = MAXREPLYLENGTH;
    };

    /**
     * Allocates a new TsmpGenericServerReply with the user specifying the 
     * maximum expected length of the server's reply.
     *
     * @param maxReplyLength the maximum expected length of the server's 
     * reply.  Value should be between 1 and 32000, inclusive, or the Pathsend
     * will fail.
     */
     
    public TsmpGenericServerReply(int maxReplyLength) {
        this._replyLength = maxReplyLength;
    }

    /**
     * MAXREPLYLENGTH is the maximum reply length for a Pathmon serverclass.
     */
   
    static final int MAXREPLYLENGTH = 32000;

    /**
     * A generic TsmpServerReply does not unload the byte array, so this 
     * method does nothing.
     */

    final protected synchronized void unmarshal() { };

    /**
     * Returns the maximum allowed value for the reply length.
     * @return the maximum allowed reply length 
     */

    public final int getReplyLength() {
        return this._replyLength;
    };

    /**
     * Returns an expected reply code as a String.
     * 
     * @param offset the starting location within the reply buffer to obtain 
     * the String
     * @param length the size of the String to return
     * @return the reply code as a String
     */

    public final String getString(int offset, int length) {

        String sret = new String(StrToJavaStr( offset, length ));

        return sret;
    };
    
    /**
     * Returns an expected signed short reply code as a short.
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @return the reply code as a short
     */

    public final short getSignedShort(int offset) {
        return (BIN16ToJavaShort( offset ));
    };

    /**
     * Returns an expected unsigned short reply code as an int.
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @return the reply code as an integer
     */

    public final int getUnsignedShort(int offset) {
        return UNBIN16ToJavaInt( offset );
    };

    /**
     * Returns an expected signed integer reply code as an int.
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @return the reply code as an integer
     */

    public final int getSignedInt(int offset) {
        return BIN32ToJavaInt( offset );
    };

    /**
     * Returns an expected unsigned integer reply code as a long.
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @return the reply code as a long
     */

    public final long getUnsignedInt(int offset) {
        return UNBIN32ToJavaLong( offset );
    };

    /**
     * Returns an expected reply code as a long.
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @return the reply code as a long
     */

    public final long getLong(int offset) {
        return BIN64ToJavaLong( offset );
    };
    
    /**
     * Returns an expected reply code as a binary string.  The allowed
     * data types are:
     * <ul>
     * <li> BINARY_8_SIGNED
     * <li> BINARY_8_UNSIGNED
     * <li> BINARY_16_SIGNED
     * <li> BINARY_16_UNSIGNED
     * <li> BINARY_32_SIGNED
     * <li> BINARY_32_UNSIGNED
     * <li> BINARY_64_SIGNED
     * </ul>
     * 
     * @param offset the starting location within the reply buffer to obtain the value
     * @param dataType the type of data to convert
     * @return the binary data value as a String
     * @exception TsmpReplyException if the data type cannot be converted 
     */

    public final String getBinary(int offset, int dataType) throws TsmpReplyException {

        Short shret;
        Integer iret;
        Long lret;
        String sret = " ";

        switch (dataType) {
          case BINARY_16_SIGNED:    // = 3
               shret = new Short(BIN16ToJavaShort( offset ));
               sret = shret.toString();
               break;
          case BINARY_16_UNSIGNED:  // = 4
               iret = new Integer(UNBIN16ToJavaInt( offset ));
               sret = iret.toString();
               break;
          case BINARY_32_SIGNED:    // = 5
               iret = new Integer(BIN32ToJavaInt( offset ));
               sret = iret.toString();
               break;
          case BINARY_32_UNSIGNED:  // = 6
               lret = new Long(UNBIN32ToJavaLong( offset ));
               sret = lret.toString();
               break;
          case BINARY_64_SIGNED:    // = 7
               lret = new Long(BIN64ToJavaLong( offset ));
               sret = lret.toString();
               break;
          case BINARY_8_SIGNED:     // = 17
          case BINARY_8_UNSIGNED:   // = 18
               sret = BIN8ToJavaStr( offset );
               break;
          default:
            throw new TsmpReplyException("Unsupported data type " + dataType);
        }
        return sret;
    }
}
