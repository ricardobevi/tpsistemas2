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

void * recver( void * args );
void * procesador( void * args );
void * sender( void * args );
void * timer( void * args );

pthread_t procesadorThread,
          senderThread,
          timerThread;

queue<t_protocolo> QRecibido;
queue<t_protocolo> QEnviar;

pthread_mutex_t ProcesadorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  ProcesadorCond  = PTHREAD_COND_INITIALIZER;

pthread_mutex_t SenderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  SenderCond  = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ClockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClockStartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  ClockStartCond  = PTHREAD_COND_INITIALIZER;

pthread_mutex_t QRecibidoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t QEnviarMutex = PTHREAD_MUTEX_INITIALIZER;


int main(){
    
    Servidor.activar( 50003, "escenarios/e1.esc" );

    unsigned numJugador = 0;
    
    pthread_mutex_lock(&ProcesadorMutex);
    pthread_mutex_lock(&SenderMutex);
    pthread_mutex_unlock(&ClockMutex);
    pthread_mutex_lock(&ClockStartMutex);

    vector<pthread_t> recvJugadores;

    pthread_create( &procesadorThread, NULL, procesador, NULL );
    pthread_create( &senderThread, NULL, sender, NULL );
    pthread_create( &timerThread, NULL, timer, NULL );

    while( Servidor.getNumJugadores() < Bomberman::JUGADORES_MAX ){
        pthread_t newRecver;
        
        cout << "Esperando Jugador..." << endl;
        
        numJugador = Servidor.nuevoJugador();

        cout << "Conectado jugador " << numJugador << endl;

        pthread_create( &newRecver, NULL, recver, (void *) (&numJugador) );

        pthread_cond_broadcast(&ClockStartCond);

        recvJugadores.push_back( newRecver );
    }

    for( unsigned i = 0 ; i < recvJugadores.size() ; i++ )
        pthread_join(recvJugadores[i], NULL);
        
    
    return 0;
}

void * recver(void * args){
    unsigned * jugPtr = (unsigned *) args;
    unsigned jugador = *jugPtr;

    t_protocolo recibido;

    recibido = Servidor.getJugador( jugador ).recv();

    while( recibido.x > -1 ){

        recibido.posicion = jugador;

        pthread_mutex_lock(&QRecibidoMutex);
        QRecibido.push(recibido);
        pthread_mutex_unlock(&QRecibidoMutex);

        pthread_cond_broadcast(&ProcesadorCond);

        recibido = Servidor.getJugador( jugador ).recv();
    }

    recibido = Servidor.eliminarJugador( jugador );

    /*Envio la eliminacion del jugador*/{
        pthread_mutex_lock(&QEnviarMutex);
        QEnviar.push(recibido);
        pthread_mutex_unlock(&QEnviarMutex);

        pthread_cond_broadcast(&SenderCond);
        
    }

    cout << "Se desconecto el jugador " << jugador << endl;
    
    return NULL;

}

void * procesador(void * args){
    t_protocolo recibido,
                enviar;

    while(1){
        pthread_cond_wait(&ProcesadorCond, &ProcesadorMutex);
        bool empty = true;

        do{
            pthread_mutex_lock(&QRecibidoMutex);{
            
                if( ! QRecibido.empty() ){
                    recibido = QRecibido.front();
                    QRecibido.pop();
                }
            
            }pthread_mutex_unlock(&QRecibidoMutex);

            enviar = Servidor.procesarAccion( recibido );
            
            if( enviar.id != 0 ){
                pthread_mutex_lock(&QEnviarMutex);
                QEnviar.push(enviar);
                pthread_mutex_unlock(&QEnviarMutex);

                pthread_cond_broadcast(&SenderCond);
            }

            pthread_mutex_lock(&QEnviarMutex);
            empty = QRecibido.empty();
            pthread_mutex_unlock(&QEnviarMutex);
            
        }while( empty == false );
    }

    return NULL;
}

void * sender( void * args ){

    while(1){
        pthread_cond_wait(&SenderCond, &SenderMutex);
        bool empty = true;
        
        do{
            t_protocolo enviar;

            enviar.id = 0;
            enviar.posicion = 0;
            enviar.x = 0;
            enviar.y = 0;
            
            pthread_mutex_lock(&QEnviarMutex);{
                
                if ( ! QEnviar.empty() ){
                    enviar = QEnviar.front();
                    QEnviar.pop();
                }
                
            }pthread_mutex_unlock(&QEnviarMutex);
            
            Servidor.update( enviar );

            pthread_mutex_lock(&QEnviarMutex);
            empty = QEnviar.empty();
            pthread_mutex_unlock(&QEnviarMutex);

        }while( empty == false );
    }
    
    return NULL;
}

void * timer( void * args ){
    t_protocolo clock;

    pthread_cond_wait(&ClockStartCond, &ClockStartMutex);

    while(1){
        sleep(1);
        pthread_mutex_lock(&ClockMutex);
        clock = Servidor.clockTick();
        pthread_mutex_unlock(&ClockMutex);

        pthread_mutex_lock(&QEnviarMutex);
        QEnviar.push(clock);
        pthread_mutex_unlock(&QEnviarMutex);

        pthread_cond_broadcast(&SenderCond);
        
    }
    
    return NULL;
}
