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

import com.tandem.ext.enscribe.*;
import com.tandem.ext.util.*;
import com.tandem.tmf.*;
import cemp_pkg.*;

public class SampleG {

    public  static void main (String args[]) throws com.tandem.util.FSException {

		EnscribeFile employee;
		String fileName = new String("cemp1");
		Current tx;
		I_Cemp1 InputRecord = new I_Cemp1();
		O_Cemp1 OutputRecord = new O_Cemp1();
		short []empNo = {1,2,3,4,5,6,7,8,9,10};
		String []firstName = {"Frank","Faye","David","Alan","Selva",
			"Uyen","Bobbie","Dave","Nayna","Bill"};
		String []lastName = {"Smith","Brown","Jones","Black","White",
			"Stevens","Collins","Dowers","Phillips","Lane"};
		String [] middle = {"A","B","C","D","E","F","G","H","I","J"};
		short [] regno = {1,1,10,11,7,10,11,1,7,5};
		short [] branch = {1,1,2,4,5,2,4,1,5,1};

		try {

			// Open the file and write some data
			employee = new EnscribeFile(fileName);
			employee.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			System.out.println(">>Opened file. ");
			//Purge any existing data
			employee.purgeData();
			tx = new Current();
			for(int i = 0; i < empNo.length; i++) {
				tx.begin();
				OutputRecord.setEmpnum(empNo[i]);
				OutputRecord.setFirstname(firstName[i]);
				OutputRecord.setLastname(lastName[i]);
				OutputRecord.setMiddle(middle[i]);
				OutputRecord.setRegnum(regno[i]);
				OutputRecord.setBranchnum(branch[i]);
				employee.write(OutputRecord);
				tx.commit(true);
			}
			System.out.println(">>Wrote all data to the file");
			System.out.println(" ");
			System.out.println(">>Closing file");
			employee.close();
			EnscribeFile employeeRead = new EnscribeFile(fileName);
			employeeRead.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			System.out.println(">>Reopening file");
			System.out.println(">>Reading all records in the file");
			while(employeeRead.read(InputRecord) != -1) {
				System.out.println(InputRecord.getEmpnum()+", "+InputRecord.getFirstname()+
					", "+InputRecord.getLastname()+", "+InputRecord.getMiddle()+
					", "+InputRecord.getRegnum()+", "+InputRecord.getBranchnum());
			}
			System.out.println(">>Positioning to record with primary key value of 7");
			byte [] key = new byte[2];
			DataConversion.JavaShortToBIN16(key,(short)7,0);
			EnscribeKeyPositionOptions kpa = new EnscribeKeyPositionOptions();
			kpa.setKeySpecifier((short)0);
			kpa.setKeyValue(key);
			kpa.setPositioningMode(EnscribeKeyPositionOptions.POSITION_EXACT);
			kpa.setKeyLength((short)2);
			employeeRead.keyPosition(kpa);
			System.out.println(">>Reading record with primary key value of 7");
			employeeRead.read(InputRecord);
			System.out.print(InputRecord.getEmpnum()+", ");
			System.out.print(InputRecord.getFirstname()+", ");
			System.out.print(InputRecord.getLastname()+", ");
			System.out.print(InputRecord.getMiddle()+", ");
			System.out.print(InputRecord.getRegnum()+", ");
			System.out.println(InputRecord.getBranchnum());
			employeeRead.close();
			System.out.println(">>All done");
		} catch (com.tandem.ext.util.DataConversionException de) {
			de.printStackTrace();
			de.getMessage();
		} catch (EnscribeFileException ee) {
			System.out.println(ee.getMessage());
		}
	}
}
