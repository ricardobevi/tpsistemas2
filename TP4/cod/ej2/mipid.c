/********************************************************/
/* Nombre: mipid.c                                      */
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



#include <unistd.h>
#include <stdio.h>
int main() {
    printf("PID: %d PPID: %d\n", getpid(), getppid());
    sleep(10);
    return 0;
}


/*******/
/* FIN */
/*******/