#ifndef BOMBERMAN_H
#define BOMBERMAN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

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

#define ACCION_MOVE 1
#define ACCION_BOMB 2

//#include "../include/t_protocolo.h"

// matriz de 9 X 37

class Bomberman {

    public:
        
        Bomberman();
        ~Bomberman();

        void activar( long int puerto, string archivoEscenario );
        
        int nuevoJugador();

        void sendEscenario( unsigned jugador );

        Jugador& getJugador( int jugador );

        unsigned getNumJugadores();

        t_protocolo procesarAccion( t_protocolo data );

        t_protocolo explotarBomba();

        int update( t_protocolo data );

        t_protocolo eliminarJugador( unsigned jugador );

        t_protocolo clockTick();

        static const unsigned JUGADORES_MAX = 4;
        static const unsigned X_MAX = 37;
        static const unsigned Y_MAX = 9;

    private:
        int Timer;
        unsigned long int HDTimer;
        
        vector < Jugador >    Jugadores;
        queue  < Bomba >      Bombas;
        vector < Explosion >  Explosiones;
        vector < Premio >     Premios;
        vector < Coordenada > Paredes;
        vector < Coordenada > ParedesDestruibles;

        Comm< char > Socket;

        int VidaInicial;
        int MaxBombInicial;
        unsigned long int TiempoBomba;

        unsigned NumJugadores;

        static const char LUGAR_VACIO = 200;

        char Escenario[ X_MAX + 1 ][ Y_MAX + 1 ];

        /*Teclas de accion*/
        int MoverArriba,
            MoverAbajo,
            MoverIzquierda,
            MoverDerecha,
            PonerBomba;

};


Bomberman :: Bomberman() {
}

Bomberman :: ~Bomberman(){

}

void Bomberman :: activar( long int puerto, string archivoEscenario ){
    ifstream archEsc( archivoEscenario.c_str() );
    
    this->Timer = 0;
    this->VidaInicial = 3;
    this->MaxBombInicial = 1;
    this->TiempoBomba = 50;
    this->NumJugadores = 0;

    MoverArriba = 'w';
    MoverAbajo = 's';
    MoverIzquierda = 'a';
    MoverDerecha = 'd';
    PonerBomba = 'b';

    this->Socket = Comm< char >(puerto);
    this->Socket.Listen();

    srand(unsigned(time(NULL)));

    for( unsigned i = 0 ; i < X_MAX + 1 ; i++ )
        for( unsigned j = 0 ; j < Y_MAX + 1 ; j++ )
            Escenario[i][j] = LUGAR_VACIO;

    /*Leo los datos del escenario*/{
        char tipoPared;
        unsigned x, y;
        ifstream archEsc( archivoEscenario.c_str() );

        archEsc >> tipoPared;
        
        while( ( ! archEsc.eof() ) && ( tipoPared != 'e' ) ){
            archEsc >> x;
            archEsc >> y;

            /*Descomentar para ver como carga el archivo
            cout << "tipoPared = " << tipoPared << endl
                 << "x = " << x << endl
                 << "y = " << y << endl;
            */
            if( tipoPared == 'd' ){
                ParedesDestruibles.push_back( Coordenada( x, y ) );
                Escenario[x][y] = 'd';
            } else {
                Paredes.push_back( Coordenada( x, y ) );
                Escenario[x][y] = 'f';
            }
            
            archEsc >> tipoPared;
            
        }

        archEsc.close();
    }
    
}

int Bomberman :: nuevoJugador(){
    Connection<char> playerCon;
    unsigned numCon = 0;
    unsigned numJugador = 0;

    numCon = this->Socket.Accept();

    playerCon = this->Socket.getConn( numCon );

    /*Saco numero de jugador*/{
        unsigned i = 0;
        
        while( i < Jugadores.size() &&
               i < JUGADORES_MAX &&
               ! Jugadores[i].eliminado() ) i = i + 1;

        if ( i >= JUGADORES_MAX )
            return -1;
        else
            numJugador = i;
    }

    Coordenada coord(0,0);

    switch(numJugador){
        case 0:
            coord.set_coordenada(0,0);
            break;
            
        case 1:
            coord.set_coordenada(X_MAX, 0);
            break;
            
        case 2:
            coord.set_coordenada(X_MAX, Y_MAX);
            break;
            
        case 3:
            coord.set_coordenada(0, Y_MAX);
            break;
    }

    Jugador Player( numJugador,
                    this->VidaInicial,
                    coord,
                    playerCon, 1, 20 );

    if ( numJugador == Jugadores.size() ){
        
        this->Jugadores.push_back( Player );
        
    } else {
        
        this->Jugadores[numJugador] = Player;
        
    }

    Escenario[0][0] = numJugador;

    this->sendEscenario( numJugador );

    this->NumJugadores++;
    
    return (int) numJugador;
}

void Bomberman :: sendEscenario( unsigned jugador ){
    unsigned i = 0;
    t_protocolo enviar;

    if( this->Jugadores[jugador].getEnvioEscenario() == false ){
    
        for( i = 0 ; i < Jugadores.size() ; i++ ){
            enviar.id = 'j';
            enviar.posicion = i;
            enviar.x = Jugadores[i].getPosicion().get_x();
            enviar.y = Jugadores[i].getPosicion().get_y();

            Jugadores[jugador].send(enviar);

        }

        for( i = 0 ; i < Paredes.size() ; i++ ){
            enviar.id = 'f';
            enviar.posicion = i;
            enviar.x = Paredes[i].get_x();
            enviar.y = Paredes[i].get_y();

            Jugadores[jugador].send(enviar);

        }

        for( i = 0 ; i < ParedesDestruibles.size() ; i++ ){
            enviar.id = 'd';
            enviar.posicion = i;
            enviar.x = ParedesDestruibles[i].get_x();
            enviar.y = ParedesDestruibles[i].get_y();

            Jugadores[jugador].send(enviar);

        }

        enviar.id = 'i';
        enviar.posicion = 0;
        enviar.x = jugador;
        enviar.y = 0;

        Jugadores[jugador].send(enviar);

        this->Jugadores[jugador].setEnvioEscenario(true);
    }
}

Jugador& Bomberman :: getJugador(int jugador){
    return this->Jugadores[ jugador ];
}

unsigned Bomberman :: getNumJugadores(){
    return this->NumJugadores;
}

t_protocolo Bomberman :: procesarAccion( t_protocolo recibido ){
    t_protocolo enviar;

    char accion = 0;
    
    unsigned jugador = recibido.posicion;

    int  x = this->Jugadores[ jugador ].getPosicion().get_x(),
         y = this->Jugadores[ jugador ].getPosicion().get_y();

 
        if ( recibido.x == MoverArriba ){
            
            if ( y > 0 &&
                ( Escenario[x][y - 1] == LUGAR_VACIO ||
                  Escenario[x][y - 1] <= 3 ) ){

                this->Jugadores[ jugador ].moverArriba();
                accion = ACCION_MOVE;
            
            }

        }else if (recibido.x == MoverAbajo){

            if ( (unsigned) y < Y_MAX &&
                ( Escenario[x][y + 1] == LUGAR_VACIO ||
                  Escenario[x][y + 1] <= 3 )  ){

                this->Jugadores[ jugador ].moverAbajo();
                accion = ACCION_MOVE;
            
            }
            
        }else if (recibido.x == MoverIzquierda){
        
            if (  x > 0 &&
                ( Escenario[x - 1][y] == LUGAR_VACIO ||
                  Escenario[x - 1][y] <= 3 ) ){

                this->Jugadores[ jugador ].moverIzquierda();
                accion = ACCION_MOVE;
            
            }
            
        }else if (recibido.x == MoverDerecha){

            if ( (unsigned) x < X_MAX &&
                ( Escenario[x + 1][y] == LUGAR_VACIO ||
                  Escenario[x + 1][y] <= 3 )
               ){
                
                this->Jugadores[ jugador ].moverDerecha();
                accion = ACCION_MOVE;
            
            }
            
        }else if (recibido.x == PonerBomba){
            accion = ACCION_BOMB;
        }


    if( accion == ACCION_MOVE ){
        
        enviar.id = 'j';
        enviar.posicion = jugador;
        enviar.x = this->Jugadores[ jugador ].getPosicion().get_x();
        enviar.y = this->Jugadores[ jugador ].getPosicion().get_y();
        
    }else if( accion == ACCION_BOMB ){
        
        if( Jugadores[ jugador ].puedePonerBomba() ){
            unsigned tam = Bombas.size();
            Bomba bomb;

            bomb = Jugadores[ jugador ].ponerBomba( tam );

            bomb.activar( HDTimer + TiempoBomba );
            
            Bombas.push( bomb );

            enviar.id = 'b';
            enviar.posicion = tam;
            bomb.getPos().get_coordenada(enviar.x, enviar.y);
            
        }else{
            enviar.id = 0;
        }


    }else {
        
        enviar.id = 0;
        enviar.posicion = jugador;
        enviar.x = 0;
        enviar.y = 0;
        
    }

    return enviar;

}

t_protocolo Bomberman :: explotarBomba(){
    t_protocolo enviar;
    
    if( Bombas.size() > 0 ) {
        
        Bomba bomb;
        
        bomb = Bombas.front();     
        
        usleep( ( bomb.getTiempoExplosion() - HDTimer ) * 100000 );

        enviar.id = 'b';
        enviar.posicion = bomb.getNumero();
        enviar.x = enviar.y = -2;
        
        Bombas.pop();
    } else {
        enviar.id = 0;
    }

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
        
        this->NumJugadores--;

        enviar.id = 'j';
        enviar.posicion = jugador;
        enviar.x = -1;
        enviar.y = -1;
        
    } else {

        enviar.id = 0;
        enviar.posicion = jugador;
        enviar.x = -1;
        enviar.y = -1;
    
    }
    
    return enviar;
}

t_protocolo Bomberman :: clockTick(){
    t_protocolo enviar;

    for( int i = 0 ; i < 10 ; i++ ){
        usleep( 100000 );
        this->HDTimer++;
    }
    
    this->Timer++;

    enviar.id = 't';
    enviar.posicion = 0;
    enviar.x = this->Timer;
    enviar.y = 0;

    return enviar;
}


#endif