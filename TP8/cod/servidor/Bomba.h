#ifndef BOMBA_H
#define BOMBA_H

#include "Coordenada.h"

class Bomba {
    public:
        Bomba(Coordenada& Pos = *(new Coordenada()),unsigned int TiempoExplosion = 0, int Tipo = 1);
        ~Bomba();
        
    private:
        int Tipo;
        unsigned int TiempoExplosion;
        Coordenada Pos;
        
};

Bomba :: Bomba( Coordenada& Pos, unsigned int TiempoExplosion, int Tipo ){
    this->Pos = Pos;
    this->TiempoExplosion = TiempoExplosion;
    this->Tipo = Tipo;
}

Bomba :: ~Bomba(){

}

#endif