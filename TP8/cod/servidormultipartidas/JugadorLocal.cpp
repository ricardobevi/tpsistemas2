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

void JugadorLocal::Close(){
	this->Closed = true;
}
