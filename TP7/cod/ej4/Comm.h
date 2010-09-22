#ifndef COMM_H
#define COMM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    
    bzero(&(this->in_address), sizeof(this->in_address));
    
    this->listenPort = htons(port);
    this->listenIpAddress = htonl(INADDR_ANY);
    
    this->in_address.sin_family = AF_INET;
    this->in_address.sin_port = this->listenPort; 
    this->in_address.sin_addr.s_addr = this->listenIpAddress;
    
    bind(this->in_socket, (struct sockaddr *)& this->in_address, sizeof(struct sockaddr));
    
    
}

template <class T>
Comm<T> :: ~Comm(){
   // close(in_socket);
}

template <class T>
int Comm<T> :: Listen(int backlog){
    return listen(this->in_socket, backlog);
}


template <class T>
unsigned Comm<T> :: Accept(){
    struct sockaddr_in addr;
    socklen_t addrLen;
    int newCon = 0;
    
    bzero( &addr, sizeof(addr) );
    
    newCon = accept(this->in_socket, (struct sockaddr *)(& addr), &addrLen);
    
    Connection<T> conn( newCon, addr );
    
    this->cons.push_back(conn);
    
    return this->cons.size() - 1;
}

template <class T>
Connection<T> Comm<T> :: getConn(unsigned index){
    return cons[index];
}



template <class T>
void Comm<T> :: Close(){
    for( unsigned i = 0 ; i < cons.size() ; i++ )
        cons[i].Close();
        
    close(in_socket);
}




#endif