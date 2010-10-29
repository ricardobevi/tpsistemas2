#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <queue>
#include <sys/types.h>


#include "c_Bomberman.h"

using namespace std;

//nombres de hilos de ejecucion
void * sender (void * args);
void * screen (void * args);
void * recver (void * args);

/*

­pthread_mutex_t ProcesadorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  ProcesadorCond  = PTHREAD_COND_INITIALIZER;
­pthread_cond_wait(&ProcesadorCond, &actualizar);
­pthread_cond_broadcast(&CondicionActualizar);

*/


// Objeto cliente bomberman
Bomberman clienteBomberman;
queue <t_protocolo>  colaNovedades;


pthread_mutex_t SemColaNovedades     = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t inicioPantalla       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inicioTeclado        = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t actualizar           = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  CondicionActualizar  = PTHREAD_COND_INITIALIZER;


void finalizarCliente(int iNumSen, siginfo_t  * info, void *ni)
{
    clienteBomberman.finalizarBomberman();
    cout << endl << "El cliente ha finalizado" << endl << endl ;
    exit (0);
}



int main(int argc, const char *argv[]){
   
    struct sigaction term;

    term.sa_sigaction = finalizarCliente;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGINT, &term, NULL);

    
    pthread_t sender_t,                                               // creo los hilos correspondientes para la ejecucion:
              recver_t,                                               // receptor de datos, emisor de datos , y gestor de pantalla
              screen_t;
              
              
              
        // mediante estos mutex freno a los hilos de emision y pantalla    
        // hasta q comience la partida (inicio) o haya q actualizar la pantalla (pantalla)
        

        pthread_mutex_lock(&inicioPantalla); 
        pthread_mutex_lock(&inicioTeclado);   
                                                                          
        pthread_cond_wait(&CondicionActualizar, &actualizar);    
        
        pthread_mutex_lock(&SemColaNovedades); 
        
        
        pthread_create( &recver_t, NULL, recver, NULL);
        pthread_create( &sender_t, NULL, sender, NULL);
        pthread_create( &screen_t, NULL, screen, NULL);

        pthread_join(recver_t, NULL);
        pthread_join(screen_t, NULL);
        pthread_join(sender_t, NULL);
        
        return 0;
}


// hilo receptor de datos desde el servidor
void * recver(void * args)
{   
    t_protocolo accion;
    
    
    // espera a recivir todo el escenario
    // hasta que se le indica 'i' como informe de fin de datos,entonces guarda el nombre del jugador 
    // y habilita la ejecucion de lectura de teclado y refresco de pantalla
    
    clienteBomberman.recivirAccion( &accion, sizeof(t_protocolo) );
    
    while ( accion.id != 'i' )
    {
            clienteBomberman.actualizarNovedades( &accion );
            clienteBomberman.recivirAccion( &accion, sizeof(t_protocolo) );          
    }
    
    clienteBomberman.set_idJugador( accion.x );

    clienteBomberman.enviarSolicitud (0);
    
    // comienzo el juego ( habilito teclado y pantalla )
    pthread_mutex_unlock(&inicioPantalla);
    pthread_mutex_unlock(&inicioTeclado);
    
    pthread_cond_broadcast( &CondicionActualizar );
    
    // realiza un bucle infinito reciviendo acciones desde el servidor
    // actualiza el escenario con dichas acciones e informa (al hilo encargado de la pantalla)
    // mediante el semaforo  que debe realizar un refresco.
    
    pthread_mutex_unlock(&SemColaNovedades); 
    
    while ( true )
    {   
            
            clienteBomberman.recivirAccion( &accion, sizeof(t_protocolo) );
            
            /*cout << "id = " << accion.id << " pos = " << accion.posicion
                 << " (" << accion.x << "," << accion.y << ")"  << endl;*/
            
                pthread_mutex_lock( &SemColaNovedades );
            
                       colaNovedades.push(accion);
                
                pthread_mutex_unlock( &SemColaNovedades );
                
                pthread_cond_broadcast( &CondicionActualizar );
                          
    }
    
    
    return NULL; //solo para que no me tire warning
}


// hilo emisor de datos hacia el servidor
void * sender(void * args)
{
    // espera mediante el semaforo inicio a que se habilite la ejecucion del hilo
    // una vez habilitado comienza un bucle infinito a la espera de que se presione una tecla
    // si esto sucede, envia la solicitud de presionado de esa tecla al servidor
    
    pthread_mutex_lock(&inicioTeclado);
    
    int teclaPresionada;

    while (true)
    {
        teclaPresionada = clienteBomberman.leerTeclado();    // esto por ahora es un getch

        clienteBomberman.enviarSolicitud (teclaPresionada);
       
    }
    
    return NULL; //solo para que no me tire warning

}


// hilo encargado de la pantalla del cliente
void * screen(void * args)
{
    
    // espera mediante el semaforo inicio a que se habilite la ejecucion del hilo
    // una vez habilitado comienza un bucle infinito a la espera de que se desea actualizar la pantalla
    // si esto sucede, se le informa por el semaforo "pantalla" y se procede a redibujar la pantalla
    
    pthread_mutex_lock(&inicioPantalla);
    
    t_protocolo accion;
    
    bool colaVacia;
    
    while ( true )
    {
         

         
         do{
         
            pthread_mutex_lock( &SemColaNovedades );
            
                //if( ! colaNovedades.empty() )
                {
                    accion = colaNovedades.front();
                    
                    colaNovedades.pop();
                }
                    
            pthread_mutex_unlock( &SemColaNovedades );
        
            
            
            clienteBomberman.actualizarNovedades( &accion ); 
            
            clienteBomberman.dibujarPantalla();
            
            
            
            pthread_mutex_lock( &SemColaNovedades );
            
            colaVacia = colaNovedades.empty();
                
            pthread_mutex_unlock( &SemColaNovedades );
            
          
         }while ( !colaVacia );
                  
        pthread_cond_wait(&CondicionActualizar, &actualizar);
          
    }
    
    return NULL; //solo para que no me tire warning
}



/*******/
/* FIN */
/*******/