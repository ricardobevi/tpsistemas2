/*
 * servidor.cpp
 *
 * Descripcion:
 * Se encarga del manejo de la partida en si, sincroniza el uso de
 * la clase Bomberman.h.
 *
 * Crea un servidor maestro que administra las conexiones entrantes
 * y da inicio a las partidas. Cada partida es manejada por un
 * servidor esclavo, hijo del maestro. Asi se tiene un proceso por
 * partida en curso (esclavo) mas un proceso que administra y crea
 * las nuevas partidas (maestro).
 *
 */

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

/* Espera nuevos jugadores remotos */
void * nuevosJugadoresRemotos(void * args);

/* Espera un nuevo jugador local */
void * nuevoJugadorLocal(void * args);

/* Espera un proceso particular */
void * pidWaiter(void * args);

/* Recibe datos de un jugador */
void * recver(void * args);

/* Procesa los datos recibidos */
void * procesador(void * args);

/* Envia los datos procesados */
void * sender(void * args);

/* Controla el tiempo de vida de las bombas */
void * bomb(void * args);

/* Controla el tiempo de vida de explosiones */
void * explosion(void * args);

/* Temporizador de la partida */
void * timer(void * args);

/* Temopoirzador de inicio de partida */
void * timeOut(void * args);

/* Encola en QRecibido */
void pushQRecibido(t_protocolo recibido);

/* Encola en QEnviar */
void pushQEnviar(t_protocolo enviar);

/* Desencola de QRecibido */
t_protocolo popQRecibido();

/* Desencola de QEnviar */
t_protocolo popQEnviar();

/* Termina una partida particular */
void end();

/* Activa todos los threads */
void activateThreads();

/* Cancela todos los threads */
void cancelThreads();

/*
 * Nota: si bien la formalidad indica que los comentarios
 * en declaraciones de variables deben ser brebes y no
 * superar una o dos lineas, me parecio interesante extender
 * la longitud para asi poder dar un brebe resumen del
 * funcionamiento del programa en estas declaraciones.
 *
 * Disfruten ;)
 */

/*
 * Definicion de los pthread_t para los
 * threads estaticos, o sea, los que no
 * se usan para recibir datos de jugadores.
 */
pthread_t procesadorThread,
          senderThread,
          bombThread,
          explosionThread,
          timerThread,
          timeOutThread,
          nuevosJugadoresRemotosThread,
          nuevoJugadorLocalThread;

/*
 * Cola de datos recibidos. Cada thread de recepcion (recver)
 * recibe las teclas precionadas por el usuario y las encola
 * en QRecibido.
 */
queue<t_protocolo> QRecibido;

/*
 * Cola de datos a enviar. El thread de envio de datos desencola
 * de a un elemento de esta cola y lo envia a todos los jugadores
 * excepto que id = 'i', en donde se lo enviara al numero de jugador
 * que figure en x.
 */
queue<t_protocolo> QEnviar;

/*
 * Cola de numero de jugadores. Cada vez que un jugador se conecta
 * se encola el numero en esta cola. Ese numero se usa lugo para
 * crear los threads de recepcion.
 */
queue<int> QNumJugadores;

/*
 * pthread_t de threads de recepcion. Cuando se crea un nuevo thread
 * de recepcion se agrega a esta estructura. En el primer dato se guarda
 * el numero de jugador y en el segundo el thread que recibe datos de
 * ese jugador.
 */
map< int, pthread_t > recvJugadores;

/*
 * pthread_t de threads de espera de partidas. Cuando se crea una nueva
 * partida, tambien se crea un nuevo proceso, este proceso debe ser tratado
 * y esperado por el padre. En el primer dato se guarda el PID del servidor
 * hijo de la partida y en el segundo el pthread_t del thread que espera a
 * dicho hijo (partida).
 */
map< int, pthread_t > pidWaiters;

/* Almacenamiento temporal del pid del hijo */
pid_t childPid;

/*
 * PID de la partida que tiene un jugador local. Como no se admite mas de
 * un jugador local a nivel global, se almacena la partida en la que juega
 * el jugador local para que cuando esta termine se habilite la recepcion
 * de un nuevo jugador local.
 */
pid_t jugadorLocalPid;

/*
 * Indica si hay un jugador local en alguna partida. VER COMENTARIO ANTERIOR.
 */
bool hayJugadorLocal = false;

/*
 * Aca inician los semaforos y condiciones para sincronizar los threads.
 * Cada semaforo y condicional cumple una funcion especifica dentro de la
 * sincronizacion de los threads.
 */

/*
 * Bloqueo de jugador local. Este bloqueo se usa para que solo haya un
 * jugador local globalmente, cuando se recibe un nuevo jugador local
 * el thread nuevoJugadorLocal() se bloquea bajo esta condicion hasta
 * que no termine la partida que contiene al jugador local.
 */
pthread_mutex_t JugLocalMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t JugLocalCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de la creacion de threads. Mas adelante se explica el porque
 * de este bloqueo. VER: lugar de creacion de threads de recepcion.
 */
pthread_mutex_t CrearNuevoThreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CrearNuevoThreadCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de procesamiento. Bloquea el thread de procesamiento hasta
 * que tenga algo que procesar en la cola QRecibido.
 */
pthread_mutex_t ProcesadorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ProcesadorCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo del thread de envio. Bloquea al thread de envio
 * de datos hasta que tenga algo que enviar en QEnviar.
 */
pthread_mutex_t SenderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t SenderCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de bombas. Bloquea el thread de tiempo de vida de
 * bombas hasta que tenga bombas que procesar.
 */
pthread_mutex_t BombMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BombCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de explosiones. Bloquea el thread de tiempo de vida
 * de explosiones hasta que alguna bomba haya explotado y tenga
 * explosiones que procesar.
 */
pthread_mutex_t ExplosionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ExplosionCond = PTHREAD_COND_INITIALIZER;

/*
 * Sincronizacion de clock. ClockStart* controla el inicio del
 * reloj de la partida. ClockMutex controla que solo un thread
 * modifique el clock al mismo tiempo. (Nota: esto no es necesario
 * para este caso, pero se considera una buena practica ya que el
 * clock es un recurso critico)
 */
pthread_mutex_t ClockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClockStartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ClockStartCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de inicio de time out o tiempo de inico de partida.
 * Se usa para mantener bloqueado al thread que controla el inicio
 * de la partida hasta que se conecte el primer jugador.
 */
pthread_mutex_t TimeOutMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeOutStartCond = PTHREAD_COND_INITIALIZER;

/*
 * Bloqueo de fin de inicio de partida (no me volvi loco, es asi).
 * Se utiliza para indicar que el inicio de partida finalizo. Esto
 * desencadena los sucesos que crean una nueva partida con los
 * jugadores conectados hasta el momento.
 */
pthread_mutex_t TimeOutEndMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeOutEndCond = PTHREAD_COND_INITIALIZER;

/*
 * Semaforos para la sincronizacion de colas QRecibido y
 * QEnviado. Para que cualquier operacion sobre las colas
 * sea atomica, se bloquea el semaforo cada vez que se quiere
 * modificar o leer datos de las colas.
 */
pthread_mutex_t QRecibidoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t QEnviarMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Semaforo para la sincronizacion de nuevos jugadores. Como hay
 * 2 threads esperando jugadores (local y remoto) se debe asegurar
 * que solo uno a la vez pueda agregar un nuevo jugador, de lo
 * contrario, el resultado es inesperado.
 */
pthread_mutex_t NuevoJugadorMutex = PTHREAD_MUTEX_INITIALIZER;



/* void cSIGPIPE(int iNumSen, siginfo_t *info, void *ni)
 *
 * Descripcion:
 * Esta funcion maneja la señal SIGPIPE. Esta señal indica que un
 * pipe se ha roto, y en este caso nos dice que se perdio una
 * conexion. Esta funcion busca el/los jugadores que se hayan
 * desconectado a travez del metodo CloseClosed de la clase
 * Bomberman.
 *
 * Recibe:
 * int iNumSen
 * siginfo_t *info
 * void *ni
 *
 * Nota: no se explican estos parametros porque no se usan en
 * este programa.
 *
 * Retorna:
 * void.
 *
 */
void cSIGPIPE(int iNumSen, siginfo_t *info, void *ni){
    cerr << "Pipe Roto." << endl;

    queue<t_protocolo> enviar;

    cout << "Un jugador se ha desconectado inesperadamente!" << endl;
    cout << "Eliminando jugadores cerrados.......";

    /*
     * Duerme un segundo para asegurarse de que se le intente mandar el
     * tiempo a todos los juegadores y asi poder conocer cual es el
     * causante de esta señal, o sea, cual se desconecto.
     *
     * Nota: Hay formas mas elegantes de hacer esto, pero esta funciona
     * de lo mas bien =)
     */
    sleep(1);


    enviar = Servidor.CloseClosed();

    while( !enviar.empty() ){
    	pushQEnviar(enviar.front());
    	enviar.pop();
    }

    cout << "[OK]" << endl;
}


/* void cTERMPadre(int iNumSen, siginfo_t *info, void *ni)
 *
 * Descripcion:
 * Esta funcion maneja la señal SIGTERM en el servidor maestro.
 * Se encarga de terminar todos los esclavos de forma correcta,
 * cerrar todos los threads, cerrar las areas de memoria compartida,
 * eliminar los semaforos y cerrar el socket de escucha.
 *
 * Recibe:
 * int iNumSen
 * siginfo_t *info
 * void *ni
 *
 * Nota: no se explican estos parametros porque no se usan en
 * este programa.
 *
 * Retorna:
 * void.
 *
 */
void cTERMPadre(int iNumSen, siginfo_t *info, void *ni){
	map<int, pthread_t>::iterator it;

	cout << "Terminando procesos...." << endl;
	for ( it = pidWaiters.begin() ; it != pidWaiters.end() ; it++ ){
		kill(SIGTERM, it->first);
		cout << "Terminando proceso...." << "Pid: " << it->first;

		/*
		 * Como cada proceso tiene asignado un thread que espera su terminacion,
		 * tiene sentido esperar al thread que espera al proceso en lugar de al
		 * proceso en si.
		 */
		pthread_join( it->second, NULL );

		cout << "[TERMINADO]" << endl;
	}

    cout << "Terminando Servidor..." << endl;

    cancelThreads();

    cout << "Cerrando thread de recepcion de jugadores locales.......";
	pthread_cancel(nuevoJugadorLocalThread);
	cout << "[OK]" << endl;

	cout << "Cerrando thread de recepcion de jugadores remotos.......";
	pthread_cancel(nuevosJugadoresRemotosThread);
	cout << "[OK]" << endl;


    cout << "Cerrando Socket de Escucha...................";
    Servidor.CloseSocket();
	cout << "[OK]" << endl;

	cout << "Cerrando Memoria Compartida..................";
	Servidor.CloseMemC();
	cout << "[OK]" << endl;

    exit(0);
}

/* void cTERMHijo(int iNumSen, siginfo_t *info, void *ni)
 *
 * Descripcion:
 * Esta funcion maneja la señal SIGTERM en el servidor esclavo.
 * Cierra todos los threads activos y cierra las conexiones puntuales
 * con los clientes que estuvieran conectados a esa partida.
 *
 * Recibe:
 * int iNumSen
 * siginfo_t *info
 * void *ni
 *
 * Nota: no se explican estos parametros porque no se usan en
 * este programa.
 *
 * Retorna:
 * void.
 *
 */
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

			pthread_mutex_lock(&CrearNuevoThreadMutex);

			while( !QNumJugadores.empty() ){
				pthread_t newRecver;
				int numJugador = QNumJugadores.front();
				int retorno;

				retorno = pthread_create(&newRecver, NULL, recver, (void *) (&numJugador));

				/*
				 * Tengo que esperar de lo contrario el valor de la variable
				 * numJugador puede cambiar antes de que internamente el thread
				 * guarde de que jugador tiene que recibir datos.
				 */

				pthread_mutex_lock(&CrearNuevoThreadMutex);

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

	if ( hayJugadorLocal == true && chPid == jugadorLocalPid ){

		cout << "Cerrando Memoria Compartida...";
		Servidor.CloseMemC();
		cout << "[Ok]" << endl;

		hayJugadorLocal = false;
		jugadorLocalPid = 0;
		pthread_cond_broadcast(&JugLocalCond);
	}

	pidWaiters.erase(chPid);

	cout << "Termino Partida (Pid: " << chPid << ")" << endl;

	return NULL;
}

void * recver(void * args) {
    int * jugPtr = (int *) args;
    int jugador = *jugPtr;

    pthread_mutex_unlock(&CrearNuevoThreadMutex);
    //pthread_cond_broadcast(&CrearNuevoThreadCond);

    cout << "* Creado thread de jugador " << jugador << endl;

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
        cout << "Inicia Timer" << endl;
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

    if(hayJugadorLocal == true)
        	sleep(10);

    cout << "Cerrando Sockets...";
    Servidor.Close();
    cout << "[OK]" << endl;

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

}
