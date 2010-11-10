/*
 * JugadorAutomatico.cpp
 *
 *  Created on: 09/11/2010
 *      Author: ric
 */

#include "JugadorAutomatico.h"
#include <stdlib.h>

JugadorAutomatico::JugadorAutomatico(unsigned (*Escenario)[10],
                                     int Numero,
                                     int Vida,
                                     Coordenada Posicion,
                                     int Velocidad,
                                     int BombasMax,
                                     int TipoBomba,
                                     int VelocidadAuto){
    // Inicio jugador automatico.

    this->Escenario = Escenario;
    this->VelocidadAuto = VelocidadAuto;

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

JugadorAutomatico::~JugadorAutomatico() {
}

int JugadorAutomatico::getVida() {
    return this->Vida;
}

int JugadorAutomatico::sumarVida() {
    return ++this->Vida;
}

int JugadorAutomatico::restarVida() {
    return --this->Vida;
}

void JugadorAutomatico::sumarVelocidad() {
    this->Velocidad++;
}

int JugadorAutomatico::getVelocidad() {
    return this->Velocidad;
}

void JugadorAutomatico::sumarBombMax() {
    this->BombasMax++;
}

void JugadorAutomatico::sumarTipoBomba() {
    this->TipoBomba++;
}

Coordenada JugadorAutomatico::moverArriba() {
    Posicion.set_y(Posicion.get_y() - 1);
    return this->Posicion;
}

Coordenada JugadorAutomatico::moverAbajo() {
    Posicion.set_y(Posicion.get_y() + 1);
    return this->Posicion;
}

Coordenada JugadorAutomatico::moverIzquierda() {
    Posicion.set_x(Posicion.get_x() - 1);
    return this->Posicion;
}

Coordenada JugadorAutomatico::moverDerecha() {
    Posicion.set_x(Posicion.get_x() + 1);
    return this->Posicion;
}

void JugadorAutomatico::setPosicion(Coordenada Posicion) {
    this->Posicion = Posicion;
}

Coordenada JugadorAutomatico::getPosicion() {
    return this->Posicion;
}

int JugadorAutomatico::getNumero() {
    return Numero;
}

int JugadorAutomatico::send( t_protocolo data ){
    return 1;
}

t_protocolo JugadorAutomatico::recv(){
    t_protocolo recibio = {0,0,-1,0};
    char aux;

    if( ! this->eliminado() ){

        aux = rand() % 5;

        switch( aux ){
            case 0:
                aux = 'w';
                break;
            case 1:
                aux = 's';
                break;
            case 2:
                aux = 'a';
                break;
            case 3:
                aux = 'd';
                break;
            case 4:
                aux = 'b';
                break;
        }

        if ( Velocidad < 10 )
            usleep(1000000 - ( Velocidad * 100000 ) );
        else
            usleep(100000);

        recibio.id = 'i';
        recibio.posicion = 0;
        recibio.x = aux;
        recibio.y = 0;

    }

    return recibio;

}

bool JugadorAutomatico::puedePonerBomba() {
    return (BombasColocadas < BombasMax);
}

Bomba JugadorAutomatico::ponerBomba(unsigned numBomba) {
    Bomba bomb(Posicion, TipoBomba, Numero, numBomba);

    this->BombasColocadas++;

    return bomb;
}

void JugadorAutomatico::explotoBomba() {
    this->BombasColocadas--;
}

void JugadorAutomatico::setEnvioEscenario(bool envio) {
    this->EnvioEscenario = envio;
}

bool JugadorAutomatico::getEnvioEscenario() {
    return this->EnvioEscenario;
}

void JugadorAutomatico::eliminar() {
    this->Numero = (this->Numero + 1) * -1;
}

bool JugadorAutomatico::eliminado() {
    return this->Numero < 0 ? true : false;
}

bool JugadorAutomatico::isClosed() {
    return Closed;
}

void JugadorAutomatico::Close(){
    Closed = true;
}
