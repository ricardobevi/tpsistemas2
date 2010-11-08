/*
 * Bomberman.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Bomberman.h"

Bomberman :: Bomberman() {
}

Bomberman :: ~Bomberman(){

}

void Bomberman :: activar( long int puerto, string archivoEscenario ){
    ifstream archEsc( archivoEscenario.c_str() );

    this->Timer = 0;
    this->VidaInicial = 3;
    this->MaxBombInicial = 1;
    this->VelocidadInicial = 3;
    this->TiempoBomba = 50;
    this->TiempoExplosion = 5;
    this->TiempoEspera = 10;
    this->TiempoPartida = 200;

    this->PartidaIniciada = false;

    this->NumJugadores = 0;

    Jugadores.resize(4);

    MoverArriba = 'w';
    MoverAbajo = 's';
    MoverIzquierda = 'a';
    MoverDerecha = 'd';
    PonerBomba = 'b';

    this->Socket = Comm< char >(puerto);
    this->Socket.Listen();

    srand(unsigned(time(NULL)));

    for( unsigned i = 0 ; i < X_MAX + 1 ; i++ )
        for( unsigned j = 0 ; j < Y_MAX + 1 ; j++ )
            Escenario[i][j] = LUGAR_VACIO;

    /*Leo los datos del escenario*/{
        char tipoPared;
        unsigned x, y;

        ifstream archEsc( archivoEscenario.c_str() );

        archEsc >> tipoPared;

        while( ( ! archEsc.eof() ) && ( tipoPared != 'e' ) ){
            archEsc >> x;
            archEsc >> y;

            /*Descomentar para ver como carga el archivo*/
            /*cout << "id = " << ParedesDestruibles.size() << endl
                 << "tipoPared = " << tipoPared << endl
                 << "x = " << x << endl
                 << "y = " << y << endl;
			*/

            if ( Escenario[x][y] == LUGAR_VACIO ){

				if( tipoPared == 'd' ){

						Escenario[x][y] = ParedesDestruibles.size();
						ParedesDestruibles.push_back( Coordenada( x, y ) );

				} else {
					Paredes.push_back( Coordenada( x, y ) );
					Escenario[x][y] = PARED_FIJA;
				}

            }

            archEsc >> tipoPared;

        }

        archEsc.close();
    }

}

int Bomberman :: nuevoJugador(){
    Connection<char> playerCon;
    unsigned numCon = 0;
    unsigned numJugador = 0;

    numCon = this->Socket.Accept();

    playerCon = this->Socket.getConn( numCon );

    /*Saco numero de jugador*/{
        unsigned i = 0;

        while( i < Jugadores.size() &&
               i < JUGADORES_MAX &&
               ! Jugadores[i].eliminado() ) i = i + 1;

        if ( i >= JUGADORES_MAX )
            return -1;
        else
            numJugador = i;
    }

    Coordenada coord(0,0);

    switch(numJugador){
        case 0:
            coord.set_coordenada(0,0);
            break;

        case 1:
            coord.set_coordenada(X_MAX, 0);
            break;

        case 2:
            coord.set_coordenada(X_MAX, Y_MAX);
            break;

        case 3:
            coord.set_coordenada(0, Y_MAX);
            break;
    }

    Jugador Player( numJugador,
                    this->VidaInicial,
                    coord,
                    playerCon,
                    0,
                    MaxBombInicial,
                    1);

    if ( numJugador == Jugadores.size() ){

        this->Jugadores.push_back( Player );

    } else {

        this->Jugadores[numJugador] = Player;

    }

    this->sendEscenario( numJugador );

    this->NumJugadores++;

    return (int) numJugador;
}

void Bomberman :: sendEscenario( unsigned jugador ){
    unsigned i = 0;
    t_protocolo enviar;

    if( this->Jugadores[jugador].getEnvioEscenario() == false ){

        for( i = 0 ; i < Jugadores.size() ; i++ ){
            enviar.id = 'j';
            enviar.posicion = i;
            enviar.x = Jugadores[i].getPosicion().get_x();
            enviar.y = Jugadores[i].getPosicion().get_y();

            Jugadores[jugador].send(enviar);

            enviar.id = 'v';
			enviar.posicion = i;
			enviar.x = enviar.y = Jugadores[i].getVida();

			Jugadores[jugador].send(enviar);

        }

        for( i = 0 ; i < Paredes.size() ; i++ ){

            enviar.id = 'f';
            enviar.posicion = i;
            enviar.x = Paredes[i].get_x();
            enviar.y = Paredes[i].get_y();

            Jugadores[jugador].send(enviar);

        }

        for( i = 0 ; i < ParedesDestruibles.size() ; i++ ){

			enviar.id = 'd';
			enviar.posicion = i;
			enviar.x = ParedesDestruibles[i].get_x();
			enviar.y = ParedesDestruibles[i].get_y();

			Jugadores[jugador].send(enviar);

        }

        for( i = 0 ; i < Premios.size() ; i++ ){

			enviar.id = Premios[i].getTipo();
			enviar.posicion = Premios[i].getId();
			Premios[i].getPos().get_coordenada( enviar.x, enviar.y );

			Jugadores[jugador].send(enviar);

		}

        enviar.id = 'i';
        enviar.posicion = 0;
        enviar.x = jugador;
        enviar.y = 0;

        Jugadores[jugador].send(enviar);

        this->Jugadores[jugador].setEnvioEscenario(true);
    }
}

Jugador& Bomberman :: getJugador(int jugador){
    return this->Jugadores[ jugador ];
}

unsigned Bomberman :: getNumJugadores(){
    return this->NumJugadores;
}

queue<t_protocolo> Bomberman :: procesarAccion( t_protocolo recibido ){
	queue<t_protocolo> procesado;
    t_protocolo enviar;

    char accion = 0;

    unsigned jugador = recibido.posicion;

    int  x = this->Jugadores[ jugador ].getPosicion().get_x(),
         y = this->Jugadores[ jugador ].getPosicion().get_y();


        if ( recibido.x == MoverArriba ){

            if ( y > 0 &&
                 Escenario[x][y - 1] == LUGAR_VACIO ){

                this->Jugadores[ jugador ].moverArriba();
                accion = ACCION_MOVE;

            }

        }else if (recibido.x == MoverAbajo){

            if ( (unsigned) y < Y_MAX &&
                 Escenario[x][y + 1] == LUGAR_VACIO ){

                this->Jugadores[ jugador ].moverAbajo();
                accion = ACCION_MOVE;

            }

        }else if (recibido.x == MoverIzquierda){

            if (  x > 0 &&
                 Escenario[x - 1][y] == LUGAR_VACIO ){

                this->Jugadores[ jugador ].moverIzquierda();
                accion = ACCION_MOVE;

            }

        }else if (recibido.x == MoverDerecha){

            if ( (unsigned) x < X_MAX &&
                 Escenario[x + 1][y] == LUGAR_VACIO){

                this->Jugadores[ jugador ].moverDerecha();
                accion = ACCION_MOVE;

            }

        }else if (recibido.x == PonerBomba){
            accion = ACCION_BOMB;
        }


    if( accion == ACCION_MOVE ){
    	queue<t_protocolo> premios;

        enviar.id = 'j';
        enviar.posicion = jugador;
        enviar.x = this->Jugadores[ jugador ].getPosicion().get_x();
        enviar.y = this->Jugadores[ jugador ].getPosicion().get_y();

        procesado.push( enviar );

        premios = this->tomaPremio( Jugadores[ jugador ].getPosicion(), jugador );

        while ( ! premios.empty() ){
			procesado.push( premios.front() );
			premios.pop();
        }

    }else if( accion == ACCION_BOMB ){

        if( Jugadores[ jugador ].puedePonerBomba() ){
            unsigned bombid = (unsigned) HDTimer;
            Bomba bomb;

            bomb = Jugadores[ jugador ].ponerBomba( bombid );

            bomb.activar( HDTimer + TiempoBomba );

            /*
			 * Bomb has been planted..
			 */

            Bombas.push( bomb );

            enviar.id = 'b';
            enviar.posicion = bombid;
            bomb.getPos().get_coordenada(enviar.x, enviar.y);

            procesado.push( enviar );

        }


    }


    return procesado;

}

queue<t_protocolo> Bomberman :: explotarBomba(){
	queue<t_protocolo> qEnviar;
    t_protocolo enviar;

    if( Bombas.size() > 0 ) {

        Bomba bomb;

        bomb = Bombas.front();

        usleep( ( bomb.getTiempoExplosion() - HDTimer ) * 100000 );

        enviar.id = 'b';
        enviar.posicion = bomb.getNumero();
        enviar.x = enviar.y = -1;

        Jugadores[bomb.getOwner()].explotoBomba();

        qEnviar.push( enviar );

        Bombas.pop();

        Explosion bum(bomb,
        		      bomb.getNumero(),
        		      Escenario,
        		      X_MAX,
        		      Y_MAX );

        bum.calcularExplosion( qEnviar, this->ParedesDestruibles, this->Premios, &this->HDTimer );

        for( unsigned i = 0 ; i < Jugadores.size() ; i++ ){

			if( ! Jugadores[i].eliminado() ){

				if ( bum.pertenece( Jugadores[i].getPosicion() ) ){

					enviar.id = 'v';
					enviar.posicion = i;
					enviar.x = enviar.y = Jugadores[i].restarVida();

					qEnviar.push( enviar );

				}
			}

        }

        bum.setTiempoExpiracion( HDTimer + TiempoExplosion );

        /*
         * Terrorist win!
         */

        Explosiones.push( bum );

    }

    return qEnviar;
}

t_protocolo Bomberman :: expirarExplosion(){
	t_protocolo enviar = {0, 0, 0, 0};

	if( Explosiones.size() > 0 ) {

		usleep( ( Explosiones.front().getTiempoExpiracion() - HDTimer ) * 100000 );

		enviar.id = 'e';
		enviar.posicion = Explosiones.front().getId();
		enviar.x = enviar.y = -1;

		Explosiones.pop();

	}

	return enviar;

}

t_protocolo Bomberman :: recvFrom( unsigned jugador ){
	t_protocolo recibido = {0,0,0,0};
	int vel = Jugadores[jugador].getVelocidad();

	unsigned long int tiempoLectura;

	tiempoLectura = vel < VelocidadInicial ? (HDTimer + VelocidadInicial) - (vel) : HDTimer ;

	do{
		recibido = Jugadores[jugador].recv();
	}while( HDTimer < tiempoLectura && recibido.x != PonerBomba );

	return recibido;
}

int Bomberman :: update( t_protocolo data ) {

    for( unsigned i = 0 ; i < Jugadores.size() ; i++ )
        if( ! this->Jugadores[i].eliminado() )
            this->Jugadores[i].send( data );

    return 0;
}

t_protocolo Bomberman :: eliminarJugador( unsigned jugador ){
    t_protocolo enviar;

    if( this->Jugadores.size() - 1 >= jugador ){

        this->Jugadores[ jugador ].eliminar();

        this->NumJugadores--;

        enviar.id = 'j';
        enviar.posicion = jugador;
        enviar.x = -1;
        enviar.y = -1;

    } else {

        enviar.id = 0;
        enviar.posicion = jugador;
        enviar.x = -1;
        enviar.y = -1;

    }

    return enviar;
}

t_protocolo Bomberman :: clockTick(){
    t_protocolo enviar;

    for( int i = 0 ; i < 10 ; i++ ){
        usleep( 100000 );
        this->HDTimer++;
    }

    this->Timer++;

    enviar.id = 't';
    enviar.posicion = 0;
    enviar.x = this->TiempoPartida - this->Timer;
    enviar.y = 0;

    return enviar;
}

queue<t_protocolo> Bomberman :: tomaPremio( Coordenada coord, unsigned jugador ){
	unsigned i = 0;
	queue<t_protocolo> enviar;
	t_protocolo tp = {0,0,0,0};

	while ( i < Premios.size() && !( coord == Premios[i].getPos() ) ) i++;

	if ( i < Premios.size() && coord == Premios[i].getPos() ){

		Premio price( Premios[i] );

		Premios.erase( Premios.begin() + i );

		tp.id = price.getTipo();
		tp.posicion = price.getId();
		tp.x = tp.y = -1;

		enviar.push( tp );

		switch( price.getTipo() ){

			case 'B':
				Jugadores[jugador].sumarBombMax();
				break;

			case 'V':
				{
				int life = Jugadores[jugador].sumarVida();

				tp.id = 'v';
				tp.posicion = jugador;
				tp.x = tp.y = life;

				enviar.push( tp );

				break;
				}
			case 'E':
				Jugadores[jugador].sumarTipoBomba();
				break;

			case 'R':
				Jugadores[jugador].sumarVelocidad();
				break;

		}


	}

	return enviar;

}
