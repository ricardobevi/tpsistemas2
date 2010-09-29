#ifndef USER_H
#define USER_H

#include <iostream>
#include <ctime>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <stdio.h>

#include "Connection.h"

#define TAM_COMMAND 128
#define TAM_STRING 512
#define TAM_RETURN 1

using namespace std;

class Usuario
{
  public:
	     Usuario(string login = "", Connection<char> socket = *(new  Connection<char>));
	     ~Usuario();
	     
	     vector<string> receiveCommand();
         void sendString(string str);
         void sendFile(string file, string saveTo);
         void recvFile( string saveTo );
	     
         string getLogin();
         string getIp();
	     time_t getLastOperation();
	     void   setLastOperation();

         void sendCloseSignal();
         void Close();
         
  private:
	     string Login;
	     Connection<char> Socket;
	     time_t LastOperation;
	     

};

Usuario :: Usuario(string login, Connection<char> socket){
  this->Login = login;
  this->Socket = socket;
}

Usuario :: ~Usuario(){

}

time_t Usuario :: getLastOperation(){
  return LastOperation;
}

void   Usuario :: setLastOperation(){
  this->LastOperation= time(NULL);
}

vector<string> Usuario :: receiveCommand(){
  vector<string>  Strings;
  
  string subStr;
  
  size_t startPos = 0,
         endPos = 0;
  
  // Recibo el comando.
  char Command[TAM_COMMAND];
   
  Socket.Recv(Command, TAM_COMMAND );
  
  //Divido la instruccion.
  string cmdStr(Command); 
  
  endPos = cmdStr.find(' ', startPos);

  subStr = cmdStr.substr(startPos, endPos - startPos);

  Strings.push_back(subStr);
   
  while(endPos != string::npos){

      startPos = endPos + 1;
      
      endPos = cmdStr.find(' ', startPos);

      subStr = cmdStr.substr(startPos, endPos - startPos);

      Strings.push_back(subStr);
      
  }
  
  return Strings;
  
}

void Usuario :: sendString(string str){

    //Envio la senal de cadena (string) al cliente.
    {
        char cmd = 1;
        Socket.Send(&cmd, TAM_RETURN);
    }

    char auxString [TAM_STRING];

    strncpy( auxString, str.c_str() ,  TAM_STRING );

    Socket.Send(  auxString ,  TAM_STRING);

}


void Usuario :: sendFile(string File, string saveTo){

    //Envio la senal de archivo al cliente.
    {
        char cmd = 2;
        Socket.Send(&cmd, TAM_RETURN);
        Socket.Send(  saveTo.c_str() ,  TAM_STRING);
    }
    
    Socket.SendFile(File.c_str());

}

void Usuario :: recvFile( string saveTo ){

    Socket.RecvFile(saveTo.c_str());
}

string Usuario :: getLogin(){
    return Login;
}

string Usuario :: getIp(){
    return Socket.getIp();
}

void Usuario :: sendCloseSignal(){
    //Envio la senal de cierre al cliente.
    {
        char cmd = 3;
        Socket.Send(&cmd, TAM_RETURN);
    }

}


void Usuario :: Close(){
    //Envio la senal de cierre al cliente.
    {
        char cmd = 3;
        Socket.Send(&cmd, TAM_RETURN);
    }
    
    Socket.Close();
}

#endif