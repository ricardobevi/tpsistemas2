#ifndef USER_H
#define USER_H

#include <ctime>
#include <string>
#include <cstring>
#include <vector>

#include "Connection.h"

#define TAM_COMMAND 128
#define TAM_STRING 512

using namespace std;

class Usuario
{
  public:
	     Usuario(string login = "", Connection<char> socket = *(new  Connection<char>));
	     ~Usuario();
	     
	     //send
	     //receive
	     
	     //send archivo
	     //receive archivo
	     
	     vector<string> receiveCommand();
         void sendString(string str);
	     
         string getLogin();
	     time_t getLastOperation();
	     void   setLastOperation();
	     
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
  
  endPos = cmdStr.find_first_of(' ', startPos);
   
  while(endPos != string::npos){
      
      subStr = cmdStr.substr(startPos, endPos); //Resto 1 para ignorar el espacio.
      
      Strings.push_back(subStr);
      
      startPos = endPos + 1; //Sumo 1 para ignorar el espacio.
      
      endPos = cmdStr.find_first_of(' ', startPos);
      
  }
  //Agrego la ultima (o la unica) cadena.
  subStr = cmdStr.substr(startPos);
  
  Strings.push_back(subStr);
  
  return Strings;
  
}

void Usuario :: sendString(string str){
  
  char auxString [TAM_STRING];
  
  strncpy( auxString, str.c_str() ,  TAM_STRING );
  
  Socket.Send(  auxString ,  TAM_STRING);
  
}

string Usuario :: getLogin(){
    return Login;
}

void Usuario :: Close(){
    Socket.Close();
}

#endif