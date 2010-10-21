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
        
        vector < unsigned int > vidas;
        vector < Coordenada > jugadores;
        vector < Coordenada > paredesFijas;
        vector < Coordenada > paredesDestruibles;
        vector < Coordenada > bombas;
         
        vector < Coordenada > premiosBomba;
        vector < Coordenada > premiosVida;
        vector < Coordenada > premiosExplosion;
        vector < Coordenada > premiosVelocidad;
        vector < vector < Coordenada >  > explosiones;
        
        unsigned int    tiempo;
              
};


#endif