import java.nio.ByteBuffer;

import com.tandem.ext.guardian.GuardianException;
import com.tandem.ext.guardian.Receive;
import com.tandem.ext.guardian.ReceiveNoOpeners;

/**
 * This is a sample to demonstrate the TMF transaction propagation between
 * the Client and Server.
 *
 */
public class TransactionServer {

	/**
	 * Buffer to hold the data read from $RECEIVE
	 */
	public static ByteBuffer buffer = ByteBuffer.allocate(57344);
	
	
	static {
		// Initialize the JDBC/MX T2 Driver
		try {
			Class.forName("com.tandem.sqlmx.SQLMXDriver");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			System.out.println("Could not find JDBC/MX T2 Driver");
			System.exit(1);
		}
	}

	/**
	 * This is the main function which delegates the request processing to
	 * a ProcessWork object. The ProcessWork parses the request and updates
	 * a SQL/MX Table using JDBC/MX T2 Driver. 
	 * The JDBC update done by the ProcessWork object can be rolled back or 
	 * committed from the client 
	 *  
	 * @param args
	 * @throws GuardianException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 * @throws ReceiveNoOpeners
	 */
	public static void main(String[] args) throws GuardianException, IllegalArgumentException, IllegalAccessException, ReceiveNoOpeners {
		
		Receive $RECEIVE = Receive.getInstance();
		$RECEIVE.setNotifyNoMoreOpeners(false);
		$RECEIVE.setReceiveDepth(1);
		
		if(!$RECEIVE.isOpen()) {
			$RECEIVE.open();
		}

		while(true) {
			int countRead = $RECEIVE.read(TransactionServer.buffer.array(), 57344);
			byte [] request = new byte[countRead];
			System.arraycopy(TransactionServer.buffer.array(), 0, request, 0, countRead);
			ProcessWork task = new ProcessWork($RECEIVE, $RECEIVE.getLastMessageInfo(), request);
			Thread th = new Thread(task);
			th.setDaemon(true);
			th.start();
		}
	}
}
