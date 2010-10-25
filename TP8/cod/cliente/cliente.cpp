/************************************/
/* Nombre: cliente.cpp              */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>

#include "c_Bomberman.h"

//nombres de hilos de ejecucion
void * sender (void * args);
void * screen (void * args);
void * recver (void * args);

// Objeto cliente bomberman
Bomberman clienteBomberman;

pthread_mutex_t inicioPantalla  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inicioTeclado = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pantalla = PTHREAD_MUTEX_INITIALIZER;


using namespace std;

int main(int argc, const char *argv[]){
   
    
    pthread_t sender_t,                                               // creo los hilos correspondientes para la ejecucion:
              recver_t,                                               // receptor de datos, emisor de datos , y gestor de pantalla
              screen_t;
   
        pthread_mutex_lock(&inicioPantalla); 
        pthread_mutex_lock(&inicioTeclado);                          // mediante estos mutex freno a los hilos de emision y pantalla :     
        pthread_mutex_lock(&pantalla);                                // hasta q comience la partida (inicio) o haya q actualizar la pantalla (pantalla)
        
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
    
    /*
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
    */
    
    // comienzo el juego ( habilito teclado y pantalla )
    pthread_mutex_unlock(&inicioPantalla);
    pthread_mutex_unlock(&inicioTeclado);
    
    // realiza un bucle infinito reciviendo acciones desde el servidor
    // actualiza el escenario con dichas acciones e informa (al hilo encargado de la pantalla)
    // mediante el semaforo  que debe realizar un refresco.
    
    while ( true )
    {
            clienteBomberman.recivirAccion( &accion, sizeof(t_protocolo) );
            clienteBomberman.actualizarNovedades( &accion );
            
            pthread_mutex_unlock( &pantalla );
    }
    
    
    return NULL; //solo para que no me tire warning
}


// hilo emisor de datos hacia el servidor
void * sender(void * args)
{
    int teclaPresionada;
    ofstream errores;
    
    // espera mediante el semaforo inicio a que se habilite la ejecucion del hilo
    // una vez habilitado comienza un bucle infinito a la espera de que se presione una tecla
    // si esto sucede, envia la solicitud de presionado de esa tecla al servidor
    
    pthread_mutex_lock(&inicioTeclado);
    
    while (true)
    {
        //teclaPresionada = clienteBomberman.leerTeclado(); // esto por ahora es un getch
        
        teclaPresionada = getch();
            
            errores.open ("errores.err", fstream::in | fstream::out | fstream::app);
            errores << (char) teclaPresionada <<endl;
            errores.close();
            
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
    
    while ( true )
    {
         pthread_mutex_lock(&pantalla);
         clienteBomberman.dibujarPantalla();
    }
    
    return NULL; //solo para que no me tire warning
}



/*******/
/* FIN */
/*******/