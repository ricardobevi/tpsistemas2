/*
 * Premio.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Premio.h"

Premio :: Premio(Coordenada Pos , unsigned id){
    int randomType = rand() % 4;

    this->Pos = Pos;
    this->id = id;

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
            this->Tipo = 'R'; //Rapido (velocidad)

    }

}

Premio ::Premio(const Premio& obj){
	this->Pos = obj.Pos;
	this->id = obj.id;
	this->Tipo = obj.Tipo;
}

Premio :: ~Premio(){

}

char Premio :: getTipo(){
    return this->Tipo;
}

char Premio :: getId(){
	return this->id;
}

Coordenada Premio :: getPos(){
	return this->Pos;
}
