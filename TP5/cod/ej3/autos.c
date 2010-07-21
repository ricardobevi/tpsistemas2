/************************************/
/* Nombre: autos.c                  */
/* Trabajo: Senales, Pipes y FIFOs  */
/* Numero de ejercicio: 3           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D`Aranno Facundo      34.842.320 */
/* Jose Ferreyra         31.144.004 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

 typedef struct autos{
     
    pid_t pidAuto;
    int cantViajes;
    
}autos_t;

autos_t miInfo;

pid_t childPids[6];

int colaAutos = 0,
    viajes = 0;

void cSIGUSR1(int iNumSen, siginfo_t *info, void *ni) {
    int tViaje;
    
    read( viajes, (int*) &tViaje, sizeof(int) );
    
    printf("Soy %d, hice %d viajes y salgo por %d segundos.\n", getpid(), miInfo.cantViajes, tViaje);
    
    sleep( tViaje );
    
    miInfo.cantViajes++;
}

void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    int i = 0;
    
    for( i = 0 ; i < 6 ; i++ ) 
        kill( childPids[i], SIGTERM );
    
    for( i = 0 ; i < 6 ; i++ ) 
        wait(NULL);
    
    printf("Terminado Autos\n");
    exit(0);
}

int main(){
    int i = 0;
    
    colaAutos = open ( "/tmp/colaAutos", O_RDWR );
    viajes = open ( "/tmp/viajes", O_RDWR );
    
    while( i < 6 && ( childPids[i] = fork() ) != 0 ) i++;
    
    if(i == 6){/* Padre */
        struct sigaction term;
                
        term.sa_sigaction = cTERM;
        sigfillset( &term.sa_mask );
        term.sa_flags = SA_SIGINFO | SA_NODEFER;
        sigaction(SIGTERM, &term, NULL);
        sigaction(SIGINT, &term, NULL);
        
        printf("En espera de señales del servidor...\n" 
               "(Matar proceso %d o control-c para terminar los 6 clientes)\n", getpid() );
        
        while(1)
            pause();
        
    }else{/* Hijos */
        struct sigaction usr1;
               
        usr1.sa_sigaction = cSIGUSR1;
        sigfillset( &usr1.sa_mask );
        sigdelset(&usr1.sa_mask, SIGTERM);
        usr1.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &usr1, NULL);
        
        miInfo.pidAuto = getpid();
        miInfo.cantViajes = 0;
        
        write ( colaAutos, (autos_t*) &miInfo, sizeof(autos_t) );
        
        while(1){
            pause();
            write ( colaAutos, (autos_t*) &miInfo, sizeof(autos_t) );
        }
    }
    
    return 0;
}


/*******/
/* FIN */
/*******/