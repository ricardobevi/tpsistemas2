/************************************/
/* Nombre: ej4-servidor.c           */
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



#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

pid_t cons[2];

void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    printf("Terminado Servidor\n");
    kill(cons[0], SIGTERM);
    kill(cons[1], SIGTERM);
    exit(0);
}

int main( int argc, char* argv[] ){
    int i = 0,
        num = 0,
        fifo = 0;
        
    struct sigaction term;
    
    term.sa_sigaction = cTERM;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGINT, &term, NULL);
    
    if (argc == 2){
        mkfifo(argv[1], 0660);
        fifo = open ( argv[1], O_RDWR );
        
        printf("Esperando clientes...\n");
        for(i = 0 ; i < 2 ; i++){
            read(fifo, (int*) &cons[i], sizeof(pid_t));
            printf("Se conecto el cliente %d con PID %d\n", i + 1, cons[i]);
        }
        
        kill(cons[0], SIGUSR1);
        kill(cons[1], SIGUSR1);
        
        printf("Ingrese numeros enteros.\n-> ");
        
        while (1){
        
            scanf("%d",&num);
            
            write ( fifo, (int*) &num, sizeof(int) );
            
            if ( num % 2 == 0 ) {
                               
                kill(cons[0], SIGUSR2);
                
                printf("Numero PAR enviado a 1.\n-> ");
                
            }else{
                
                kill(cons[1], SIGUSR2);
                
                printf("Numero IMPAR enviado a 2.\n-> ");
                
            }
        }
    }else{
        
        printf("Mal Argumentos\n");
        
    }
    
    return 0;
}


/*******/
/* FIN */
/*******/