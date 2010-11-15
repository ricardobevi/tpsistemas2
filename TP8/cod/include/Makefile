cli: cli.o  MemCompartida.o Semaforo.o serv
	g++ -Wall -lpthread -lncurses -ggdb -o cli cli.o MemCompartida.o Semaforo.o

cliente.o: cli.cpp 
	g++ -Wall -lpthread -lncurses -ggdb -c -o cliente.o cliente.cpp

MemCompartida.o: MemCompartida.cpp MemCompartida.h
	g++ -Wall -ggdb -lpthread -c -o MemCompartida.o MemCompartida.cpp

Semaforo.o:Semaforo.cpp Semaforo.h
	g++ -Wall -ggdb -lpthread -c -o Semaforo.o Semaforo.cpp

serv: serv.o 
	g++ -Wall -lpthread -lncurses -ggdb -o serv serv.o MemCompartida.o Semaforo.o 
	
serv.o: serv.cpp
	g++ -Wall -lpthread -lncurses -ggdb -c -o serv.o serv.cpp

clean:
	rm cli.o MemCompartida.o Semaforo.o serv.o ./cli ./serv
	
