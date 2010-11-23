/*

Realizar un programa servidor que atienda hasta 5 clientes concurrentes, pero que no
rechace nunca una conexión, que pueden ser ejecutados en múltiples máquinas.
El servidor le asignará a cada uno de los clientes un número único de cliente y le informará la hora
actual del servidor a través del canal de comunicaciones que tienen en común.


El cliente deberá comparar la hora recibida con la hora local e informarle al servidor la diferencia de
tiempo existente entre ambos relojes.


El servidor, al momento de recibir una señal para finalizar, deberá mostrar el resumen de los números de clientes, la IP 
y la diferencia en los relojes que se calculó en el cliente.
Nota: el servidor deberá manejar las comunicaciones a través de procesos livianos

                ssize_t Send( const T* buf, size_t size ) const;
                ssize_t Recv( T* buf, size_t size );
        
        

Comm(long int port = 50000);
int Listen(int backlog = 10);
unsigned Accept();

Connection<T> getConn(unsigned index);

void CloseCons();
void Close();


*/


#include <vector>
#include <iostream>
#include <sys/types.h>
#include <queue>
#include <time.h>
#include <signal.h>
#include <stdlib.h>

#include "Comm.h"

#define PORT 60000


// --------------------------- estructuras  -------------------------------

typedef struct{
    
    string ip;
    int difTiempo;
    int numCliente;
    
}result;

// -------------------------- fin estructuras  -----------------------------




// --------------------------- variables globales -------------------------------



Comm < char > Comunicacion( PORT );


queue < unsigned >  colaClientes;
queue < result >  colaResultados;


pthread_mutex_t SemColaClientes  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SemColaResultados  = PTHREAD_MUTEX_INITIALIZER;

pthread_t   proc1_t,                                               
            proc2_t,                                               
            proc3_t, 
            proc4_t, 
            proc5_t;
            
// -------------------------- fin variables globales -----------------------------






// --------------------------- prototipo de funciones -------------------------------

void * procesador(void * args);

void finalizar(int iNumSen, siginfo_t  * info, void *ni);

// --------------------------  fin prototipo de funciones ---------------------------




// main : parte principal del programa

int main(int argc, const char *argv[])
{

    //  analisis de señales
    
    struct sigaction term;

    term.sa_sigaction = finalizar;
    sigfillset( &term.sa_mask );
    
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    
    sigaction(SIGINT , &term, NULL);
    sigaction(SIGPIPE , &term, NULL);
    sigaction(SIGTERM , &term, NULL);
    sigaction(SIGQUIT , &term, NULL);
    sigaction(SIGABRT , &term, NULL);
    sigaction(SIGSEGV , &term, NULL);  
    
    //  fin analisis de señales
    
    

     
     pthread_create( &proc1_t, NULL, procesador, NULL);
     pthread_create( &proc2_t, NULL, procesador, NULL);
     pthread_create( &proc3_t, NULL, procesador, NULL);
     pthread_create( &proc4_t, NULL, procesador, NULL);
     pthread_create( &proc5_t, NULL, procesador, NULL);
     
     cout << endl << "El servidor ha iniciado y esta a la espera de clientes: " << endl;
     
     Comunicacion.Listen();
     
     unsigned numCliente;
     
     while ( true )
     {
         
         numCliente= Comunicacion.Accept();
         
         cout << "Conectado cliente  " <<  numCliente <<  ".........[OK]" << endl;
         
         pthread_mutex_lock( &SemColaClientes ); 
         
         colaClientes.push( numCliente );
         
         pthread_mutex_unlock( &SemColaClientes ); 
         

     }
    
    
}


void * procesador(void * args)
{
   int cliAProcesar = 0;
   result resultFinal;
   int difTiempo = 0;
   Connection <char>  cliente;
   time_t tiempoServ;
   
   while( true )
   {
       
        pthread_mutex_lock( &SemColaClientes ); 
            
            if (  ! colaClientes.empty()  )
            {
                cliAProcesar = colaClientes.front();
                colaClientes.pop();
            }   
            else
            {
                cliAProcesar = -1;
            }
                 
        pthread_mutex_unlock( &SemColaClientes ); 
        
        
        if ( cliAProcesar != -1 )  // si hay clientes a procesar
        {
           
            cliente = Comunicacion.getConn( ( unsigned ) cliAProcesar );
            
            
            //envio tiempo actual al cliente
            time( &tiempoServ );
             
            cliente.Send(   (const char * ) &tiempoServ ,  sizeof( time_t )  );
            
            
            // recibo diferencia de tiempo de cliente a servidor
            cliente.Recv(   (char * ) &difTiempo  , sizeof( int )   );
              
            
            // encolo resultados de diferecia de tiempo cliente-servidor
            resultFinal.ip = cliente.getIp();
            resultFinal.difTiempo =  difTiempo;
            resultFinal.numCliente = cliAProcesar;
            
            pthread_mutex_lock( &SemColaResultados ); 
            
            colaResultados.push( resultFinal );
                 
            pthread_mutex_unlock( &SemColaResultados ); 
            
        } 
        else // si no hay clientes a procesar espero 2 segundos y vuelvo a intentar
            sleep ( 2 );
        
        
   }
    
    
    
    
}




// funcion que captura la señal de fin, cierra todos los hilos de procesamiento e informa los resultados obtenidos

void finalizar(int iNumSen, siginfo_t  * info, void *ni)
{
    
    // cierro correctamente los hilos de ejecucion y el socket
    pthread_cancel(proc1_t);
    pthread_cancel(proc2_t);
    pthread_cancel(proc3_t);
    pthread_cancel(proc4_t);
    pthread_cancel(proc5_t);

    
    Comunicacion.Close();
   
    sleep(1);
    
    system("reset");
    
    // Realizo el informe solicitado : muestro cola de resultados
    
    result auxResult;
    
    cout << endl << "Informe de resultados: " << endl  ;
    while ( !colaResultados.empty() ) 
    {
        auxResult = colaResultados.front();
        cout   << endl  << "Cliente numero: " << auxResult.numCliente << " con ip: " << auxResult.ip << "  obtuvo diferencia de tiempos de: " <<  auxResult.difTiempo;
        colaResultados.pop();
           
    }
    
    
    cout << endl << endl  << "El Servidor ha finalizado" << endl << endl ;
    exit (0);
}




