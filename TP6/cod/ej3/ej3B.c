/***********************************************/
/* Nombre: ej3B.c                              */
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
    
    mem = shmget(clave, TAM * sizeof(int), 0660);
    
    nums = (int*) shmat(mem, NULL, 0);

    if ( ( semaforo = semget(clave, 2, 0660 ) ) == -1 ){
        perror("main: semget");
        exit(1);
    }
    
    while(1){
        float promedio = 0;
        
        P(semaforo, 1);
        
        numsAux[0] = nums[0];
        numsAux[1] = nums[1];
        numsAux[2] = nums[2];
 
        V(semaforo, 0);
        
        promedio = (numsAux[0] + numsAux[1] + numsAux[2]) / 3.0;
        
        printf("Promedio: ");
        
        if( promedio < 16 )
            printf("\033[01;34m");
        else if( promedio < 23 )
            printf("\033[01;33m");
        else if( promedio >= 23 )
            printf("\033[22;31m");
        
        
        printf("%0.2f\033[00;37m", promedio);
        
        printf("\n");
        
        
    }

    shmdt(nums);
    
    return 0;
}


/*******/
/* FIN */
/*******/