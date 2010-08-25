#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>


/*Includes de Semaforos*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void P( int semaforo, unsigned short semNum ){
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = -1;
    opSem.sem_flg = 0;
    
    if ( semop(semaforo, &opSem, 1) == -1){
        perror("P: semop");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
}

void V( int semaforo, unsigned short semNum ){
    struct sembuf opSem;
    
    opSem.sem_num = semNum;
    opSem.sem_op = 1;
    opSem.sem_flg = 0;
    
    if ( semop(semaforo, &opSem, 1) == -1){
        perror("V: semop");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
}

int main(){
    char abc[26] = {'a','b','c','d',
                    'e','f','g','h',
                    'i','j','k','l',
                    'm','n','o','p',
                    'q','r','s','t',
                    'u','v','w','x',
                    'y','z'};
                    
    union semun{
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    }arg;
                  
    pid_t childPids[3];
    int i = 0,
        letra = 0,
        semaforo = 0;
        
    key_t clave;
    
    clave = ftok(".", 'R');

    if ( ( semaforo = semget(clave, 2, IPC_CREAT | IPC_EXCL | 0660 ) ) == -1 ){
        perror("main: semget");
        if ( errno == EEXIST )
            printf("El semaforo ya existe, posiblemente porque el programa ya se este ejecutando.\n");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
    
    arg.val = 1;
    if ( semctl(semaforo, 0, SETVAL, arg) == -1){
        perror("main: semctl1");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
    
    arg.val = 0;    
    if ( semctl(semaforo, 1, SETVAL, arg) == -1){
        perror("main: semctl2");
        semctl(semaforo, 0, IPC_RMID);
        exit(1);
    }
    
    
    while( i < 2 && ( childPids[i] = fork() ) != 0 ) i++;
    
    if(i == 2){/* Padre */
        
        wait(NULL);
        wait(NULL);
        
        semctl(semaforo, 0, IPC_RMID);
        
    }else if ( childPids[ i ] == 0 ){/* Hijos */
        int j;
        
        if (i == 0){ 
            
            for(j = 0; j < 26 ; j = j + 2){
                
                P(semaforo, 0);
                
                printf("pid: %d - %c\n", getpid(), abc[j]);
                V(semaforo, 1);
            }
            
            exit(0);
            
        }else{
            
            
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