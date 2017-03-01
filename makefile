CC = gcc -std=c99 
LD = gcc -std=c99 
CFLAGS = -O3 -Wall -Werror 
LDFLAGS = -lm 
RM = /bin/rm -f
OBJS = main.o file_operations.o quad.o
EXECUTABLE = galsim

all:$(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LD) $(OBJS) -o $(EXECUTABLE) $(LDFLAGS)

file_operations.o: file_operations.h file_operations.c
	$(CC) $(CFLAGS) -c file_operations.c

main.o: main.c file_operations.h quad.h
	$(CC) $(CFLAGS) -fopenmp -c main.c

quad.o: quad.h quad.c
	$(CC) $(CFLAGS) -c quad.c 

clean:
	$(RM) $(EXECUTABLE) $(OBJS)
