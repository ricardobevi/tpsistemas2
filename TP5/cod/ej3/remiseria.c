/************************************/
/* Nombre: remiseria.c              */
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
#include <time.h>
#include <unistd.h>

#define SMIN 1
#define SMAX 5

 typedef struct autos{
     
    pid_t pidAuto;
    int cantViajes;
    
}autos_t;

int colaAutos = 0,
    viajes = 0;
    
void cSIGUSR1(int iNumSen, siginfo_t *info, void *ni) {
    int tViaje =  ( rand() % (SMAX - SMIN) ) + SMIN;
    autos_t infoAuto;
    
    read(colaAutos, (autos_t*) &infoAuto, sizeof(autos_t));
    
    write( viajes, (int*) &tViaje, sizeof(int) );
    
    kill(infoAuto.pidAuto, SIGUSR1);
    
}

void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    int i = 0;
    autos_t infoAuto;
    
    printf("\n");
    printf("PID Auto | Salidas\n"
           "------------------\n");
    
    for(i = 0 ; i < 6 ; i++){
        read(colaAutos, (autos_t*) &infoAuto, sizeof(autos_t));
        printf("%-8d | %7d\n", infoAuto.pidAuto, infoAuto.cantViajes);
    }
    
    printf("Terminado servidor remiseria.\n");
    exit(0);
}

int main(){
    struct sigaction usr1,
                     term;

    printf("Iniciando manejadores de senales...");
    
    usr1.sa_sigaction = cSIGUSR1;
    term.sa_sigaction = cTERM;
    
    sigfillset( &usr1.sa_mask );
    sigfillset( &term.sa_mask );
    
    usr1.sa_flags = SA_SIGINFO;
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGUSR1, &usr1, NULL);
    
    printf("OK!\n");
    
    printf("Creando archivos FIFO...");
    
    mkfifo("/tmp/colaAutos", 0660);
    mkfifo("/tmp/viajes", 0660);
    
    printf("OK!\n");
    
    printf("Abriendo archivos FIFO...");
    colaAutos = open ( "/tmp/colaAutos", O_RDWR );
    viajes = open ( "/tmp/viajes", O_RDWR );
    printf("OK!\n");
    
    printf("Iniciando semilla aleatoria...");
    srand( time(NULL) );
    printf("OK!\n");
    
    printf("Iniciando trabajo en segundo plano...");
    daemon(1, 1);
    printf("OK!\n");
    
    printf("Servidor iniciado correctamente (PID %d), esperando senales.\n", getpid() );
    
    while(1) 
        pause();
    
    close(colaAutos);
    close(viajes);
    
    return 0;
}


/*******/
/* FIN */
/*******/