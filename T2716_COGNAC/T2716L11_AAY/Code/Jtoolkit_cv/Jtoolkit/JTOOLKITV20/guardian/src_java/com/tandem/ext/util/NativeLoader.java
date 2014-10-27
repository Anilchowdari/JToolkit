package com.tandem.ext.util;

public class NativeLoader{

	public static void LoadLibrary(){
		try{
			String sys = System.getProperty("os.name"); /* changes made for the RFE solution 10-121009-4998*/
			
			if(sys.equals("NONSTOP_KERNEL")){
		        //load native libraries as usual 
        	        //System.out.println("Loading PUT DLL..");
			String dataModel=System.getProperty("sun.arch.data.model");
				if(dataModel.equals("64")) 
				{
					System.loadLibrary("tdmext_64");
				}
				else 
				{
					System.loadLibrary("tdmext_32");
				}	
			
		    }
			else{
				//do not load native libraries				
				//empty body    //or break/contunue/return whichever is appropriate
			}
		}
		/* changes begin for the RFE solution 10-121009-4998*/
		catch(SecurityException se){
			System.out.println("User do not have the required permission to access getProperty()");
			se.printStackTrace();
    	}
		catch (NullPointerException ne){		
			System.out.println("Search pattern cant be null");
			ne.printStackTrace();
		}    
		catch (IllegalArgumentException  ie){
			System.out.println("Invalid or incomplete argument");
			ie.printStackTrace();
		}
		/* changes end for the RFE solution 10-121009-4998*/
	}
}
	
