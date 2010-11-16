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
        int getNumero();
        void setNumero( int num );

        int getTipo();

        virtual int send( t_protocolo data ) = 0;

        virtual t_protocolo recv() = 0;

        bool puedePonerBomba();
        Bomba ponerBomba(unsigned numBomba);
        void explotoBomba();

        void setEnvioEscenario( bool envio );
        bool getEnvioEscenario();

        void eliminar();
        bool eliminado();

        void setEspectador(bool espectador);
        bool isEspectador();

        bool isClosed();

        virtual void Close() = 0;

        static const int JUGADOR_INACTIVO = 0;
		static const int JUGADOR_REMOTO = 1;
		static const int JUGADOR_LOCAL = 2;
		static const int JUGADOR_ARTIFICIAL = 3;

    protected:
        int Numero;
        int Tipo;
		int Vida;
		Coordenada Posicion;
		int Velocidad;
		int BombasColocadas;
		int BombasMax;
		int TipoBomba;

		bool EnvioEscenario;

		bool Espectador;

		bool Closed;


};

#endif /* JUGADOR_H_ */
