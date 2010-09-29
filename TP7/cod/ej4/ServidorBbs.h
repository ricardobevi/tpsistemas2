#ifndef BBS_H
#define BBS_H

#include <vector>
#include <iostream>
#include <map>
#include <sys/types.h>
#include <dirent.h>

#include "Usuario.h"
#include "Comm.h"


#define TAM_LOGIN 50
#define TAM_RETURN 1

using namespace std;

class ServidorBbs
{
  public:
	     ServidorBbs();
	     ~ServidorBbs();
	     
	     int ActivarServidor( string RutaDescarga, string RutaNovedades , int CantUsuarios ,int timeOut);
	     string EsperarCliente();
	     
         Usuario& getUsuario(string Login);
         string getListaUsuarios();
         string getListaArchivos();
         const char * getRutaNovedades();
         int    getTimeOut();
         
         
         bool sendMessage( string Login,string sendTo,string Msg );
         bool sendFile( string Login, string File, string saveTo );
         bool recvFile( string Login, string File );
         
         void timeoutSignal( );
         void sendCloseSignal(string Login);
         void CloseUsuario(string Login);
         void Close();
     
  private:
         int timeOut;
	     string RutaDescarga;
	     string RutaNovedades;
	     map < string, Usuario >  Usuarios;
	     Comm<char> Socket;
  
};

ServidorBbs :: ServidorBbs(){
}

ServidorBbs :: ~ServidorBbs(){
}

int ServidorBbs :: ActivarServidor( string RutaDescarga, string RutaNovedades , int CantUsuarios ,int timeOut){
  this->RutaNovedades = RutaNovedades;
  this->timeOut = timeOut * 60;
  this->Socket.Listen(CantUsuarios);

  if( RutaDescarga[ RutaDescarga.size() - 1 ] != '\\' &&
      RutaDescarga[ RutaDescarga.size() - 1 ] != '/' ) RutaDescarga += "/";

  this->RutaDescarga = RutaDescarga;
  
  return 0;
}

string ServidorBbs :: EsperarCliente(){
  unsigned numConnection = 0;
  char Login[TAM_LOGIN],
       ret;
  
  numConnection = this->Socket.Accept();
  
  Socket.getConn(numConnection).Recv(Login, TAM_LOGIN);
  
  if( Usuarios.find( Login ) == Usuarios.end() ){
      cout << Login << " inicia sesion." << endl;

      Usuario usr( Login, Socket.getConn(numConnection) );

      this->Usuarios[Login]= usr;

      ret = 1;
      
  }else{
      ret = 0;
      Login[0] = '\0';
  }

  Socket.getConn(numConnection).Send( &ret, TAM_RETURN);
  
  return string(Login);
  
}

Usuario& ServidorBbs :: getUsuario(string Login){
  return Usuarios[Login];
}

string ServidorBbs :: getListaUsuarios(){
 
    string ListaUsuarios;
    
     map < string, Usuario > :: iterator it;
    
    for( it= Usuarios.begin() ; it != Usuarios.end() ; it++){
      
        ListaUsuarios = ListaUsuarios + it->first;
        ListaUsuarios.push_back('\n');
    }
    
    return (ListaUsuarios);
}

string ServidorBbs :: getListaArchivos(){

    string ListaArchivos;

    DIR * directorio;

    struct dirent * entradaDir;

    directorio = opendir ( RutaDescarga.c_str() );

    while ( ( entradaDir = readdir ( directorio ) ) ){
        if ( entradaDir->d_type == DT_REG ){
            ListaArchivos += entradaDir->d_name;
            ListaArchivos += "\n";
        }
    }

    
    return (ListaArchivos);
}


bool ServidorBbs :: sendMessage( string Login, string sendTo, string Msg )
{
    
    if( Usuarios.find( sendTo ) == Usuarios.end() )  return false;
            
    Login.append(" dice:");
    
    Usuarios[sendTo].sendString( Login );
    Usuarios[sendTo].sendString( Msg );
    
    return true;
    
}

bool ServidorBbs :: sendFile( string Login, string File, string saveTo ){

    Usuarios[Login].sendFile(RutaDescarga + File, saveTo);

    return true;

}

bool ServidorBbs :: recvFile( string Login, string File ){

    Usuarios[Login].recvFile(RutaDescarga + File);

    return true;

}

void ServidorBbs :: sendCloseSignal(string Login){
    Usuarios[Login].sendCloseSignal();
}

void ServidorBbs :: CloseUsuario(string Login){
    cout << Login <<  " cierra sesion."  << endl;

    Usuarios[Login].Close();
    Usuarios.erase(Login);
}

void ServidorBbs :: Close(){
    cout << "Cerrando sesiones..." << endl;

    map < string, Usuario >::iterator it;
    
    for( it = Usuarios.begin() ; it != Usuarios.end() ; it++ )
        this->CloseUsuario( (*it).first );

    Socket.Close();
}

void ServidorBbs :: timeoutSignal( ){

    // si hay usuarios en el servidor comienzo a buscar cual esta fuera del timeout
    if ( Usuarios.size() > 0 )
    {
            map < string, Usuario >::iterator it  ;
            
            it = Usuarios.begin();
            
            for( it = Usuarios.begin() ; it != Usuarios.end() ; it++ )
            {
                    // para ver proceso de timeout descomentar la siguiente linea
                    //cout << endl << (it->first) << ":: Ultima op: " << (it->second).getLastOperation() << "   now: " <<   time(NULL) ;
                    //cout << "   Dif:  " <<    difftime(  time(NULL) , (it->second).getLastOperation() )  << " segundos";
                    
                    // si el tiempo inactivo en el servidor es mayor al timeout da de baja al login del usuario
                    if( difftime(  time(NULL) , (it->second).getLastOperation() ) >=  (double)  this->timeOut ){
                                // para ver proceso de timeout descomentar la siguiente linea
                                // cout <<endl  << (it->first) << " fue dado de baja "  <<endl;
                                this-> sendCloseSignal((*it).first);
                    }
            }
    }

}

int ServidorBbs :: getTimeOut(){
    
    return this->timeOut;
}

const char *  ServidorBbs :: getRutaNovedades(){

    return (this->RutaNovedades).c_str();
}

#endif