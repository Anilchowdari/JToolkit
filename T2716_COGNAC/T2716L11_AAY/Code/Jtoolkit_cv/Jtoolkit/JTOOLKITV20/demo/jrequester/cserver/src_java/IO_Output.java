import java.util.HashMap;
import java.util.MissingResourceException;

import com.tandem.ext.util.DataConversion;
import com.tandem.ext.util.DataConversionException;

/**
 * IO_Output class was generated for DDL Rec/Def: output on: 3/30/11 at:
 * 22:46:28. This is an INPUT/OUTPUT class which can be used to read and write
 * data.
 */

public class IO_Output implements com.tandem.ext.guardian.GuardianInput,
		com.tandem.ext.guardian.GuardianOutput {
	protected int _numConversionErrors;
	protected boolean _allowErrors = false;
	protected HashMap _errorDetail;

	protected int _bufferLen = 260;
	public String message;
	public int retcode;

	/**
	 * IO_Output Constructor
	 */
	public IO_Output() {
		_errorDetail = new HashMap();
	}

	/**
	 * Returns the expected length of the byte array used by this class.
	 * 
	 * @return int expected length of byte array.
	 */
	public int getLength() {
		return _bufferLen;
	}

	/**
	 * Indicates whether DataConversion Exceptions are thrown during execution
	 * of the unmarshal method. Specifying false indicates that the first data
	 * conversion error encountered during execution of the unmarshal method,
	 * causes a DataConversionException to be thrown. All subsequent data in the
	 * byte array supplied to the unmarshal method is NOT converted
	 * 
	 * Specifying true indicates that DataConversionExceptions are ignored
	 * during execution of the unmarshal method. umarshal will attempt to
	 * convert all data in the byte array. To determine if errors have occurred
	 * during execution of unmarshal, the user can call
	 * getNumConversionFailures(). This method returns the number of fields that
	 * failed to convert. If the call to getNumConversionFailures() returns a
	 * number greater than zero, a call to getConversionFailureDetails() returns
	 * a HashMap whose keys are the fields that failed to convert and whose
	 * value is the associated DataConversionException.
	 * 
	 * @param boolean
	 *            true to indicate that conversion errors are to be allowed;
	 *            false otherwise.
	 * @see #getNumConversionFailures
	 * @see #getConversionFailureDetails
	 */
	public void allowErrors(boolean value) {
		_allowErrors = value;
	}

	/**
	 * Returns the number of fields that failed to convert during execution of
	 * the unmarshal method.
	 * 
	 * @return the number of fields that failed to convert during the execution
	 *         of unmarshal. A returned value of zero indicates that all fields
	 *         converted.
	 * @see #allowErrors
	 * @see #getConversionFailureDetails
	 */
	public int getNumConversionFailures() {
		return _numConversionErrors;
	}

	/**
	 * Returns a HashMap with information about fields that failed to convert
	 * during execution of the unmarshal method. Keys are the fields that failed
	 * to convert and values are the associated DataConversionExceptions.
	 * 
	 * @return HashMap containing details of data conversion failures. If no
	 *         failures occurred the HashMap.isEmpty() method returns true.
	 * @see #getNumConversionFailures
	 * @see #allowErrors
	 */
	public HashMap getConversionFailureDetails() {
		return _errorDetail;
	}

	/**
	 * Used to set a value for message.
	 * 
	 * @param value
	 *            the value to be set.
	 */
	public void setMessage(String value) {
		message = value;
	}

	/**
	 * Used to set a value for retcode.
	 * 
	 * @param value
	 *            the value to be set.
	 */
	public void setRetcode(int value) {
		retcode = value;
	}

	/**
	 * Used to clear any existing values for message.
	 */
	public void clearMessage() {
		message = null;
	}

	/**
	 * Used to clear any existing values for retcode.
	 */
	public void clearRetcode() {
		retcode = (int) 0;
	}

	/**
	 * Used to clear the values of all instance variables in this class.
	 */
	public void clear_All() {
		message = null;
		retcode = (int) 0;
	}

	/**
	 * Used to obtain the value of message.
	 * 
	 * @return the value for message.
	 */
	public String getMessage() {
		return (message);
	}

	/**
	 * Used to obtain the value of retcode.
	 * 
	 * @return the value for retcode.
	 */
	public int getRetcode() {
		return (retcode);
	}

	/**
	 * Moves the data from the supplied byte array to the instance variables.
	 * 
	 * @param _buffer
	 *            a byte array which contains the data to be moved.
	 * @param _count
	 *            the number of bytes to be converted.
	 * @exception com.tandem.ext.util.DataConversionException
	 *                if a field fails to convert correctly and allowErrors has
	 *                been set to false; or allowErrors is set to true and
	 *                _count < getLength().
	 * @see #allowErrors
	 */
	public void unmarshal(byte[] _buffer, int _count)
			throws com.tandem.ext.util.DataConversionException {

		int _offset;
		int _fieldSize;
		_errorDetail.clear();
		_numConversionErrors = 0;
		if (!_allowErrors && _count < _bufferLen) {
			_numConversionErrors++;
			throw new DataConversionException(
					DataConversionException.ConvError.PARAM,
					"The value supplied for the _count variable is less than expected (see getLength())");
		}
		if (_allowErrors && _count < _bufferLen) {
			DataConversionException _bfe = new DataConversionException(
					DataConversionException.ConvError.PARAM,
					"The value supplied for the _count variable is less than expected (see  getLength())");
			_numConversionErrors++;
			_errorDetail.put(_buffer, _bfe);
		}
		String encodeSetting = new String();

		String defaultEncoding = "UTF8";
		if (defaultEncoding == null) {
			throw new MissingResourceException(
					"Property not found for character encoding default : ",
					this.getClass().getName(), "UTF8");
		}

		_offset = 0;
		if (_offset >= _count)
			return;
		_fieldSize = Math.min(255, _count - _offset);
		encodeSetting = System.getProperty("message.encoding", defaultEncoding);
		try {
			message = DataConversion.CStrToJavaStr(_buffer, _offset,
					_fieldSize, encodeSetting);
		} catch (DataConversionException dce) {
			_numConversionErrors++;
			if (_allowErrors) {
				_errorDetail.put("message", dce);
			} else {
				throw dce;
			}
		}

		_offset = 256;
		if (_offset + 4 > _count)
			return;
		try {
			retcode = DataConversion.BIN32ToJavaInt(_buffer, _offset);
		} catch (DataConversionException dce) {
			_numConversionErrors++;
			if (_allowErrors) {
				_errorDetail.put("retcode", dce);
			} else {
				throw dce;
			}
		}
	}

	/**
	 * Moves and converts the data from the instance variables to a byte array
	 * 
	 * @return a byte array containing the converted data.
	 * @exception com.tandem.ext.util.DataConversionException
	 *                if an instance variable fails to convert successfully.
	 */

	public byte[] marshal() throws com.tandem.ext.util.DataConversionException,
			NumberFormatException {

		byte[] _buffer = new byte[_bufferLen];
		marshal(_buffer);
		return _buffer;
	}

	/**
	 * Moves and converts the data from the instance variables to a byte array
	 * 
	 * @param _buffer
	 *            the byte array to which the data is to be moved.
	 * @return a byte array containing the converted data.
	 * @exception com.tandem.guardian.DataConversionException
	 *                if the length of the supplied byte array is not greater
	 *                than or equal to the minimum required length or if an
	 *                instance variable fails to convert successfully.
	 */

	public byte[] marshal(byte[] _buffer)
			throws com.tandem.ext.util.DataConversionException {
		if (_buffer.length < _bufferLen)
			throw new DataConversionException(
					DataConversionException.ConvError.RANGE,
					"Buffer length is less than minimum required buffer length");

		int _offset;
		String encodeSetting = new String();

		String defaultEncoding = "UTF8";
		if (defaultEncoding == null) {
			throw new MissingResourceException(
					"Property not found for character encoding default : ",
					this.getClass().getName(), "UTF8");
		}

		_offset = 0;
		if (message != null) {
			encodeSetting = System.getProperty("message.encoding",
					defaultEncoding);
			DataConversion.JavaStrToCStr(_buffer, message, _offset, 255,
					encodeSetting);
		}

		_offset = 256;
		DataConversion.JavaIntToBIN32(_buffer, retcode, _offset);
		return _buffer;
	}
}
