/* ------------------------------------------------------------------------
* Filename:     DataConversion.java 
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

import java.io.UnsupportedEncodingException;
import java.io.CharConversionException;
import com.tandem.ext.util.NativeLoader;
import java.math.*;


/**
 * The <code>DataConversion</code> class contains methods to convert data
 * to and from Guardian data types and Java data types.
 *
 * @version 1.0, 8/22/2001
 */
public final class DataConversion  {
	static {   
	NativeLoader loader = new NativeLoader();
	loader.LoadLibrary();
	getOSName();
	}


   /* All methods are static, we have a private ctor to prevent instantiation */
   private DataConversion () { };

    /**
     * Indicates a numeric string trailing embedded sign data type.
	 * See the Data Definition Language Reference manual for
	 * more information about numeric strings with embedded
	 * trailing signs.
     */
    public static final int NUMSTR_TRAILING_EMBEDDED = 15;
    /**
     * Indicates a numeric string trailing separate sign data type.
     * See the Data Definition Language Reference manual for
	 * more information about numeric strings with separate
	 * trailing signs.
     */
    public static final int NUMSTR_TRAILING_SEPARATE = 13;
    /**
     * Indicates a numeric string leading embedded sign data type.
     * See the Data Definition Language Reference manual for
	 * more information about numeric strings with embedded
	 * leading signs.
     */
    public static final int NUMSTR_LEADING_EMBEDDED = 14;
    /**
     * Indicates a numeric string leading separate sign data type.
     * See the Data Definition Language Reference manual for
	 * more information about numeric strings with separate
	 * leading signs.
     */
    public static final int NUMSTR_LEADING_SEPARATE = 12;
    /**
     * Indicates an unsigned numeric string data type.
     * See the Data Definition Language Reference manual for
	 * more information about unsigned numeric strings.
	 */
    public static final int NUMSTR_UNSIGNED = 16;

    static final byte decimalPt = (byte) '.';

    static final int USHRT_MAX = 65535;
    static final long UINT_MAX = 4294967295L;
	
	/**
	* Indicates the Rounding method used for an incoming
	* BigDecimal. The rounding method being used is
	* ROUND_HALF_EVEN. 
	**/
	public static final short ROUNDING_METHOD = BigDecimal.ROUND_HALF_EVEN;

   /*----------------------------------------------------------*/
   /*-- INPUT DATA CHECKING METHODS ---------------------------*/
   /* ---------------------------------------------------------*/

   /*
    * Check to see if all input variables are valid
    *
    * @param whichMethod to indicate if the called method is the method
    * JavaStrToNumStr (whichMethod = 2), or not (whichMethod = 0)
    * @param buf the buffer
    * @param inString a java String
    * @param offset the location within the buffer to place the converted
    * string
    * @param fieldSize the length of the original string
    *
    * @exception DataConversionException when input data is invalid
    */

    static void inputDataCheck (int whichMethod, byte[] buf, String inString,
           int offset, int fieldSize) throws DataConversionException
   {
      inputDataCheck (buf, inString, offset);

      if ( fieldSize <= 0 )
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the fieldSize variable can not be less than or equal to zero");

      if ((offset + fieldSize) > buf.length)
         throw new DataConversionException(DataConversionException.ConvError.RANGE, "The buffer is not big enough to hold the output");

      // check to see if the method NumStrTOJavaStr called this method
      switch (whichMethod)
      {
         case 0:
            if (inString.length() > fieldSize)
               throw new DataConversionException(DataConversionException.ConvError.RANGE, "The length of the input string is greater than the fieldSize");
            break;

         case 2:
            if ((inString.length() > fieldSize) && (inString.indexOf('.') == -1))
               throw new DataConversionException(DataConversionException.ConvError.RANGE, "The length of the input string can not be greater than the fieldSize unless the string contains a decimal point");

            if (inString.length() > fieldSize +1)
               throw new DataConversionException(DataConversionException.ConvError.RANGE, "The length of the input string is greater than the fieldSize + 1.");
            break;
         default:
      }
    }

   /*  
    * Check to see if all input variables are valid
    *
    * @param whichMethod to indicate if the called method is the method
    * JavaStrToNumStr (whichMethod = 2), or not (whichMethod = 0)
    * @param buf the buffer
    * @param inString a java String
    * @param offset the location within the buffer to place the converted
    * string
    * @param fieldSize the length of the original string
    * @param nOutType One of:
    * @see#NUMSTR_TRAILING_EMBEDDED,
    * @see#NUMSTR_TRAILING_SEPARATE,
    * @see#NUMSTR_LEADING_EMBEDDED,
    * @see#NUMSTR_LEADING_SEPARATE,
    * @see#NUMSTR_UNSIGNED
    *
    * @exception DataConversionException when input data is invalid
    */

    //Begin of Solution :10-100825-2712
    static void inputDataCheck(int whichMethod, byte[] buf, String inString,
     int offset, int fieldSize, int nOutType)throws DataConversionException {

     inputDataCheck(buf, inString, offset);

       if (fieldSize <= 0) {
         throw new DataConversionException
         (DataConversionException.ConvError.PARAM,"The value of "
          + "picture-string cannot be less than or equal to zero");
       }

       if ((offset + fieldSize) > buf.length) {
         throw new DataConversionException
         (DataConversionException.ConvError.RANGE,"The buffer"
         + " is not big enough to hold the output");
       }

       switch (whichMethod) {
         case 0:
           if (inString.length() > fieldSize) {
             throw new DataConversionException(
             DataConversionException.ConvError.RANGE,"The input"
             + " value does not match the picture-string ");
           }
           break;
         case 2:
         // Perform different inputDataCheck for PIC 'S' and 'T' clause.
           switch (nOutType) {
             case NUMSTR_UNSIGNED:
             case NUMSTR_LEADING_SEPARATE:
             case NUMSTR_TRAILING_SEPARATE:

            	 if ((inString.length() > fieldSize)
                     && (inString.indexOf(decimalPt) == -1)) {
                     // If inString does not contains decimal
                   throw new DataConversionException(
                 DataConversionException.ConvError.RANGE,
                 "The input value does not match the picture-string ");
               }

                 //
            	 if (inString.length() > fieldSize + 1) {
                    // If inString contains decimal                    
                   throw new DataConversionException(
                   DataConversionException.ConvError.RANGE,
                   "The input value does not match the picture-string");
               }
               break;

             case NUMSTR_LEADING_EMBEDDED:
             case NUMSTR_TRAILING_EMBEDDED:	       

               // We check whether the input value contains decimal 
               // point or not. For example: 1.1 or 11. 

               if(inString.indexOf(decimalPt) == -1) {

               /*
                * We reach here if it does not contains decimal.
                *
                * It should accepts valid inputs and throws exception for  
                * invalid values. For example: 99T , the fieldSize is 3 
                * and it accepts values till -111 & 111 i.e. inString of 
                * length 4 (fieldSize + 1)and not more than that.However 
                * it should throw exception for 1111 though it contains 
                * 4 digits.
                * 
                * First checking for inString of length till fieldSize + 1.
                */

                if(inString.length() > fieldSize + 1) {
                   throw new DataConversionException(
	               DataConversionException.ConvError.RANGE,
                   "The input value does not match the picture-string");
                 }
                 else if ((inString.length() > fieldSize)
                     && ( ! inString.startsWith("+"))
                     && ( ! inString.startsWith("-"))) {
                        // Throw exception when inString is of length 
                        // (fieldSize + 1) and contains only digits(no sign).
                       throw new DataConversionException(
                       DataConversionException.ConvError.RANGE,
                       "The input value does not match the picture-string");
                 }
               }

               else {

            	 /*
            	  * We reach here if it contains decimal.
                  *
                  * It should accepts valid inputs and throws exception for  
                  * invalid values. For example: 9V9T , the fieldSize is 3 
                  * and it accepts values till -1.11 & 1.11 i.e. inString of  
                  * length 5 (fieldSize + 2)and not more than that.However it
                  * should throw exception for 1.111though it contains 5 
                  * digits.
                  * 
                  * First checking for inString of length till fieldSize + 2.
                  */

            	 if (inString.length() > fieldSize + 2) {
                   throw new DataConversionException(
                   DataConversionException.ConvError.RANGE,
                   "The input value does not match the picture-string");
                 }
                 else if ((inString.length() > fieldSize + 1)
                     && ( ! inString.startsWith("+"))
                     && ( ! inString.startsWith("-"))) {
                            // Throw exception when inString is of length 
                            // (fieldSize + 2)and contains only digits(no sign).
                   throw new DataConversionException(
                   DataConversionException.ConvError.RANGE,
                   "The input value does not match the picture-string");
                 }
               }   
               break;
             default:
               throw new DataConversionException(
                 DataConversionException.ConvError.RANGE,"Invalid PICTURE type");
         }
       }
   }

	/* End of Solution :10-100825-2712 */


   /*
    * Check to see if all input variables are valid
    *
    * @param buf the buffer
    * @param offset the location within the buffer to place the converted
    * string
    * @param fieldSize the length of the original string
    * @param nInScale the scale to apply to the converted value
    * @param nInType One of:
	* @see#NUMSTR_TRAILING_EMBEDDED,
    * @see#NUMSTR_TRAILING_SEPARATE,
	* @see#NUMSTR_LEADING_EMBEDDED,
    * @see#NUMSTR_LEADING_SEPARATE,
	* @see#NUMSTR_UNSIGNED
    *
    * @exception DataConversionException when buf.length == 0,
	* offset < 0, offset > buf.length, offset + fieldSize > buf.length, fieldSize < 0, nInScale > fieldSize, nInScale < 0,
	* nInType is not one of NUMSTR_TRAILING_EMBEDDED, NUMSTR_TRAILING_SEPARATE,
	* NUMSTR_LEADING_EMBEDDED, NUMSTR_LEADING_SEPARATE, or NUMSTR_UNSIGNED.
    */
   static void inputDataCheck (byte[] buf, int offset, int fieldSize,
            int nInScale, int nInType) throws DataConversionException
   {
      inputDataCheck (buf, offset, fieldSize);

      if (nInScale > fieldSize || nInScale < 0)
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either the value of the variable nInScale is greater than the value of the variable fieldSize or the value of the variable nInScale is less than zero.");

   }

   static void inputDataCheck (byte[] buf, String inString,
                            int offset) throws DataConversionException

   {
	   //Begin changes for the solution number: 10-031024-0747
	   if ( inString == null )
		   throw new DataConversionException(DataConversionException.ConvError.PARAM, "Value of inString variable is null");
	   //End changes end for the solution number: 10-031024-0747
      inputDataCheck (buf, offset);
   }

   static void inputDataCheck (byte[] buf, int offset, int fieldSize)
                                               throws DataConversionException
   {
      inputDataCheck (buf, offset);

      if ( fieldSize <= 0 )
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the fieldSize variable can not be less than or equal to zero");
   }

   static void inputDataCheck (byte[] buf, int offset)
                                               throws DataConversionException
   {
      if (buf.length == 0)
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "The buffer is empty");

      if ( offset < 0 || offset > buf.length )
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the offset can not be less than zero or greater than the length of the buffer");
   }
   /*  
    * Check to see if any of the 8 native-dependent Data Conversion methods are called off-Platform
    *
    * @param passing 8 native API's  to the loadcheck function
    * @exceptions in case of any refernces to  8 native API's
    */
   /*Begin changes for the RFE solution 10-121009-4998 */
   
	static String osname=null;   
   static void  loadcheck(String native_api) 
   { 
   		if(osname==null) getOSName();
		if(!"NONSTOP_KERNEL".equals(osname)){
			 throw new RuntimeException("Cannot use DataConversion method "+native_api+" on non-NSK platform");
		}
	}

	static void getOSName(){
		try{
			osname = System.getProperty("os.name");			
		  }
		catch(Exception e)
		{
			  System.out.println("Exception occured while getting system property" +e.getMessage());
		}
	}
	
   /*End changes for the REF solution 10-121009-4998 */
   
	/* Begin changes for Solution Number: 10-060905-8795 */
	  
	/* In all API's phrases "Tandem float" or "Tandem double" were replaced with "TNS float" and "TNS double" 
	 * and likewise "Himalaya Float/Double" were described as "IEEE Float/Double" to be 
	 * consistent with other HP NonStop documentation".
	 */
	 
	/* End Changes for Solution Number: 10-060905-8795 */

   /* ---------------------------------------------------------*/
   /*-- CONVERSION METHODS FROM JAVA TYPES TO GUARDIAN TYPES --*/
   /* ---------------------------------------------------------*/

    /**
     * Converts a blank-padded string to a null-terminated string and places
     * the contents into the buffer.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     *
     * @exception DataConversionException when there is an error converting,
	 * inString is null, inString.length() == 0, inString.length() > fieldSize,
	 * fieldSize > buf.length,
	 * buf.length = 0, fieldSize < 0 || > buf.length, offset > buf.length,
	 * offset + fieldSize > buf.length, or offset < 0.
     */
    public static final void JavaStrToCStr (byte[] buf, String inString,
                     int offset, int fieldSize) throws DataConversionException
    {
        inputDataCheck (0, buf, inString, offset, fieldSize);

    	//Copy string to buffer at offset,
       inString.getBytes(0, inString.length(), buf, offset);
    }

    /**
     * Converts a blank-padded string to a null-terminated string and places
     * the contents into the buffer, using enc for data encoding.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     * @param enc the character-encoding name
     *
     * @exception DataConversionException when there is an error converting,
	 * inString is null, inString.length() == 0, inString.length() > fieldSize,
	 * fieldSize > buf.length,
	 * buf.length = 0, fieldSize < 0 || > buf.length, offset > buf.length,
	 * offset + fieldSize > buf.length, offset < 0, or enc == null.
     */
    public static final void JavaStrToCStr (byte[] buf, String inString,
           int offset, int fieldSize, String enc) throws DataConversionException
    {
        inputDataCheck (0, buf, inString, offset, fieldSize);    	

    	if (enc == null)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is null");

       try {
          byte tempBytes[] = inString.getBytes(enc);
          for (int i = offset; i<offset + fieldSize; i++)
             buf[i]=(byte)' ';
          for (int i = offset, j=0; j<tempBytes.length; i++, j++)
             buf[i] = tempBytes[j];
       } catch (UnsupportedEncodingException x) {
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is not supported");
       }
    }

    /**
     * Converts a blank-padded string to a non null-terminated string
     * and places the contents into the buffer.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     *
     * @exception DataConversionException when there is an error converting,
     * inString is null, inString.length() == 0, inString.length() > fieldSize,
	 * fieldSize > buf.length,
	 * buf.length = 0, fieldSize < 0 || > buf.length, offset > buf.length,
	 * offset + fieldSize > buf.length, or offset < 0.
     */
    public static final void JavaStrToCobolStr (byte[] buf, String inString,
                      int offset, int fieldSize) throws DataConversionException
    {

        inputDataCheck (0, buf, inString, offset, fieldSize);

        for (int i=offset; i<offset+fieldSize; i++)
          buf[i] = (byte) ' ';
       inString.getBytes(0, inString.length(), buf, offset);
    }

    /**
     * Converts a blank-padded string to a non null-terminated string
     * and places the contents into the buffer, using enc for data
     * encoding.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     * @param fieldSize the length of the original string
     * @param enc the character-encoding name
     *
     * @exception DataConversionException when there is an error converting,
     * inString is null, inString.length() == 0, inString.length() > fieldSize,
	 * fieldSize > buf.length,
	 * buf.length = 0, fieldSize < 0 || > buf.length, offset > buf.length,
	 * offset + fieldSize > buf.length, offset < 0, or enc == null.
     */
    public static final void JavaStrToCobolStr (byte[] buf, String inString,
           int offset, int fieldSize, String enc) throws DataConversionException
    {
       inputDataCheck (0, buf, inString, offset, fieldSize);

       if (enc == null)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is null");

       try {
          byte tempBytes[] = inString.getBytes(enc);
          for (int i = offset; i<offset + fieldSize; i++)
             buf[i]=(byte)' ';
          for (int i = offset, j=0; j<tempBytes.length; i++, j++)
             buf[i] = tempBytes[j];
       } catch (UnsupportedEncodingException x) {
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is not supported");
       }
    }
//  Begin Changes for Solution Number 10-040622-7210
    // The description for Param nOutType has been added in the line 343

    /**
     * Converts a Java string into a numeric string and puts the result in the buffer.
	 * Numeric strings are usually used when communicating with
	 * servers written in COBOL, or when writing data to a data
	 * base that will then be read by a server written in COBOL. See
	 * the Data Definition Reference Manual for more information
	 * about numeric strings.
     * The following applies to the length of inString:
	 * When nOutType is NUMSTR_TRAILING_SEPARATE, or NUMSTR_LEADING_SEPARATE,
	 * or NUMSTR_UNSIGNED, if inString contains
	 * a decimal point ".", then inString.length() + 1 cannot be longer than
	 * fieldSize.  If inString does not contain a decimal point than inString
	 * cannot be longer than field size.
	 * When nOutType is NUMSTR_TRAILING_EMBEDDED or NUMSTR_LEADING_EMBEDDED,
	 * if inString contains a decimal point and a sign character, then
	 * inString.length() cannot be longer than fieldSize + 2.  If inString
	 * contains a sign character but does not contain a decimal point, then
	 * inString.length() cannot be longer than fieldSize + 1. If inString
	 * contains neither a sign character nor a decimal point, then
	 * inString cannot be longer than fieldSize.
      *
     * @param buf the buffer the converted string is put into
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     * @param fieldSize the length of the field in the buffer
     * @param nOutScale the scale to apply to the converted string
     * @param nOutType One of: NUMSTR_TRAILING_EMBEDDED,
     *                         NUMSTR_TRAILING_SEPARATE, 
     *                         NUMSTR_LEADING_EMBEDDED,
     *                         NUMSTR_LEADING_SEPARATE,
     *                         NUMSTR_UNSIGNED.
     * @see #NUMSTR_TRAILING_EMBEDDED
     * @see #NUMSTR_TRAILING_SEPARATE
     * @see #NUMSTR_LEADING_EMBEDDED
     * @see #NUMSTR_LEADING_SEPARATE
     * @see #NUMSTR_UNSIGNED
     *
     * @exception DataConversionException when there is an error converting,
     * inString is null, inString.length() == 0, fieldSize > buf.length,
	 * buf.length = 0, fieldSize < 0 || > buf.length, offset > buf.length,
	 * offset + fieldSize > buf.length, offset < 0, or
	 * nOutType is not one of NUMSTR_TRAILING_EMBEDDED, NUMSTR_TRAILING_SEPARATE,
	 * NUMSTR_LEADING_EMBEDDED, NUMSTR_LEADING_SEPARATE, or NUMSTR_UNSIGNED.
	 */

// End Changes for Solution Number 10-040622-7210

	public static final void JavaStrToNumStr (byte[] buf, String inString,
		int offset, int fieldSize, int nOutScale, int nOutType)
		throws DataConversionException
	{
		byte inBytes[];
		int inLen, nInSize, inEndDig, inDig=0, saveDig;
		int outStartDig, outEndDig, outDig=0, nOutSize;
		boolean negative;
		
		//Solution:10-100825-2712 This method called two times,hence commented.
		//inputDataCheck (2, buf, inString, offset, fieldSize); 

		inBytes = inString.getBytes();
		inLen = inString.length();
		nInSize = inLen;
		nOutSize = fieldSize;
		outDig = offset;
		
		//Solution:10-100825-2712:The below line commented because its called in next line with addition of parameter nOutType
		//inputDataCheck (2, buf, inString, offset, fieldSize);
		inputDataCheck (2, buf, inString, offset, fieldSize, nOutType); 
		

		if (fieldSize < nOutScale || nOutScale < 0)
			throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either the value of the variable nOutScale is greater than the value of the variable fieldSize or the value of the variable nOutScale is less than zero.");
		//Begin changes for the solution number: 10-031024-0747
		if (inString.length()==0)                  
			inString.getBytes(0, 0, buf, offset); //Copy string to buffer at offset
		else
		{    
			negative = (inBytes[inDig] == '-');
			if (inBytes[inDig] == '-' || inBytes[inDig] == '+')
			{
				nInSize--;
				inDig++;
			}

			for (int i=offset; i<(offset+fieldSize); i++)
				buf[i] = (byte) '0';

			if ( nInSize == 0 || (nInSize == 1 && inBytes[inDig] == decimalPt) )
				throw new DataConversionException(DataConversionException.ConvError.SYNTAX, "The input string format is invalid");

			switch (nOutType)
			{
				case NUMSTR_LEADING_SEPARATE:
					if(nOutSize < 2)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
					buf[outDig++] = (byte) (negative ? '-' : '+');
					nOutSize--;
					break;

				case NUMSTR_TRAILING_SEPARATE:
					if(nOutSize < 2)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
					buf[--nOutSize + offset] = (byte) (negative ? '-' : '+');
					break;

				case NUMSTR_LEADING_EMBEDDED:
					if(nOutSize < 1)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
					if (negative) buf[outDig] |= 0x80;
					break;

				case NUMSTR_TRAILING_EMBEDDED:
					if(nOutSize < 1)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
					if (negative) buf[nOutSize - 1 + offset] |= 0x80;
					break;

				case NUMSTR_UNSIGNED:
					if(nOutSize < 1)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
					if (negative)
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "wrong input string format for the unsigned numeric string data type");
					break;
				default:
					throw new DataConversionException(DataConversionException.ConvError.PARAM, "The input for the nOutType variable is not supoorted");
			}

			// Skip leading zeros and scan digits in the integer part of the number.
			// When the last integer digit is scanned, moved the significant digits
			// into the integer field of the result, checking for overflow first.
			while (nInSize > 0 && inBytes[inDig] == '0')
			{
				inDig++;
				nInSize--;
			}
			saveDig = inDig;
			while (nInSize > 0 && inBytes[inDig] != decimalPt)
			{
				if (inBytes[inDig] >= '0' && inBytes[inDig] <= '9')
				{
					inDig++;
					nInSize--;
				}
				else
				{
					throw new DataConversionException(DataConversionException.ConvError.PARAM, "The input string is not a numeric string");
				}
			}
			outEndDig = outDig + (nOutSize - nOutScale);
			inEndDig = inDig;
			if(nOutScale < 0)
			{
				outEndDig += nOutScale;
				inEndDig += nOutScale;
			}
			if (outEndDig - outDig < inEndDig - saveDig)
				throw new DataConversionException(DataConversionException.ConvError.RANGE);
			while (inEndDig > saveDig)
			{
				--outEndDig;
				--inEndDig;
				buf[outEndDig] |= inBytes[inEndDig];
			}

			if (nInSize > 0 && inBytes[inDig] == decimalPt)
			{
				// The number has a fraction part.  Skip leading zeros and
				// scan digits in this part.  When the last digit is scanned,
				// move the significant digits into the fraction field of
				// the result, checking for overflow first.  Discard any input
				// digits of lower order than the output scale, or use zeros
				// for output digits of lower order than the input scale.

				short scale;
				inEndDig = ++inDig;
				nInSize--;

				while (nInSize > 0 && inBytes[inDig] == '0')
				{
					inDig++;
					nInSize--;
				}
				saveDig = inDig;
				while (nInSize > 0)
				{
					if (inBytes[inDig] >= '0' && inBytes[inDig] <= '9')
					{
						inDig++;
						nInSize--;
					}
					else
					{
						throw new DataConversionException(DataConversionException.ConvError.PARAM, "The input string is not a numeric string");
					}
				}
				scale = (short) (inDig - inEndDig);
				inEndDig = inDig;
				outEndDig = outDig + nOutSize;

				if (scale > nOutScale)
					inEndDig -= scale - nOutScale;
				else
					if (nOutScale > scale) outEndDig -= nOutScale - scale;

				if ( outEndDig - outDig < inEndDig - saveDig)
					throw new DataConversionException(DataConversionException.ConvError.RANGE);
				while (inEndDig > saveDig)
				{
					--outEndDig;
					--inEndDig;
					buf[outEndDig] |= inBytes[inEndDig];
				}
			}
		}
		//End changes for the solution number: 10-031024-0747
	}

    /**
     * Converts a one byte Java String to a one byte ASCII string
     * and places it in the buffer; corresponds to a BINARY 8 field
	 * as described in the Data Definition Language Reference Manual.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted byte
     *
     * @exception DataConversionException when there is an error converting;
	 * if buf.length == 0, inString == null, inString.length() == 0 || inString.length > 1,
	 * offset > buf.length, or offset < 0.
     */
    public static final void JavaStrToBIN8 (byte[] buf, String inString,
                         int offset) throws DataConversionException
    {
       /* Make sure string is only one byte */
       if (inString.length() != 1)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The length of the input string has to be 1");

       inputDataCheck ( buf, inString, offset );

       // Move byte into buffer at offset
       inString.getBytes(0, 0,  buf, offset);
    }

    /**
     * Converts a one byte Java string to a 2 byte ASCII string for COBOL;
	 * corresponds to a BINARY 8 field as described in the Data Definition
	 * Language Reference Manual.
     * The contents of the string is stored in the first element and a blank
     * character is stored in the second. The converted value is placed in
     * the buffer.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, inString == null, inString.length() == 0 || inString.length > 1,
	 * offset > buf.length, or offset < 0.
     */
    public static final void JavaStrToBIN8Cobol (byte[] buf, String inString,
                         int offset) throws DataConversionException
    {
       /* Make sure string is only one byte */

       if (inString.length() != 1)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The length of the input string has to be 1");

       inputDataCheck ( buf, inString, offset );

       // Move byte into bufer at offset and add space in second byte
       inString.getBytes(0, 0,  buf, offset);
       buf[offset + 1] = (byte) ' ';
    }

    /**
     * Converts a Java short into an IEEE format short and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param shortValue the short value to convert
     * @param offset the location within the buffer to place the
     * converted short
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset > buf.length, or offset + 2 > buf.length.
     */
    public static final  void JavaShortToBIN16 (byte[] buf, short shortValue,
                         int offset) throws DataConversionException
    {
       if ((offset + 2) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       inputDataCheck ( buf, offset );

       /* convert the shortValue into 2 bytes */
       int tmpByte;

       for (int i=0; i<2; i++)
       {
          tmpByte = shortValue >> (8 * i);
          buf[offset + (1 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a Java integer into an unsigned IEEE format short and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param intValue the integer value to convert
     * @param offset the location within the buffer to place the
     * converted short
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0, offset + 2 > buf.length, or
	 * the value of intValue > 65535 || intValue < 0.
     */
    public static final void JavaIntToUNBIN16 (byte[] buf, int intValue,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 2) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       if ( (intValue > USHRT_MAX) | (intValue < 0) )
          throw new DataConversionException(DataConversionException.ConvError.RANGE, "The value supplied for the intValue can not be greater than 65535 or less than 0");

       /* convert the value into 2 bytes */
       int tmpByte;

       for (int i=0; i<2; i++)
       {
          tmpByte = intValue >> (8 * i);
          buf[offset + (1 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a Java int into an IEEE format integer and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param intValue the integer value to convert
     * @param offset the location within the buffer to place the
     * converted int
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0, or offset + 4 > buf.length.
     */
    public static final void JavaIntToBIN32 (byte[] buf, int intValue,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 4) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /* convert the value into 4 bytes */
       int tmpByte;

       for (int i=0; i<4; i++)
       {
          tmpByte = intValue >> (8 * i);
          buf[offset + (3 - i)] = (byte) (tmpByte  & 0xff);
       }
    }

    /**
     * Converts a Java long into an unsigned IEEE format integer and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param longValue the long value to convert
     * @param offset the location within the buffer to place the
     * converted int
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0, offset + 4 > buf.length, or
	 * longValue > 4294967295L.
     */
    public static final void JavaLongToUNBIN32 (byte[] buf, long longValue,
                         int offset) throws DataConversionException
    {
      inputDataCheck ( buf, offset );

      if ((offset + 4) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       if ((longValue > UINT_MAX) || (longValue < 0))
          throw new DataConversionException(DataConversionException.ConvError.RANGE, "The value supplied for the longValue variable can not be greater than 4294967295 or less than 0");

       /* convert the value into 4 bytes */
       long tmpByte;

       for (int i=0; i<4; i++)
       {
          tmpByte = longValue >> (8 * i);
          buf[offset + (3 - i) ] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a Java long into a 64 bit long long value in IEEE format
     * and places the result in the buffer.
     *
     * @param buf the buffer
     * @param longlongValue the long value to convert
     * @param offset the location within the buffer to place the
     * converted long long value.
     *
     * @exception DataConversionException when there is an error converting;
	 * if buf.length == 0, offset < 0, or offset + 8 > buf.length.
     */
    public static final void JavaLongToBIN64 (byte[] buf, long longlongValue,
                           int offset) throws DataConversionException
    {
      inputDataCheck ( buf, offset );

      if ((offset + 8) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /* convert the value into 8 bytes */
       long tmpByte;

       for (int i=0; i<8; i++)
       {
          tmpByte = longlongValue >> (8*i);
          buf[offset + (7 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a one byte Java String to a Logical 1 field
     * and places it in the buffer.  This corresponds to a LOGICAL1
	 * field as described in the Data Definition Language Reference
	 * Manual. Normally a LOGICAL1 field is used to hold a value indicating
	 * true or false.  JavaStrToLogical1 makes no attempt to verify the
	 * contents of the field.
     *
     * @param buf the buffer
     * @param inString a java String
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting.
	 * if buf.length == 0, inString == null, offset < 0, inString.length() < 1,
	 * or offset + 1 > buf.length.
     */
    public static final void JavaStrToLogical1 (byte[] buf, String inString,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, inString, offset );

       /* Make sure string is only one byte */
       int len = inString.length();

       if(len != 1 || (offset + 1) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either the length of the input string is not equal to one or There is not enough room from offset to the end of the buffer to hold the output");

       /* Move byte into buffer at offset */
       inString.getBytes(0, 0, buf, offset);
    }

    /**
     * Converts a Java short into a logical 2 field and places the result
     * in the buffer.  This corresponds to a LOGICAL 2 fields as described
	 * in the Data Definition Language Reference Manual.  Normally, such a
	 * field is used to hold a value which defines true or false, JavaShortToLogical2
	 * makes no attempt to verify the contents of shortValue.
     *
     * @param buf the buffer
     * @param shortValue the short value to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0,
	 * or offset + 2 > buf.length.
     */
    public static final void JavaShortToLogical2 (byte[] buf, short shortValue,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 2) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /* convert the shortValue into 2 bytes */
       int tmpByte;

       for (int i=0; i<2; i++)
       {
          tmpByte = shortValue >> (8 * i);
          buf[offset + (1 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a Java integer into a logical 4 field and places the
     * result in the buffer.  This corresponds to a LOGICAL 4 field as
	 * described in the Data Definition Language Reference Manual.  Normally,
	 * such a field is used to store a value which can be defined as true
	 * or false. JavaIntToLogical4 makes no attempt to verify the contents of
	 * intValue.
     *
     * @param buf the buffer
     * @param intValue the value to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0,
	 * or offset + 4 > buf.length.
     */

    public static final void JavaIntToLogical4 (byte[] buf, int intValue,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 4) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /* convert the value into 4 bytes */
       int tmpByte;

       for (int i=0; i<4; i++)
       {
          tmpByte = intValue >> (8 * i);
          buf[offset + (3 - i)] = (byte) (tmpByte & 0xff);
       }
    }
	
	//Beginning of Soln_10-130423-7115
	/**
	* BigDecimal Data Conversion implementation method for OUTPUT class.
	* This method converts a Java BigDecimal to equivalent byte arrays. 
	* 
	* 
	* @param buf the buffer
	* @param in_value the incoming BigDecimal value to convert
	* @param offset the location within the buffer to place the converted BigDecimal
	* @param data_type the incoming DDL type
	* @param fieldSize the length(in bytes) of DDL datatype PIC [S]9(m)V(n) COMP.
	* @param scale the number of digits after decimal points
	*
	* @exception DataConversionException when there is an error converting,
	* if buf.length == 0, offset > buf.length, or offset + fieldsize > buf.length,
	* if fieldsize < 0.
	*/

	public static final void BigDecimalToBIN (byte[] buf, BigDecimal in_value,
                         int offset, int data_type, int fieldsize, int scale) throws DataConversionException
	{
		
		inputDataCheck (buf, offset, fieldsize);			
					    
		BigDecimal bd2 = in_value.setScale(scale,ROUNDING_METHOD); //set the scale of BigDecimal value			   
				   
		//The fixed decimal needs to shifted 'scale' bits to the right.
		//e.g. 1.29 needs to be converted to 129 and the latter is then stored in the byte arrays.
		BigDecimal bd3 = bd2.movePointRight(scale);	
						 			  			   			   
		switch (data_type)
		{     
			//BINARY_16_SIGNED (2 bytes)
		   	case 2:   
			        short shortValue = bd3.shortValue();
				JavaShortToBIN16(buf, shortValue, offset);
			        break;
				  
			//BINARY_16_UNSIGNED (2 bytes)	  
		 	case 3:	 
			        int intValue1 = bd3.intValue();
				if ( (intValue1 > USHRT_MAX) || (intValue1 < 0) )
					throw new DataConversionException(DataConversionException.ConvError.RANGE, "The value supplied is too big to be stored as a BINARY_16_UNSIGNED type or it is less than 0");
				JavaIntToUNBIN16(buf, intValue1, offset); 
				break;
				  
			//BINARY_32_SIGNED (4 bytes)	  
			case 4: 
				int intValue2 = bd3.intValue();
				JavaIntToBIN32(buf, intValue2, offset);
				break;
			           					   
			//BINARY_32_UNSIGNED (4 bytes)	   
			case 5: 
			         
			        long longValue = bd3.longValue();
				if ( (longValue > UINT_MAX) || (longValue < 0) )
					throw new DataConversionException(DataConversionException.ConvError.RANGE, "The value supplied is too big to be stored as a BINARY_32_UNSIGNED type or it is less than 0");
				//System.out.println("Value of longValue is :" +longValue);
				JavaLongToUNBIN32(buf, longValue, offset);
			 	break;
				   
			//BINARY_64_SIGNED (8 bytes)	   
			case 6:  
			        long longlongValue = bd3.longValue();
				JavaLongToBIN64(buf, longlongValue, offset);
			        break;
					   
			default: 
			         throw new DataConversionException(DataConversionException.ConvError.PARAM, "Invalid data-type, must be a BINARY type having scale more than 1"); 
		}		
	}
	
	
	/**
	* BigDecimal Data Conversion implementation method for INPUT class.
	* This method converts a byte array to equivalent Java BigDecimal. 
	* 
	* @param buf the buffer which contains byte arrays to be converted
	* @param offset the location within the buffer
	* @param data_type the BINARY type to find out the number of bytes allocated.
	* @param fieldSize the length(in bytes) of DDL datatype PIC [S]9(m)V(n) COMP.
	* @param scale the number of digits after decimal points
	*
	* @exception DataConversionException when there is an error converting,
	* if buf.length == 0, offset < 0, offset > buf.length, or offset + fieldsize > buf.length.
	*/
	 public static final BigDecimal BINToJavaBigDecimal(byte[] buf, int offset, 
	 					int data_type, int fieldsize, int scale) throws DataConversionException
	{
		
		inputDataCheck (buf, offset, fieldsize);
		
		
		BigDecimal bd1 = null;
		BigDecimal bd2 = null;
		switch(data_type)
		{
			//BINARY_16_SIGNED (fieldsize = 2 bytes)
			case 2: 
				short shortValue = BIN16ToJavaShort(buf, offset);
				bd2 = new BigDecimal(shortValue).movePointLeft(scale);
				break;
				
			//BINARY_16_UNSIGNED (fieldsize = 2 bytes)
			case 3:
				int intValue1 = UNBIN16ToJavaInt(buf, offset);
				bd2 = new BigDecimal(intValue1).movePointLeft(scale);
				break;
			
			//BINARY_32_SIGNED (fieldsize = 4 bytes)
			case 4:
				int intValue2 = BIN32ToJavaInt(buf, offset);
				bd2 = new BigDecimal(intValue2).movePointLeft(scale);
				break;				
				
			//BINARY_32_UNSIGNED (fieldsize = 4 bytes)
			case 5:
				long longValue = UNBIN32ToJavaLong(buf, offset);
				bd2 = new BigDecimal(longValue).movePointLeft(scale);
			    break;
				
			//BINARY_64_SIGNED (fieldsize = 8 bytes)
			case 6:
				long longlongValue = BIN64ToJavaLong(buf, offset);
				bd2 = new BigDecimal(longlongValue).movePointLeft(scale);
				break;
				
			default:  
				throw new DataConversionException(DataConversionException.ConvError.PARAM, "Invalid data-type, must be a BINARY type having scale more than 1");
					
		}
		return bd2;
		
	}
    //End of Soln_10-130423-7115
    
    /**
     * Converts a Java String representing a IEEE floating point
     * value into a IEEE float and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param inString the String value to convert
     * @param offset the location within the buffer to place the
     * converted float
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, inString == null, offset < 0, or the contents
	 * of inString cannot be converted into a float value.
     */
    public static final void StrToFloat (byte[] buf, String inString,
                         int offset) throws DataConversionException
    {
	   loadcheck("StrToFloat()"); /* New Function added for the RFE solution 10-121009-4998*/
       inputDataCheck ( buf, inString, offset );

       // Check the inString format
       try {
          Float f = new Float (inString);
          f = null;   //not use, release the memory
       } catch (NumberFormatException ne) {
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either inString does not contain a parseable number or the value is out ofrange.");
       }

       StrToFloatNative (buf, inString, offset);
    }

    private static final native void StrToFloatNative (byte[] buf,
                    String inString,int offset) throws DataConversionException;

    /**
     * Converts a Java String representing a IEEE double
     * value into a IEEE double and places the result
     * into the buffer.
     *
     * @param buf the buffer
     * @param inString the String value to convert
     * @param offset the location within the buffer to place the
     * converted double
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, inString == null, offset < 0, or the contents
	 * of inString cannot be converted into a double value.
     */
    public static final void StrToDouble (byte[] buf, String inString,
                         int offset) throws DataConversionException
    {
	   loadcheck("StrToDouble()");  /* New Function added for the RFE solution 10-121009-4998*/
       inputDataCheck ( buf, inString, offset );

       // Check the inString format
      try {
         Double d = new Double (inString);
         d = null; //not use, release the memory
      } catch (NumberFormatException ne) {
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either inString does not contain a parseable number or the value is out of range.");
      }

       StrToDoubleNative (buf, inString, offset);
    }

    private static final native void StrToDoubleNative (byte[] buf,
                   String inString, int offset) throws DataConversionException;

    /**
     * Converts a int representing a bit field into a short and places the result
     * into the buffer.  This corresponds to a BIT field as
	 * described in the Data Definition Language Reference
	 * Manual.
     *
     * @param buf the buffer
     * @param value the value to convert
     * @param offset the location within the buffer to place the
     * converted short
     *
     * @exception DataConversionException when there is an error converting.
	 * if buf.length == 0, offset < 0, offset + 2 > buf.length, or
	 * value > 32767.
     */
    public static final void BITToTdmShort (byte[] buf, int value,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 2) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /*  Make sure BITvalue is in right range */
       if ((value > 32767) && (value < 0))
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be greater than 32767 or less than 0");

       /* Convert value to bytes */
       int tmpByte;

       for (int i=0; i<2; i++)
       {
          tmpByte = value >> (8 * i);
          buf[offset + (1 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts an Java int representing a bit field into an unsigned IEEE short and places the result
     * in the buffer.  This corresponds to an unsigned BIT field as described
	 * in the Data Definition Language Reference manual.
     *
     * @param buf the buffer
     * @param value the value to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0, offset + 2 > buf.length, or
	 * value > 65535 || value < 0.
     */
    public static final void UBITToTdmShort (byte[] buf, int value,
                         int offset) throws DataConversionException
    {
       inputDataCheck ( buf, offset );

       if ((offset + 2) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "There is not enough room from offset to the end of the buffer to hold the output");

       /*  Make sure BITvalue is in right range */
       if((value > 65535) && (value < 0))
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be greater than 65535 or less than 0");

       /* Convert value to bytes */
       int tmpByte;

       for (int i=0; i<2; i++)
       {
          tmpByte = value >> (8 * i);
          buf[offset + (1 - i)] = (byte) (tmpByte & 0xff);
       }
    }

    /**
     * Converts a Java String into a IEEE format short and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param value a java String to convert
     * @param offset the location within the buffer to place the
     * converted value
     *
     * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, offset < 0, offset + 2 > buf.length, or
	 * the contents of value cannot be converted to a short.
     */
    public static final  void JavaStrToBIN16(byte[] buf, String value,
                         int offset) throws DataConversionException
    {
       short s;

       inputDataCheck ( buf, offset );

       // Convert the input String to short
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
         s = Short.valueOf(value).shortValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       // Convert short to unsigned short
       JavaShortToBIN16 (buf, s, offset);
    }

    /**
     * Converts a Java String into a IEEE format unsigned short and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted value.
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, offset + 2 > buf.length, or
	 * the contents of value cannot be converted to an unsigned short.
     */
    public static final void JavaStrToUNBIN16 (byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       int intVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to integer
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          intVal = Integer.valueOf(value).intValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out ofrange.");
       }

       JavaIntToUNBIN16 (buf, intVal, offset);
    }

    /**
     * Converts a Java String into a IEEE format integer and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted value
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, offset + 4 > buf.length, or
	 * the contents of value cannot be converted to a int.
     */

    public static final void JavaStrToBIN32 (byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       int intVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to integer
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM , "The value supplied for the value variable can not be null");

       try {
          intVal = Integer.valueOf(value).intValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       JavaIntToBIN32 (buf, intVal, offset);
    }

    /**
     * Converts a Java String into a IEEE format unsigned integer and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted value
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, offset + 4 > buf.length, or
	 * the contents of value cannot be converted to an unsigned short.
     */

    public static final void JavaStrToUNBIN32 (byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       long longVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to long
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          longVal = Long.valueOf(value).longValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       JavaLongToUNBIN32 (buf, longVal, offset);
    }

    /**
     * Converts a Java String into a IEEE format long long and places the result
     * in the buffer.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted value
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, offset + 8 > buf.length, or
	 * the contents of value cannot be converted to a long long.
     */

    public static final void JavaStrToBIN64(byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       long longVal;
       // Convert the input String to long

       inputDataCheck ( buf, offset );

       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          longVal = Long.valueOf(value).longValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       JavaLongToBIN64 (buf, longVal, offset);
    }

    /**
     * Converts a Java String into a two byte ASCII string and places the result
     * in the buffer.  This corresponds to a LOGICAL 2 fields as described
	 * in the Data Definition Language Reference Manual.  Normally, such a
	 * field is used to hold a value which defines true or false, JavaShortToLogical2
	 * makes no attempt to verify the contents of value.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted value
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, value == null, value does not contain a numeric value,
	 * or offset + 2 > buf.length.
     */
    public static final void JavaStrToLogical2 (byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       short s;

       inputDataCheck ( buf, offset );

       // Convert the input String to short
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          s = Short.valueOf(value).shortValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       JavaShortToLogical2 (buf, s, offset);
    }

    /**
     * Converts a Java String into a four byte ASCII string and places the result
     * in the buffer.  This corresponds to a LOGICAL 4 field as
	 * described in the Data Definition Language Reference Manual.  Normally,
	 * such a field is used to store a value which can be defined as true
	 * or false. JavaIntToLogical4 makes no attempt to verify the contents of
	 * value.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset < 0, value == null, the contents of value are not numeric,
	 * or offset + 4 > buf.length.
     */
    public static final void JavaStrToLogical4 (byte[] buf, String value,
                        int offset) throws DataConversionException
    {
       int intVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to int
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          intVal = Integer.valueOf(value).intValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       JavaIntToLogical4 (buf, intVal, offset);
    }

    /**
     * Converts a Java String representing a signed bit string
     * into a short and places the result in the buffer. This corresponds to a BIT field as
	 * described in the Data Definition Language Reference
	 * Manual.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset > buf.length, offset < 0,
     * offset + 2 > buf.length, value == null, value does not contain the
     * representation of a number, or the number that value represents
     * is greater than 32767.
     */
    public static final void BITStrToTdmShort (byte[] buf, String value,
                         int offset) throws DataConversionException
    {
       int intVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to int
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          intVal = Integer.valueOf(value).intValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       BITToTdmShort (buf, intVal, offset);
    }

    /**
     * Converts a Java String representing an unsigned Java bit string into
     * a short and places the result in the buffer. This corresponds to an unsigned BIT field as described
	 * in the Data Definition Language Reference manual.
     *
     * @param buf the buffer
     * @param value the String to convert
     * @param offset the location within the buffer to place the
     * converted string
     *
     * @exception DataConversionException when there is an error converting,
     * if buf.length == 0, offset > buf.length, offset < 0,
	 * offset + 2 > buf.length, value == null, value does not contain the
	 * representation of a number, or the number that value represents
     * is greater than 65535.
     */
    public static final void UBITStrToTdmShort (byte[] buf, String value,
                         int offset) throws DataConversionException
    {
       int intVal;

       inputDataCheck ( buf, offset );

       // Convert the input String to int
       if (value == null)
          throw new DataConversionException (DataConversionException.ConvError.PARAM, "The value supplied for the value variable can not be null");

       try {
          intVal = Integer.valueOf(value).intValue();
       } catch (NumberFormatException ne) {
          throw new DataConversionException (DataConversionException.ConvError.SYNTAX, "Either value does not contain a parseable number or the value is out of range.");
       }

       UBITToTdmShort (buf, intVal, offset);
    }


	/* Begin changes for Solution Number: 10-060905-8795 */

	/**
	 * Converts a Java String representing a 32-bit IEEE float
	 * value into a 32-bit TNS float and places the result
	 * in the buffer.
	 *
	 * @param buf the buffer
	 * @param inString the String value to convert
	 * @param offset the location within the buffer to place the
	 * converted float
	 *
	 * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, inString == null, offset < 0, or the contents
	 * of inString cannot be converted into a float value.
	 */
	public static final void StrToTNSFloat32 (byte[] buf, String inString,
		int offset) throws DataConversionException
	{
	    loadcheck("StrToTNSFloat32()");  /* New Function added for the RFE solution 10-121009-4998*/
		inputDataCheck ( buf, inString, offset );

		// Check the inString format
		try 
		{
			Float f = new Float (inString);
			f = null;   //not use, release the memory
		} 
		catch (NumberFormatException ne) 
		{
			throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either inString does not contain a parseable number or the value is out of range.");
		}

		StrIEEEToTNSFloat32Native (buf, inString, offset);
	}
	private static final native void StrIEEEToTNSFloat32Native (byte[] buf, String inString,int offset);

	/**
	 * Converts a Java String representing a 64-bit IEEE Float
	 * value into a 64-bit TNS Float and places the result
	 * into the buffer.
	 *
	 * @param buf the buffer
	 * @param inString the String value to convert
	 * @param offset the location within the buffer to place the
	 * converted double
	 *
	 * @exception DataConversionException when there is an error converting,
	 * if buf.length == 0, inString == null, offset < 0, or the contents
	 * of inString cannot be converted into a double value.
	 */
	public static final void StrToTNSFloat64 (byte[] buf, String inString,
		int offset) throws DataConversionException
	{
	    loadcheck("StrToTNSFloat64()"); /* New Function added for the RFE solution 10-121009-4998*/
		inputDataCheck ( buf, inString, offset );

		// Check the inString format
		try 
		{
			Float f = new Float (inString);
			f = null;   //not use, release the memory
		} 
		catch (NumberFormatException ne) 
		{
			throw new DataConversionException(DataConversionException.ConvError.PARAM, "Either inString does not contain a parseable number or the value is out of range.");
		}

		StrIEEEToTNSFloat64Native (buf, inString, offset);
	}
	private static final native void StrIEEEToTNSFloat64Native (byte[] buf, String inString,int offset);

	/* End Changes for Solution Number: 10-060905-8795 */


   /* ---------------------------------------------------------*/
   /*-- CONVERSION METHODS FROM GUARDIAN TYPES TO JAVA TYPES --*/
   /* ---------------------------------------------------------*/

//  Begin Changes for Solution Number 10-040622-7210
    // The description for Param nInType has been added in the line 1315

    /**
     * Converts a fixed-length decimal numeric string with a specified
     * format and scale to a Java string which may contain a sign.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     * @param fieldSize the length of the value to extract
     * @param nInScale the scale to apply to the converted value
     * @param nInType One of: NUMSTR_TRAILING_EMBEDDED,
     *                        NUMSTR_TRAILING_SEPARATE,
     *                        NUMSTR_LEADING_EMBEDDED,
     *                        NUMSTR_LEADING_SEPARATE,
     *                        NUMSTR_UNSIGNED.
     * @see #NUMSTR_TRAILING_EMBEDDED
     * @see #NUMSTR_TRAILING_SEPARATE
     * @see #NUMSTR_LEADING_EMBEDDED
     * @see #NUMSTR_LEADING_SEPARATE
     * @see #NUMSTR_UNSIGNED
     *
     * @return the converted String; returns null if nInType is not one of:
     * NUMSTR_TRAILING_EMBEDDED, NUMSTR_TRAILING_SEPARATE,
     * NUMSTR_LEADING_EMBEDDED, NUMSTR_LEADING_SEPARATE, or NUMSTR_UNSIGNED
     * @exception DataConversionException if an error occurs converting
     * the data, if buf.length == 0, if offset < 0, if fieldSize < 0,
     * if nInScale < 0, if offset > buf.length, or  if offset + fieldSize > buf.length.
     */

// End Changes for Solution Number 10-040622-7210

    public static final String NumStrToJavaStr (byte[] buf, int offset,
        int fieldSize, int nInScale, int nInType) throws DataConversionException
    {
       byte embSign = (byte)'\0';
       byte [] byteArrayOut = new byte [256];
       int indexIn = offset, indexOut = 0;
       int nInSize = fieldSize;

       inputDataCheck (buf, offset, fieldSize, nInScale, nInType);

       switch (nInType)
       {
          case NUMSTR_LEADING_SEPARATE:
             if (fieldSize < 2)
               throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
             if (buf[offset] != '-' && buf[offset] != '+')
               throw new DataConversionException(DataConversionException.ConvError.PARAM, "NUMSTR_LEADING_SEPARATE case but the buffer does not contain neither a leading '-' nor '+'");
             if (buf[offset] == '-')
                byteArrayOut[indexOut++] = (byte) '-';
             indexIn++;
             nInSize--;
             break;

          case NUMSTR_TRAILING_SEPARATE:
             if (fieldSize < 2)
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
             if (buf[offset + fieldSize - 1] != '-' && buf[offset + fieldSize - 1] != '+')
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "NUMSTR_TRAILING_SEPARATE case but the buffer does not contain neither a trailing '-' nor '+' ");
             if (buf[offset + fieldSize - 1] == '-')
                byteArrayOut[indexOut++] = (byte) '-';
             nInSize--;
             break;

          case NUMSTR_LEADING_EMBEDDED:
             if (fieldSize < 1)
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
             if ((buf[offset] & 0x80) == 0x80)
             {
                byteArrayOut[indexOut++] = (byte) '-';
                buf[offset] &= ~ 0x80;
                embSign = buf[offset];
             }
             break;

          case NUMSTR_TRAILING_EMBEDDED:
             if (fieldSize < 1)
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
             if ((buf[offset + fieldSize - 1] &  0x80) == 0x80)
             {
                byteArrayOut[indexOut++] = (byte) '-';
                buf[offset + fieldSize - 1] &= ~ 0x80;
                embSign = buf[offset + fieldSize - 1];
             }
             break;

          case NUMSTR_UNSIGNED:
             if (fieldSize < 1)
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value of the fieldSize variable is less than expected");
             break;

          default:
                throw new DataConversionException(DataConversionException.ConvError.PARAM, "The input for the nInType variable is not supoorted");
       }

       if (nInScale >= nInSize)
       {
          /* The number has only a fraction part, and may require extra leading zeros */
          byteArrayOut[indexOut++] = (byte) '.';
          for (int i = 0; i<nInScale - nInSize; i++)
             byteArrayOut[indexOut++] = (byte) '0';
          for (int i = 0; i<nInSize; i++)
             byteArrayOut[indexOut++] = buf[indexIn++];
       }
       else
       if (nInScale > 0)
       {
          // The number has both an integer part and a fraction part.  Trim
          // the leading zeros from the integer part, leaving at least one digit
          while (nInSize > nInScale && (buf[indexIn] == '0'))
          {
             indexIn++;
             nInSize--;
          }

          // copy the integer part
          for (int i=0; i<nInSize - nInScale; i++)
             byteArrayOut[indexOut++] = buf[indexIn++];

          byteArrayOut[indexOut++] = (byte) '.';

          // copy the fraction part
          for (int i=0; i<nInScale; i++)
             byteArrayOut[indexOut++] = buf[indexIn++];
       }
       else
       {
          // nInScale is zero or negative case
          // The number has no fraction part and may require extra trailing
          // zeros to make the integer.
          // Trim leading zeros from the integer, leaving at least one digit.
          // Add trailing zeros if the number is not zero.


          while (nInSize > 1 && (buf[indexIn] == '0'))
          {
             indexIn++;
             nInSize--;
          }

          for (int i = 0; i<nInSize; i++)
             byteArrayOut[indexOut++] = buf[indexIn++];

         if (nInScale < 0 && (nInSize > 1 ||
                                        buf[indexIn] != '0') )

         {
             for (int i = 0; i<- nInScale; i++)
               byteArrayOut[indexOut++] = (byte) '0';
         }
       }

       if ( (embSign != 0 ) && byteArrayOut[0] == '-')
       {
          if (nInType == NUMSTR_LEADING_EMBEDDED)
                                    buf[offset] |= 0x80;
          else
             buf[offset +fieldSize - 1] |= 0x80;
       }

       String str = new String (byteArrayOut, 0, indexOut);

       return str.trim();

    }

    /**
     * Converts a null terminated string to a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     * @param fieldSize the length of the value to extract
     *
     * @return the Java String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * fieldSize < 0, or fieldSize + offset > buf.length.
     */
    public static final String CStrToJavaStr (byte[] buf, int offset,
                                 int fieldSize) throws DataConversionException
    {
       inputDataCheck (buf, offset, fieldSize);
	   /* Changes start for Soln. 10-130204-6166 */
	   String funstr1 = null;
       try{
          funstr1 = new String (buf, offset, fieldSize);		  		  
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
	   /* Checking for the validity of input string and whether a NULL character is present */
	   if( (funstr1 != null) && ((funstr1.indexOf("\0") != -1 )) ){
			String[] funstr2 = funstr1.split("\0");
			/* Soln 10-140827-3480 : Jtoolkit: DataConversion.CStrToJavaStr of empty
			 * string (starting with NULL char) fails : Exception in thread 'main'
			 * java.lang.ArrayIndexOutOfBoundsException: 0 at
			 * com.tandem.ext.util.DataConversion.CStrToJavaStr(DataConversion.java: 1890)
			 */
			if ( funstr2.length != 0 )
			funstr1 = funstr2[0];
	   }
	   return funstr1;
	   /* Changes end for Soln. 10-130204-6166 */
    }

    /**
     * Converts a null terminated string to a Java String using the
     * value of enc for character encoding.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     * @param fieldSize the length of the value to extract
     * @param enc the character-encoding name
     *
     * @return the converted Java String
     * @exception DataConversionException if an error occurs during
	 * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * fieldSize < 0, fieldSize + offset > buf.length, enc == null, or
     * enc does not contain a valid character-encoding name.
     */
    public static final String CStrToJavaStr (byte[] buf, int offset,
                      int fieldSize, String enc) throws DataConversionException
    {
       inputDataCheck (buf, offset, fieldSize);

       if (enc == null)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is null");
		  
       /* Changes start for Soln. 10-130204-6166 */
       String funstr1 = null;
       try{	  
           funstr1 = new String (buf, offset, fieldSize, enc);
	   }catch (UnsupportedEncodingException x) {
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is not supported");
       }
	   /* Checking for the validity of input string and whether a NULL character is present */
	   if( (funstr1 != null) &&  (funstr1.indexOf("\0") != -1)  ){
			String[] funstr2 = funstr1.split("\0");
			funstr1 = funstr2[0];
	   }
	   return funstr1;
	   /* Changes end for Soln. 10-130204-6166 */
    }

    /**
     * Converts a character string (non-null terminated) into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     * @param fieldSize the length of the value to extract
     *
     * @return the converted Java String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * fieldSize < 0, or fieldSize + offset > buf.length.
     */
    public static final String CobolStrToJavaStr (byte[] buf, int offset,
                                 int fieldSize) throws DataConversionException
    {
       inputDataCheck (buf, offset, fieldSize);

       try {
          return (new String ( buf, offset, fieldSize));
       }catch (Exception e) {
          throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
    }

    /**
     * Converts a character string (non-null terminated) into a Java String
     * using the value of enc for character encoding.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     * @param fieldSize the length of the value to extract
     * @param enc the character-encoding name
     *
     * @return the converted String
     * @exception DataConversionException if an error occurs during
	 * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * fieldSize < 0, fieldSize + offset > buf.length, enc == null,
     * or enc does not contain a supported encoding character set.
     */
    public static final String CobolStrToJavaStr (byte[] buf, int offset, int fieldSize, String enc) throws DataConversionException
    {
       inputDataCheck (buf, offset, fieldSize);

       if (enc == null)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is null");

       try {
          return (new String ( buf, offset, fieldSize, enc));
       }catch (UnsupportedEncodingException x) {
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the enc variable is not supported");
       }
    }

    /**
     * Converts a single byte containing a binary value into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * or offset + 1 > buf.length.
     */
    public static final String BIN8ToJavaStr (byte[] buf, int offset)
                                         throws DataConversionException
    {
       if (buf.length == 0)
         throw new DataConversionException(DataConversionException.ConvError.PARAM, "The buffer is empty");

       if (offset <0 || (offset + 1) > buf.length)
          throw new DataConversionException(DataConversionException.ConvError.PARAM, "The value supplied for the offset can not be less than zero or greater than the length of the buffer - 1");

       try {
          return ( new String ( buf, offset, 1) );
       }catch (Exception e) {
          throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
    }

    /**
     * Converts a signed short in IEEE format into a Java short
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted Java short
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0,or offset > buf.length.
     */
    public static final short BIN16ToJavaShort (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[2];

       if((offset + 2) > buf.length)
       	  throw new DataConversionException(DataConversionException.ConvError.RANGE,
       	  	"The length of the buffer after the offset is not large enough to contain a short value");

       try {
          for (int i = 0; i<2; i++)
          {
             b = new Byte(buf[offset + i]);
             intVal[i] = b.intValue();
          }
       }catch (Exception e) {
          throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
        return ( (short) ((intVal[1] & 0x00FF) | (intVal[0] << 8 )));
    }

    /**
     * Converts a unsigned short in IEEE format into a Java int
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted Java int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * or offset + 2 > buf.length.
     */
    public static final int UNBIN16ToJavaInt (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[2];

       if((offset + 2) > buf.length)
	          	  throw new DataConversionException(DataConversionException.ConvError.RANGE,
       	  	"The length of the buffer after the offset is not large enough to contain an unsigned short value");

       try {
         for (int i = 0; i<2; i++)
         {
            b = new Byte(buf[offset + i]);
            // covert to unsigned int
            intVal[i] = b.intValue() & 0xff;
         }
       }catch (Exception e) {
          throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
       return ((intVal[1] & 0x00FF) | (intVal[0] << 8 ));
    }

    /**
     * Converts a signed integer in IEEE format into a Java int
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted Java int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length,
     * or offset + 4 > buf.length.
     */
    public static final int BIN32ToJavaInt (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[4];

       if((offset + 4) > buf.length)
	      throw new DataConversionException(DataConversionException.ConvError.RANGE,
       	  	"The length of the buffer after the offset is not large enough to contain a int value");

       try {
          for (int i = 0; i<4; i++)
          {
             b = new Byte(buf[offset + i]);
             intVal[i] = b.intValue() & 0xff;
          }
       }catch (Exception e) {
          throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
        return ((intVal[3] & 0x00ff) | (intVal[2] << 8) |
                                 (intVal[1] << 16) |(intVal[0] << 24));
    }

    /**
     * Converts a unsigned integer in IEEE format into a Java long
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted Java long
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * or offset + 4 > buf.length.
     */
    public static final long UNBIN32ToJavaLong (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
        long [] intVal = new long[4];

       if((offset + 4) > buf.length)
	      throw new DataConversionException(DataConversionException.ConvError.RANGE,
       	  	"The length of the buffer after the offset is not large enough to contain a long value");

       try {
          for (int i = 0; i<4; i++)
          {
             b = new Byte(buf[offset + i]);
             intVal[i] = b.intValue() & 0xff;
          }
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }

        return ((intVal[3] & 0x00ff) | (intVal[2] << 8) |
                                 (intVal[1] << 16) |(intVal[0] << 24));
    }

    /**
     * Converts a 64 bit long long in IEEE format into a Java long
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted long
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, offset > buf.length,
     * or offset + 8 > buf.length.
     */
    public static final long BIN64ToJavaLong (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       long [] longVal = new long[8];

       if((offset + 8) > buf.length)
	          	  throw new DataConversionException(DataConversionException.ConvError.RANGE,
       	  	"The length of the buffer after the offset is not large enough to contain a long long value");

       try {
          for (int i = 0; i<8; i++)
          {
             b = new Byte(buf[offset + i]);
             longVal[i] = b.intValue() & 0xff;
          }
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
        return ((longVal[7] & 0x00ff) | (longVal[6] << 8) |
                (longVal[5] << 16) | (longVal[4] << 24) |
                (longVal[3] << 32) | (longVal[2] << 40) |
                (longVal[1] << 48) | (longVal[0] << 56));
    }

    /**
     * Converts a TNS float into a Java String representing a
     * TNS float.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String Float32ToStr(byte[] buf, int offset) throws DataConversionException
    {
	   loadcheck("Float32ToStr()"); /*New function added for the RFE Solution 10-121009-4998*/
       inputDataCheck (buf, offset);

       return Float32ToStrNative (buf, offset);
    }
    private static final native String Float32ToStrNative (byte[] buf, int offset);

    /**
     * Converts a TNS double into a Java String representing
     * TNS double.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted String
  	 * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */

    public static final String Float64ToStr (byte[] buf, int offset) throws DataConversionException
    {
	   loadcheck("Float64ToStr()"); /*New Function added for the RFE solution 10-121009-4998*/
       inputDataCheck (buf, offset);

       return Float64ToStrNative(buf, offset);
    }
    private static final native String Float64ToStrNative (byte[] buf, int offset);


    /**
     * Converts a Logical 1 field (see the Data Definition Language Reference Manual into a Java String. The
     * character corresponds to a Logical 1 field as described
     * in the Data Definition Language Reference Manual.
     *
     * @param offset the location in the reply buffer to extract the value
     *
     * @return the converted String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String Logical1ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       try {
          return ( new String ( buf, offset, 1) );
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
    }

    /**
     * Converts a Logical 2 field (see the Data Definition Language Reference Manual into a Java short.  The
     * two character bytes correspond to a Logical 2 field as described
     * in the Data Definition Language Reference Manual.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final short Logical2ToJavaShort (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[2];

       try {
          for (int i = 0; i<2; i++)
          {
             b = new Byte(buf[offset + i]);
             intVal[i] = b.intValue();
          }
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
        return ( (short) ((intVal[1] & 0x00FF) | (intVal[0] << 8 )));
    }

    /**
     * Converts a Logical 4 field (see the Data Definition Language Reference Manual) into a Java int.  The
     * character string corresponds to a Logical 4 field as described
     * in the Data Definition Language Reference Manual.
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final int Logical4ToJavaInt (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[4];

       try {
          for (int i = 0; i<4; i++)
          {
             b = new Byte(buf[offset + i]);
             intVal[i] = b.intValue() & 0xff;
          }
       }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
       return ((intVal[3] & 0x00ff) | (intVal[2] << 8) |
                                 (intVal[1] << 16) |(intVal[0] << 24));
    }

    /**
     * Converts a signed bit field in IEEE format into a Java int
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final int TdmShortToBIT (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[2];

       try {
          for (int i = 0; i<2; i++)
          {
             b = new Byte(buf[offset + i]);
             // covert to unsigned int
             intVal[i] = b.intValue() & 0xff;
          }
      }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
       return ((intVal[1] & 0x00FF) | (intVal[0] << 8 ));
    }

    /**
     * Converts a unsigned bit field in IEEE format into a Java int
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final int TdmShortToUBIT (byte[] buf, int offset)
					throws DataConversionException
    {
       inputDataCheck (buf, offset);

       Byte b;
       int [] intVal = new int[2];

       try {
          for (int i = 0; i<2; i++)
          {
             b = new Byte(buf[offset + i]);
             // covert to unsigned int
             intVal[i] = b.intValue() & 0xff;
          }
      }catch (Exception e) {
           throw new DataConversionException(DataConversionException.ConvError.SYNTAX,e.getMessage());
       }
       return ((intVal[1] & 0x00FF) | (intVal[0] << 8 ));
    }

    /**
     * Converts a signed short in IEEE format into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted short
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String BIN16ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        short s = BIN16ToJavaShort(buf, offset);
        return Short.toString(s);
    }

    /**
     * Converts a unsigned short in IEEE format into a Java int
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String UNBIN16ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        int inVal = UNBIN16ToJavaInt (buf, offset);
        return Integer.toString(inVal);
    }

    /**
     * Converts a signed integer in IEEE format into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String BIN32ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        int inVal = BIN32ToJavaInt (buf, offset);
        return Integer.toString(inVal);
    }

    /**
     * Converts an unsigned integer in IEEE format into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String UNBIN32ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        long longVal = UNBIN32ToJavaLong (buf, offset);
        return Long.toString(longVal);
    }

    /**
     * Converts a 64 bit long long in IEEE format  into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String BIN64ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        long longVal = BIN64ToJavaLong (buf, offset);
        return Long.toString(longVal);
    }

    /**
     * Converts a Logical 2 field (see the Data Definition Language Reference Manual)  into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted String
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String Logical2ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        short sVal = Logical2ToJavaShort (buf, offset);
        return Short.toString(sVal);
    }

    /**
     * Converts a Logical 4 field (see the Data Definition Language Reference Manual) into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String Logical4ToJavaStr (byte[] buf, int offset)
					throws DataConversionException
    {
        int intVal = Logical4ToJavaInt (buf, offset);
        return Integer.toString(intVal);
    }

    /**
     * Converts a signed bit field in IEEE format into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String TdmShortToBITStr (byte[] buf, int offset)
                                         throws DataConversionException
    {
        int inVal = TdmShortToBIT (buf, offset);
        return Integer.toString(inVal);
    }

    /**
     * Converts a unsigned bit field in IEEE format into a Java String
     *
     * @param buf the buffer which contains the data to be converted
     * @param offset the location in the buffer to extract the value
     *
     * @return the converted int
     * @exception DataConversionException if an error occurs during
     * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
     */
    public static final String TdmShortToUBITStr (byte[] buf, int offset)
                                               throws DataConversionException
    {
        int inVal = TdmShortToUBIT (buf, offset);
        return Integer.toString(inVal);
    }

	/* Begin changes for Solution Number: 10-060905-8795 */

	/**
	 * Converts a 32-bit TNS float into a Java String representing a 32-bit IEEE float.
	 *
	 * @param buf the buffer which contains the data to be converted
	 * @param offset the location in the buffer to extract the value
	 *
	 * @return the converted String
	 * @exception DataConversionException if an error occurs during
	 * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
	 */
	public static final String TNSFloat32ToStr(byte[] buf, int offset) throws DataConversionException
	{
	    loadcheck("TNSFloat32ToStr()");  /* New Function added for the RFE solution 10-121009-4998*/
		inputDataCheck (buf, offset);

		float f1 = Float32TNSToIEEENative (buf, offset);
		return String.valueOf(f1);
	}    
	private static final native float Float32TNSToIEEENative (byte[] buf, int offset);


	/**
	 * Converts a 64-bit TNS Float into a Java String representing 64-bit IEEE Float.
	 *
	 * @param buf the buffer which contains the data to be converted
	 * @param offset the location in the buffer to extract the value
	 *
	 * @return the converted String
	 * @exception DataConversionException if an error occurs during
	 * conversion, if buf.length == 0, offset < 0, or offset > buf.length.
	 */

	public static final String TNSFloat64ToStr (byte[] buf, int offset) throws DataConversionException
	{
	    loadcheck("TNSFloat64ToStr()"); /* New Function added for the RFE solution 10-121009-4998*/
		inputDataCheck (buf, offset);

		double f1 = Float64TNSToIEEENative(buf, offset);
		return String.valueOf(f1);
	}
	private static final native double Float64TNSToIEEENative (byte[] buf, int offset);
	
	/* End changes for Solution Number: 10-060905-8795 */   

}  /* end class DataConversion */
