#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <vector>
#include "../include/Coordenada.h"
#include <stdlib.h>

using namespace std;

class Escenario
{
        
    public:
        
        Escenario();
                
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


Escenario::  Escenario()
{
   for (unsigned int i = 0 ; i<4 ; i++) 
        vidas.push_back(0);

   
   jugadores.push_back( Coordenada (-1,-1)  );
   jugadores.push_back( Coordenada (-1,-1) );
   jugadores.push_back( Coordenada (-1,-1)  );
   jugadores.push_back( Coordenada (-1,-1) );
   
                
}

#endif