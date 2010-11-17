/*
 * Premio.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Premio.h"

#include <iostream>

Premio :: Premio(Coordenada Pos , unsigned long int id, int ProbPremio){

    this->Pos = Pos;
    this->id = id;
    this->ProbPremio = ProbPremio;

    float cantElementos = 4.0 / ( (float) ProbPremio / 100.0 );

    int randomType = rand() % (int) cantElementos;

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
            break;

        default:
            this->Tipo = 'N'; //NADA

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
