/***********************************************/
/* Nombre: semaforo.h                          */
/* Trabajo: TP6 Semaforos y Memoria Compartida */
/* Numero de ejercicio: 3                      */
/* Entrega: Primer Entrega                     */
/*                                             */
/* Grupo N 63                                  */
/* D'Aranno Facundo      34.842.320            */
/* Jose Ferreyra         31.144.004            */
/* Marcela A. Uslenghi   26.920.315            */
/* Bevilacqua Ricardo    34.304.983            */
/***********************************************/




/*
 * SEMAFORO.H
 * Contiene las primitivas para el manejo de semaforos.
 */


#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

/*
 * int P( int semId, unsigned short semNum )
 *
 * Decrementa el valor de un semaforo. Si el valor decrementado es menor
 * a 0, deja el semaforo en 0 y se bloquea hasta que este tome valor 1 y
 * pueda decrementarlo una unidad.
 *
 * Recibe:
 * int semId                          El id del vector de semaforos.
 * unsigned short semNum              El numero de semaforo a decrementar.
 * 
 * Devuelve:
 * Restorna un entero. 0 si no hubo error y -1 en caso contrario.
 */

int P( int semId, unsigned short semNum ){
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = -1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: P: semop");
        semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;
}

/*
 * int V( int semId, unsigned short semNum )
 *
 * Incrementa el valor de un semaforo.
 *
 * Recibe:
 * int semId                          El id del vector de semaforos.
 * unsigned short semNum              El numero de semaforo a decrementar.
 * 
 * Devuelve:
 * Restorna un entero. 0 si no hubo error y -1 en caso contrario.
 */

int V( int semId, unsigned short semNum ){
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = 1;
    opSem.sem_flg = 0;
    
    if ( semop(semId, &opSem, 1) == -1){
        perror("semaforo.h: V: semop");
        semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;
}

/*
 * int setSem (int semId, int semNum, int semVal)
 *
 * Setea el valor de un semaforo.
 *
 * Recibe:
 * int semId               El id del vector de semaforos.
 * int semNum              El numero de semaforo a decrementar.
 * int semVal              El valor al cual setear el semaforo.
 * 
 * Devuelve:
 * Restorna un entero. 0 si no hubo error y -1 en caso contrario.
 */

int setSem(int semId, int semNum, int semVal){
    arg.val = semVal;
    if ( semctl(semId, semNum, SETVAL, arg) == -1){
        perror("semaforo.h: setSem: semctl");
        semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;

}

/*
 * int rmSem (semId)
 *
 * Borra un vector de semaforos.
 *
 * Recibe:
 * int semId               El id del vector de semaforos.
 * 
 * Devuelve:
 * Restorna un entero. 0 si no hubo error y -1 en caso contrario.
 */

int rmSem(semId){
    return semctl(semId, 0, IPC_RMID);
}

#endif



/*******/
/* FIN */
/*******/