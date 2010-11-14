todo: cli serv

cli: cli.o Semaforo.o MemCompartida.o
	g++ -Wall -o cli cli.o Semaforo.o MemCompartida.o

serv: serv.o Semaforo.o MemCompartida.o
	g++ -Wall -o serv serv.o Semaforo.o MemCompartida.o

cli.o: cli.cpp
	g++ -Wall -c -o cli.o cli.cpp
	
serv.o: serv.cpp
	g++ -Wall -c -o serv.o serv.cpp

Semaforo.o: Semaforo.h Semaforo.cpp
	g++ -Wall -c -o Semaforo.o Semaforo.cpp

MemCompartida.o: MemCompartida.h MemCompartida.cpp
	g++ -Wall -c -o MemCompartida.o MemCompartida.cpp
