package com.hp.nonstop.io.process;

import java.util.logging.Level;
import java.util.logging.Logger;

import com.hp.nonstop.io.process.impl.NSProcessConstants;

/**
 * Class representing the options to open the <i>Server</i> process
 * <p>
 * The following are the options
 * <ul>
 * 
 * 
 * <li>Access: A process can be opened in either Read-write or Write-only mode.
 * In Read-write mode the requester engages the server in two-way communication.
 * In Write-only mode the requester engages the server in one-way communication.
 * The possible values are {@link #ACCESS_READ_WRITE},
 * {@link #ACCESS_WRITE_ONLY}</li>
 * 
 * 
 * 
 * <li>Exclusion: Specifies the desired mode of compatibility with other
 * openers for the server process.</li>
 * 
 * 
 * <li>Timeout: The time (in milliseconds) to wait for the I/O operations to
 * complete.</li>
 * 
 * </ul>
 * </p>
 * 
 * 
 * <h2>Timeout considerations</h2>
 * The timeout value indicates how long the calls to <code>read</code> of
 * {@link ProcessInputStream} will wait for a response from the server process.
 * The I/O operation is not canceled if the timeout expires.<br/> In case of
 * the <code>service</code> methods of {@link Process} the I/O is canceled if
 * the operation times out.
 * 
 * <h2>Message size considerations</h2>
 * The IPC message between the requester and the server can range from 0 bytes
 * to {2097152} bytes. The size of internal message buffers can be optimized
 * based on the maximum size of message that can be exchanged between the
 * requester and server. Its recommended that {maxRequestMessageSize} be used to
 * set the maximum request message size and {maxResponseMessageSize} be used to
 * set the maximum response message size.
 * 
 * @author Vyom Tewari
 */
public class ProcessOpenOption {

	/**
	 * Access mode of the process opened will be READ and WRITE.
	 */
	public static final short ACCESS_READ_WRITE = 0;

	/**
	 * Access mode of the process opened will be WRITE only. <BR>
	 * <BR>
	 * The connection established using WRITE only option, will allow only write
	 * operations to be called. An attempt to call read on the opened process
	 * will throw an exception.
	 */
	public static final short ACCESS_WRITE_ONLY = 2;

	/**
	 * Exclusion mode of the process opened will be Shared. This is the only
	 * valid exclusion value
	 */
	public static final short EXCLUSION_SHARED = 0;

	/**
	 * Specify the default access mode of the process will be opened.
	 */
	private short access = ACCESS_READ_WRITE;

	/**
	 * Default buffer size, this will be used if user does not set buffer size.
	 */
	public static final int DEFAULT_MESSAGE_SIZE = NSProcessConstants.DEFAULT_MESSAGE_SIZE;

	/**
	 * Default Access mode of the process opened will be READ and WRITE.
	 */
	public static final short DEFAULT_ACCESS_MODE = 0;

	/**
	 * Default exclusion mode of the process opened will be Shared.
	 */
	public static final short DEFAULT_EXCLUSION_MODE = 0;

	/**
	 * Default nowait-depth mode of the process opened will be 1.
	 */
	public static final short DEFAULT_NOWAIT_DEPTH = 1;

	/**
	 * Default timeout of the process opened will be indefinite.
	 */
	public static final long DEFAULT_TIMEOUT = -1;

	/**
	 * Indefinite time out.
	 */
	private static final long INDEFINITE = -1;

	/**
	 * Exclusion mode of the process opened.
	 */
	private short exclusion = EXCLUSION_SHARED;

	/**
	 * Nowait-depth of the process opened.
	 */
	private short nowaitdepth = DEFAULT_NOWAIT_DEPTH;

	/**
	 * Max response message size of the process opened.
	 */
	private int maxResponseMessageSize = DEFAULT_MESSAGE_SIZE;

	/**
	 * Max request message size of the process opened.
	 */
	private int maxRequestMessageSize = DEFAULT_MESSAGE_SIZE;
	/**
	 * Time in milliseconds. The WRITE_READX_ call will return time-out error if
	 * the server didn't respond by this time-out value. The default time out
	 * value is -1(INDEFINITE) seconds.
	 */
	private long timeout = DEFAULT_TIMEOUT;

	private static Logger logger = Logger
			.getLogger("com.hp.nonstop.io.process.ProcessOpenOption");

	/**
	 * default constructor.
	 */
	public ProcessOpenOption() {
	}

	/**
	 * @param access
	 *            The access mode, in which connection to the stand-alone
	 *            NonStop server process will be opened.
	 * @param exclusion
	 *            The exclusion mode, specifying the desired mode of
	 *            compatibility with other openers of the process. The only
	 *            valid value is {@link #EXCLUSION_SHARED}
	 * @param timeout
	 *            The time in which read will timed out, if no data is
	 *            available.
	 * 
	 * @throws IllegalArgumentException
	 *             If any of the parameter constraints are not met.
	 */
	public ProcessOpenOption(short access, short exclusion, long timeout)
			throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open with access: " + access
				+ " exclusion: " + exclusion + " timeout: " + timeout);
		if (access != ACCESS_READ_WRITE && access != ACCESS_WRITE_ONLY)
			throw new IllegalArgumentException(Process.sm
					.getString("access.invalid"));
		if (exclusion != EXCLUSION_SHARED)
			throw new IllegalArgumentException(Process.sm
					.getString("exclusion.invalid"));
		if (nowaitdepth != DEFAULT_NOWAIT_DEPTH)
			throw new IllegalArgumentException(Process.sm
					.getString("nowaitdepth.invalid"));
		if (timeout == INDEFINITE || timeout > 0) {
			this.timeout = timeout;
		} else {
			throw new IllegalArgumentException(Process.sm
					.getString("timeout.invalid"));
		}
		this.access = access;
		this.exclusion = exclusion;
	}

	/**
	 * @param access
	 *            The access mode, in which connection to the stand-alone
	 *            NonStop server process will be opened.
	 * @param exclusion
	 *            The exclusion mode, specifying the desired mode of
	 *            compatibility with other openers of the process. The only
	 *            valid value is {@link #EXCLUSION_SHARED}
	 * @param timeout
	 *            The time in which read will timed out, if no data is
	 *            available.
	 * @param maxRequestMessageSize
	 *            Sets the max request message size to be used when
	 *            communicating with the server process.
	 * @param maxResponseMessageSize
	 *            Sets the max response message size to be used when
	 *            communicating with the server process.
	 * @throws IllegalArgumentException
	 *             If any of the parameter constraints are not met.
	 */
	public ProcessOpenOption(short access, short exclusion, long timeout,
			int maxRequestMessageSize, int maxResponseMessageSize)
			throws IllegalArgumentException {
		logger.log(Level.FINER, "Process open with access: " + access
				+ " exclusion: " + exclusion + " timeout: " + timeout
				+ "maxRequestMess: " + maxRequestMessageSize
				+ " maxResponseSize: " + maxResponseMessageSize);
		if (access != ACCESS_READ_WRITE && access != ACCESS_WRITE_ONLY)
			throw new IllegalArgumentException(Process.sm
					.getString("access.invalid"));
		if (exclusion != EXCLUSION_SHARED)
			throw new IllegalArgumentException(Process.sm
					.getString("exclusion.invalid"));
		if (nowaitdepth != DEFAULT_NOWAIT_DEPTH)
			throw new IllegalArgumentException(Process.sm
					.getString("nowaitdepth.invalid"));
		if (timeout == INDEFINITE || timeout > 0) {
			this.timeout = timeout;
		} else {
			throw new IllegalArgumentException(Process.sm
					.getString("timeout.invalid"));
		}
		if (maxRequestMessageSize <= 0
				|| maxRequestMessageSize > (NSProcessConstants.MAX_MESSAGE_SIZE)) {
			throw new IllegalArgumentException(Process.sm
					.getString("max.request.message.size"));
		}
		if (maxResponseMessageSize <= 0
				|| maxResponseMessageSize > (NSProcessConstants.MAX_MESSAGE_SIZE)) {
			throw new IllegalArgumentException(Process.sm
					.getString("max.response.message.size"));
		}
		this.access = access;
		this.exclusion = exclusion;
		this.maxRequestMessageSize = maxRequestMessageSize;
		this.maxResponseMessageSize = maxResponseMessageSize;
	}

	/**
	 * Returns the timeout used for communicating with the server process.
	 * 
	 * @return The timeout used for communicating with the server process.
	 */
	public final long getTimeout() {
		return timeout;
	}

	/**
	 * Returns the exclusion mode used while establishing the communication
	 * channel.
	 * 
	 * @return The exclusion mode used while establishing the communication
	 *         channel.
	 */
	public final short getExclusion() {
		return exclusion;
	}

	/**
	 * Returns the access mode used while establishing the communication
	 * channel.
	 * 
	 * @return The access mode used while establishing the communication
	 *         channel.
	 */
	public final short getAccess() {
		return access;
	}

	/**
	 * Return the nowait-depth to be used when communicating with the server
	 * process.
	 * 
	 * @return The nowait-depth to be used when communicating with the server
	 *         process.
	 */
	public final short getNowaitdepth() {
		return nowaitdepth;
	}

	/**
	 * Return the max request message size to be used when communicating with
	 * the server process.
	 * 
	 * @return The max request message size to be used when communicating with
	 *         the server process.
	 */
	public final int getMaxRequestMessageSize() {
		return maxRequestMessageSize;
	}

	/**
	 * Return the max response message size to be used when communicating with
	 * the server process.
	 * 
	 * @return The max response message size to be used when communicating with
	 *         the server process.
	 */
	public final int getMaxResponseMessageSize() {
		return maxResponseMessageSize;
	}

	public String toString() {
		StringBuffer buffer = new StringBuffer();
		buffer.append("TIMEOUT:").append(timeout).append(":EXCLUSION:").append(
				exclusion).append(":MAXREQUESTSIZE:").append(
				maxRequestMessageSize).append(":MAXRESPONSESIZE:").append(
				maxResponseMessageSize);
		return buffer.toString();
	}
}