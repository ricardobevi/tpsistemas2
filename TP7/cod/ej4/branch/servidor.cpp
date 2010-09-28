#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <signal.h>
#include <stdio.h>

#include "ServidorBbs.h"

void * sender(void * args);
void * recver(void * args);

void cTERM(int iNumSen, siginfo_t *info, void *ni);

ServidorBbs ServerBbs;

int main(int argc, const char *argv[]){
    vector<pthread_t> threads;

    struct sigaction term;

    term.sa_sigaction = cTERM;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGINT, &term, NULL);

    ServerBbs.ActivarServidor(argv[1], argv[2], atoi(argv[3]) );

    string Login;

    while(1){

        pthread_t newSender,
        newRecver;

        Login = ServerBbs.EsperarCliente();

        if ( Login != "" ){
        
            pthread_create( &newSender, NULL, sender, (void *) (&Login) );
            pthread_create( &newRecver, NULL, recver, (void *) (&Login) );

            threads.push_back(newSender);
            threads.push_back(newRecver);
        
        }else{
            cout << "Intento de ingreso erroneo." << endl;
        }
    }

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}

void * sender(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;

    ServerBbs.getUsuario(Login).sendString("Bienvenido al Servidor BBS R y F\n");
    
    return 0;
}

void * recver(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;
    
    vector<string> Command;
    
    Command = ServerBbs.getUsuario(Login).receiveCommand();
    
    while( Command[0] != "fin" ){
    
        cout << "Comando: " << Command[0] << endl;
        
        if (Command[0] == "hora"){
              time_t segundos;
              struct tm * hora;

              time ( &segundos );
              hora = localtime ( &segundos );
              ServerBbs.getUsuario(Login).sendString(  asctime (hora)  );
        
        }
        else if ( Command[0] == "usuarios"){
        
              string listaUsuarios  =  ServerBbs.getListaUsuarios();
              ServerBbs.getUsuario(Login).sendString(  listaUsuarios  );
           
        }
        else if ( Command[0] == "mensaje" ){
           
           if(Command.size() >= 3 ){
               string Msg;
               
               for( unsigned i = 2 ; i < Command.size() ; i++ ){
                   Msg += Command[i] + " ";
               }
               
               cout << "Enviando Mensaje..." << endl
                    << "De: " << Login << endl
                    << "A:  " << Command[1] << endl
                    << "Mensaje: " << Msg << endl;

               ServerBbs.sendMessage( Login, Command[1], Msg );
           }
        }else if ( Command[0] == "alist"){

              string listaArchivos  =  ServerBbs.getListaArchivos();
              ServerBbs.getUsuario(Login).sendString(  listaArchivos  );

        }else if ( Command[0] == "getfile"){

              if ( Command.size() >= 2 ){
                  ServerBbs.sendFile( Login, Command[1] );
              }

        }
        else{
            cout << "Comando desconocido: " << Command[0] << endl;
        }
        
        Command = ServerBbs.getUsuario(Login).receiveCommand();
        
    }
    
    ServerBbs.CloseUsuario(Login);
    
    return 0;
}


void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    cout << "Terminado Servidor..." << endl;
    
    ServerBbs.Close();
    
    exit(0);
}