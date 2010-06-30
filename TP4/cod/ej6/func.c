/********************************************************/
/* Nombre: func.c                                       */
/* Trabajo: Fork, Procesos Concurrentes, Zombies y Exec */
/* Numero de ejercicio: 6                               */
/* Entrega: Primer Entrega                              */
/*                                                      */
/* Grupo N 63                                           */
/* D`Aranno Facundo      34.842.320                     */
/* Jose Ferreyra         31.144.004                     */
/* Marcela A. Uslenghi   26.920.315                     */
/* Bevilacqua Ricardo    34.304.983                     */
/********************************************************/



#include<stdio.h>
#include<string.h>
#include "split.h"

int main(){
    char x[1000];
    int i = 0;
    struct arg argumentos;
    
    fgets(x, 1000, stdin);
    x[strlen(x)] = '\0';
    
    split_args(x, &argumentos);
    printf("%d\nbkground = %d\ncrear = %d\nagregar = %d\narch = %s\n",argumentos.size, 
                                                                    argumentos.bkground,
                                                                    argumentos.crear,
                                                                    argumentos.agregar,
                                                                    argumentos.arch);
    
    for(i = 0; i < argumentos.size ; i++ )
        puts(argumentos.argum[i]);
    
    return 0;
}


/*******/
/* FIN */
/*******/