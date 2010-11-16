/*
 * JugadorRemoto.h
 *
 *  Created on: 09/11/2010
 *      Author: ric
 */

#ifndef JUGADORREMOTO_H_
#define JUGADORREMOTO_H_

#include "Jugador.h"

class JugadorRemoto: public Jugador {
    public:
        JugadorRemoto(int Numero = -1,
                      int Vida = 0,
                      Coordenada Posicion = *(new Coordenada),
                      Connection<char>& Socket = *(new Connection<char> ),
                      int Velocidad = 1,
                      int BombasMax = 1,
                      int TipoBomba = 1);

        virtual ~JugadorRemoto();

        int send(t_protocolo data);

        t_protocolo recv();

        void Close();

    private:
        Connection<char> Socket;

};

#endif /* JUGADORREMOTO_H_ */
