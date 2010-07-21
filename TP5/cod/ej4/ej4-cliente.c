/************************************/
/* Nombre: ej4-cliente.c            */
/* Trabajo: Senales, Pipes y FIFOs  */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D`Aranno Facundo      34.842.320 */
/* Jose Ferreyra         31.144.004 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>

pid_t serverPid;
char *archivoOut;
int fifo,
    nums[100],
    cant = 0,
    max = 0,
    min = 0,
    sum = 0;

void cSIGUSR1(int iNumSen, siginfo_t *info, void *ni) {
  serverPid = info->si_pid;
}

void cSIGUSR2(int iNumSen, siginfo_t *info, void *ni) {
    read(fifo, (int*) &nums[cant], sizeof(int) );
    sum += nums[cant];
    
    if ( cant == 0 )
        max = min = nums[cant];
    
    if ( nums[cant] > max )
        max = nums[cant];
    else
        min = nums[cant];
    
    cant++;
}

void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    int i = 0;
    
    FILE * arch = fopen(archivoOut, "w");
    fprintf( arch, "Maximo = %d\n"
                   "Minimo = %d\n"
                   "Promedio = %.2f\n", max, min, (float) sum / cant );
    
    for( i = 0 ; i < cant ; i++ )
        fprintf( arch, "%d\n", nums[i] );
    
    fclose(arch);
    printf("Terminado Cliente\n");
    kill(serverPid, SIGTERM);
    exit(0);
}

int main( int argc, char* argv[] ){
    pid_t miPid = 0;
    struct sigaction usr1,
                     usr2,
                     term;
    
    int i = 0;
    
    daemon(1, 1);
    
    usr1.sa_sigaction = cSIGUSR1;
    usr2.sa_sigaction = cSIGUSR2;
    term.sa_sigaction = cTERM;
    
    sigfillset( &usr1.sa_mask );
    sigfillset( &usr2.sa_mask );
    sigfillset( &term.sa_mask );
    
    usr1.sa_flags = SA_SIGINFO;
    usr2.sa_flags = SA_SIGINFO;
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    
    sigaction(SIGTERM, &term, NULL);
    
    if (argc == 3){
        
        if ( ( fifo = open ( argv[1], O_RDWR ) ) < 0 ){
            perror("Error FIFO: ");
            return -1;
        }
        
        archivoOut = argv[2];
        
        miPid = getpid();
        
        write(fifo, (int*) &miPid, sizeof(pid_t));
        
        sigaction(SIGUSR1, &usr1, NULL);
        sigaction(SIGUSR2, &usr2, NULL);
        
        while(1);
        
    }else{
        printf("Mal Argumentos\n");
    }
    
    return 0;
}


/*******/
/* FIN */
/*******/