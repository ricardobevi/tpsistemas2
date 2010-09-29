/************************************/
/* Nombre: servidor.cpp             */
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
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <unistd.h>

#include "ServidorBbs.h"

#define TAM_MAX_NOVEDAD 512

void * sender(void * args);
void * recver(void * args);
void * timer( void * args);
void * sendFile(void * args);

void cTERM(int iNumSen, siginfo_t *info, void *ni);

void help(void);
int validacion( const char* DESCARGAS ,const char* NOVEDADES,int CANT_USER, int TIMEOUT);

ServidorBbs ServerBbs;

pthread_mutex_t downQueueMutex = PTHREAD_MUTEX_INITIALIZER;
map< string, queue<string*> > downQueue;


int main(int argc, const char *argv[]){
    
    if ( validacion( argv[1], argv[2], atoi(argv[3]) ,atoi(argv[4]) ) ||  strcmp(argv[1], "--help") == 0   )
    {
          help();
          exit(1);
    }
        
    vector<pthread_t> threads;
    pthread_t tTimer;

    struct sigaction term;

    term.sa_sigaction = cTERM;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGINT, &term, NULL);

    ServerBbs.ActivarServidor(argv[1], argv[2], atoi(argv[3]) ,atoi(argv[4]) );

    pthread_create( &tTimer, NULL, timer, (void * ) NULL );

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
    
    // Establece hora de inicio del cliente como su ultima operacion
    ServerBbs.getUsuario(Login).setLastOperation();

    // -------------------------------mensajes de bienvenida y modo de uso al servidor bbs-------------------------------

    string bienvenida;

    bienvenida= "MODO DE USO:\n"
                "    hora : Muestra la hora del servidor.\n"
                "    usuarios : Muestra los usuarios conectados al servidor.\n"
                "    novedades [nueva novedad] : Muestra (o agrega) las novedades del servidor.\n"
                "    mensaje destinatario msg : envia el mensaje 'msg' al destinatario especificado\n"
                "                               en el primer parametro 'destinatario.'\n"
                "    listar : lista archivos ubicados en el servidor.\n"
                "    descargar archivo_a_descargar [ruta_donde_guardarlo]: descarga el archivo especifico del servidor.\n"
                "    subir archivo_a_subir : Sube un archivo al servidor.";

    ServerBbs.getUsuario(Login).sendString( bienvenida );

    // -------------------------------fin mensajes de bienvenida y modo de uso al servidor bbs----------------------------
    
    return 0;
}

void * recver(void * args){
    string * usrPtr = (string *) args;
    string Login = *usrPtr;
    vector<string> Command;
    pthread_t sendFileT;
    string ip = ServerBbs.getUsuario(Login).getIp();
    
    Command = ServerBbs.getUsuario(Login).receiveCommand();
  
    while( Command[0] != "fin" ){

        // setea el tiempo de ultima operacion realizada para procesar el timeout
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
                    ifstream archivoNovedadesSalida( ServerBbs.getRutaNovedades()  );
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
                    archivoNovedades.open( ServerBbs.getRutaNovedades() ,fstream::app);
                     
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

                  //El login
                  args[0] = Login;

                  //El archivo a mandar
                  args[1] = Command[1];

                  //La ruta a donde guardarlo
                  args[2] = Command.size() == 2 ? Command[1] : Command[2];

                  pthread_mutex_lock(&downQueueMutex);
                  
                  if ( downQueue.find( ip ) == downQueue.end() ){
                      cout << "IP: " << ip << endl;
                      downQueue[ ip ].push( args );
                      pthread_create( &sendFileT, NULL, sendFile, (void *) &ip );
                      
                  }else {
                      
                      downQueue[ ip ].push( args );
                      
                  }

                  pthread_mutex_unlock(&downQueueMutex);

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
    string * p_ip = (string *) args;
    string ip = *p_ip;

    while( ! downQueue[ ip ].empty() ){

        usleep(500);
        
        pthread_mutex_lock(&downQueueMutex);
        string * argv = (string *) downQueue[ ip ].front();
        pthread_mutex_unlock(&downQueueMutex);
    
        cout << "Enviando " << argv[1] << " a " << argv[0] << endl;

        ServerBbs.sendFile( argv[0], argv[1], argv[2] );

        cout << "Fin de envio de " << argv[1] << " a " << argv[0] << endl;

        pthread_mutex_lock(&downQueueMutex);
        downQueue[ ip ].pop();
        pthread_mutex_unlock(&downQueueMutex);

    }

    downQueue.erase( ip );

    return 0;
}


void cTERM(int iNumSen, siginfo_t *info, void *ni) {
    cout << "Terminado Servidor..." << endl;
    
    ServerBbs.Close();
    
    exit(0);
}

void * timer( void * ){

    while( true ){
        
        // realiza un sleep de una veinteaba parte del timeout establecido
        // de forma de no realizar bucles innesesarios, ni pasar por alto un usuario 
        // fuera del timeout establecido. pasado dicho tiempo, llama a la funcion que buscara
        // usuarios que hayan pasado el tiempo limite de inactividad ( timeoutSignal )
        sleep( ServerBbs.getTimeOut() / 20 );
        ServerBbs.timeoutSignal( );
        
    }

    return 0;
}

void help(void){
    
    cout << "MODO DE USO:" << endl << "\t\t ./serv  ruta_directorio_archivos_servidor  archivo_novedades  cantidad_usuarios  tiempo_limite" << endl << endl;
    cout << "ruta_directorio_archivos_servidor:  ruta del directorio donde se almacenan los archivos del servidor"<< endl<< endl;
    cout << "archivo_novedades:  ruta del archivo donde se almacenan las novedades del servidor"<< endl<< endl;
    cout << "cantidad_usuarios:  especificacion de cuantos usuarios simultaneos pueden estar conectados al servidor al mismo tiempo"<< endl<< endl;
    cout << "tiempo_limite:   tiempo en minutos en el que se desconectara a un usuario inactivo ( que no esta realizando ninguna operacion )"<< endl<< endl;
    
}

int validacion( const char* DESCARGAS ,const char* NOVEDADES,int CANT_USER, int TIMEOUT)
{
    int error = 0;
    
    char * rutaActual = get_current_dir_name();  
    
    if( chdir( DESCARGAS ) == -1 ){
    
        cout << endl << "ERROR: la ruta de directorio de descargas no es valida"  <<endl;
        chdir ( rutaActual);
        error = 1;
    }
    else
    {
        chdir ( rutaActual);
    }
       
    
    
    ifstream  testNovedad(NOVEDADES);
     
    if( !testNovedad.good() ){
     
        cout << endl << "ERROR: El archivo novedades no existe o la ruta no es valida, verifique la ruta o cree el archivo" <<endl;
        error = 1;
    }
    
     if( CANT_USER < 0 ){
     
        cout << endl << "ERROR: la cantidad de usuarios debe ser mayor a cero " <<endl;
        error = 1;
    }
   
    
    if( TIMEOUT < 0 ){
     
        cout << endl << "ERROR: el timeout debe ser mayor a cero " <<endl;
        error = 1;
    }
    
    cout << endl;
    return error;
    
}

/*******/
/* FIN */
/*******/