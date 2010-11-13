/*
 * Premio.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef PREMIO_H_
#define PREMIO_H_

#include <ctime>
#include <cstdlib>

#include "../include/Coordenada.h"

class Premio {
    public:
        Premio(Coordenada Pos = *(new Coordenada), unsigned long int id = 0, int ProbPremio = 100);
        Premio( const Premio& obj);
        ~Premio();

        char getTipo();
        char getId();
        Coordenada getPos();

    private:
        char Tipo;
        Coordenada Pos;
        unsigned long int id;
        int ProbPremio;


};

#endif /* PREMIO_H_ */
