/*
 * Bomba.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Bomba.h"

Bomba :: Bomba( Coordenada& Pos,
		        unsigned Tipo,
		        int Owner,
		        unsigned Numero ){

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

int Bomba :: getTipo(){
	return this->Tipo;
}

Coordenada Bomba :: getPos(){
    return this->Pos;
}

unsigned Bomba :: getNumero(){
    return this->Numero;
}

int Bomba :: getOwner(){
    return this->Owner;
}

Bomba Bomba :: operator = ( const Bomba& obj ){
    this->Pos = obj.Pos;
    this->Tipo = obj.Tipo;
    this->Owner = obj.Owner;
    this->Numero = obj.Numero;
    this->TiempoExplosion = obj.TiempoExplosion;

    return *this;
}
