/********************************************************/
/* Nombre: split.h                                      */
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



#ifndef SPLIT_H
#define SPLIT_H

#include<stdio.h>
#include<string.h>

/*
 * Defino la estructura basica para devolver por la funcion.
 * Esta estructura contiene:
 *
 * unsigned int size  Cantidad de argumentos incluyendo el nombre del programa.
 *
 * char *agrum[100]   Vector que almacena punteros a los argumentos.
 *
 * char arch[100]     Cadena que, en caso de haberlo, almacena el nombre del 
 *                    archivo al cual redirigir la salida.
 *
 * unsigned char bkground  Indica si se especifico la opcion de background.
 *                         (1 = SI, 0 = NO)
 *
 * unsigned char crear     Indica si se debe crear el archivo de salida.
 *                         (1 = SI, 0 = NO)
 *
 * unsigned char agregar   Indica si se debe agregar la salida al archivo.
 *                         (1 = SI, 0 = NO)
 * 
 */
struct arg{
    unsigned int size;
    char *argum[100];
    char arch[100];
    unsigned char bkground;
    unsigned char crear;
    unsigned char agregar;
};

/*       _\|/_
         (o o)
 +----oOO-{_}-OOo--------------------------------------------------+
 |Funcion split_args                                               |
 |                                                                 |
 |Esta funcion divide los argumentos pasados por linea de comandos.|
 |Tambien indica si hay un archivo al cual redirigir la salida y si|
 |se especifico o no la opcion de background.                      |
 |                                                                 |
 |Recive:                                                          |
 |                                                                 |
 |char * str                Cadena que contiene los argumentos.    |
 |                                                                 |
 |struct arg * argumentos   Puntero a una estructura del tipo arg. |
 |                                                                 |
 |Devuelve:                                                        |
 |                                                                 |
 |La cantidad de argumentos, contando al programa.                 |
 +----------------------------------------------------------------*/


int split_args( char * str, struct arg * argumentos ){
    char * lastArg;
    unsigned char flag_comillas = 0;
    int i = 0;
       
    argumentos->size = 0;
    argumentos->bkground = 0;
    argumentos->crear = 0;
    argumentos->agregar = 0;

    while( *str != '\0' && *str == ' ' ) str++;
    
    if( *str != '\0' ){
        
        argumentos->argum[argumentos->size++] = str;
    
        do{
                       
            if( !flag_comillas && *str == ' '){
                *str = '\0';
                str++;
                while( *str != '\0' && *str == ' ' ) str++;
                               
                if( *str != '\0' )
                    argumentos->argum[argumentos->size++] = str;
            }
            
            if(*str == '"'){
                flag_comillas = !flag_comillas;
            }
                       
        }while( *++str != '\0' );
        
        lastArg = argumentos->argum[argumentos->size - 1];
               
        if(lastArg[strlen(lastArg) - 1] == '&'){
            argumentos->bkground = 1;
            
            if( strlen(lastArg) == 1 )
                argumentos->size--;
            
            lastArg[strlen(lastArg) - 1] = '\0';
        }
        
        do{    
            if( argumentos->argum[i][0] == '"' ){
                argumentos->argum[i]++;
                argumentos->argum[i][ strlen(argumentos->argum[i]) -1 ] = '\0';
            }
            
        }while( ++i < argumentos->size && argumentos->argum[i][0] != '>');
        
        if( i < argumentos->size && argumentos->argum[i][0] == '>' ){
            if( (i + 1) < argumentos->size ){
                
                if( argumentos->argum[i][1] == '>' )
                    argumentos->agregar = 1;
                else
                    argumentos->crear = 1;
                
                strcpy( argumentos->arch, argumentos->argum[i + 1] );
                
            }
            
            argumentos->size = i;
            
        }
        
        
    }
    
    argumentos->argum[argumentos->size] = NULL;
    
    return argumentos->size;
}

#endif


/*******/
/* FIN */
/*******/