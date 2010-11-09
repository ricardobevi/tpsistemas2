/*
 * Jugador.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Jugador.h"

Jugador::Jugador(int Numero, int Vida, Coordenada Posicion,
		Connection<char>& Socket, int Velocidad, int BombasMax, int TipoBomba) {

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

Jugador::~Jugador() {
}

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

int Jugador::getNumero(){
	return Numero;
}

int Jugador::send(t_protocolo data) {

	int enviado = -1;

	if (!Closed) {

		cout << "Enviando a jugador " << this->Numero << endl;

		cout << "id = " << data.id << endl
             << "posicion = " << data.posicion << endl
             << "x = " << data.x << endl
             << "y = " << data.y << endl;

		enviado = (int) Socket.Send((char*) &data, sizeof(t_protocolo));

		if (enviado == -1)
			Closed = true;

	}

	return enviado;
}

t_protocolo Jugador::recv() {
	t_protocolo recibido = { 0, 0, 0, 0 };

	if (!Closed) {
		ssize_t st_recibido;

		st_recibido = Socket.Recv((char*) &recibido, sizeof(t_protocolo));

		if (st_recibido <= -1)
			Closed = true;


		 cout << "    Recibido de jugador " << this->Numero << endl;

		 cout << "    id = "<< recibido.id << endl
		 << "    posicion = "<< recibido.posicion << endl
		 << "    x = "<< recibido.x << endl
		 << "    y = "<< recibido.y << endl;


	}

	return recibido;
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

bool Jugador::isClosed(){
	return Closed;
}

void Jugador::Close(){
	this->Socket.Close();
	Closed = true;
}
