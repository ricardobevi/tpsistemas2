#include <iostream>
#include <fstream>
#include <pthread.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <queue>

#include "Bomberman.h"

Bomberman Servidor;
bool timeOutActivo = false;

void * nuevosJugadoresRemotos(void * args);
void * nuevoJugadorLocal(void * args);
void * pidWaiter(void * args);
void * recver(void * args);
void * procesador(void * args);
void * sender(void * args);
void * bomb(void * args);
void * explosion(void * args);
void * timer(void * args);
void * timeOut(void * args);

void pushQRecibido(t_protocolo recibido);
void pushQEnviar(t_protocolo enviar);
t_protocolo popQRecibido();
t_protocolo popQEnviar();
void end();
void activateThreads();
void cancelThreads();

pthread_t procesadorThread,
          senderThread,
          bombThread,
          explosionThread,
          timerThread,
          timeOutThread,
          nuevosJugadoresRemotosThread,
          nuevoJugadorLocalThread;

queue<t_protocolo> QRecibido;
queue<t_protocolo> QEnviar;

queue<int> QNumJugadores;

map< int, pthread_t > recvJugadores;
map< int, pthread_t > pidWaiters;

pid_t childPid;
pid_t jugadorLocalPid;

bool hayJugadorLocal = false;

pthread_mutex_t JugLocalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t JugLocalCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ProcesadorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ProcesadorCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t SenderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t SenderCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t BombMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BombCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ExplosionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ExplosionCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ClockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClockStartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ClockStartCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t TimeOutMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeOutStartCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t TimeOutEndMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeOutEndCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t QRecibidoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t QEnviarMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t NuevoJugadorMutex = PTHREAD_MUTEX_INITIALIZER;

void cSIGPIPE(int iNumSen, siginfo_t *info, void *ni){
    cerr << "Pipe Roto." << endl;

    queue<t_protocolo> enviar;

    cout << "Un jugador se ha desconectado inesperadamente!" << endl;
    cout << "Eliminando jugadores cerrados.......";
    sleep(1);
    enviar = Servidor.CloseClosed();

    while( !enviar.empty() ){
    	pushQEnviar(enviar.front());
    	enviar.pop();
    }

    cout << "[OK]" << endl;
}

void cTERMPadre(int iNumSen, siginfo_t *info, void *ni){
	map<int, pthread_t>::iterator it;

	cout << "Terminando procesos...." << endl;
	for ( it = pidWaiters.begin() ; it != pidWaiters.end() ; it++ ){
		kill(SIGTERM, it->first);
		pthread_join( it->second, NULL );
		cout << "Terminando proceso...." << "Pid: " << it->first << endl;
	}

    cout << "Terminando Servidor..." << endl;

    cancelThreads();

    cout << "Cerrando Socket de Escucha...................";
    Servidor.CloseSocket();
	cout << "[OK]" << endl;

	cout << "Cerrando Memoria Compartida..................";
	Servidor.CloseMemC();
	cout << "[OK]" << endl;

    exit(0);
}

void cTERMHijo(int iNumSen, siginfo_t *info, void *ni){
	end();
	exit(-1);
}

int main(int argc, const char *argv[]) {

    struct sigaction term;

    term.sa_sigaction = cSIGPIPE;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGPIPE, &term, NULL);

    term.sa_sigaction = cTERMPadre;
    sigaction(SIGINT, &term, NULL);
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGQUIT, &term, NULL);
    sigaction(SIGABRT, &term, NULL);
    sigaction(SIGSEGV, &term, NULL);

    if ( argc > 1 )
        Servidor.activar(argv[1]);
    else{
        Servidor.activar("bomberman.conf");
    }

    cout << "Creando thread de time out..................";
	pthread_create(&timeOutThread, NULL, timeOut, NULL );
	cout << "[OK]" << endl;

    activateThreads();

	pthread_create(&nuevosJugadoresRemotosThread, NULL, nuevosJugadoresRemotos, NULL );
	pthread_create(&nuevoJugadorLocalThread, NULL, nuevoJugadorLocal, NULL );

    while(1){

    	pthread_cond_wait(&TimeOutEndCond, &TimeOutEndMutex);

		cout << "Creando nueva partida..." << endl;

		childPid = fork();

		if ( childPid == 0 ){/*hijo*/

			term.sa_sigaction = cTERMHijo;
			sigaction(SIGINT, &term, NULL);
			sigaction(SIGTERM, &term, NULL);
			sigaction(SIGQUIT, &term, NULL);
			sigaction(SIGABRT, &term, NULL);
			sigaction(SIGSEGV, &term, NULL);

			cout << "[OK] (Pid: " << getpid() << ")" << endl;

			activateThreads();

			queue<t_protocolo> escenarioAEnviar;

			pthread_mutex_lock(&ClockMutex);
			Servidor.resetClock();
			pthread_mutex_unlock(&ClockMutex);

			if ( Servidor.getNumJugadores() == 1 )
					QNumJugadores.push(1);

			escenarioAEnviar = Servidor.iniciarPartida();

			while ( !escenarioAEnviar.empty() ) {
				pushQEnviar(escenarioAEnviar.front());
				escenarioAEnviar.pop();
			}

			while( !QNumJugadores.empty() ){
				pthread_t newRecver;
				int numJugador = QNumJugadores.front();
				int retorno;

				retorno = pthread_create(&newRecver, NULL, recver, (void *) (&numJugador));

				cout << retorno << " Creado thread de jugador " << numJugador << endl;

				recvJugadores[numJugador] = newRecver;

				QNumJugadores.pop();
			}

			pthread_cond_broadcast(&ClockStartCond);

			pthread_join( senderThread, NULL );

		    end();

			exit(0);

		}else{

			if ( hayJugadorLocal == true )
				jugadorLocalPid = childPid;

			Servidor.Reset();

			pthread_create(&pidWaiters[childPid], NULL, pidWaiter, (void *) (&childPid));

			while( !QNumJugadores.empty() )
				QNumJugadores.pop();

			cout << "Creando thread de time out..................";
			pthread_create(&timeOutThread, NULL, timeOut, NULL );
			cout << "[OK]" << endl;


		}

    }

    cancelThreads();

    return 0;
}

void * nuevosJugadoresRemotos(void * args){
	int numJugador = 0;

	while ( 1 ) {

		cout << "Esperando Jugador Remoto..." << endl;

		Jugador * player = Servidor.esperarJugadorRemoto();

		pthread_mutex_lock(&NuevoJugadorMutex);
		numJugador = Servidor.nuevoJugador( player );
		pthread_mutex_unlock(&NuevoJugadorMutex);

		if ( numJugador >= 0 ) {

			cout << "Conectado jugador remoto " << numJugador << endl;

			QNumJugadores.push( numJugador );

			pthread_cond_broadcast(&TimeOutStartCond);

		} else {
			cout << "Conectado espectador remoto " << numJugador << endl;
		}


	}

	return NULL;
}

void * nuevoJugadorLocal(void * args){
	int numJugador = 0;

	while ( 1 ) {

		cout << "Esperando Jugador Local..." << endl;

		Jugador * player = Servidor.esperarJugadorLocal();

		pthread_mutex_lock(&NuevoJugadorMutex);
		numJugador = Servidor.nuevoJugador( player );
		pthread_mutex_unlock(&NuevoJugadorMutex);

		if ( numJugador >= 0 ) {

			cout << "Conectado jugador local " << numJugador << endl;

			QNumJugadores.push( numJugador );

			pthread_cond_broadcast(&TimeOutStartCond);

		} else {
			cout << "Conectado espectador local " << numJugador << endl;
		}

		hayJugadorLocal = true;

		pthread_cond_wait(&JugLocalCond, &JugLocalMutex);

	}

	return NULL;
}

void * pidWaiter( void * args ){
	pid_t * chPidPtr = (pid_t *) args;
	pid_t chPid = *chPidPtr;

	waitpid( chPid, NULL, 0 );

	cout << "Termino Partida (Pid: " << chPid << ")" << endl;

	if ( hayJugadorLocal == true && chPid == jugadorLocalPid ){
		Servidor.CloseMemC();
		hayJugadorLocal = false;
		jugadorLocalPid = 0;
		pthread_cond_broadcast(&JugLocalCond);
	}

	pidWaiters.erase(pthread_self());

	return NULL;
}

void * recver(void * args) {
    int * jugPtr = (int *) args;
    int jugador = *jugPtr;

    t_protocolo recibido;

    recibido = Servidor.recvFrom(jugador);

    while ( recibido.x > -1 && Servidor.getNumJugadores() > 1 ) {

        if ( recibido.id != 0 ) {

            recibido.posicion = jugador;

            pushQRecibido(recibido);

        }

        recibido = Servidor.recvFrom(jugador);
    }

    if ( recibido.x == -1 )
        recibido = Servidor.eliminarJugador(jugador, true);
    else
        recibido = Servidor.eliminarJugador(jugador, false);

    pushQEnviar(recibido);

    recvJugadores.erase(jugador);

    cout << "Se cierra thread del jugador " << jugador << endl;

    return NULL;

}

void * procesador(void * args) {
    t_protocolo recibido;
    queue<t_protocolo> enviar;

    while ( 1 ) {
        pthread_cond_wait(&ProcesadorCond, &ProcesadorMutex);
        bool empty = true;

        do {

            recibido = popQRecibido();

            enviar = Servidor.procesarAccion(recibido);

            while ( !enviar.empty() ) {

                if ( enviar.front().id == 'b' )
                    pthread_cond_broadcast(&BombCond);
                
                if ( enviar.front().id != 0 )
                    pushQEnviar(enviar.front());

                enviar.pop();
            }

            pthread_mutex_lock(&QEnviarMutex);
            empty = QRecibido.empty();
            pthread_mutex_unlock(&QEnviarMutex);
            
        } while ( empty == false );
    }

    return NULL;
}

void * sender(void * args) {
    t_protocolo enviar;

    do {

        pthread_cond_wait(&SenderCond, &SenderMutex);
        bool empty = true;
        

        do {

            enviar = popQEnviar();
            
            Servidor.update(enviar);

            if ( enviar.id == 'F' ){
                end();
                return NULL;
            }

            pthread_mutex_lock(&QEnviarMutex);
            empty = QEnviar.empty();
            pthread_mutex_unlock(&QEnviarMutex);

        } while ( empty == false && enviar.id != 'F' );



    } while ( enviar.id != 'F' );

    return NULL;
}

void * bomb(void * args) {

    while ( 1 ) {
        pthread_cond_wait(&BombCond, &BombMutex);

        queue<t_protocolo> enviar;

        enviar = Servidor.explotarBomba();
        
        while ( !enviar.empty() ) {

            pthread_cond_broadcast(&ExplosionCond);

            while ( !enviar.empty() ) {
                pushQEnviar(enviar.front());
                enviar.pop();
            }

            enviar = Servidor.explotarBomba();

        }
    }
    
    return NULL;
}

void * explosion(void * args) {

    while ( 1 ) {
        t_protocolo enviar = { 0, 0, 0, 0 };

        pthread_cond_wait(&ExplosionCond, &ExplosionMutex);

        enviar = Servidor.expirarExplosion();

        while ( enviar.id != 0 ) {

            pushQEnviar(enviar);

            enviar = Servidor.expirarExplosion();

        }

    }

    return NULL;

}

void * timer(void * args) {
    t_protocolo clock;
    bool reset = false;

    while ( 1 ){

        reset = false;
        pthread_cond_wait(&ClockStartCond, &ClockStartMutex);

        pthread_mutex_lock(&ClockMutex);
        clock = Servidor.clockTick();
        pthread_mutex_unlock(&ClockMutex);

        while ( clock.x >= 0 && (! reset) ) {

            if ( clock.x == 0 || Servidor.getNumJugadores() == 0 ){

                queue<t_protocolo> QFinDePartida;

                QFinDePartida = Servidor.finalizarPartida();

                while( !QFinDePartida.empty() ){
                    pushQEnviar( QFinDePartida.front() );
                    QFinDePartida.pop();
                }

                reset = true;

            }

            pushQEnviar(clock);

            pthread_mutex_lock(&ClockMutex);
            clock = Servidor.clockTick();
            pthread_mutex_unlock(&ClockMutex);

       }
        
    }
    
    return NULL;
}

void * timeOut(void * args) {
    t_protocolo clock;

    pthread_cond_wait(&TimeOutStartCond, &TimeOutMutex);

    cout << "Inicia thread de TimeOut." << endl;

    pthread_mutex_lock(&ClockMutex);
    clock = Servidor.clockTick();
    pthread_mutex_unlock(&ClockMutex);

    while ( clock.x >= 0 ) {

        pushQEnviar(clock);

        pthread_mutex_lock(&ClockMutex);
        clock = Servidor.clockTick();
        pthread_mutex_unlock(&ClockMutex);

    }

    t_protocolo condInicio = { 's', 0, 0, 0 };

    pushQEnviar(condInicio);

    pthread_cond_broadcast(&TimeOutEndCond);

    cout << "Termina thread de TimeOut." << endl;

    return NULL;
}

void pushQRecibido(t_protocolo recibido) {
    pthread_mutex_lock(&QRecibidoMutex);
    QRecibido.push(recibido);
    pthread_mutex_unlock(&QRecibidoMutex);

    pthread_cond_broadcast(&ProcesadorCond);
}

void pushQEnviar(t_protocolo enviar) {
    pthread_mutex_lock(&QEnviarMutex);
    QEnviar.push(enviar);
    pthread_mutex_unlock(&QEnviarMutex);

    pthread_cond_broadcast(&SenderCond);
}

t_protocolo popQRecibido() {
    t_protocolo recibido;

    pthread_mutex_lock(&QRecibidoMutex);
    {

        if ( !QRecibido.empty() ) {
            recibido = QRecibido.front();
            QRecibido.pop();
        }

    }
    pthread_mutex_unlock(&QRecibidoMutex);

    return recibido;
}

t_protocolo popQEnviar() {
    t_protocolo enviar;
    
    enviar.id = 0;
    enviar.posicion = 0;
    enviar.x = 0;
    enviar.y = 0;

    pthread_mutex_lock(&QEnviarMutex);
    {

        if ( !QEnviar.empty() ) {
            enviar = QEnviar.front();
            QEnviar.pop();
        }

    }
    pthread_mutex_unlock(&QEnviarMutex);

    return enviar;
}

void end(){
	cout << "Terminando Partida...Pid: " << getpid() << endl;

    cancelThreads();

    cout << "Cerrando Sockets..." << endl;
    Servidor.Close();
    cout << "[OK]" << endl;

    cout << "Cerrando Memoria Compartida..." << endl;
    Servidor.CloseMemC();
    cout << "[Ok]" << endl;

}

void activateThreads(){

	cout << "Creando thread de envio de datos............";
	pthread_create(&senderThread, NULL, sender, NULL );
	cout << "[OK]" << endl;

	cout << "Creando thread de timer.....................";
	pthread_create(&timerThread, NULL, timer, NULL );
	cout << "[OK]" << endl;

    cout << "Creando thread de procesamiento.............";
    pthread_create(&procesadorThread, NULL, procesador, NULL );
    cout << "[OK]" << endl;

    cout << "Creando thread de bombas....................";
    pthread_create(&bombThread, NULL, bomb, NULL );
    cout << "[OK]" << endl;

    cout << "Creando thread de explosiones...............";
    pthread_create(&explosionThread, NULL, explosion, NULL );
    cout << "[OK]" << endl;

}

void cancelThreads(){
    cout << "Cerrando thread de procesamiento........";
    pthread_cancel(procesadorThread);
    cout << "[OK]" << endl;

    cout << "Cerrando thread de bombas...............";
    pthread_cancel(bombThread);
    cout << "[OK]" << endl;

    cout << "Cerrando thread de explosiones..........";
    pthread_cancel(explosionThread);
    cout << "[OK]" << endl;

    cout << "Cerrando thread de timer................";
    pthread_cancel(timerThread);
    cout << "[OK]" << endl;

    cout << "Cerrando thread de time out.............";
    pthread_cancel(timeOutThread);
    cout << "[OK]" << endl;

    map<int, pthread_t>::iterator it;

    for ( it = recvJugadores.begin() ; it != recvJugadores.end() ; it++ ){
		pthread_cancel( it->second );
	}

    cout << "Cerrando thread de envio de datos.......";
    pthread_cancel(senderThread);
    cout << "[OK]" << endl;
}
