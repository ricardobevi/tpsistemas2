/********************************************************/
/* Nombre: ej3.c                                        */
/* Trabajo: Fork, Procesos Concurrentes, Zombies y Exec */
/* Numero de ejercicio: 3                               */
/* Entrega: Primer Entrega                              */
/*                                                      */
/* Grupo N 63                                           */
/* D`Aranno Facundo      34.842.320                     */
/* Jose Ferreyra         31.144.004                     */
/* Marcela A. Uslenghi   26.920.315                     */
/* Bevilacqua Ricardo    34.304.983                     */
/********************************************************/



#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    
    pid_t childpid;
    
    /*
     * Hace un vfork,lo cual genera un proceso hijo,a diferencia del fork
     * esto bloquea al padre hasta que el hijo ejecuta una llamada a la funcion de la familia exec() o realiza un _exit()
     * luego valida los valores devueltos por el vfork
     */
    if ( ( childpid = vfork() ) == -1 ) {
        
        perror("Error al crear el hijo");
        exit(1);
        
    }else if( childpid == 0 ){
        /*
         * Ejecuta execvp(). Carga el programa pasado como primer parametro
         * y le pasa como parametros los siquientes argumentos del programa
         * principal. En caso de error finaliza con . Esto desbloquea al padre.
         */
        if( execvp(argv[1], &argv[1]) < 0 ){
            _exit(1);
        }
    }else {
        
        /*
         * las siguientes lineas son del padre : este espera al hijo, cuando este termina informa que el es
         * el padre y termina la ejecucion del programa
         */
        
        wait(NULL);
        printf("\nSoy el padre y termino\n");
        exit(0);
    }
    
}



/*******/
/* FIN */
/*******/