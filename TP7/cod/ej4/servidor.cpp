#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <string>

#include "ServidorBbs.h"

void * sender(void * args);
void * recver(void * args);

ServidorBbs ServerBbs;

int main(int argc, const char *argv[]){
  vector<pthread_t> threads;
   
  
  ServerBbs.ActivarServidor(argv[1], argv[2], atoi(argv[3]) );
  
  string Login;
  
  while(1){
    
    pthread_t newSender,
              newRecver;
    
    Login = ServerBbs.EsperarCliente();
    
    pthread_create( &newSender, NULL, sender, (void *) (&Login) );
    pthread_create( &newRecver, NULL, recver, (void *) (&Login) );
    
    threads.push_back(newSender);
    threads.push_back(newRecver);
  }
  
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  
  return 0;
}

void * sender(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;
    
    return 0;
}

void * recver(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;
    string Command;
    
    
    Command = ServerBbs.getUsuario(Login).receiveCommand();
    
    while( Command != "fin" ){
    
        cout << "Comando: " << Command << endl;
        
        if (Command == "hora"){
              time_t segundos;
              struct tm * hora;

              time ( &segundos );
              hora = localtime ( &segundos );
              ServerBbs.getUsuario(Login).sendString(  asctime (hora)  );
        
        }
        else if ( Command == "usuarios"){
        
              string listaUsuarios  =  ServerBbs.getListaUsuarios();
              ServerBbs.getUsuario(Login).sendString(  listaUsuarios  );
           
        }
        else if ( Command.substr(0,7) == "mensaje" ){
           
           string mensaje = Command.substr( 7, Command.size() -1 );
           
           unsigned pos = Command.find_first_of(' ',0) -1;
           cout << "Pos:" << pos << endl;
           
           string sendTo = Command.substr(7,pos);
           cout << "Enviar a:" << sendTo<< endl;
           
           pos++;
           string Msg = Command.substr(pos,Command.size()-1 );
           cout << "Mensaje:" << Msg<< endl; 
           
           //sendMessage(Login,sendTo,Msg );
           
        }
        else{
            
            cout << "Comando desconocido: " << Command << endl;
        }
        
        Command = ServerBbs.getUsuario(Login ).receiveCommand();
        
    }
    cout << "Cierra Sesion: " << Login  << endl;
    
    ServerBbs.CloseUsuario(Login);
    
    return 0;
}