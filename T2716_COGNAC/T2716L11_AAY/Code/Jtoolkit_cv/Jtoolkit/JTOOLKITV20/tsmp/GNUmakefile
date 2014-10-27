#
# Makefile for com.tandem.tsmp package
# 
# (cross-compiler version)
# 

include ../Defs.gmk

OTHER_CLASSES = ../guardian/guardian.jar

##
## CLASSPATH
##
export CLASSPATH
CLASSPATH=.:./classes:$(OTHER_CLASSES):$(JAVA_HOME)/jre/lib/rt.jar

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
LIB=tsmp
PKGPATH=com/tandem/tsmp
PKGNAME=com.tandem.tsmp

##
## Package Source Files 
##

FILES.java = \
	$(JAVASRC)/$(PKGPATH)/TsmpServerException.java \
	$(JAVASRC)/$(PKGPATH)/TsmpReplyException.java \
	$(JAVASRC)/$(PKGPATH)/TsmpRequestException.java \
	$(JAVASRC)/$(PKGPATH)/TsmpSendException.java \
	$(JAVASRC)/$(PKGPATH)/TsmpServerUnavailableException.java \
        $(JAVASRC)/$(PKGPATH)/TsmpDialog.java \
	$(JAVASRC)/$(PKGPATH)/TsmpServer.java \
	$(JAVASRC)/$(PKGPATH)/TsmpServerReply.java \
	$(JAVASRC)/$(PKGPATH)/TsmpGenericServerReply.java \
	$(JAVASRC)/$(PKGPATH)/TsmpServerRequest.java 

FILES.c = \
	$(CSRC)/convert.c \
        $(CSRC)/context.c \
	$(CSRC)/clientRequest.c 

# List of object files for optimized build
FILES.o = $(FILES.c:$(CSRC)/%.c=$(OBJ)/%.o)
# List of object files for debug build
FILESD.o = $(FILES.c:$(CSRC)/%.c=$(OBJ)$(SUFFIX)/%.o)

FILES.class = $(FILES.java:$(JAVASRC)/%.java=$(CLASSBIN)/%.class)
FILES.class.compile = $(subst /,.,$(FILES.class:$(CLASSBIN)/%.class=%))

##
## Rules
##

# 'all' makes the jar file, regenerates the jni header files and makes the optimized
# and non-optimized library files.  Building javadoc files is not part of this rule.
# Javadoc files are built for the customer using the top-level makefile but developers
# that just want to check that they didn't break javadoc can run "gmake javadoc"
# using this makefile
all : $(LIB).jar .stubs library 

library : $(LIB).lib $(LIB)$(SUFFIX).lib

javadoc : .doc $(FILES.java) $(LIB).jar
	$(JAVADOC) -d doc -use -serialwarn \
	-linkoffline http://download.oracle.com/javase/1.4.2/docs/api . \
	-classpath ./$(LIB).jar:$(CLASSPATH)  \
	-sourcepath ./src_java \
	-protected -header "NonStop(TM) Server for Java(TM) Extensions - Pathsend" \
	-windowtitle "NonStop(TM) Server for Java(TM) Extensions" \
	$(PKGNAME)

clean :
	rm -rf .class* .stubs
	rm -rf .doc doc $(LIB).jar $(LIB).lib $(LIB)$(SUFFIX).lib
	rm -rf $(CLASSBIN) $(OBJ) $(OBJ)$(SUFFIX) $(STUBS)

## ------------------------------------------------------
## --- Should be no need to modify from this point on ---
## ------------------------------------------------------
##
## Rules for building package bytecode
## 

.delete.classlist:
	@/bin/rm -f .classes.list

.compile.classlist:
	if [ -s .classes.list ] ; \
	then $(JAVAC) -d $(CLASSBIN) \
	    $(JAVACFLAGS)  \
	    $(shell if [ -s .classes.list ] ; then cat .classes.list; fi ) ; \
	fi

$(CLASSBIN):
	@-mkdir -p $@

$(CLASSBIN)/%.class : $(JAVASRC)/%.java
	@echo $? >>.classes.list

.classes: $(CLASSBIN) .delete.classlist $(FILES.class) .compile.classlist
	@touch .classes

$(LIB).jar: $(CLASSBIN) .classes
	rm -f $(LIB).jar; \
	cd $(CLASSBIN); \
	$(JAR) cmv0f ../man.txt ../$@ *; \
	cd ..;

.doc:
	@-mkdir -p doc

##
## Rules for building native stubs (JNI style)
## 
## - This will build all *.h (jni) stub files.
##
.stubs: $(FILES.class) 
	echo $(FILES.class)
	$(JAVAH) -jni -d $(STUBS) \
	$(FILES.class.compile)
	@touch .stubs

$(STUBS): 
	@-mkdir -p $@

$(OBJ): 
	@-mkdir -p $@

$(OBJ)$(SUFFIX):
	@-mkdir -p $@

##
## Rules for building native library
##
## - Compile your own native implementation with IEEE floating point support

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

