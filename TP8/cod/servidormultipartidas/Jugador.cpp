/*
 * Jugador.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Jugador.h"

int Jugador::getVida() {
    return this->Vida;
}

int Jugador::sumarVida() {
    return ++this->Vida;
}

int Jugador::restarVida() {
    return --this->Vida;
}

void Jugador::sumarVelocidad() {
    this->Velocidad++;
}

int Jugador::getVelocidad() {
    return this->Velocidad;
}

void Jugador::sumarBombMax() {
    this->BombasMax++;
}

void Jugador::sumarTipoBomba() {
    this->TipoBomba++;
}

Coordenada Jugador::moverArriba() {
    Posicion.set_y(Posicion.get_y() - 1);
    return this->Posicion;
}

Coordenada Jugador::moverAbajo() {
    Posicion.set_y(Posicion.get_y() + 1);
    return this->Posicion;
}

Coordenada Jugador::moverIzquierda() {
    Posicion.set_x(Posicion.get_x() - 1);
    return this->Posicion;
}

Coordenada Jugador::moverDerecha() {
    Posicion.set_x(Posicion.get_x() + 1);
    return this->Posicion;
}

void Jugador::setPosicion(Coordenada Posicion) {
    this->Posicion = Posicion;
}

Coordenada Jugador::getPosicion() {
    return this->Posicion;
}

int Jugador::getNumero() {
    return Numero;
}

void Jugador::setNumero( int num ){
	this->Numero = num;
}

int Jugador::getTipo(){
	return this->Tipo;
}

bool Jugador::puedePonerBomba() {
    return (BombasColocadas < BombasMax);
}

Bomba Jugador::ponerBomba(unsigned numBomba) {
    Bomba bomb(Posicion, TipoBomba, Numero, numBomba);

    this->BombasColocadas++;

    return bomb;
}

void Jugador::explotoBomba() {
    this->BombasColocadas--;
}

void Jugador::setEnvioEscenario(bool envio) {
    this->EnvioEscenario = envio;
}

bool Jugador::getEnvioEscenario() {
    return this->EnvioEscenario;
}

void Jugador::eliminar() {
    this->Numero = (this->Numero + 1) * -1;
}

bool Jugador::eliminado() {
    return this->Numero < 0 ? true : false;
}

void Jugador::setEspectador(bool espectador) {
    this->Espectador = espectador;
}

bool Jugador::isEspectador() {
    return this->Espectador;
}

bool Jugador::isClosed() {
    return Closed;
}
