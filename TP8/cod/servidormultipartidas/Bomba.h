/*
 * Bomba.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef BOMBA_H_
#define BOMBA_H_

#include "../include/Coordenada.h"

class Bomba {
    public:
        Bomba(Coordenada& Pos = *(new Coordenada()),
        	  unsigned Tipo = 1,
        	  int Owner = -1,
        	  unsigned Numero = 0);

        ~Bomba();

        void activar( unsigned long int Tiempo );

        unsigned long int getTiempoExplosion();

        int getTipo();
        Coordenada getPos();
        unsigned getNumero();
        int getOwner();

        Bomba operator = ( const Bomba& obj );

    private:
        unsigned Tipo;
        unsigned long int TiempoExplosion;
        int Owner;
        unsigned Numero;
        Coordenada Pos;

};

#endif /* BOMBA_H_ */
