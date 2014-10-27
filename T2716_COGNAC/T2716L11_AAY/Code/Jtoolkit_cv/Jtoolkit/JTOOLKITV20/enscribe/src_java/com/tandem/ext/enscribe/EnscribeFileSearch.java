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
import com.tandem.ext.enscribe.EnscribeFileException;
import com.tandem.ext.util.NativeLoader;

/**
 * <P>EnscribeFileSearch finds files that match
 * a particular file name pattern.  For more information, see the Guardian procedures
 * FILENAME_FINDSTART_, FILENAME_FINDNEXT_ and FILENAME_FINDFINISH_.</P>
 *
 * Refer to Appendix D of the Guardian Procedure Calls Reference Manual for
 * the syntax of file name patterns.
 */

public class EnscribeFileSearch
{
	static {	  
		NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();	
	 }
	private short searchid_ = 0;
	private boolean searchStarted_ = false;

	/**
	 * Same as RESOLVE_LEVEL_NODE_NAME.
	 */
	public static final short RESOLVE_LEVEL_DEFAULT = -1;
	/**
	 *  Node name (for example, node, volume, subvolume and file).
	 */
	public static final short RESOLVE_LEVEL_NODE_NAME = -1;
	/**
	 * Destination name (for example, volume, device, or process).
	 */
	public static final short RESOLVE_LEVEL_DESTINATION_NAME = 0;
	/**
	 * First qualifier (for example, subvolume).
	 */
	public static final short RESOLVE_LEVEL_FIRST_QUALIFIER = 1;
	/**
	 * Second qualifier (file identifier if disk file).
	 */
	public static final short RESOLVE_LEVEL_SECOND_QUALIFIER = 2;

	private short resolve_level_ = RESOLVE_LEVEL_NODE_NAME;

   /**
    * Construct an EnscribeFileSearch object.
    *
    */
	public void EnscribeFileSearch() {
	} // constructor

   /**
    * Initialize a search.
    *
    * @param searchPattern a String with a valid file name pattern that specifies
	* the set of names to be searched.
    * @param resolveLevel the resolve level, how much of the file name to return.
    * Indicates the part that will be the leftmost component of a returned name.
    *
    * @exception com.tandem.enscribe.EnscribeFileException if FILENAME_FINDSTART_ returns
    * an error or if searchPattern is in OSS format.
    *
    * @see #RESOLVE_LEVEL_NODE_NAME
    * @see #RESOLVE_LEVEL_DESTINATION_NAME
    * @see #RESOLVE_LEVEL_FIRST_QUALIFIER
    * @see #RESOLVE_LEVEL_SECOND_QUALIFIER
    * @see #searchStart(String , short , String ,  boolean )
    */
	public void searchStart(String searchPattern, short resolveLevel) throws EnscribeFileException{
		if(searchStarted_)
			searchFinish();
		if(searchPattern == null)
			throw new IllegalArgumentException("searchPattern cannot be null");
		if(searchPattern.indexOf("/") != -1)
			throw new EnscribeFileException("searchPattern must be in Guardian file name format",
				EnscribeFileException.FEBADFORM,"searchStart","UNDEFINED");
		short []
			errorArray = new short [2];
		searchid_ = GfilenameFindStart(searchPattern, resolveLevel,
									  "",
									  false,
									  false,
									  errorArray);

		if(errorArray[0] != 0)
			throw new EnscribeFileException("Search start error",
				errorArray[0],"searchStart","UNDEFINED");

		searchStarted_ = true;
	} // searchStart

   /**
    * Initialize a search starting with a particular file.
    *
    * @param searchPattern a String with a valid file name pattern that specifies
	* the set of names to be searched.
    * @param resolveLevel the resolve level, how much of the file name to return
    * @param startingFileName a String which specifies a file name that indicates where, within
    * the set of file names that meet the search criteria, selection should
    * begin.
    * @param skipStartingFileName a boolean. true: the first name to be returned is startname.
    * false: name is to be skipped and the following name should be the first to be returned.
    *
    * @exception com.tandem.enscribe.EnscribeFileException if FILENAME_FINDSTART_ returns
    * an error or if search pattern or file name are in OSS format.
    *
    * @see #RESOLVE_LEVEL_NODE_NAME
    * @see #RESOLVE_LEVEL_DESTINATION_NAME
    * @see #RESOLVE_LEVEL_FIRST_QUALIFIER
    * @see #RESOLVE_LEVEL_SECOND_QUALIFIER
    * @see #searchStart(String, short)
    */
	public void searchStart(String searchPattern, short resolveLevel,
							String startingFileName,
							boolean skipStartingFileName) throws EnscribeFileException{

		if(searchPattern == null)
			throw new IllegalArgumentException("searchPattern cannot be null");
		if(startingFileName == null)
			throw new IllegalArgumentException("startingFileName cannot be null");
		if(searchStarted_)
			searchFinish();
		if(searchPattern.indexOf("/") != -1)
			throw new EnscribeFileException("searchPattern must be in Guardian file name format",
				EnscribeFileException.FEBADFORM,"searchStart","UNDEFINED");
		if(startingFileName.indexOf("/") != -1)
			throw new EnscribeFileException("startingFileName must be in Guardian file name format",
				EnscribeFileException.FEBADFORM,"searchStart","UNDEFINED");
		short []
			errorArray = new short [2];
		searchid_ = GfilenameFindStart(searchPattern, resolveLevel,
									  startingFileName,
									  true,
									  skipStartingFileName,
									  errorArray);

		if(errorArray[0] != 0)
			throw new EnscribeFileException("Search start error",
				errorArray[0],"searchStart","UNDEFINED");

		searchStarted_ = true;
	} // searchStart

   /**
    * Returns the next file name.
    *
    * @return String next file name
    *
    * @exception com.tandem.enscribe.EnscribeFileException if FILENAME_FINDNEXT_ returns an error.
    */
	public String findNext() throws EnscribeFileException{
		if(!searchStarted_)
			throw new EnscribeFileException("Search not started",
				EnscribeFileException.FEINVALOP,"findNext","UNDEFINED");
		short []
			errorArray = new short [2];
		String
			fileName = GfilenameFindNext(searchid_,
										 errorArray);
		if(errorArray[0] == (short)1)
			throw new EnscribeFileException("End of search",
				(short)1,"findNext","UNDEFINED");
		if(errorArray[0] != 0)
			throw new EnscribeFileException("Search findNext error",
				errorArray[0],"findNext","UNDEFINED");
		return fileName;
	} // findNext

   /**
    * Releases memory used for FILENAME_FINDxxxx
    *
    */
	private void searchFinish() {
		if(searchStarted_)
			GfilenameFindFinish(searchid_);
		searchStarted_ = false;
	} // searchFinish

   //---------------------------------------------
   //---------------------------------------------
   private final native synchronized
	void GfilenameFindFinish(/*in*/ short searchid);

	// returns String of filename
   private final native synchronized
	String GfilenameFindNext(/*in*/ short searchid,
				 /*out*/ short[] errorArray);

	// returns searchid
   private final native synchronized
	short GfilenameFindStart(/*in*/ String searchpattern,
					/*in*/ short resolveLevel,
					/*in*/ String startingFileName,
					/*in*/ boolean useStartingFileName,
					/*in*/ boolean skipStartingFileName,
					/*out*/ short[] errorArray);


} // EnscribeFileSearch
