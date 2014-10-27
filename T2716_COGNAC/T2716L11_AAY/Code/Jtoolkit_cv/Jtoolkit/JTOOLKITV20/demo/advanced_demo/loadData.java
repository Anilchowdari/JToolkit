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
import test_pkg.*;

public class loadData {

	static String fileLoc = new String("$<vol>.<subvol>");

    public  static void main (String args[]) throws com.tandem.util.FSException {

		writeCustomerRecords();
		writeOrdersRecords();
		writePartsRecords();
		writeFromsupRecords();
		writeSupplierRecords();
		writeOdetailRecords();
	}
	private static void writeCustomerRecords() {
		EnscribeFile customer;
		O_Customer cust = new O_Customer();
		Current tx;

		int [] custnum = {1,2,3,4,5,6,7,8,9,10};
		String [] custname = {"Acme Furniture Company","BR Woods",
			"Stanley Cleaners", "Jones Appliances","Phillips Hardware",
			"42 West","Johnson and Jones", "Appliance Warehouse",
			"Fun Stuff Inc", "McMahon and Son"};
		String [] addr = {"123 A Street", "54 Main", "2345 University",
			"58 Palm Ave","345 West First", "1256 El Camino",
			"78 Bayfront Expressway", "12 So. Calero","678 Orange Ave",
			"90 West 10th Street"};
		String [] city = {"Los Angeles", "Denver", "Flagstaff", "Reno",
			"Boston", "Portland","Seattle","Miami","Cleveland",
			"Madison"};
		String [] state = {"CA","CO","AZ","NV","MA","OR","WA","FL",
			"OH","WI"};
		String [] zip = {"95123","96789","98909","94563","83456",
			"93490","78690","99910","93333","95555"};

		try {
			tx = new Current();
			// Open the file and write some data
			customer = new EnscribeFile(fileLoc+".customer");
			customer.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			customer.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < custnum.length; i++) {
				cust.setCustnum(custnum[i]);
				cust.setCustname(custname[i]);
				cust.setAddress(addr[i]);
				cust.setCity(city[i]);
				cust.setState(state[i]);
				cust.setZip(zip[i]);
				customer.write(cust);
			}
			System.out.println(">>Wrote all data to the customer file");
			System.out.println(" ");
			customer.close();
			tx.commit(false);
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		} catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}
	private static void writeOrdersRecords() {

		EnscribeFile orders;
		O_Orders ord = new O_Orders();
		Current tx;

		int [] ordnum = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
		String [] ord_mo = {"12","10","01","04","10","05","02","07","06","09",
			  "01","03","11","04","02","06","05","10","01","08"};
		String [] ord_day = {"01","24","05","31","15","16","27","13","19","20",
						   "06","23","14","25","26","16","08","09","11","12"};
		String [] ord_year = {"01","01","02","01","02","01","02","02","01","02",
			"01","01","02","01","02","01","02","02","01","02"};

		String [] del_mo = {"01","10","02","04","11","05","02","08","07","07","10",
			"02","04","12","04","03","04","07","07","11","02","09"};

		String [] del_day = {"03","24","05","31","15","16","27","13","19","20",
						   "09","28","14","26","26","16","08","10","11","12"};

		String [] del_year = {"02","01","02","01","02","01","02","02","01","02",
			"01","01","02","01","02","01","02","02","01","02"};

		int [] custnum = {10,9,8,7,6,1,2,3,4,5,7,8,9,2,3,4,5,6,1,10};
		try {
			tx = new Current();
			// Open the file and write some data
			orders = new EnscribeFile(fileLoc+".orders");
			orders.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			orders.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < ordnum.length; i++) {
				ord.setOrdernum(ordnum[i]);
				ord.setOrderdate_month(ord_mo[i]);
				ord.setOrderdate_day_of_month(ord_day[i]);
				ord.setOrderdate_year(ord_year[i]);
				ord.setDeldate_month(del_mo[i]);
				ord.setDeldate_day_of_month(del_day[i]);
				ord.setDeldate_year(del_year[i]);
				ord.setCustnum(custnum[i]);
				orders.write(ord);
			}
			System.out.println(">>Wrote all data to the orders file");
			System.out.println(" ");
			orders.close();
			tx.commit(false);
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		} catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}
	public static void writePartsRecords() {
		EnscribeFile parts;
		O_Parts prts = new O_Parts();
		Current tx;


		int [] partnum = {1,2,3,4,5,6,7,8,9,10};
		String [] partname = {"washer","dryer","dishwasher","oven","microwave",
			"trash compactor","dish drawer","convection oven","disposal","cook top"};
		String[] inventory = {"35","40","23","54","12","18","34","17","45","19"};
		String[] location = {"A1","A2","A3","A4","A5","A6","A7","A8","A9","A10"};
		String[] price = {"334.56","256.00","324.56","470.90","79.34",
			"250.78","350.87","125.67","78.00","200.98"};

		try {
			tx = new Current();
			// Open the file and write some data
			parts = new EnscribeFile(fileLoc+".parts");
			parts.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			parts.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < partnum.length; i++) {
				prts.setPartnum(partnum[i]);
				prts.setPartname(partname[i]);
				prts.setInventory(inventory[i]);
				prts.setLocation(location[i]);
				prts.setPrice(price[i]);
				parts.write(prts);
			}
			System.out.println(">>Wrote all data to the parts file");
			System.out.println(" ");
			tx.commit(true);
			parts.close();
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		} catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}
	private static void writeFromsupRecords() {
		EnscribeFile fromsup;
		O_Fromsup frsup = new O_Fromsup();
		Current tx;

		int [] partnum = {1,2,3,4,5,6,7,8,9,10};
		int [] suppnum = {4,3,2,1,1,2,3,4,2,3};
		String [] partcost = {"400.50","275.00","390.56","498.90","89.00",
			"275.78","398.87","149.67","85.00","250.98"};

		try {
			tx = new Current();
			// Open the file and write some data
			fromsup = new EnscribeFile(fileLoc+".fromsup");
			fromsup.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			fromsup.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < partnum.length; i++) {
				frsup.setPartnum(partnum[i]);
				frsup.setSuppnum(suppnum[i]);
				frsup.setPartcost(partcost[i]);
				fromsup.write(frsup);
			}
			System.out.println(">>Wrote all data to the fromsup file");
			System.out.println(" ");
			tx.commit(true);
			fromsup.close();
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		} catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}

	private static void writeSupplierRecords() {
		EnscribeFile supplier;
		O_Supplier supp = new O_Supplier();
		Current tx;

		int [] suppnum = {1,2,3,4};
		String[] suppname = {"Big One","Little One","Medium One","Tiny One"};
		try {
			tx = new Current();
			// Open the file and write some data
			supplier = new EnscribeFile(fileLoc+".supplier");
			supplier.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			supplier.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < suppnum.length; i++) {
				supp.setSuppnum(suppnum[i]);
				supp.setSuppname(suppname[i]);
				supplier.write(supp);
			}
			System.out.println(">>Wrote all data to the supplier file");
			System.out.println(" ");
			tx.commit(true);
			supplier.close();
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		} catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}
	private static void writeOdetailRecords() {
		EnscribeFile odetail;
		O_Odetail od = new O_Odetail();
		Current tx;

		int [] ordernum = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
		int [] partnum = {4,5,6,7,8,9,10,1,2,3,6,7,8,9,10,5,4,3,2,1};
		String [] quantity = {"04","01","10","05","20","04","15","09","13","23",
							  "12","19","17","02","16","08","05","07","11","22"};

		try {
			tx = new Current();
			// Open the file and write some data
			odetail = new EnscribeFile(fileLoc+".odetail");
			odetail.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			tx.begin();
			//Purge any existing data
			odetail.purgeData();
			tx.commit(false);

			tx.begin();
			for(int i = 0; i < ordernum.length; i++) {
				od.setOrdernum(ordernum[i]);
				od.setPartnum(partnum[i]);
				od.setQuantity(quantity[i]);
				odetail.write(od);
			}
			System.out.println(">>Wrote all data to the odetail file");
			System.out.println(" ");
			tx.commit(true);
			odetail.close();
		} catch (EnscribeFileException ef) {
			System.out.println(ef.getMessage());
			System.exit(1);
		} catch (com.tandem.util.FSException fs) {
			System.out.println(fs.getMessage());
			System.exit(1);
		}catch (DataConversionException dc) {
			System.out.println(dc.getMessage());
			System.exit(1);
		}
	}

}
