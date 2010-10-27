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

//#define JUGADORES_MAX 4

Bomberman Servidor;

void * recver( void * args );
void * procesador( void * args );
void * sender( void * args );

queue<t_protocolo> QRecibido;
queue<t_protocolo> QEnviar;

pthread_mutex_t ProcesadorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SenderMutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
    
    Servidor.activar( 50002, "escenarios/e1.esc" );

    unsigned numJugador = 0;
    
    pthread_mutex_lock(&ProcesadorMutex);
    pthread_mutex_lock(&SenderMutex);

    vector<pthread_t> recvJugadores;
    pthread_t procesadorThread,
              senderThread;

    pthread_create( &procesadorThread, NULL, procesador, NULL );
    pthread_create( &senderThread, NULL, sender, NULL );

    while( Servidor.getNumJugadores() < Bomberman::JUGADORES_MAX ){
        pthread_t newRecver;
        
        cout << "Esperando Jugador..." << endl;
        
        numJugador = Servidor.nuevoJugador();

        cout << "Conectado jugador " << numJugador << endl;

        pthread_create( &newRecver, NULL, recver, (void *) (&numJugador) );

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

        QRecibido.push(recibido);

        if( pthread_mutex_trylock(&ProcesadorMutex) != 0 )
            pthread_mutex_unlock(&ProcesadorMutex);

        recibido = Servidor.getJugador( jugador ).recv();
    }

    recibido = Servidor.eliminarJugador( jugador );

    /*Envio la eliminacion del jugador*/{
        QEnviar.push(recibido);

        if( pthread_mutex_trylock(&SenderMutex) != 0 )
                pthread_mutex_unlock(&SenderMutex);
    }

    cout << "Se desconecto el jugador " << jugador << endl;
    
    return NULL;

}

void * procesador(void * args){
    t_protocolo recibido,
                enviar;

    while(1){
        pthread_mutex_lock(&ProcesadorMutex);

        do{
            recibido = QRecibido.front();
            QRecibido.pop();

            enviar = Servidor.procesarAccion( recibido );

            QEnviar.push(enviar);

            if( pthread_mutex_trylock(&SenderMutex) != 0 )
                pthread_mutex_unlock(&SenderMutex);

        }while( QRecibido.empty() == false );
    }

    return NULL;
}

void * sender( void * args ){
   t_protocolo enviar;

    while(1){
        pthread_mutex_lock(&SenderMutex);

        do{
            enviar = QEnviar.front();
            QEnviar.pop();

            Servidor.update( enviar );

        }while( QEnviar.empty() == false );
    }
    
    return NULL;
}
