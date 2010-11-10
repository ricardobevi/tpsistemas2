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

        Jugador();

        ~Jugador();

        virtual int getVida() = 0;
        virtual int restarVida() = 0;
        virtual int sumarVida() = 0;

        virtual void sumarVelocidad() = 0;
        virtual int getVelocidad() = 0;
        virtual void sumarBombMax() = 0;
        virtual void sumarTipoBomba() = 0;

        virtual Coordenada moverArriba() = 0;
        virtual Coordenada moverAbajo() = 0;
        virtual Coordenada moverIzquierda() = 0;
        virtual Coordenada moverDerecha() = 0;

        virtual void setPosicion( Coordenada Posicion ) = 0;

        virtual Coordenada getPosicion() = 0;
        virtual int getNumero() = 0;

        virtual int send( t_protocolo data ) = 0;

        virtual t_protocolo recv() = 0;

        virtual bool puedePonerBomba() = 0;
        virtual Bomba ponerBomba(unsigned numBomba) = 0;
        virtual void explotoBomba() = 0;

        virtual void setEnvioEscenario( bool envio ) = 0;
        virtual bool getEnvioEscenario() = 0;

        virtual void eliminar() = 0;
        virtual bool eliminado() = 0;

        virtual bool isClosed() = 0;

        virtual void Close() = 0;


};

#endif /* JUGADOR_H_ */
