/*
 * JugadorAutomatico.h
 *
 *  Created on: 09/11/2010
 *      Author: ric
 */

#ifndef JUGADORAUTOMATICO_H_
#define JUGADORAUTOMATICO_H_

#include "Jugador.h"

#include <unistd.h>

class JugadorAutomatico: public Jugador {
    public:
        JugadorAutomatico(unsigned (*Escenario)[10],
                          int Numero = -1,
                          int Vida = 0,
                          Coordenada Posicion = *(new Coordenada),
                          int Velocidad = 1,
                          int BombasMax = 1,
                          int TipoBomba = 1,
                          int VelocidadAuto = 1);

        virtual ~JugadorAutomatico();

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

        void setPosicion(Coordenada Posicion);

        Coordenada getPosicion();
        int getNumero();

        int send(t_protocolo data);

        t_protocolo recv();

        bool puedePonerBomba();
        Bomba ponerBomba(unsigned numBomba);
        void explotoBomba();

        void setEnvioEscenario(bool envio);
        bool getEnvioEscenario();

        void eliminar();
        bool eliminado();

        void setEspectador( bool espectador );
        bool isEspectador();

        bool isClosed();

        void Close();

    private:
        int Numero;
        int Vida;
        Coordenada Posicion;
        int Velocidad;
        int BombasColocadas;
        int BombasMax;
        int TipoBomba;

        bool EnvioEscenario;

        bool Espectador;

        bool Closed;

        unsigned (*Escenario)[10];
        int VelocidadAuto;


};

#endif /* JUGADORAUTOMATICO_H_ */
