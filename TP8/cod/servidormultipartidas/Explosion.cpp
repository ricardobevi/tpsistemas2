/*
 * Explosion.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Explosion.h"
#include "Bomberman.h"

Explosion :: Explosion( Bomba Bomb,
						unsigned id,
						unsigned (*Escenario)[10],
					    unsigned max_x,
					    unsigned max_y,
					    int porcentajePremios){

    this->Bomb = Bomb;
    this->Escenario = Escenario;
    this->id = id;
	this->max_x = max_x;
	this->max_y = max_y;
	this->PorcentajePremios = porcentajePremios;


}

Explosion :: ~Explosion(){
}

unsigned Explosion :: getId(){
	return this->id;
}

void Explosion ::  calcularExplosion( queue< t_protocolo >& tp_retorno,
        		                      vector<Coordenada>& paredesDestruibles,
        		                      vector<Premio>& premios,
        		                      unsigned long int * timer){
	unsigned expancion = Bomb.getTipo();
	unsigned i = 1;
	int startX = 0;
	int startY = 0;
	t_protocolo enviar;

	Bomb.getPos().get_coordenada( startX, startY );

	enviar.id = 'e';
	enviar.posicion = id;
	enviar.x = startX;
	enviar.y = startY;

	tp_retorno.push( enviar );

    /*
     * Pruebo cuanto puede crecer hacia arriba.
     */

    i = 1;

	while ( (startY - (int) i) >= 0 &&
			i <= expancion &&
			Escenario[startX][startY - i] >= Bomberman::LUGAR_VACIO ) {

		enviar.x = startX;
		enviar.y = startY - i;

		tp_retorno.push( enviar );
		MExpancion[startX].push_back(startY - i);

		i++;
	}

	if ( (startY - (int) i) >= 0 &&
		 i <= expancion &&
		 Escenario[startX][startY - i] < Bomberman::PARED_FIJA ){

		enviar.x = startX;
		enviar.y = startY - i;

		tp_retorno.push( enviar );
		MExpancion[startX].push_back(startY - i);

	}

    /*
	 * Pruebo cuanto puede crecer hacia abajo.
	 */

	i = 1;

	while ( (startY + i) <= this->max_y &&
			i <= expancion &&
		    Escenario[startX][startY + i] >= Bomberman::LUGAR_VACIO ) {

		enviar.x = startX;
		enviar.y = startY + i;

		tp_retorno.push( enviar );
		MExpancion[startX].push_back(startY + i);

		i++;
	}

	if ( (startY + i) <= this->max_y &&
			i <= expancion &&
			Escenario[startX][startY + i] < Bomberman::PARED_FIJA ) {

		enviar.x = startX;
		enviar.y = startY + i;

		tp_retorno.push( enviar );
		MExpancion[startX].push_back(startY + i);

	}

	/*
	 * Pruebo cuanto puede crecer hacia la izquierda.
	 */

	i = 1;

	while ( (startX - (int) i) >= 0 &&
			i <= expancion &&
			Escenario[startX - i][startY] >= Bomberman::LUGAR_VACIO ) {

		enviar.x = startX - i;
		enviar.y = startY;

		tp_retorno.push( enviar );
		MExpancion[startX - i].push_back(startY);

		i++;
	}

	if ( (startX - (int) i) >= 0 &&
			i <= expancion &&
			Escenario[startX - i][startY] < Bomberman::PARED_FIJA ) {

		enviar.x = startX - i;
		enviar.y = startY;

		tp_retorno.push( enviar );
		MExpancion[startX - i].push_back(startY);

		i++;
	}

	/*
	 * Pruebo cuanto puede crecer hacia la derecha.
	 */

	i = 1;

	while ( (startX + i) <= this->max_x &&
			i <= expancion &&
			Escenario[startX + i][startY] >= Bomberman::LUGAR_VACIO ) {

		enviar.x = startX + i;
		enviar.y = startY;

		tp_retorno.push( enviar );
		MExpancion[startX + i].push_back(startY);

		i++;
	}


	if ( (startX + i) <= this->max_x &&
			i <= expancion &&
			Escenario[startX + i][startY] < Bomberman::PARED_FIJA ) {

		enviar.x = startX + i;
		enviar.y = startY;

		tp_retorno.push( enviar );
		MExpancion[startX + i].push_back(startY);

		i++;
	}



	/*
	 * Verifico si se destruyo alguna pared y la elimino.
	 */

	map< int, vector <int> >::iterator it;

	for ( it = MExpancion.begin() ; it != MExpancion.end(); it++ ){

		for ( unsigned j = 0 ; j < (*it).second.size() ; j++ ){

			int paredID = Escenario[ (*it).first ][ (*it).second[j] ];

			if ( (unsigned) paredID < Bomberman::PARED_FIJA ){
				enviar.id = 'd';
				enviar.posicion = paredID;
				enviar.x = enviar.y = -2;

				paredesDestruibles[ paredID ].set_coordenada(-1, -1);

				Escenario[ (*it).first ][ (*it).second[j] ] = Bomberman::LUGAR_VACIO;

				tp_retorno.push( enviar );

				Premio price( Coordenada( (*it).first, (*it).second[j] ), *timer, PorcentajePremios );

				if( price.getTipo() != 'N' )
				    premios.push_back( price );

				enviar.id = price.getTipo();
				enviar.posicion = price.getId();
				enviar.x = (*it).first;
				enviar.y = (*it).second[j];

				tp_retorno.push( enviar );

			}

		}

	}

	/*
	 * Envio la orden para mostrar la explosion.
	 */

	enviar.id = 'a';
	enviar.posicion = id;
	enviar.x = startX;
	enviar.y = startY;

	tp_retorno.push( enviar );
	MExpancion[startX].push_back(startY);
}

bool Explosion :: pertenece( Coordenada coord ){
	bool retorno = false;
	int x = coord.get_x();
	int y = coord.get_y();
	unsigned i = 0;

	if ( MExpancion.find ( x ) != MExpancion.end() ) {

		while ( i < MExpancion[ x ].size()
				&& MExpancion[ x ][ i ] != y ) i++;

		if ( i < MExpancion[ x ].size() &&
		     MExpancion[ x ][ i ] == y ) retorno = true;

	}

    return retorno;
}

void Explosion :: setTiempoExpiracion( unsigned long int Tiempo ){
	this->TiempoExpiracion = Tiempo;
}

unsigned long int Explosion ::  getTiempoExpiracion(){
	return this->TiempoExpiracion;
}
