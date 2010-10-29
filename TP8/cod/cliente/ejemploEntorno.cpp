#include <iostream>
#include <stdlib.h>

#include "entorno.h"

using namespace std;

int main(int argc, char *argv[])
{
    // ****************  DATOS ESCENARIO ********************* //
    /*

        vector <  int > vidas;
        vector < Coordenada > jugadores;
        vector < Coordenada > paredesFijas;
        vector < Coordenada > paredesDestruibles;
        vector < Coordenada > premiosBomba;
        vector < Coordenada > premiosVida;
        vector < Coordenada > premiosExplosion;
        vector < Coordenada > premiosVelocidad;
        vector < Coordenada > bombas;
        vector < vector < Coordenada >  > explosiones;
        
        unsigned int    tiempo;
    */      
    
   // cargo un monton de datos en escenario para poder trabajar el entorno
    
   Escenario escenarioEjemplo;
   escenarioEjemplo.tiempo = 20;
   (escenarioEjemplo.vidas).resize(4);
   (escenarioEjemplo.vidas).at(0) = 5;
   (escenarioEjemplo.vidas).at(1) = 2;
   (escenarioEjemplo.vidas).at(2) = 0;
   (escenarioEjemplo.vidas).at(3) = 0;
   
   (escenarioEjemplo.jugadores).push_back( Coordenada(0,0) );
   (escenarioEjemplo.jugadores).push_back( Coordenada(0,37) );
   (escenarioEjemplo.jugadores).push_back( Coordenada(9,0) );
   (escenarioEjemplo.jugadores).push_back( Coordenada(9,37) );
   
   //inserto paredes fijas
   
   for(unsigned int i=1; i <= 9 ; i = i+2)
       for(unsigned int j=1; j <= 37 ; j=j+2 )
       {
           if( !(i == 0  && j== 0)   &&
               !(i == 9  && j== 37)  && 
               !(i == 0  && j== 37)  && 
               !(i == 9  && j== 0)    )
               
                    escenarioEjemplo.paredesFijas.push_back(Coordenada(i,j));
       }
   
   //inserto paredes Destruibles
   
   escenarioEjemplo.paredesDestruibles.push_back(Coordenada(5,5));
   escenarioEjemplo.paredesDestruibles.push_back(Coordenada(13,13));
   escenarioEjemplo.paredesDestruibles.push_back(Coordenada(2,2));
   escenarioEjemplo.paredesDestruibles.push_back(Coordenada(0,5));
   escenarioEjemplo.paredesDestruibles.push_back(Coordenada(0,15));
    
   
   escenarioEjemplo.premiosVida.push_back(Coordenada(0,20));
   escenarioEjemplo.premiosExplosion.push_back(Coordenada(5,31));
   escenarioEjemplo.premiosVelocidad.push_back(Coordenada(4,12));
   escenarioEjemplo.premiosBomba.push_back(Coordenada(8,15));
   
    escenarioEjemplo.bombas.push_back(Coordenada(6,25));
    
    vector < Coordenada > explosion;
    explosion.push_back(Coordenada(4,0));
    explosion.push_back(Coordenada(4,1));
    explosion.push_back(Coordenada(4,2));
    explosion.push_back(Coordenada(4,3));
     explosion.push_back(Coordenada(4,4));
    explosion.push_back(Coordenada(5,2));
    explosion.push_back(Coordenada(3,2));
    
    escenarioEjemplo.explosiones.push_back(explosion);
   
   Entorno ejemplo( &escenarioEjemplo );    
}
  