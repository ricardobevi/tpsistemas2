#ifndef JUGADOR_H
#define JUGADOR_H

#include "../include/Coordenada.h"
#include "../include/Connection.h"

#include "../include/t_protocolo.h"

class Jugador {

    public:

        Jugador( int Vida,
                 Coordenada Posicion,
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

        void send( t_protocolo data );

        t_protocolo recv();
        
    private:

        int Vida;
        Coordenada Posicion;
        int BombasColocadas;
        int BombasMax;
        
        Connection<char> Socket;

};

Jugador :: Jugador( int Vida,
                    Coordenada Posicion,
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
    Vida--;
    return this->Vida;
}

int Jugador :: sumarVida(int cant){
    Vida++;
    return this->Vida;
}

Coordenada Jugador :: moverArriba(){
    Posicion.set_x( Posicion.get_x() - 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverAbajo(){
    Posicion.set_x( Posicion.get_x() + 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverIzquierda(){
    Posicion.set_y( Posicion.get_y() - 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverDerecha(){
    Posicion.set_y( Posicion.get_y() + 1 );
    return this->Posicion;
}

void Jugador :: setPosicion( Coordenada Posicion ){
    this->Posicion = Posicion;
}

Coordenada Jugador :: getPosicion(){
    return this->Posicion;
}

void Jugador :: send( t_protocolo data ){
    Socket.Send ( (char*) &data, sizeof(t_protocolo) );
}

t_protocolo Jugador :: recv(){
    t_protocolo recibido;
    
    Socket.Recv ( (char*) &recibido, sizeof(t_protocolo) );

    return recibido;
    
}


#endif