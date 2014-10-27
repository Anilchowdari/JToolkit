package com.hp.nonstop.io.process.impl;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.hp.nonstop.io.process.ProcessIdentifier;
import com.hp.nonstop.io.process.ProcessOpenOption;
import com.hp.nonstop.io.process.StringManager;

// This class is supposed to be singleton across the entire JVM
public final class NSProcessChannelHelper {

	// This is used to indicate that the channelHelper has to shutdown and stop
	// any threads it has spawned
	volatile boolean shutdown = false;
	private static NSProcessChannelHelper helper;
	private static final NSProcessChannel channel = new NSProcessChannel();

	// This service is going to start a single thread that will
	private static Thread readanythread;

	// The use of a List is intentional. There are places where we need to
	// extract an object from the list
	// so positional access is required. this is not available in a set.
	// This collection contains list of objects for which read is initiated.
	private List readinitiatedlist;

	// This collection contains responses for which either no listeners are
	// registered
	// or read has not yet been initiated
	// Such responses are put in this data structure
	private List responseobtainedlist;

	// This collection contains all the listeners registered
	// private Map listeners;

	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.impl.NSProcessChannelHelper");

	protected static StringManager sm = StringManager.getManager("resources");

	// The private modifier is to ensure that an instance of this
	// object is obtained only through the static method
	private NSProcessChannelHelper() {
		responseobtainedlist = new ArrayList();
		readinitiatedlist = new ArrayList();
		listenOnAnyIOCompletion();
	}

	// TODO
	// There are better ways to implement other than making the entire method
	// synchronized
	// Search javaworld.net and take the best approach
	// Also ensure that this method always returns the same object irrespective
	// of which class loader
	// has loaded this class. Its essential (absolutely) to have only one
	// instance of this per JVM
	public static synchronized NSProcessChannelHelper getInstance() {
		if (helper == null) {
			try {
				/* Changes Begin for sol_10-131008-9186 
				Class cls = getClass("com.hp.nonstop.io.process.impl.NSProcessChannelHelper"); */
				Class cls = Class.forName("com.hp.nonstop.io.process.impl.NSProcessChannelHelper");
				/* Changes End for sol_10-131008-9186 */
				
				helper = (NSProcessChannelHelper) cls.newInstance();
				if (logger.isLoggable(Level.FINE)) {
					StringBuffer buf = new StringBuffer();
					buf.append("A new channel helper is created:").append(
							helper);
					logger.fine(buf.toString());
				}
			} catch (ClassNotFoundException e) {
				return null;
			} catch (InstantiationException ie) {
				return null;
			} catch (IllegalAccessException ia) {
				return null;
			}
		} else {
			return helper;
		}
		return helper;
	}

	/*  
	     getClass() method  is completley removed for the sol_10-131008-9186 
	
	private static Class getClass(String classname)
			throws ClassNotFoundException {
		ClassLoader classLoader = Thread.currentThread()
				.getContextClassLoader();
		if (classLoader == null)
			classLoader = NSProcessChannelHelper.class.getClassLoader();
		return (classLoader.loadClass(classname));
	} 
	
	*/

	public short open(ProcessIdentifier id, ProcessOpenOption option)
			throws NSProcessException {
		NSProcessOpenOptions options = new NSProcessOpenOptions();
		options.setAccess(option.getAccess());
		options.setExclusion(option.getExclusion());
		options.setNowaitDepth(option.getNowaitdepth());
		options.setMaxRequestMessageSize(option.getMaxRequestMessageSize());
		options.setMaxResponseMessageSize(option.getMaxResponseMessageSize());
		// options.setProcessName(id.getName());
		if (id != null && id.getProcessName() != null)
			options.setProcessName(channel.constructNamedProcessName(id));
		else
			options.setProcessName(channel.constructUnnamedProcessName(id));
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Opening a process with the following options:").append(
					"name:").append(options.getProcessName())
					.append(":access:").append(options.getAccess()).append(
							"req size:").append(
							options.getMaxRequestMessageSize()).append(
							"resp size:").append(
							options.getMaxResponseMessageSize());
			logger.fine(buf.toString());
		}
		return channel.open(options);
	}

	public void close(short filenum) throws NSProcessException {
		if (logger.isLoggable(Level.FINE)) {
			logger.fine("Closing file:" + filenum);
		}
		channel.close(filenum);
	}

	/**
	 * method used to write contents of <code>data</code> to the server
	 * process
	 * 
	 * @param data
	 *            contains data, filenum, tag etc
	 * @param requestMessageSize
	 *            Indicates the actual size of data that needs to be written
	 * @param responseMessageSize
	 *            indicates the maximum size of the response expected
	 * @throws NSProcessException
	 */
	public void write(NSProcessData data, final int requestMessageSize,
			final int responseMessageSize) throws NSProcessException {
		if (logger.isLoggable(Level.FINE)) {
			StringBuffer buf = new StringBuffer();
			buf.append("Writing to the server:").append("filenum:").append(
					data.getFilenum()).append(":tag:").append(data.getTag())
					.append(":req size:").append(requestMessageSize).append(
							":resp size:").append(responseMessageSize);
			logger.fine(buf.toString());
		}
		channel.write(data, requestMessageSize, responseMessageSize);
	}

	/**
	 * Checks if there is reply data available for the filenum and tag indicated
	 * by the input. If there is a reply available the reply data is populated
	 * by invoking the method setData & setError of the input NSProcessData
	 * object. Once the data is populated all the threads that could be waiting
	 * on the data are intimated by invoking the notifyAll method of the input
	 * object. </br> If there no reply available yet the object is added to the
	 * list of filenum+tag for which a response is expected.
	 * 
	 * @param data
	 *            NSProcessData object used to indicate filenum and tag for
	 *            which response is to be read
	 * @return true if there is a reply available for the input. False if there
	 *         is no reply available yet
	 */
	public boolean read(NSProcessData data, long timeout)
			throws NSProcessException {
		// First check if there is already response obtained for the filenum+tag
		// of the input NSProcessData
		// If there is one populate the input data structure with the data and
		// return
		// Else add this object to the readinitializelist
		// Once the 'readany' thread gets some data,
		// check if the response obtained matches with any objects on the
		// readinitializedlist
		// if so populate the object call the 'all' on the object.
		int position = -1;
		if (responseobtainedlist.contains(data)) {
			// This means there is already a response for the requested filenum
			// + tag
			// Hopefully there will not be a different thread that empties this
			// object. This can happen if this thread is pre-empted between the
			// call to the "if" the "synchronized" block.

			NSProcessData response = null;
			synchronized (responseobtainedlist) {
				position = responseobtainedlist.indexOf(data);
				if (position != -1) {
					response = (NSProcessData) responseobtainedlist
							.get(position);
					responseobtainedlist.remove(position);
				} else {
					logger
							.log(
									Level.WARNING,
									"Seemingly two threads are trying to read the response for a particular I/O with filenum["
											+ data.getFilenum()
											+ "] Tag["
											+ data.getTag()
											+ "]. The earlier thread has already read the response");
					throw new NSProcessException(
							"Seemingly two threads are trying to read the response for a particular I/O with filenum["
									+ data.getFilenum()
									+ "] Tag["
									+ data.getTag()
									+ "]. The earlier thread has already read the response");
				}
			}
			synchronized (data) {
				// now populate the data
				// Moving this outside the synchronized block just so that this
				// operation does not add to the total time we hold the lock
				// Its safe to assume that 'response' is not null
				data.setData(response.getData());
				data.setError(response.getError());
				data.setErrorDescription(response.getErrorDescription());
				data.setReadCompleted(true);
				data.notifyAll();
			}
			return true;
		}

		// At this point we know that there is no response yet for the requested
		// object
		// so add the input object to the readinitiatedlist
		if ((position = readinitiatedlist.indexOf(data)) != -1) {
			// This means that there is already read initiated for the given
			// filenum + tag
			// there cannot be two write operations for teh same filenum and the
			// same tag at the same time !!
			logger.log(Level.WARNING,
					"It seems like the are two I/Os on the same file ["
							+ data.getFilenum() + "] with the same tag["
							+ data.getTag() + "]");
			throw new NSProcessException(
					"It seems like the are two I/Os on the same file ["
							+ data.getFilenum() + "] with the same tag["
							+ data.getTag() + "]");
		}
		synchronized (readinitiatedlist) {
			readinitiatedlist.add(data);
		}
		synchronized (data) {
			try {
				if (timeout == -1)
					data.wait();
				else
					data.wait(timeout);
				// Now the question is how do we know if the wait ended with a
				// timeout !!!
				// For this if the data is actually obtained from the server
				// then the
				// flag response obtained is set to true. If it is false then it
				// means the
				// wait timedout
				if (data.isReadCompleted() == false) {
					StringBuffer buf = new StringBuffer();
					buf.append("Read timedout.");
					throw new NSProcessException(sm
							.getString("operation.timeout"),
							NativeStatus.TIMEDOUT, data.getFilenum(), data
									.getTag());
				}
				return true;
			} catch (InterruptedException e) {
				logger.log(Level.WARNING, "Read Interrupted.");
				Thread.currentThread().interrupt();
			}
		}
		return false;
	}

	/**
	 * Cancels an I/O operation. Now after canceling the operation the API
	 * should intimate any "read" methods of the cancel operation. If the read
	 * is infact invoked in a different thread and the cancel done in a
	 * different thread then it only right that the read is intimated of the
	 * cancel operation. So the solution to this problem is that if there is a
	 * cancel then the response will not be added to the responseobtainedlist.
	 * See how this message is handled in "handleResponse" method.
	 * 
	 * @param filenum
	 * @param tag
	 * @throws NSProcessException
	 */
	public void cancel(short filenum, int tag) throws NSProcessException {
		logger.log(Level.FINER,
				"Inside NSProcessChannelHealper.cancel(short,int) method.");
		channel.cancel(filenum, tag);
		NSProcessData data = new NSProcessData(filenum, tag);
		data.setError(NativeStatus.OPERATION_CANCELLED);
		data.setData(null);
		/*
		 * We are giving control to handleResponse so that any thread that is
		 * doing a "read" will be intimated of the cancel
		 */
		handleResponse(data);
	}

	public void setShutdown(boolean shutdown) {
		this.shutdown = shutdown;
	}

	private void handleResponse(NSProcessData response) {
		logger
				.log(Level.FINER,
						"Inside NSProcessChannelHealper.handleResponse(NSProcessdata) method.");
		// First check if there is somebody is wanting to read data for this
		// response in the readinitiatedlist. if found just give the data to
		// that person.
		// if none found add the response to the
		// reponseobtainedlist
		if (readinitiatedlist.contains(response)) {
			NSProcessData data = null;
			synchronized (readinitiatedlist) {
				int position = readinitiatedlist.indexOf(response);
				if (position != -1) {
					data = (NSProcessData) readinitiatedlist.get(position);
					readinitiatedlist.remove(position);
				} else {
					// Practically I think this can never happen
					// theoretically it may happen in the following scenario
					// if there a different thread gets executed between
					// if(readinitiatedlist.contains(...) and the synchronized
					// block then this condition can happen
				}
			}
			synchronized (data) {
				data.setData(response.getData());
				data.setError(response.getError());
				data.setReadCompleted(true);
				data.setErrorDescription(response.getErrorDescription());
				data.notifyAll();
			}
			return;
		}

		// See the method doc for "cancel"
		if (response.getError() == NativeStatus.OPERATION_CANCELLED) {
			return;
		}

		if (responseobtainedlist.contains(response)) {
			logger
					.log(
							Level.WARNING,
							"While the response to an earlier I/O on File["
									+ response.getFilenum()
									+ "]tag["
									+ response.getTag()
									+ "] is yet to be read, response to a later I/O on the same file, tag has arrived. Overwriting the earlier response");
		}
		synchronized (responseobtainedlist) {
			responseobtainedlist.add(response);
		}
	}

	// This method is invoked if a read (actually readAny) has resulted in any
	// type of error
	// The error could be a file system error or an internal error (the ones
	// indicated in NativeStatus, which is highly unlikely !!)
	// Whatever may be the error the appropriate read operation on the filenum
	// and tag
	// should be indicated of this error.
	private void handleException(NSProcessException e) {
		try {
			NSProcessData response = new NSProcessData(e.getFilenum(), e
					.getTag());
			response.setError(e.getError());
			response.setErrorDescription(e.getMessage());
			response.setData(null);

			handleResponse(response);
		} catch (NSProcessException e1) {
			StringWriter writer = new StringWriter();
			e1.printStackTrace(new PrintWriter(writer, true));
			writer.write("Root cause\n");
			e.printStackTrace(new PrintWriter(writer, true));
			logger.severe("Unexpected exception\n" + writer.toString());
		}
	}

	private void listenOnAnyIOCompletion() {
		logger.log(Level.FINE, "Inside the reader thread.");
		// Its intentional to have a single thread here
		if (readanythread != null)
			return;

		readanythread = new Thread(new Runnable() {
			public void run() {
				while (shutdown == false) {
					try {
						// This is the time duration for which the channel and
						// thus the JNI code will wait for any message from any
						// of the servers or from the file system in case of an
						// error
						// There is no real significance of the value
						int READTIMEOUT = 10;// seconds
						NSProcessData data = channel.readAny(READTIMEOUT);
						handleResponse(data);
					} catch (NSProcessException e) {
						// If its a timeout then there was no response found in
						// the READTIMEOUT seconds
						// which is OK. No need to throw an exception or handle
						// the exception
						// Otherwise something is wrong !!!!
						if (e.getError() != NativeStatus.TIMEDOUT) {
							handleException(e);
						}
					}
				}
			}
		});

		readanythread.setName("pio-readany");
		readanythread.setDaemon(true);
		readanythread.start();
	}

	protected Object clone() throws CloneNotSupportedException {
		return new CloneNotSupportedException("Clone is not allowed.");
	}

	public void setLogLevel(final short level) {
		channel.setNativeLogLevel(level);
	}
}