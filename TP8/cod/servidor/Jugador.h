/*
 * Jugador.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef JUGADOR_H_
#define JUGADOR_H_

#include "Bomba.h"

#include "../include/Coordenada.h"
#include "../include/Connection.h"

#include "../include/t_protocolo.h"

class Jugador {

    public:

        Jugador( int Numero,
                 int Vida,
                 Coordenada Posicion,
                 Connection<char>& Socket,
                 int Velocidad = 1,
                 int BombasMax = 1,
                 int TipoBomba = 1 );

        ~Jugador();

        int getVida();
        int restarVida();
        int sumarVida();

        void sumarVelocidad();
        int getVelocidad();
        void sumarBombMax();
        void sumarTipoBomba();

        Coordenada moverArriba();
        Coordenada moverAbajo();
        Coordenada moverIzquierda();
        Coordenada moverDerecha();

        void setPosicion( Coordenada Posicion );

        Coordenada getPosicion();

        void send( t_protocolo data );

        t_protocolo recv();

        bool puedePonerBomba();
        Bomba ponerBomba(unsigned numBomba);
        void explotoBomba();

        void setEnvioEscenario( bool envio );
        bool getEnvioEscenario();

        void eliminar();
        bool eliminado();

    private:

        int Numero;
        int Vida;
        Coordenada Posicion;
        int Velocidad;
        int BombasColocadas;
        int BombasMax;
        int TipoBomba;

        bool EnvioEscenario;

        Connection<char> Socket;

};

#endif /* JUGADOR_H_ */
