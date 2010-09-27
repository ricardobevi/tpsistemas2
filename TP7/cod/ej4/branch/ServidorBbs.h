#ifndef BBS_H
#define BBS_H

#include <vector>
#include <iostream>
#include <map>

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
	     
	     int ActivarServidor( string RutaDescarga, string RutaNovedades , int CantUsuarios );
	     string EsperarCliente();
	     
         Usuario& getUsuario(string Login);
         string getListaUsuarios();
         
         bool sendMessage( string Login,string sendTo,string Msg );
         
         void CloseUsuario(string Login);
         void Close();
     
  private:
	     string RutaDescarga;
	     string RutaNovedades;
	     map < string, Usuario >  Usuarios;
	     Comm<char> Socket;
  
};

ServidorBbs :: ServidorBbs(){
}

ServidorBbs :: ~ServidorBbs(){
}

int ServidorBbs :: ActivarServidor( string RutaDescarga, string RutaNovedades , int CantUsuarios ){
  this->RutaDescarga = RutaDescarga;
  this->RutaNovedades = RutaNovedades;
  this->Socket.Listen(CantUsuarios);
  
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

void ServidorBbs :: Close(){
    Socket.Close();
}

void ServidorBbs :: CloseUsuario(string Login){
    cout << Login <<  " cierra sesion."  << endl;
    
    Usuarios[Login].Close();
    Usuarios.erase(Login);
}


bool ServidorBbs :: sendMessage( string Login, string sendTo, string Msg )
{
    
    if( Usuarios.find( sendTo ) == Usuarios.end() )  return false;
            
    Login.append(" dice:");
    
    Usuarios[sendTo].sendString( Login );
    Usuarios[sendTo].sendString( Msg );
    
    return true;
    
}

#endif