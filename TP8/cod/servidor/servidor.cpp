#include <iostream>
#include <fstream>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "Bomberman.h"

Bomberman Servidor;

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

pthread_t procesadorThread, senderThread, bombThread, explosionThread, timerThread, timeOutThread;

queue<t_protocolo> QRecibido;
queue<t_protocolo> QEnviar;
queue<int> QNumJugadores;

vector<pthread_t> recvJugadores;

/*
pthread_mutex_t RecibirMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t RecibirCond = PTHREAD_COND_INITIALIZER;
*/

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

pthread_mutex_t QRecibidoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t QEnviarMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {

    Servidor.activar(50003, "escenarios/e2.esc");
    
    int numJugador = 0;


    pthread_create(&procesadorThread, NULL, procesador, NULL );
    pthread_create(&senderThread, NULL, sender, NULL );
    pthread_create(&bombThread, NULL, bomb, NULL );
    pthread_create(&explosionThread, NULL, explosion, NULL );
    pthread_create(&timerThread, NULL, timer, NULL );
    pthread_create(&timeOutThread, NULL, timeOut, NULL );

    while ( 1 ) {
        
        cout << "Esperando Jugador..." << endl;
        
        numJugador = Servidor.nuevoJugador();

        if ( numJugador >= 0 ) {

            cout << "Conectado jugador " << numJugador << endl;

            QNumJugadores.push( numJugador );

            pthread_cond_broadcast(&TimeOutStartCond);

        } else {
            cout << "Conectado espectador " << numJugador << endl;
        }

    }

    pthread_join(timeOutThread, NULL);

    return 0;
}

void * recver(void * args) {
    unsigned * jugPtr = (unsigned *) args;
    unsigned jugador = *jugPtr;

    t_protocolo recibido;

    recibido = Servidor.recvFrom(jugador);

    while ( recibido.x > -1 ) {

        if ( recibido.id != 0 ) {

            recibido.posicion = jugador;

            pushQRecibido(recibido);

        }

        recibido = Servidor.recvFrom(jugador);

    }

    recibido = Servidor.eliminarJugador(jugador, true);

    pushQEnviar(recibido);

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

    while ( 1 ) {
        pthread_cond_wait(&SenderCond, &SenderMutex);
        bool empty = true;
        
        do {
            t_protocolo enviar;

            enviar = popQEnviar();
            
            Servidor.update(enviar);

            pthread_mutex_lock(&QEnviarMutex);
            empty = QEnviar.empty();
            pthread_mutex_unlock(&QEnviarMutex);

        } while ( empty == false );
    }
    
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

    pthread_cond_wait(&ClockStartCond, &ClockStartMutex);

    while ( 1 ) {
        pthread_mutex_lock(&ClockMutex);
        clock = Servidor.clockTick();
        pthread_mutex_unlock(&ClockMutex);

        pushQEnviar(clock);
        
    }
    
    return NULL;
}

void * timeOut(void * args) {
    t_protocolo clock;
    queue<t_protocolo> escenarioAEnviar;

    pthread_cond_wait(&TimeOutStartCond, &TimeOutMutex);

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

    pthread_mutex_lock(&ClockMutex);
    Servidor.resetClock();
    pthread_mutex_unlock(&ClockMutex);

    escenarioAEnviar = Servidor.iniciarPartida();

    while ( !escenarioAEnviar.empty() ) {
        pushQEnviar(escenarioAEnviar.front());
        escenarioAEnviar.pop();
    }

    while( !QNumJugadores.empty() ){
        pthread_t newRecver;
        int numJugador = QNumJugadores.front();

        pthread_create(&newRecver, NULL, recver, (void *) (&numJugador));
        cout << "thread de recepcion para " << numJugador << endl;
        recvJugadores.push_back(newRecver);

        QNumJugadores.pop();
    }

    pthread_cond_broadcast(&ClockStartCond);

    for ( unsigned i = 0 ; i < recvJugadores.size() ; i++ )
            pthread_join(recvJugadores[i], NULL);

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
