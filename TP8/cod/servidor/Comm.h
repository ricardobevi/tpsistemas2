/*
 * Clase Comm
 * Version: 0.2
 * Ultima revision: 19-10-2010
 *
 */

/* Descripcion:
 *
 * Esta clase se encarga del manejo de comuniaciones de parte del servidor.
 * Crea un socket que pone a la escucha de nuevas conexiones por parte de
 * clientes y guarda estas conexiones en un vector. Luego, se puede acceder
 * a cada una de las posiciones del vector para comunicar con un cliente
 * particular. Los clientes se manejan con objetos Connection los cuales son
 * bidireccionales e implementan los metodos para enviar y recibir datos.
 *
 * Hay un objeto Connection del lado del cliente y uno de lado del servidor,
 * de esta forma el cliente se comunica con el servidor a travez de este objeto
 * y viceversa.
 *
 */

#ifndef COMM_H
#define COMM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>

#include "Connection.h"


using namespace std;

template <class T = char>
class Comm{
    public:
        Comm(long int port = 50000);
        ~Comm();
        
        int Listen(int backlog = 10);
        unsigned Accept();
        
        Connection<T> getConn(unsigned index);
        
        void Close();
        
    private:
        int in_socket;
        struct sockaddr_in in_address;
        
        vector< Connection<T> > cons;
        
        long int port;
        
        unsigned short int listenPort;
        unsigned long int listenIpAddress;


};

/*
 * Comm(long int port)
 *
 * Contructor que crea un socket de escucha en el puerto pasado
 * por parametro.
 *
 * Recibe:
 * long int port        Puerto en el cual setear el socket de escucha.
 *
 */

template <class T>
Comm<T> :: Comm(long int port){
    this->port = port;
    
    this->in_socket = socket(AF_INET, SOCK_STREAM, 0);

    if ( this->in_socket == -1 )
        perror("error: Comm.h: Comm(long int port): socket()");
    
    bzero(&(this->in_address), sizeof(this->in_address));
    
    this->listenPort = htons(port);
    this->listenIpAddress = htonl(INADDR_ANY);
    
    this->in_address.sin_family = AF_INET;
    this->in_address.sin_port = this->listenPort; 
    this->in_address.sin_addr.s_addr = this->listenIpAddress;
    
    if ( bind(this->in_socket,
              (struct sockaddr *)& this->in_address,
              sizeof(struct sockaddr)) == -1 )
        perror("error: Comm.h: Comm(long int port): bind()");
    
}

/*
 * ~Comm()
 * Destructor de la clase, como no se pide memoria dinamicamente, este no
 * realiza ninguna accion.
 *
 */

template <class T>
Comm<T> :: ~Comm(){
   // close(in_socket);
}

/*
 * Listen(int backlog)
 *
 * Da la orden al socket para que empiece a escuchar conexiones entrantes.
 * Setea el socket en escucha.
 *
 * Recibe:
 * int backlog        Cantidad de conexiones que se pueden acumular en la cola
 *                    de espera.
 * Devuelve:
 * El valor de retorno de la funcion listen(). 0 en caso de exito, -1 en caso
 * de error.
 */

template <class T>
int Comm<T> :: Listen(int backlog){
    int retVal = 0;

    retVal = listen(this->in_socket, backlog);

    if ( retVal == -1 )
        perror("error: Comm.h: Listen(int backlog): listen()");
    
    return retVal;
}

/*
 * Accept()
 *
 * Acepta una nueva conexion de un cliente, crea un objeto Connection y lo
 * agrega al vector de conexiones.
 *
 * Devuelve:
 * El objeto Connection creado, este tiene toda la informacion de la conexion
 * asi como tambien metodos para enviar y recibir datos y/o archivos.
 */
template <class T>
unsigned Comm<T> :: Accept(){
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    int newCon = 0;
    
    bzero( &addr, sizeof(addr) );
    
    newCon = accept(this->in_socket, (struct sockaddr *)(& addr), &addrLen);

    if ( newCon == -1 ){
        perror("error: Comm.h: Accept(): accept()");
        return -1;
    }

    bzero(&(addr.sin_zero), 8);
    
    Connection<T> conn( newCon, addr );
    
    this->cons.push_back(conn);
    
    return this->cons.size() - 1;
}


/*
 * getConn(unsigned index)
 *
 * Devuelve un objeto conexion que este en una posicion del vector de
 * conexiones.
 *
 * Recibe:
 * unsigned index        El indice del vector de conexiones.
 *
 * Devuelve:
 * En caso de que el indice exista, retorna el objeto Connection en esa
 * posicion. Caso contrario, devuelve un objeto Connection vacio.
 */
template <class T>
Connection<T> Comm<T> :: getConn(unsigned index){
    if ( index <= cons.size() )
        return cons[index];
    else
        return *(new Connection<T>);
}


/*
 * Close()
 *
 * Cierra todas las conexiones al servidor y el socket de escucha del propio
 * servidor.
 */
template <class T>
void Comm<T> :: Close(){
    for( unsigned i = 0 ; i < cons.size() ; i++ )
        cons[i].Close();
        
    close(in_socket);
}




#endif

