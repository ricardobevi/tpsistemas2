/*
 * JugadorLocal.h
 *
 *  Created on: 13/11/2010
 *      Author: ric
 */

#ifndef JUGADORLOCAL_H_
#define JUGADORLOCAL_H_

#include "Jugador.h"
#include "../include/MemCompartida.h"

class JugadorLocal: public Jugador {
public:
	JugadorLocal(int Numero,
				 int Vida,
				 Coordenada Posicion,
				 MemCompartida * mem,
				 int Velocidad,
				 int BombasMax,
				 int TipoBomba);

	~JugadorLocal();

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
	void setNumero( int num );

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

	MemCompartida * MemC;
};

#endif /* JUGADORLOCAL_H_ */
