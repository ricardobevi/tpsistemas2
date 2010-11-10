#ifndef ENTORNO_H
#define ENTORNO_H

#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "c_escenario.h"

using namespace std;

class Entorno
{
    private:
       
         WINDOW *  pantalla;                        // puntero a el objeto pantalla - propio de ncurses
         int startx, starty , filas , columnas;     // datos necesarios para iniciar la pantalla, como filas columnas, y posicion de inicio del cursor
         Escenario * escenarioActual;               // puntero al escenario desde donde deben levantarce los datos de la pantalla
         int idJugador;
    public:
        
         Entorno();
         void cargarEntorno(Escenario * escenario);  // constructor de pantalla: inicializa todos los valores y caracteristicas de la pantalla
                                                     // luego actualiza la pantalla y comienza a esperar lecturas de teclado
                                                    
         void actualizarPantalla( void );            // borra el contenido de la pantalla y lo redibuja con el contenido de escenarioActual
         void finalizarPantalla( void );             // Elimina la pantalla
         
         ~Entorno ();
         
         void dibujarTimeOut(int timeout);
         
         void set_idJugador( int idJugador );
         
         void finDePartida(  vector <  pair <int,int>  >  & puestos  );
    

                           
};



void  Entorno ::  finDePartida(  vector < pair <int,int> >  & puestos  )
{
    
 
    delwin(pantalla);         
    pantalla = newwin(filas,columnas,starty, startx);
    box(pantalla,0 ,0);
    
            wattron(pantalla,COLOR_PAIR(10));
            mvwprintw(pantalla, 1, 7, " (   ( /(   (  `      (        )\\ )  (  `     (      ( /(           "); 
            mvwprintw(pantalla, 2, 7, " ( )\\  )\\())  )\\))(   ( )\\  (   (()/(  )\\))(    )\\     )\\())  "); 
            mvwprintw(pantalla, 3, 7, " )((_)((_)\\  ((_)()\\  )((_) )\\   /(_))((_)()\\((((_)(  ((_)\\     "); 
            mvwprintw(pantalla, 4, 7, " (_)_   ((_) (_()((_)((_)_ ((_) (_))  (_()((_))\\ _ )\\  _((_)      "); 
            wattroff(pantalla,COLOR_PAIR(10));
            
            wattron(pantalla,COLOR_PAIR(9));
            mvwprintw(pantalla, 5, 7, " | _ ) / _ \\ |  \\/  | | _ )| __|| _ \\ |  \\/  |(_)_\\(_)| \\| |   "); 
            mvwprintw(pantalla, 6, 7, " | _ \\| (_) || |\\/| | | _ \\| _| |   / | |\\/| | / _ \\  | .` |    "); 
            mvwprintw(pantalla, 7, 7, " |___/ \\___/ |_|  |_| |___/|___||_|_\\ |_|  |_|/_/ \\_\\ |_|\\_|    "); 
            wattroff(pantalla,COLOR_PAIR(9));
            
    
    
    
    wattron(pantalla,COLOR_PAIR(10));
    mvwprintw(pantalla,20 , 43  ,"Presione una tecla para finalizar " );
    wattroff(pantalla,COLOR_PAIR(10));
    
    int jugador;
    int posX = 9, posY = 15;
    int pos;
    while ( puestos.size() > 0 )
    {
        jugador = (puestos.back()).first;
        pos     = (puestos.back()).second;
        
       
              switch ( pos )
            {
                case 1:                      
                        wattron(pantalla, COLOR_PAIR(10));
                        mvwprintw(pantalla,posX , 20  ,"Primer Puesto");
                        wattroff(pantalla,COLOR_PAIR(10));  
                        break;
                    
                case 2 :
                        wattron(pantalla,COLOR_PAIR(10));
                        mvwprintw(pantalla,posX , 20  ,"Segundo Puesto");
                        wattroff(pantalla,COLOR_PAIR(10));  
                        break;
                    
                case 3 :        
                 
                        wattron(pantalla,COLOR_PAIR(10));
                        mvwprintw(pantalla,posX , 20  ,"Tercer Puesto");
                        wattroff(pantalla,COLOR_PAIR(10));  
                        break;
                    
                case 4 :
                        wattron(pantalla,COLOR_PAIR(10));
                        mvwprintw(pantalla,posX , 20  ,"Cuarto Puesto");
                        wattroff(pantalla,COLOR_PAIR(10));  
                        break;
            }
            
            
            switch ( jugador )
            {
                case 0 :                      
                    // jugador1 : Rojo
                    wattron(pantalla,COLOR_PAIR(1));
                    mvwprintw(pantalla,posX      , posY  ,"^^");
                    mvwprintw(pantalla,posX + 1  , posY  ,"\\/");
                    wattroff(pantalla,COLOR_PAIR(1));
                    break;
                    
                case 1 :
                    // jugador3 : Azul
                    wattron(pantalla,COLOR_PAIR(2));
                    mvwprintw(pantalla,posX ,posY ,"^^" );
                    mvwprintw(pantalla,posX + 1, posY  ,"\\/");
                    wattroff(pantalla,COLOR_PAIR(2));
                    break;
                    
                case 2 :        
                    // jugador3 : Amarillo
                    wattron(pantalla,COLOR_PAIR(3));
                    mvwprintw(pantalla,posX    , posY  ,"^^");
                    mvwprintw(pantalla,posX + 1, posY  ,"\\/");
                    wattroff(pantalla,COLOR_PAIR(3));
                    break;
                    
                case 3 :
                    // jugador4 : Verde 
                    wattron(pantalla,COLOR_PAIR(4));
                    mvwprintw(pantalla,posX     , posY  ,"^^");
                    mvwprintw(pantalla,posX + 1 , posY  ,"\\/");
                    wattroff(pantalla,COLOR_PAIR(4));
                    break;
            }
            
            puestos.pop_back();
            posX += 3 ;
    
    }

                      
    wrefresh( pantalla );    
    refresh();

   
}


void Entorno ::  dibujarTimeOut(int timeout)
{
    //actualizo pantalla            
    if ( timeout == -1 ) 
    {
        attron(COLOR_PAIR(2));
        mvwprintw(pantalla, 11, 25, " Espere mientras el cliente se conecta y carga el escenario");
        attroff(COLOR_PAIR(2));
    }
    else
    {
            wattron(pantalla,COLOR_PAIR(10));
            mvwprintw(pantalla, 2, 7, "            )     *                (       *                )        "); 
            mvwprintw(pantalla, 3, 7, " (   ( /(   (  `      (        )\\ )  (  `     (      ( /(           "); 
            mvwprintw(pantalla, 4, 7, " ( )\\  )\\())  )\\))(   ( )\\  (   (()/(  )\\))(    )\\     )\\())  "); 
            mvwprintw(pantalla, 5, 7, " )((_)((_)\\  ((_)()\\  )((_) )\\   /(_))((_)()\\((((_)(  ((_)\\     "); 
            mvwprintw(pantalla, 6, 7, " (_)_   ((_) (_()((_)((_)_ ((_) (_))  (_()((_))\\ _ )\\  _((_)      "); 
            wattroff(pantalla,COLOR_PAIR(10));
            
            wattron(pantalla,COLOR_PAIR(9));
            mvwprintw(pantalla, 7, 7, " | _ ) / _ \\ |  \\/  | | _ )| __|| _ \\ |  \\/  |(_)_\\(_)| \\| |   "); 
            mvwprintw(pantalla, 8, 7, " | _ \\| (_) || |\\/| | | _ \\| _| |   / | |\\/| | / _ \\  | .` |    "); 
            mvwprintw(pantalla, 9, 7, " |___/ \\___/ |_|  |_| |___/|___||_|_\\ |_|  |_|/_/ \\_\\ |_|\\_|    "); 
            wattroff(pantalla,COLOR_PAIR(9));
         
            wattron(pantalla,COLOR_PAIR(10));
            mvwprintw(pantalla, 13, 25, "ESPERANDO A JUGADORES" );
            mvwprintw(pantalla, 15, 35, "%d" , timeout);
            wattroff(pantalla,COLOR_PAIR(10));
            
            wattron(pantalla,COLOR_PAIR(11));
            mvwprintw(pantalla, 20, 17, "creditos: Facundo - Marcela - Ricardo" );
            wattroff(pantalla,COLOR_PAIR(11));
                   
    }
  
    refresh();
    wrefresh( pantalla ); 
    
}


void Entorno :: set_idJugador( int idJugador){
    this -> idJugador = idJugador;
}
               
Entorno :: Entorno()
{
    
}

// inicializa el entorno
void Entorno :: cargarEntorno ( Escenario * escenario )
{
    
   escenarioActual  =  escenario;
   pantalla = NULL;     

   filas = 22;
   columnas = 78;
   starty = 1;  
   startx = 1;   
   
   
   // inicializa caracteristicas del ncurses
   //clearok(pantalla,true);
   //nodelay(stdscr, TRUE);
   initscr();  
   cbreak();
   noecho();
   curs_set(0);
   nonl();
   keypad(stdscr, TRUE);

   
   
   // inicializa los colores a utilizar
   start_color();
   
   init_pair(1, COLOR_BLACK, COLOR_RED     );        //jugador1
   init_pair(2, COLOR_BLACK, COLOR_BLUE    );        //jugador2
   init_pair(3, COLOR_BLACK, COLOR_YELLOW  );        //jugador3
   init_pair(4, COLOR_BLACK, COLOR_GREEN   );        //jugador4
   init_pair(5, COLOR_BLACK, COLOR_WHITE   );        //pared fija
   init_pair(6, COLOR_BLACK, COLOR_CYAN    );        //pared destruible
   init_pair(7, COLOR_WHITE, COLOR_MAGENTA );        //premios
   init_pair(8, COLOR_YELLOW, COLOR_RED     );        //Bomba y Explosion
   init_pair(9, COLOR_RED, COLOR_BLACK     );         // bomberman inicio
   init_pair(10, COLOR_YELLOW, COLOR_BLACK     );         // bomberman inicio
   init_pair(11, COLOR_WHITE, COLOR_BLACK     );         // bomberman inicio
   
   
   pantalla = newwin(filas,columnas,starty, startx);
   
   box(pantalla,0 ,0);      
   
   // descomentar la siguiente linea permite ver que el entorno se crea con exito 
   //ubicando la pantalla y los jugadores en sus respectivas posiciones
   
   //actualizarPantalla();

    
}

// Destructor, elimina los contenidos dinamicos ( por ahora ninguno )
Entorno :: ~Entorno ()
{
}

// Elimina la pantalla
void Entorno :: finalizarPantalla( void )
{
    endwin();
}



// borra el contenido de la pantalla y lo redibuja con el contenido de escenarioActual
void Entorno :: actualizarPantalla( void )
{
    
    //----------------------------------- variables e iteradores auxiliares para la actualizacion de la pantalla ------------------------ //
    int x,y;
    vector < Coordenada > :: iterator it;
    map< int, Coordenada >::iterator mit;
    int posX, posY;
    //----------------------------------- FIN variables e iteradores auxiliares para la actualizacion de la pantalla -------------------- //
    
    
    
    
     //------------------------------------------ Cabecera de pantalla ( vidas y tiempo de juego ) --------------------------------------//
     
    
       
    // jugador1 : Rojo
    attron(COLOR_PAIR(1));
    mvprintw(0,1,"Jug 1: %0.2d", (escenarioActual->vidas).at(0) );
    attroff(COLOR_PAIR(1));
    
    // jugador3 : Azul
    attron(COLOR_PAIR(2));
    mvprintw(0,16,"Jug 2: %0.2d" ,(escenarioActual->vidas).at(1) );
    attroff(COLOR_PAIR(2));
   
    mvprintw(0,36,"Time %0.2d", escenarioActual->tiempo);
         
    // jugador3 : Amarillo
    attron(COLOR_PAIR(3));
    mvprintw(0,53,"Jug 3: %0.2d" ,(escenarioActual->vidas).at(2));
    attroff(COLOR_PAIR(3));

    // jugador4 : Verde 
    attron(COLOR_PAIR(4));
    mvprintw(0,68,"Jug 4: %0.2d", (escenarioActual->vidas).at(3));
    attroff(COLOR_PAIR(4));
    
    //------------------------------------------ FIN Cabecera de pantalla ( vidas y tiempo de juego ) --------------------------------------//
    
   
   
   
   
   
   //-------------------------------------------- Creacion de la ventana vacia y su recuadro -----------------------------------------------//
   if ( pantalla != NULL ) 
                            delwin(pantalla); 
    
    pantalla = newwin(filas,columnas,starty, startx);
   
    box(pantalla,0 ,0);
   //--------------------------------------------- FIN Creacion de la ventana vacia y su recuadro ------------------------------------------//
   
   
   
   
   
   
   
   //----------------------------------------------- Actualizo Posicion de los jugadores ---------------------------------------------------//
   /*
        A tener en cuenta: para trasformar la coordenada a la matriz real de 23*78 se realiza una trasnformacion lineal  (2*x+1 , 2*y+1)
                           siendo la matriz aparente de (0,0) ~ (9,37)
   */

    for( unsigned i = 0 ; i < escenarioActual->jugadores.size() ; i++ ){
        
        posX = (escenarioActual->jugadores).at(i).get_x() ;
        posY = (escenarioActual->jugadores).at(i).get_y() ;

        posX = 2 * posX + 1;
        posY = 2 * posY + 1;

        wattron(pantalla,COLOR_PAIR(i + 1));
        
        //mvwprintw(pantalla,posX , posY  ,"^^");
        //mvwprintw(pantalla,posX + 1 ,posY  ,"\\/");

        mvwprintw(pantalla,posY , posX  ,"^^");
        mvwprintw(pantalla,posY + 1 ,posX  ,"\\/");
        
        wattroff(pantalla,COLOR_PAIR(i + 1));
    }
    
    //----------------------------------------------- FIN Actualizo Posicion de los jugadores ------------------------------------------------//
     
     

     
     
    //------------------------------------------------------  Actualizo Paredes Fijas------ --------------------------------------------------//
    // paredes fijas
   
    wattron( pantalla,COLOR_PAIR(5) );
    for(it = escenarioActual->paredesFijas.begin() ; it != escenarioActual->paredesFijas.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;
    
        mvwprintw(pantalla, y    , x, "  ");
        mvwprintw(pantalla, y + 1, x, "  ");
    }
    wattroff(pantalla,COLOR_PAIR(5));   
    
    //------------------------------------------------------ FIN Actualizo Paredes Fijas-----------------------------------------------------//
    
    
    
    
     //------------------------------------------------------  Actualizo Paredes destruibles-------------------------------------------------//
    // paredes destruibles
    
    wattron( pantalla,COLOR_PAIR(6) );
    for(it = escenarioActual->paredesDestruibles.begin() ; it != escenarioActual->paredesDestruibles.end() ; it++)
    {
        if ( it->get_x() >= 0 ){

            x = 2 * it->get_x() +1;
            y = 2 * it->get_y() +1;

            mvwprintw(pantalla, y    , x, "xx");
            mvwprintw(pantalla, y + 1, x, "xx");
            
        }
        
    }
    wattroff(pantalla,COLOR_PAIR(6));
    //------------------------------------------------------ FIN Actualizo Paredes destruibles---------------------------------------------//





    //------------------------------------------------------  Actualizo Bombas colocadas en pantalla---------------------------------------//
    // Bombas

    wattron(pantalla,COLOR_PAIR(8));
   
    for(mit = escenarioActual->bombas.begin() ; mit != escenarioActual->bombas.end() ; mit++)
    {
        x = 2 *  (*mit).second.get_x() +1;
        y = 2 *  (*mit).second.get_y() +1;

        
        //mvwchgat(pantalla,  x,  y, 2, A_BLINK, 1,NULL );
        mvwprintw(pantalla, y  ,x, "/\\");
        //mvwchgat(pantalla,  x+1,y, 2, A_BLINK, 1,NULL );
        mvwprintw(pantalla, y+1,x, "\\/");
        
        
    }
    wattroff(pantalla,COLOR_PAIR(8));
    //------------------------------------------------------ FIN  Actualizo Bombas colocadas en pantalla-----------------------------------//
    
    
    
    
    //------------------------------------------------------  Actualizo explosiones en pantalla --------------------------------------------//
    // Explosiones
    
    wattron(pantalla,COLOR_PAIR(1));

    map< int, vector< Coordenada > >::iterator meit;
    
    for(meit = escenarioActual->explosiones.begin() ; meit != escenarioActual->explosiones.end() ; meit++)
    {
        
        if (  (*meit).second[0].coordenadaBandera()  )   // si la posicion cero es distinta de -1 -1 significa que
        {                                                                 // la explosion esta completa y debo imprimirla en pantalla
        
                for( unsigned puntoExplosion  = 0 ; puntoExplosion < (*meit).second.size() ; puntoExplosion++)
                {
           
                        (*meit).second[ puntoExplosion ].get_coordenada(x,y) ;

                        x = 2 * x + 1;
                        y = 2 * y + 1;
                       
                        mvwprintw(pantalla, y    ,x, "  ");
                        mvwprintw(pantalla, y+1  ,x, "  ");
                }   
        }
        
    }
    
    wattroff(pantalla,COLOR_PAIR(1));
    
    //------------------------------------------------------ FIN Actualizo explosiones en pantalla ---------------------------------------//





    //-------------------------------------------------------  Actualizo Premios en pantalla ---------------------------------------------//
    //PREMIOS
    
    wattron( pantalla,COLOR_PAIR(7) );
    
    //premios de vida
    for(mit = escenarioActual->premiosVida.begin() ; mit != escenarioActual->premiosVida.end() ; mit++)
    {
        x = 2 * mit->second.get_x() +1;
        y = 2 * mit->second.get_y() +1;

        mvwprintw(pantalla, y  ,x, "VI");
        mvwprintw(pantalla, y+1,x, "DA");
        
    }
    
    //premios de explosion
    for(mit = escenarioActual->premiosExplosion.begin() ; mit != escenarioActual->premiosExplosion.end() ; mit++)
    {
        x = 2 * mit->second.get_x() +1;
        y = 2 * mit->second.get_y() +1;

        mvwprintw(pantalla, y  ,x, "/\\");
        mvwprintw(pantalla, y+1,x, "\\/");
        
    }
    
    //premios de bombas
    for(mit = escenarioActual->premiosBomba.begin() ; mit != escenarioActual->premiosBomba.end() ; mit++)
    {
        x = 2 * mit->second.get_x() +1;
        y = 2 * mit->second.get_y() +1;

        mvwprintw(pantalla, y  ,x, "BO");
        mvwprintw(pantalla, y+1,x, "MB");
        
    }
    
    //premios de velocidad
    for(mit = escenarioActual->premiosVelocidad.begin() ; mit != escenarioActual->premiosVelocidad.end() ; mit++)
    {
        x = 2 * mit->second.get_x() +1;
        y = 2 * mit->second.get_y() +1;

        mvwprintw(pantalla, y  ,x, ">>");
        mvwprintw(pantalla, y+1,x, ">>");
        
    }
    
    wattroff(pantalla,COLOR_PAIR(7));
    
    if( idJugador < 0 )
      mvwprintw(pantalla,21,4, "  Modo Espectador : usted solo puede ver como se desarrolla el juego  ");
    
     //--------------------------------------------------  FIN Actualizo Premios en pantalla ---------------------------------------------//
   
    //mvwprintw(pantalla, 21,0, " ERROR >>  ");

    //actualizo pantalla  

    refresh();
    wrefresh( pantalla ); 

    //  -------------------------------------------------- FIN LEO EL ESCENARIO Y ACTUALIZO LA PANTALLA--------------------------------- //

}



#endif

