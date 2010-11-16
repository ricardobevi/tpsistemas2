#include <iostream>
#include <stdio.h>
#include <signal.h>

#include "MemCompartida.h"


using namespace std;

MemCompartida MemCli;

void finalizar(int iNumSen, siginfo_t  * info, void *ni)
{
    
    MemCli.eliminarMemoriaCompartida( SERVIDOR );
    
    
    if( iNumSen == SIGUSR1 )
    {
        cout << endl << "El cliente ha terminado";
    }
    
    cout << endl << "El servidor ha finalizado" << endl << endl ;
    exit (0);
}


int main(int argc, const char *argv[])
{
    
     struct sigaction term;

    term.sa_sigaction = finalizar;
    sigfillset( &term.sa_mask );
    term.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGINT , &term, NULL);
    sigaction(SIGPIPE , &term, NULL);
    sigaction(SIGTERM , &term, NULL);
    sigaction(SIGQUIT , &term, NULL);
    sigaction(SIGABRT , &term, NULL);
    sigaction(SIGSEGV , &term, NULL);  
    sigaction(SIGUSR1 , &term, NULL);  
    
    
    MemCli.CargarMemCompartida( SERVIDOR );
    MemCli.esperarUsuario();
    MemCli.obtenerPid( SERVIDOR );
    

    
    
    
    
    cout << endl << endl << "Comienzo del servidor "  ;
  
    t_protocolo envio , recepcion ;
    
    envio.x = 2002;
    
    recepcion.x = 0;

    
    
    
    
    MemCli.enviarACliente( envio );
            cout << endl << "El servidor ya envio "  ;
    
    MemCli.recibirDeCliente( recepcion );
            cout << endl << "El servidor ya recibio "  ;
    
            
    //------------------------------------------------------------------------------------------------------
    
    cout << endl << "Se ha enviado : "  << envio.x  <<  endl << "Se ha recibido : "  <<  recepcion.x << endl;
    
    //------------------------------------------------------------------------------------------------------  
    
     cout << endl << endl << "El servidor esperara envios del cliente "  ;
    while ( ((int) recepcion.x) !=  -1)
    {
        MemCli.recibirDeCliente( recepcion );
        cout << endl <<  "Se ha recibido : "  <<  recepcion.x << endl;
    }

    cout << endl << "Fin de servidor" << endl;
    MemCli.eliminarMemoriaCompartida( SERVIDOR );
    return 0;
    
}