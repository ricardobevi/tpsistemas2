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

        void activar(long int puerto, string archivoEscenario);

        int nuevoJugador();

        queue<t_protocolo> sendEscenario(int jugador, bool toAll = false);

        Jugador& getJugador(int jugador);

        unsigned getNumJugadores();

        unsigned long int getTiempoEspera();

        queue<t_protocolo> iniciarPartida();

        queue<t_protocolo> procesarAccion(t_protocolo data);

        queue<t_protocolo> explotarBomba();

        t_protocolo expirarExplosion();

        t_protocolo recvFrom(unsigned jugador);

        int update(t_protocolo data);

        t_protocolo eliminarJugador(unsigned jugador, bool close = false);

        void eliminarEspectador(int espectador);

        t_protocolo clockTick();

        void resetClock();

        static const unsigned JUGADORES_MAX = 4;
        static const unsigned X_MAX = 37;
        static const unsigned Y_MAX = 9;

        static const unsigned BOMBA = 60001;
        static const unsigned LUGAR_VACIO = 60000;
        static const unsigned PARED_FIJA = 59999;

    private:
        int Timer;
        unsigned long int HDTimer;

        Jugador Jugadores[JUGADORES_MAX];
        bool JugadoresActivos[JUGADORES_MAX];

        map<int, Jugador> Espectadores;

        queue<Bomba> Bombas;
        queue<Explosion> Explosiones;
        vector<Premio> Premios;
        vector<Coordenada> Paredes;
        vector<Coordenada> ParedesDestruibles;

        Comm<char> Socket;

        int VidaInicial;
        int MaxBombInicial;
        int VelocidadInicial;
        unsigned long int TiempoBomba, TiempoExplosion, TiempoEspera, TiempoPartida;

        bool PartidaIniciada;

        unsigned NumJugadores;

        unsigned Escenario[X_MAX + 1][Y_MAX + 1];

        /*Teclas de accion*/
        int MoverArriba, MoverAbajo, MoverIzquierda, MoverDerecha, PonerBomba;

        queue<t_protocolo> tomaPremio(Coordenada coord, unsigned jugador);

};

#endif /* BOMBERMAN_H_ */
