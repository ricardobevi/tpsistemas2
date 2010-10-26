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

#define JUGMAX 4

// matriz de 9 X 37

class Bomberman {

    public:
        
        Bomberman();
        ~Bomberman();

        void activar( long int puerto );
        
        int nuevoJugador();

        void sendEscenario( unsigned jugador );

        Jugador& getJugador( int jugador );

        unsigned getNumJugadores();

        t_protocolo procesarAccion( t_protocolo data );

        int update( t_protocolo data );

        t_protocolo eliminarJugador( unsigned jugador );

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
    unsigned numJugador = 0;

    numCon = this->Socket.Accept();

    playerCon = this->Socket.getConn( numCon );

    Coordenada coord(0,0);

    /*Saco numero de jugador*/{
        unsigned i = 0;
        
        while( i < Jugadores.size() &&
               i < JUGMAX &&
               ! Jugadores[i].eliminado() ) i = i + 1;

        if ( i >= JUGMAX )
            return -1;
        else
            numJugador = i;
    }   

    Jugador Player( numJugador,
                    this->VidaInicial,
                    coord,
                    this->MaxBombInicial,
                    playerCon );

    if ( numJugador == Jugadores.size() ){
        
        this->Jugadores.push_back( Player );
        
    } else {
        
        this->Jugadores[numJugador] = Player;
        
    }

    this->sendEscenario( numJugador );
    
    return (int) numJugador;
}

void Bomberman :: sendEscenario( unsigned jugador ){
    unsigned i = 0;
    t_protocolo enviar;
    
    for( i = 0 ; i < Jugadores.size() ; i++ ){
        enviar.id = 'j';
        enviar.posicion = i;
        enviar.x = Jugadores[i].getPosicion().get_x();
        enviar.y = Jugadores[i].getPosicion().get_y();
        
        Jugadores[jugador].send(enviar);
        
    }

    enviar.id = 'i';
    enviar.posicion = 0;
    enviar.x = jugador;
    enviar.y = 0;

    Jugadores[jugador].send(enviar);

}

Jugador& Bomberman :: getJugador(int jugador){
    return this->Jugadores[ jugador ];
}

unsigned Bomberman :: getNumJugadores(){
    return this->Jugadores.size();
}

t_protocolo Bomberman :: procesarAccion( t_protocolo recibido ){
    t_protocolo enviar;
    
    unsigned jugador = recibido.posicion;

    switch( recibido.x ){
        case 'w':
            this->Jugadores[ jugador ].moverArriba();
            break;

        case 's':
            this->Jugadores[ jugador ].moverAbajo();
            break;

        case 'd':
            this->Jugadores[ jugador ].moverDerecha();
            break;

        case 'a':
            this->Jugadores[ jugador ].moverIzquierda();
            break;

    }

    enviar.id = 'j';
    enviar.posicion = jugador;
    enviar.x = this->Jugadores[ jugador ].getPosicion().get_x();
    enviar.y = this->Jugadores[ jugador ].getPosicion().get_y();

    return enviar;

}

int Bomberman :: update( t_protocolo data ) {

    for( unsigned i = 0 ; i < Jugadores.size() ; i++ )
        if( ! this->Jugadores[i].eliminado() )
            this->Jugadores[i].send( data );
    
    return 0;
}

t_protocolo Bomberman :: eliminarJugador( unsigned jugador ){
    t_protocolo enviar;
    
    if( this->Jugadores.size() - 1 >= jugador ){
        this->Jugadores[ jugador ].eliminar();
    }

    enviar.id = 'j';
    enviar.posicion = jugador;
    enviar.x = -1;
    enviar.y = 0;

    return enviar;
}

int Bomberman :: clockTick(){
    this->Timer++;
    return this->Timer;
}


#endif