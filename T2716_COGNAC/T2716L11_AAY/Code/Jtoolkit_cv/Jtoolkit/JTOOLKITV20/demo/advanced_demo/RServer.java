// -------------------------------------------------------------------------
//
// You are granted a limited copyright to modify and use this sample
// code for your internal purposes only. THIS SOFTWARE IS PROVIDED "AS-
// IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE OR NONINFRINGEMENT. IN NO EVENT SHALL THE
// HEWLETT-PACKARD COMPANY OR ITS SUBSIDIARIES BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE INCURRED BY YOU OR ANY THIRD PARTY IN CONNECTION WITH THE
// USE OF THIS SOFTWARE.
//
// -------------------------------------------------------------------------   

import java.io.*;
import com.tandem.ext.guardian.*;
import com.tandem.tmf.Current;
import com.tandem.util.*;
import com.tandem.ext.util.*;
import com.tandem.ext.enscribe.*;
import java.util.*;
import test_pkg.*;

/**
 * This is a multi-threaded server that performs operations
 * on an Enscribe order-entry database consisting of an order
 * file, an order detail file, a customer file, and a parts file.
 *
 * The server starts a thread (ReceiveHandler) to handle the open and
 * read of $RECEIVE. When the ReceiveHandler thread receives a message
 * on $RECEIVE, it creates a ServiceHandler thread.
 *
 * A ServiceHandler thread decomposes the request message to determine
 * which service is being requested, then calls a method to handle the
 * request.  Currently the ServiceHandler thread has methods that read
 * customer records, read order records and their detail information,
 * add a new customer, and update the parts file.
 * When the appropriate method has handled the customer's
 * request, the ServiceHandler thread replies to the request and stops
 * running.
 *
 * This server also contains a Timer thread that will cancel the last
 * operation invoked by an EnscribeFile object if a time limit expires.
 * The Timer thread is used by the ServiceHandler thread methods to
 * avoid deadlock.
 *
 * When you start this server, you can specify the number of reads
 * on $RECEIVE that can be outstanding before a service replies.
 **/
public class RServer {
  /**
   * A main() method for running the server as a standalone program.
   * The command-line argument should be the number of reads that
   * can be outstanding before a service replies.
   **/
  public static void main(String[] args) {
    try {
      	// Parse the argument list
      	int numConcurrentRequests = 2;
      	RServer s = null;
	  	if(args.length > 0) {
	  		numConcurrentRequests = Integer.parseInt(args[0]);
			// Create a RServer object that uses standard out as its log and
			// that can work on numConcurrentRequest messages concurrently
      		s = new RServer(System.out, numConcurrentRequests);
		} else {
			// Create a RServer object that uses standard out as its log and
			// that can work on the default (4) number of messages concurrently.
			 s = new RServer(System.out, 4);
		}
 	} catch (Exception e) { // Display a message if anything goes wrong
      System.err.println("RServer: " + e);
      System.err.println("Usage: java RServer <number_of_Concurrent_Requests>");
      System.exit(1);
    }
  }
  public final static int ERROR_REPLY_SIZE = 102;

  // This is the state for the server
  ReceiveHandler receiveHandler;	   // The ReceiveHandler object
  ThreadGroup threadGroup;             // The threadgroup for all our threads
  PrintWriter logStream;               // Where we send our logging output to

  /**
   * This is the RServer() constructor.  It must be passed a stream
   * to send log output to (may be null), and the limit on the number of
   * concurrent connections.  It creates and starts a ReceiveHandler
   * thread which opens and reads $RECEIVE.  It also creates a ServiceManager
   * thread which manages the threads that actually do the request
   **/
  public RServer(OutputStream logStream, int maxMessages) {
    setLogStream(logStream);
    log("Starting server");
    threadGroup = new ThreadGroup("RServer");
	log("Starting ReceiveHandler");
	receiveHandler = new ReceiveHandler(threadGroup,maxMessages,this);
	receiveHandler.start();
  }

  /**
   * A public method to set the current logging stream.  Pass null
   * to turn logging off
   **/
  public void setLogStream(OutputStream out) {
    if (out != null) logStream = new PrintWriter(new OutputStreamWriter(out));
    else logStream = null;
  }

  /** Write the specified string to the log */
  protected synchronized void log(String s) {
    if (logStream != null) {
      logStream.println("[" + new Date() + "] " + s);
      logStream.flush();
    }
  }
  /** Write the specified object to the log */
  protected void log(Object o) { log(o.toString()); }

  /**
   * A class that acts as a timer and cancels an Enscribe operation if
   * the operation has not completed when the timelimit is up
   **/

  public class Timer extends Thread {
	  Object timerMonitor = new Object();
	  long timeToWait = 0;
	  boolean waitForTimeOut = false;
	  boolean operationComplete = true;
	  EnscribeFile file = null;
	  boolean stop = false;

	  // Timer constructor
	  public Timer(long timeToWait, EnscribeFile file) {
		 this.timeToWait = timeToWait;
		 this.file = file;
		 this.setDaemon(true);
	  }
	  /*
	   * Run method for the Timer.  The timer will continue to run
	   * until stop is true.  While it runs, it will either wait
	   * for a specified amount of time (timeToWait) or forever unless
	   * it is interrupted.
	   */
	  public void run() {
		while(!stop) {
		  if(waitForTimeOut) {
			 waitForCompletion();
		  } else {
		 	 waitForNotification();
		  }
	    } // end while
	  }
	  /*
	   * This is the method where the Timer waits for a period of
	   * time for the Enscribe operation to complete.  Two things
	   * can happen in this routine - the Timer can be interrupted
	   * which means that the EnscribeOperation completed or it
	   * can wake up when it has waited for the specified time.
	   * If it wakes up after waiting, it still checks the value
	   * of waitForTimeOut and only if it is true, does the Timer
	   * cancel the last Enscribe Operation on file (the EnscribeFile
	   * object).
	   */
	  private void waitForCompletion() {
		synchronized (timerMonitor) {
		try {
			timerMonitor.wait(timeToWait);
			if(waitForTimeOut) {
				try {
				   if(file != null) {
				 	  file.cancelLastOperation();
					  waitForTimeOut = false;
				   } else {
					  log("Programming error EnscribeFile is null in Timer");
					  System.exit(1);
				   }
				 } catch (EnscribeFileException ef) {
					log("Timer received an EnscribeFile Exception: "+ef.getMessage());
					System.exit(1);
				 }
			 }
	    } catch (Exception e) {}

	    }
	  }
	  /*
	   * When this method is invoked, the timer waits until it
	   * is interrupted.
	   */
	  private void waitForNotification() {
		synchronized(timerMonitor) {
		try {
			timerMonitor.wait();
		} catch (Exception e) {}
		}
	  }
	  /* This is the routine called by users of the timer to
	   * tell the timer to wait for a timeout value or to
	   * wait forever.
	   *
	   * If wait is true, the timer waits for the value of
	   * timeToWait. If wait is false, the timer waits forever
	   * unless it is interrupted or stopped.
	   */
	  public void waitForTimeout(boolean wait) {
		  synchronized(timerMonitor) {
			  waitForTimeOut = wait;
			  timerMonitor.notify();
		  }
	  }
	  /** This tells the Timer to stop timing and go away. */
	  public void pleaseStop() {
	     this.stop = true;   // set the stop flag
	     waitForTimeOut = false;
	     this.interrupt();
      }
      /*
       * This identifes a new EnscribeFile object for
       * use with the timer.
       */
      public void setFileObject(EnscribeFile efile) {
		this.file = efile;
	  }
  }

  /**
   * This nested Thread subclass handles reads on $RECEIVE.  It reads $RECEIVE
   * and when it gets a message, it creates a ServiceHandler thread
   * to handle the request.
   */
  public class ReceiveHandler extends Thread {
  	Receive recv = null;
    int receiveDepth;  	// The number of messages that can be worked on
						// concurrently.
	RServer server;
	ThreadGroup group;


  /**
   * The ReceiveHandler constructor creates a thread for itself in the specified
   * threadgroup.  It creates a Receive object to read $RECEIVE,
   * accepting up to numConcurrentMessages before a reply.
   **/
  public ReceiveHandler(ThreadGroup group, int numConcurrentMessages,
  	RServer server) {
      	super(group, "ReceiveDepth:" + numConcurrentMessages);
		receiveDepth = numConcurrentMessages;
		this.server = server;
		this.group = group;
	 	// open $Receive
	  	try {
	          recv = Receive.getInstance();
			  recv.setReceiveDepth(numConcurrentMessages);
	          log("Opening $RECEIVE with receive depth set to: "+numConcurrentMessages);
	          recv.open();
	  	} catch (GuardianException ex) {
	          log("FATAL ERROR: Unable to open $RECEIVE");
	          log(ex.getMessage());
	          System.exit(1);
      	}
	  	catch (IllegalAccessException ex) {
	           log(ex.getMessage());
	  	}
    }

  /**
   * A receiveHandler is a Thread, and this is its body.
   * It reads messages, creates a new ServiceHandler thread
   * passing the message in the constructor, then starts the
   * ServiceHandler thread which processes the message.
   **/
  public void run() {

    ReceiveInfo ri = null;
	int countRead; // number of bytes read
	boolean moreOpeners = true;
	// construct byte array big enough to hold largest possible request
	// this server can receive
	byte[] maxMsg = new byte[3000];
    int errorReturn = GError.EOK;
	int msgOut = 0;
	byte[] errorReply = new byte[200];
	int errorReplyCode = 99;
	String errorReplyMsg = "Unable to start thread to handle request";

    while(moreOpeners) {      // loop until we're asked to stop.
       	try {
       		countRead = recv.read(maxMsg, maxMsg.length);
	  		ri = recv.getLastMessageInfo();
	  		ServiceHandler s = new ServiceHandler(group,recv,
				ri, maxMsg, countRead,server);
			if(s == null) {
				log("Internal Error: ServiceHandler is null");
				DataConversion.JavaIntToBIN32(errorReply,errorReplyCode,0);
				DataConversion.JavaStrToCobolStr(errorReply,errorReplyMsg,2,
						errorReplyMsg.length());
				recv.reply(errorReply,2 + errorReplyMsg.length(),(short)0);
			} else {
				s.start();
			}
	    } catch (ReceiveNoOpeners ex) {
		  	moreOpeners = false;
		  	log("There are no more openers so this server will stop");
		} catch (GuardianException ex) {
			log("Unexpected Guardian Error on $RECEIVE");
		   	log(ex.getMessage());
		   	System.exit(1);
	    } catch (Exception ex) {
			log(ex.getMessage());
		   	ex.printStackTrace();
		   	System.exit(1);
        } // end try
    }  // end while
  }
 }

 /**
  * This class is a subclass of Thread that handles an individual request
  * message received on $RECEIVE.  Each
  * ServiceHandler thread processes the request message and then
  * replies to the client.
  **/
 public class ServiceHandler extends Thread {

	private final static int CUST_INFO_REQ = 1;
	private final static int ORDER_INFO_REQ = 2;
	private final static int ADD_CUSTOMER_REQ = 3;
  	private final static int ODETAIL_INFO_REQ = 4;
  	private final static int GET_PART_INFO_REQ = 5;
  	private final static int UPDATE_PART_INFO_REQ = 6;
  	private final static int CUST_INFO_REPLY = 7;
	private final static int ORDER_INFO_REPLY = 8;
	private final static int ADD_CUSTOMER_REPLY = 9;
	private final static int ODETAIL_INFO_REPLY = 10;
	private final static int GET_PART_INFO_REPLY = 11;
	private final static int ERROR_REPLY_CODE = 99;

    Receive r;     	// The Receive object
    ReceiveInfo ri; // The ReceiveInfo object
	byte [] msg;	// The message that was received on $RECEIVE
	int msgCount;	// The number of bytes read from $RECEIVE
	RServer server; // The RServer object.
	GuardianOutput reply;   // A byte array to contain the reply to the message.
	Current tx;		// Used if a transaction must be doomed.
	EnscribeFile orders; // An object to represent the orders file.
	EnscribeFile customer; // An object to represent the customer file.
	EnscribeFile odetail; // An object to represent the odetail file.
	EnscribeFile parts; // An object to represent the parts file.

   /**
    * This constructor just saves some state and calls the superclass
    * constructor to create a thread.  ServiceHandler
    * objects are created by the ReceiveHandler.
    * The ReceiveHandler thread is part of
    * the RServer ThreadGroup, so all ServiceHandler threads are part of that
    * group, too.
    **/
    public ServiceHandler(ThreadGroup group, Receive r, ReceiveInfo ri,
		byte[] msg, int msgCount, RServer server) {

	  super(group,"ServiceHandler");
      this.r = r;
      this.ri = ri;
	  this.msg = msg;
	  this.msgCount = msgCount;
	  this.server = server;
	  tx = new Current();
	}

    /**
     * This is the body of each and every ServiceHandler thread.
     * It activates the transaction associated with the message,
	 * passes the message, the message count, and a reply buffer to the
     * appropriate method to satisfy the request.  That method
     * is responsible for extracting data from the request and composing
	 * the data that is put in the reply buffer.  When the method
	 * completes, the ServiceManager checks to see if the transaction
	 * should be doomed (set to rollback only) and then replies to
	 * the client.
     *
     **/
    public void run() {


	  // reply is a buffer the size of the largest possible reply for this server
	  try {
	  	ri.activateRequestTransID();
	   	int request_code = DataConversion.BIN32ToJavaInt(msg, 0);
		switch(request_code) {
			case CUST_INFO_REQ:
				reply = getCustomerInfo(msg, msgCount,server);
				break;
			case ORDER_INFO_REQ:
				reply = getOrderInfo(msg, msgCount, server);
				break;
			case ADD_CUSTOMER_REQ:
				reply = addCustomer(msg,msgCount, server);
				break;
	  		case ODETAIL_INFO_REQ:
	  			reply = getOdetailInfo(msg,msgCount, server);
	  			break;
	  		case GET_PART_INFO_REQ:
	  			reply = getPartInfo(msg,msgCount, server);
	  			break;
	  		case UPDATE_PART_INFO_REQ:
	  			reply = updatePartInfo(msg,msgCount,server);
	  			break;
	  		default:
	  			reply = errorReply((short)0,"Unknown request code; request rejected");
	  			break;
		}
   		r.reply(reply,ri,(short)0);
      } catch (GuardianException e) {
	  	log("In ServiceHandler run; got :"+e.getMessage());
	  	reply = errorReply((short)0,"In ServiceHandler run; got :"+e.getMessage());
	  	try {
	  		r.reply(reply,ri,(short)0);
		} catch (Exception ge) {log("Reply Error: "+ge.getMessage());}
  	  } catch (com.tandem.ext.util.DataConversionException dce) {
		log("In ServiceHandler run; got :"+dce.getMessage());
		reply = errorReply((short)0,"In ServiceHandler run; got :"+dce.getMessage());
		try {
	  		r.reply(reply,ri,(short)0);
	  	} catch (Exception ge) {log("Reply error: "+ge.getMessage());}
	  } catch (com.tandem.util.FSException fse) {
		log("Got an FSException in Service Handler run: "+fse.getMessage());
		reply = errorReply((short)0,"In ServiceHandler run; got :"+fse.getMessage());
		try {
	  		r.reply(reply, ri,(short)0);
	  	} catch (Exception ge) {log("Reply error: "+ ge.getMessage());}
      }
   }

  /**
   * This method returns a list of customers which it obtains
   * by reading through any locks held by other programs.
   **/
  protected GuardianOutput getCustomerInfo (byte [] msg,
  	int count, RServer server) throws com.tandem.util.FSException {

	int i = 0;

	I_Customerinforequest req = new I_Customerinforequest();
	O_Customerinforeply rep = new O_Customerinforeply();
	I_Customer cust = new I_Customer();

	int num_cust = 0;
	int starting_cust = 0;

	try {
		// Get the number of Customers and the starting customer
		// number
		req.unmarshal(msg,count);
		num_cust = req.getNum_customers_requested();
		starting_cust = req.getStarting_custnum();

		customer = new EnscribeFile("customer");
		// Open the customer file
		customer.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);

		// Set up key positioning options
		EnscribeKeyPositionOptions kpo = new EnscribeKeyPositionOptions();
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,starting_cust,0);
		kpo.setKeyLength((short)4);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short) 0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_APPROXIMATE);
		// Position the customer file
		customer.keyPosition(kpo);

		// Specify that we want to read through locks
		customer.setFileLockingMode(EnscribeFile.READ_THROUGH_NORMAL_LOCKING_MODE);

		// Read the customer file
		for(i = 0; i < num_cust; i++) {
			int countread = customer.read(cust);
			if(countread == -1) { // end of file
				break;
			} else {
				// Set up the reply
				rep.setCustnum(cust.getCustnum(),i);
				rep.setCustname(cust.getCustname(),i);
				rep.setAddress(cust.getAddress(),i);
				rep.setCity(cust.getCity(),i);
				rep.setState(cust.getState(),i);
				rep.setZip(cust.getZip(),i);
			}
		}

		rep.setNum_customers((short)(i));
		rep.setReply_code(CUST_INFO_REPLY);

		// Close the customer file
		customer.close();
		return rep;
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(customer != null && customer.isFileOpen()) customer.close();
		tx.rollback_only();
		return errorReply((short) ef.getErrorNum(),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(customer != null && customer.isFileOpen()) customer.close();
		tx.rollback_only();
		return errorReply((short)0,"Data conversion error: "+dc.getMessage());
	}
  }
  /**
   * Method used to reply if an error occurs
   *
   **/
  protected GuardianOutput errorReply(short errorNum,String msg) {

	O_Error_reply rep = new O_Error_reply();

	rep.setReply_code(ERROR_REPLY_CODE);
	rep.setError_number(errorNum);
	rep.setMsg(msg);
	return rep;
  }

  /**
   * This is another example service.  Given a request that contains an
   * order number and a customer number, this method reads data from
   * the orders file and the customer file and returns information about
   * the specific order.
   *
   * Since this method locks both the customer file and the orders file,
   * it starts a Timer thread so that it will not wait forever if it
   * cannot obtain a lock.
   **/
  protected GuardianOutput getOrderInfo (byte [] msg, int count, RServer server)
  	throws com.tandem.util.FSException {

	I_Orderinforequest req = new I_Orderinforequest();
	O_Orderinforeply rep = new O_Orderinforeply();

	I_Orders ord = new I_Orders();
	I_Customer cust = new I_Customer();

	RServer.Timer timer = null;
	int custnum = 0;
	int i = 0;
	int ordernum = 0;

	try {
		req.unmarshal(msg,count);
		custnum = req.getCustnum();
		ordernum = req.getOrdernum();
		orders = new EnscribeFile("orders");
		customer = new EnscribeFile("customer");

		// Open the orders file
		orders.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
		// Open the customer file
		customer.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
		// Set up key position options for orders file
		EnscribeKeyPositionOptions kpo = new EnscribeKeyPositionOptions();
		kpo.setKeyLength((short)4);
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,ordernum,0);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short)0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
		// Position orders file
		orders.keyPosition(kpo);

		timer = server.new Timer(6000,orders);
		timer.start();
		timer.waitForTimeout(true);
		orders.lockRecord();
		timer.waitForTimeout(false);
		// read orders file
		orders.read(ord);
		// Close orders file
		orders.unlockRecord();
		orders.close();

		// Set reply data from orders file
		rep.setOrdernum(ord.getOrdernum());
		rep.setCustnum(ord.getCustnum());
		rep.setOrder_date_month(ord.getOrderdate_month());
		rep.setOrder_date_day_of_month(ord.getOrderdate_day_of_month());
		rep.setOrder_date_year(ord.getOrderdate_year());
		rep.setDel_date_month(ord.getDeldate_month());
		rep.setDel_date_day_of_month(ord.getDeldate_day_of_month());
		rep.setDel_date_year(ord.getDeldate_year());

		// Set up position data for customer file
		kpo.setKeyLength((short)4);
		// Convert custnum to Himalaya format
		DataConversion.JavaIntToBIN32(key,custnum,0);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short)0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
		// Position customer file
		customer.keyPosition(kpo);
		timer.setFileObject(customer);
		timer.waitForTimeout(true);
		customer.lockRecord();
		timer.pleaseStop();
		// Read customer file
		customer.read(cust);
		// unlock customer file
		customer.unlockRecord();
		// Close customer file
		customer.close();
		// Set reply data from customer file
		rep.setCustname(cust.getCustname());
		rep.setReply_code(ORDER_INFO_REPLY);
		return rep;
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(customer != null && customer.isFileOpen()) customer.close();
		if(orders != null && orders.isFileOpen()) orders.close();
		tx.rollback_only();
		return errorReply(((short)ef.getErrorNum()),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(customer != null && customer.isFileOpen()) customer.close();
		if(orders != null && orders.isFileOpen()) orders.close();
		tx.rollback_only();
		return errorReply((short)0,"Data Conversion error: "+dc.getMessage());
	}
  }

  /**
   * This service adds a new customer record.
   **/
  protected GuardianOutput addCustomer (byte [] msg, int count, RServer server)
  	throws FSException {
  	 I_Add_customer req = new I_Add_customer();
	 O_Add_customer_reply rep = new O_Add_customer_reply();
	 O_Customer cust = new O_Customer();
	 I_Customer custRead = new I_Customer();
	 EnscribeKeyPositionOptions kpo;
	 RServer.Timer timer = null;

	 try {
	 	req.unmarshal(msg,count);
		cust.setCustname(req.getCustname());
		cust.setAddress(req.getAddress());
		cust.setCity(req.getCity());
		cust.setState(req.getState());
		cust.setZip(req.getZip());

		//Read the file and find the last customer
		customer = new EnscribeFile("customer");
		customer.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
		kpo = new EnscribeKeyPositionOptions();
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,Integer.MAX_VALUE,0);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short)0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_APPROXIMATE);
		kpo.setReadReversed(true);
		customer.keyPosition(kpo);
		timer = server.new Timer(6000,customer);
		timer.start();
		timer.waitForTimeout(true);
		customer.readLock(custRead);
		timer.pleaseStop();
		// Now write the new record and unlock
		cust.setCustnum(custRead.getCustnum()+1);
		customer.write(cust);
		customer.unlockRecord();
		customer.close();
		rep.setReply_code(ADD_CUSTOMER_REPLY);
		rep.setNewcustnum(custRead.getCustnum()+1);
		return rep;
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(customer != null && customer.isFileOpen()) customer.close();
		tx.rollback_only();
		return errorReply(((short)ef.getErrorNum()),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(customer != null && customer.isFileOpen()) customer.close();
		tx.rollback_only();
		return errorReply((short)0,"Data Conversion error: "+dc.getMessage());
	}
  }
  /**
   * This returns detailed information about a specific order
   **/
  protected GuardianOutput getOdetailInfo (byte [] msg, int count, RServer server)
  	throws FSException {

	  I_Orderdetailrequest req = new I_Orderdetailrequest();
	  O_Orderdetailreply rep = new O_Orderdetailreply();

	  I_Odetail od = new I_Odetail();
	  I_Parts pt = new I_Parts();

	  RServer.Timer timer = null;
	  int partnum = 0;
	  int i = 0;
	  int ordernum = 0;

	  try {
	  	req.unmarshal(msg,count);
	  	ordernum = req.getOrdernum();

	  	rep.setReply_code(ODETAIL_INFO_REPLY);
		rep.setNumdetails((short)0);

	  	odetail = new EnscribeFile("odetail");
	  	parts = new EnscribeFile("parts");

	  	// Open the odetail file
	  	odetail.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
	  	// Open the parts file
	  	parts.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
	  	// Set up key position options for odetail file

		EnscribeKeyPositionOptions kpo = new EnscribeKeyPositionOptions();
		kpo.setKeyLength((short)4);
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,ordernum,0);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short)0);
		kpo.setCompareLength((short)4);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_GENERIC);
		// Position orders file
		odetail.keyPosition(kpo);

		timer = server.new Timer(9000,odetail);
		timer.start();
		timer.waitForTimeout(true);
		while(odetail.readLock(od) != -1) {
			rep.setQuantity_ordered(od.getQuantity(),i);
			DataConversion.JavaIntToBIN32(key,od.getPartnum(),0);
			kpo.setKeyValue(key);
			kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
			kpo.setKeySpecifier((short) 0);
			kpo.setKeyLength((short)4);
			parts.keyPosition(kpo);
			timer.setFileObject(parts);
			parts.readLock(pt);
			timer.waitForTimeout(false);
			rep.setItem_name(pt.getPartname(),i);
			rep.setItem_price(pt.getPrice(),i);
			i++;
			odetail.unlockRecord();
			parts.unlockRecord();
			timer.setFileObject(odetail);
			timer.waitForTimeout(true);
		}
		timer.pleaseStop();
		rep.setNumdetails((short)i);
		odetail.close();
		parts.close();
		return rep;
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(odetail != null && odetail.isFileOpen()) odetail.close();
		if(parts != null && parts.isFileOpen()) parts.close();
		try { tx.rollback_only(); } catch (FSException fse1) {}
		return errorReply(((short)ef.getErrorNum()),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(odetail != null && odetail.isFileOpen()) odetail.close();
		if(parts != null && parts.isFileOpen()) parts.close();
		try {tx.rollback_only(); } catch (FSException fse2) {}
		return errorReply((short)0,"Data Conversion error: "+dc.getMessage());
	}
  }
 /**
  * This method returns information about the part associated
  * with a particular part number.
  **/
  protected GuardianOutput getPartInfo (byte [] msg,
  	int count, RServer server) throws FSException {

	int i = 0;

	I_Getpartinforeq req = new I_Getpartinforeq();
	int partnum = 0;

	try {
		// Get the part number
		req.unmarshal(msg,count);
		partnum = req.getPartnum();
	} catch (com.tandem.ext.util.DataConversionException dc) {
		return errorReply((short) 0,"Data Conversion error: "+dc.getMessage());
	}
	return getPartRecord(partnum);
 }

 /**
  * This method reads a part record given a part number.
  */
 private GuardianOutput getPartRecord(int partnum)
 	throws FSException {

	O_Getpartinforeply rep = new O_Getpartinforeply();
	I_Parts part = new I_Parts();

	try{
		parts = new EnscribeFile("parts");
		// Open the part file
		parts.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);

		// Set up key positioning options
		EnscribeKeyPositionOptions kpo = new EnscribeKeyPositionOptions();
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,partnum,0);
		kpo.setKeyLength((short)4);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short) 0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
		// Position the customer file
		parts.keyPosition(kpo);
		// Specify that we want to read through locks
		parts.setFileLockingMode(EnscribeFile.READ_THROUGH_NORMAL_LOCKING_MODE);

		// Read the parts file
		int countread = parts.read(part);
		if(countread == -1) { // end of file
			String ErrorMessage = new String("The specified part does not exist");
			return errorReply((short)11,ErrorMessage);
		} else {
			// Set up the reply
			rep.setPartnum(part.getPartnum());
			rep.setPartname(part.getPartname());
			rep.setInventory(part.getInventory());
			rep.setLocation(part.getLocation());
			rep.setPrice(part.getPrice());
		}
		rep.setReply_code(GET_PART_INFO_REPLY);
		// Close the parts file
		parts.close();
		return rep;
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(parts != null && parts.isFileOpen()) parts.close();
		tx.rollback_only();
		return errorReply(((short)ef.getErrorNum()),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(parts !=null && parts.isFileOpen()) parts.close();
		 tx.rollback_only();
		return errorReply((short) 0,"Data Conversion error: "+dc.getMessage());
	}

  }
 /**
  * This method updates the inventory of a particular part.  The
  * update is performed on the part with a specified part number.
  **/
  protected GuardianOutput updatePartInfo (byte [] msg, int count, RServer server)
  	throws FSException {
  	 I_Updatepartinfo req = new I_Updatepartinfo();
	 O_Getpartinforeply rep = new O_Getpartinforeply();
	 IO_Parts part = new IO_Parts();

	 int partnum = 0;
	 String changeInventory = null;
	 int newInventory = 0;

	 EnscribeKeyPositionOptions kpo;
	 RServer.Timer timer = null;

	 try {
	 	req.unmarshal(msg,count);
	 	partnum = req.getPartnum();
	 	changeInventory = req.getChange_inventory();

		//Read the file and find the last customer
		parts = new EnscribeFile("parts");
		parts.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
		kpo = new EnscribeKeyPositionOptions();
		byte [] key = new byte[4];
		DataConversion.JavaIntToBIN32(key,partnum,0);
		kpo.setKeyValue(key);
		kpo.setKeySpecifier((short)0);
		kpo.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
		parts.keyPosition(kpo);
		timer = server.new Timer(6000,parts);
		timer.start();
		timer.waitForTimeout(true);
		parts.readUpdateLock(part);
		timer.pleaseStop();
		try {
			newInventory = (Integer.parseInt(changeInventory) + (Integer.parseInt(part.getInventory())));
		} catch (NumberFormatException nf) {
			parts.close();
			tx.rollback_only();
			errorReply((short)0,"Invalid value passed for inventory"+nf.getMessage());
		}
		// Now update the record and unlock
		part.setInventory(Integer.toString(newInventory));
		parts.writeUpdateUnlock(part);
		parts.close();
		return getPartRecord(partnum);
	} catch (com.tandem.ext.enscribe.EnscribeFileException ef) {
		if(parts != null && parts.isFileOpen()) parts.close();
		tx.rollback_only();
		return errorReply(((short)ef.getErrorNum()),
			"Error accessing: "+ef.getFileName()+" : "+ef.getMessage());
	} catch (com.tandem.ext.util.DataConversionException dc) {
		if(parts != null && parts.isFileOpen()) parts.close();
		tx.rollback_only();
		return errorReply((short)0,"Data Conversion error: "+dc.getMessage());
	}
  }
}
}
