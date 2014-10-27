import java.io.*;
/**
 * This sample java program illustrates using the methods of Umask for 
 * setting and retrieving the file mode creation mask.
 **/
public class UmaskDemo
{
	public static void main(String[] args) throws IOException
	{
	    // Get the file mode creation mask of the current process.
		System.out.println("Current Umask = " + Umask.getUmask());

		// Masks Write permission for Owner and Read permission for Group.
		Umask.setUmask(Umask.S_IRGRP | Umask.S_IWUSR);

		System.out.println("New Umask  = " + Umask.getUmask());

		new File("Sample.txt").createNewFile();
	}
}
