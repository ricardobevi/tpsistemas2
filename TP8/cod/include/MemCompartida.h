/*
 * MemCompartida.h
 *
 *  Created on: 14/11/2010
 *      Author: ric
 */

#ifndef MEMCOMPARTIDA_H_
#define MEMCOMPARTIDA_H_

#include <sys/shm.h>

#include "Semaforo.h"
#include  "constantes.h"

using namespace std;

class MemCompartida
{
    private:

        t_protocolo * MemoriaSAC , //memoria de servidor a cliente
                    * MemoriaCAS;  //memoria de cliente a servidor
                   
                   
        int IdMemSac ,IdMemCas;

        key_t claveMemCas, claveMemSac;
        
        Semaforo semaforos;
        
        int tipoDeAplicacion;

    public:

        MemCompartida();
        MemCompartida(int tipoDeAplicacion );
        ~MemCompartida();

        void CargarMemCompartida(int tipoDeAplicacion);
        int esperarUsuario();
        int conectarce();

        void  enviarAServidor( t_protocolo& datoAServidor);
        void  enviarACliente ( t_protocolo& datoACliente );

        void recibirDeServidor( t_protocolo& datoAServidor);
        void recibirDeCliente ( t_protocolo& datoACliente );

        int eliminarMemoriaCompartida( int tipoDeAplicacion );


};

#endif /* MEMCOMPARTIDA_H_ */
