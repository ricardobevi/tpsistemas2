/*
 * JugadorRemoto.cpp
 *
 *  Created on: 09/11/2010
 *      Author: ric
 */

#include "JugadorRemoto.h"

JugadorRemoto::JugadorRemoto(int Numero,
                             int Vida,
                             Coordenada Posicion,
                             Connection<char>& Socket,
                             int Velocidad,
                             int BombasMax,
                             int TipoBomba) {

    this->Socket = Socket;

    this->Numero = Numero;
    this->Vida = Vida;
    this->Posicion = Posicion;
    this->Velocidad = Velocidad;
    this->BombasColocadas = 0;
    this->BombasMax = BombasMax;
    this->TipoBomba = TipoBomba;

    EnvioEscenario = false;
    Espectador = false;
    Closed = false;

    this->Tipo = JUGADOR_REMOTO;

}

JugadorRemoto::~JugadorRemoto() {
    // TODO Auto-generated destructor stub
}

int JugadorRemoto::send(t_protocolo data) {

    int enviado = -1;

    if ( !Closed ) {
         /*
         cout << "Enviando a jugador " << this->Numero << endl;

         cout << "id = " << data.id << endl << "posicion = " << data.posicion << endl << "x = "
         << data.x << endl << "y = " << data.y << endl;
        */
        enviado = (int) Socket.Send((char*) &data, sizeof(t_protocolo));

        if ( enviado <= -1 )
            Closed = true;

    }

    return enviado;
}

t_protocolo JugadorRemoto::recv() {
    t_protocolo recibido = { 'i', 0, -1, 0 };

    if ( !Closed ) {

        if ( !Espectador ) {
            ssize_t st_recibido;

            st_recibido = Socket.Recv((char*) &recibido, sizeof(t_protocolo));

            if ( st_recibido <= -1 )
                Closed = true;
        }else{
            recibido.x = -2;
        }
        /*
         cout << "    Recibido de jugador " << this->Numero << endl;

         cout << "    id = " << recibido.id << endl << "    posicion = " << recibido.posicion
         << endl << "    x = " << recibido.x << endl << "    y = " << recibido.y << endl;
         */
    }

    return recibido;
}


void JugadorRemoto::Close() {
    this->Socket.Close();
    Closed = true;
}
