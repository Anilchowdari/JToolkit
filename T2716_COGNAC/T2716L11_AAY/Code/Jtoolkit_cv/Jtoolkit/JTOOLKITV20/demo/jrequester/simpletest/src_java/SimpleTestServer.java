import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;

import com.tandem.ext.guardian.GError;
import com.tandem.ext.guardian.GuardianException;
import com.tandem.ext.guardian.Receive;
import com.tandem.ext.guardian.ReceiveInfo;
import com.tandem.ext.util.DataConversionException;

public class SimpleTestServer implements Runnable {

	static Receive recv = null;
	private static final int RECEIVE_DEPTH = 50;
	private static final int LARGEST_MESSAGE = 2000;

	private ByteBuffer buf = null;
	private ReceiveInfo recvinfo = null;

	public SimpleTestServer(ByteBuffer buf, ReceiveInfo recvinfo) {
		this.buf = buf;
		this.recvinfo = recvinfo;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		int openers = 0;
		// open $Receive
		try {
			recv = Receive.getInstance();
			recv.setReceiveDepth(RECEIVE_DEPTH);
			recv.setNotifyNoMoreOpeners(false);
			recv.setSystemMessageMask(Receive.SMM_OPEN | Receive.SMM_CLOSE);
			recv.open();
		} catch (GuardianException ex) {
			System.err.println("FATAL ERROR: Unable to open $RECEIVE");
			System.err.println(ex.getMessage());
			System.exit(1);
		} catch (Exception e) {
			System.out.println("Illegal access" + e);
		}
		byte[] maxMsg = new byte[LARGEST_MESSAGE]; // holds largest possible
		// request
		while (true) {
			int bytesread = 0;
			try {
				bytesread = recv.read(maxMsg, maxMsg.length);
			} catch (Exception e) {
				e.printStackTrace(System.out);
				System.exit(1);
			}

			ReceiveInfo recvinfo = recv.getLastMessageInfo();
			if (recvinfo.isSystemMessage()) {
				int msgnum = -1;
				try {
					msgnum = recvinfo.getSystemMessageNumber(maxMsg);
				} catch (DataConversionException e) {
					// TODO Auto-generated catch block
					e.printStackTrace(System.out);
					System.exit(1);
				}
				switch (msgnum) {
				case ReceiveInfo.SYSMSG_OPEN:
					// System.out.println("An open message");
					openers++;
					break;
				case ReceiveInfo.SYSMSG_CLOSE:
					// System.out.println("A close message");
					openers--;
					break;
				default:
					System.out.println("Unknown system message " + msgnum);
					break;
				}
				try {
					recv.reply(null, 0, recvinfo, GError.EOK);
				} catch (IllegalArgumentException e) {
					e.printStackTrace(System.out);
				} catch (GuardianException e) {
					e.printStackTrace(System.out);
				}

			} else {
				ByteBuffer buf = ByteBuffer.allocate(bytesread);
				buf.put(maxMsg, 0, bytesread);
				buf.flip();
				Thread th = new Thread(new SimpleTestServer(buf, recvinfo));
				th.setDaemon(true);
				th.start();
			}
		}
	}

	public void run() {
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					new ByteArrayInputStream(buf.array()), Charset
							.forName("UTF-8")));

			String s = null;

			while ((s = reader.readLine()) != null)
				System.out.println("Message from a client [" + s + "]");

			ByteArrayOutputStream baos = new ByteArrayOutputStream(recvinfo
					.getMaxReply());

			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
					baos));
			String response = "Yes. I did pack your box with five dozen liquor jugs";
			writer.write(response);
			System.out.println("Replying with the response[" + response + "]");
			writer.flush();

			// assuming that am allowed to send a reply of this size
			recv.reply(baos.toByteArray(), baos.size(), recvinfo, GError.EOK);

		} catch (IOException e) {
			handleException(e);
		} catch (IllegalArgumentException e) {
			handleException(e);
		} catch (GuardianException e) {
			handleException(e);
		}
	}

	private void handleException(Exception ex) {
		String message = ex.getMessage();
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream(recvinfo
					.getMaxReply());
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
					baos));
			writer.write(message);
			writer.flush();
			byte[] data = baos.toByteArray();
			recv.reply(data, data.length, recvinfo, GError.EOK);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (GuardianException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
