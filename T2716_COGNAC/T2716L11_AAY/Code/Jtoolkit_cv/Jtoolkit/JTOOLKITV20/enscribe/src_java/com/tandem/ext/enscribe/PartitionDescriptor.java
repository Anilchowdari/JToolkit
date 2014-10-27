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

import java.util.Vector;

/**
 * <P>PartitionDescriptor contains methods that set attributes describing
 * detailed partition information.  These attributes include the name of
 * the secondary partition volume, the primary extent size, the secondary
 * extent size, and for key sequenced files, the partial key value.</P>
 *
 *
 * <P>Refer to the description of the FILE_CREATELIST_ procedure or the
 * FILE_ALTERLIST_ procedure (both documented in the Guardian
 * Procedure Calls Reference Manual) for more information about partition descriptor
 * attributes and their values.</P>
 * @see com.tandem.ext.enscribe.EnscribeCreationAttributes
 */
public class PartitionDescriptor
{
	private long   primaryExtentSize_ = 4;
	private long   secondaryExtentSize_ = 4;
	private String	volumeName_ = "";
	private boolean partialKeySet_ = false;
	private Vector	partialKeyValue_ = new Vector(32,32);


	/**
    * Construct a PartitionDescriptor object
    *
    */
    public PartitionDescriptor() {
    } // PartitionDescriptor

   /**
    * Sets the primary extent size. Corresponds to the primary
    * extent described in item 97 of FILE_CREATELIST_.  If this
    * PartitionDescriptor is being used to create a file, this
    * attribute is REQUIRED.
    *
    * @param extSize number of pages for the primary extent.
    * Must be greater than zero.
    */
    public void setPrimaryExtentSize(long extSize) {
        primaryExtentSize_ = extSize;
    } // setPrimaryExtentSize

   /**
    * Returns the number of pages for the primary extent.
    *
    * @return number of pages for the primary extent
    */
    public long getPrimaryExtentSize() {
        return primaryExtentSize_;
    } // getPrimaryExtentSize

   /**
    * Sets the secondary extent size.  Corresponds to the secondary
    * extent described in item 97 of FILE_CREATELIST_.  If this
    * PartitionDescriptor is being used to create a file, this
    * attribute is REQUIRED.
    *
    * @param extSize number of pages for the secondary extent.
    * Must be greater than zero.
    */
    public void setSecondaryExtentSize(long extSize) {
        secondaryExtentSize_ = extSize;
    } // setSecondaryExtentSize

   /**
    * Returns the number of pages for the secondary extent.
    *
    * @return number of pages for the secondary extent
    */
    public long getSecondaryExtentSize() {
        return secondaryExtentSize_;
    } // getSecondaryExtentSize

   /**
    * Sets the volume name for this partition. Corresponds to a single
    * partition volume name as described in item 93 of FILE_CREATELIST_.
    * If this PartitionDescriptor is being used to create a file, this
    * attribute is REQUIRED.
    *
    * @param vol a String with the volume name for this partition; vol
    * must not include a subvolume name.
    */
    public void setVolumeName(String vol) {
        volumeName_ = vol;
    } // setVolumeName

   /**
    * Returns the volume name specified for this partition.
    *
    * @return volume name specified for this partition; returns null if
    * no volume name was specified.
    */
    public String getVolumeName() {
        return volumeName_;
    } // getVolumeName

   /**
    * Clears the partial key value.
    *
    */
    public void clearPartialKeyValue() {
        partialKeySet_ = false;
    } // clearPartialKeyValue

   /**
    * Sets the partial key value for this partition. Corresponds to a
    * single partial key value as specified in item 94 of FILE_CREATELIST_.
    * If this PartitionDescriptor is being used to create a key-sequenced
    * file, this attribute is REQUIRED.
    *
    * @param keyValue a String with the partial key value for this partition
    */
    public void setPartialKeyValue(String keyValue) {
		if(keyValue == null)
			throw new IllegalArgumentException("keyValue cannot be null");
		byte [] keyBytes = keyValue.getBytes();
		partialKeySet_ = true;
		partialKeyValue_.removeAllElements();
		int i;
		for(i=0;i<keyBytes.length;i++)
			partialKeyValue_.addElement(new Integer(keyBytes[i]));
    } // setPartialKeyValue

    /**
	 * Sets the partial key value for this partition. Corresponds to a
	 * single partial key value as specified in item 94 of FILE_CREATELIST_.
	 * If this PartitionDescriptor is being used to create a key-sequenced
	 * file, this attribute is REQUIRED.
	 *
	 * @param keyValue a String with the partial key value for this partition
	 */
	 public void setPartialKeyValue(byte [] keyValue) {
		partialKeySet_ = true;
		partialKeyValue_.removeAllElements();
		int i;
		for(i=0;i<keyValue.length;i++)
			partialKeyValue_.addElement(new Integer(keyValue[i]));
    } // setPartialKeyValue

   /**
    * Returns the partial key value for this partition.
    *
    * @return byte array of the partial key value for this partition
    */
    public String getPartialKeyValue() {
		byte []
			retval = new byte[partialKeyValue_.size()];
		int
			i;
		Integer
			ea;
		for(i=0;i<partialKeyValue_.size();i++)
		{
			ea = (Integer)partialKeyValue_.elementAt(i);
			retval[i] = (byte)ea.intValue();
		}

		return new String(retval);
    } // getPartialKeyValue
	/**
    * Returns the partial key value for this partition.
    *
    * @return byte array of the partial key value for this partition
    */
    byte [] getPartialKeyBytesValue() {
		byte []
			retval = new byte[partialKeyValue_.size()];
		int
			i;
		Integer
			ea;
		for(i=0;i<partialKeyValue_.size();i++)
		{
			ea = (Integer)partialKeyValue_.elementAt(i);
			retval[i] = (byte)ea.intValue();
		}

		return retval;
    } // getPartialKeyValue
}
