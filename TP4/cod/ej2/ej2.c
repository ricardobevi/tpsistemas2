/********************************************************/
/* Nombre: ej2.c                                        */
/* Trabajo: Fork, Procesos Concurrentes, Zombies y Exec */
/* Numero de ejercicio: 2                               */
/* Entrega: Primer Entrega                              */
/*                                                      */
/* Grupo N 63                                           */
/* D`Aranno Facundo      34.842.320                     */
/* Jose Ferreyra         31.144.004                     */
/* Marcela A. Uslenghi   26.920.315                     */
/* Bevilacqua Ricardo    34.304.983                     */
/********************************************************/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    
    pid_t hijo;

    if( ( hijo = fork() ) < 0 ) {
        perror("Error!");
        exit(EXIT_FAILURE);
    }
    if( hijo == 0 ) {
        execv("./mipid", NULL);
        perror("Error!");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    printf("Fin!\n");

}





/*******/
/* FIN */
/*******/