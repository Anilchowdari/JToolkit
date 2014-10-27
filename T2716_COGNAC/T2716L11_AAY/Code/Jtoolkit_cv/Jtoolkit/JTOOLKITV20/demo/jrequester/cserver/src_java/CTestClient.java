import java.io.IOException;

import com.hp.nonstop.io.process.Process;
import com.hp.nonstop.io.process.ProcessIdentifier;
import com.tandem.ext.util.DataConversionException;

public class CTestClient {

	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println("Usage java " + CTestClient.class.getName()
					+ " <name of server process(without \\$)>");
			System.exit(0);
		}
		new CTestClient(args[0]);

	}

	public CTestClient(String pname) {
		IO_Input request = new IO_Input(); // Input to the server
		IO_Output response = new IO_Output(); // Output from the server

		request.setAge(35);
		request.setEmpid((short) 7007);
		request.setFirstname("James");
		request.setLastname("Bond");
		request.setOperation((byte) 'A');

		Process p = new Process(new ProcessIdentifier(pname));
		try {
			p.open();
			p.service(request,response);

			System.out.println("Server responded with the message:"
					+ response.getMessage());
		} catch (IOException e) {
			e.printStackTrace();
		} catch (DataConversionException e) {
			e.printStackTrace();
		} finally {
			try {
				p.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

	}

}
