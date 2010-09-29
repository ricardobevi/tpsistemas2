/************************************/
/* Nombre: Connection.h             */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#ifndef CONNECTION_H
#define CONNECTION_H


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <string>

#define MAX_BUFFER 65535


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

        ssize_t SendFile( string fileToSend ) const;
        ssize_t RecvFile( string fileToSave );

        Connection<T> operator=( const Connection<T> & obj);

        string getIp();
        
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
ssize_t Connection<T> :: SendFile( string fileToSend ) const{
    unsigned long int tam = 0;
    ssize_t enviado = 0;
    size_t blockSize = 1;
    char * buffer;

    ifstream is;
    is.open (fileToSend.c_str(), ios::binary );

    /*Veo el tamano del archivo*/
    is.seekg (0, ios::end);
    tam = is.tellg();
    is.seekg (0, ios::beg);

    {
        size_t tamULInt = sizeof(unsigned long int);
        blockSize = tam > MAX_BUFFER ? MAX_BUFFER : tam;

        this->Send( (char*) &(tam), tamULInt );

        while( ( tam % blockSize ) != 0 )
            blockSize--;

        this->Send( (char*) &(blockSize), sizeof(size_t) );
    }

    buffer = new char[blockSize];
    
    is.read (buffer, blockSize);
    
    while(! is.eof() ){
        enviado += this->Send( buffer,  blockSize );

        is.read (buffer, blockSize);

    }

    is.close();

    delete [] buffer;
    
    return enviado;
}

template <class T>
ssize_t Connection<T> :: RecvFile( string fileToSave ){
    unsigned long int tam = 0,
                      recibido = 0;
                      
    size_t blockSize = 1;
    char * buffer;
    
    ofstream os;
    os.open (fileToSave.c_str(), ios::binary );

    {
        size_t tamULInt = sizeof(unsigned long int);

        this->Recv( (char*) &(tam), tamULInt );

        this->Recv( (char*) &(blockSize), sizeof(size_t) );
    }

    buffer = new char[blockSize];

    recibido += this->Recv(buffer, blockSize);
    os.write(buffer, blockSize);

    while( recibido < tam ){
        recibido += this->Recv(buffer, blockSize);
        os.write(buffer, blockSize);
    }

    os.close();

    delete [] buffer;

    return (ssize_t)recibido;
}

template <class T>
string Connection<T> :: getIp(){
    return string( inet_ntoa( this->in_sock.sin_addr ) );
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


/*******/
/* FIN */
/*******/