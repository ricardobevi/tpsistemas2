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


struct arg{
    unsigned int size;
    char *argum[100];
    char arch[100];
    unsigned char bkground;
    unsigned char crear;
    unsigned char agregar;
};


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