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
    Espectador = false;
    Closed = false;

    this->Tipo = JUGADOR_ARTIFICIAL;

}

JugadorAutomatico::~JugadorAutomatico() {
}

int JugadorAutomatico::send( t_protocolo data ){
    return 1;
}

t_protocolo JugadorAutomatico::recv(){
    t_protocolo recibio = {0,0,-1,0};
    char aux;

    if( ! this->eliminado() && !Espectador ){

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

    }else if ( Espectador ){
        recibio.id = 'i';
        recibio.posicion = 0;
        recibio.x = -1;
        recibio.y = 0;
    }

    return recibio;

}

void JugadorAutomatico::Close(){
	this->Closed = true;
}
