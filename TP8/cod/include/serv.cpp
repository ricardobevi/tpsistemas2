#include <iostream>
#include <stdio.h>

#include "MemCompartida.h"



using namespace std;

int main(int argc, const char *argv[])
{
    MemCompartida MemCli( SERVIDOR );
    
    MemCli.esperarUsuario();
    
        cout << endl << endl << "Comienzo del servidor "  ;
    
    t_protocolo envio , recepcion ;
    
    envio.x = 2002;
    
    recepcion.x = 0;

    
    MemCli.enviarACliente( envio );
    
    cout << endl << "El servidor ya envio "  ;
    
    MemCli.recibirDeCliente( recepcion );
    
    cout << endl << "El servidor ya recibio "  ;
    
    cout << endl << "Se ha enviado : "  << envio.x  <<  endl << "Se ha recibido : "  <<  recepcion.x << endl;
    
    /*
        void esperarUsuario();
        void conectarce();
        
        void enviarAServidor( t_protocolo& datoAServidor);
        void  enviarACliente( t_protocolo& datoACliente);
        
        void recibirDeServidor( t_protocolo& datoAServidor);
        void recibirDeCliente( t_protocolo& datoACliente);
        */
    
}