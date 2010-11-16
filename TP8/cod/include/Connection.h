/*
 * Clase Connection
 * Version: 0.2
 * Ultima revision: 4-11-2010
 *
 */

/* Descripcion:
 *
 * Esta clase implementa todos los metodos necesarios para una comunicacion
 * bidireccional con el otro extremo (de cliente a servidor o viceversa).
 *
 */

#ifndef CONNECTION_H
#define CONNECTION_H


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

/*Tamano maximo del buffer para enviar archivos*/
#define MAX_BUFFER 65535

/*
 * Para asegurar la compatibilidad entre sistemas de
 * 32 y 64 bits aca se especifica el tipo de dato en
 * donde guardar el tamanio del archivo.
 * Con "unsigned int" deberia haber compatibilidad entre
 * ambas plataformas.
 */
#define INT_TYPE unsigned int


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
        bool isLocal();
        
        void Close();
        
    private:
        int io_socket;
        struct sockaddr_in in_sock;

        struct fileLen {
            INT_TYPE tam,
                     blockSize;
        };

        unsigned short int listenPort;
        unsigned long int listenIpAddress;

        string Host;


};


/*
 * Connection(string addr, long int port)
 *
 * Crea un socket que se conectara a la direccion y puerto especificados
 * por parametro.
 *
 * Recibe:
 * string addr        Direccion ip del host.
 * long int port      Puerto del host al cual se conectara.
 */
template <class T>
Connection<T> :: Connection(string addr, long int port){

    this->listenPort = htons(port);
    this->Host = addr;
    struct hostent *he = gethostbyname(addr.c_str());

    if ( he == NULL ){
        perror("[FATAL] error: Connection.h: Connection(string addr, long int port): gethostbyname()");
    }else{

        this->in_sock.sin_family = AF_INET;
        this->in_sock.sin_port = this->listenPort;
        memcpy(&this->in_sock.sin_addr.s_addr, he->h_addr_list[0], he->h_length);

        bzero(&(this->in_sock.sin_zero), 8);

    }
    
}

/*
 * Connection( int io_socket, struct sockaddr_in in_sock )
 *
 * Crea el objeto con un identificador de socket y una estructura de conexion
 * externas a la clase. Este constructor es el usado en el servidor, donde el
 * la funcion accept() ya devuelve un descriptor de archivo para un socket
 * bidireccional.
 *
 * Recibe:
 * int io_socket                 Descriptor de archivo del socket bidireccional.
 * 
 * struct sockaddr_in in_sock    Estructura sockaddr_in con los datos de la
 *                               conexion.
 */
template <class T>
Connection<T> :: Connection( int io_socket, struct sockaddr_in in_sock ){
    this->io_socket = io_socket;
    this->in_sock = in_sock;
}



/*
 * ~Connection()
 * Destructor de la clase, como no se pide memoria dinamicamente, este no
 * realiza ninguna accion.
 *
 */
template <class T>
Connection<T> :: ~Connection(){
    //close(io_socket);
}


/*
 * Connect()
 *
 * Conecta al host remoto.
 *
 * Devuelve:
 * 0 en caso de exito, -1 en caso de error.
 */
template <class T>
int Connection<T> :: Connect(){
    int connectReturn = 0;
    
    this->io_socket = socket(AF_INET, SOCK_STREAM, 0);

	if ( this->io_socket == -1 )
		perror("error: Connection.h: Connection(string addr, long int port): socket()");

    connectReturn = connect(this->io_socket, (struct sockaddr *) (& (this->in_sock)), sizeof(struct sockaddr));

    if ( connectReturn == -1 )
        perror("error: Connection.h: Connect(): connect()");
    
    return connectReturn;
}

template <class T>
int Connection<T> :: Connect(string addr, long int port){
    int connectReturn = 0;
    
    this->Host = addr;

    this->io_socket = socket(AF_INET, SOCK_STREAM, 0);

    if ( this->io_socket == -1 ){
        perror("error: Connection.h: Connect(string addr, long int port): socket()");
        return this->io_socket;
    }
    
    this->listenPort = htons(port);
    struct hostent *he = gethostbyname(addr.c_str());
    
    if ( he == NULL ){
        perror("[FATAL] error: Connection.h: Connect(string addr, long int port): gethostbyname()");
        return -1;
    }

    this->in_sock.sin_family = AF_INET;
    this->in_sock.sin_port = this->listenPort; 
    memcpy(&this->in_sock.sin_addr.s_addr, he->h_addr_list[0], he->h_length);
    
    bzero(&(this->in_sock.sin_zero), 8);
    
    connectReturn = connect(this->io_socket, (struct sockaddr *) (& (this->in_sock)), sizeof(struct sockaddr));

    if ( connectReturn == -1 )
        perror("error: Connection.h: Connect(string addr, long int port): connect()");
    
    return connectReturn;
}

template <class T>
ssize_t Connection<T> :: Send( const T* buf, size_t size ) const{
    ssize_t sendSize = 0;
    
    sendSize = send( io_socket, buf, size, 0 );

    while( (unsigned) sendSize <  (unsigned) size && sendSize != -1 )
        sendSize += send( io_socket, buf + sendSize, size - sendSize, 0 );

    if ( sendSize == -1 )
        perror("error: Connection.h: Send( const T* buf, size_t size ): send()");
    
    return sendSize;
}

template <class T>
ssize_t Connection<T> :: Recv( T* buf, size_t size ){
    ssize_t recvSize = 0;
    
    bzero(buf, size);
    
    recvSize = recv( io_socket, buf, size, 0 );

    while( (unsigned) recvSize <  (unsigned) size && recvSize != -1 )
        recvSize += recv( io_socket, buf + recvSize, size - recvSize, 0 );

    if ( recvSize == -1 )
        perror("error: Connection.h: Recv( T* buf, size_t size ): recv()");
       
    return recvSize;
}

template <class T>
ssize_t Connection<T> :: SendFile( string fileToSend ) const{
    INT_TYPE tam = 0,
             blockSize = 1;
             
    ssize_t enviado = 0;
    char * buffer;

    struct fileLen fileLenAndBS;

    ifstream is;
    is.open (fileToSend.c_str(), ios::binary );

    /*Veo el tamano del archivo*/
    is.seekg (0, ios::end);
    tam = is.tellg();
    is.seekg (0, ios::beg);


    blockSize = tam > MAX_BUFFER ? MAX_BUFFER : tam;

    while( ( tam % blockSize ) != 0 )
        blockSize--;

    fileLenAndBS.tam = tam;
    fileLenAndBS.blockSize = blockSize;

    this->Send( (char*) &(fileLenAndBS), sizeof(struct fileLen) );

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
    INT_TYPE tam = 0,
             blockSize = 1;
                      
    size_t recibido = 0;
    char * buffer;
    
    struct fileLen fileLenAndBS;

    
    ofstream os;
    os.open (fileToSave.c_str(), ios::binary );

    this->Recv( (char*) &(fileLenAndBS), sizeof(struct fileLen) );

    tam = fileLenAndBS.tam;
    blockSize = fileLenAndBS.blockSize;

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
bool Connection<T> :: isLocal(){
	bool local = false;

	char hostname[128];
	struct hostent *he;
	struct in_addr **addrs;

	for(int i = 0; this->Host[i] != '\0'; i++){
		this->Host[i] = tolower(this->Host[i]);
	}

	gethostname(hostname, sizeof hostname);

	string hname(hostname);

	hname += ".local";

	he = gethostbyname( hname.c_str() );

	addrs = (struct in_addr **) he->h_addr_list;

	cout << "Host = " << this->Host << endl;

	if ( this->Host == "localhost" ||
		 this->Host == "127.0.0.1" ||
		 this->Host == hostname    ||
		 this->Host == inet_ntoa(*addrs[0]) )
		local = true;

	return local;

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
