#ifndef BBS_H
#define BBS_H

#include <vector>
#include <iostream>
#include <map>

#include "Usuario.h"
#include "Comm.h"


#define TAMLOGIN 50

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
  char Login[TAMLOGIN];
  
  numConnection = this->Socket.Accept();
  
  Socket.getConn(numConnection).Recv(Login, TAMLOGIN);
  
  // validar si ya existe this->Usuarios[login] mediante saludo de tres vias
  
  cout << "Login = " << Login << endl;
  
  Usuario usr( Login, Socket.getConn(numConnection) );
 
  
  this->Usuarios[Login]= usr;
  
  return (Login);
  
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
    Usuarios[Login].Close();
    Usuarios.erase(Login);
}

bool sendMessage( string Login,string sendTo,string Msg )
{
    map < string, Usuario > :: iterator end = Usuarios.end();
    if (  Usuarios.find(sendTo) == (end++))  return false;
            
    Login.push_back(":");
    
    Usuarios[sendTo].sendString( Login );
    Usuarios[sendTo].sendString( Msg );
    
    return true;
    
}

#endif