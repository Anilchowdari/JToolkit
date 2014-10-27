package com.hp.nonstop.io.process;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * <p>
 * This class identifies the NonStop <i>Server</i> process. While this class
 * describes the parameters to identify a NonStop server process in brief, a
 * complete description of the parameters can be found in the section "File
 * Names and Process Identifiers" in <a href=
 * "http://bizsupport2.austin.hp.com/bc/docs/support/SupportManual/c02141550/c02141550.pdf"
 * >Guardian Procedure Calls Reference Manual"</a>
 * </p>
 * 
 * @author Vyom Tewari
 */
public class ProcessIdentifier {

	/**
	 * Name of the process opened.
	 */
	private String processName = null;

	/**
	 * The node on which the process is running.
	 */
	private String node = null;

	/**
	 * specifies the system-assigned sequence number of the process.
	 */
	private long seqNo = -1;

	/**
	 * Is a string containing 1-7 alphanumeric characters.
	 */
	private String qualifier1 = null;

	/**
	 * Is a string containing 1-8 alphanumeric characters.
	 */
	private String qualifier2 = null;

	/**
	 * Specifies the processor number of the processor in which the process is
	 * running.
	 */
	private int cpu = -1;

	/**
	 * Specifies the process identification number of the process.
	 */
	private int pin = -1;

	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.ProcessIdentifier");

	/**
	 * Constructs the ProcessIdentifier for the <i>Server</i> process
	 * 
	 * @param pname
	 *            The name of the <i>Server</i> process. Example "X1AK","TEST".
	 *            Note that the process name does not contain the "$" sign.
	 * 
	 * @throws IllegalArgumentException
	 *             If process name is null or more than 5 character and first
	 *             character is not letter.
	 */
	public ProcessIdentifier(String pname) throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open with process name: " + pname);
		if (pname == null)
			throw new IllegalArgumentException(
					Process.sm.getString("process.name.is.null"));
		else {
			if (pname.length() > 5) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.length"));
			}
			if (Character.isLetter(pname.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.firstChar"));
			}
			if (!isValidString(pname)) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.valid"));
			}
			processName = pname;
		}
	}

	/**
	 * Constructs the ProcessIdentifier for the <i>Server</i> process
	 * 
	 * @param node
	 *            The node on which the process is running. Example
	 *            "sw","prod","test". Note that the value does not begin with
	 *            the "\" sign. The value of this field can be <code>null</code>
	 *            .
	 * 
	 * @param pname
	 *            The name of the <i>Server</i> process. Example "X1AK","TEST".
	 *            Note that the value of this parameter does not begin with the
	 *            "$" sign. The value of this parameter cannot be
	 *            <code>null</code>.
	 * @param seqNo
	 *            Specifies the system-assigned sequence number of the process.
	 *            This parameter has a maximum of 13 digits. If <code>-1</code>
	 *            this parameter will be disregarded. Example 4300411433.
	 * @param qualifier1
	 *            Is a string containing 1-7 alphanumeric characters. This
	 *            parameter can be null. Example "term","jmx","user74". Note
	 *            that the value of this parameter does not begin with "#" sign.
	 * @param qualifier2
	 *            Is a string containing 1-8 alphanumeric characters. The first
	 *            character must always be a letter. If this parameter is not
	 *            null then {@link #qualifier1} too should not be null.
	 * @throws IllegalArgumentException
	 *             If any of the parameter constraints are not met.
	 */
	public ProcessIdentifier(String node, String pname, long seqNo,
			String qualifier1, String qualifier2)
			throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open with node: " + node
				+ " process name: " + pname + "seq no:" + seqNo
				+ " qualifier1: " + qualifier1 + " qualifier2: " + qualifier2);
		if (node != null) {
			if (node.length() > 7) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.length"));
			}
			if (Character.isLetter(node.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.firstChar"));
			}
			if (!isValidString(node)) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.valid"));
			}
			this.node = node;
		}
		if (pname == null) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.name.is.null"));
		} else {
			if (pname.length() > 5) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.length"));
			}
			if (Character.isLetter(pname.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.firstChar"));
			}
			if (!isValidString(pname)) {
				throw new IllegalArgumentException(
						Process.sm.getString("process.name.valid"));
			}
			this.processName = pname;
		}

		if (qualifier1 == null && qualifier2 != null) {
			throw new IllegalArgumentException(
					Process.sm.getString("one.qualifier.is.null"));
		}
		if (qualifier1 != null) {
			if (qualifier1.length() > 7) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier1.length"));
			}
			if (Character.isLetter(qualifier1.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier1.firstChar"));
			}
			if (!isValidString(qualifier1)) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier1.valid"));
			}
		}
		if (qualifier2 != null) {
			if (qualifier2.length() > 8) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier2.length"));
			}
			if (Character.isLetter(qualifier2.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier2.firstChar"));
			}
			if (!isValidString(qualifier2)) {
				throw new IllegalArgumentException(
						Process.sm.getString("qualifier2.valid"));
			}
		}
		if (seqNo != -1
				&& (seqNo < 0 || (new String("" + seqNo).length()) > 13)) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.seqno"));
		}
		this.seqNo = seqNo;
		this.qualifier1 = qualifier1;
		this.qualifier2 = qualifier2;
	}

	/**
	 * Constructs the ProcessIdentifier for the <i>Server</i> process
	 * 
	 * @param cpu
	 *            Specifies the processor number of the processor in which the
	 *            process is running. Cpu is one or two digits representing a
	 *            value in the range 0 through 15.
	 * @param pin
	 *            Specifies the process identification number of the process.
	 *            pin is one to five digits representing a value in the range 0
	 *            through the maximum value allowed for the processor.
	 * @param seqNo
	 *            Specifies the system-assigned sequence number of the process.
	 *            This parameter has a maximum of 13 digits. The sequence number
	 *            is mandatory for unnamed processes. The sequence number cannot
	 *            be removed from an unnamed process file name because a fatal
	 *            error will result. Example 4300411433.
	 * 
	 * @throws IllegalArgumentException
	 *             If any of the parameter constraints are not met.
	 */
	public ProcessIdentifier(int cpu, int pin, long seqNo)
			throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open on cpu: " + cpu + " pin: " + pin
				+ "seq no:" + seqNo);
		if (cpu < 0 || cpu > 15) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.cpu"));
		}
		if (pin < 0 || pin > 99999) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.pin"));
		}
		if (seqNo < 0 || (new String("" + seqNo).length()) > 13) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.seqno"));
		}
		this.cpu = cpu;
		this.pin = pin;
		this.seqNo = seqNo;
	}

	/**
	 * Constructs the ProcessIdentifier for the <i>Server</i> process.
	 * 
	 * @param node
	 *            The node on which the process is running. Example
	 *            "sw","prod","test". Note that the value does not begin with
	 *            the "\" sign. The value of this field can be <code>null</code>
	 *            .
	 * 
	 * @param cpu
	 *            Specifies the processor number of the processor in which the
	 *            process is running. Cpu is one or two digits representing a
	 *            value in the range 0 through 15.
	 * @param pin
	 *            Specifies the process identification number of the process.
	 *            pin is one to five digits representing a value in the range 0
	 *            through the maximum value allowed for the processor.
	 * @param seqNo
	 *            Specifies the system-assigned sequence number of the process.
	 *            This parameter has a maximum of 13 digits. The sequence number
	 *            is mandatory for unnamed processes. The sequence number cannot
	 *            be removed from an unnamed process file name because a fatal
	 *            error will result. Example 4300411433.
	 * 
	 * @throws IllegalArgumentException
	 *             If any of the parameter constraints are not met.
	 */
	public ProcessIdentifier(String node, int cpu, int pin, long seqNo)
			throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open with node name: " + node
				+ " cpu: " + cpu + " pin: " + pin + "seq no:" + seqNo);
		if (node != null) {
			if (node.length() > 7) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.length"));
			}
			if (Character.isLetter(node.charAt(0)) == false) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.firstChar"));
			}
			if (!isValidString(node)) {
				throw new IllegalArgumentException(
						Process.sm.getString("node.valid"));
			}
			this.node = node;
		}
		if (cpu < 0 || cpu > 15) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.cpu"));
		}
		if (pin < 0 || pin > 99999) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.pin"));
		}
		if (seqNo < 0 || (new String("" + seqNo).length()) > 13) {
			throw new IllegalArgumentException(
					Process.sm.getString("process.seqno"));
		}
		this.cpu = cpu;
		this.pin = pin;
		this.seqNo = seqNo;
	}

	/**
	 * Returns name of the <i>Server</i> process.
	 * 
	 * @return Name of the <i>Server</i> process.
	 */
	public String getProcessName() {
		return processName;
	}

	/**
	 * Returns the node name where <i>Server</i> process is running.
	 * 
	 * @return Node name where <i>Server</i> process is running.
	 */
	public String getNode() {
		return node;
	}

	/**
	 * Returns the system-assigned sequence number of the process.
	 * 
	 * @return Sequence number of Server process.
	 */
	public long getSeqNo() {
		return seqNo;
	}

	/**
	 * Returns the qualifier1,qualifier1 is a string containing 1-7 alphanumeric
	 * characters.
	 * 
	 * @return A string containing 1-7 alphanumeric characters.
	 */
	public String getQualifier1() {
		return qualifier1;
	}

	/**
	 * Returns the qualifier2,qualifier2 is a string containing 1-8 alphanumeric
	 * characters.
	 * 
	 * @return A string containing 1-8 alphanumeric characters.
	 */
	public String getQualifier2() {
		return qualifier2;
	}

	/**
	 * Returns the processor number of the processor in which the process is
	 * running.
	 * 
	 * @return The processor number of the processor in which the process is
	 *         running.
	 */
	public int getCpu() {
		return cpu;
	}

	/**
	 * Returns the process identification number of the process.
	 * 
	 * @return The process identification number of the process.
	 */
	public int getPin() {
		return pin;
	}

	private boolean isValidString(String seq) {
		int len = seq.length();
		for (int i = 0; i < len; i++) {
			char c = seq.charAt(i);
			if ('0' <= c && c <= '9')
				continue;
			if ('a' <= c && c <= 'z')
				continue;
			if ('A' <= c && c <= 'Z')
				continue;
			return false;
		}
		return true;
	}

	public String toString() {
		StringBuffer buffer = new StringBuffer();
		buffer.append("PROCESS:")
				.append(processName != null ? processName : "")
				.append(":NODE:").append(node != null ? node : "")
				.append(":CPU:").append(cpu).append(":PIN:").append(pin)
				.append(":SEQNO:").append(seqNo).append(":QUALIFIER1:")
				.append(qualifier1 != null ? qualifier1 : "")
				.append(":QUALIFIER2:")
				.append(qualifier2 != null ? qualifier2 : "");
		return buffer.toString();
	}
}