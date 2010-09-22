#include <iostream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>

#include "Connection.h"

#define TAMLOGIN 50
#define TAM_COMMAND 128
#define TAM_STRING 512


void * sender(void * args);
void * recver(void * args);

Connection<char> sock;
char login[TAMLOGIN];

int main(int argc, const char *argv[]){
  pthread_t sender_t,
	        recver_t;
  
  sock.Connect( argv[1], atoi(argv[2]) );
  
  strncpy(login, argv[3], TAMLOGIN);
  
  pthread_create( &sender_t, NULL, sender, NULL);
  pthread_create( &recver_t, NULL, recver, NULL);
  
  pthread_join(sender_t, NULL);
  //pthread_join(recver_t, NULL);
  
  sock.Close();
  
  return 0;
}

void * sender(void * args){
   
    sock.Send(login, TAMLOGIN);
    
    char Command[TAM_COMMAND];
       
    
    do{
        
       cin.getline( Command,TAM_COMMAND,'\n');
       sock.Send(Command, TAM_COMMAND);
       
    }while(  strcmp(Command,"fin") != 0 );
    
    return 0;
}

void * recver(void * args){
    char cadena[ TAM_STRING ];
    
    while(1){
        sock.Recv(cadena, TAM_STRING );
        cout << cadena << endl;
    }
    
    return 0;
}