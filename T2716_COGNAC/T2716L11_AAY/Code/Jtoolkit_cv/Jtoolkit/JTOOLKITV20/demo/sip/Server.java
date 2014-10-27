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


import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InterruptedIOException;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.io.*;

public class Server {
    static BufferedReader  is;
    static PrintWriter os;
    static Socket           sock;
    static ServerSocket     serverSock;

    public static void main(String[] args) {

	String data;
	String reply;
	String line;

       	try {
		// Get the port to listen on
		int port = Integer.parseInt(args[0]);
	    	System.out.println("Trying to open socket");
        	serverSock = new ServerSocket(port);
	    	System.out.println("After opening socket");
       	} catch (UnknownHostException e) {
            exit(e);
       	} catch (IOException e) {
            exit(e);
       	}
       	System.out.println("ready for requests");

 	int len = 0;

        for (;;) {
            try {
		sock = serverSock.accept();
		is = new BufferedReader(new InputStreamReader(sock.getInputStream()));
	        os = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
	        while((line = is.readLine()) != null){
			if(line.length() == 0) {
				System.out.println("No more messages from client");
				break;
			}
                	System.out.println("data in = "+line);
			Thread.currentThread().sleep(800);
			reply = new String("Returning line to client: "+line);
			os.println(reply);
			os.flush();
		}
		os.close();
		is.close();
		sock.close();
            } catch (Exception e) {
                System.out.println(e.getMessage());
		exit(e);
            }
        }
    }

    private static void exit() {
        Runtime.getRuntime().exit(1);
    }
    private static void exit(Exception e) {
        System.out.println("exception : " + e);
        exit();
    }
}
