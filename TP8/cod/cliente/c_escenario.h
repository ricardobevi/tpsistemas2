#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <vector>
#include <map>
#include "../include/Coordenada.h"
#include <stdlib.h>

using namespace std;

class Escenario
{
        
    public:
        
        Escenario();
                
        vector < int > vidas;
        vector < Coordenada > jugadores;
        vector < Coordenada > paredesFijas;
        vector < Coordenada > paredesDestruibles;
        map < int, Coordenada > bombas;
         
        map < int, Coordenada > premiosBomba;
        map < int, Coordenada > premiosVida;
        map < int, Coordenada > premiosExplosion;
        map < int, Coordenada > premiosVelocidad;
        map < int , vector < Coordenada >  > explosiones;
        
        int    tiempo;
        
        

        
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