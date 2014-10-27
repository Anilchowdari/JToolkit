CFLAGS = -Wextensions -D_XOPEN_SOURCE_EXTENDED=1
CXX=c89

INCLUDES = .

SERVER_OBJS = Server.o

SERVER_EXE = server

$(SERVER_EXE):	$(SERVER_OBJS)
	$(CXX) -o $(SERVER_EXE) $(SERVER_OBJS)
	
.c.o:
	$(CXX) $(CFLAGS) -I$(INCLUDES) -c $< -o $@ 

all: $(SERVER_EXE)

clean:
	rm -f $(SERVER_OBJS) $(SERVER_EXE)
