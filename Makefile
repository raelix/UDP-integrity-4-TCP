GCCFLAGS= -ansi -Wall -Wunused -pedantic -ggdb
LINKERFLAGS=-lpthread -lm

all:  Ritardatore.out Proxyreciver.out Proxysender.out Proxysender.o Proxyreciver.o

Util.o: Util.c 
	gcc  ${GCCFLAGS} -c Util.c

Ritardatore.o:	Ritardatore.c Util.h
	gcc  ${GCCFLAGS} -c Ritardatore.c

Ritardatore.out:	Ritardatore.o Util.o
	gcc -o Ritardatore.out  Util.o Ritardatore.o ${GCCFLAGS} ${LINKERFLAGS}

Proxyreciver.out:	includi.h Proxyreciver.o 
	gcc  ${GCCFLAGS} -o Proxyreciver.out Proxyreciver.o

Proxysender.out:	includi.h Proxysender.o 
	gcc  ${GCCFLAGS} -o Proxysender.out Proxysender.o

Proxysender.o: includi.h Proxysender.c
	gcc -c Proxysender.c

Proxyreciver.o: includi.h Proxyreciver.c
	gcc -c Proxyreciver.c
	
clean:	
	rm -f core* *.stackdump
	rm -f *.out
	rm -f Ritardatore.o Ritardatore.out
	rm -f *.o




