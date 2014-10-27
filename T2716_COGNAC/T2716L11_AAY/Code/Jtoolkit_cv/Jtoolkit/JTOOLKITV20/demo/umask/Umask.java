/*---------------------------------------------------------------------------
 * Copyright 2010
 * Hewlett-Packard Development Company, L.P.
 * Protected as an unpublished work.
 * All rights reserved.
 *
 * The computer program listings, specifications and
 * documentation herein are the property of Hewlett-Packard
 * Development Company, L.P., or a third party supplier and
 * shall not be reproduced, copied, disclosed, or used in whole
 * or in part for any reason without the prior express written
 * permission of Hewlett-Packard Development Company, L.P.

 *-------------------------------------------------------------------------*/

/**
 * Umask Class provides utility methods for setting and getting
 *  file mode creation mask of the current process.
 * @version 1.0 
 */
public class Umask {
    static {
        System.loadLibrary("umask");
    }
    /** set user id on execution */
    public static final int S_ISUID = 04000; 

    /** set group id on execution */
    public static final int S_ISGID = 02000; 

    /** read permission: owner */
    public static final int S_IRUSR = 00400; 

    /** write permission: owner */
    public static final int S_IWUSR = 00200; 

    /** execute permission: owner */
    public static final int S_IXUSR = 00100; 

    /** rwx mask for owner class */
    public static final int S_IRWXU = (S_IRUSR | S_IWUSR | S_IXUSR);

    /** read permission: group */
    public static final int S_IRGRP = 00040; 

    /** write permission: group */
    public static final int S_IWGRP = 00020; 

    /** execute permission: group */
    public static final int S_IXGRP = 00010;

    /** rwx mask for group class */
    public static final int S_IRWXG = (S_IRGRP | S_IWGRP | S_IXGRP);

    /** read permission: other */
    public static final int S_IROTH = 00004;

    /** write permission: other */
    public static final int S_IWOTH = 00002;

    /** execute permission: other */
    public static final int S_IXOTH = 00001; 

    /** rwx mask for other class */
    public static final int S_IRWXO = (S_IROTH | S_IWOTH | S_IXOTH); 
    
    private Umask(){
    }

    /**
     * Sets the file mode creation mask of the current process.
     * @param mode_t  specifies the umask value to be set    
     */
    public static void setUmask(int mode_t){
        if(isValidUmask(mode_t))
        umaskSet(mode_t);        
    }
    
    /**
     * Gets the file mode creation mask of the current process.
     * @return file mode creation mask value
     */
    public static int getUmask(){
        int oldUmask = 0;
        oldUmask = umaskGet();
        return oldUmask;
    }
    
    /**
     * Validates mode_t value is valid octal value. 
     * @param mode_t
     * @returns true if the mode_t is valid octal value,else false.
     */
    private static boolean isValidUmask(int mode_t){
        for(int count = 0;count <= String.valueOf(mode_t).length();count ++){
            if((mode_t % 10)>7){
                System.out.println("Bad number");
                return false;
            }else{
                mode_t = mode_t / 10;                
            }
        }return true;
    }
    private native static void umaskSet(int mode);
    private native static  int umaskGet();
}
