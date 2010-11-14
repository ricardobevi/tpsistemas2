/*
 * Bomberman.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Bomberman.h"

Bomberman::Bomberman() {
    this->Socket = NULL;
}

Bomberman::~Bomberman() {
	map<int, Jugador *>::iterator it;

	delete this->Socket;
	delete [] Jugadores;

	for ( it = Espectadores.begin(); it != Espectadores.end() ; it++ )
		delete it->second;

	delete MemC;

}

void Bomberman::activar(string archivoConfiguracion) {

    this->ArchConf = archivoConfiguracion;

    cout << "Archivo de Configuracion: " << archivoConfiguracion << endl;

    ifstream archConf(archivoConfiguracion.c_str());
    int puerto;

    string parametro, archivoEscenario;

    archConf >> parametro;

    while ( !archConf.eof() ) {

        if ( parametro == "Puerto" ) {
            archConf >> puerto;
            cout << "Puerto = " << puerto << endl;

        } else if ( parametro == "Escenario" ) {
            archConf >> archivoEscenario;
            cout << "Escenario = " << archivoEscenario << endl;

        } else if ( parametro == "VidaInicial" ) {
            archConf >> this->VidaInicial;
            cout << "VidaInicial = " << VidaInicial << endl;

        } else if ( parametro == "MaxBombInicial" ) {
            archConf >> this->MaxBombInicial;
            cout << "MaxBombInicial = " << MaxBombInicial << endl;

        } else if ( parametro == "VelocidadInicial" ) {
            archConf >> this->VelocidadInicial;
            cout << "VelocidadInicial = " << VelocidadInicial << endl;

        } else if ( parametro == "VelocidadJugadorAutomatico" ) {
            archConf >> this->VelocidadJugadorAutomatico;
            cout << "VelocidadJugadorAutomatico = " << VelocidadJugadorAutomatico << endl;

        } else if ( parametro == "TiempoBomba" ) {
            archConf >> this->TiempoBomba;
            cout << "TiempoBomba = " << TiempoBomba << endl;

        } else if ( parametro == "TiempoExplosion" ) {
            archConf >> this->TiempoExplosion;
            cout << "TiempoExplosion = " << TiempoExplosion << endl;

        } else if ( parametro == "TiempoEspera" ) {
            archConf >> this->TiempoEspera;
            cout << "TiempoEspera = " << TiempoEspera << endl;

        } else if ( parametro == "TiempoPartida" ) {
            archConf >> this->TiempoPartida;
            cout << "TiempoPartida = " << TiempoPartida << endl;

        } else if ( parametro == "PorcentajePremios" ) {
            archConf >> this->PorcentajePremios;
            cout << "PorcentajePremios = " << PorcentajePremios << "%" << endl;

        } else if ( 1 ) {
            cerr << archivoConfiguracion << ": Error en archivo de configuracion." << endl;
            cerr << "Caracter " << archConf.tellg() << endl;
            exit(-1);
        }

        archConf >> parametro;

    }

    archConf.close();

    ifstream archEsc(archivoEscenario.c_str());

    this->PartidaIniciada = false;
    this->Timer = 0;
    this->NumJugadores = 0;

    for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ )
        this->TipoJugador[i] = JUGADOR_INACTIVO;

    MoverArriba = 'w';
    MoverAbajo = 's';
    MoverIzquierda = 'a';
    MoverDerecha = 'd';
    PonerBomba = 'b';

    /*
     * Si el servidor ya se inicializo, no tengo que asignarle un
     * nuevo objeto, uso la conexion ya existente.
     */
    if ( this->Socket == NULL ){
        this->Socket = new Comm<char> (puerto);
        this->Socket->Listen();
    }

    srand(unsigned(time(NULL)));

    for ( unsigned i = 0 ; i < X_MAX + 1 ; i++ )
        for ( unsigned j = 0 ; j < Y_MAX + 1 ; j++ )
            Escenario[i][j] = LUGAR_VACIO;

    /*Leo los datos del escenario*/{
        char tipoPared;
        unsigned x, y;

        ifstream archEsc(archivoEscenario.c_str());

        archEsc >> tipoPared;

        while ( (!archEsc.eof()) && (tipoPared != 'e') ) {
            archEsc >> x;
            archEsc >> y;

            /*Descomentar para ver como carga el archivo*/
            /*cout << "id = " << ParedesDestruibles.size() << endl
             << "tipoPared = " << tipoPared << endl
             << "x = " << x << endl
             << "y = " << y << endl;
             */

            if ( Escenario[x][y] == LUGAR_VACIO ) {

                if ( tipoPared == 'd' ) {

                    Escenario[x][y] = ParedesDestruibles.size();
                    ParedesDestruibles.push_back(Coordenada(x, y));

                } else {
                    Paredes.push_back(Coordenada(x, y));
                    Escenario[x][y] = PARED_FIJA;
                }

            }

            archEsc >> tipoPared;

        }

        archEsc.close();
    }

}

Jugador * Bomberman::esperarJugadorRemoto(){
	Connection<char> playerCon;
	unsigned numCon = 0;

	numCon = this->Socket->Accept();

	playerCon = this->Socket->getConn(numCon);

	Jugador * Player = new JugadorRemoto(-1,
										 this->VidaInicial,
										 Coordenada(-1,-1),
										 playerCon,
										 0,
										 MaxBombInicial,
										 1);

	return Player;

}

Jugador * Bomberman::esperarJugadorLocal(){
	MemC = new MemCompartida(SERVIDOR);

	MemC->esperarUsuario();

	Jugador * Player = new JugadorLocal(-1,
										 this->VidaInicial,
										 Coordenada(-1,-1),
										 MemC,
										 0,
										 MaxBombInicial,
										 1);

	return Player;

}

int Bomberman::nuevoJugador(Jugador * Player) {
    int numJugador = 0;

    /*Calculo el numero de jugador*/
    {

        if ( NumJugadores < JUGADORES_MAX && !PartidaIniciada ) {

            unsigned i = 0;

            while ( i < JUGADORES_MAX && TipoJugador[i] != JUGADOR_INACTIVO )
                i++;

            numJugador = i;

        } else {

            do {
                numJugador = (-1) * (JUGADORES_MAX + rand() % 100);
            } while ( Espectadores.find(numJugador) != Espectadores.end() );

        }

    }

    Player->setNumero(numJugador);

    Coordenada coord(0, 0);

    switch ( numJugador ) {
        case 0:
            coord.set_coordenada(0, 0);
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

        default:
            coord.set_coordenada(-1, -1);
    }

    t_protocolo enviaNumJugador;

    enviaNumJugador.id = 'i';
    enviaNumJugador.posicion = 0;
    enviaNumJugador.x = numJugador;
    enviaNumJugador.y = 0;

    Player->send(enviaNumJugador);
    Player->setPosicion(coord);

    if ( PartidaIniciada ) {
        t_protocolo simulacionIinicio = { 's', 0, 0, 0 };
        Player->send(simulacionIinicio);
    }

    if ( numJugador >= 0 ) {
        this->Jugadores[numJugador] = Player;
        this->TipoJugador[numJugador] = JUGADOR_REMOTO;
        this->NumJugadores++;

    } else {

        this->Espectadores[numJugador] = Player;

    }

    if ( PartidaIniciada )
        this->sendEscenario(numJugador);

    return numJugador;
}

queue<t_protocolo> Bomberman::sendEscenario(int jugador, bool toAll) {
    unsigned i = 0;
    queue<t_protocolo> qEnviar;
    map<int, Jugador *>::iterator it;
    t_protocolo enviar;

    for ( i = 0; i < JUGADORES_MAX ; i++ ) {

        if ( TipoJugador[i] != JUGADOR_INACTIVO ) {

            enviar.id = 'j';
            enviar.posicion = i;
            enviar.x = Jugadores[i]->getPosicion().get_x();
            enviar.y = Jugadores[i]->getPosicion().get_y();

            qEnviar.push(enviar);

            enviar.id = 'v';
            enviar.posicion = i;

            enviar.x = enviar.y = Jugadores[i]->getVida();

            qEnviar.push(enviar);
        }
    }

    for ( i = 0; i < Paredes.size() ; i++ ) {

        enviar.id = 'f';
        enviar.posicion = i;
        enviar.x = Paredes[i].get_x();
        enviar.y = Paredes[i].get_y();

        qEnviar.push(enviar);

    }

    for ( i = 0; i < ParedesDestruibles.size() ; i++ ) {

        enviar.id = 'd';
        enviar.posicion = i;
        enviar.x = ParedesDestruibles[i].get_x();
        enviar.y = ParedesDestruibles[i].get_y();

        qEnviar.push(enviar);

    }

    for ( i = 0; i < Premios.size() ; i++ ) {

        enviar.id = Premios[i].getTipo();
        enviar.posicion = Premios[i].getId();
        Premios[i].getPos().get_coordenada(enviar.x, enviar.y);

        qEnviar.push(enviar);

    }

    enviar.id = 'i';
    enviar.posicion = 0;
    enviar.x = jugador;
    enviar.y = 0;

    qEnviar.push(enviar);

    if ( !toAll ) {

        while ( !qEnviar.empty() ) {

            if ( jugador >= 0 )
                Jugadores[jugador]->send(qEnviar.front());
            else
                Espectadores[jugador]->send(qEnviar.front());

            qEnviar.pop();

        }

    } else {

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {
            if ( TipoJugador[i] != JUGADOR_INACTIVO ) {
                enviar.x = i;
                qEnviar.push(enviar);
            }
        }

        for ( it = Espectadores.begin(); it != Espectadores.end() ; it++ ) {
            enviar.x = it->first;
            qEnviar.push(enviar);
        }

    }

    return qEnviar;

}

Jugador * Bomberman::getJugador(int jugador) {
    return this->Jugadores[jugador];
}

unsigned Bomberman::getNumJugadores() {
    return this->NumJugadores;
}

unsigned long int Bomberman::getTiempoEspera() {
    return this->TiempoEspera;
}

queue<t_protocolo> Bomberman::iniciarPartida() {

    if ( NumJugadores == 1 ) {

        unsigned i = 0;

        while ( i < JUGADORES_MAX && TipoJugador[i] != JUGADOR_INACTIVO )
            i++;

        Jugadores[i] = new JugadorAutomatico(Escenario, i, this->VidaInicial, Coordenada(X_MAX, 0),
                0, MaxBombInicial, VelocidadJugadorAutomatico);

        TipoJugador[i] = JUGADOR_ARTIFICIAL;

        this->NumJugadores++;

    }

    this->PartidaIniciada = true;

    return this->sendEscenario(0, true);
}

queue<t_protocolo> Bomberman::finalizarPartida() {
    queue<t_protocolo> QFinPartida;
    t_protocolo finPartida;
    int empatados = 0;

    finPartida.id = 'P';
    finPartida.posicion = 0;

    for( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ){

        if( TipoJugador[i] != JUGADOR_INACTIVO ){
            finPartida.x = (int) i;
            finPartida.y = 1;

            QFinPartida.push(finPartida);

            empatados++;
        }

    }

    while ( !Posiciones.empty() ) {
        finPartida.x = Posiciones.front();
        finPartida.y = Posiciones.size() + empatados;

        QFinPartida.push(finPartida);

        Posiciones.pop();
    }

    finPartida.id = 'F';

    QFinPartida.push(finPartida);

    return QFinPartida;

}

queue<t_protocolo> Bomberman::procesarAccion(t_protocolo recibido) {
    queue<t_protocolo> procesado;
    t_protocolo enviar = { 0, 0, 0, 0 };

    if ( TipoJugador != JUGADOR_INACTIVO ) {

        char accion = 0;

        unsigned jugador = recibido.posicion;

        int x = this->Jugadores[jugador]->getPosicion().get_x(), y =
                this->Jugadores[jugador]->getPosicion().get_y();

        if ( recibido.x == MoverArriba ) {

            if ( y > 0 && Escenario[x][y - 1] == LUGAR_VACIO ) {

                this->Jugadores[jugador]->moverArriba();
                accion = ACCION_MOVE;

            }

        } else if ( recibido.x == MoverAbajo ) {

            if ( (unsigned) y < Y_MAX && Escenario[x][y + 1] == LUGAR_VACIO ) {

                this->Jugadores[jugador]->moverAbajo();
                accion = ACCION_MOVE;

            }

        } else if ( recibido.x == MoverIzquierda ) {

            if ( x > 0 && Escenario[x - 1][y] == LUGAR_VACIO ) {

                this->Jugadores[jugador]->moverIzquierda();
                accion = ACCION_MOVE;

            }

        } else if ( recibido.x == MoverDerecha ) {

            if ( (unsigned) x < X_MAX && Escenario[x + 1][y] == LUGAR_VACIO ) {

                this->Jugadores[jugador]->moverDerecha();
                accion = ACCION_MOVE;

            }

        } else if ( recibido.x == PonerBomba ) {
            accion = ACCION_BOMB;
        }

        if ( accion == ACCION_MOVE ) {
            queue<t_protocolo> premios;

            enviar.id = 'j';
            enviar.posicion = jugador;
            enviar.x = this->Jugadores[jugador]->getPosicion().get_x();
            enviar.y = this->Jugadores[jugador]->getPosicion().get_y();

            procesado.push(enviar);

            premios = this->tomaPremio(Jugadores[jugador]->getPosicion(), jugador);

            while ( !premios.empty() ) {
                procesado.push(premios.front());
                premios.pop();
            }

        } else if ( accion == ACCION_BOMB ) {

            if ( Jugadores[jugador]->puedePonerBomba() ) {
                unsigned bombid = (unsigned) HDTimer;
                Bomba bomb;

                bomb = Jugadores[jugador]->ponerBomba(bombid);

                bomb.activar(HDTimer + TiempoBomba);

                Escenario[bomb.getPos().get_x()][bomb.getPos().get_y()] = BOMBA;

                /*
                 * Bomb has been planted..
                 */

                Bombas.push(bomb);

                enviar.id = 'b';
                enviar.posicion = bombid;
                bomb.getPos().get_coordenada(enviar.x, enviar.y);

                procesado.push(enviar);

            }

        }

    }

    return procesado;

}

queue<t_protocolo> Bomberman::explotarBomba() {
    queue<t_protocolo> qEnviar;
    t_protocolo enviar;

    if ( Bombas.size() > 0 ) {

        Bomba bomb;

        bomb = Bombas.front();

        usleep((bomb.getTiempoExplosion() - HDTimer) * 100000);

        enviar.id = 'b';
        enviar.posicion = bomb.getNumero();
        enviar.x = enviar.y = -1;

        if ( TipoJugador[bomb.getOwner()] != JUGADOR_INACTIVO )
            Jugadores[bomb.getOwner()]->explotoBomba();

        qEnviar.push(enviar);

        Escenario[bomb.getPos().get_x()][bomb.getPos().get_y()] = LUGAR_VACIO;

        Bombas.pop();

        Explosion bum(bomb, bomb.getNumero(), Escenario, X_MAX, Y_MAX, PorcentajePremios);

        bum.calcularExplosion(qEnviar, this->ParedesDestruibles, this->Premios, &this->HDTimer);

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {
            int vidaNueva;

            if ( TipoJugador[i] != JUGADOR_INACTIVO && bum.pertenece(Jugadores[i]->getPosicion()) ) {

                vidaNueva = Jugadores[i]->restarVida();

                enviar.id = 'v';
                enviar.posicion = i;
                enviar.x = enviar.y = vidaNueva;

                qEnviar.push(enviar);

                if ( vidaNueva <= 0 ) {

                    qEnviar.push(this->eliminarJugador(i));

                }

            }

            bum.setTiempoExpiracion(HDTimer + TiempoExplosion);

            /*
             * Terrorist win!
             */

            Explosiones.push(bum);

        }

    }

    return qEnviar;
}

t_protocolo Bomberman::expirarExplosion() {
    t_protocolo enviar = { 0, 0, 0, 0 };

    if ( Explosiones.size() > 0 ) {

        usleep((Explosiones.front().getTiempoExpiracion() - HDTimer) * 100000);

        enviar.id = 'e';
        enviar.posicion = Explosiones.front().getId();
        enviar.x = enviar.y = -1;

        Explosiones.pop();

    }

    return enviar;

}

t_protocolo Bomberman::recvFrom(unsigned jugador) {
    t_protocolo recibido = { 0, 0, -1, 0 };
    int vel = Jugadores[jugador]->getVelocidad();
    //TODO: Ver los otros tipos de jugadores
    if ( this->PartidaIniciada ) {
        unsigned long int tiempoLectura;

        tiempoLectura = vel < VelocidadInicial ? (HDTimer + VelocidadInicial) - (vel) : HDTimer;

        do {

            recibido = Jugadores[jugador]->recv();

        } while ( HDTimer < tiempoLectura && recibido.x != PonerBomba );

    }

    return recibido;
}

int Bomberman::update(t_protocolo data) {
    map<int, Jugador *>::iterator it;

    /*
     * Si el dato es del tipo 'i', tengo que mandarlo solo al
     * jugador que llama, ya que si se lo envio a todos complica
     * el timeout para conexiones entrantes.(creo que puede sacarce)
     */

    if ( data.id != 'i' ) {

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {

            if ( TipoJugador[i] != JUGADOR_INACTIVO ) {

                if ( Jugadores[i]->isClosed() )
                    this->eliminarJugador(i);
                else {
                    this->Jugadores[i]->send(data);
                }

            }

        }

        for ( it = Espectadores.begin(); it != Espectadores.end() ; it++ ) {
            if ( it->second->isClosed() )
                this->eliminarEspectador(it->first);
            else
                it->second->send(data);
        }
    } else {

        int numJugador = data.x;

        if ( numJugador >= 0 && numJugador < (int) NumJugadores ) {
            if ( Jugadores[numJugador]->isClosed() )
                this->eliminarJugador(numJugador);
            else
                this->Jugadores[numJugador]->send(data);
        } else {

            Espectadores[numJugador]->send(data);
        }

    }

    return 0;
}

t_protocolo Bomberman::eliminarJugador(unsigned jugador, bool close) {
    t_protocolo enviar = {0,jugador,-1-1};

    if ( TipoJugador[jugador] != JUGADOR_INACTIVO ) {

        this->Jugadores[jugador]->eliminar();

        TipoJugador[jugador] = JUGADOR_INACTIVO;
        this->NumJugadores--;

        if ( !Jugadores[jugador]->isClosed() && close == false && TipoJugador[jugador]
                != JUGADOR_ARTIFICIAL ) {

            this->Jugadores[jugador]->setEspectador(true);
            Espectadores[this->Jugadores[jugador]->getNumero()] = this->Jugadores[jugador];
            cout << "El Jugador " << jugador << " ha pasado a modo espectador." << endl;

        } else {

            Jugadores[jugador]->Close();
            cout << "El Jugador " << jugador << " se desconecto." << endl;

        }

        if ( PartidaIniciada == true ){

			Posiciones.push(jugador);

			enviar.id = 'j';
			enviar.posicion = jugador;
			enviar.x = -1;
			enviar.y = -1;

        }

    }

    if ( NumJugadores == 1 && PartidaIniciada == true ) {
        unsigned i = 0;

        while ( TipoJugador[i] == JUGADOR_INACTIVO )
            i++;

        this->eliminarJugador(i);

    }

    return enviar;
}

void Bomberman::eliminarEspectador(int espectador) {
    Espectadores[espectador]->Close();
    Espectadores.erase(espectador);
}

t_protocolo Bomberman::clockTick() {
    t_protocolo enviar;

    for ( int i = 0 ; i < 10 ; i++ ) {
        usleep(100000);
        this->HDTimer++;
    }

    this->Timer++;

    enviar.id = 't';
    enviar.posicion = 0;
    enviar.x = PartidaIniciada ? TiempoPartida - Timer : TiempoEspera - Timer;
    enviar.y = 0;

    return enviar;
}

void Bomberman::resetClock() {
    HDTimer = 0;
    Timer = 0;
}

queue<t_protocolo> Bomberman::CloseClosed(){
	queue<t_protocolo> eliminados;
	t_protocolo retEliminado;

    for(unsigned i = 0 ; i < JUGADORES_MAX ; i++){
		if( TipoJugador[i] != JUGADOR_INACTIVO && Jugadores[i]->isClosed() ){
			retEliminado = this->eliminarJugador( i, true );

			if( retEliminado.id == 'j' )
				eliminados.push(retEliminado);
		}
    }

    return eliminados;
}

queue<t_protocolo> Bomberman::tomaPremio(Coordenada coord, unsigned jugador) {
    unsigned i = 0;
    queue<t_protocolo> enviar;
    t_protocolo tp = { 0, 0, 0, 0 };

    while ( i < Premios.size() && !(coord == Premios[i].getPos()) )
        i++;

    if ( i < Premios.size() && coord == Premios[i].getPos() ) {

        Premio price(Premios[i]);

        Premios.erase(Premios.begin() + i);

        tp.id = price.getTipo();
        tp.posicion = price.getId();
        tp.x = tp.y = -1;

        enviar.push(tp);

        switch ( price.getTipo() ) {

            case 'B':
                Jugadores[jugador]->sumarBombMax();
                break;

            case 'V': {
                int life = Jugadores[jugador]->sumarVida();

                tp.id = 'v';
                tp.posicion = jugador;
                tp.x = tp.y = life;

                enviar.push(tp);

                break;
            }
            case 'E':
                Jugadores[jugador]->sumarTipoBomba();
                break;

            case 'R':
                Jugadores[jugador]->sumarVelocidad();
                break;

        }

    }

    return enviar;

}

void Bomberman::Close() {
    Socket->CloseCons();
}

void Bomberman::CloseSocket() {
    Socket->Close();
}

void Bomberman::Reset() {
    map<int, Jugador *>::iterator it;

    this->PartidaIniciada = false;
    this->HDTimer = 0;
    this->Timer = 0;
    this->NumJugadores = 0;

    for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ){
        this->TipoJugador[i] = JUGADOR_INACTIVO;
    }

    Espectadores.clear();

    while( ! Bombas.empty() )
        Bombas.pop();

    while( ! Explosiones.empty() )
        Explosiones.pop();

    Premios.clear();

    while( ! Posiciones.empty() )
        Posiciones.pop();

}
