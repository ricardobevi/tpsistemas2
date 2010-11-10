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
    Closed = false;

}

JugadorRemoto::~JugadorRemoto() {
    // TODO Auto-generated destructor stub
}

int JugadorRemoto::getVida() {
    return this->Vida;
}

int JugadorRemoto::sumarVida() {
    return ++this->Vida;
}

int JugadorRemoto::restarVida() {
    return --this->Vida;
}

void JugadorRemoto::sumarVelocidad() {
    this->Velocidad++;
}

int JugadorRemoto::getVelocidad() {
    return this->Velocidad;
}

void JugadorRemoto::sumarBombMax() {
    this->BombasMax++;
}

void JugadorRemoto::sumarTipoBomba() {
    this->TipoBomba++;
}

Coordenada JugadorRemoto::moverArriba() {
    Posicion.set_y(Posicion.get_y() - 1);
    return this->Posicion;
}

Coordenada JugadorRemoto::moverAbajo() {
    Posicion.set_y(Posicion.get_y() + 1);
    return this->Posicion;
}

Coordenada JugadorRemoto::moverIzquierda() {
    Posicion.set_x(Posicion.get_x() - 1);
    return this->Posicion;
}

Coordenada JugadorRemoto::moverDerecha() {
    Posicion.set_x(Posicion.get_x() + 1);
    return this->Posicion;
}

void JugadorRemoto::setPosicion(Coordenada Posicion) {
    this->Posicion = Posicion;
}

Coordenada JugadorRemoto::getPosicion() {
    return this->Posicion;
}

int JugadorRemoto::getNumero() {
    return Numero;
}

int JugadorRemoto::send(t_protocolo data) {

    int enviado = -1;

    if ( !Closed ) {
        /*
        cout << "Enviando a jugador " << this->Numero << endl;

        cout << "id = " << data.id << endl << "posicion = " << data.posicion << endl << "x = "
                << data.x << endl << "y = " << data.y << endl;*/

        enviado = (int) Socket.Send((char*) &data, sizeof(t_protocolo));

        if ( enviado <= -1 )
            Closed = true;

    }

    return enviado;
}

t_protocolo JugadorRemoto::recv() {
    t_protocolo recibido = { 0, 0, -1, 0 };

    if ( !Closed ) {
        ssize_t st_recibido;

        st_recibido = Socket.Recv((char*) &recibido, sizeof(t_protocolo));

        if ( st_recibido <= -1 )
            Closed = true;

        cout << "    Recibido de jugador " << this->Numero << endl;

        cout << "    id = " << recibido.id << endl << "    posicion = " << recibido.posicion
                << endl << "    x = " << recibido.x << endl << "    y = " << recibido.y << endl;

    }

    return recibido;
}

bool JugadorRemoto::puedePonerBomba() {
    return (BombasColocadas < BombasMax);
}

Bomba JugadorRemoto::ponerBomba(unsigned numBomba) {
    Bomba bomb(Posicion, TipoBomba, Numero, numBomba);

    this->BombasColocadas++;

    return bomb;
}

void JugadorRemoto::explotoBomba() {
    this->BombasColocadas--;
}

void JugadorRemoto::setEnvioEscenario(bool envio) {
    this->EnvioEscenario = envio;
}

bool JugadorRemoto::getEnvioEscenario() {
    return this->EnvioEscenario;
}

void JugadorRemoto::eliminar() {
    this->Numero = (this->Numero + 1) * -1;
}

bool JugadorRemoto::eliminado() {
    return this->Numero < 0 ? true : false;
}

bool JugadorRemoto::isClosed() {
    return Closed;
}

void JugadorRemoto::Close(){
    this->Socket.Close();
    Closed = true;
}
