serv: ../include/Comm.h ../include/Connection.h ../include/Coordenada.o bin/Jugador.o \
      bin/Premio.o bin/Explosion.o bin/Bomba.o bin/Bomberman.o bin/servidor.o \
      bin/JugadorRemoto.o bin/JugadorAutomatico.o
	g++ -Wall -ggdb -lpthread -o serv bin/servidor.o bin/Jugador.o bin/Premio.o bin/Explosion.o \
	 bin/Bomba.o bin/Bomberman.o ../include/Coordenada.o bin/JugadorRemoto.o bin/JugadorAutomatico.o

bin/servidor.o: servidor.cpp
	g++ -Wall -ggdb -lpthread -c -o bin/servidor.o servidor.cpp

bin/Jugador.o: Jugador.cpp Jugador.h
	g++ -Wall -ggdb -lpthread -c -o bin/Jugador.o Jugador.cpp
	
bin/JugadorRemoto.o: JugadorRemoto.cpp JugadorRemoto.h
	g++ -Wall -ggdb -lpthread -c -o bin/JugadorRemoto.o JugadorRemoto.cpp
	
bin/JugadorAutomatico.o: JugadorAutomatico.cpp JugadorAutomatico.h
	g++ -Wall -ggdb -lpthread -c -o bin/JugadorAutomatico.o JugadorAutomatico.cpp

bin/Premio.o: Premio.cpp Premio.h
	g++ -Wall -ggdb -lpthread -c -o bin/Premio.o Premio.cpp
	
bin/Explosion.o: Explosion.cpp Explosion.h
	g++ -Wall -ggdb -lpthread -c -o bin/Explosion.o Explosion.cpp
	
bin/Bomba.o: Bomba.cpp Bomba.h
	g++ -Wall -ggdb -lpthread -c -o bin/Bomba.o Bomba.cpp
	
bin/Bomberman.o: Bomberman.cpp Bomberman.h
	g++ -Wall -ggdb -lpthread -c -o bin/Bomberman.o Bomberman.cpp

../include/Coordenada.o: ../include/Coordenada.cpp ../include/Coordenada.h
	g++ -Wall -ggdb -lpthread -c -o ../include/Coordenada.o ../include/Coordenada.cpp
	
clean:
	rm bin/servidor.o bin/Jugador.o bin/Premio.o bin/Explosion.o \
	bin/Bomba.o bin/Bomberman.o ../include/Coordenada.o \
	bin/JugadorRemoto.o bin/JugadorAutomatico.o
	
all: clean serv
