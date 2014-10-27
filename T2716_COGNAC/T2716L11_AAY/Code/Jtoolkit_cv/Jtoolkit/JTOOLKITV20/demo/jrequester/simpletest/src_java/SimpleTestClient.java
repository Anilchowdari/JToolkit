import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import com.hp.nonstop.io.process.Process;
import com.hp.nonstop.io.process.ProcessIdentifier;
import com.hp.nonstop.io.process.ProcessOpenOption;
import com.hp.nonstop.io.process.ProcessTimeoutException;

public class SimpleTestClient {

	public static void main(String[] argv) {
		if (argv.length != 1) {
			System.out.println("Usage java " + SimpleTestClient.class.getName()
					+ " <name of server process(without \\$)>");
			System.exit(0);
		}
		new SimpleTestClient(argv[0]);
	}

	public SimpleTestClient(String procname) {
		// Opening a process with all the defaults
		ProcessOpenOption option = new ProcessOpenOption(
				ProcessOpenOption.DEFAULT_ACCESS_MODE,
				ProcessOpenOption.DEFAULT_EXCLUSION_MODE, 5000, // Timeout in ms
				1000, // maximum message size sent to the server
				500);// maximum size of the response from the server
		Process p = new Process(new ProcessIdentifier(procname), option);
		try {
			p.open();
			// Using a buffered writer to be able to write character data to the
			// output stream
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(p
					.getOutputStream()));
			writer.write("Pack my box with five dozen liquor jugs");
			
			System.out.println("Sending the message [Pack my box with five dozen liquor jugs] to the server process "+procname);
			writer.flush();

			// Using a buffered reader to be able to read character data from
			// input stream
			BufferedReader reader = new BufferedReader(new InputStreamReader(p
					.getInputStream()));
			String s = null;
			while ((s = reader.readLine()) != null) {
				System.out.println("Response from the server ["+s+"]");
			}
		} catch (IOException ex) {
			if (ex instanceof ProcessTimeoutException) {
				System.out
						.println("A time out has occured. So cancelling the request");
				try {
					p.cancel();
				} catch (IOException e) {
					e.printStackTrace();
				}
			} else
				ex.printStackTrace();
		} finally {
			try {
				p.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}
}
