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

void * sender(void * args);
void * recver(void * args);

queue<t_protocolo> QRecibido;

pthread_mutex_t RecibidoMutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
    
    Servidor.activar(50002);

    unsigned numJugador;
    
    while(1){
        pthread_t newSender,
                  newRecver;

        numJugador = 0;

        pthread_mutex_lock(&RecibidoMutex);

        cout << "Esperando Jugador..." << endl;
        
        numJugador = Servidor.nuevoJugador();

        cout << "Conectado jugador " << numJugador << endl;

        pthread_create( &newSender, NULL, sender, (void *) (&numJugador) );
        pthread_create( &newRecver, NULL, recver, (void *) (&numJugador) );

        pthread_join(newSender, NULL);
        pthread_join(newRecver, NULL);
        
    }
    
    return 0;
}

void * sender(void * args){
    unsigned * jugPtr = (unsigned *) args;
    unsigned jugador = *jugPtr;
    
    t_protocolo recibido,
                enviar;
    
    while(1){
        pthread_mutex_lock(&RecibidoMutex);
        
        do{
            recibido = QRecibido.front();
            QRecibido.pop();
            
            switch( recibido.x ){
                case 'w':
                    Servidor.getJugador( jugador ).moverArriba();
                    break;
                    
                case 's':
                    Servidor.getJugador( jugador ).moverAbajo();
                    break;

                case 'd':
                    Servidor.getJugador( jugador ).moverDerecha();
                    break;

                case 'a':
                    Servidor.getJugador( jugador ).moverIzquierda();
                    break;

            }

            enviar.id = 'j';
            enviar.posicion = jugador;
            enviar.x = Servidor.getJugador( jugador ).getPosicion().get_x();
            enviar.y = Servidor.getJugador( jugador ).getPosicion().get_y();

            Servidor.getJugador( jugador ).send( enviar );

        }while( QRecibido.empty() == false );
    }

    return NULL;
}

void * recver(void * args){
    unsigned * jugPtr = (unsigned *) args;
    unsigned jugador = *jugPtr;

    t_protocolo recibido;

    while(1){

        recibido = Servidor.getJugador( jugador ).recv();

        QRecibido.push(recibido);

        if( pthread_mutex_trylock(&RecibidoMutex) != 0 )
            pthread_mutex_unlock(&RecibidoMutex);
        
    }
    return NULL;

}