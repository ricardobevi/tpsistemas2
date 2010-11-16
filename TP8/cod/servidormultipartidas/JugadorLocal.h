/*
 * JugadorLocal.h
 *
 *  Created on: 13/11/2010
 *      Author: ric
 */

#ifndef JUGADORLOCAL_H_
#define JUGADORLOCAL_H_

#include "../include/MemCompartida.h"
#include "Jugador.h"

class JugadorLocal: public Jugador {
public:
	JugadorLocal(int Numero,
				 int Vida,
				 Coordenada Posicion,
				 MemCompartida * mem,
				 int Velocidad,
				 int BombasMax,
				 int TipoBomba);

	virtual ~JugadorLocal();


	int send(t_protocolo data);

	t_protocolo recv();

	void Close();

private:
	MemCompartida * MemC;

};

#endif /* JUGADORLOCAL_H_ */
