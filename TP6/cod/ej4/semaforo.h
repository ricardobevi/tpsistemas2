/***********************************************/
/* Nombre: semaforo.h                          */
/* Trabajo: TP6 Semaforos y Memoria Compartida */
/* Numero de ejercicio: 4                      */
/* Entrega: Primer Entrega                     */
/*                                             */
/* Grupo N 63                                  */
/* D'Aranno Facundo      34.842.320            */
/* Jose Ferreyra         31.144.004            */
/* Marcela A. Uslenghi   26.920.315            */
/* Bevilacqua Ricardo    34.304.983            */
/***********************************************/



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


int obtenerSemaforo(key_t clave, int valor) {
   int IdSemaforo;
   union semun CtlSem;
   IdSemaforo = semget(clave, 1, IPC_CREAT | 0600);
   CtlSem.val = valor;
   semctl(IdSemaforo, 0, SETVAL, CtlSem);
   return IdSemaforo;
}

void pedirSemaforo(int IdSemaforo) {
   struct sembuf OpSem;
   OpSem.sem_num = 0;
   OpSem.sem_op = -1;   /* Decrementa semval o bloquea si cero */
   OpSem.sem_flg = 0;
   semop(IdSemaforo, &OpSem, 1);
}

void devolverSemaforo(int IdSemaforo) {
   struct sembuf OpSem;
   OpSem.sem_num = 0;
   OpSem.sem_op = 1;    /* Incrementa en 1 el semáforo */
   OpSem.sem_flg = 0;
   semop(IdSemaforo, &OpSem, 1);
}

void eliminarSemaforo(int IdSemaforo) {
   semctl(IdSemaforo, 0, IPC_RMID);
}

#endif



/*******/
/* FIN */
/*******/