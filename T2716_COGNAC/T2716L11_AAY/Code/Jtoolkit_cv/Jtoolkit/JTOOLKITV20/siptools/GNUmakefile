#
# Makefile for siptools
#
# (cross-compiler version)
#

include ../Defs.gmk

##
## Compiler flags
##
# flags that are the same for all executables for debug and non-debug builds
CFLAGS_COMMON += -D_GUARDIAN_SOCKETS \
	-D_XOPEN_SOURCE_EXTENDED=1 -Wnowarn=141,209,770,1506 \
	-Wfieldalign=shared2 
SIPC_CCFLAGS       = -Whighpin=off $(CC_FLAGS)   $(CFLAGS_COMMON) -I./$(SIPC_SRC)
SIPC_DEBUG_CCFLAGS = -Whighpin=off $(CC_DEBUG_FLAGS) $(CFLAGS_COMMON) -I./$(SIPC_SRC)

##
## Linker flags
##
SIPC_LIBS = -Whighpin=off -L ./lib -lcgix 


##
## Source directories
##
SIPC_SRC = src_c
OBJ = obj
OBJG = obj_g

##
## Source Files 
##
SIPC_FILES.c = \
	$(SIPC_SRC)/access.c \
	$(SIPC_SRC)/logon.c  \
	$(SIPC_SRC)/pmproc.c \
	$(SIPC_SRC)/pmspi.c  \
	$(SIPC_SRC)/util.c

##
## Object Files
##
# List of object files for optimized build
SIPC_FILES.o = $(SIPC_FILES.c:$(SIPC_SRC)/%.c=$(OBJ)/%.o)
# List of object files for debug build
SIPC_FILESD.o = $(SIPC_FILES.c:$(SIPC_SRC)/%.c=$(OBJG)/%.o)

##
## Rules
##

all : sipconfobj_g sipconfobj 

clean:
	rm -f sipconfobj_g sipconfobj
	rm -Rf $(OBJ) $(OBJG)

clobber: $(MAKE) clean

vproc:
	-rm obj*/access.o
	$(MAKE)


sipconfobj: $(OBJ) $(SIPC_FILES.o)
	$(CC) $(SIPC_FILES.o) $(SIPC_LIBS) $(LDLIBS_COMMON) -o $@

sipconfobj_g: $(OBJG) $(SIPC_FILESD.o)
	$(CC) $(SIPC_FILESD.o) $(SIPC_LIBS) $(LDLIBS_COMMON) -o $@

$(OBJ):
	@-mkdir -p $@

$(OBJG):
	@-mkdir -p $@


#$(SIPC_FILES.o) : $(SIPC_SRC)/%.c
$(OBJ)/%.o : $(SIPC_SRC)/%.c
	$(CC) $(SIPC_CCFLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

#$(SIPC_FILESD.o) : $(SIPC_SRC)/%.c
$(OBJG)/%.o : $(SIPC_SRC)/%.c
	$(CC) $(SIPC_DEBUG_CCFLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

