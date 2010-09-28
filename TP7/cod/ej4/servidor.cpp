#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <fstream>

#include "ServidorBbs.h"

#define TAM_MAX_NOVEDAD 512

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
    
    // -------------------------------mensajes de bienvenida y modo de uso al servidor bbs-------------------------------
   
    string bienvenida;
            
    bienvenida= "MODO DE USO:\n\thora: Muestra la hora del servidor\n\tusuarios: Muestra los usuarios conectados al servidor";
     
    ServerBbs.getUsuario(Login).sendString( bienvenida );
    
    bienvenida="\tnovedades [nueva]: Muestra las novedades del servidor.\n\t\t\tEn caso de enviar el parametro opcional [nueva] este sera subido  como una nueva novedad";
    
    ServerBbs.getUsuario(Login).sendString( bienvenida );
    
    bienvenida= "\tmensaje destinatario msg: envia el mensaje 'msg' al destinatario especificado en el primer parametro 'destinatario' ";
    
    ServerBbs.getUsuario(Login).sendString( bienvenida );
    
    bienvenida= "\tlistar: lista archivos ubicados en el servidor\n\tdescargar: descarga el archivo especifico del servidor\n\t";
    
    ServerBbs.getUsuario(Login).sendString( bienvenida );
    
    // -------------------------------fin mensajes de bienvenida y modo de uso al servidor bbs----------------------------
    
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
           
        }else if (Command[0] == "novedades"){
            
                if ( Command.size() == 1 )
                {                    
                    ifstream archivoNovedadesSalida("./Novedades");
                    char novedad[TAM_MAX_NOVEDAD];
                    
                    archivoNovedadesSalida.getline( novedad ,  TAM_MAX_NOVEDAD , '\n');
                   
                    ServerBbs.getUsuario(Login).sendString(   "\nNovedades: \n"  );
                    while ( !archivoNovedadesSalida.eof() )
                    {    
                            ServerBbs.getUsuario(Login).sendString(  novedad  );
                            archivoNovedadesSalida.getline( novedad ,  TAM_MAX_NOVEDAD , '\n');
                    }
                    ServerBbs.getUsuario(Login).sendString(   "\n"  );
                    
                    archivoNovedadesSalida.close();
                    
                }
                else
                {
                    string nuevaNovedad;
                    for( unsigned i = 1 ; i < Command.size() ; i++ ) nuevaNovedad += Command[i] + " ";
                    
                    ofstream archivoNovedades;
                    archivoNovedades.open("./Novedades",fstream::app);
                     
                    cout << endl <<"Nueva novedad: " <<  nuevaNovedad << endl;
                    archivoNovedades  << nuevaNovedad << endl;
                    
                    archivoNovedades.close();
 
                }
                    
        }else if ( Command[0] == "listar"){

              string listaArchivos  =  ServerBbs.getListaArchivos();
              ServerBbs.getUsuario(Login).sendString(  listaArchivos  );

        }else if ( Command[0] == "descargar"){

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