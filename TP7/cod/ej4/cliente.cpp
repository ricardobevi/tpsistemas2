/************************************/
/* Nombre: cliente.cpp              */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>

#include "Usuario.h"

#define TAM_LOGIN 50
#define TAM_RETURN 1

#define TAM_COMMAND 128

void help(void);
void * sender(void * args);
void * recver(void * args);


int validacion(const char* IP_SERVIDOR ,int PUERTO_DE_ENLACE,const char*  NOMBRE_DE_USUARIO);

Connection<char> sock;
char login[TAM_LOGIN];

pthread_mutex_t screenMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t screenMutex2 = PTHREAD_MUTEX_INITIALIZER;

using namespace std;

int main(int argc, const char *argv[]){
    pthread_t sender_t,
              recver_t;

    if ( strcmp(argv[1], "--help") == 0  || validacion( argv[1] , atoi(argv[2]) , argv[3] )  )
    {
        help();
        exit(1);
    }
        
    sock.Connect( argv[1], atoi(argv[2]) );

    strncpy(login, argv[3], TAM_LOGIN);

    //Logueo
    
    char ret;

    sock.Send(login, TAM_LOGIN);

    sock.Recv(&ret, TAM_RETURN);

    if( ret == 1 ){

        pthread_mutex_lock(&screenMutex1);
        
        //Una vez que me logueo empiezo los hilos de transmicion y recepcion.
        pthread_create( &recver_t, NULL, recver, NULL);
        pthread_create( &sender_t, NULL, sender, NULL);
        
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
       
       pthread_mutex_lock(&screenMutex1);       

       cout << "-> ";
       
       pthread_mutex_unlock(&screenMutex2);
       
       cin.getline( Command,TAM_COMMAND,'\n');

              
       if ( strcmp( Command, "" ) != 0 )
           sock.Send(Command, TAM_COMMAND);
           
       if ( Command[0] == 's' && /*Demaciado bruto, lo se, pero efectivo y rapido.*/
            Command[1] == 'u' &&
            Command[2] == 'b' &&
            Command[3] == 'i' &&
            Command[4] == 'r'){

           cout << "Enviando Archivo...";
       
           sock.SendFile(&Command[6]);

           cout << "Enviado!" << endl;

           pthread_mutex_unlock(&screenMutex1);
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
                {
                    sock.Recv(cadena, TAM_STRING );

                    pthread_mutex_lock(&screenMutex2);
                    cout << cadena << endl;
                    pthread_mutex_unlock(&screenMutex1);
                }
                break;
                
            case 2: //Recibe Archivo
                {
                    char saveTo[TAM_STRING];

                    pthread_mutex_lock(&screenMutex2);
                    
                    sock.Recv(saveTo, TAM_STRING );

                    cout << endl << "Recibiendo " << saveTo << "...";
                    
                    sock.RecvFile(saveTo);

                    cout << "OK!" << endl;

                    pthread_mutex_unlock(&screenMutex1);

                 }
                 break;
                
            default:
                cerr << "[" << cmd << "] " << "Orden desconocida." << endl;
                
        }

        sock.Recv(&cmd, TAM_RETURN );
    }

    sock.Send("fin", TAM_COMMAND);

    cout << "El servidor se ha desconectado." << endl;

    exit(0);
    
    return 0;
}

void help(void){
    
    cout << "MODO DE USO:" << endl << "\t\t ./cli    IP_SERVIDOR    PUERTO_DE_ENLACE   NOMBRE_DE_USUARIO" << endl << endl;
    cout << "IP_SERVIDOR:  ip del servidor (si el servidor esta en el mismo computador 127.0.0.1)"<< endl<< endl;
    cout << "PUERTO_DE_ENLACE:  puerto del computador por el cual el servidor recivira y enviara peticiones y datos (parametro a enviar por defecto 50000)"<< endl<< endl;
    cout << "NOMBRE_DE_USUARIO:  nombre del usuario que desea iniciar servicio ( puede denegarce en caso de existencia de duplicados de este login )"<< endl<< endl;
    
    
}

int validacion( const char* IP_SERVIDOR ,int PUERTO_DE_ENLACE,const char*  NOMBRE_DE_USUARIO)
{
    int error = 0;
    
    /* 
    if(  PUERTO_DE_ENLACE != 50000){
     
        cout << endl << "ERROR: el puerto debe ser 50000"  <<endl;
        error = 1;
    }
    */
        
    if( strlen(NOMBRE_DE_USUARIO) > TAM_LOGIN ){
     
        cout << endl << "ERROR: tamaño de login superior a " <<  TAM_LOGIN   << " caracteres"<<endl;
        error = 1;
    }
    
    cout << endl;
    return error;
    
}


/*******/
/* FIN */
/*******/
