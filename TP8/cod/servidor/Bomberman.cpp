/*
 * Bomberman.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Bomberman.h"

Bomberman::Bomberman() {
}

Bomberman::~Bomberman() {

}

void Bomberman::activar(long int puerto, string archivoEscenario) {
    ifstream archEsc(archivoEscenario.c_str());

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

    for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ )
        this->JugadoresActivos[i] = false;

    MoverArriba = 'w';
    MoverAbajo = 's';
    MoverIzquierda = 'a';
    MoverDerecha = 'd';
    PonerBomba = 'b';

    this->Socket = Comm<char> (puerto);
    this->Socket.Listen();

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

int Bomberman::nuevoJugador() {
    Connection<char> playerCon;
    unsigned numCon = 0;
    int numJugador = 0;

    numCon = this->Socket.Accept();

    playerCon = this->Socket.getConn(numCon);

    /*Saco numero de jugador*/
    {

        if ( NumJugadores < JUGADORES_MAX && !PartidaIniciada ) {

            unsigned i = 0;

            while ( i < JUGADORES_MAX && JugadoresActivos[i] )
                i++;

            numJugador = i;

        } else {

            do {
                numJugador = (-1) * (JUGADORES_MAX + rand() % 100);
            } while ( Espectadores.find(numJugador) != Espectadores.end() );

        }

    }

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

    Jugador Player(numJugador, this->VidaInicial, coord, playerCon, 0, MaxBombInicial, 1);

    t_protocolo enviaNumJugador;

    enviaNumJugador.id = 'i';
    enviaNumJugador.posicion = 0;
    enviaNumJugador.x = numJugador;
    enviaNumJugador.y = 0;

    Player.send(enviaNumJugador);

    if ( PartidaIniciada ){
        t_protocolo simulacionIinicio = { 's', 0, 0, 0 };
        Player.send(simulacionIinicio);
    }

    if ( numJugador >= 0 ) {

        this->Jugadores[numJugador] = Player;
        this->JugadoresActivos[numJugador] = true;
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
    map<int, Jugador>::iterator it;
    t_protocolo enviar;


    for ( i = 0; i < JUGADORES_MAX ; i++ ) {
        if ( JugadoresActivos[i] ) {
            enviar.id = 'j';
            enviar.posicion = i;
            enviar.x = Jugadores[i].getPosicion().get_x();
            enviar.y = Jugadores[i].getPosicion().get_y();

            qEnviar.push(enviar);

            enviar.id = 'v';
            enviar.posicion = i;
            enviar.x = enviar.y = Jugadores[i].getVida();

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
                Jugadores[jugador].send(qEnviar.front());
            else
                Espectadores[jugador].send(qEnviar.front());

            qEnviar.pop();

        }

    } else {

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {
            if ( JugadoresActivos[i] == true ) {
                enviar.x = i;
                qEnviar.push(enviar);
            }
        }

        for ( it = Espectadores.begin() ; it != Espectadores.end() ; it++ ) {
            enviar.x = it->first;
            qEnviar.push(enviar);
        }

    }

    return qEnviar;

}

Jugador& Bomberman::getJugador(int jugador) {
    return this->Jugadores[jugador];
}

unsigned Bomberman::getNumJugadores() {
    return this->NumJugadores;
}

unsigned long int Bomberman::getTiempoEspera() {
    return this->TiempoEspera;
}

queue<t_protocolo> Bomberman::iniciarPartida() {

    this->PartidaIniciada = true;

    return this->sendEscenario(0, true);;
}

queue<t_protocolo> Bomberman::procesarAccion(t_protocolo recibido) {
    queue<t_protocolo> procesado;
    t_protocolo enviar = { 0, 0, 0, 0 };

    char accion = 0;

    unsigned jugador = recibido.posicion;

    int x = this->Jugadores[jugador].getPosicion().get_x(), y =
            this->Jugadores[jugador].getPosicion().get_y();

    if ( recibido.x == MoverArriba ) {

        if ( y > 0 && Escenario[x][y - 1] == LUGAR_VACIO ) {

            this->Jugadores[jugador].moverArriba();
            accion = ACCION_MOVE;

        }

    } else if ( recibido.x == MoverAbajo ) {

        if ( (unsigned) y < Y_MAX && Escenario[x][y + 1] == LUGAR_VACIO ) {

            this->Jugadores[jugador].moverAbajo();
            accion = ACCION_MOVE;

        }

    } else if ( recibido.x == MoverIzquierda ) {

        if ( x > 0 && Escenario[x - 1][y] == LUGAR_VACIO ) {

            this->Jugadores[jugador].moverIzquierda();
            accion = ACCION_MOVE;

        }

    } else if ( recibido.x == MoverDerecha ) {

        if ( (unsigned) x < X_MAX && Escenario[x + 1][y] == LUGAR_VACIO ) {

            this->Jugadores[jugador].moverDerecha();
            accion = ACCION_MOVE;

        }

    } else if ( recibido.x == PonerBomba ) {
        accion = ACCION_BOMB;
    }

    if ( accion == ACCION_MOVE ) {
        queue<t_protocolo> premios;

        enviar.id = 'j';
        enviar.posicion = jugador;
        enviar.x = this->Jugadores[jugador].getPosicion().get_x();
        enviar.y = this->Jugadores[jugador].getPosicion().get_y();

        procesado.push(enviar);

        premios = this->tomaPremio(Jugadores[jugador].getPosicion(), jugador);

        while ( !premios.empty() ) {
            procesado.push(premios.front());
            premios.pop();
        }

    } else if ( accion == ACCION_BOMB ) {

        if ( Jugadores[jugador].puedePonerBomba() ) {
            unsigned bombid = (unsigned) HDTimer;
            Bomba bomb;

            bomb = Jugadores[jugador].ponerBomba(bombid);

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

        Jugadores[bomb.getOwner()].explotoBomba();

        qEnviar.push(enviar);

        Escenario[bomb.getPos().get_x()][bomb.getPos().get_y()] = LUGAR_VACIO;

        Bombas.pop();

        Explosion bum(bomb, bomb.getNumero(), Escenario, X_MAX, Y_MAX);

        bum.calcularExplosion(qEnviar, this->ParedesDestruibles, this->Premios, &this->HDTimer);

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {

            if ( JugadoresActivos[i] ) {

                if ( bum.pertenece(Jugadores[i].getPosicion()) ) {

                    enviar.id = 'v';
                    enviar.posicion = i;
                    enviar.x = enviar.y = Jugadores[i].restarVida();

                    qEnviar.push(enviar);

                }
            }

        }

        bum.setTiempoExpiracion(HDTimer + TiempoExplosion);

        /*
         * Terrorist win!
         */

        Explosiones.push(bum);

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
    t_protocolo recibido = { 0, 0, 0, 0 };
    int vel = Jugadores[jugador].getVelocidad();

    if ( this->PartidaIniciada ) {
        unsigned long int tiempoLectura;

        tiempoLectura = vel < VelocidadInicial ? (HDTimer + VelocidadInicial) - (vel) : HDTimer;

        do {
            recibido = Jugadores[jugador].recv();
        } while ( HDTimer < tiempoLectura && recibido.x != PonerBomba );

    }

    return recibido;
}

int Bomberman::update(t_protocolo data) {
    map<int, Jugador>::iterator it;

    /*
     * Si el dato es del tipo 'i', tengo que mandarlo solo al
     * jugador que llama, ya que si se lo envio a todos complica
     * el timeout para conexiones entrantes.
     */

    if ( data.id != 'i' ) {

        for ( unsigned i = 0 ; i < JUGADORES_MAX ; i++ ) {

            if ( this->JugadoresActivos[i] ) {
                if ( Jugadores[i].isClosed() )
                    this->eliminarJugador(i);
                else {
                    this->Jugadores[i].send(data);
                }
            }

        }

        for ( it = Espectadores.begin(); it != Espectadores.end() ; it++ ){
            if ( it->second.isClosed() )
                this->eliminarEspectador(it->first);
            else
                it->second.send(data);
        }
    } else {

        int numJugador = data.x;

        if ( numJugador <= (int) JUGADORES_MAX && this->JugadoresActivos[numJugador] ) {
            if ( Jugadores[numJugador].isClosed() )
                this->eliminarJugador(numJugador);
            else
                this->Jugadores[numJugador].send(data);
        } else {

            Espectadores[numJugador].send(data);
        }

    }

    return 0;
}

t_protocolo Bomberman::eliminarJugador(unsigned jugador, bool close) {
    t_protocolo enviar;

    if ( JugadoresActivos[jugador] ) {

        this->Jugadores[jugador].eliminar();

        this->NumJugadores--;
        JugadoresActivos[jugador] = false;

        if ( !Jugadores[jugador].isClosed() && close == false ) {
            Espectadores[this->Jugadores[jugador].getNumero()] = this->Jugadores[jugador];
            cout << "El Jugador " << jugador << " ha pasado a modo espectador." << endl;
        } else {
            Jugadores[jugador].Close();
            cout << "El Jugador " << jugador << " se desconecto." << endl;
        }

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

void Bomberman::eliminarEspectador(int espectador){
    Espectadores[espectador].Close();
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
                Jugadores[jugador].sumarBombMax();
                break;

            case 'V': {
                int life = Jugadores[jugador].sumarVida();

                tp.id = 'v';
                tp.posicion = jugador;
                tp.x = tp.y = life;

                enviar.push(tp);

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
