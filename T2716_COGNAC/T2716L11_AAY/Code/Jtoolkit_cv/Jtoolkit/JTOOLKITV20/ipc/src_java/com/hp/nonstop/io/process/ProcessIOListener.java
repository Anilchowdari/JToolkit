package com.hp.nonstop.io.process;

/**
 * 
 * The listener for receiving process IO events. The class that is interested in
 * processing an IO event implements this interface, and the object created with
 * that class is registered with the Process object using its addIOListener
 * method. When an IO event occurs, that object's onIOEvent method is invoked.
 * 
 * @author Vyom Tewari
 */
interface ProcessIOListener {

	/**
	 * Invoked when an IO Event occurs
	 */
	public void onIOEvent(ProcessIOEvent e);
}
