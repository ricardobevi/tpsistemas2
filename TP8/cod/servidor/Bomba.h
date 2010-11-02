#ifndef BOMBA_H
#define BOMBA_H

#include "../include/Coordenada.h"

class Bomba {
    public:
        Bomba(Coordenada& Pos = *(new Coordenada()), int Tipo = 1, int Owner = -1, unsigned Numero = 0);
        ~Bomba();

        void activar( unsigned long int Tiempo );

        unsigned long int getTiempoExplosion();

        Coordenada getPos();
        unsigned getNumero();

        Bomba operator = ( const Bomba& obj );

    private:
        int Tipo;
        unsigned long int TiempoExplosion;
        int Owner;
        unsigned Numero;
        Coordenada Pos;
        
};

Bomba :: Bomba( Coordenada& Pos, int Tipo, int Owner, unsigned Numero){
    this->Pos = Pos;
    this->Tipo = Tipo;
    this->Owner = Owner;
    this->Numero = Numero;
    this->TiempoExplosion = 0;
}

Bomba :: ~Bomba(){

}

void Bomba :: activar( unsigned long int Tiempo ){
    this->TiempoExplosion = Tiempo;
}

unsigned long int Bomba :: getTiempoExplosion(){
    return this->TiempoExplosion;
}

Coordenada Bomba :: getPos(){
    return this->Pos;
}

unsigned Bomba :: getNumero(){
    return this->Numero;
}

Bomba Bomba :: operator = ( const Bomba& obj ){
    this->Pos = obj.Pos;
    this->Tipo = obj.Tipo;
    this->Owner = obj.Owner;
    this->Numero = obj.Numero;
    this->TiempoExplosion = obj.TiempoExplosion;

    return *this;
}

#endif