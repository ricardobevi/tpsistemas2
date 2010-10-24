#include <iostream>
#include <fstream>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
# include <stdio.h>
# include <stdlib.h>

#include "Bomberman.h"

Bomberman Servidor;

void * sender(void * args);
void * recver(void * args);

int main(){
    
    Servidor.activar(50002);

    unsigned numJugador;
    
    while(1){
        pthread_t newSender,
                  newRecver;

        numJugador = 0;

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

    t_protocolo enviar;

    enviar.id = 'i';
    

    return NULL;
}

void * recver(void * args){
    unsigned * jugPtr = (unsigned *) args;
    unsigned jugador = *jugPtr;

    t_protocolo recibido;

    while(1){

        recibido = Servidor.recvFrom( jugador );

        cout << "Jugador " << jugador << endl;
        cout << "id = " << recibido.id << endl
             << "posicion = " << recibido.posicion << endl
             << "x = " << recibido.x << endl
             << "y = " << recibido.y << endl;

    }
    return NULL;

}