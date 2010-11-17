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
#include <ctime>

#include "c_Bomberman.h"

using namespace std;

//nombres de hilos de ejecucion
void * sender (void * args);
void * screen (void * args);
void * recver (void * args);


// Objeto cliente bomberman
Bomberman clienteBomberman;
queue <t_protocolo>  colaNovedades;


pthread_mutex_t SemColaNovedades     = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t inicioPantalla       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inicioTeclado        = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t actualizar           = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  CondicionActualizar  = PTHREAD_COND_INITIALIZER;


pthread_t sender_t,                                               // creo los hilos correspondientes para la ejecucion:
          recver_t,                                               // receptor de datos, emisor de datos , y gestor de pantalla
          screen_t;

// manejo de señales para cerrar el cliente en caso de falla de el servidor
// o cerrar correctamente el cliente ante un control + c
void finalizarCliente(int iNumSen, siginfo_t  * info, void *ni)
{
    pthread_cancel(sender_t);
    pthread_cancel(recver_t);
    pthread_cancel(screen_t);
    clienteBomberman.finalizarBomberman();
    
    system("reset");
    
    if (iNumSen == SIGPIPE)
    {
        cout << endl  << endl <<"ERROR: Fallo la conexion al socket" << endl << " El servidor no esta activo o termino abruptamente. "  << endl;
    }
    
     
    if (iNumSen == SIGUSR1)
    {
        cout << endl  << endl <<"ERROR: Fallo la comunicacion al segmento de memoria compartida" << endl << " El servidor no esta activo o termino abruptamente. "  << endl;
    }
    
    cout << endl << "El cliente ha finalizado" << endl << endl ;
    exit (0);
}


// hilo principal de ejecucion ( main )

int main(int argc, const char *argv[]){
   
    struct sigaction term;

    term.sa_sigaction = finalizarCliente;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGINT , &term, NULL);
    sigaction(SIGPIPE , &term, NULL);
    sigaction(SIGTERM , &term, NULL);
    sigaction(SIGQUIT , &term, NULL);
    sigaction(SIGABRT , &term, NULL);
    sigaction(SIGSEGV , &term, NULL);    
    sigaction(SIGUSR1 , &term, NULL);   

    srand(unsigned(time(NULL)));
           
              
        // mediante estos mutex freno a los hilos de emision y pantalla    
        // hasta q comience la partida (inicio) o haya q actualizar la pantalla (pantalla)
        

        pthread_mutex_lock(&inicioPantalla); 
        pthread_mutex_lock(&inicioTeclado);   
                                                                          
        //pthread_cond_wait(&CondicionActualizar, &actualizar);    
        
        pthread_mutex_lock(&SemColaNovedades); 
            
        pthread_create( &recver_t, NULL, recver, NULL);
        pthread_create( &sender_t, NULL, sender, NULL);
        pthread_create( &screen_t, NULL, screen, NULL);

        pthread_join(screen_t, NULL);
        
        pthread_cancel(sender_t);
        pthread_cancel(recver_t);
        
        clienteBomberman.finalizarBomberman();
  
        system("reset");
        
        cout << endl << "Fin de juego." << endl << endl ;
        
        pthread_join(recver_t, NULL);
        pthread_join(sender_t, NULL);
        
        return 0;
              
}



/**********************************************************************************************************/

// hilo receptor de datos desde el servidor

void * recver(void * args)
{   
    t_protocolo accion;

    // recibe el numero de jugador :)
    clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );

    clienteBomberman.set_idJugador( accion.x );
    
    // las siguientes lineas son las encargadas de la pantalla del timeout al ingreso del juego
    // envia al servidor cual es su timeout y
    // espera ese tiempo de timeout como maximo para esperar otros jugadores

    clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );

    while ( accion.id != 's' )
    {
            clienteBomberman.esperaDeJugadores(  accion.x );
            clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );
    }

    // espera a recivir todo el escenario
    // hasta que se le indica 'i' como informe de fin de datos
    // y habilita la ejecucion de lectura de teclado y refresco de pantalla
    clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );

    while ( accion.id != 'i' )
    {
            clienteBomberman.actualizarNovedades( &accion );
            clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );
    }


    
    // comienzo el juego ( habilito teclado y pantalla )
    pthread_mutex_unlock(&inicioPantalla);
    
    // si espectador devuelve true significa que el jugador no podra jugar y solo observara la partida
    // por lo tanto no se le activara el hilo de lectura de teclado al mismo
    if ( clienteBomberman.espectador() == false ) 
                    pthread_mutex_unlock(&inicioTeclado);
    
    
    pthread_cond_broadcast( &CondicionActualizar );
    
    // realiza un bucle infinito reciviendo acciones desde el servidor
    // actualiza el escenario con dichas acciones e informa (al hilo encargado de la pantalla)
    // mediante el semaforo  que debe realizar un refresco.
    
    pthread_mutex_unlock(&SemColaNovedades); 
    
    while ( accion.id != 'F' )
    {   
            
                clienteBomberman.recibirAccion( &accion, sizeof(t_protocolo) );
            

                pthread_mutex_lock( &SemColaNovedades );
                     
                colaNovedades.push(accion);
                
                pthread_mutex_unlock( &SemColaNovedades );
                     
                
                pthread_cond_broadcast( &CondicionActualizar );
                          
    }
    
    
    return NULL; //solo para que no me tire warning
}


/* *******************************************************************  

prueba para ver si funca fin de partida

   cout << "id = " << accion.id << " pos = " << accion.posicion
    << " (" << accion.x << "," << accion.y << ")"  << endl;
                    
    t_protocolo prueba;
    prueba.id = 'P';
    prueba.posicion = 0;

    prueba.x = 1;
    prueba.y = 4;
    colaNovedades.push(prueba);

    prueba.x = 2;
    prueba.y = 3;
    colaNovedades.push(prueba);

    prueba.x = 0;
    prueba.y = 2;
    colaNovedades.push(prueba);

    prueba.x = 3;
    prueba.y = 1;
    colaNovedades.push(prueba);

    ...
    ...


    sleep( 3 );

    pthread_mutex_lock( &SemColaNovedades );
    prueba.id = 'F';
    colaNovedades.push(prueba);
    pthread_mutex_unlock( &SemColaNovedades );
                       
*************************************************************************** */



     
/*************************************************************************** */
// hilo emisor de datos hacia el servidor
void * sender(void * args)
{
    // espera mediante el semaforo inicio a que se habilite la ejecucion del hilo
    // una vez habilitado comienza un bucle infinito a la espera de que se presione una tecla
    // si esto sucede, envia la solicitud de presionado de esa tecla al servidor
    
    pthread_mutex_lock(&inicioTeclado);
    
    //clienteBomberman.enviarSolicitud ( getch() );

    int teclaPresionada;
    
    while ( ! clienteBomberman.espectador() )
    {   
        teclaPresionada = clienteBomberman.leerTeclado();
        clienteBomberman.enviarSolicitud ( teclaPresionada );     
    }
    
    return NULL; //solo para que no me tire warning

}



/*************************************************************************** */

// hilo encargado de la pantalla del cliente
void * screen(void * args)
{
    
    // espera mediante el semaforo inicio a que se habilite la ejecucion del hilo
    // una vez habilitado comienza un bucle infinito a la espera de que se desea actualizar la pantalla
    // si esto sucede, se le informa por el semaforo "pantalla" y se procede a redibujar la pantalla
    
    pthread_mutex_lock(&inicioPantalla);
    
    t_protocolo accion;
    
    bool colaVacia;
    int  finDePartida = 0;
    
    while ( !finDePartida )
    {
         

         // itero en el siguiente do while mientras la cola de novedades tenga algo para actualizar, cuando esta ya este vacia, Salgo.
         do{
         
                    pthread_mutex_lock( &SemColaNovedades );
                    
                                if( ! colaNovedades.empty() )
                                {
                                    accion = colaNovedades.front();
                                    
                                    colaNovedades.pop();
                                }
                            
                    pthread_mutex_unlock( &SemColaNovedades );
                
                    
                    
                   finDePartida =  clienteBomberman.actualizarNovedades( &accion ); 
                    
                    clienteBomberman.dibujarPantalla();
                    
                    
                    
                    pthread_mutex_lock( &SemColaNovedades );
                    
                    colaVacia = colaNovedades.empty();
                        
                    pthread_mutex_unlock( &SemColaNovedades );
                    
          
         }while ( !colaVacia &&  !finDePartida );
         
         
        // espero a que me avise el hilo de recepcion que se ha modificado la cola          
        if (!finDePartida)
            pthread_cond_wait(&CondicionActualizar, &actualizar);
          
    }
    
    return NULL; //solo para que no me tire warning
}



/*******/
/* FIN */
/*******/