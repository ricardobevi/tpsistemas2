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
#include "JugadorRemoto.h"
#include "JugadorLocal.h"
#include "JugadorAutomatico.h"
#include "Bomba.h"
#include "Premio.h"
#include "Explosion.h"
#include "../include/Coordenada.h"
#include "../include/MemCompartida.h"


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

        void activar(string archivoConfiguracion);

        Jugador * esperarJugadorRemoto();

        Jugador * esperarJugadorLocal();

        bool hayJugadorLocal();

        int nuevoJugador(Jugador * Player);

        queue<t_protocolo> sendEscenario(int jugador, bool toAll = false);

        Jugador * getJugador(int jugador);

        unsigned getNumJugadores();

        unsigned long int getTiempoEspera();

        queue<t_protocolo> iniciarPartida();

        queue<t_protocolo> finalizarPartida();

        queue<t_protocolo> procesarAccion(t_protocolo data);

        queue<t_protocolo> explotarBomba();

        t_protocolo expirarExplosion();

        t_protocolo recvFrom(unsigned jugador);

        int update(t_protocolo data);

        t_protocolo eliminarJugador(unsigned jugador, bool close = false);

        void eliminarEspectador(int espectador);

        t_protocolo clockTick();

        void resetClock();

        queue<t_protocolo> CloseClosed();

        void Close();

        void CloseSocket();

        void CloseMemC();

        void Reset();

        static const unsigned JUGADORES_MAX = 4;
        static const unsigned X_MAX = 37;
        static const unsigned Y_MAX = 9;

        static const unsigned BOMBA = 60001;
        static const unsigned LUGAR_VACIO = 60000;
        static const unsigned PARED_FIJA = 59999;

    private:
        string ArchConf;

        int Timer;
        unsigned long int HDTimer;

        unsigned Escenario[X_MAX + 1][Y_MAX + 1];

        Jugador * Jugadores[JUGADORES_MAX];
        map<int, Jugador *> Espectadores;

        int TipoJugador[JUGADORES_MAX];

        static const int JUGADOR_INACTIVO = 0;
        static const int JUGADOR_REMOTO = 1;
        static const int JUGADOR_LOCAL = 2;
        static const int JUGADOR_ARTIFICIAL = 3;

        queue<Bomba> Bombas;
        queue<Explosion> Explosiones;
        vector<Premio> Premios;
        vector<Coordenada> Paredes;
        vector<Coordenada> ParedesDestruibles;

        Comm<char> * Socket;
        MemCompartida MemC;

        int VidaInicial;
        int MaxBombInicial;
        int VelocidadInicial;
        int VelocidadJugadorAutomatico;
        int PorcentajePremios;
        unsigned long int TiempoBomba, TiempoExplosion, TiempoEspera, TiempoPartida;

        bool PartidaIniciada;

        unsigned NumJugadores;

        queue<int> Posiciones;

        /*Teclas de accion*/
        int MoverArriba, MoverAbajo, MoverIzquierda, MoverDerecha, PonerBomba;

        queue<t_protocolo> tomaPremio(Coordenada coord, unsigned jugador);

};

#endif /* BOMBERMAN_H_ */
