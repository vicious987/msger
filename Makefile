.PHONY : clean

CC = gcc
CFLAGS = -pthread -Wall

main : p2p.c p2p.h main.c 
	$(CC) $(CFLAGS) -o main main.c p2p.c

#p2p.o : p2p.c p2p.h
#	$(CC) -c p2p.c -o p2p.o

#main.o : main.c

clean :
	rm -f *.o  main
