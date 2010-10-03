/************************************/
/* Nombre: Comm.h                   */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



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

template <class T>
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
              sizeof(struct sockaddr)) == -1 ){
        perror("error: Comm.h: Comm(long int port): bind()");

        if ( errno == EADDRINUSE ){
            int tr = 1;
            
            cout << "Trying to reuse the address...";
            
            if ( setsockopt( this->in_socket,
                             SOL_SOCKET,
                             SO_REUSEADDR,
                             &tr,
                             sizeof(int) ) == -1 ) {
                
                perror("error: Comm.h: Comm(long int port): setsockopt()");
                exit(1);
            
            }else{

                if ( bind(this->in_socket,
                          (struct sockaddr *)& this->in_address,
                          sizeof(struct sockaddr)) == -1 ){
                    perror("error: Comm.h: Comm(long int port): bind()");
                    exit(1);
                }
                else
                    cout << "Success!" << endl;
                
            }
        }
    }
    
}

template <class T>
Comm<T> :: ~Comm(){
   // close(in_socket);
}

template <class T>
int Comm<T> :: Listen(int backlog){
    int retVal = 0;

    retVal = listen(this->in_socket, backlog);

    if ( retVal == -1 )
        perror("error: Comm.h: Listen(int backlog): listen()");
    
    return retVal;
}


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

template <class T>
Connection<T> Comm<T> :: getConn(unsigned index){
    if ( index <= cons.size() )
        return cons[index];
    else
        return *(new Connection<T>);
}



template <class T>
void Comm<T> :: Close(){
    for( unsigned i = 0 ; i < cons.size() ; i++ )
        cons[i].Close();
        
    close(in_socket);
}




#endif


/*******/
/* FIN */
/*******/
