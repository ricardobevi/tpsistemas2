#ifndef CONNECTION_H
#define CONNECTION_H


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <string>


using namespace std;

template <class T>
class Connection{
    public:
        Connection(string addr = "127.0.0.1", long int port = 50000);
        Connection( int io_socket, struct sockaddr_in in_sock );
        ~Connection();
                
        int Connect();
        int Connect(string addr, long int port);
        
        
        ssize_t Send( const T* buf, size_t size ) const;
        ssize_t Recv( T* buf, size_t size );

	Connection<T> operator=( const Connection<T> & obj);
        
        void Close();
        
    private:
        int io_socket;
        struct sockaddr_in in_sock;

        unsigned short int listenPort;
        unsigned long int listenIpAddress;


};

template <class T>
Connection<T> :: Connection(string addr, long int port){
    this->io_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    this->listenPort = htons(port);
    this->listenIpAddress = inet_addr(addr.c_str());
    
    this->in_sock.sin_family = AF_INET;
    this->in_sock.sin_port = this->listenPort; 
    this->in_sock.sin_addr.s_addr = this->listenIpAddress;
    
    bzero(&(this->in_sock.sin_zero), 8);

    
}

template <class T>
Connection<T> :: Connection( int io_socket, struct sockaddr_in in_sock ){
    this->io_socket = io_socket;
    this->in_sock = in_sock;
}

template <class T>
Connection<T> :: ~Connection(){
    //close(io_socket);
}

template <class T>
int Connection<T> :: Connect(){
    int connectReturn = 0;
    
    connectReturn = connect(this->io_socket, (struct sockaddr *) (& (this->in_sock)), sizeof(struct sockaddr));
    
    return connectReturn;
}

template <class T>
int Connection<T> :: Connect(string addr, long int port){
    int connectReturn = 0;
    
    this->io_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    this->listenPort = htons(port);
    this->listenIpAddress = inet_addr(addr.c_str());
    
    this->in_sock.sin_family = AF_INET;
    this->in_sock.sin_port = this->listenPort; 
    this->in_sock.sin_addr.s_addr = this->listenIpAddress;
    
    bzero(&(this->in_sock.sin_zero), 8);
    
    connectReturn = connect(this->io_socket, (struct sockaddr *) (& (this->in_sock)), sizeof(struct sockaddr));
    
    return connectReturn;
}

template <class T>
ssize_t Connection<T> :: Send( const T* buf, size_t size ) const{
    ssize_t sendSize = 0;
    
    sendSize = send( io_socket, buf, size, 0 );
    
    return sendSize;
}

template <class T>
ssize_t Connection<T> :: Recv( T* buf, size_t size ){
    ssize_t recvSize = 0;
    
    bzero(buf, size);
    
    recvSize = recv( io_socket, buf, size, 0 );
       
    return recvSize;
}

template <class T>
void Connection<T> :: Close(){
    close(io_socket);
}


template <class T>
Connection<T> Connection<T> :: operator=( const Connection<T> & obj){
    
    this->io_socket = obj.io_socket;
    this->in_sock = obj.in_sock;
    this->listenPort = obj.listenPort;
    this->listenIpAddress = obj.listenIpAddress;
    
    return *this;
    
}


#endif