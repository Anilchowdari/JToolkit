#
# Makefile for com.tandem.ext.guardian and
#              com.tandem.ext.util packages
#
# (cross-compiler version)
# 

# Include file that defines location of compiler, java, pthreads etc.
include ../Defs.gmk

##
## CLASSPATH
##
export CLASSPATH
CLASSPATH=.:./classes:$(JAVA_HOME)/jre/lib/rt.jar

# Create relinkable library file rather than archive file
# Strip symbols for non-debug builds
#LD_FLAGS = -Wr -Wx
#LD_DEBUG_FLAGS = -Wr

INCLUDE_DIRS = -I./$(STUBS) -I$(JNI_OSS_INCL)/include -I$(JNI_OSS_INCL)/include/oss \
	-I$(CSRC) -I$(PTHREADS_DIR)


##
## Source and target directories
## 
JAVASRC=src_java
CSRC=src_c
CLASSBIN=classes
STUBS=stubs
OBJ=obj

#Used for debug builds
SUFFIX= _g

##
## Package Name
##
LIB = guardian
PKGPATH = com/tandem/ext
SUBDIRS = util guardian

##
## Package Source Files 
##

# Add Java source files here
FILES.java = \
	$(JAVASRC)/$(PKGPATH)/util/DataConversion.java \
	$(JAVASRC)/$(PKGPATH)/util/DataConversionException.java \
	$(JAVASRC)/$(PKGPATH)/guardian/GuardianInput.java \
	$(JAVASRC)/$(PKGPATH)/guardian/GuardianOutput.java \
	$(JAVASRC)/$(PKGPATH)/guardian/GError.java \
	$(JAVASRC)/$(PKGPATH)/guardian/GuardianException.java \
	$(JAVASRC)/$(PKGPATH)/guardian/GuardianWarning.java \
	$(JAVASRC)/$(PKGPATH)/guardian/ProcessHandle.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgCancel.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgCpuDown.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgCpuUp.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgOpen.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgClose.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgNodeDown.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgNodeUp.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgRemoteCpuDown.java \
	$(JAVASRC)/$(PKGPATH)/guardian/SysMsgRemoteCpuUp.java \
	$(JAVASRC)/$(PKGPATH)/guardian/ReceiveInfo.java \
	$(JAVASRC)/$(PKGPATH)/guardian/ReceiveNoOpeners.java \
	$(JAVASRC)/$(PKGPATH)/guardian/Receive.java 

# Add C files to this list
FILES.c = \
	$(CSRC)/util/DataConversion.c \
	$(CSRC)/guardian/tmrqst.c \
	$(CSRC)/guardian/ProcessHandle.c \
	$(CSRC)/guardian/Receive.c \
	$(CSRC)/guardian/ReceiveInfo.c 

# List of object files for optimized build
FILES.o = $(FILES.c:$(CSRC)/%.c=$(OBJ)/%.o)
# List of object files for debug build
FILESD.o = $(FILES.c:$(CSRC)/%.c=$(OBJ)$(SUFFIX)/%.o)

FILES.class = $(FILES.java:$(JAVASRC)/%.java=$(CLASSBIN)/%.class)
FILES.class.compile = $(subst /,.,$(FILES.class:$(CLASSBIN)/%.class=%))

# What's this used for ?
#FILES_stubs.h = $(FILES.class:$(CLASSBIN)/$(PKGPATH)/%.class=$(STUBS)/%.h)

##
## Rules
##

# 'all' makes the jar file, regenerates the jni header files and makes the optimized
# and non-optimized library files.  Building javadoc files is not part of this rule.
# Javadoc files are built for the customer using the top-level makefile but developers
# that just want to check that they didn't break javadoc can run "gmake javadoc"
# using this makefile
all: $(LIB).jar .stubs library 

library: $(LIB).lib $(LIB)$(SUFFIX).lib

javadoc : .doc $(FILES.java) $(LIB).jar
	$(JAVADOC) -d doc -use -serialwarn \
	-linkoffline http://download.oracle.com/javase/1.4.2/docs/api . \
	-classpath ./$(LIB).jar:$(CLASSPATH)  \
	-sourcepath ./src_java \
	-protected -windowtitle "NonStop(TM) Server for Java(TM) Extensions" \
	com.tandem.ext.guardian com.tandem.ext.util 

clean :
	rm -rf .class* .stubs 
	rm -rf .doc doc $(LIB).jar $(LIB).lib $(LIB)$(SUFFIX).lib
	rm -rf $(CLASSBIN) $(OBJ) $(OBJ)$(SUFFIX) $(STUBS)

## ------------------------------------------------------
## --- Should be no need to modify from this point on ---
## ------------------------------------------------------
##
##
## Rules for building package bytecode
## 

.delete.classlist:
	@/bin/rm -f .classes.list

.compile.classlist:
	if [ -s .classes.list ] ; \
	then $(JAVAC)  -d $(CLASSBIN) \
	    $(JAVACFLAGS)  \
	    $(shell if [ -s .classes.list ] ; then cat .classes.list; fi ) ; \
	fi

$(CLASSBIN):
	@-mkdir -p $@

$(CLASSBIN)/%.class : $(JAVASRC)/%.java
	@echo $? >>.classes.list

# What purpose does the hidden directory .classes serve? 
.classes: $(CLASSBIN) .delete.classlist $(FILES.class) .compile.classlist
	@touch .classes

$(LIB).jar: $(CLASSBIN) .classes
	rm -f $(LIB).jar; \
	cd $(CLASSBIN); \
	$(JAR) cmv0f ../man.txt ../$@ *; \
	cd ..; \

# Create the doc directory for the javadoc build step
.doc:
	@-mkdir -p doc

##
## Rules for building native stubs (JNI style)
## 
## - This will build the *.h (jni) stub files.
##
# What purpose does the hidden directory .stubs serve?  There used to be a hidden
# directory called .native which I got rid of (do I need it?).
.stubs: $(FILES.class) 
	$(JAVAH) -jni -d $(STUBS) \
	$(FILES.class.compile)
	@touch .stubs

##
## Rules for building native library
##
## - Compile your own native implementation with IEEE floating point support  

$(OBJ):
	@-mkdir -p $@
	@for i in $(SUBDIRS) ; do \
	   echo "making $@/$$i" ; \
	   mkdir -p $@/$$i ; \
	done

$(OBJ)$(SUFFIX):
	@-mkdir -p $@
	@for i in $(SUBDIRS) ; do \
	   echo "making $@/$$i" ; \
	   mkdir -p $@/$$i ; \
	done

$(OBJ)/%.o :: $(CSRC)/%.c 
	$(CC) $(CC_FLAGS) $(CC_FLAGS_IEEE) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(OBJ)$(SUFFIX)/%.o :: $(CSRC)/%.c 
	$(CC) $(CC_DEBUG_FLAGS) $(CC_FLAGS_IEEE) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(LIB).lib: .stubs $(OBJ) $(FILES.o)
	$(LD) $(LD_FLAGS) $(LD_FLAGS_IEEE) $(FILES.o) -o $@

$(LIB)$(SUFFIX).lib: .stubs $(OBJ)$(SUFFIX) $(FILESD.o)
	$(LD) $(LD_DEBUG_FLAGS) $(LD_FLAGS_IEEE) $(FILESD.o) -o $@

