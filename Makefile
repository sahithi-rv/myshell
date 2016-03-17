CC = gcc
CFLAGS = -Wall 
LIBS = -lm

BIN = shell
OBJS = shell.o operate.o extern_com.o

$(BIN):: $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) 

shell.o:: operate.h

operate.o:: operate.h

extern_com.o::operate.h



clean::
	/bin/rm -f *~ *.o

