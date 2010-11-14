#ifndef SEMAFORO_H
#define SEMAFORO_H

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


// constructor de clase que no realiza accion alguna
 Semaforo :: Semaforo( )
 {
    
 }
 
 
// destructor del semaforo
Semaforo ::~Semaforo( )
{
     this -> rmSem();
}
 


// servidor: crea el vector de  cantSem semaforos y los inicializa en cero
int Semaforo :: crearSemaforo( int clave , int cantSem )
{
    this -> cantSem = cantSem;
    
    semId = semget( clave, cantSem, IPC_CREAT | 0660 );

    
    if (semId == -1 )
    {
        perror("semaforo.h:  crearSemaforo( int clave , int cantSem ):");
        return -1;
    }
    
    return 1;

}

//  metodo para cliente: mapea los semaforos en memoria para poder utilizarlos
int Semaforo :: mapearSemaforo( int clave , int cantSem)
{
    this -> cantSem = cantSem;
    
    semId = semget( clave, cantSem,  IPC_CREAT | 0660 );
    
    if (semId == -1 )
    {
        perror("semaforo.h: mapearSemaforo( int clave , int cantSem):");
        return -1;
    }
    
    
    return 1;

}


//metodo bloqueante que solicita el numero de semaforo indicado por parametro
int Semaforo :: P( unsigned short semNum )
{
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = -1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: P: semop");
        this -> rmSem();
        return -1;
    }
    
    return 0;
}


//metodo que libera el numero de semaforo indicado por parametro
int Semaforo :: V( unsigned short semNum )
{
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = 1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: V: semop");
        this -> rmSem();
        return -1;
    }
    
    return 0;
}


// metodo utilizado para setear el valor de un semaforo manualmente ( legacy - actualmente no utilizado )
int Semaforo :: setSem( int  semNum, int semVal )
{

    union semun argumento;
    argumento.val = semVal;
      
    
    
    if ( semctl( semId , semNum, SETVAL, argumento) == -1  )
    {
        perror("semaforo if ( s.h: setSem: semctl");
        this -> rmSem();
        return -1;
    }
    
    return 0;

}


// metodo que elimina el semaforo
int Semaforo :: rmSem()
{
    return semctl( semId ,  0 , IPC_RMID);
}



#endif