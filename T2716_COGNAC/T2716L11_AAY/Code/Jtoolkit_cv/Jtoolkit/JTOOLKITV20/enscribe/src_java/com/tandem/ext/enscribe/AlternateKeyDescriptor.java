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


/**
 * <P>The AlternateKeyDescriptor handles attributes that describe a specific alternate
 * key.  This class can be passed as a parameter when creating or altering
 * a structured file.  It is also returned when information is requested
 * about a structured file.
 *
 * See the description of item code 101 in the item codes table of the
 * FILE_CREATELIST_, FILE_ALTERLIST_, or FILE_GETINFOLIST_ procedures
 * (documented in the Guardian Procedure Calls Reference Manual) for more
 * information on alternate key descriptor attributes and their values.</P>
 * @see KeySequencedCreationAttributes
 * @see StructuredCreationAttributes
 * @see EnscribeFileAttributes
 */
public class AlternateKeyDescriptor
{
	private int   keyFileNum_ = -1;
    private String   keySpecifierString_ = "";
    private short   keySpecifierNumeric_ = -1;
    private int   keyLength_ = -1;
    private int   keyOffset_ = -1;
    private short   nullValue_ = -1;
    private boolean nullValueSpecified_ = false;
    private boolean   uniqueKey_ = false;
    private boolean   automaticUpdatingPermitted_ = true;
    private boolean   orderByInsertionSequence_ = false;
    private boolean   keySpecifierIsString_ = false;
	private boolean  keySpecifierIsNumeric_ = false;
	private String alternateKeyFileName_ = null;
	boolean alternateKeyFileNameSet_ = false;


   /**
    * Construct an AlternateFileDescriptor object.
    *
    */
    public AlternateKeyDescriptor() {
    } // AlternateKeyDescriptor

	/**
	 * Sets the name of the alternate keyfile.  This attribute is REQUIRED.
	 * @param altKeyFileName a String containing the alternate key file name
	 * @see #getAlternateKeyFileName
	 */
	 public void setAlternateKeyFileName(String altKeyFileName) {
		alternateKeyFileName_ = altKeyFileName;
		alternateKeyFileNameSet_ = true;
	 }
	 /**
	  * Gets the name of the alternate keyfile.
	  * @return a String containing the alternate key file name if it has been
	  * set; null otherwise
	  * @see #setAlternateKeyFileName
	  */
	 public String getAlternateKeyFileName() {
		if(alternateKeyFileNameSet_)
	 		return alternateKeyFileName_;
	 	else
	 		return null;
 	}

	/**
	 * Sets the number of the file associated with this key.
	 * The first number assigned is 0.
	 *
	 * @param fnum the number of the file associated with this descriptor
	 * @see #getKeyFileNum
	 */
	 void setKeyFileNum(int fnum){
			keyFileNum_ = fnum;
     } // setKeyNum

 	/**
	 * Gets the number of the file associated with this descriptor.  This
	 * number is arbitrarily assigned if the file is created by JEnscribe.
	 *
	 * @return the number of the file associated with this descriptor
	 */
    public int getKeyFileNum(){
	  	return keyFileNum_;
    } // setKeyNum


    /**
    * Sets the key specifier that uniquely identifies the
    * alternate key field.  The key specifier must be set by using
    * either form of setKeySpecifier.
    *
    * @param specifier a 2 byte String specifier which identifies the
    * alternate key field
    */
    public void setKeySpecifier(String specifier) {
        keySpecifierString_ = specifier;
		keySpecifierIsNumeric_ = false;
		keySpecifierIsString_ = true;
    } // setKeySpecifier

   /**
    * Sets the key specifier that uniquely identifies the alternate key field.
    * The specifier must be nonzero.  The key specifier must be set by
    * using either form of setKeySpecifier.
    *
    * @param specifier the numeric specifier which identifies the
    * alternate key field
    * @see #setKeySpecifier
    */
    public void setKeySpecifier(short specifier) {
        keySpecifierNumeric_ = specifier;
		keySpecifierIsNumeric_ = true;
		keySpecifierIsString_ = false;
    } // setKeySpecifierNumeric

   /**
    * Returns the numeric key specifier that identifies the alternate key field.
    *
    * @return key specifier as a short; returns -1 if the key specifier is
    * not a numeric value.
    * @see #setKeySpecifier
    */
    public short getKeySpecifierNumeric() {
        return keySpecifierNumeric_;
    } // getKeySpecifier

   /**
    * Sets the key length, in bytes, of the alternate key field.  This
    * attribute is REQUIRED.
    *
    * @param keyLength the length of the alternate key field
    */
    public void setKeyLength(int keyLength) {
        keyLength_ = keyLength;
    } // setKeyLength

   /**
    * Returns the length of the alternate key field.
    * Returns -1 if setKeyLength() hasn't been called.
    *
    * @return length of the alternate key field
    * @see #setKeyLength
    */
    public int getKeyLength() {
        return keyLength_;
    } // getKeyLength


   /**
    * Sets the key offset, which is the number of bytes from the beginning of the record to where
    * the alternate key field starts.  This attribute is REQUIRED.
    *
    * @param offset the key offset in number of bytes
    */
    public void setKeyOffset(int offset) {
        keyOffset_ = offset;
    } // setKeyOffset

   /**
    * Returns the key offset within the record.
    * Returns -1 if setKeyOffset() hasn't been called.
    *
    * @return the key offset
    * @see #setKeyOffset
    */
    public int getKeyOffset() {
        return keyOffset_;
    } // getKeyOffset


   /**
    * Sets a null value that is used for the alternate key field if no value for this field is
    * supplied during a write operation.  If this method is not invoked,
    * NO null value is specified for this alternate key.
    *
    * @param nullValue short null value to use
    */
    public void setNullValue(short nullValue) {
        nullValue_ = nullValue;
        nullValueSpecified_ = true;
    } // setNullValue

   /**
    * Returns the null value that will be used for this alternate key
    * field.
    *
    * @return the null value
    * @see #setNullValue
    */
    public short getNullValue() {
        return nullValue_;
    } // getNullValue

   /**
    * Clears the null value, if any.
    */
    public void clearNullValue() {
        nullValueSpecified_ = false;
    } // clearNullValue

   /**
    * Indicates whether a null value has been specified.
    *
    * @return true if a null value has been specified
    */
    public boolean isNullValueSpecified() {
       return nullValueSpecified_;
    } // isNullValueSpecified

   /**
    * Specifies that this alternate key is unique.  If this method
    * is not invoked, this alternate key is NOT unique.
    *
    */
    public void setKeyIsUnique(boolean value) {
         uniqueKey_ = value;
    } // setKeyIsUnique

   /**
    * Indicates whether this alternate key field is unique.
    *
    * @return true if the alternate key field is to be unique,
    * false if duplicates are to be allowed.
    *
    * @see #setKeyIsUnique
    */
    public boolean isKeyUnique() {
       return uniqueKey_;
    } // isKeyUnique

   /**
    * Specifies whether automatic updating is turned off for this key.  If
    * this method is not invoked, automatic updating IS performed for this
    * key.
    * @param value true if automatic updating is performed on this key; false if not
    * @see #isAutomaticUpdatingProhibited
    */
    public void setAutomaticUpdating(boolean value) {
        automaticUpdatingPermitted_ = value;
    } // prohibitAutomaticUpdating

   /**
    * Indicates whether automatic updating is to be prohibited.
    *
    * @return true if automatic updating is prohibited,
    * false if it is to be allowed
    *
    * @see #setAutomaticUpdating
    */
    public boolean isAutomaticUpdatingProhibited() {
        return automaticUpdatingPermitted_;
    } // isAutomaticUpdatingProhibited

   /**
    * Specifies whether duplicate alternate key values are ordered by the value of the primary key field.
    * If this method is not invoked, duplicate alternate key values are ordered by
    * the sequence in which they were inserted in the alternate-key file.
    * @param value true if duplicate alternate key values should be ordered by the value of the primary
    * key field; false if otherwise
    * @see #isOrderedByPrimaryKeyValue
    * @see #setOrderByInsertionSequence
    * @see #isOrderedByInsertionSequence
    */
    public void setOrderByPrimaryKeyValue(boolean value) {
        orderByInsertionSequence_ = value;
    } // orderByPrimaryKey

   /**
    * Indicates whether duplicate alternate key values will be ordered by the primary key field value.
    *
    * @return true duplicate alternate key values are to be ordered
    * by primary key value
    *
    * @see #setOrderByInsertionSequence
    * @see #setOrderByPrimaryKeyValue
    * @see #isOrderedByInsertionSequence
    */
    public boolean isOrderedByPrimaryKeyValue() {
        return !orderByInsertionSequence_;
    } // isOrderedByPrimaryKeyValue

   /**
    * Specifies whether duplicate alternate key values are to be ordered by the sequence in which they
    * were inserted in the alternate key file.
    * @param value true if duplicate altenate key values are to be ordered by the sequenced in
    * which they were inserted; false otherwise.
    * @see #isOrderedByPrimaryKeyValue
    * @see #setOrderByPrimaryKeyValue
    * @see #isOrderedByInsertionSequence
    */
    public void setOrderByInsertionSequence(boolean value) {
        orderByInsertionSequence_ = value;
    } // orderByInsertionSequence

   /**
    * Indicates whether alternate key values are ordered by insertion sequence.
    *
    * @return true if this alternate key is to be ordered by insertion
    * sequence
    *
    * @see #isOrderedByPrimaryKeyValue
    * @see #setOrderByPrimaryKeyValue
    * @see #setOrderByInsertionSequence
    */
    public boolean isOrderedByInsertionSequence() {
        return orderByInsertionSequence_;
    } // isOrderedByInsertionSequence



   // ---------------------------------------------
   // ---------------------------------------------
	/*
    * Gets the alternate key information in a "," separated list.
    * Used internally.
    *
    * @return alternate key information in a comma separated list;
    */
     String getAttributeString() {
		StringBuffer str = new StringBuffer(100);

		// leading double quote says it is alpha
		if(keySpecifierIsNumeric_)
		{
			str.append(keySpecifierNumeric_).append(",");
		}
		else
		{
			str.append("\"").append(keySpecifierString_).append("\",");
		}
		str.append(keyLength_).append(",").append(keyOffset_).append(",").append(keyFileNum_).append(",");

		if(nullValueSpecified_)
		{
			str.append(nullValue_).append(",1,");
		}
		else
			str.append("0,0,");

		if(uniqueKey_)
			str.append("1,");
		else
			str.append("0,");

		if(automaticUpdatingPermitted_)
			str.append("0,");
		else
			str.append("1,");

		if(orderByInsertionSequence_)
			str.append("1,");
		else
			str.append("0,");
		//System.out.println("From akd: "+str.toString());
        return str.toString().trim();
    } // getAttributeString


} // AlternateKeyDescriptor
