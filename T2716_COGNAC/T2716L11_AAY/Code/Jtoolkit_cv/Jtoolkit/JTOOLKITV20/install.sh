#!/bin/ksh

# @(#)install.sh  3.0 04/09/21
#
#---------------------------------------------------------------------------
# Copyright 2004
# Hewlett-Packard Development Company, L.P.
# Copyright 2002 Compaq Computer Corporation
# Protected as an unpublished work.
# All rights reserved.
#
# The computer program listings, specifications and
# documentation herein are the property of Compaq Computer
# Corporation and successor entities such as Hewlett-Packard
# Development Company, L.P., or a third party supplier and
# shall not be reproduced, copied, disclosed, or used in whole
# or in part for any reason without the prior express written
# permission of Hewlett-Packard Development Company, L.P.

#---------------------------------------------------------------------------
#--- By default, the JToolkit native library file (libtdmext.lib) will be 
#--- moved to /usr/tandem/java_public_lib for NonStop Server for Java 4 or 
#--- to /usr/tandem/java_public_lib_jdk142 for version 2 of NonStop Server for 
#--- Java 4 before rebuilding the JVM.
#--- To move the native library file to a different location other than the 
#--- default,change the NATIVE_LIB_DIR to where it should be moved.
#
#--- For example, if NonStop Server for Java 4 is installed in a 
#--- non-standard location /home/jdev/nssjava/jdk141_v10 and you
#--- wish to copy the native library file to
#--- /home/jdev/java_public_lib then alter the NATIVE_LIB_DIR variable
#--- as NATIVE_LIB_DIR=/home/jdev.
#
#--- if version 2 of NonStop Server for Java 4 is installed in a 
#--- non-standard location /home/jdev/nssjava/jdk142_v20 and you wish to
#--- copy the native library file to /home/jdev/java_public_lib_jdk142
#--- then alter the NATIVE_LIB_DIR variable as NATIVE_LIB_DIR=/home/jdev.
#
#--------------------------------------------------------------------------    

NATIVE_LIB_DIR=/usr/tandem

#--------------------------------------------------------------------------
#       Nothing below this line should need to be edited.
#--------------------------------------------------------------------------

#Check for JAVA_HOME environment variable
if [ -z "$JAVA_HOME" ]
then
print "Environment variable JAVA_HOME is not set."
print "Set the JAVA_HOME environment variable and run install again."
print "Installation aborted."
exit 1
fi

#Check for Java Version
if  $JAVA_HOME/bin/java -version 2>&1 | /bin/grep java | /bin/grep "1.4.2" >> Setlib 
then
NATIVE_LIB_DIR=$NATIVE_LIB_DIR/java_public_lib_jdk142
print "Installing with version 2 of NonStop Server for Java 4"
elif $JAVA_HOME/bin/java -version 2>&1 | /bin/grep java | /bin/grep "1.4.1" >> Setlib
then
NATIVE_LIB_DIR=$NATIVE_LIB_DIR/java_public_lib
print "Installing with NonStop Server for Java 4"
else
print "Installation Aborted"
print "Check for Java Version"
exit 1
fi

rm -rf Setlib

if [ ! -f $JAVA_HOME/install/Makefile ]
then
print "Could not find $JAVA_HOME/install/Makefile."
print "Set the correct path for the JAVA_HOME environment variable."
print "Installation aborted." 
exit 1 
fi

print -n "This will install NonStop HP JToolkit in $JAVA_HOME."
print -n "Proceed (y/n)?"
read PROMPT
if [ "$PROMPT" = "y" -o "$PROMPT" = "Y" ]
then
print "Installing HP NonStop JToolkit..."
else
print "Installation aborted. "
exit 1
fi   

#Check for the directory java_public_lib or java_public_lib_jdk142
if [ ! -d $NATIVE_LIB_DIR ]
then
print "Directory $NATIVE_LIB_DIR does not exists."
print -n "Creating directory $NATIVE_LIB_DIR..."
mkdir -p $NATIVE_LIB_DIR
  if [ $? -ne 0 ]
  then
  print "\nCould not create directory $NATIVE_LIB_DIR"
  print "Installation aborted."
  exit 2
  else
  print "Done."
  fi
fi           

#Check for Install Dir Standard or Non Standard
standard="FALSE"
if echo ${NATIVE_LIB_DIR} | grep "/usr/tandem/java_public_lib*" >> install.log
then
standard="TRUE"
fi

rm -rf install.log

#Copy the libtdmext.lib file to NATIVE_LIB_DIR
print -n "Copying file libtdmext.lib to $NATIVE_LIB_DIR.."
cp lib/libtdmext.lib $NATIVE_LIB_DIR
if [ $? -ne 0 ]
then
print "Error."
print "Could not copy libtdmext.lib to $NATIVE_LIB_DIR."
print "Installation aborted."
exit 2
else
print "done."
fi

#
# prompt user if rebuilding of the VM is needed
#
print -n "Copying files are done.  Do you want to rebuild the VM (y/n)?"
read PROMPT
if [ "$PROMPT" = "y" -o "$PROMPT" = "Y" ]
then 
# Make sure the CUSTLIB_DIRS contains the Non Standard Location
  if [ "$standard" = "FALSE" ]
  then
  print -n "Is the Non-Standard lib location "
  print -n  $NATIVE_LIB_DIR
  print " included in the CUSTLIB_DIRS of the Java Makefile.(y/n)? " 
  read PROMPT
    if  ! [ "$PROMPT" = "y" -o  "$PROMPT" = "Y" ]
    then
    print "Install is done without rebuilding the VM."
    exit 2
    fi
  fi 
echo make -f $JAVA_HOME/install/Makefile
make -f $JAVA_HOME/install/Makefile 
print "Install is done.  The VM is rebuilt."
else
print "Install is done without rebuilding the VM."
fi 

