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
        
    public:
        
         Entorno();
         void cargarEntorno(Escenario * escenario);  // constructor de pantalla: inicializa todos los valores y caracteristicas de la pantalla
                                                     // luego actualiza la pantalla y comienza a esperar lecturas de teclado
                                                    
         void actualizarPantalla( void );            // borra el contenido de la pantalla y lo redibuja con el contenido de escenarioActual
         void finalizarPantalla( void );             // Elimina la pantalla
         
         ~Entorno ();
         
         int leerTeclado();                         // lee contantemente el teclado a la espera de que se presione una tecla,
                                                     //en dicho caso realiza la accion correspondiente
                           
};

int Entorno :: leerTeclado()
{
    int aux;
    
    fflush( stdin );
    aux = getch(); 

    return aux;
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
   initscr();  
   cbreak();
   noecho();
   curs_set(0);
   keypad(stdscr, TRUE);
   //nodelay(stdscr, TRUE);
   
   
   // inicializa los colores a utilizar
   start_color();
   
   init_pair(1, COLOR_BLACK, COLOR_RED     );        //jugador1
   init_pair(2, COLOR_BLACK, COLOR_BLUE    );        //jugador2
   init_pair(3, COLOR_BLACK, COLOR_YELLOW  );        //jugador3
   init_pair(4, COLOR_BLACK, COLOR_GREEN   );        //jugador4
   init_pair(5, COLOR_BLACK, COLOR_WHITE   );        //pared fija
   init_pair(6, COLOR_BLACK, COLOR_CYAN    );        //pared destruible
   init_pair(7, COLOR_WHITE, COLOR_MAGENTA );        //premios
   init_pair(8,COLOR_YELLOW, COLOR_RED     );        //Bomba y Explosion
      
   
   
   
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
    unsigned int x,y;
    vector < Coordenada > :: iterator it;
    vector < vector <Coordenada> > :: iterator explosion;
    unsigned int posX, posY;
    //----------------------------------- FIN variables e iteradores auxiliares para la actualizacion de la pantalla -------------------- //
    
    
    
    
     //------------------------------------------ Cabecera de pantalla ( vidas y tiempo de juego ) --------------------------------------//
     
     mvprintw(0,36,"Time %d", escenarioActual->tiempo);
       
    // jugador1 : Rojo
    attron(COLOR_PAIR(1));
    mvprintw(0,1,"Jug 1: %d", (escenarioActual->vidas).at(0) );
    attroff(COLOR_PAIR(1));
    
    // jugador3 : Azul
    attron(COLOR_PAIR(2));
    mvprintw(0,16,"Jug 2: %d" ,(escenarioActual->vidas).at(1) );
    attroff(COLOR_PAIR(2));

    // jugador3 : Amarillo
    attron(COLOR_PAIR(3));
    mvprintw(0,53,"Jug 3: %d" ,(escenarioActual->vidas).at(2));
    attroff(COLOR_PAIR(3));

    // jugador4 : Verde 
    attron(COLOR_PAIR(4));
    mvprintw(0,68,"Jug 4: %d", (escenarioActual->vidas).at(3));
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
        
        Aclaracion:        podria realizarce con un for() pero ya esta hecho asi 
   */

    // jugador1 : ROJO
    posX = (escenarioActual->jugadores).at(0).get_x() ;
    posY = (escenarioActual->jugadores).at(0).get_y() ;

    posX = 2 * posX +1;
    posY = 2 * posY +1;
    
    wattron(pantalla,COLOR_PAIR(1));
    mvwprintw(pantalla,posX , posY  ,"^^");
    mvwprintw(pantalla,posX + 1 ,posY  ,"\\/");
    wattroff(pantalla,COLOR_PAIR(1));
    
    // jugador2 :  AZUL
    
    posX = (escenarioActual->jugadores).at(1).get_x() ;
    posY = (escenarioActual->jugadores).at(1).get_y() ;
   
    posX = 2 * posX +1;
    posY = 2 * posY +1;
    
    wattron(pantalla,COLOR_PAIR(2));
    mvwprintw(pantalla,posX , posY  ,"^^");
    mvwprintw(pantalla,posX + 1 ,posY  ,"\\/");
    wattroff(pantalla,COLOR_PAIR(2));
    
    // jugador3 : AMARILLO
    posX = (escenarioActual->jugadores).at(2).get_x() ;
    posY = (escenarioActual->jugadores).at(2).get_y() ;
   
    posX = 2 * posX +1;
    posY = 2 * posY +1;
    
    wattron(pantalla,COLOR_PAIR(3));
    mvwprintw(pantalla,posX , posY  ,"^^");
    mvwprintw(pantalla,posX + 1 ,posY  ,"\\/");
    wattroff(pantalla,COLOR_PAIR(3));
    
    // jugador4 : VERDE
    posX = (escenarioActual->jugadores).at(3).get_x() ;
    posY = (escenarioActual->jugadores).at(3).get_y() ;
   
    posX = 2 * posX +1;
    posY = 2 * posY +1;
    
    wattron(pantalla,COLOR_PAIR(4));
    mvwprintw(pantalla,posX , posY  ,"^^");
    mvwprintw(pantalla,posX + 1 ,posY  ,"\\/");
    wattroff(pantalla,COLOR_PAIR(4));

    //----------------------------------------------- FIN Actualizo Posicion de los jugadores ------------------------------------------------//
     
     

     
     
    //------------------------------------------------------  Actualizo Paredes Fijas------ --------------------------------------------------//
    // paredes fijas
   
    wattron( pantalla,COLOR_PAIR(5) );
    for(it = escenarioActual->paredesFijas.begin() ; it != escenarioActual->paredesFijas.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;
    
        mvwprintw(pantalla, x  ,y, "XX");
        mvwprintw(pantalla, x+1,y, "XX");  
    }
    wattroff(pantalla,COLOR_PAIR(5));   
    
    //------------------------------------------------------ FIN Actualizo Paredes Fijas-----------------------------------------------------//
    
    
    
    
     //------------------------------------------------------  Actualizo Paredes destruibles-------------------------------------------------//
    // paredes destruibles
    
    wattron( pantalla,COLOR_PAIR(6) );
    for(it = escenarioActual->paredesDestruibles.begin() ; it != escenarioActual->paredesDestruibles.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "  ");
        mvwprintw(pantalla, x+1,y, "  ");  
        
    }
    wattroff(pantalla,COLOR_PAIR(6));
    //------------------------------------------------------ FIN Actualizo Paredes destruibles---------------------------------------------//





    //------------------------------------------------------  Actualizo Bombas colocadas en pantalla---------------------------------------//
    // Bombas

    wattron(pantalla,COLOR_PAIR(8));
    for(it = escenarioActual->bombas.begin() ; it != escenarioActual->bombas.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        
        //mvwchgat(pantalla,  x,  y, 2, A_BLINK, 1,NULL );
        mvwprintw(pantalla, x  ,y, "/\\");
        //mvwchgat(pantalla,  x+1,y, 2, A_BLINK, 1,NULL );
        mvwprintw(pantalla, x+1,y, "\\/");  
        
        
    }
    wattroff(pantalla,COLOR_PAIR(8));
    //------------------------------------------------------ FIN  Actualizo Bombas colocadas en pantalla-----------------------------------//
    
    
    
    
    //------------------------------------------------------  Actualizo explosiones en pantalla --------------------------------------------//
    // Explosiones
    
    wattron(pantalla,COLOR_PAIR(1));
    for(explosion = escenarioActual->explosiones.begin() ; explosion != escenarioActual->explosiones.end() ; explosion++)
    {
        for(it = explosion->begin() ; it != explosion->end() ; it++)
        {
       
            x = 2 * it->get_x() +1;
            y = 2 * it->get_y() +1;

            
            mvwprintw(pantalla, x  ,y, "  ");
            mvwprintw(pantalla, x+1,y, "  ");  
        }   
        
    }
    wattroff(pantalla,COLOR_PAIR(1));
    
    //------------------------------------------------------ FIN Actualizo explosiones en pantalla ---------------------------------------//





    //-------------------------------------------------------  Actualizo Premios en pantalla ---------------------------------------------//
    //PREMIOS
    
    wattron( pantalla,COLOR_PAIR(7) );
    
    //premios de vida
    for(it = escenarioActual->premiosVida.begin() ; it != escenarioActual->premiosVida.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "VI");
        mvwprintw(pantalla, x+1,y, "DA");  
        
    }
    
    //premios de explosion
    for(it = escenarioActual->premiosExplosion.begin() ; it != escenarioActual->premiosExplosion.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "++");
        mvwprintw(pantalla, x+1,y, "++");  
        
    }
    
    //premios de bombas
    for(it = escenarioActual->premiosBomba.begin() ; it != escenarioActual->premiosBomba.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "BO");
        mvwprintw(pantalla, x+1,y, "MB");  
        
    }
    
    //premios de velocidad
    for(it = escenarioActual->premiosVelocidad.begin() ; it != escenarioActual->premiosVelocidad.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, ">>");
        mvwprintw(pantalla, x+1,y, ">>");  
        
    }
    
    wattroff(pantalla,COLOR_PAIR(7));
     //--------------------------------------------------  FIN Actualizo Premios en pantalla ---------------------------------------------//
   
 

    //actualizo pantalla   
    refresh();
    wrefresh( pantalla ); 

    //  -------------------------------------------------- FIN LEO EL ESCENARIO Y ACTUALIZO LA PANTALLA--------------------------------- //

}

/*

// El siguiente metodo permite leer el teclado y enviar acciones al entorno directamente sin depender de un servidor.
// su creacion fue unicamente a modo de prueba para probar el correcto funcionamiento del entorno


// Objetivo: lee constantemente el teclado a la espera de que se presione una tecla,
// en dicho caso realiza la accion correspondiente


char Entorno :: leerTeclado()
{
 int tecla;
 unsigned int jugador = 0;
    
    while( (tecla = getch()) != 'q' )
    {  
        switch(tecla)
        {   
            case KEY_LEFT:
                
                (escenarioActual->jugadores).at( jugador ).get_y()--;
                
                break;
                
            case KEY_RIGHT:
                                
                (escenarioActual->jugadores).at( jugador ).get_y()++;
                
                break;
                
            case KEY_UP:
                
                (escenarioActual->jugadores).at( jugador ).get_x()--;
                
                break;
                
            case KEY_DOWN:
                
                (escenarioActual->jugadores).at( jugador ).get_x()++;
                
                break;  
        }
        
        actualizarPantalla();

    }
    
    finalizarPantalla( );
    
    return 'c';
        
}
*/



#endif

