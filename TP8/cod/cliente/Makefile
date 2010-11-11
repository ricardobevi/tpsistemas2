cli: cliente.o ../include/Coordenada.o
	g++ -Wall -lpthread -lncurses -ggdb -o cli cliente.o ../include/Coordenada.o

cliente.o: cliente.cpp c_Bomberman.h c_escenario.h entorno.h 
	g++ -Wall -lpthread -lncurses -ggdb -c -o cliente.o cliente.cpp

../include/Coordenada.o: ../include/Coordenada.cpp ../include/Coordenada.h
	g++ -Wall -ggdb -lpthread -c -o ../include/Coordenada.o ../include/Coordenada.cpp

clean:
	rm cliente.o ../include/Coordenada.o

all: clean cli
