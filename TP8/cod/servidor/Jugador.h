#ifndef JUGADOR_H
#define JUGADOR_H

#include "Coordenada.h"
#include "Connection.h"

typedef struct {
    char      id;
    unsigned  posicion;
    int       x;
    int       y;
} tProtocolo;

class Jugador {

    public:

        Jugador( int Vida,
                 Coordenada& Posicion,
                 int BombasMax,
                 Connection<char>& Socket);

        ~Jugador();

        bool ponerBomba();
        int restarVida();
        int sumarVida(int cant);

        Coordenada moverArriba();
        Coordenada moverAbajo();
        Coordenada moverIzquierda();
        Coordenada moverDerecha();

        void setPosicion( Coordenada Posicion );

        Coordenada getPosicion();

        void send( tProtocolo data );

        tProtocolo recv();
        
    private:

        int Vida;
        Coordenada Posicion;
        int BombasColocadas;
        int BombasMax;

        Connection<char> Socket;

};

Jugador :: Jugador( int Vida,
                    Coordenada& Posicion,
                    int BombasMax,
                    Connection<char>& Socket){

    this->Socket = Socket;
    
    this->Vida = Vida;
    this->Posicion = Posicion;
    this->BombasMax = BombasMax;

    this->BombasColocadas = 0;

}

Jugador :: ~Jugador(){
}

bool Jugador :: ponerBomba(){

    return true;
}

int Jugador :: restarVida(){

    return this->Vida;
}

int Jugador :: sumarVida(int cant){

    return this->Vida;
}

Coordenada Jugador :: moverArriba(){

    return this->Posicion;
}

Coordenada Jugador :: moverAbajo(){

    return this->Posicion;
}

Coordenada Jugador :: moverIzquierda(){
    
    return this->Posicion;
}

Coordenada Jugador :: moverDerecha(){
    
    return this->Posicion;
}

void Jugador :: setPosicion( Coordenada Posicion ){
    this->Posicion = Posicion;
}

Coordenada Jugador :: getPosicion(){
    return this->Posicion;
}

void Jugador :: send( tProtocolo data ){
    Socket.Send ( (char*) &data, sizeof(tProtocolo) );
}

tProtocolo Jugador :: recv(){
    tProtocolo recibido;
    
    Socket.Recv ( (char*) &recibido, sizeof(tProtocolo) );

    return recibido;
    
}


#endif