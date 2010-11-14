/*
 * Semaforo.h
 *
 *  Created on: 14/11/2010
 *      Author: facu
 */

#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "t_protocolo.h"
#include  "constantes.h"

using namespace std;

union semun{
int val;
struct semid_ds *buf;
unsigned short *array;
};

class Semaforo
{
    private:

        int semId;
        int cantSem;

    public:
        Semaforo( );
        ~Semaforo( );

        int crearSemaforo( key_t clave , int cantSem);
        int mapearSemaforo( key_t clave , int cantSem);

        int P( unsigned short semNum );
        int V( unsigned short semNum );
        int setSem(  int semNum, int semVal);
        int rmSem();

};

#endif /* SEMAFORO_H_ */
