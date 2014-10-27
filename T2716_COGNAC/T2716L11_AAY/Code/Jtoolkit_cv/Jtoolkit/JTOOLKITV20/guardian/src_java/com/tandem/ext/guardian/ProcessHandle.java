/* ------------------------------------------------------------------------
* Filename:     ProcessPhandle.java
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

package com.tandem.ext.guardian;

import com.tandem.ext.util.NativeLoader;

/**
 * The <code>ProcessPhandle</code> class represents a Guardian process handle.
 * This class can be used to decompose the phandle into its individual component
 * parts.  A ProcessPhandle does not necessarily represent a running process.
 *
 * @version 1.0, 9/6/2001
 */
public final class ProcessHandle {

   /* load library of native methods */
   static { 
   	NativeLoader loader = new NativeLoader();
	loader.LoadLibrary();
      
   }

   private static final short[] NullPH_ = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
   private static final ProcessHandle NullPhandle_ = new ProcessHandle(NullPH_);
   private static final int PHANDLE_SIZE = 10;

   /* the phandle in native format */
   private short[] phandle_ = null; 
   /* process string of the phandle */
   private String phandle_string_ = null;
   /* the individual components of a phandle */
   private short cpu_ = -1;
   private short pin_ = -1;
   private int nodeNumber_ = -1;
   private String nodeName_ = null;
   private String processName_ = null;
   private long sequenceNumber_ = -1;

   /* hashCode method has to be implemented because equals is implemented and the
      equals contract says that equal objects must return the same hashCode.
      We cache the hashCode to prevent having to calculate it more than once. */
   private volatile int hashCode_ = 0;


   /**
    * Constructs a ProcessPhandle from an array of 10 shorts.
    *
    * @param phandle a process handle
    */
   public ProcessHandle (short[] phandle) {
      if (phandle == null) throw new IllegalArgumentException(
                  "ProcessHandle must not be constructed with null array");
      if (phandle.length != PHANDLE_SIZE) throw new IllegalArgumentException(
                  "phandle must be array of " + PHANDLE_SIZE + " shorts");
      // We just store the phandle array for now, it gets decomposed into its
      // component parts the first time one of the get... methods is invoked
      phandle_ = phandle;
   } /* end ctor */

   /**
    * Call this to determine if the ProcessHandle represents a named process.
    * If the process is no longer running the name may not be able to be
    * determined and will appear unnamed.
    *
    * @return true if the ProcessHandle represents a named process
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public boolean isNamed() throws GuardianException {
      if (processName_ == null) {
         decompose(phandle_);
      }
      return (!processName_.equals(""));
   } /* end isNamed */

   /**
    * Call this to determine if the ProcessHandle represents a null process
    * handle.
    *
    * @return true if the ProcessHandle represents a null process handle
    * (-1 in each word).
    */
   public boolean isNull() {
      return this.equals(NullPhandle_);
   } /* end isNull */

   /**
    * Indicates whether two process handles are identical.  Two
    * process handles representing different processes of the same process
    * pair are <strong>not</strong considered identical.
    *
    * @param   obj   the ProcessHandle object with which to compare.
    * @return  <code>true</code> if each element of the process handles have
    * the same value; <code>false</code> otherwise.
    */
   public boolean equals(Object o) {
      if (o == this) return true;
      
      if (!(o instanceof ProcessHandle)) return false;

      ProcessHandle ph = (ProcessHandle)o;

      // JNI is expensive so rather than call the Guardian proc call
      // we just compare each element of the array
      for (int i=0; i<PHANDLE_SIZE; i++) {
         if (phandle_[i] != ph.phandle_[i]) return false;
      }

      return true;
   } /* end equals */
   
   /**
    * Returns a hash code value for the object. This method is
    * supported for the benefit of hashtables such as those provided by
    * <code>java.util.Hashtable</code>.
    *
    * @return  a hash code value for this object.
    * @see     #equals(java.lang.Object)
    */
   public int hashCode() {
      if (hashCode_ == 0) {
         int result = 17;
       
         for (int i=0; i< PHANDLE_SIZE; i++) {
            result = 37*result + (int)phandle_[i];
         }
         hashCode_ = result;
      }
      return hashCode_;
   } /* end hashCode */

   /**
    * Returns the process handle in its native form.
    *
    * @return an array of shorts representing the process handle
    */
   public short[] getPhandleArray() {
      return phandle_;
   }

   /**
    * Returns the CPU number in which the process is or was running.
    *
    * @return the CPU number
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public short getCPU() throws GuardianException {
      if (cpu_ == -1) {
         decompose(phandle_);
      }
      return cpu_;
   }

   /**
    * Returns process identification number of the process.
    *
    * @return the pin of the process
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public short getPin() throws GuardianException {
      if (pin_ == -1) {
         decompose(phandle_);
      }
      return pin_;
   }

   /**
    * Returns the node number of the node where the process resides.
    *
    * @return the node number
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public int getNodeNumber() throws GuardianException {
      if (nodeNumber_ == -1) {
         decompose(phandle_);
      }
      return nodeNumber_;
   }

   /**
    * Returns the name of the node where the process resides.
    *
    * @return the node name
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public String getNodeName() throws GuardianException {
      if (nodeName_ == null) {
         decompose(phandle_);
      }
      return nodeName_;
   }

   /**
    * Returns the process name of the process.  If the process is unnamed
    * an empty string will be returned.  If a named process is no longer running
    * an empty string may be returned.
    *
    * @return process name of the process
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public String getProcessName() throws GuardianException {
      if (processName_ == null) {
         decompose(phandle_);
      }
      return processName_;
   }

   /**
    * Returns sequence identification number of the process.
    *
    * @return sequence identification number of the process handle 
    * @exception GuardianException if the underlying Guardian process call
    * fails with an error.
    */
   public long getSequenceNumber() throws GuardianException {
      if (sequenceNumber_ == -1) {
         decompose(phandle_);
      }
      return sequenceNumber_;
   }

   /**
    * Returns a ProcessHandle representing the current process.
    *
    * @return the process handle of the current process 
    */
   public static ProcessHandle getMine() {
      // call native ProcessHandle_GetMine_ and pass the phandle to
      // the ctor and return what was constructed
      return new ProcessHandle(nativeGetMine());
   } /* end getMine */

   /**
    * Returns a string of characters that identifies the process.
    * The string in one of the following forms:  <pre>
    * [\ node.]cpu,pin
    * [\ node.]$process-name  </pre>
    * If the process-name is unavailable or the process is not named the process
    * string will be in the cpu,pin form.  The string will be the return value
    * of {@link Object#toString} if the call to PROCESSHANDLE_TO_STRING_ fails.
    *
    * @return equivalent process string
    */
   public String toString() {
      if (phandle_string_ == null) {
         phandle_string_ = getString(phandle_);
      }
      if (phandle_string_.length() == 0) {
         phandle_string_ = super.toString();
      }
      return phandle_string_;
   }

   // native methods
   private native static short[] nativeGetMine();
   private native void decompose(short[] phandle);
   private native String getString(short[] phandle);
}  /* end class ProcessPhandle */
