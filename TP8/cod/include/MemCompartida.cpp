/*
 * MemCompartida.cpp
 *
 *  Created on: 14/11/2010
 *      Author: ric
 */

#include "MemCompartida.h"
#include <signal.h>

MemCompartida :: MemCompartida()
{
    pidDestino = 0;
}


void  MemCompartida :: CargarMemCompartida(int tipoDeAplicacion)
{
    Activo = true;
    
    this -> tipoDeAplicacion =  tipoDeAplicacion ;

    int  retornoSemaforo ;
    
    key_t claveSemaforo = ftok( PATH_KEY , SEM_KEY );
    
    claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
    claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );


    // tomando el parametro enviado diferencia entre servidor y cliente:
    //  *servidor: crea los semaforos y la memoria compartida
    //  *cliente: asigna y mapea lo creado por el servidor, mas no crea nada por si solo
        
        if( tipoDeAplicacion == SERVIDOR )
             retornoSemaforo = semaforos.crearSemaforo( claveSemaforo , CANT_SEM);
        
        else //CLIENTE
            
            retornoSemaforo = semaforos.mapearSemaforo( claveSemaforo , CANT_SEM);
            
        
        
        if (  retornoSemaforo == -1 )
        {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) : Semaforo :");
                exit(1);
        }
        else
        {
                if( tipoDeAplicacion == SERVIDOR )
                {
                        IdMemSac = shmget( claveMemSac, sizeof (t_protocolo) , IPC_CREAT  | 0666 );
                        IdMemCas = shmget( claveMemCas, sizeof (t_protocolo) , IPC_CREAT  | 0666 );
                }
                else
                {
                        IdMemSac = shmget( claveMemSac, sizeof (t_protocolo) , 0666 );
                        IdMemCas = shmget( claveMemCas, sizeof (t_protocolo) , 0666 );
                }
            
            
        }
            
    // si se produjo algun tipo de error, cierro correctamente la memoria y los semaforos informando dicho error
    // sino mapeo las memorias compartidas para utilizarlas
    if ( IdMemSac == - 1 || IdMemCas ==  -1 )
    {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) : Memoria :");

    }
    else
    {
                MemoriaSAC = (t_protocolo * ) shmat ( IdMemSac ,NULL , 0);
                MemoriaCAS = (t_protocolo * ) shmat ( IdMemCas ,NULL , 0);
    }


}
// crea la memoria compartida para realizar pasajes de datos tipo t_protocolo en forma bidireccional,
// para ello crea los semaforos y las areas de memoria compartidas y las mapea para utilzarlas.
MemCompartida :: MemCompartida( int tipoDeAplicacion )
{

    Activo = true;
    
    this -> tipoDeAplicacion =  tipoDeAplicacion ;

    int  retornoSemaforo ;
    
    key_t claveSemaforo = ftok( PATH_KEY , SEM_KEY );
    
    claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
    claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );


    // tomando el parametro enviado diferencia entre servidor y cliente:
    //  *servidor: crea los semaforos y la memoria compartida
    //  *cliente: asigna y mapea lo creado por el servidor, mas no crea nada por si solo
        
        if( tipoDeAplicacion == SERVIDOR )
             retornoSemaforo = semaforos.crearSemaforo( claveSemaforo , CANT_SEM);
        
        else //CLIENTE
            
            retornoSemaforo = semaforos.mapearSemaforo( claveSemaforo , CANT_SEM);
            
        
        
        if (  retornoSemaforo == -1 )
        {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) : Semaforo :");
                exit(1);
        }
        else
        {
                if( tipoDeAplicacion == SERVIDOR )
                {
                        IdMemSac = shmget( claveMemSac, sizeof (t_protocolo) , IPC_CREAT  | 0666 );
                        IdMemCas = shmget( claveMemCas, sizeof (t_protocolo) , IPC_CREAT  | 0666 );
                }
                else
                {
                        IdMemSac = shmget( claveMemSac, sizeof (t_protocolo) , 0666 );
                        IdMemCas = shmget( claveMemCas, sizeof (t_protocolo) , 0666 );
                }
            
            
        }
            
    // si se produjo algun tipo de error, cierro correctamente la memoria y los semaforos informando dicho error
    // sino mapeo las memorias compartidas para utilizarlas
    if ( IdMemSac == - 1 || IdMemCas ==  -1 )
    {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) : Memoria :");

    }
    else
    {
                MemoriaSAC = (t_protocolo * ) shmat ( IdMemSac ,NULL , 0);
                MemoriaCAS = (t_protocolo * ) shmat ( IdMemCas ,NULL , 0);
    }


}

// destructor de memoria compartida
MemCompartida :: ~MemCompartida()
{
}


// metodo del servidor para esperar que se conecte un Usuario
// y comenzar a escuchar el movimiento en las areas de memoria compartida:
// secuencia logica:
//                      P( inicio_servidor )
//                      V( inicio_cliente )
//                      V( cliente_CAS   ) --> habilito el envio de datos desde el cliente al servidor
//
int MemCompartida :: esperarUsuario()
{


    if( semaforos.P( S_INICIO_SERV )  == -1)
    {
        cout << endl << "Error al iniciar el servidor";
        return -1;
    }
    //else
    //    cout << endl << "Servidor iniciado";


    if ( semaforos.V( S_INICIO_CLI ) == -1 )
    {
        cout << endl << "Error al iniciar el servidor";
        return -1;
    }

    //else
    //    cout << endl << "Liberacion con exito de semaforo  de inicio del cliente";


     if( semaforos.V( S_BLOCK_CLI_CAS )  == -1)
     {
        cout << endl << "Error al habilitar el CAS cliente";
        return -1;
     }
     //else
     //        cout << endl << "Exito al habilitar el CAS cliente";
     
        
     //this -> obtenerPid( SERVIDOR );
     
     return 1;

}




// metodo del Cliente para esperar que el servidor cree las estructuras necesarias
// y comenzar a enviar datos al area de memoria compartida:
// secuencia logica:
//                      V( inicio_servidor )
//                      P( inicio_cliente )
//                      V( Servidor_SAC   ) --> habilito el envio de datos desde el servidor al cliente
//
int  MemCompartida :: conectarce()
{



    if( semaforos.V( S_INICIO_SERV ) == -1 )
    {
        cout << endl << "Error al conectarce";
        return -1;
    }

    //else
    //   cout << endl << "Liberacion con exito de semaforo  de inicio del servidor";



    if( semaforos.P( S_INICIO_CLI ) == -1 )
    {
        cout << endl << "Error al iniciar cliente ";
        return -1;
    }
    //else
    //   cout << endl << "Cliente iniciado";




     if(  semaforos.V( S_BLOCK_SERV_SAC )  == -1)
     {
        cout << endl << "Error al habilitar el SAC servidor";
        return -1;
     }
     //else
     //       cout << endl << "Exito al habilitar el SAC servidor";
     
     
    // this -> obtenerPid( CLIENTE );
     
     return 1;
}


// envio de datos ( datoAServidor ) desde el cliente al servidor a travez de el area de memoria compartida CAS
void  MemCompartida :: enviarAServidor( t_protocolo& datoAServidor)
{

    if ( semaforos.P(  S_BLOCK_CLI_CAS )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );

    *MemoriaCAS =  datoAServidor;

    if ( semaforos.V( S_BLOCK_SERV_CAS )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );

}


  
// envio de datos ( datoACliente ) desde el servidor  al cliente a travez de el area de memoria compartida SAC
void  MemCompartida :: enviarACliente( t_protocolo& datoACliente)
{

 
    if (  semaforos.P(   S_BLOCK_SERV_SAC  )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );

    *MemoriaSAC =  datoACliente;

    if (  semaforos.V(  S_BLOCK_CLI_SAC  )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );
}


// recepcion de datos ( datoAServidor ) desde el servidor  al cliente a travez de el area de memoria compartida SAC
void MemCompartida :: recibirDeServidor( t_protocolo& datoAServidor)
{

    if ( semaforos.P(   S_BLOCK_CLI_SAC   )   == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );
    

    datoAServidor = *MemoriaSAC ;
    
    if ( semaforos.V(  S_BLOCK_SERV_SAC )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );
    

}


// recepcion de datos ( datoACliente ) desde el cliente   al servidor a travez de el area de memoria compartida CAS
void MemCompartida :: recibirDeCliente( t_protocolo& datoACliente)
{

    if (  semaforos.P(  S_BLOCK_SERV_CAS   )  == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );
   

    datoACliente = *MemoriaCAS;

    if ( semaforos.V(  S_BLOCK_CLI_CAS  )   == -1 )
                this -> eliminarMemoriaCompartida( CLIENTE );
    


}


int MemCompartida :: obtenerPid( int tipoDeAplicacion ){
    
     t_protocolo miPid , pidRecibido ;
     
     miPid.x =  getpid();
     
     if ( tipoDeAplicacion == SERVIDOR )
     {
        this -> enviarACliente(miPid);
        this -> recibirDeCliente(pidRecibido);
     }
     else
     {
          this -> enviarAServidor(miPid);
          this -> recibirDeServidor(pidRecibido);
     }
     
     pidDestino = pidRecibido.x;
     
     return 1;
     
}

//metodo que elimina de forma limpia los semaforos y memorias compartidas
int MemCompartida :: eliminarMemoriaCompartida( int tipoDeAplicacion )
{
     int exito = 1;  

    if (tipoDeAplicacion == SERVIDOR )
    {
             if ( Activo == true  )
            {

                    //if( pidDestino != 0 ){
                        //cout << "enviando senial al proceso " << pidDestino << endl;
                        //kill( pidDestino ,  SIGUSR1 );

                        //semaforos.P(CLIENTE_INACTIVO);
                        
                    //}
                    
                    if ( semaforos.rmSem() == -1 ) 
                    {
                        perror("MemCompartida.h: eliminarMemoriaCompartida() : semaforos.rmSem : ");
                        exito = -1;
                    }
                    
                    
                    if ( shmdt(MemoriaCAS) == -1  ) 
                    {
                            perror("MemCompartida.h: shmdt() : MemCas: ");  
                            exito = -1;
                    }
                    
                    
                    if ( shmdt(MemoriaSAC) == -1 ) 
                    {
                            perror("MemCompartida.h: shmdt() : MemSac: ");   
                            exito = -1;
                    }
                    
                    sleep(10);

                    if ( shmctl( IdMemSac , IPC_RMID , (struct shmid_ds *)NULL)  == -1 )
                    {
                        perror("MemCompartida.h: eliminarMemoriaCompartida() : MemSac : ");
                        exito = -1;
                    }
                    

                    if ( shmctl( IdMemCas , IPC_RMID ,(struct shmid_ds *)NULL)   == -1 )
                    {
                        perror("MemCompartida.h: eliminarMemoriaCompartida() : MemCas : ");
                        exito = -1;
                    }
                    
                    
                    Activo = false;
            }
       
       
       

        
    }
    else
    {    
        if  ( Activo == true )
        {
            if ( shmdt(MemoriaCAS) == -1  ) 
            {
                    perror("MemCompartida.h: shmdt() : MemCas: ");  
                    exito = -1;
            }
            
            
            if ( shmdt(MemoriaSAC) == -1 ) 
            {
                    perror("MemCompartida.h: shmdt() : MemSac: ");   
                    exito = -1;
            }
            
            //semaforos.V(CLIENTE_INACTIVO);
            
            Activo = false;
            
            //kill( pidDestino ,  SIGUSR1 );
            
        }
    }
    
    
    
    
    return exito;
}
