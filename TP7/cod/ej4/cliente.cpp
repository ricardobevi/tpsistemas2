#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdio.h>

#include "Connection.h"

#define TAM_LOGIN 50
#define TAM_RETURN 1

#define TAM_COMMAND 128
#define TAM_STRING 512


void * sender(void * args);
void * recver(void * args);

Connection<char> sock;
char login[TAM_LOGIN];

using namespace std;

int main(int argc, const char *argv[]){
    pthread_t sender_t,
            recver_t;

    sock.Connect( argv[1], atoi(argv[2]) );

    strncpy(login, argv[3], TAM_LOGIN);

    //Logueo
    
    char ret;

    sock.Send(login, TAM_LOGIN);

    sock.Recv(&ret, TAM_RETURN);

    if( ret == 1 ){
        //Una vez que me logueo empiezo los hilos de transmicion y recepcion.
        pthread_create( &sender_t, NULL, sender, NULL);
        pthread_create( &recver_t, NULL, recver, NULL);
        
        pthread_join(sender_t, NULL);
        //pthread_join(recver_t, NULL);
    }else{
        cout << "Ingreso Erroneo, el usuario ya existe." << endl;
    }

    return 0;
}

void * sender(void * args){
    
    
    char Command[TAM_COMMAND];

    do{

       cout << "-> ";
       cin.getline( Command,TAM_COMMAND,'\n');
       sock.Send(Command, TAM_COMMAND);

       /*Demaciado bruto, lo se, pero efectivo y rapido.*/

       if ( Command[0] == 's' &&
            Command[1] == 'u' &&
            Command[2] == 'b' &&
            Command[3] == 'i' &&
            Command[4] == 'r'){

           cout << "Enviando Archivo...";
       
           sock.SendFile(&Command[6]);

           cout << "Enviado!" << endl;
           
       }
    }while(  strcmp(Command,"fin") != 0 );

    sock.Close();
    
    return 0;
}

void * recver(void * args){
    char cmd,
         cadena[ TAM_STRING ];

    sock.Recv(&cmd, TAM_RETURN );
    
    while(cmd != 3){
        switch( cmd ){
            
            case 1: //Recibe Cadena
                sock.Recv(cadena, TAM_STRING );
                cout << cadena << endl;
                break;
                
            case 2: //Recibe Archivo
                char saveTo[TAM_STRING];
                sock.Recv(saveTo, TAM_STRING );
                
                sock.RecvFile(saveTo);
                
                break;

            default:
                cout << "Orden desconocida." << endl;
                
        }

        sock.Recv(&cmd, TAM_RETURN );
    }

    sock.Send("fin", TAM_COMMAND);

    cout << "El servidor se ha desconectado." << endl;

    exit(0);
    
    return 0;
}