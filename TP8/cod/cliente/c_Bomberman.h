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
        int timeOut;
        
        int idJugador;
        
    public:
         
         Escenario escenarioCliente;                           // Objeto que identifica el estado actual de la pantalla
                                                               // (posicion de todos sus elementos)

         Bomberman();
         ~Bomberman();
         
         int  leerTeclado( int tipoTeclado = 1);
         void set_idJugador( int jugador );
         void dibujarPantalla();
         void recivirAccion(t_protocolo * accion, size_t tam );
         void actualizarNovedades( t_protocolo * accion );
         void enviarSolicitud ( int teclaPresionada);
         void ComunicarServidor();
         
         void finalizarBomberman(void);
         
         bool espectador(void);
         
         void esperaDeJugadores(  int timeout  );
         
         int get_timeout();
                   
};


int  Bomberman :: get_timeout()
{
    return this-> timeOut;
}



void Bomberman ::  esperaDeJugadores(  int timeout  )
{
    entornoCliente.dibujarTimeOut(timeout);
}

bool Bomberman :: espectador(void)
{
    bool espectador =false;
    
    if (idJugador == -1)
        espectador = true;
    
    return espectador;
}

// envia la tecla que se presiono al servidor a travez de 
// un objeto t_protocolo:
//                          id = 'i' de informe
//                          x  = tecla presionada
void  Bomberman :: enviarSolicitud ( int teclaPresionada )
{
    t_protocolo solicitud;
    ofstream errores;
    
    solicitud.id = 'i';
    solicitud.x  = teclaPresionada ;
    solicitud.y = 0 ; 
    solicitud.posicion = 0 ;

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
    int conexionExitosa;
    
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
    
    conexionExitosa = connectionCliente.Connect( ipServidor , puerto );
    
    if ( conexionExitosa == -1  )
    {
        ofstream error("errores.err");
        error << endl <<"ERROR: Fallo la conexion al socket o el servidor no esta activo " << endl ;
        error.close();
        cout << endl <<"ERROR: Fallo la conexion al socket o el servidor no esta activo " << endl << endl << "El cliente ha finalizado" << endl << endl ;
        exit (0);
        
    }

    entornoCliente.cargarEntorno( &escenarioCliente );     
         
}


Bomberman :: ~Bomberman()
{
}

// Cierra correctamente la pantalla y la conexion del cliente
void Bomberman :: finalizarBomberman( void )
{
    this->enviarSolicitud(-1);
    connectionCliente.Close();
    entornoCliente.finalizarPantalla();
}


// lee la tecla presionada por el usuario y devuelve la misma como resultado ( por ahora solo eso )
int  Bomberman ::leerTeclado( int tipoTeclado )
{
    int aux;
    usleep( 250 );
    //fflush(stdin);
    
    if ( tipoTeclado )
    {
        aux = getchar();
    }
    else
    {
        //teclado automatico
        aux = rand() % 5;

        switch( aux ){
            case 0:
                aux = 'w';
                break;
            case 1:
                aux = 's';
                break;
            case 2:
                aux = 'a';
                break;
            case 3:
                aux = 'd';
                break;
            case 4:
                aux = 'b';
                break;
        }

        usleep(300000);
    }
    
    return aux;
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
    connectionCliente.Recv( (char*) accion, tam );
}



// Actualiza el escenarioCliente dependiendo de las novedades que hayan llegado 
void Bomberman :: actualizarNovedades( t_protocolo * accion )
{
     /*

                   v :     vector < int > vidas;
                   j :     vector < Coordenada > jugadores;
                   f :     vector < Coordenada > paredesFijas;
                   d :     vector < Coordenada > paredesDestruibles;
                   b :     vector < Coordenada > bombas;
                   e :     vector < vector < Coordenada >  > explosiones;  
                   t :     int    tiempo;
                   
                   B ;     vector < Coordenada > premiosBomba;
                   V :     vector < Coordenada > premiosVida;
                   E :     vector < Coordenada > premiosExplosion;
                   R :     vector < Coordenada > premiosVelocidad;
                   
                   F:      fin de partida: x-  numero de jugador -  y posicion
                   
                   a:      Actualizar explosion para mostrarla en pantalla 
                  

       */
            
            ofstream error("errores.err");
            // el siguiente switch determina que operaciones deben realizarce en base a que novedad ah sido enviada
            switch ( accion->id )
            {
                case 'v':
                    
                        if(accion->posicion <  escenarioCliente.vidas.size() )
                            escenarioCliente.vidas[accion->posicion]= accion->x;
                        else
                        {
                            error << endl <<"Fallo de segmentacion en vector: vidas"<< endl << "pos: " << accion->posicion << endl << "x: " << accion->x << endl << "y: "<< accion->y;
                            error.close();
                            kill(SIGINT, getpid() );
                        }
                            
                            
                        break;
                        
                case 'j':
                    
                        if( accion->posicion < escenarioCliente.jugadores.size() )
                            escenarioCliente.jugadores[accion->posicion].set_coordenada( accion->x, accion->y);
                        else
                        {
                            error << endl <<"Fallo de segmentacion en vector: jugadores"<< endl << "pos: " << accion->posicion << endl << "x: " << accion->x << endl << "y: "<< accion->y;
                            error.close();
                            kill(SIGINT, getpid() );
                        }
                        
                        break;
                
                case 'f':
                    
                        //if( accion->posicion >= escenarioCliente.paredesFijas.size() )
                            escenarioCliente.paredesFijas.push_back( Coordenada( accion->x, accion->y) );
                        /*else
                        {
                            error << endl <<"Fallo de segmentacion en vector: jugadores"<< endl << "pos: " << accion->posicion << endl << "x: " << accion->x << endl << "y: "<< accion->y;
                            error.close();
                            kill(SIGINT, getpid() );
                        }*/
                        
                        break;
                    
                case 'd':
                    
                        // Efectua una de las sigueintes acciones sobre el vector correspondiente dependiendo del parametro enviado por el servidor:
                        //      x negativa: eliminar del vector la posicion indicada por la estructura
                        //      x positiva:
                        //                  posicion existente:     actualiza dicho valores
                        //                  posicion no existente:  introduce dicho valor al final del vector
                        //
                        
                        if (  accion->posicion >= escenarioCliente.paredesDestruibles.size() )
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
                      
                        if (  accion->x != -1)
                        {
                            escenarioCliente.bombas[accion->posicion] =  Coordenada( accion->x, accion->y) ;
                        }
                        else
                        {
                            escenarioCliente.bombas.erase( accion->posicion )  ;
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

                             
                            if (  accion->x == -1) // eliminar una explosion
                            {
                                escenarioCliente.explosiones.erase( accion->posicion )  ;
                               
                            }
                            else
                            {
                                    // agrego cuadraditos a una explosion
                                     escenarioCliente.explosiones[ accion->posicion ].push_back( Coordenada( accion->x, accion->y) ); 
   
                            }

                       
                        break;
                
                case 'a':
                        
                        // informa fin de envio de explosion para mostrar en pantalla
                        escenarioCliente.explosiones[accion->posicion][0] = Coordenada( accion->x, accion->y) ; 
                        break;
                        
                case 't':
                    
                        escenarioCliente.tiempo = accion -> x;
                        break;
                
                
                case 'B':
                    
                        // idem anterior
                    
                        if (  accion->x == -1) // eliminar un premio Bomba
                        {
                            escenarioCliente.premiosBomba.erase( accion->posicion )  ;

                        }
                        else
                        {
                                // agrego un premio Bomba
                                 escenarioCliente.premiosBomba[ accion->posicion ] = Coordenada( accion->x, accion->y);

                        }

                        break;
                
                case 'V':
                    
                        // idem anterior

                        if (  accion->x == -1) // eliminar un premio Vida
                        {
                            escenarioCliente.premiosVida.erase( accion->posicion )  ;

                        }
                        else
                        {
                                // agrego un premio Vida
                                 escenarioCliente.premiosVida[ accion->posicion ] = Coordenada( accion->x, accion->y);

                        }
                    
                    
                        break;
                
                case 'E':

                        // idem anterior

                        if (  accion->x == -1) // eliminar un premio Explosion
                        {
                            escenarioCliente.premiosExplosion.erase( accion->posicion )  ;

                        }
                        else
                        {
                                // agrego un premio Explosion
                                 escenarioCliente.premiosExplosion[ accion->posicion ] = Coordenada( accion->x, accion->y);

                        }
                    
                    
                        break;
                                    
                
                case 'R':

                        // idem anterior

                        if (  accion->x == -1) // eliminar un premio Velocidad
                        {
                            escenarioCliente.premiosVelocidad.erase( accion->posicion )  ;

                        }
                        else
                        {
                                // agrego un premio Velocidad
                                 escenarioCliente.premiosVelocidad[ accion->posicion ] = Coordenada( accion->x, accion->y);

                        }
                    
                    
                        break;
                        
                        
                 case 'F':
                        
                        //Jugador Numero :  x      Puesto : y
                        //Jugador Numero :  x      Puesto : y
                        //Jugador Numero :  x      Puesto : y
                        //Jugador Numero :  x      Puesto : y
                       
                        break;

            }
            

}

void  Bomberman ::ComunicarServidor()
{
    // ni la mas palida de q va aca, es mas, creo q esto no va
}

#endif