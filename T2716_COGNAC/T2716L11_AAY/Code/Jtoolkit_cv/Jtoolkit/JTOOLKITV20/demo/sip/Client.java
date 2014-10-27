// This example is from _Java Examples in a Nutshell_. (http://www.oreilly.com)
// Copyright (c) 1997 by David Flanagan
// This example is provided WITHOUT ANY WARRANTY either expressed or implied.
// You may study, use, modify, and distribute it for non-commercial purposes.
// For any commercial use, see http://www.davidflanagan.com/javaexamples

import java.io.*;
import java.net.*;

/**
 * This program connects to a server at a specified host and port.
 * It reads text from the console and sends it to the server.
 * It reads text from the server and sends it to the console.
 **/
public class Client {
  public static void main(String[] args) throws IOException {

  	char[] buffer = new char[1024];
    	int chars_read;
	String line;
	String fullLine;
    	try {
      		// Check the number of arguments
      		if (args.length != 3)
        		throw new IllegalArgumentException("Wrong number of arguments");

      		// Parse the host and port specifications
      		String host = args[0];
      		int port = Integer.parseInt(args[1]);
	  	int clientId = Integer.parseInt(args[2]);

	  	line = new String("Client"+clientId+" sends message: ");

      		// Connect to the specified host and port
      		Socket s = new Socket(host, port);

      		// Set up streams for reading from and writing to the server.
      		// The from_server stream is final for use in the anonymous class below
      		final Reader from_server = new InputStreamReader(s.getInputStream());
      		PrintWriter to_server =
        		new PrintWriter(new OutputStreamWriter(s.getOutputStream()));

      		// Tell the user that we've connected
      		System.out.println("Connected to " + s.getInetAddress() + ":"+ s.getPort());

      		try {
	  		for(int i = 0; i < 100; i++) {
				fullLine = new String(line + i);
				System.out.println("Sending: "+fullLine);
				to_server.println(fullLine);
        			to_server.flush();
            			chars_read = from_server.read(buffer);
				if(chars_read == 0)
					break;
            			System.out.println(buffer);
				Thread.currentThread().sleep(300);
        		}
			s.close();
	  	}  catch (IOException e) { System.out.println(e); }
      		// When we closes the connection, the loop above will end.
      		// Tell the user what happened, and call System.exit(), causing
      		// the main thread to exit along with this one.
      		System.out.println("Connection closed.");
      		System.exit(0);
    }
    // If anything goes wrong, print an error message
    catch (Exception e) {
      	System.err.println(e);
      	System.err.println("Usage: java Client <hostname> <port> <clientId>");
    }
  }
}
