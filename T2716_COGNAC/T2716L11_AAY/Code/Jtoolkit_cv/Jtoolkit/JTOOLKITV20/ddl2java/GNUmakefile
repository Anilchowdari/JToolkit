#
# Makefile for ddl2java
#
# (cross-compiler version)
# 

include ../Defs.gmk

# flags that are the same for debug and non-debug builds
CFLAGS_COMMON += -Wnowarn=141,209 \
	-Wfieldalign=shared2 -D_XOPEN_SOURCE_EXTENDED=1 
CPP_FLAGS = -Wversion2 $(CC_FLAGS)
CPP_DEBUG_FLAGS = -Wversion2 $(CC_DEBUG_FLAGS)
# Strip symbols for non-debug builds
#LD_FLAGS = $(LD_FLAGS_COMMON) -Wnld="-s"
#LD_DEBUG_FLAGS = $(LD_FLAGS_COMMON) -Winspect
INCLUDE_DIRS = -I./$(SRC)

# The executable we're building
PROGRAM = ddl2java

##
## Source and object directories
## 
SRC=src
OBJ=obj

# Used for debug builds
SUFFIX= _g

##
## Source Files 
##

# Add C++ files to this list
FILES.cpp = \
	$(SRC)/classgen.cpp   \
	$(SRC)/ddl2java.cpp   \
	$(SRC)/ddlproc.cpp    \
	$(SRC)/ddlread.cpp    \
	$(SRC)/detailList.cpp \
	$(SRC)/itemlist.cpp   \
	$(SRC)/keysfile.cpp   \
	$(SRC)/utils.cpp 


# Add C files to this list
FILES.c = 
	

# List of object files for optimized build
FILES.o = $(FILES.c:$(SRC)/%.c=$(OBJ)/%.o)
FILES.o += $(FILES.cpp:$(SRC)/%.cpp=$(OBJ)/%.o)
# List of object files for debug build
FILESD.o = $(FILES.c:$(SRC)/%.c=$(OBJ)$(SUFFIX)/%.o)
FILESD.o += $(FILES.cpp:$(SRC)/%.cpp=$(OBJ)$(SUFFIX)/%.o)

##
## Rules
##

all: $(PROGRAM) $(PROGRAM)$(SUFFIX)

clean :
	rm -rf $(PROGRAM) $(PROGRAM)$(SUFFIX) $(OBJ) $(OBJ)$(SUFFIX)

## ------------------------------------------------------
## --- Should be no need to modify from this point on ---
## ------------------------------------------------------

##
## Rules for building executable
##

$(OBJ):
	@-mkdir -p $@

$(OBJ)$(SUFFIX):
	@-mkdir -p $@

$(OBJ)/%.o :: $(SRC)/%.c 
	$(CC) $(CC_FLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(OBJ)/%.o :: $(SRC)/%.cpp 
	$(CC) $(CPP_FLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(OBJ)$(SUFFIX)/%.o :: $(SRC)/%.c 
	$(CC) $(CC_DEBUG_FLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(OBJ)$(SUFFIX)/%.o :: $(SRC)/%.cpp 
	$(CC) $(CPP_DEBUG_FLAGS) $(INCLUDE_DIRS) \
	-c -o $@ $< 

$(PROGRAM): $(OBJ) $(FILES.o)
	$(CC) $(FILES.o) $(LDLIBS_COMMON) -o $@

$(PROGRAM)$(SUFFIX): $(OBJ)$(SUFFIX) $(FILESD.o)
	$(CC) $(FILESD.o) $(LDLIBS_COMMON) -o $@

