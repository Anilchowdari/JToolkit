##
## The location of java that will be used to build
##
JAVA_HOME = /usr/tandem/java

##
## Java tool locations
##
JAR = $(JAVA_HOME)/bin/jar
JAVAC = $(JAVA_HOME)/bin/javac
JAVAH = $(JAVA_HOME)/bin/javah

##
## The location of c89 that will be used to build
##
COMP_ROOT = /
LINK_ROOT = /

##
## C and Linker tools
##
CC = $(COMP_ROOT)/usr/bin/c89

LD = $(LINK_ROOT)/usr/bin/xld -set systype oss
LDLIBS_COMMON = -L $(COMP_ROOT)/usr/lib -lcrtl -lcpp3 -lcre -lcppc -lput -lrld
LD_SHARED = -shared $(LDLIBS_COMMON) -export_all
LD_FLAGS_IEEE = -set FLOATTYPE IEEE_FLOAT

all:
	$(JAVAC) Umask.java 
	$(JAR) -cvf umask.jar Umask.class
	$(JAVAH) -jni Umask
	$(CC) -c -g -I $(JAVA_HOME)/include -I $(JAVA_HOME)/include/oss/ Umask.c
	$(LD) $(LD_SHARED) $(LD_FLAGS_IEEE) -o libumask.so Umask.o

clean:
	rm -f Umask.class
	rm -f umask.jar
	rm -f Umask.h
	rm -f libumask.so
