/*
 * Explosion.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include <vector>
#include <queue>
#include <map>

#include "../include/Coordenada.h"
#include "../include/t_protocolo.h"

#include "Bomba.h"
#include "Premio.h"

using namespace std;

class Explosion {
    public:
        Explosion(Bomba Bomb = *(new Bomba()),
                  unsigned id = 0,
                  unsigned(*Escenario)[10] = NULL,
                  unsigned max_x = 0,
                  unsigned max_y = 0);

        ~Explosion();

        unsigned getId();

        void calcularExplosion(queue<t_protocolo>& tp_retorno,
                               vector<Coordenada>& paredesDestruibles,
                               vector<Premio>& premios,
                               unsigned long int * timer);

        bool pertenece(Coordenada coord);

        void setTiempoExpiracion(unsigned long int Tiempo);
        unsigned long int getTiempoExpiracion();

    private:
        map<int, vector<int> > MExpancion;
        Bomba Bomb;

        unsigned (*Escenario)[10], id, max_x, max_y;

        unsigned long int TiempoExpiracion;

};

#endif /* EXPLOSION_H_ */
