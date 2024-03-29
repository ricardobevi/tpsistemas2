/************************************/
/* Nombre: ej2.c                    */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 2           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#include <stdio.h>
#include <pthread.h>

#define TAM 50000
#define MAXNUM 50

unsigned int vec[TAM];
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void * llenador(void * args){
    long int i;
    int num = 0;
    
    for( num = 0 ; num <= MAXNUM ; num++ ){
        pthread_mutex_lock(&mutex1);
        
        for( i = 0 ; i < TAM ; i++)
            vec[i] = num;
            
        pthread_mutex_unlock(&mutex2);
    }
    return 0;
}

void * comprobador(void * args){
    int num;
    
    do{
        long int i = 0;
        
        pthread_mutex_lock(&mutex2);
        
        while( i++ < (TAM - 1) && vec[i] == vec [ i + 1 ] );
        
        num = vec[i - 1];
        
        pthread_mutex_unlock(&mutex1);
        
        if( i >= (TAM - 1) ) {
            printf("El vector tiene los mismos numeros. Numero: %d\n", num);
        } else {
            long int vCantidad[MAXNUM + 1];

            for( i = 0 ; i < MAXNUM ; i++)
                vCantidad[i] = 0;

            for( i = 0 ; i < TAM ; i++)
                vCantidad[vec[i]]++;

            printf("\nERROR! El vector contiene:\n");

            for( i = 0 ; i < MAXNUM ; i++ )
                if( vCantidad[i] > 0 )
                    printf("    %ld veces el numero: %ld\n", vCantidad[i], i );

            printf("\n");
        }
        
    }while( num < MAXNUM );
    
    return 0;
}

int main(){
    
    pthread_t tid1,
              tid2;

    pthread_mutex_lock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    pthread_create( &tid1, NULL, llenador, NULL);
    
    pthread_create( &tid2, NULL, comprobador, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}


/*******/
/* FIN */
/*******/