compilar: compCli compServ
	
compCli: cliente.cpp Comm.h Connection.h
	g++  -lpthread -o cli cliente.cpp 


compServ: servidor.cpp Comm.h Connection.h
	g++  -lpthread -o serv servidor.cpp
	

clean:
	rm -R serv cli