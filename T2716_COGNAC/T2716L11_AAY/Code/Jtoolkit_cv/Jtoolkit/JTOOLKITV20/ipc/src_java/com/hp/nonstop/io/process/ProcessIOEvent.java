package com.hp.nonstop.io.process;

/**
 * Class representation of the IO Event. The event identifies the operation that
 * triggered the event generation.<br/>
 * The following are the IO operations that can trigger an IO Event
 * <ul>
 * <li>Before flush: The event is generated just before a message is written to
 * the <i>Server</i>.</li>
 * <li>After flush: The event is triggered upon a successful write to the
 * <i>Server</i>.</li>
 * <li>Response received: The event is triggered when a response is received
 * from the <i>Server</i>. Any response from previous messages will be
 * overwritten with the new response, immediately after processing this event.</li>
 * <li>Data ready: The event is triggered when a new response is received from
 * the <i>Server</i>, and it is ready to be read by the <i>Requester</i>.</li>
 * </ul>
 * 
 * Every message is associated with a message tag. The message tag can be used
 * by the Requester application can to associate a response with the request.
 * 
 * @author Vyom Tewari
 * 
 */
class ProcessIOEvent {
	/**
	 * Operation returns the operation performed on the Process class. operation
	 * was either a read or write.
	 */
	int eventID;

	/**
	 * Status returns the status code of the operation performed on the Process
	 * class.
	 */
	int messageTag;

	/**
	 * The time, in milliseconds, when this event occurred.
	 */
	long fireTime;

	/**
	 * "Before Write" event ID
	 */
	public static final int BEFORE_FLUSH = 0x00A;

	/**
	 * "Write Completed" event ID
	 */
	public static final int AFTER_FLUSH = 0x00B;

	/**
	 * "Response Received" event ID
	 */
	public static final int RESPONSE_RECEIVED = 0x00C;

	/**
	 * "data ready" event ID
	 */
	public static final int DATA_READY = 0x00D;

	/**
	 * 
	 * @param pid
	 * @param eventID
	 * @param messageTag
	 * @param fireTime
	 */
	ProcessIOEvent(ProcessIdentifier pid, int eventID, int messageTag,
			long fireTime) {

	}

	/**
	 * 
	 * 
	 * @return The Event ID of the operation of this event.
	 */
	public int getEventID() {
		return eventID;
	}

	/**
	 * 
	 * 
	 * @return The message tag associated with the IO operation.
	 */
	public int getMessageTag() {
		return messageTag;
	}

	/**
	 * Returns the time, in milliseconds, when this event occurred.
	 * 
	 * @return time - when the event occurred.
	 */
	public long getFireTime() {
		return fireTime;
	}

	/**
	 * 
	 * @return the Process object whose IO operation triggered this event
	 */
	public Process getProcessIdentifier() {
		return null;
	}
}
