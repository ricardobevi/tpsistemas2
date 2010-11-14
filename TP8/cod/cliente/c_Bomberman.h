#ifndef C_BOMBERMAN_H
#define C_BOMBERMAN_H

#include <fstream>
#include <ncurses.h>
#include <string>
#include "entorno.h"
#include "c_escenario.h"

#include "../include/t_protocolo.h"
#include "../include/Connection.h"
#include "../include/MemCompartida.h"

using namespace std;

class Bomberman
{
    private:
        
        Connection       < char >     connectionCliente;
        Entorno          entornoCliente;
        MemCompartida    memCompartida;
        
        vector < pair <int,int> >   puestos;
        
        
        string ipServidor ;
        unsigned int puerto;
        int timeOut;
        
        
        int     tecla_arriba,
                tecla_abajo,
                tecla_izquierda,
                tecla_derecha,
                tecla_bomba;
    
    
        int idJugador;
        
    public:
         
         Escenario escenarioCliente;                           // Objeto que identifica el estado actual de la pantalla
                                                               // (posicion de todos sus elementos)
         Bomberman();
         ~Bomberman();
         
         int  leerTeclado( int tipoTeclado = 1);
         void set_idJugador( int jugador );
         int get_idJugador();
         void dibujarPantalla();
         void recibirAccion(t_protocolo * accion, size_t tam );
         int actualizarNovedades( t_protocolo * accion );
         void enviarSolicitud ( int teclaPresionada);
         void ComunicarServidor();
         
         void finalizarBomberman(void);
         
         bool espectador(void);
         
         void esperaDeJugadores(  int timeout  );
         
         int get_timeout();
         
         int jugadorLocal();
                   
};


int Bomberman :: jugadorLocal()
{
    /*
    
    //get ipServidor
    
    system( "ifconfig -a | grep inet | "
          "sed 's/\\([ ]*[^ ]*\\)\\([ ]*[^ ]*\\).*$/\\1 \\2/' "
          " > address.txt" ) ;
    
    ipServidor != "localhost" && ipServidor != "127.0.0.1" ) // me falta configurar el caso de que sea mi ip real
          
    */
    return 1;
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
        getch();
        exit(1);
    }

    configuracion >> ipServidor ;
    configuracion >> puerto;
    configuracion >> timeOut;
    
    
    string  arch_tecla_arriba,
            arch_tecla_abajo,
            arch_tecla_izquierda,
            arch_tecla_derecha,
            arch_tecla_bomba;
    
    
    configuracion >> arch_tecla_arriba;
    if ( arch_tecla_arriba ==  "flecha_arr" )
        tecla_arriba = KEY_UP;
    else
        tecla_arriba =  ( int ) *arch_tecla_arriba.c_str();  
    
    
    
    configuracion >> arch_tecla_abajo;
     if ( arch_tecla_abajo ==  "flecha_aba" )
         tecla_abajo = KEY_DOWN;
     else
         tecla_abajo =  ( int ) *arch_tecla_abajo.c_str();
     
     
    configuracion >> arch_tecla_izquierda;
     if ( arch_tecla_izquierda ==  "flecha_izq" )
         tecla_izquierda =  KEY_LEFT;
     else
         tecla_izquierda = ( int ) * arch_tecla_izquierda.c_str();
      
     
    configuracion >> arch_tecla_derecha;
     if ( arch_tecla_derecha ==  "flecha_der" )
         tecla_derecha =  KEY_RIGHT;
     else
         tecla_derecha =  ( int ) * arch_tecla_derecha.c_str();
         
     
     configuracion >> arch_tecla_bomba;
     if (  arch_tecla_bomba ==  "espacio" )
            tecla_bomba = ' ' ;
     else
            tecla_bomba = ( int ) * arch_tecla_bomba.c_str();
     
     
    configuracion.close();
    

    
    if( ! this -> jugadorLocal() )  
    {
            conexionExitosa = connectionCliente.Connect( ipServidor , puerto );
    }
    else
    {
            memCompartida.CargarMemCompartida( CLIENTE );
            memCompartida.conectarce();
            conexionExitosa = 1;
    }

    
    if ( conexionExitosa <= -1 )
    {
        ofstream error("errores.err");
        error << endl <<"ERROR: Fallo la conexion al socket o el servidor no esta activo " << endl ;
        error.close();
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
    entornoCliente.finalizarPantalla();
    
    if( ! this -> jugadorLocal() )
        connectionCliente.Close();
    else
        memCompartida.eliminarMemoriaCompartida();
   
}


// lee la tecla presionada por el usuario y devuelve la misma como resultado ( por ahora solo eso )
int  Bomberman ::leerTeclado( int tipoTeclado )
{
    int aux;
    usleep( 300 );
    //fflush(stdin);
    
    if ( tipoTeclado )
    {
        aux = getchar();
        //aux = getch();
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
void Bomberman :: recibirAccion(t_protocolo * accion, size_t tam  )
{
    
    // aca va el recv de tipo protocolo
    if ( ! this->jugadorLocal() )
            connectionCliente.Recv( (char*) accion, tam );
    else
            memCompartida.recibirDeServidor( *accion );
    
            
}


// envia la tecla que se presiono al servidor a travez de 
// un objeto t_protocolo:
//                          id = 'i' de informe
//                          x  = tecla presionada
void  Bomberman :: enviarSolicitud ( int teclaPresionada )
{
        t_protocolo solicitud;
        
        if( teclaPresionada == tecla_arriba    )
            solicitud.x  = 'w';
        else if( teclaPresionada ==  tecla_abajo    )
            solicitud.x  = 's';
        else if( teclaPresionada == tecla_izquierda )
            solicitud.x  = 'a';
        else if( teclaPresionada ==  tecla_derecha  )
            solicitud.x  = 'd';
        else if(  teclaPresionada == tecla_bomba    )
            solicitud.x  = 'b';
        else if ( teclaPresionada == -1 )
            solicitud.x = -1;
        else
            return;

        solicitud.id = 'i';  
        solicitud.y = 0 ; 
        solicitud.posicion = 0 ;

        //send de tipo protocolo
        if ( ! this->jugadorLocal() )
            connectionCliente.Send( (char*) &solicitud, sizeof(t_protocolo) );
        else
            memCompartida.enviarAServidor( solicitud );
        
    
}



// Actualiza el escenarioCliente dependiendo de las novedades que hayan llegado 
int Bomberman :: actualizarNovedades( t_protocolo * accion )
{
    
    int devolucion = 0;
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
            
            
            // el siguiente switch determina que operaciones deben realizarce en base a que novedad ah sido enviada
            switch ( accion->id )
            {
                case 'v':
                    
                        if(accion->posicion <  escenarioCliente.vidas.size() )
                            escenarioCliente.vidas[accion->posicion]= accion->x;
         
                        break;
                        
                case 'j':
                    
                        if( accion->posicion < escenarioCliente.jugadores.size() )
                            escenarioCliente.jugadores[accion->posicion].set_coordenada( accion->x, accion->y);
                        
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
                        
                        
                 case 'P':
                        // los puestos son enviados 4 3 2 1 
                        //Jugador Numero :  x    puesto numero : y
                       
                        puestos.push_back(  pair <int,int> (accion->x , accion->y ) );
                       
                        break;
                        
                  case 'F':
                        
                        entornoCliente.finDePartida( puestos );
                        getchar();
                        devolucion = 1;
                        
                        break;        
                  
                        
                  case 'i':
                    
                        set_idJugador( accion -> x );
                        
                        break;      

            }
            
            
          return devolucion;  

}


int  Bomberman :: get_timeout()
{
    return this-> timeOut;
}



void Bomberman ::  esperaDeJugadores(  int timeout  )
{    
    
    entornoCliente.dibujarTimeOut( timeout );
           
}


bool Bomberman :: espectador(void)
{
    bool espectador = false;
    
    if (idJugador <= -1)
        espectador = true;
    
    return espectador;
}



// carga el valor del numero de jugador ( 1-2-3-4 ) en idJugador
void Bomberman :: set_idJugador( int jugador )
{
    entornoCliente.set_idJugador( jugador );
    idJugador = jugador ;
}



int Bomberman :: get_idJugador(){
    return this->idJugador;
    
}
       

#endif