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


/*
 * Demo client which communicates with legacy TSMP server.
 */
import employee_class.*;
import java.io.*;
import java.util.*;
import com.tandem.tsmp.*;
import com.tandem.tmf.*;
import com.tandem.util.*;
import com.tandem.ext.util.*;
import com.tandem.ext.*;

class dialogEmployeeInfo {

	static protected String pmonName;
	static protected String svrClassName;
	static protected Properties properties = null;

	public static void main(String args[]) {

	  String pmonName = null;
	  String svrClassName = null;
	  int i = 0;


	  if((args.length == 0) || (!args[0].startsWith("-")) || (args.length < 4)) {
		System.err.println("Usage: java dialogEmployeeInfo -p pathmonName -s serverClassName");
		System.exit(1);
	  }
	  
	  while(i < args.length) {
		if(args[i].equals("-p")) {
			pmonName = args[++i];
		} else if(args[i].equals("-s")) {
			svrClassName = args[++i];
		}
		i++;
	  }
	  if((pmonName == null) || (svrClassName == null)) {
		System.err.println("Usage: java dialogEmployeeInfo -p pathmonName -s serverClassName");
		System.exit(1);
	  }
          System.out.println("================================================");
	  System.out.println("           Going to add employees");
          System.out.println("================================================");
	  addEmployee(pmonName, svrClassName);
          System.out.println("==============================================================");
	  System.out.println("            Reading employees from the data base");
          System.out.println("==============================================================");
	  getEmployeeInfo(pmonName, svrClassName);
          System.out.println("==============================================================");
	  System.out.println("            Deleting employees from the data base");
          System.out.println("==============================================================");
	  deleteEmployee(pmonName, svrClassName);
	  System.out.println("Exiting from dialogEmployeeInfo....");
	  
	}
		 
	private static void addEmployee(String pmonName, String svrClassName) {
	  String employeeNum[] = {"0001", "0002", "0003", "0004","0005"};
	  String firstName[] = {"Anne", "Mike", "Steve", "Frank","Steve"};
	  String lastName[] = {"Smith", "Jones", "Brown", "Green","Waugh"};
	  String middleInitial[] = {"A", "B", "C", "D","E"};
          String regnum[] = {"01", "01", "02", "03","04"};
          String branchnum [] = {"00", "01", "00", "01","00"};
	  String request_code = new String("02");
	  Current tx = new Current();

	  // Create a TsmpServer object supplying the pathmon name and
	  // the serverclass name.
	  TsmpServer myserver = new TsmpServer(pmonName,svrClassName);
          TsmpDialog mydialog = myserver.createDialog();
          System.out.println("          *********Pathmon Name : "+mydialog.getPathmonName()+"*********");
          System.out.println("          *********ServerClassName : "+mydialog.getServerClassName()+"*********");
          System.out.println("          *********Transaction Model Value : "+mydialog.getTransactionModel()+"*********");
          mydialog.setTransactionModel(mydialog.TM_ANY_TRANSACTION_PER_DIALOG); //setting transaction model value
          mydialog.setTimeout(100); // setting the time out value
          System.out.println("          *********Dialog's Timeout value : "+mydialog.getTimeout()+"*********");
          byte [] greply = new byte [100];
	  O_Employee_add empAdd = new O_Employee_add();

	  for(int i = 0; i < 5; i++) {
		empAdd.setRequest_code(request_code);
		empAdd.setEmployee_number(employeeNum[i]);
		empAdd.setFirst(firstName[i]);
		empAdd.setMiddle(middleInitial[i]);
		empAdd.setLast(lastName[i]);
		empAdd.setRegnum(regnum[i]);
		empAdd.setBranchnum(branchnum[i]);
		// Send the request to the server
			
 		try {
  		    // Begin a transaction
		    tx.begin();
                     if(mydialog.getStatus()==0){
                       mydialog.reset();
                       mydialog.setTransactionModel(mydialog.TM_ANY_TRANSACTION_PER_DIALOG);
                       System.out.println("\n----->Dialog has been reset ,hence reused ");
                    }
                    if(mydialog.getStatus()==-1)
                          System.out.println("\n         *********Dialog Begin*********");
                    else
                          System.out.println("\n         *********Dialog Send*********");
		    mydialog.send(empAdd, greply);
                   
                     if(i==4)
                        mydialog.abort(); // aborting the dialog for the fifth entry 
                    switch(mydialog.getStatus()){
                      case 70:
                               System.out.println("----->Status of the Dialog is Continue");
                               break;
                      case 0:
                               System.out.println("----->Dialog has been Ended ");
                               break;
                      case 1:
                               System.out.println("----->Dialog has been aborted");
                               System.out.println("----->Transaction Rollbacked");
                               tx.rollback();
                               return;
                   } 
		} catch (TsmpServerException e) {
		    System.out.println(e.getMessage());
		    try {
			// rollback because of errors
		        tx.rollback();
		    } catch (FSException er) {}
		    System.exit(1);
		} catch (FSException fs) {
		    System.out.println(fs.getMessage());
		    try {
			// rollback
		        tx.rollback();
		    } catch (FSException er2) {}
		    System.exit(1);
		} catch (Exception e) {
                    System.out.println(e.getMessage());
                    System.exit(1);
                }
		
		try {
                    if(DataConversion.CStrToJavaStr(greply, 0, 2).equals("02")) {
			I_Employee_reply empReply = new I_Employee_reply();
                        empReply.unmarshal(greply, greply.length);
			System.out.println("Employee added successfully");
			tx.commit(true);
                    } else if(DataConversion.CStrToJavaStr(greply, 0, 2).equals("00")) {
			I_Error_reply errReply = new I_Error_reply();
                        errReply.unmarshal(greply, greply.length);
			System.out.println(errReply.getError_text());
			tx.rollback();
			System.exit(1);
		    } else {
			System.out.println("Invalid reply code returned: " + DataConversion.CStrToJavaStr(greply,0,1));
			try {
			    tx.rollback();
			} catch (FSException er3) {}
			System.exit(1);
		    }
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(1);
		}
	  }
	  return;
	}				
	
	private static void getEmployeeInfo(String pmonName, String svrClassName) {
	  String getempNum[] = {"0001","0002","0003","0004"};
	  String empnum;
	  String firstName;
	  String lastName;
	  String middleInitial;
          String regnum;
          String branchnum;
	  String request_code = new String("01");

	  // Create a TsmpServer object supplying the pathmon name and
	  // the serverclass name.
	  TsmpServer getserver = new TsmpServer(pmonName,svrClassName);
          TsmpDialog getdialog = getserver.createDialog();
          byte []greply = new byte [100];
	  O_Employee_request empRequest = new O_Employee_request();

		  
          for(int i = 0; i < 4; i++) {
		
		empRequest.setRequest_code(request_code);
		empRequest.setEmployee_number(getempNum[i]);
		
		// Send the request to the server
			
 		try {
		    // Server is doing a read-only operation so no transaction is required
                    if(getdialog.getStatus()==0){
                       getdialog.reset();
                       getdialog.setTransactionModel(getdialog.TM_ANY_TRANSACTION_PER_DIALOG);
                       System.out.println("----->Dialog has been reset ,hence reused ");
                    } 

		    getdialog.send(empRequest, greply);
		} catch (Exception e) {
		    System.out.println(e.getMessage());
		    System.exit(1);
		}
		try {
                    if(DataConversion.CStrToJavaStr(greply, 0, 2).equals("01")) {
			I_Employee_reply empReply = new I_Employee_reply();
                        empReply.unmarshal(greply, greply.length);
			System.out.println("First Name: " + empReply.getFirst());
			System.out.println("Middle Initial: " + empReply.getMiddle());
			System.out.println("Last Name: " + empReply.getLast());
			System.out.println("Region: "  + empReply.getRegnum());
			System.out.println("Branch: " + empReply.getBranchnum());
                    } else if(DataConversion.CStrToJavaStr(greply, 0, 2).equals("00")) {
			I_Error_reply errReply = new I_Error_reply();
                        errReply.unmarshal(greply, greply.length);
			System.out.println(errReply.getError_text());
			System.exit(1);
		    } else {
			System.out.println("Invalid reply code returned: " + DataConversion.CStrToJavaStr(greply,0,1));
			System.exit(1);
		    }				
		} catch (Exception ex) {
			ex.printStackTrace();
			System.exit(1);
		}
	  } 
	}
	private static void deleteEmployee(String pmonName, String svrClassName) {
	
          String delEmpNum[] = {"0001", "0002", "0003", "0004"};
	  String empNum;
	  Current tx = new Current();

	  // Create a TsmpServer object supplying the pathmon name and the serverclass name.
	  TsmpServer delserver = new TsmpServer(pmonName, svrClassName);
          TsmpDialog deldialog = delserver.createDialog();
          deldialog.setTransactionModel(deldialog.TM_ANY_TRANSACTION_PER_DIALOG);
	  I_Error_reply delReply = new I_Error_reply();
	  O_Employee_request empRequest = new O_Employee_request();

	  for(int i = 0; i < 4; i++) {
	    empRequest.setRequest_code("03");
	    empRequest.setEmployee_number(delEmpNum[i]);
		
	    try {
		tx.begin();
                    if(deldialog.getStatus()==0){
                       deldialog.reset();
                       deldialog.setTransactionModel(deldialog.TM_ANY_TRANSACTION_PER_DIALOG);
                       System.out.println("----->Dialog has been reset ,hence reused ");
                    }

		deldialog.send(empRequest, delReply);
	    } catch (Exception e) {
		System.out.println(e.getMessage());
		try { tx.rollback(); } catch (FSException r1) {}
		System.exit(1);
	    }
		
	    try {
		if(delReply.getReply_code().trim().equals("3")) {
		    System.out.println(delReply.getError_text());
		    tx.commit(true);
		} else if(delReply.getReply_code().trim().equals("0")) {
		    System.out.println(delReply.getError_text());
		    try { tx.rollback(); } catch (FSException r1) {}
		} else {
		    System.out.println("Invalid reply code from delete: " + delReply.getReply_code());
		    try { tx.rollback(); } catch (FSException r2) {}
		}
	    } catch (Exception e) {
		System.out.println(e.getMessage());
	    }
	  }
    }
		
	
}				
		
