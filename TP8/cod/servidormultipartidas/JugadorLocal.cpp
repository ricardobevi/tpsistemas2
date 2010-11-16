/*
 * JugadorLocal.cpp
 *
 *  Created on: 13/11/2010
 *      Author: ric
 */

#include "JugadorLocal.h"

JugadorLocal::JugadorLocal(int Numero,
                           int Vida,
                           Coordenada Posicion,
                           MemCompartida * mem,
                           int Velocidad,
                           int BombasMax,
                           int TipoBomba) {


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

    this->MemC = mem;

}

JugadorLocal::~JugadorLocal() {
    // TODO Auto-generated destructor stub
}

int JugadorLocal::getVida() {
    return this->Vida;
}

int JugadorLocal::sumarVida() {
    return ++this->Vida;
}

int JugadorLocal::restarVida() {
    return --this->Vida;
}

void JugadorLocal::sumarVelocidad() {
    this->Velocidad++;
}

int JugadorLocal::getVelocidad() {
    return this->Velocidad;
}

void JugadorLocal::sumarBombMax() {
    this->BombasMax++;
}

void JugadorLocal::sumarTipoBomba() {
    this->TipoBomba++;
}

Coordenada JugadorLocal::moverArriba() {
    Posicion.set_y(Posicion.get_y() - 1);
    return this->Posicion;
}

Coordenada JugadorLocal::moverAbajo() {
    Posicion.set_y(Posicion.get_y() + 1);
    return this->Posicion;
}

Coordenada JugadorLocal::moverIzquierda() {
    Posicion.set_x(Posicion.get_x() - 1);
    return this->Posicion;
}

Coordenada JugadorLocal::moverDerecha() {
    Posicion.set_x(Posicion.get_x() + 1);
    return this->Posicion;
}

void JugadorLocal::setPosicion(Coordenada Posicion) {
    this->Posicion = Posicion;
}

Coordenada JugadorLocal::getPosicion() {
    return this->Posicion;
}

int JugadorLocal::getNumero() {
    return Numero;
}

void JugadorLocal::setNumero( int num ){
	this->Numero = num;
}

int JugadorLocal::send(t_protocolo data) {

    int enviado = 1;

    if ( !Closed ) {
         /*
         cout << "Enviando a jugador " << this->Numero << endl;

         cout << "id = " << data.id << endl << "posicion = " << data.posicion << endl << "x = "
         << data.x << endl << "y = " << data.y << endl;
        */
        //enviado = (int) Socket.Send((char*) &data, sizeof(t_protocolo));

        /*if ( enviado <= -1 )
            Closed = true;*/

    	MemC->enviarACliente( data );

    }

    return enviado;
}

t_protocolo JugadorLocal::recv() {
    t_protocolo recibido = { 'i', 0, -1, 0 };

    if ( !Closed ) {

		if ( !Espectador ) {

			MemC->recibirDeCliente( recibido );

			/*
			cout << "    Recibido de jugador " << this->Numero << endl;

			cout << "    id = " << recibido.id << endl << "    posicion = " << recibido.posicion
			<< endl << "    x = " << recibido.x << endl << "    y = " << recibido.y << endl;
			*/

		}else{

			recibido.x = -2;

		}

	}



    return recibido;
}

bool JugadorLocal::puedePonerBomba() {
    return (BombasColocadas < BombasMax);
}

Bomba JugadorLocal::ponerBomba(unsigned numBomba) {
    Bomba bomb(Posicion, TipoBomba, Numero, numBomba);

    this->BombasColocadas++;

    return bomb;
}

void JugadorLocal::explotoBomba() {
    this->BombasColocadas--;
}

void JugadorLocal::setEnvioEscenario(bool envio) {
    this->EnvioEscenario = envio;
}

bool JugadorLocal::getEnvioEscenario() {
    return this->EnvioEscenario;
}

void JugadorLocal::eliminar() {
    this->Numero = (this->Numero + 1) * -1;
}

bool JugadorLocal::eliminado() {
    return this->Numero < 0 ? true : false;
}

void JugadorLocal::setEspectador(bool espectador) {
    this->Espectador = espectador;
}

bool JugadorLocal::isEspectador() {
    return this->Espectador;
}

bool JugadorLocal::isClosed() {
    return Closed;
}

void JugadorLocal::Close() {
	this->MemC->eliminarMemoriaCompartida( SERVIDOR );
    Closed = true;
}
