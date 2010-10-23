#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <vector>

#include "Coordenada.h"
#include "Bomba.h"

using namespace std;

class Explosion {
    public:
        Explosion( Bomba& Bomb = *( new Bomba() ) );
        ~Explosion();
        
    private:
        vector < Coordenada > Expancion;
        Bomba Bomb;

};


Explosion :: Explosion( Bomba& Bomb ){
    //this->Bomb = Bomb;
}

Explosion :: ~Explosion(){

}

#endif