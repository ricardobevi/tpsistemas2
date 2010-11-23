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
#include <time.h>
#include <stdlib.h>

#include "Connection.h"

#define PORT 60000
#define IP_SERVIDOR "localhost"


int main(int argc, const char *argv[]){
    
    cout << endl << "Procesando datos ................. " ; 
    
    Connection <char> conneccionAServidor;
    
    time_t tiempoServ ,tiempoCliente;
    int difTiempo;
    
    
    conneccionAServidor.Connect( IP_SERVIDOR ,  PORT );
    
    
    conneccionAServidor.Recv(    (char * ) &tiempoServ       ,  sizeof( time_t ) );
    
    time ( &tiempoCliente );
    
    difTiempo = tiempoCliente - tiempoServ ;
    
    conneccionAServidor.Send(    (const char * ) &difTiempo  ,  sizeof( int )    );
    
    cout <<  "[OK]" <<endl ;
    
    cout << endl << "El cliente ya envio los datos y ha finalizado" << endl;
    
}












