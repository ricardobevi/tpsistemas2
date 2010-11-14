#ifndef MEM_COMPARTIDA_H
#define MEM_COMPARTIDA_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
 
 
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


#include "t_protocolo.h"
#include "Semaforo.h"

using namespace std;

class MemCompartida
{
    private:
        
        t_protocolo * MemoriaSAC , * MemoriaCAS;
        Semaforo semaforos;
        int tipoDeAplicacion;
        
    public:
        
        MemCompartida(int tipoDeAplicacion );
        ~MemCompartida();
        
        void esperarUsuario();
        void conectarce();
        
        void enviarAServidor( t_protocolo& datoAServidor);
        void  enviarACliente( t_protocolo& datoACliente);
        
        void recibirDeServidor( t_protocolo& datoAServidor);
        void recibirDeCliente( t_protocolo& datoACliente);
        
        
};



/* -------------------------- define para memoria compartida --------------------------- //
#define SERVIDOR   0
#define CLIENTE    1

#define PATH_KEY  /bin/bash
#define CANT_SEM      5
#define SEM_KEY       2010
#define MEM_SAC_KEY   2002
#define MEM_CAS_KEY   1409

#define S_INICIO 0

#define S_BLOCK_CLI_SAC   1
#define S_BLOCK_SERV_SAC  2
#define S_BLOCK_CLI_CAS   3
#define S_BLOCK_SERV_CAS  4
// -------------------------- define para memoria compartida --------------------------- */

MemCompartida :: MemCompartida( int tipoDeAplicacion )
{
    
    this -> tipoDeAplicacion =  tipoDeAplicacion ;
    
    int shSAC ,shCAS ;
    key_t claveSemaforo = ftok( PATH_KEY , SEM_KEY );
    key_t claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
    key_t claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );
        
    
    
        if( tipoDeAplicacion == SERVIDOR )
        {       
            semaforos.crearSemaforo( claveSemaforo , CANT_SEM, SERVIDOR);

            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , IPC_CREAT | IPC_EXCL | 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , IPC_CREAT | IPC_EXCL | 0660 );
            
        }
        else //CLIENTE
        {
                
            semaforos.crearSemaforo( claveSemaforo , CANT_SEM ,CLIENTE);
                    
            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , 0660 );    
            
        }
    
    
    
    if ( shSAC == - 1 || shCAS ==  -1 )
    {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) :");
                shmctl(claveMemSac,IPC_RMID,0);
                shmctl(claveMemCas,IPC_RMID,0);
    }
            
    MemoriaSAC = (t_protocolo * ) shmat (shSAC,NULL , 0);
    MemoriaCAS = (t_protocolo * ) shmat (shCAS,NULL , 0);
    
}


MemCompartida :: ~MemCompartida()
{
    shmdt(MemoriaCAS);
    shmdt(MemoriaSAC);
    
    if (tipoDeAplicacion == SERVIDOR )
    {
        key_t claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
        key_t claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );
        
        shmctl(claveMemSac,IPC_RMID,0);
        shmctl(claveMemCas,IPC_RMID,0);
    }
}


void MemCompartida :: esperarUsuario()
{
    
    
    if( semaforos.P( S_INICIO_SERV )  == -1)
        cout << endl << "Error al iniciar el servidor";
    else
        cout << endl << "Servidor iniciado";
     
    
    if ( semaforos.V( S_INICIO_CLI ) == -1 )
        cout << endl << "Error al esperar Usuarios";
    else
        cout << endl << "Liberacion con exito de semaforo  de inicio del cliente";
    
     
     if( semaforos.V( S_BLOCK_CLI_CAS )  == -1)  
            cout << endl << "Error al habilitar el CAS cliente";
     else
            cout << endl << "Exito al habilitar el CAS cliente";
    
}


void  MemCompartida :: conectarce()
{
    
    
    
    if( semaforos.V( S_INICIO_SERV ) == -1 ) 
        cout << endl << "Error al conectarce";
    else
        cout << endl << "Liberacion con exito de semaforo  de inicio del servidor";
    
    
    
    if( semaforos.P( S_INICIO_CLI ) == -1 )
        cout << endl << "Error al iniciar cliente ";   
    else
        cout << endl << "Cliente iniciado";
        
    
    
    
     if(  semaforos.V( S_BLOCK_SERV_SAC )  == -1)  
            cout << endl << "Error al habilitar el SAC servidor";
     else
            cout << endl << "Exito al habilitar el SAC servidor";
     
      
}

void  MemCompartida :: enviarAServidor( t_protocolo& datoAServidor)
{
    
    semaforos.P(  S_BLOCK_CLI_CAS );
    
    *MemoriaCAS =  datoAServidor;
    
    semaforos.V( S_BLOCK_SERV_CAS );
    
}

void  MemCompartida :: enviarACliente( t_protocolo& datoACliente)
{
        
    semaforos.P(   S_BLOCK_SERV_SAC  );
    
    *MemoriaSAC =  datoACliente;
    
    semaforos.V(  S_BLOCK_CLI_SAC  );
}

void MemCompartida :: recibirDeServidor( t_protocolo& datoAServidor)
{
        
    semaforos.P(   S_BLOCK_CLI_SAC   );
    
    datoAServidor = *MemoriaSAC ;
    
    semaforos.V(  S_BLOCK_SERV_SAC );
    
}

void MemCompartida :: recibirDeCliente( t_protocolo& datoACliente)
{

           
    semaforos.P(  S_BLOCK_SERV_CAS   );
    
    datoACliente = *MemoriaCAS;
    
    semaforos.V(  S_BLOCK_CLI_CAS  );
    
    
}

#endif