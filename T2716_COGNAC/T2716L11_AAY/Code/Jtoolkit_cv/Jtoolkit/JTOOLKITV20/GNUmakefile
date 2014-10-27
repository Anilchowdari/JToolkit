##-------------------------------------------------------------------------------
## To use this gmake makefile 
## - edit the Defs.gmk file
## - edit the Specification-Version and Implementation-Version in the man.txt file 
## then execute one or more of the following:
##
## gmake [all] - build all subdirectories and SIP tools, creates one .lib and one .jar
##               for all subdirs and generates javadoc 
##
## gmake external_install - creates release tree structure and copies the released
##                          files into the release tree
##
## gmake tar - creates tar file of the product, this file can be installed
##             on a system for testing purposes before we're ready to submit to
##             prodrlse
##   
## gmake prodrlse - makes tar files of the html doc, obj files, src and everything
##                  else we submit to prodrlse
## 
##-------------------------------------------------------------------------------

# Edit this include file to give it correct vproc and compiler etc.
include Defs.gmk

RELEASE_DIR = release/javaextv20

DEMODIRS = $(RELEASE_DIR)/demo/advanced_demo $(RELEASE_DIR)/demo/enscribe $(RELEASE_DIR)/demo/pathsend $(RELEASE_DIR)/demo/sip



#List of subdirectories that build executables
PROGRAM_SUBDIRS = ddl2java siptools 


# List of subdirectories that build java APIs
# Build guardian before enscribe because enscribe depends on guardian
JAVAAPI_SUBDIRS = guardian enscribe tsmp

# List of JNI libraries for both debug and optimized builds
# These libs all get linked into one library for customer installation
JNI_LIBS = guardian/guardian.lib enscribe/enscribe.lib tsmp/tsmp.lib
JNI_LIBS_G = guardian/guardian_g.lib enscribe/enscribe_g.lib tsmp/tsmp_g.lib

# List of java packages, used to generated javadoc generated HTML documentation
PACKAGES = com.tandem.ext.enscribe com.tandem.ext.guardian com.tandem.ext.util com.tandem.tsmp

# This needs to change if subdirectories are added to JAVAAPI_SUBDIRS
# It is used in the JAVADOC command to find class references
CLASSPATH = .:./guardian/classes:./enscribe/classes:./tsmp/classes:$(JAVA_HOME)/jre/lib/rt.jar

## ------------------------------------------------
## Shouldn't need to edit anything below this line
## ------------------------------------------------

all :: exe_subdirs lib_subdirs tdmext.jar libtdmext.lib libtdmext_g.lib javadoc

exe_subdirs:
	# Create the executables
	@for i in $(PROGRAM_SUBDIRS) ; do \
	   echo ">>> Making "$$i" all..."; \
	   cd $$i; $(MAKE) all || exit 1; cd ..; \
	   echo "<<< Finished making "$$i" all." ; \
	done

lib_subdirs:
	# Compile the Java classes and JNI code
	@for i in $(JAVAAPI_SUBDIRS) ; do \
	   echo ">>> Making "$$i" all..."; \
	   cd $$i; $(MAKE) all || exit 1; cd ..; \
	   echo "<<< Finished making "$$i" all." ; \
	done

clean:
	@for i in $(PROGRAM_SUBDIRS) ; do \
	   echo ">>> Making "$$i" "$@"..."; \
	   cd $$i; $(MAKE) $@ || exit 1; cd ..; \
	   echo "<<< Finished making "$$i" "$@"." ; \
	done
	@for i in $(JAVAAPI_SUBDIRS) ; do \
	   echo ">>> Making "$$i" "$@"..."; \
	   cd $$i; $(MAKE) $@ || exit 1; cd ..; \
	   echo "<<< Finished making "$$i" "$@"." ; \
	done
	-rm tdmext.jar
	-rm libtdmext.lib
	-rm libtdmext_g.lib
	-rm -Rf htmldoc
	-rm -Rf release
	-rm vproc.o
	-rm -rf prodrlse
	-rm T2716.tar

vproc.o : 
	-rm vproc.o
	$(CC) $(CC_FLAGS) $(CC_FLAGS_IEEE) $(INCLUDE_DIRS) -c -o $@ vproc.c

vproc_only: vproc.o
	# Do this step if only want to update the VPROC info
	cd siptools; $(MAKE) vproc || exit 1; cd .. 
	cd ddl2java; $(MAKE) vproc || exit 1; cd ..

tdmext.jar: lib_subdirs
	# Takes all the previously compiled classes and bundles them in one jar file
	rm -f ./$@
	$(JAR) cmv0f ./man.txt $@
	@for i in $(JAVAAPI_SUBDIRS) ; do \
	   echo ">>> Jar-ing "$$i" classes..."; \
	   cd $$i/classes; $(JAR) uv0f ../../$@ * || exit 1; cd ../..; \
	   echo "<<< Finished Jar-ing "$$i " classes." ; \
	done

libtdmext.lib: lib_subdirs vproc.o
	# Takes all previously compiled libraries and bundle them in one lib file
	$(LD) $(LD_FLAGS) $(LD_FLAGS_IEEE) $(JNI_LIBS) vproc.o -o $@

libtdmext_g.lib: lib_subdirs vproc.o
	# Takes all previously compiled libraries and bundle them in one lib file
	$(LD) $(LD_DEBUG_FLAGS) $(JNI_LIBS_G) vproc.o -o $@

javadoc: tdmext.jar
	# In order to javadoc multiple packages at once the source needs to be in
	# one tree so we'll copy it there and then delete it afterwards
	rm -Rf src_java
	mkdir src_java
	cp -p -R ./guardian/src_java/* src_java/.
	cp -p -R ./enscribe/src_java/* src_java/.
	cp -p -R ./tsmp/src_java/* src_java/.
	rm -Rf htmldoc
	mkdir htmldoc
	$(JAVADOC) -d htmldoc -use -serialwarn \
	-overview ./overview.html \
	-linkoffline http://download.oracle.com/javase/1.4.2/docs/api . \
	-classpath ./tdmext.jar:$(CLASSPATH) \
	-sourcepath ./src_java \
	-protected -windowtitle "HP JToolkit for NonStop(TM) Servers $(API_VERSION) API Specification" \
	-doctitle "HP JToolkit for NonStop<sup><font size="-2">TM</font></sup> Servers $(API_VERSION) API Specification" \
	-header '<b>HP JToolkit for NonStop Servers</b><br><font size="-1">$(VERSION)</font>' \
	$(PACKAGES)
	rm -Rf src_java

external_install:
	# Create release tree that eventually gets pax'd up and installed 
	rm -rf release
	#mkdir -p $(RELEASE_DIR)/bin/oss/posix_threads $(RELEASE_DIR)/jre/lib
	mkdir -p $(RELEASE_DIR)/bin $(RELEASE_DIR)/lib
	mkdir -p $(RELEASE_DIR)/demo
	cp -p libtdmext.lib $(RELEASE_DIR)/lib
	cp -p tdmext.jar $(RELEASE_DIR)/lib
	chmod a+r-xw $(RELEASE_DIR)/lib/*.*
        # tar all the demos over and move the tar file to the 
        # release directory to unpax
	find demo -name CVS > excludeCVS
	tar hcfX - excludeCVS demo | (cd $(RELEASE_DIR); tar xf - )
	rm ./excludeCVS
	chmod -R 755 $(RELEASE_DIR)/demo
	@for i in $(DEMODIRS) ; do  \
                copyrightREADME.sh $$i "README"; \
	done	
	# copy the COPYRIGHT, LICENSE, README, HPLogo.gif, index.html
	cp -p doc/COPYRIGHT $(RELEASE_DIR)
	cp -p doc/LICENSE $(RELEASE_DIR)
	cp -p doc/HPLogo.gif $(RELEASE_DIR)
	cp -p doc/index.html $(RELEASE_DIR)
	# and any other files over to $(RELEASE_DIR)
        # giving write permission to the user
	# chmod a+xr-w $(RELEASE_DIR)/install
	cp -p install.sh $(RELEASE_DIR)/install
	chmod 755 $(RELEASE_DIR)/install
	# do appropriate chmods (chmod a+r-wx)
	chmod a+r-wx $(RELEASE_DIR)/COPYRIGHT $(RELEASE_DIR)/LICENSE 
	chmod a+r-wx $(RELEASE_DIR)/HPLogo.gif $(RELEASE_DIR)/index.html
	# copy ddl2java and sip to $(RELEASE_DIR)/bin
	cp -p ./siptools/sipconf $(RELEASE_DIR)/bin
	cp -p ./siptools/sipconfobj $(RELEASE_DIR)/bin
	cp -p ./ddl2java/ddl2java $(RELEASE_DIR)/bin
	chmod a+rx-w $(RELEASE_DIR)/bin/*

tar: T2716.tar

T2716.tar: $(RELEASE_DIR)/bin
	# Create and compress pax file
	rm -f $@
	cd release;\
	pax -s:./:/usr/tandem/: -wvf ../$@ ./*
	cd ..

prodrlse:
	-rm -rf prodrlse
	# Create what's necessary for prodrlse
	-mkdir -p prodrlse
	# cp BUILD, RELEASE, SOFTDOC, T2716DOC
	cp -p ./doc/BUILD ./prodrlse
	cp -p ./doc/RELEASE ./prodrlse
	cp -p ./doc/SOFTDOC ./prodrlse
	cp -p ./doc/T2716DOC ./prodrlse
	cp -p ./T2716.tar ./prodrlse/T2716PAX
	# tar up the src using: tar -cvfXh ...
	tar -hcvf ../T2716SRC ./*
	# compress the T2716SRC
	compress ../T2716SRC
	# rename T2716SRC.Z as T2716SRC
	mv ../T2716SRC.Z ./prodrlse/T2716SRC 

	# create pax of debug-able code for support

