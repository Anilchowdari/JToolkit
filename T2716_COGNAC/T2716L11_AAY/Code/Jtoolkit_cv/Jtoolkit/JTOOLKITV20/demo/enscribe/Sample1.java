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
import com.tandem.tmf.*;

public class Sample1 {

    public  static void main (String args[]) throws com.tandem.util.FSException {

		EnscribeFile sample;
		EnscribeFile altfile;
		String fileName = new String("sample");
		String altFileName = new String("altfile0");
		Current tx;
		String record;

		String str[] = {
			"01BobbieChapman        700000Java Development Group   05",
			"02Faye Brackett        780000Java Development Group   05",
			"03Frank Butera         790000Java HotSpot Group       06",
			"04David Schorow       1000000Java Guru Group          07",
			"05Alan Joseph          700000Java HotSpot Group       06",
			"06Emile Roth           800000Java HotSpot Group       06",
			"07Bill Blosen          900000Java Management Group    09",
			"08Ken Holt            1000000Java Management Group    09",
			"09Dave Velasco         800000Java Pthreads Group      08",
			"10Last Guy              50000Last Guy Group           99"};


		//First create the file

		try {
			sample = new EnscribeFile(fileName);
			altfile = new EnscribeFile(altFileName);
			// If the file exists,purge it
			try {
				if(sample.isExistingFile())
					sample.purge();
			} catch (EnscribeFileException pe) {
				System.out.println("Shouldn't get an exception on the purge because we check to see if it exists first");
				pe.getMessage();
			}
			// if the alternate key file exists, purge it
			try {
				if(altfile.isExistingFile())
					altfile.purge();
				} catch (EnscribeFileException pe) {
					System.out.println("Shouldn't get an exception on the purge because we check to see if it exists first");
					pe.getMessage();
			}

			// Create the primary file
			KeySequencedCreationAttributes creationAttr = new
		 		KeySequencedCreationAttributes();
		 	// Set general file attributes
			creationAttr.setPrimaryExtentSize(16);
			creationAttr.setSecondaryExtentSize(16);
			creationAttr.setMaximumExtents(100);
			creationAttr.setAudited(true);
			creationAttr.setRecordLength(56);
			creationAttr.setBlockLength(4096);
			creationAttr.setKeyLength(2);
			creationAttr.setKeyOffset(0);
			// Set alternate key attributes
			AlternateKeyDescriptor [] ak = new AlternateKeyDescriptor[1];
			ak[0] = new AlternateKeyDescriptor();
			ak[0].setKeyOffset(54);
			ak[0].setKeyLength(2);
			ak[0].setKeySpecifier("DP");
			ak[0].setAlternateKeyFileName(altFileName);
			creationAttr.setAlternateKeyDescriptors(ak);
			sample.create(creationAttr);
			sample.createAltKeyFiles();

			System.out.println(">>Created File");
			System.out.println("");

			// Obtain the attributes of the newly created file
			System.out.println(">>Attributes of new file: ");
			EnscribeFileAttributes myAttr = sample.getFileInfo();
			System.out.println("   File code = "+myAttr.getFileCode());
			System.out.println("   File security = "+myAttr.getGuardianSecurityString());
			System.out.println("   Primary keyoffset = "+myAttr.getKeyOffset());
			System.out.println("   Maximum extents = "+myAttr.getMaximumExtents());
			System.out.println("   File is audited = "+myAttr.isAudited());
			System.out.println("   Record length = "+myAttr.getRecordLength());
			System.out.println("   File Type = "+myAttr.getFileType());
			System.out.println("   File is key sequenced = "+myAttr.isKeySequenced());
			System.out.println("   Primary key length is "+myAttr.getKeyLength());
			System.out.println("   Alternate Key Attributes: ");
			AlternateKeyDescriptor [] altdesc = myAttr.getAlternateKeyAttributes();
			// Only one, since that is how we created the file
			System.out.println("      Number of alt keys = "+myAttr.getNumberOfAltkeys());
			System.out.println("      Alternate key file name = "+altdesc[0].getAlternateKeyFileName());
			System.out.println("      Key Specifier is "+altdesc[0].getKeySpecifierNumeric());
			System.out.println("      Key Length is "+altdesc[0].getKeyLength());
			System.out.println("      Key offset is "+altdesc[0].getKeyOffset());
			System.out.println(" ");

			// Open the file and write some data
			sample.open(EnscribeOpenOptions.READ_WRITE,EnscribeOpenOptions.SHARED);
			System.out.println(">>Opened file. ");
			tx = new Current();
			tx.begin();
			for(int i = 0; i < str.length; i++) {
				sample.write(str[i].getBytes(),str[i].length());
			}
			tx.commit(true);
			System.out.println(">>Wrote data to the file");
			System.out.println(" ");

			//Going to position on alternate key value 06
			System.out.println(">>Reading records with alternate key value of 06");
			EnscribeKeyPositionOptions kpa = new EnscribeKeyPositionOptions();
			// The following is needed because we are positioning using the
			// alternate key. If we were using the primary key, we would
			// specify kpa.setKeySpecifier(0);
			short keySpec = altdesc[0].getKeySpecifierNumeric();
			kpa.setKeySpecifier(keySpec);
			String keyvalue = new String("06");
			kpa.setKeyValue(keyvalue);
			kpa.setPositioningMode(EnscribeKeyPositionOptions.POSITION_GENERIC);
			kpa.setKeyLength((short)2);
			kpa.setCompareLength(2);
			tx.begin();
			sample.keyPosition(kpa);
			int maxRead = sample.getMaxReadCount();
			byte [] inBuf = new byte[maxRead];
			int count;
		   	tx.begin();
			while(sample.read(inBuf, maxRead) != -1) {
				record = new String(inBuf);
				System.out.println("   "+record);
			}
			// Now read in the reverse direction
			System.out.println("");
			System.out.println(">>Reading same records in reverse direction");
			kpa.setReadReversed(true,true);
			sample.keyPosition(kpa);
			while(sample.read(inBuf,maxRead) != -1) {
				record = new String(inBuf);
				System.out.println("   "+record);
			}
			tx.commit(false);
			sample.close();
		} catch (EnscribeFileException ee) {
			System.out.println(ee.getMessage());
		}
	}
}
