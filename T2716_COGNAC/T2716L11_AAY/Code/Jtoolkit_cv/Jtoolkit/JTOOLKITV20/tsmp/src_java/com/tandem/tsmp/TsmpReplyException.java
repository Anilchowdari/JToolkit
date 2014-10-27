/*
 * @(#)TsmpReplyException.java	1.0 98/03/06
 * 
 *  @@@ START COPYRIGHT @@@
 *
 *             Copyright 2002 Compaq Computer Corporation
 *             Protected as an unpublished work.
 * 
 * The computer program listings, specifications and documentation contained
 * herein are the property of Compaq Computer Corporation or its supplier
 * and may not be reproduced, copied, disclosed, or used in whole or part for
 * any reason without the prior express written permission of Compaq Computer
 * Corporation.
 *
 *  @@@ END COPYRIGHT @@@
 * 
 */

package com.tandem.tsmp;
import java.util.*;

/**
 * This exception indicates that an error has occurred while trying to 
 * convert data for either a TsmpServerReply or a TsmpGenericServerReply. 
 *
 * @author  Steve Rispoli
 * @version 1.0, 03/06/98
 * @see     com.tandem.tsmp.TsmpServer
 * @see     com.tandem.tsmp.TsmpServerReply
 * @see     com.tandem.tsmp.TsmpGenericServerReply
 * @since   JDK1.1
 */

public
class TsmpReplyException extends TsmpServerException {

    /**
     * Constructs an <code>TsmpReplyException</code> with the specified
     * descriptive message. The fieldList vector is undefined.
     *
     * @param   msg the descriptive message
     * @since   JDK1.1
     */

    public TsmpReplyException(String msg) {
        super(msg);
    }

    /**
     * Constructs an <code>TsmpReplyException</code> with the specified
     * descriptive message and the vector containing information on which 
     * fields had conversion errors.
     *
     * @param msg the descriptive message
     * @param list the list of fields that had conversion errors
     * @since   JDK1.1
     */

    public TsmpReplyException(String msg, Vector list) {
        super(msg + " " + list);
        fieldList = list;
    }

    /**
     * Return the list of unconverted fields as a String.
     */

    public String getUnconvertedFields() {
        return fieldList.toString();
    }

    /**
     * fieldList the list of fields for the given class that have conversion
     * errors associated with them.
     * @since JDK1.1
     */

    private Vector fieldList;

}
