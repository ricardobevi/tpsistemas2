#ifndef C_BOMBERMAN_H
#define C_BOMBERMAN_H

#include <fstream>

#include "entorno.h"
#include "c_escenario.h"
#include "../include/t_protocolo.h"
#include "../include/Connection.h"

using namespace std;

class Bomberman
{
    private:
        
        Connection  < char >  connectionCliente;
        Entorno     entornoCliente;
        
        string ipServidor ;
        unsigned int puerto;
        unsigned int timeOut;
        
        int idJugador;
        
    public:
         
         Escenario escenarioCliente;                           // Objeto que identifica el estado actual de la pantalla
                                                               // (posicion de todos sus elementos)
     
         
    public:
        
         Bomberman();
         ~Bomberman();
         
         char leerTeclado();
         void set_idJugador( int jugador );
         void dibujarPantalla();
         void recivirAccion(t_protocolo * accion, size_t tam );
         void actualizarNovedades( t_protocolo * accion );
         void enviarSolicitud ( char teclaPresionada);
         void ComunicarServidor();
         
         void finalizarBomberman(void);
                   
};


// envia la tecla que se presiono al servidor a travez de 
// un objeto t_protocolo:
//                          id = 'i' de informe
//                          x  = tecla presionada
void  Bomberman :: enviarSolicitud ( char teclaPresionada)
{
    t_protocolo solicitud;
    
    solicitud.id = 'i';
    solicitud.x  = teclaPresionada ;

    //send de tipo protocolo
    connectionCliente.Send( (char*) &solicitud, sizeof(t_protocolo) );
}


// carga el valor del numero de jugador ( 1-2-3-4 ) en idJugador
void Bomberman :: set_idJugador( int jugador )
{
    idJugador = jugador ;
}
    

// Carga la configuracion del archivo de configuracion en el objeto 
// e inicializa el entorno y la conexion con sus valores correspondientes

Bomberman :: Bomberman()
{
   
    ifstream configuracion("c_bomberman.conf");
    
    if( !configuracion.good() )
    {
        cout<< endl << "ERROR: error de apertura en el archivo" << endl;
        system("PAUSE");
        exit(1);
    }
    
    configuracion >> ipServidor ;
    configuracion >> puerto;
    configuracion >> timeOut;
       
    configuracion.close();
    
    entornoCliente.cargarEntorno( &escenarioCliente );
    
    connectionCliente.Connect( ipServidor , puerto);
}


Bomberman :: ~Bomberman()
{
}

// Cierra correctamente la pantalla y la conexion del cliente
void Bomberman :: finalizarBomberman( void )
{
    connectionCliente.Close();
    entornoCliente.finalizarPantalla();
}


// lee la tecla presionada por el usuario y devuelve la misma como resultado ( por ahora solo eso )
char  Bomberman ::leerTeclado()
{
     return  getch();
}


// recarga la pantalla con los valores nuevos obtenidos
void  Bomberman ::dibujarPantalla()
{        
         entornoCliente.actualizarPantalla();       
}
         
// metodo que carga a "accion" con las novedades enviadas por el servidor    
void Bomberman :: recivirAccion(t_protocolo * accion, size_t tam  )
{
    // aca va el recv de tipo protocolo
    //connectionCliente.Recv( accion, tam );
}



// Actualiza el escenarioCliente dependiendo de las novedades que hayan llegado 
void Bomberman :: actualizarNovedades( t_protocolo * accion )
{
     /*

                   v :     vector < unsigned int > vidas;
                   j :     vector < Coordenada > jugadores;
                   f :     vector < Coordenada > paredesFijas;
                   d :     vector < Coordenada > paredesDestruibles;
                   b :     vector < Coordenada > bombas;
                   e :     vector < vector < Coordenada >  > explosiones;  
                   t :     unsigned int    tiempo;
                   
                   B ;     vector < Coordenada > premiosBomba;
                   V :     vector < Coordenada > premiosVida;
                   E :     vector < Coordenada > premiosExplosion;
                   R :     vector < Coordenada > premiosVelocidad;
                  

            */
            
            // el siguiente switch determina que operaciones deben realizarce en base a que novedad ah sido enviada
            switch ( accion->id )
            {
                case 'v':
                        escenarioCliente.vidas[accion->posicion]= accion->x;
                        break;
                        
                case 'j':
                        escenarioCliente.jugadores[accion->posicion] = Coordenada( accion->x, accion->y);
                        break;
                
                case 'f':
                        escenarioCliente.paredesFijas.push_back( Coordenada( accion->x, accion->y) );
                        break;
                    
                case 'd':
                    
                        // Efectua una de las sigueintes acciones sobre el vector correspondiente dependiendo del parametro enviado por el servidor:
                        //      x negativa: eliminar del vector la posicion indicada por la estructura
                        //      x positiva:
                        //                  posicion existente:     actualiza dicho valores
                        //                  posicion no existente:  introduce dicho valor al final del vector
                        //
                        
                        if ( escenarioCliente.paredesDestruibles.size() <=  accion->posicion )
                        {
                            escenarioCliente.paredesDestruibles.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.paredesDestruibles[accion->posicion] =  Coordenada( accion->x, accion->y);
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it = escenarioCliente.paredesDestruibles.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.paredesDestruibles.erase( it )  ;
                             }
                            
                        }    
                        
                        
                        break;
                        
                
                case 'b':
                    
                        // idem anterior
                    
                        if ( escenarioCliente.bombas.size() <=  accion->posicion )
                        {
                            escenarioCliente.bombas.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.bombas[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it =escenarioCliente.bombas.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.bombas.erase( it )  ;
                            }
                            
                        }                     

                        
                        break;
                
                case 'e':
                        
                        // Efectua una de las sigueintes acciones sobre el vector correspondiente dependiendo del parametro enviado por el servidor:
                        //      x negativa: eliminar del vector la posicion indicada por la estructura ( una explosion entera )
                        //      x positiva:
                        //                  posicion existente:     agrega un punto mas en el plano con explosion asignandocela 
                        //                                          a la explosion que le corresponda por posicion en el vector
                        //
                        //                  posicion no existente:  introduce dicho valor al final del vector como una nueva explosion

                    
                        if ( escenarioCliente.explosiones.size() <=  accion->posicion )
                        {
                            vector < Coordenada >  aux ;
                            aux.push_back( Coordenada( accion->x, accion->y) );
                            escenarioCliente.explosiones.push_back( aux );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.explosiones[accion->posicion].push_back(  Coordenada( accion->x, accion->y) ) ;
                            }
                            else
                            {
                                vector < vector < Coordenada >  > :: iterator  it =escenarioCliente.explosiones.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.explosiones.erase( it )  ;
                            }
                            
                        }      
                    
                       
                        break;
                
                case 't':
                    
                        escenarioCliente.tiempo = accion -> x;
                        break;
                
                
                case 'B':
                    
                        // idem anterior
                    
                        if ( escenarioCliente.premiosBomba.size() <=  accion->posicion )
                        {
                            escenarioCliente.premiosBomba.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.premiosBomba[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it = escenarioCliente.premiosBomba.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.premiosBomba.erase( it )  ;
                            }
                            
                        }                     

                        break;
                
                case 'V':
                    
                        // idem anterior                    
                    
                        if ( escenarioCliente.premiosVida.size() <=  accion->posicion )
                        {
                            escenarioCliente.premiosVida.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.premiosVida[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it = escenarioCliente.premiosVida.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.premiosVida.erase( it )  ;
                            }
                            
                        }                           
                    
                    
                        break;
                
                case 'E':

                        // idem anterior
                        
                        if ( escenarioCliente.premiosExplosion.size() <=  accion->posicion )
                        {
                            escenarioCliente.premiosExplosion.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.premiosExplosion[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it = escenarioCliente.premiosExplosion.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.premiosExplosion.erase( it )  ;
                            }
                            
                        }                           
                    
                    
                        break;
                                    
                
                case 'R':

                        // idem anterior
                        
                        if ( escenarioCliente.premiosVelocidad.size() <=  accion->posicion )
                        {
                            escenarioCliente.premiosVelocidad.push_back( Coordenada( accion->x, accion->y) );
                        }
                        else
                        {
                             
                            if (  accion->x != -1)
                            {
                                escenarioCliente.premiosVelocidad[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                            }
                            else
                            {
                                vector < Coordenada > :: iterator  it = escenarioCliente.premiosVelocidad.begin();
                                it += accion->posicion -1 ;
                                escenarioCliente.premiosVelocidad.erase( it )  ;
                            }
                            
                        }                           
                    
                    
                        break;
                               
                

            }
            

}

void  Bomberman ::ComunicarServidor()
{
    // ni la mas palida de q va aca, es mas, creo q esto no va
}

#endif