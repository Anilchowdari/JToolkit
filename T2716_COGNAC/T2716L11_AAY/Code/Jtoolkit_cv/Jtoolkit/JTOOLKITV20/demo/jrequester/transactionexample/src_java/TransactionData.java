import java.io.Serializable;

public class TransactionData implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 431236L;
	/*
	 * id, fname, lname will be sent from the client to the server
	 */
	int id;
	String fname;
	String lname;

	/*
	 * retcode and description will be sent from the server to the client
	 */
	int retcode;
	String description;

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public String getFname() {
		return fname;
	}

	public void setFname(String fname) {
		this.fname = fname;
	}

	public String getLname() {
		return lname;
	}

	public void setLname(String lname) {
		this.lname = lname;
	}

	public int getRetcode() {
		return retcode;
	}

	public void setRetcode(int retcode) {
		this.retcode = retcode;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

}
