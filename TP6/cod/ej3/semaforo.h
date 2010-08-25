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


int setSem(int semId, int semNum, int semVal){
    arg.val = semVal;
    if ( semctl(semId, semNum, SETVAL, arg) == -1){
        perror("semaforo.h: setSem: semctl");
        semctl(semId, 0, IPC_RMID);
        return -1;
    }
    
    return 0;

}

int rmSem(semId){
    return semctl(semId, 0, IPC_RMID);
}

#endif
