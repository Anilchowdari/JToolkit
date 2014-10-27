import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Random;

import com.hp.nonstop.io.process.Process;
import com.hp.nonstop.io.process.ProcessIdentifier;
import com.hp.nonstop.io.process.ProcessInputStream;
import com.hp.nonstop.io.process.ProcessOutputStream;
import com.tandem.tmf.Current;

public class TransactionClient {

	/**
	 * This is a demo program to show the Transaction propagation using the JRequester
	 * API of JToolkit
	 *
	 * This client application controls the transaction in which the server process
	 * does some DB operation.
	 * 
	 * @param args -
	 *            The IPC Server process name to which request is sent
	 * @throws IOException
	 */
	public static void main(String[] args) throws Exception {

		if (args.length != 1) {
			System.out.println("Usage: java "
					+ TransactionClient.class.getName()
					+ " <server process name without \\$>");
			System.exit(0);
		}

		final Process process = new Process(new ProcessIdentifier(args[0]));
		Random rnd = new Random(478988678124L);

		process.open();
		System.out.println("Successfully opened the process " + args[0]);

		ProcessOutputStream ostream = process.getOutputStream();
		ProcessInputStream istream = process.getInputStream();

		BufferedReader ipReader = new BufferedReader(new InputStreamReader(
				System.in));

		for (int nfor = 0; nfor < 3; ++nfor) {
			// Preparing data for update
			TransactionData data = new TransactionData();
			data.setId(nfor + 1);
			data.setFname("FN" + rnd.nextInt(9999));
			System.out.println("Trying to update the first name of id["
					+ data.getId() + "] to " + data.getFname());

			Current txn = new Current();
			txn.begin();
			System.out.println("Current Transaction "
					+ txn.get_control().toString());

			ObjectOutputStream os = new ObjectOutputStream(ostream);
			os.writeObject(data);
			os.flush();

			ObjectInputStream is = new ObjectInputStream(istream);
			Object response = is.readObject();
			if (!(response instanceof TransactionData)) {
				System.out
						.println("Unexpected response from the server. Was expecting an object of class TransactionData but got "
								+ response.getClass());
				System.exit(1);
			}

			data = (TransactionData) response;
			if (data.getRetcode() < 0) {
				System.out.println("Update operation unsuccessful.["
						+ data.getDescription() + "]");
				txn.rollback();
				continue;
			}

			System.out
					.print("Press \"Y\" to commit \"N\" to roll back the transaction "
							+ txn.get_control().toString()+":");

			String choice = ipReader.readLine();

			if (choice.equalsIgnoreCase("Y")) {
				System.out.println("Committing the Transaction");
				txn.commit(false);
			} else {
				System.out.println("Rolling back the Transaction");
				txn.rollback();
			}

		}
		ipReader.close();
		process.close();
	}
}
