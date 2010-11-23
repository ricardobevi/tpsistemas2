/************************************/
/* Nombre: ServidorBbs.h            */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 4           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#ifndef BBS_H
#define BBS_H

#include <vector>
#include <iostream>
#include <map>
#include <sys/types.h>
#include <dirent.h>

#include "Usuario.h"
#include "Comm.h"


#define TAM_LOGIN 50
#define TAM_RETURN 1

using namespace std;

class ServidorBbs
{
  public:
	     ServidorBbs();
	     ~ServidorBbs();
	     
	     int ActivarServidor( string archivoConfiguracion );
	     string EsperarCliente();
	     
         Usuario& getUsuario(string Login);
         string getListaUsuarios();
         string getListaArchivos();
         const char * getRutaNovedades();
         int    getTimeOut();
         
         
         bool sendMessage( string Login,string sendTo,string Msg );
         bool sendFile( string Login, string File, string saveTo );
         bool recvFile( string Login, string File );
         
         void timeoutSignal( );
         void sendCloseSignal(string Login);
         void CloseUsuario(string Login);
         void Close();
     
  private:
         int timeOut;
         int CantUsuarios;
	     string RutaDescarga;
	     string RutaNovedades;
         string ArchConf;
         long int Puerto;
	     map < string, Usuario >  Usuarios;
	     Comm<char> * Socket;

         int validacion( const char* DESCARGAS ,const char* NOVEDADES,int CANT_USER, int TIMEOUT);
  
};

ServidorBbs :: ServidorBbs(){
}

ServidorBbs :: ~ServidorBbs(){
}

int ServidorBbs :: ActivarServidor( string archivoConfiguracion ){
    int retorno = 1;

    this->ArchConf = archivoConfiguracion;

    cout << "Archivo de Configuracion: " << archivoConfiguracion << endl;

    ifstream archConf(archivoConfiguracion.c_str());

    string parametro;

    archConf >> parametro;

    while ( !archConf.eof() ) {

        if ( parametro == "RutaDescarga" ) {

            archConf >> RutaDescarga;
            
            if( RutaDescarga[ RutaDescarga.size() - 1 ] != '\\' &&
                RutaDescarga[ RutaDescarga.size() - 1 ] != '/' ) RutaDescarga += "/";
            
            cout << "RutaDescarga = " << RutaDescarga << endl;

        } else if ( parametro == "RutaNovedades" ) {
            archConf >> RutaNovedades;          
            cout << "RutaNovedades = " << RutaNovedades << endl;

        } else if ( parametro == "CantUsuarios" ) {
            archConf >> this->CantUsuarios;
            cout << "CantUsuarios = " << CantUsuarios << endl;

        } else if ( parametro == "timeOut" ) {
            archConf >> this->timeOut;
            cout << "timeOut = " << timeOut << endl;

        } else if ( parametro == "Puerto" ) {
            archConf >> Puerto;
            cout << "Puerto = " << Puerto << endl;

        } else if ( 1 ) {
            cerr << archivoConfiguracion << ": Error en archivo de configuracion." << endl;
            cerr << "Caracter " << archConf.tellg() << endl;
            exit(-1);
        }

        archConf >> parametro;

    }

    archConf.close();

    retorno = validacion( RutaDescarga.c_str(), RutaNovedades.c_str(), CantUsuarios, timeOut );

    if( retorno == 0 ){
        this->Socket = new Comm<char>(this->Puerto);
        this->Socket->Listen(CantUsuarios);
    }
    
    return retorno;
}

string ServidorBbs :: EsperarCliente(){
  unsigned numConnection = 0;
  char Login[TAM_LOGIN],
       ret;
  
  numConnection = this->Socket->Accept();
  
  Socket->getConn(numConnection).Recv(Login, TAM_LOGIN);
  
  if( Usuarios.find( Login ) == Usuarios.end() ){
      cout << Login << " inicia sesion."
           << "[IP: " << Socket->getConn(numConnection).getIp() << "]" << endl;

      Usuario usr( Login, Socket->getConn(numConnection) );

      this->Usuarios[Login]= usr;

      ret = 1;
      
  }else{
      ret = 0;
      Login[0] = '\0';
  }

  Socket->getConn(numConnection).Send( &ret, TAM_RETURN);
  
  return string(Login);
  
}

Usuario& ServidorBbs :: getUsuario(string Login){
  return Usuarios[Login];
}

string ServidorBbs :: getListaUsuarios(){
 
    string ListaUsuarios;
    
     map < string, Usuario > :: iterator it;
    
    for( it= Usuarios.begin() ; it != Usuarios.end() ; it++){
      
        ListaUsuarios = ListaUsuarios + it->first;
        ListaUsuarios.push_back('\n');
    }
    
    return (ListaUsuarios);
}

string ServidorBbs :: getListaArchivos(){

    string ListaArchivos;

    DIR * directorio;

    struct dirent * entradaDir;

    directorio = opendir ( RutaDescarga.c_str() );

    while ( ( entradaDir = readdir ( directorio ) ) ){
        if ( entradaDir->d_type == DT_REG ){
            ListaArchivos += entradaDir->d_name;
            ListaArchivos += "\n";
        }
    }

    
    return (ListaArchivos);
}


bool ServidorBbs :: sendMessage( string Login, string sendTo, string Msg )
{
    
    if( Usuarios.find( sendTo ) == Usuarios.end() )  return false;
            
    Login.append(" dice:");
    
    Usuarios[sendTo].sendString( Login );
    Usuarios[sendTo].sendString( Msg );
    
    return true;
    
}

bool ServidorBbs :: sendFile( string Login, string File, string saveTo ){

    Usuarios[Login].sendFile(RutaDescarga + File, saveTo);

    return true;

}

bool ServidorBbs :: recvFile( string Login, string File ){

    Usuarios[Login].recvFile(RutaDescarga + File);

    return true;

}

void ServidorBbs :: sendCloseSignal(string Login){
    Usuarios[Login].sendCloseSignal();
}

void ServidorBbs :: CloseUsuario(string Login){
    cout << Login <<  " cierra sesion."  << endl;

    Usuarios[Login].Close();
    Usuarios.erase(Login);
}

void ServidorBbs :: Close(){
    cout << "Cerrando sesiones..." << endl;

    map < string, Usuario >::iterator it;
    
    for( it = Usuarios.begin() ; it != Usuarios.end() ; it++ )
        this->CloseUsuario( (*it).first );

    Socket->Close();
}

void ServidorBbs :: timeoutSignal( ){

    // si hay usuarios en el servidor comienzo a buscar cual esta fuera del timeout
    if ( Usuarios.size() > 0 )
    {
            map < string, Usuario >::iterator it  ;
            
            it = Usuarios.begin();
            
            for( it = Usuarios.begin() ; it != Usuarios.end() ; it++ )
            {
                    // para ver proceso de timeout descomentar la siguiente linea
                    //cout << endl << (it->first) << ":: Ultima op: " << (it->second).getLastOperation() << "   now: " <<   time(NULL) ;
                    //cout << "   Dif:  " <<    difftime(  time(NULL) , (it->second).getLastOperation() )  << " segundos";
                    
                    // si el tiempo inactivo en el servidor es mayor al timeout da de baja al login del usuario
                    if( difftime(  time(NULL) , (it->second).getLastOperation() ) >=  (double)  this->timeOut ){
                                // para ver proceso de timeout descomentar la siguiente linea
                                // cout <<endl  << (it->first) << " fue dado de baja "  <<endl;
                                this-> sendCloseSignal((*it).first);
                    }
            }
    }

}

int ServidorBbs :: getTimeOut(){
    
    return this->timeOut;
}

const char *  ServidorBbs :: getRutaNovedades(){

    return (this->RutaNovedades).c_str();
}

int ServidorBbs :: validacion( const char* DESCARGAS ,const char* NOVEDADES,int CANT_USER, int TIMEOUT)
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

#endif


/*******/
/* FIN */
/*******/
