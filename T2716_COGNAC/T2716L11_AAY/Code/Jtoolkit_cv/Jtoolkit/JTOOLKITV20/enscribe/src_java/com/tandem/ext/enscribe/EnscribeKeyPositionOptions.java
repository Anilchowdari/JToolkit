/*
 * Copyright 2000 Compaq Computer Corporation
 *           Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
*/

package com.tandem.ext.enscribe;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

/**
 * <P>EnscribeKeyPositionOptions handles attributes used by
 * the EnscribeFile.keyPosition method.
 * Refer to the description of the FILE_SETKEY_ procedure
 * (documented in the Guardian Procedure Calls Reference Manual) for more information
 * about key positioning attributes and their values.</P>
 *
 * @see EnscribeFile
 */

 public class EnscribeKeyPositionOptions
 {

    private Vector keyValue_ = new Vector(32,32);

    private String keySpecifier_;
	private short  keySpecifierNumeric_ = 0; // primary key
	private boolean keySpecifierIsNumber_ = true;

    private int compareLength_ = 0;
    private short keyLength_ = -1;
    private int mode_ = POSITION_APPROXIMATE;

    private boolean positionToNextRecord_ = false;
    private boolean readReverse_ = false;
    private boolean positionToLastRecord_ = false;
    private boolean keyValueIsString = false;

   /**
    * Position Approximate - Position to the first key field that contains a value equal
	* to or greater than the key value for the length of the key in bytes.
    */
    public static final int POSITION_APPROXIMATE = 0;

   /**
    * Position Generic - Positioning starts by going to the first record whose key field contains a value
    * equal to or greater than the key value for the length of the key in bytes,
    * then positions to the record
    * whose key field does not start with a value equal to the key value for
    * compare length bytes.
    */
    public static final int POSITION_GENERIC = 1;

   /**
    * Position Exact - Position to the first record whose key field contains the key value
	* exactly compare length bytes in length.
    */
    public static final int POSITION_EXACT = 2;

   /**
    * Construct an EnscribeKeyPositionOptions object.
    */
    public EnscribeKeyPositionOptions() {
    } // EnscribeKeyPositionOptions

   /**
    * Sets the value of the primary or alternate key which is to be used to position a
	* file. Corresponds to the key-value parameter of FILE_SETKEY_.
    *
    * @param keyValue byte array of the value of the key used for positioning.
    *
    * @see #getKeyValue
    */
    public void setKeyValue(byte[] keyValue) {
        int keylen = keyValue.length;
		int i;
		keyValue_.removeAllElements();
		for(i=0;i<keylen;i++)
		{
			keyValue_.addElement(new Integer(keyValue[i]));
		}
		keyValueIsString = false;
    } // setKeyValue
    
    //Begin Solution No.10-060510-6371
    /**
     * Sets the value of the primary or alternate key which is to be used to 
     * position a file; corresponds to the key-value parameter of FILE_SETKEY_.
     *
     * @param keyValue the value of the key used for positioning.
     * 
     * @param encodeSetting the encoding of the passed keyValue String.
     * 
     * @throws UnsupportedEncodingException
     * 
     * @see #getKeyValue
     */
     public void setKeyValue(String keyValue,String encodeSetting) 
         throws UnsupportedEncodingException {
    	 if (encodeSetting.equals(null))
    	 {
    		 encodeSetting=System.getProperty("file.encoding");
    	 }
         setKeyValue(keyValue.getBytes(encodeSetting));
    } // setStringKeyValue
    //End Solution No.10-060510-6371 

   /**
    * Sets the value of the primary or alternate key which is to be used to position a
	* file; corresponds to the key-value parameter of FILE_SETKEY_.
    *
    * @param keyValue the value of the key used for positioning.
    *
    * @see #getKeyValue
    */
    public void setKeyValue(String keyValue) {
		if(keyValue == null)
			throw new IllegalArgumentException("keyValue cannot be null");
        char [] caKeyValue = keyValue.toCharArray();
		int keylen = caKeyValue.length;
		int i;
		keyValue_.removeAllElements();
		for(i=0;i<keylen;i++)
		{
			keyValue_.addElement(new Integer(caKeyValue[i]));
		}
		keyValueIsString = true;
    } // setStringKeyValue

   /**
    * Returns the current setting of the value of the primary or alternate key which is
	* to be used to position a file.
	*
    * @return byte array of the value of the primary or alternate key
	*
	* @see #setKeyValue
    */
    public byte[] getNumericKeyValue() {
		byte [] retval = new byte [keyValue_.size()];
		int i;
		Integer bVal;
		for(i=0;i<keyValue_.size();i++)
		{
			bVal = (Integer)keyValue_.elementAt(i);
			retval[i] = (byte)bVal.intValue();
		}
        return retval;
    } // getKeyValue

   /**
    * Returns the current setting of the value of the primary or alternate key which is
	* to be used to position a file.
	*
    * @return String with the value of the primary or alternate key
	*
	* @see #setKeyValue
    */
    public String getKeyValue() {
		byte [] byteVal = getNumericKeyValue();
		String str = new String(byteVal);
        return str;
    } // getStringKeyValue


   /**
    * Sets the key specifier which identifies the key field to be used to position a file.
    * Corresponds to the key-specifier parameter of FILE_SETKEY_.
    *
    * @param keySpecifier identifier of the key used for positioning.
    * A null string indicates primary key
    *
    * @see #getStringKeySpecifier
    * @see #getNumericKeySpecifier
    */
    public void setKeySpecifier(String keySpecifier) {
        if(keySpecifier == null)
		{
			keySpecifierIsNumber_ = true;
			keySpecifierNumeric_ = 0;
		}
		else
		{
			keySpecifierIsNumber_ = false;
			keySpecifier_ = keySpecifier;
		}
    } // setKeySpecifier

   /**
    * Returns the current setting of the key specifier.
	* If the key specifier is numeric, will return null.
	*
    * @return String with the two byte key specifier
	*
	* @see #setKeySpecifier
	* @see #isKeySpecifierNumeric
    */
    public String getStringKeySpecifier() {
        if(keySpecifierIsNumber_)
			return null;
		else
			return keySpecifier_;
    } // getStringKeySpecifier

   /**
    * Sets the key specifier which identifies the key field to be used to position a file.
    * Corresponds to the key-specifier parameter of FILE_SETKEY_.
    *
    * @param keySpecifier key used for positioning.
    * A 0 indicates primary key
    *
    * @see #setKeySpecifier
    * @see #getNumericKeySpecifier
    * @see #getStringKeySpecifier
    */
    public void setKeySpecifier(short keySpecifier) {
		keySpecifierIsNumber_ = true;
		keySpecifierNumeric_ = keySpecifier;
    } // setKeySpecifierNumeric

   /**
    * Returns the current numeric setting of the key specifier.
    * If key specifier is not numeric, will return -1.
    *
    * @return short key specifier
    *
    * @see #getStringKeySpecifier
    * @see #isKeySpecifierNumeric
    */
    public short getNumericKeySpecifier() {
        return keySpecifierNumeric_;
    } // getKeySpecifierNumeric

   /**
    * Returns true if key specifier has been set to a numeric value.
    *
    * @return true if key specifier has been set to a numeric value
    *
    * @see #getNumericKeySpecifier
    */
    public boolean isKeySpecifierNumeric() {
        return keySpecifierIsNumber_;
    } // isKeySpecifierNumeric
  /**
    * Returns true if key specifier has been set to a String value.
    *
    * @return true if key specifier has been set to a String value
    *
    * @see #getStringKeySpecifier
    */
    public boolean isKeySpecifierString() {
		if(keySpecifierIsNumber_)
			return false;
		else
			return true;
    } // isKeySpecifierString


   /**
    * Sets the length in bytes to use for key comparisons.
	* Corresponds to the comparelength parameter of FILE_SETKEY_ described in
	* the Guardian Procedure Calls Manual.
	*
    * @param compareLength the length in bytes to use for key comparisons.
    *
    * @see #getCompareLength
    */
    public void setCompareLength(int compareLength) {
		compareLength_ = compareLength;
    } // setCompareLength

   /**
    * Returns the current setting of compare length.
    *
    * @return compare length
    *
    * @see #setCompareLength
    */
    public int getCompareLength() {
        return compareLength_;
    } // getCompareLength

   /**
    * Specifies number of bytes of the key value to use to
    * establish the initial position. Corresponds to the right byte of the length-word
    * parameter of FILE_SETKEY_.  To indicate that the positioning methods should
    * use the key's defined length, specify -1 (the default)
    *
    * @param keyLength number of bytes in key value to be used for the starting
	* position or -1 to indicate that the key's defined length should be used.
	*
	* @see #getKeyLength
    */
    public void setKeyLength(short keyLength) {
        keyLength_ = keyLength;
    } // setKeyLength

   /**
    * Returns the current setting of the key length.
    *
    * @return  key length
    *
    * @see #setKeyLength
    */
    public short getKeyLength() {
        return keyLength_;
    } // getKeyLength

   /**
    * Indicates whether positioning should be to the first record with
    * exactly the specified key-length (specified by setKeyLength()) and key-value
    * (specified by setKeyValue()) is found.  If you specify true, the
    * record is skipped; if you specify false (the default) the record is not
    * skipped.
    *
    * If the keyspecifier (specified by setKeySpecifier() ) indicates a
    * non-unique alternate key and you specify true,  the record is skipped
    * only if both its alternate key and its primary key match the
    * corresponding portions of the specified key-value (which should be an
    * alternate key value concatenated with a primary key value)
    * for key-length bytes  (which should be the sum of the alternate and
    * primary key lengths).
    *
    * This method should not be used for positioning by primary key in relative
	* or entry-sequenced files.
    * @param value true to indicate that the first record is to be skipped; false to position to the
    * next record.
    * Corresponds to bit 2 of positioning-mode parameter of FILE_SETKEY_.
    * @see #isPositionedToNext
    */
    public void setPositionToNext(boolean value) {
        positionToNextRecord_ = value;
    } // setPositionToNext

   /**
    * Returns a value that indicates whether positioning will skip the first qualifying record.
    *
    * @return true if the next qualifying record is to be skipped; false otherwise
    *
    * @see #setPositionToNext
    */
    public boolean isPositionedToNext() {
        return positionToNextRecord_;
    } // getPositionToNext

   /**
    * Indicates whether reads should occur in reverse order.
    * @return true if reads are to be read in reverse order
    * @see #setReadReversed
    *
    */
    public boolean areReadsReversed() {
        return readReverse_;
    } // getReadReverse



   /**
    * Specifies whether records should be returned in descending order. (The
    * file will be read in reverse order). If you specify true for value
    * and true for postitionToLast, positioning is
    * performed to the last record in a set of qualifying records.
    * @param value true to read the file from the end to the beginning; false otherwise.
    * @param positionToLast true to position to the last qualifying record in a set of records.
    * @see #areReadsReversed
    * @see #isPositionedToLast
    */
    public void setReadReversed(boolean value, boolean positionToLast) {
        readReverse_ = value;
        if(value)
        	positionToLastRecord_ = positionToLast;
        else
        	positionToLastRecord_ = false;
    } // setReadReversed

    /**
	 * Specifies whether records should be returned in descending order. (The
	 * file will be read in reverse order).
	 * @param value true to read the file from the end to the beginning; false otherwise.
	 * @see #areReadsReversed
	 * @see #isPositionedToLast
	 */
	 public void setReadReversed(boolean value) {
	     readReverse_ = value;
    } // setReadReversed

   /**
    * Indicates whether positioning will be to the last qualifying record in a
    * set of records.
    *
    * @return true positioning will be to the last record.
    *
    * @see #setReadReversed
    */
    public boolean isPositionedToLast() {
        return positionToLastRecord_;
    } // getPositionedToLast

    /**
    * Specifies the mode for reading.
    *
    * @param mode one of APPROXIMATE, EXACT, GENERIC.
    * @see #POSITION_APPROXIMATE
    * @see #POSITION_EXACT
    * @see #POSITION_GENERIC
    * @exception java.lang.IllegalArgumentException if mode is an invalid value.
    */
    public void setPositioningMode(int mode)
    	throws java.lang.IllegalArgumentException {
        if(mode == POSITION_APPROXIMATE ||
		   mode == POSITION_EXACT ||
		   mode == POSITION_GENERIC)
		{
			mode_ = mode;
		}
		else
            throw new IllegalArgumentException("Invalid keyposition mode. ");
    } // setPositioningMode

   /**
    * Returns the current setting of the positioning mode.
    *
    * @return the current setting of the positioning mode
	*
    * @see #setPositioningMode
    */
    public int getPositioningMode() {
        return mode_;
    } // getPositioningMode

} // EnscribeKeyPositionOptions
