SRC=do_work.c service.c
OBJ=$(SRC:.c=.o) # replaces the .c from SRC with .o
EXE=robinhood.exe

CC=gcc
CFLAGS=-Wall -O2 -mno-cygwin
LDFLAGS=-lwsock32
RM=rm

# combined w/ next line will compile recently changed .c files
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

# all is dependent on $(EXE) to be complete
all: $(EXE)

# $(EXE) is dependent on all of the files in $(OBJ) to exist
$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(EXE)

# '-' causes errors not to exit the process
clean:
	-$(RM) $(OBJ) $(EXE)
