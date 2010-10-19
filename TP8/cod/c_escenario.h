#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <vector>
#include "coordenada.h"
#include <stdlib.h>
//#include "explosion.h"

using namespace std;

class Escenario
{
        
    public:
        
        vector < Coordenada > vidas;
        vector < Coordenada > jugadores;
        vector < Coordenada > paredesFijas;
        vector < Coordenada > paredesDestruibles;
        vector < Coordenada > premiosBomba;
        vector < Coordenada > premiosVida;
        vector < Coordenada > premiosExplosion;
        vector < Coordenada > premiosVelocidad;
        vector < unsigned int  > explosiones;
        
        unsigned int    tiempo;
              
        
};


#endif