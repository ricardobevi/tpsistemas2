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


        int send(t_protocolo data);

        t_protocolo recv();

        void Close();

    private:
        unsigned (*Escenario)[10];
        int VelocidadAuto;


};

#endif /* JUGADORAUTOMATICO_H_ */
