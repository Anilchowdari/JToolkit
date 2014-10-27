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
import com.tandem.ext.enscribe.ParseDelimitedStringException;
import java.util.*;

class ParseDelimitedString
{
	String delimitedString_ = "";
	int startIdx_ = 0;
	int endIdx_ = -1;
	int delimiter_ = ',';
	boolean lastStringWasQuoted_ = false;

	/**
    * Constructs a <code>ParseDelimitedString</code> with the passed string
    *
    * @param   string to parse
    */
    ParseDelimitedString(String str) {
        delimitedString_ = str;
    } // ParseDelimitedString

	/**
    * Constructs a <code>ParseDelimitedString</code> with the passed string
    * and specified delimiter.
    *
    * @param str String to parse
    * @param delim delimiter to use
    */
    ParseDelimitedString(String str, int delim) {
        delimitedString_ = str;
		delimiter_ = delim;
    } // ParseDelimitedString

	/**
    * Returns the next string with double quotes removed.
    *
    * @return next string
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end or unmatched double quote.
    */
    String getNextString() throws ParseDelimitedStringException{
		startIdx_  = endIdx_ + 1;
		if(startIdx_ >= delimitedString_.length())
			throw new ParseDelimitedStringException("No more strings: at end of string");
		String
			ss;
		lastStringWasQuoted_ = false;
		if(delimitedString_.substring(startIdx_).startsWith("\""))
		{
			lastStringWasQuoted_ = true;
			// we can't just look for next delimiter as the delimiter may
			// be inside of double quotes.
			// Note: We don't deal with double quotes within double quotes

			endIdx_ = startIdx_ + 1;
			while(true)
			{
				if(endIdx_ >= delimitedString_.length())
					throw new ParseDelimitedStringException("Unmatched double quote: at end of string");
				if(delimitedString_.substring(endIdx_, endIdx_+1).equals("\""))
			   {
					endIdx_++;
					if(endIdx_ >= delimitedString_.length())
						endIdx_ = -1;
					break;
				}
				endIdx_++;
			}

		}
		else
			endIdx_ = delimitedString_.indexOf(delimiter_, startIdx_);
		if(endIdx_ == -1) // no trailing delimiter
		{
			ss = delimitedString_.substring(startIdx_);
			endIdx_ = delimitedString_.length();
		}
		else
			ss = delimitedString_.substring(startIdx_, endIdx_);
		if(lastStringWasQuoted_) // remove the leading and trailing double quote
		{
			ss = ss.substring(1, ss.length()-1);
		}
		return ss;
    } // getNextString

	/**
    * Returns the next int.
    *
    * @return next int
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    * or unmatched double quote or not an integer.
    */
    int getNextInt() throws ParseDelimitedStringException{
		String
			nextStr = "";
		try
		{
			nextStr = getNextString();
		}
		catch (ParseDelimitedStringException ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}

		Integer
			val;
		try
		{
			val = Integer.decode(nextStr);
			return val.intValue();
		}
		catch (NumberFormatException  ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}

	} // getNextInt
	/**
    * Returns the next long.
    *
    * @return next long
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    * or unmatched double quote or not an integer.
    */
    long getNextLong() throws ParseDelimitedStringException{
		String
			nextStr = "";
		try
		{
			nextStr = getNextString();
		}
		catch (ParseDelimitedStringException ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}

		Long val;
		try
		{
			val = Long.decode(nextStr);
			return val.longValue();
		}
		catch (NumberFormatException  ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}

	} // getNextLong

	/**
    * Returns the next short.
    *
    * @return next short
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    * or unmatched double quote or not an short.
    */
    short getNextShort() throws ParseDelimitedStringException{
		int
			nextInt;
		try
		{
			nextInt = getNextInt();
		}
		catch (ParseDelimitedStringException ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}
		if(nextInt > 32767)
			throw new ParseDelimitedStringException("Cannot convert to short");
		return (short)nextInt;

	} // getNextShort

	/**
    * Returns a boolean of true if next int is a non-zero, false otherwise.
    *
    * @return true if next int is non-zero
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    * or unmatched double quote or not an short.
    */
    boolean getNextBoolean() throws ParseDelimitedStringException{
		int
			nextInt;
		try
		{
			nextInt = getNextInt();
		}
		catch (ParseDelimitedStringException ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}
		return (nextInt != 0);
	} // getNextBoolean

	/**
    * Returns next GregorianCalendar object.
    *
    * @return a GregorianCalendar object
    *
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    * or unmatched double quote or not a Calendar.
    */
    GregorianCalendar getNextCalendar() throws ParseDelimitedStringException{
		String
			nextStr = "";

		try
		{
			int year = getNextInt();
			int month = getNextInt();
			int day = getNextInt();
			int hrs = getNextInt();
			int min = getNextInt();
			int sec = getNextInt();
			int millisec = getNextInt();
			//Next line is because month is zero based !!!
			month --;
			//System.out.println("year:month:day= "+year+":"+month+":"+day);
			if(year < 0)
				return null;
			GregorianCalendar dateVal = new GregorianCalendar(year, month, day, hrs, min, sec);
			dateVal.add(Calendar.MILLISECOND,millisec);
			return (dateVal);
		}
		catch (ParseDelimitedStringException  ex) {
			throw new ParseDelimitedStringException(ex.getMessage());
		}

	} // getNextCalendar

	/**
    * Returns the current index in the string being parsed.
    *
    * @return the current index in string
    */
    int getCurrentIndex() {
		return endIdx_;
	} // getCurrentIndex

	/**
    * Returns the whole string that is being parsed.
    *
    * @return the delimited string being parsed
    */
    String getDelimitedString() {
		return delimitedString_;
	} // getDelimitedString

	/**
    * Returns the delimiter used for parsing.
    *
    * @return the current delimiter
    */
    int getDelimiter() {
		return delimiter_;
	} // getDelimiter

	/**
    * Sets the current delimiter to be used for parsing.  Returns
    * the previous delimiter.
    *
    * @return the previous delimiter
    */
    int setDelimiter(int delim) {
		int
			oldDelim = delimiter_;
		delimiter_ = delim;
		return oldDelim;
	} // setDelimiter

	/**
    * Returns the remainder of the string.
    *
    * @return the remainder of the string
    * @exception com.tandem.enscribe.ParseDelimitedStringException if an already at end
    */
    String getRemainder() throws ParseDelimitedStringException{
		startIdx_  = endIdx_ + 1;
		if(startIdx_ >= delimitedString_.length())
			throw new ParseDelimitedStringException("No more strings: at end of string");
		return delimitedString_.substring(startIdx_);
	} // getRemainder

	/**
    * Resets the delimited string to the given string
    *
    */
    void resetString(String str) {
		startIdx_  = 0;
		endIdx_ =  -1;
		delimitedString_ = str;
	} // resetString

	/**
    * Returns true if the last string was quoted.
    *
    * @return true if last string was quoted
    */
    boolean wasLastStringWasQuoted() {
		return lastStringWasQuoted_;
	} // wasLastStringWasQuoted

}
