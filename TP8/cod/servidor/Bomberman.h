/*
 * Bomberman.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef BOMBERMAN_H_
#define BOMBERMAN_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

// Librerias para el generador de numeros aleatorios.
#include <ctime>
#include <cstdlib>

#include "Jugador.h"
#include "Bomba.h"
#include "Premio.h"
#include "Explosion.h"
#include "../include/Coordenada.h"

#include "../include/Comm.h"
#include "../include/Connection.h"

#define ACCION_MOVE 1
#define ACCION_BOMB 2

//#include "../include/t_protocolo.h"

// matriz de 9 X 37

class Bomberman {

    public:

        Bomberman();
        ~Bomberman();

        void activar( long int puerto, string archivoEscenario );

        int nuevoJugador();

        void sendEscenario( unsigned jugador );

        Jugador& getJugador( int jugador );

        unsigned getNumJugadores();

        queue<t_protocolo> procesarAccion( t_protocolo data );

        queue<t_protocolo> explotarBomba();

        t_protocolo expirarExplosion();

        t_protocolo recvFrom( unsigned jugador );

        int update( t_protocolo data );

        t_protocolo eliminarJugador( unsigned jugador );

        t_protocolo clockTick();

        static const unsigned JUGADORES_MAX = 4;
        static const unsigned X_MAX = 37;
        static const unsigned Y_MAX = 9;

        static const unsigned LUGAR_VACIO = 60000;
		static const unsigned PARED_FIJA = 59999;

    private:
        int Timer;
        unsigned long int HDTimer;

        vector < Jugador >    Jugadores;
        vector < bool > JugadorActivo;
        vector < Jugador > Espectadores;

        queue  < Bomba >      Bombas;
        queue  < Explosion >  Explosiones;
        vector < Premio >     Premios;
        vector < Coordenada > Paredes;
        vector < Coordenada > ParedesDestruibles;

        Comm< char > Socket;

        int VidaInicial;
        int MaxBombInicial;
        int VelocidadInicial;
        unsigned long int TiempoBomba,
						  TiempoExplosion,
						  TiempoEspera,
						  TiempoPartida;

        bool PartidaIniciada;

        unsigned NumJugadores;

        unsigned Escenario[ X_MAX + 1 ][ Y_MAX + 1 ];

        /*Teclas de accion*/
        int MoverArriba,
            MoverAbajo,
            MoverIzquierda,
            MoverDerecha,
            PonerBomba;

        queue<t_protocolo> tomaPremio( Coordenada coord, unsigned jugador );

};

#endif /* BOMBERMAN_H_ */
