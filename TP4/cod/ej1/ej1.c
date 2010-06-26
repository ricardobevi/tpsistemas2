#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    
    int i;
    pid_t hijo;
    
    /* 
     * Este for crea 1 proceso hijo, nunca llega a hacer las 3 iteraciones
     * ya que el hijo ejecuta un break y el padre, una vez terminado el 
     * hijo, realiza un exit(). Como solo genera un hijo, es inecesario
     * usar 3 wait(), con uno alcanzaria.
     */
    for( i = 0 ; i < 3 ; i++ ) {
        if( ( hijo = fork() ) == 0 ) {
            printf("Mi PPID es %d y mi PID es %d\n", getppid(), getpid());
            break;
        }
        else if( hijo != 0 ) {
            printf("Mi PPID es %d y mi PID es %d\n", getppid(), getpid());
            wait(NULL);
            wait(NULL);
            wait(NULL);
            exit(0);
        }
    }
    
    /*
     * El hijo entra en este for crea un nuevo proceso (nieto) y espera
     * a que termine con wait(). El nieto crea un bisnieto y espera que termine.
     * El bisnieto crea un tataranieto y espera a que termine. Por ultimo
     * el tataranieto llega al ultimo ciclo del for y termina, terminando en
     * cadena todos los procesos anteriores. El wait(NULL) no se bloquea si
     * al proceso no le quedan hijos.
     */
    for( i = 0 ; i < 3 ; i++ ) {
        if( ( hijo = fork() ) == 0 ) {
            printf("Mi PPID es %d y mi PID es %d\n", getppid(), getpid());
            sleep(5);
        }
        else if( hijo != 0 ) {
            printf("Mi PPID es %d y mi PID es %d\n", getppid(), getpid());
            wait(NULL);
            wait(NULL);
            wait(NULL);
            exit(0);
        }
    }
    
    return 0;
}
