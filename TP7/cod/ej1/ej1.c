/************************************/
/* Nombre: ej1.c                    */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 1           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#include <stdio.h>
#include <pthread.h>

#define TAM 500000

unsigned int vec[TAM];

void * llenador(void * args){
    long int i;
    int num = 0;
    
    for( num = 0 ; num <= 50 ; num++ )
        for( i = 0 ; i < TAM ; i++)
            vec[i] = num;
    
    return 0;
}

void * comprobador(void * args){
    
    
    do{
        long int i = 0;    
        
        while( i++ < (TAM - 1) && vec[i] == vec [ i + 1 ] );
        
        if( i >= (TAM - 1) )
            printf("El vector tiene los mismos numeros. Numero: %d\n", vec[i - 1]);
        else
            printf("ERROR!\n");
    
    }while( vec[TAM - 1] < 50 );
    
    return 0;
}

int main(){
    
    pthread_t tid1,
              tid2;

    pthread_create( &tid1, NULL, llenadora, NULL);
    
    pthread_create( &tid2, NULL, comprobador, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}


/*******/
/* FIN */
/*******/