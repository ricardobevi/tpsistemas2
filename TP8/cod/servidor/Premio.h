#ifndef PREMIO_H
#define PREMIO_H

#include <ctime>
#include <cstdlib>

#include "Coordenada.h"

class Premio {
    public:
        Premio(Coordenada& Pos = *(new Coordenada));
        ~Premio();

        char getTipo();
        
    private:
        char Tipo;
        Coordenada Pos;


};

Premio :: Premio(Coordenada& Pos){
    int randomType = rand() % 3;

    this->Pos = Pos;

    switch ( randomType ) {
        case 0:
            this->Tipo = 'B'; //Bomba
            break;
            
        case 1:
            this->Tipo = 'V'; //Vida
            break;
            
        case 2:
            this->Tipo = 'E'; //Explosion
            break;
            
        case 3:
            this->Tipo = 'S'; //Speed (velocidad)
            
    }
    
}

Premio :: ~Premio(){

}

char Premio :: getTipo(){
    return this->Tipo;
}

#endif