#include <iostream>
#include <stdio.h>
#include <signal.h>

#include "MemCompartida.h"


using namespace std;


MemCompartida MemCli;

void finalizar(int iNumSen, siginfo_t  * info, void *ni)
{
    
    MemCli.eliminarMemoriaCompartida( CLIENTE );
    
    if( iNumSen == SIGUSR1 )
    {
        cout << endl << "El servidor ha terminado"  ;
    }
    
    cout << endl << "El cliente ha finalizado" << endl << endl ;
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
    
    MemCli.CargarMemCompartida( CLIENTE );
    
    MemCli.conectarce();
        
    
    
    
    cout << endl  << endl << "Comienzo del cliente "  ;
    
    t_protocolo envio , recepcion ;
    
    envio.x = 1409;
    
    recepcion.x = 0;
    
    
    
    
    MemCli.enviarAServidor(envio);
                    cout << endl << "El cliente ya envio "  ;
    
    MemCli.recibirDeServidor(recepcion);
                    cout << endl << "El cliente ya recibio "  ;
    
                    
    
    //------------------------------------------------------------------------------------------------------                
    
    cout << endl << "Se ha enviado : " <<  envio.x  <<  endl << "Se ha recibido : "  <<  recepcion.x << endl;
    
    //------------------------------------------------------------------------------------------------------
    

    cout << endl << "El cliente enviara datos: "  << endl;
    
    do{
            cout << "*Dato a enviar: " ;
            cin >> envio.x;
            MemCli.enviarAServidor(envio);

    }while ( envio.x !=  -1);
    

    cout << endl << "Fin de cliente"  << endl;
    
    MemCli.eliminarMemoriaCompartida(CLIENTE);
     
    return 0;   
}