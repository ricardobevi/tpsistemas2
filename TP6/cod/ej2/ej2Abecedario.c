/***********************************************/
/* Nombre: ej2Abecedario.c                     */
/* Trabajo: TP6 Semaforos y Memoria Compartida */
/* Numero de ejercicio: 2                      */
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
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>


#include "semaforo.h"

int main(){
    char abc[26] = {'a','b','c','d',
                    'e','f','g','h',
                    'i','j','k','l',
                    'm','n','o','p',
                    'q','r','s','t',
                    'u','v','w','x',
                    'y','z'};
                    
    
    pid_t childPids[3];
    int i = 0,
        letra = 0,
        semaforo = 0;
        
    key_t clave;
    
    clave = ftok(".", 'R');
    
    /*
     * Creo los semaforos que voy a utilizar, en este caso, al tener que lograr
     * una alternancia estricta, utilizo 2.
     */

    if ( ( semaforo = semget(clave, 2, IPC_CREAT | IPC_EXCL | 0660 ) ) == -1 ){
        perror("main: semget");
        if ( errno == EEXIST )
            printf("El semaforo ya existe, posiblemente porque el programa ya se este ejecutando.\n");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
    
    /*
     * Seteo un semaforo con valor 0 y otro con valor 1, asegurandome asi
     * que empiece por un proceso determinado.
     */
    
    if ( setSem(semaforo, 0, 1) ) 
        exit(1);
    
    if ( setSem(semaforo, 1, 0) ) 
        exit(1);
    
    /*
     * Creo mis dos hijos. Con este while puedo crear mis hijos a partir del
     * padre y no me quedan mas generaciones (no quedan nietos, etc.). Luego
     * analizando el valor con que queda la variable "i" puedo definir a cada
     * uno de los hijos y al padre.
     */
    
    while( i < 2 && ( childPids[i] = fork() ) != 0 ) i++;
    
    if(i == 2){/* Padre */
        
        /*
         * El padre espera por sus dos hijos, remueve el semaforo y termina.
         */
        
        wait(NULL);
        wait(NULL);
        
        rmSem(semaforo);
        
    }else if ( childPids[ i ] == 0 ){/* Hijos */
        
        /*
         * Aca entran ambos hijos y defino que el primer hijo imprima las
         * las letras "pares" (a,c,e,g...) y el segundo las "impares" (b,d,e,.)
         */
        
        int j;
        
        if (i == 0){ 
            
            /*
             * Este es el hijo 1 que imprime las letras pares.
             */
            
            for(j = 0; j < 26 ; j = j + 2){
                
                P(semaforo, 0);
                
                printf("pid: %d - %c\n", getpid(), abc[j]);
                V(semaforo, 1);
            }
            
            exit(0);
            
        }else{
            
            /*
             * Este es el hijo 2 que imprime las letras impares.
             */
            
            for(j = 1; j < 26 ; j = j + 2){
                
                P(semaforo, 1);
                
                printf("pid: %d - %c\n", getpid(), abc[j]);
                
                V(semaforo, 0);
            }
            
            exit(0);
            
        }
        
    }

    return 0;
}


/*******/
/* FIN */
/*******/