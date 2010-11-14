/*
 * MemCompartida.cpp
 *
 *  Created on: 14/11/2010
 *      Author: ric
 */

#include "MemCompartida.h"

MemCompartida :: MemCompartida()
{

}


void  MemCompartida :: CargarMemCompartida(int tipoDeAplicacion)
{
    this -> tipoDeAplicacion =  tipoDeAplicacion ;

    int shSAC ,shCAS, retornoSemaforo ;
    key_t claveSemaforo = ftok( PATH_KEY , SEM_KEY );
    key_t claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
    key_t claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );


    // tomando el parametro enviado diferencia entre servidor y cliente:
    //  *servidor: crea los semaforos y la memoria compartida
    //  *cliente: asigna y mapea lo creado por el servidor, mas no crea nada por si solo
        if( tipoDeAplicacion == SERVIDOR )
        {
            retornoSemaforo = semaforos.crearSemaforo( claveSemaforo , CANT_SEM);


            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , IPC_CREAT  | 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , IPC_CREAT  | 0660 );

            semaforos.V(SERVIDOR_ACTIVO);

        }
        else //CLIENTE
        {

            retornoSemaforo = semaforos.mapearSemaforo( claveSemaforo , CANT_SEM );
            semaforos.P(SERVIDOR_ACTIVO);

            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , 0660 );

        }



    // si se produjo algun tipo de error, cierro correctamente la memoria y los semaforos informando dicho error
    // sino mapeo las memorias compartidas para utilizarlas
    if ( shSAC == - 1 || shCAS ==  -1 || retornoSemaforo == -1 )
    {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) :");
                this -> eliminarMemoriaCompartida();

    }
    else
    {
                MemoriaSAC = (t_protocolo * ) shmat (shSAC,NULL , 0);
                MemoriaCAS = (t_protocolo * ) shmat (shCAS,NULL , 0);
    }


}
// crea la memoria compartida para realizar pasajes de datos tipo t_protocolo en forma bidireccional,
// para ello crea los semaforos y las areas de memoria compartidas y las mapea para utilzarlas.
MemCompartida :: MemCompartida( int tipoDeAplicacion )
{

    this -> tipoDeAplicacion =  tipoDeAplicacion ;

    int shSAC ,shCAS, retornoSemaforo ;
    key_t claveSemaforo = ftok( PATH_KEY , SEM_KEY );
    key_t claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
    key_t claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );


    // tomando el parametro enviado diferencia entre servidor y cliente:
    //  *servidor: crea los semaforos y la memoria compartida
    //  *cliente: asigna y mapea lo creado por el servidor, mas no crea nada por si solo
        if( tipoDeAplicacion == SERVIDOR )
        {
            retornoSemaforo = semaforos.crearSemaforo( claveSemaforo , CANT_SEM);


            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , IPC_CREAT  | 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , IPC_CREAT  | 0660 );

            semaforos.V(SERVIDOR_ACTIVO);

        }
        else //CLIENTE
        {

            retornoSemaforo = semaforos.mapearSemaforo( claveSemaforo , CANT_SEM );
            semaforos.P(SERVIDOR_ACTIVO);

            shSAC = shmget( claveMemSac, sizeof (t_protocolo) , 0660 );
            shCAS = shmget( claveMemCas, sizeof (t_protocolo) , 0660 );

        }



    // si se produjo algun tipo de error, cierro correctamente la memoria y los semaforos informando dicho error
    // sino mapeo las memorias compartidas para utilizarlas
    if ( shSAC == - 1 || shCAS ==  -1 || retornoSemaforo == -1 )
    {
                perror("MemCompartida.h: MemCompartida ( int tipoDeAplicacion ) :");
                this -> eliminarMemoriaCompartida();

    }
    else
    {
                MemoriaSAC = (t_protocolo * ) shmat (shSAC,NULL , 0);
                MemoriaCAS = (t_protocolo * ) shmat (shCAS,NULL , 0);
    }


}

// destructor de memoria compartida
MemCompartida :: ~MemCompartida()
{
    this -> eliminarMemoriaCompartida();
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
        this -> eliminarMemoriaCompartida();
        return -1;
    }
    //else
    //    cout << endl << "Servidor iniciado";


    if ( semaforos.V( S_INICIO_CLI ) == -1 )
    {
        cout << endl << "Error al iniciar el servidor";
        this -> eliminarMemoriaCompartida();
        return -1;
    }

    //else
    //    cout << endl << "Liberacion con exito de semaforo  de inicio del cliente";


     if( semaforos.V( S_BLOCK_CLI_CAS )  == -1)
     {
        cout << endl << "Error al habilitar el CAS cliente";
        this -> eliminarMemoriaCompartida();
        return -1;
     }
     //else
     //        cout << endl << "Exito al habilitar el CAS cliente";

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
        this -> eliminarMemoriaCompartida();
        return -1;
    }

    //else
    //   cout << endl << "Liberacion con exito de semaforo  de inicio del servidor";



    if( semaforos.P( S_INICIO_CLI ) == -1 )
    {
        cout << endl << "Error al iniciar cliente ";
        this -> eliminarMemoriaCompartida();
        return -1;
    }
    //else
    //   cout << endl << "Cliente iniciado";




     if(  semaforos.V( S_BLOCK_SERV_SAC )  == -1)
     {
        cout << endl << "Error al habilitar el SAC servidor";
        this -> eliminarMemoriaCompartida();
        return -1;
     }
     //else
     //       cout << endl << "Exito al habilitar el SAC servidor";

     return 1;
}


// envio de datos ( datoAServidor ) desde el cliente al servidor a travez de el area de memoria compartida CAS
void  MemCompartida :: enviarAServidor( t_protocolo& datoAServidor)
{

    semaforos.P(  S_BLOCK_CLI_CAS );

    *MemoriaCAS =  datoAServidor;

    semaforos.V( S_BLOCK_SERV_CAS );

}


// envio de datos ( datoACliente ) desde el servidor  al cliente a travez de el area de memoria compartida SAC
void  MemCompartida :: enviarACliente( t_protocolo& datoACliente)
{

    semaforos.P(   S_BLOCK_SERV_SAC  );

    *MemoriaSAC =  datoACliente;

    semaforos.V(  S_BLOCK_CLI_SAC  );
}

// recepcion de datos ( datoAServidor ) desde el servidor  al cliente a travez de el area de memoria compartida SAC
void MemCompartida :: recibirDeServidor( t_protocolo& datoAServidor)
{

    semaforos.P(   S_BLOCK_CLI_SAC   );

    datoAServidor = *MemoriaSAC ;

    semaforos.V(  S_BLOCK_SERV_SAC );

}


// recepcion de datos ( datoACliente ) desde el cliente   al servidor a travez de el area de memoria compartida CAS
void MemCompartida :: recibirDeCliente( t_protocolo& datoACliente)
{


    semaforos.P(  S_BLOCK_SERV_CAS   );

    datoACliente = *MemoriaCAS;

    semaforos.V(  S_BLOCK_CLI_CAS  );


}


//metodo que elimina de forma limpia los semaforos y memorias compartidas
int MemCompartida :: eliminarMemoriaCompartida()
{
    /*
    // esto tira errores y nose exactamente porque
    if ( semaforos.rmSem() == -1 )
    {
        perror("MemCompartida.h: eliminarMemoriaCompartida() : rmSem() : ");
        return -1;
    }
    */

    shmdt(MemoriaCAS);
    shmdt(MemoriaSAC);



    if (tipoDeAplicacion == SERVIDOR )
    {
        key_t claveMemCas   = ftok( PATH_KEY , MEM_CAS_KEY );
        key_t claveMemSac   = ftok( PATH_KEY , MEM_SAC_KEY );

        shmctl(claveMemSac,IPC_RMID,0) ;
        shmctl(claveMemCas,IPC_RMID,0) ;

        /*

        // esto tira errores y nose exactamente porque

        if ( shmctl(claveMemSac,IPC_RMID,0)  == -1 )
        {
            perror("MemCompartida.h: eliminarMemoriaCompartida() : MemSac : ");
            return -1;
        }

        if ( shmctl(claveMemCas,IPC_RMID,0)   == -1 )
        {
            perror("MemCompartida.h: eliminarMemoriaCompartida() : MemCas : ");
            return -1;
        }
        */

    }

    return 1;
}
