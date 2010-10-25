#ifndef BOMBERMAN_H
#define BOMBERMAN_H

#include <iostream>
#include <fstream>
#include <vector>

// Librerias para el generador de numeros aleatorios.
#include <ctime>
#include <cstdlib>

#include "Jugador.h"
#include "Bomba.h"
#include "Premio.h"
#include "Explosion.h"
#include "../include/Coordenada.h"

#include "../include/Comm.h"
#include "../include/Connection.h"

//#include "../include/t_protocolo.h"

// matriz de 9 X 37

class Bomberman {

    public:
        
        Bomberman();
        ~Bomberman();

        void activar( long int puerto );
        
        int nuevoJugador();

        Jugador& getJugador(int jugador);

        unsigned getNumJugadores();

        int update(t_protocolo data);

        int clockTick();

    private:
        int Timer;
        
        vector < Jugador >    Jugadores;
        vector < Bomba >      Bombas;
        vector < Explosion >  Explosiones;
        vector < Premio >     Premios;
        vector < Coordenada > Paredes;
        vector < Coordenada > ParedesDestruibles;

        Comm< char > Socket;

        int VidaInicial;
        int MaxBombInicial;

        static const int X = 9;
        static const int Y = 37;

};


Bomberman :: Bomberman() {
}

Bomberman :: ~Bomberman(){

}

void Bomberman :: activar( long int puerto ){
    
    this->Timer = 0;
    this->VidaInicial = 3;
    this->MaxBombInicial = 1;

    this->Socket = Comm< char >(puerto);
    this->Socket.Listen();

    srand(unsigned(time(NULL)));
    
}

int Bomberman :: nuevoJugador(){
    Connection<char> playerCon;
    unsigned numCon = 0;

    numCon = Socket.Accept();

    playerCon = Socket.getConn( numCon );

    Coordenada coord(0,0);
    
    Jugador Player( this->VidaInicial,
                    coord,
                    this->MaxBombInicial,
                    playerCon );

    Jugadores.push_back( Player );
    
    return Jugadores.size() - 1;
}

Jugador& Bomberman :: getJugador(int jugador){
    return Jugadores[ jugador ];
}

unsigned Bomberman :: getNumJugadores(){
    return Jugadores.size();
}

int Bomberman :: update( t_protocolo data ) {

    for( unsigned i = 0 ; i < Jugadores.size() ; i++ )
        Jugadores[i].send( data );
    
    return 0;
}

int Bomberman :: clockTick(){
    this->Timer++;
    return this->Timer;
}


#endif