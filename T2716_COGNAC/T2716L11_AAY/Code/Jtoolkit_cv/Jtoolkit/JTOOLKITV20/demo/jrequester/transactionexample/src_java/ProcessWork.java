import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;
import java.util.logging.StreamHandler;

import com.tandem.ext.guardian.GuardianException;
import com.tandem.ext.guardian.Receive;
import com.tandem.ext.guardian.ReceiveInfo;
import com.tandem.tmf.Current;

/**
 * This class executes the JDBC/MX Update within the TMF Transaction propagated
 * from the client.
 * 
 * 
 */
public class ProcessWork implements Runnable {

	Receive $RECEIVE;
	ReceiveInfo receiveInfo;
	byte[] requestData;
	int replyLength;
	public static Logger logger = Logger.getLogger("ProcessWork");
	Connection con;
	PreparedStatement pstmt;

	static {
		logger.addHandler(new StreamHandler(System.out, new SimpleFormatter()));
	}

	/**
	 * Constructor for ProcessWork Class. This creates a PreparedStatement
	 * object which updates the SQL/MX Table. The request data is of the format
	 * <ID>,<NEW FNAME>
	 * 
	 * @param $RECEIVE
	 * @param receiveInfo
	 * @param requestData
	 * @throws IOException
	 */
	public ProcessWork(Receive $RECEIVE, ReceiveInfo receiveInfo,
			byte[] requestData) {

		this.$RECEIVE = $RECEIVE;
		this.receiveInfo = receiveInfo;
		this.requestData = requestData;

		if (con == null) {
			try {
				this.con = DriverManager.getConnection("jdbc:sqlmx:");
			} catch (SQLException e) {
				ProcessWork.logger.severe(e.getMessage());
			}
		}

	}

	/**
	 * This is the function where actual work is done. The TMF Transaction
	 * propagated for this unit of work is activated and SQL/MX update is done
	 * within this transaction. The number of rows updated is returned to the
	 * IPC Client.
	 */
	public void run() {
		ObjectInputStream oin;
		try {
			oin = new ObjectInputStream(new ByteArrayInputStream(requestData));
			Object request = oin.readObject();
			if (request instanceof TransactionData) {
				TransactionData data = (TransactionData) request;
				receiveInfo.activateRequestTransID();
				Current txn = new Current();

				System.out.println("Current Transaction "
						+ txn.get_control().toString());

				pstmt = con
						.prepareStatement("UPDATE EMP SET FNAME = ? WHERE ID = ?");

				pstmt.setString(1, data.getFname());
				pstmt.setInt(2, data.getId());

				int retVal = pstmt.executeUpdate();
				reply(retVal, "Successfully updated");
			} else {
				reply(-1, "Invalid object in the input:"
						+ request.getClass().getName());
			}

		} catch (IOException e) {
			e.printStackTrace();
			reply(-1, e.getMessage());
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			reply(-1, e.getMessage());
		} catch (GuardianException e) {
			e.printStackTrace();
			reply(-1, e.getMessage());
		} catch (SQLException e) {
			e.printStackTrace();
			reply(e.getErrorCode(), e.getMessage());
		}
	}

	private void reply(int code, String description) {
		TransactionData reply = new TransactionData();
		reply.setDescription(description);
		reply.setRetcode(code);
		ByteArrayOutputStream baos = new ByteArrayOutputStream(1024);
		try {
			ObjectOutputStream oos = new ObjectOutputStream(baos);
			oos.writeObject(reply);
			oos.flush();
			this.$RECEIVE.reply(baos.toByteArray(), baos.size(),
					this.receiveInfo, 0);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (GuardianException e) {
			e.printStackTrace();
		}
	}
}
