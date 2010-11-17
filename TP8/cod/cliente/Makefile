remoto: cliente.o ../include/Coordenada.o ../include/MemCompartida.o ../include/Semaforo.o
	g++ -Wall -lpthread -lncurses -ggdb -o cli cliente.o ../include/Coordenada.o ../include/MemCompartida.o ../include/Semaforo.o

local: clienteLocal.o ../include/Coordenada.o ../include/MemCompartida.o ../include/Semaforo.o
	g++ -Wall -lpthread -lncurses -ggdb -o cliLocal clienteLocal.o ../include/Coordenada.o ../include/MemCompartida.o ../include/Semaforo.o

cliente.o: cliente.cpp c_Bomberman.h c_escenario.h entorno.h ../include/Connection.h
	g++ -Wall -lpthread -lncurses -ggdb -c -o cliente.o cliente.cpp
	
clienteLocal.o: clienteLocal.cpp c_BombermanLocal.h c_escenario.h entorno.h ../include/Connection.h
	g++ -Wall -lpthread -lncurses -ggdb -c -o clienteLocal.o clienteLocal.cpp

../include/Coordenada.o: ../include/Coordenada.cpp ../include/Coordenada.h
	g++ -Wall -ggdb -lpthread -c -o ../include/Coordenada.o ../include/Coordenada.cpp

../include/MemCompartida.o: ../include/MemCompartida.cpp ../include/MemCompartida.h
	g++ -Wall -ggdb -lpthread -c -o ../include/MemCompartida.o ../include/MemCompartida.cpp

../include/Semaforo.o: ../include/Semaforo.cpp ../include/Semaforo.h
	g++ -Wall -ggdb -lpthread -c -o ../include/Semaforo.o ../include/Semaforo.cpp

clean:
	rm ../include/Coordenada.o cliente.o ../include/MemCompartida.o ../include/Semaforo.o ./cli 
	
all: local remoto
