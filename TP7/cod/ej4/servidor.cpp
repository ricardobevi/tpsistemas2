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
void * timer(void * args);
void * sendFile(void * args);

void cTERM(int iNumSen, siginfo_t *info, void *ni);

ServidorBbs ServerBbs;

int main(int argc, const char *argv[]){
    vector<pthread_t> threads;
    pthread_t tTimer;
    int timeOut = 10;

    struct sigaction term;

    term.sa_sigaction = cTERM;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGINT, &term, NULL);

    ServerBbs.ActivarServidor(argv[1], argv[2], atoi(argv[3]) );

    pthread_create( &tTimer, NULL, timer, (void *) (&timeOut) );

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
    
    return 0;
}

void * recver(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;
    vector<string> Command;
    pthread_t sendFileT;
    
    Command = ServerBbs.getUsuario(Login).receiveCommand();
  
    while( Command[0] != "fin" ){

        ServerBbs.getUsuario(Login).setLastOperation();
        
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
                  string args[3];

                  args[0] = Login;
                  args[1] = Command[1];
                  args[2] = Command.size() == 2 ? Command[1] : Command[2];
                  
                  pthread_create( &sendFileT, NULL, sendFile, (void *) (&args) );
              }

        }else if ( Command[0] == "subir"){

              if ( Command.size() >= 2 ){
                  /* O se reciben archivos, o se reciben comandos.
                   * Es por eso que se hace en el mismo thread.
                   */
                  cout << "Recibiendo " << Command[1] << " de " << Login << endl;

                  ServerBbs.recvFile( Login, Command[1] );

                  cout << "Fin de recepcion de " << Command[1] << " de " << Login << endl;
                  
              }

        }else{
            cout << "Comando desconocido: " << Command[0] << endl;
        }
        
        Command = ServerBbs.getUsuario(Login).receiveCommand();
        
    }
    
    ServerBbs.CloseUsuario(Login);
    
    return 0;
}

void * sendFile(void * args){
    string * argv = (string *) args;

    cout << "Enviando " << argv[1] << " a " << argv[0] << endl;

    ServerBbs.sendFile( argv[0], argv[1], argv[2] );

    cout << "Fin de envio de " << argv[1] << " a " << argv[0] << endl;

    return 0;
}


void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    cout << "Terminado Servidor..." << endl;
    
    ServerBbs.Close();
    
    exit(0);
}

void * timer(void * args){
    int * timeOutP = (int *) args;
    int timeOut = *timeOutP;

    while(1){
        sleep(timeOut);
        //TODO: leer el vector de usuarios y ver cual exedio el tiempo.
        //ServerBbs.sendCloseSignal("Ricky");
    }

    return 0;
}