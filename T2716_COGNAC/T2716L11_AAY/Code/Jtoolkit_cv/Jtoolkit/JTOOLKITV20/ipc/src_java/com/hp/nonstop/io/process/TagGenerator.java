package com.hp.nonstop.io.process;

/**
 * An interface to generate tags
 * 
 * @author Suveer Nagendra
 * 
 */

public interface TagGenerator {
	/**
	 * Returns the tag to be used for write operation. The method is expected to
	 * return positive numbers. If the method returns a negative value or zero,
	 * the value will be ignored, and randomly generated tag will be used for
	 * the write
	 * 
	 * @return tag used for write operation
	 */
	public int getNextTag();
}
