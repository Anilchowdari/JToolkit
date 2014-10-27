/**
 * @author Gowri Shankar
 * Copyright 2009 
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Compaq Computer
 * Corporation and successor entities such as Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.
 */
package com.tandem.ext.enscribe;

import com.tandem.ext.util.NativeLoader;


/**
 * <P>
 * FileNameConverter helps convert a partially qualified guardian filename to 
 * fully qualified guardian filename. It also helps convert a C-series internal filename
 * format to D-series filename format. 
 * 
 * </P>
 */

public class FileNameConverter {

    static {
        	 NativeLoader loader = new NativeLoader();
		loader.LoadLibrary();   
	 }
	
        /**
         * Specifies the maximum length of the filename.
         * 
         * @see Guardian Procedure Calls Manual for more information about the
         * length of the filenames provided to FILENAME_SCAN_ and FILENAME_RESOLVE_. 
         */
    private static final short MAX_LENGTH = 35;
        
        /**
         * Specifies the maximum length of the C-series filename.
         * 
         * @see Guardian Procedure Calls Manual for more information about the
         * length of the filenames provided to OLDFILENAME_TO_FILENAME_
         */
    private static final short CSeries_LENGTH = 24;
    
        /**
         * Private Constructor to suppress instantiation.  
         */
    private FileNameConverter() { 
	
    }
        
	/**
	 * Converts a partially qualified guardian filename to fully qualified 
	 * guardian filename.
	 *  
	 * @see	'Guardian Procedure Calls Manual' for FILENAME_SCAN_ and FILENAME_RESOLVE_.
	 * 
	 * @param partialFileName the partially qualified filename (relative path) 
	 * that is to be converted to its fully qualified (absolute) filename.
	 * 
	 * @exception EnscribeFileException
	 *                if a file-system error occurs or if the filename specified by 
	 *                partialFileName is invalid.
	 * @exception IllegalArgumentException
	 *                if partialFileName is NULL.
	 * 
	 * <pre>
	 * Sample input/output Format:
	 * --------------------------
         * 1. partialFileName          : FILENAME
	 *    Fully Qualified FileName : \NODE-NAME.$VOLUME.SUBVOLUME.FILENAME
	 *  
	 * 2. partialFileName          : SUBVOLUME.FILENAME
	 *    Fully Qualified FileName : \NODE-NAME.$VOLUME.SUBVOLUME.FILENAME
	 *   
	 * 3. partialFileName          : $VOLUME.SUBVOLUME.FILENAME
	 *    Fully Qualified FileName : \NODE-NAME.$VOLUME.$SUBVOLUME.FILENAME
	 *  
	 *    
	 * Examples:
	 * ---------
	 * 1. partialFileName          : jtoolkit
	 *    Fully Qualified FileName : \DMR03.$SYSTEM.SYSTEM.JTOOLKIT
	 *  
	 * 2. partialFileName          : SYSTEM.jtk
	 *    Fully Qualified FileName : \DMR03.$SYSTEM.SYSTEM.JTK
	 *  
	 * 3. partialFileName          : $SYSTEM.SYSTEM.jtk
	 *    Fully Qualified FileName : \DMR03.$SYSTEM.SYSTEM.JTK
	 * 
	 * </pre>
	 */
    public static String toFullyQualifiedFileName (byte[] partialFileName) 
    	throws EnscribeFileException, IllegalArgumentException {
				
        if (partialFileName == null)
            throw new IllegalArgumentException("Filename cannot be null");
        
        else if (partialFileName.length > MAX_LENGTH)
        	 throw new EnscribeFileException("Illegal Filename",
                    EnscribeFileException.FEErrorUndefined, "toFullyQualifiedFileName",
                    "UNDEFINED");
        
        byte[] fullName = new byte[MAX_LENGTH];
        int ret = jniToFullyQualifiedFileName(partialFileName, fullName);

        if (ret != 0)
            throw new EnscribeFileException("File system error : " + ret,
                    EnscribeFileException.FEErrorUndefined, "toFullyQualifiedFileName",
                    "UNDEFINED");
		
        return new String(fullName);
    }

	/**
	 * Converts a C-series internal filename format to 
	 * D-series filename format. 
	 * 
	 * @see	'Guardian Procedure Calls Manual' for OLDFILENAME_TO_FILENAME_. 
	 * 
	 * @param oldFileName the filename in C-series internal format that is to be 
	 * converted to its D-series format.
	 * 
	 * @exception EnscribeFileException
	 *                if a file-system error occurs or if the filename specified by 
	 *                oldFileName is invalid.
	 * @exception IllegalArgumentException
	 *                if oldFileName is NULL.
	 *  
	 *  <pre>
	 *  Sample input/output Format:
	 *  --------------------------
	 *  1. oldFileName      : "$VOLUME SUBVOL FILE"    
	 *     DSeries FileName : $VOLUME.SUBVOL.MYFILE
	 *     
	 *  2. oldFileName      : "\sysnumVOLUMESUBVOL MYFILE"
	 *     DSeries FileName : "system-name.$VOLUME.SUBVOL.MYFILE"
	 *   
	 *     
	 *  Examples:
	 *  ---------
	 *  1. oldFileName      : "$SYSTEM SYSTEM  jtk     "
	 *     DSeries FileName : \DMR03.$SYSTEM.SYSTEM.jtk
	 *     
	 *  2. oldFileName      : "\59SYSTEMsys00   osimage "
	 *     DSeries FileName : \DMR03.$SYSTEM.SYS00.OSIMAGE
	 *   </pre>     
	 */
    public static String toDSeriesFileName (byte[] oldFileName)
            throws EnscribeFileException, IllegalArgumentException {
		
        if (oldFileName == null)
            throw new IllegalArgumentException("Filename cannot be null");
        
        else if (oldFileName.length != CSeries_LENGTH)
        	throw new EnscribeFileException("Illegal Filename",
                    EnscribeFileException.FEErrorUndefined, "toDSeriesFileName",
                    "UNDEFINED");

        byte[] fName = new byte[MAX_LENGTH];
        int ret = jniToDSeriesFileName(oldFileName, fName);

        if (ret != 0)
            throw new EnscribeFileException("File system error : " + ret,
                    EnscribeFileException.FEErrorUndefined, "toDSeriesFileName",
                    "UNDEFINED");
		
        return new String(fName);
	}

	/**
	 * 
	 * Native methods : 
	 * jniToFullyQualifiedFileName - To expand the partially qualified filename to fully qualified filename. 
	 * jniToDSeriesFileName - To convert the C-series internal format filename to D-series format.
	 * 
	 */

	// returns error
    private final static native int jniToFullyQualifiedFileName(byte[] partialFileName, byte[] fullFileName);

	// returns error
    private final static native int jniToDSeriesFileName(byte[] oldFilename, byte[] fileName);

}
