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

Bomberman servidor;

int main(){
    
    servidor.activar(50000);

    int numJugador;
    
    while(1){
        pthread_t newSender,
                  newRecver;

        numJugador = 0;
                  
        numJugador = servidor.nuevoJugador();

        pthread_create( &newSender, NULL, sender, (void *) (&Login) );
        pthread_create( &newRecver, NULL, recver, (void *) (&Login) );
        
    }
    
    return 0;
}

