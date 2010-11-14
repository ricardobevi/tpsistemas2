#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "t_protocolo.h"

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
         
        void crearSemaforo( key_t clave , int cantSem, int tipoDeAplicacion);
       
        
        int P( unsigned short semNum );
        int V( unsigned short semNum );
        int setSem(  int semNum, int semVal);
        int rmSem();
        
};



 Semaforo :: Semaforo( )
 {
    
 }
 
 
void Semaforo :: crearSemaforo( int clave , int cantSem , int tipoDeAplicacion)
{
    this -> cantSem = cantSem;
    
    if ( tipoDeAplicacion == SERVIDOR )
        semId = semget( clave, cantSem, IPC_CREAT | IPC_EXCL | 0660 );
    else // CLIENTE
        semId = semget( clave, cantSem,  0660 );
    
    if (semId == -1 )
    {
        perror("semaforo.h: Semaforo:");
        semctl(semId, 0, IPC_RMID);
    }
    
    
    if ( tipoDeAplicacion == SERVIDOR )
    {
        union semun argumento;
        
        argumento.val = 0;
        
        for (int i = 0 ; i < cantSem ; i ++ )
                semctl( semId, i, SETVAL , argumento);
    }
    

}

Semaforo ::~Semaforo( )
{
    semctl(semId, 0, IPC_RMID);
}


int Semaforo :: P( unsigned short semNum )
{
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = -1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: P: semop");
        //semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;
}

int Semaforo :: V( unsigned short semNum )
{
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = 1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: V: semop");
        //semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;
}


int Semaforo :: setSem( int  semNum, int semVal )
{

    union semun argumento;
    argumento.val = semVal;
      
    
    
    if ( semctl( semId , semNum, SETVAL, argumento) == -1  )
    {
        perror("semaforo if ( s.h: setSem: semctl");
        semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;

}


int Semaforo :: rmSem()
{
    return semctl(semId, 0, IPC_RMID);
}



#endif