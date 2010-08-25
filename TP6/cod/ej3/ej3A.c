/***********************************************/
/* Nombre: ej3A.c                              */
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



#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "semaforo.h"

#define TAM 3

int main(){
    key_t clave;
    int mem,
        *nums,
        numsAux[3],
        semaforo,
        i = 0;
    
    clave = ftok(".",'F');
    
    mem = shmget(clave, TAM * sizeof(int), IPC_CREAT | 0660);
    
    nums = (int*) shmat(mem, NULL, 0);

    if ( ( semaforo = semget(clave, 2, IPC_CREAT | IPC_EXCL | 0660 ) ) == -1 ){
        perror("main: semget");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
    
    setSem(semaforo, 0, 1);
    setSem(semaforo, 1, 0);
    
    printf("Ingrese 3 numeros enteros.\n");
    for( i = 0 ; i < 3 ; i++){
        printf("-> ");
        scanf("%d", &numsAux[i]);
    }
    
    while(numsAux[0] != 99 && numsAux[1] != 99 && numsAux[2] != 99){
        
        if( P(semaforo, 0) )
            exit(1);
        
        printf("Escribiendo en memoria...\n\n");
        
        nums[0] = numsAux[0];
        nums[1] = numsAux[1];
        nums[2] = numsAux[2];
        
        if( V(semaforo, 1) )
            exit(1);
        
        printf("Ingrese 3 numeros enteros.\n");
        for( i = 0 ; i < 3 ; i++){
            printf("-> ");
            scanf("%d", &numsAux[i]);
        }
    }
    
    shmdt(nums);
    
    shmctl(mem, IPC_RMID, 0);
    
    rmSem(semaforo);
    
    return 0;
}


/*******/
/* FIN */
/*******/