#include <iostream>
#include <stdio.h>

#include "MemCompartida.h"


using namespace std;

int main(int argc, const char *argv[])
{
    MemCompartida MemCli( CLIENTE );
    
    MemCli.conectarce();
        
    cout << endl  << endl << "Comienzo del cliente "  ;
    
    t_protocolo envio , recepcion ;
    
    envio.x = 1409;
    
    recepcion.x = 0;
    
    MemCli.enviarAServidor(envio);
    
     cout << endl << "El cliente ya envio "  ;
    
    MemCli.recibirDeServidor(recepcion);
    
     cout << endl << "El cliente ya recibio "  ;
    
    cout << endl << "Se ha enviado : " <<  envio.x  <<  endl << "Se ha recibido : "  <<  recepcion.x << endl;
    
    /*
        void esperarUsuario();
        void conectarce();
        
        void enviarAServidor( t_protocolo& datoAServidor);
        void  enviarACliente( t_protocolo& datoACliente);
        
        void recibirDeServidor( t_protocolo& datoAServidor);
        void recibirDeCliente( t_protocolo& datoACliente);
        */
    
}