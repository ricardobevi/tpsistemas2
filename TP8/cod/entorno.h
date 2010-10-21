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
       
         WINDOW *  pantalla;
         int startx, starty , filas , columnas;
         Escenario * escenarioActual;
        
    public:
        
         Entorno( Escenario * escenario );
         ~Entorno();
         
         void actualizarPantalla( void );
         void finalizarPantalla( void );
         void  leerTeclado();
                           
};


Entorno :: Entorno ( Escenario * escenario )
{
    
   escenarioActual  =  escenario;
   pantalla = NULL;     

   filas = 22;
   columnas = 78;
   starty = 1;  
   startx = 1;   
   
   initscr();  
   cbreak();
   noecho();
   curs_set(0);
   keypad(stdscr, TRUE);
   nodelay(stdscr, TRUE);
   start_color();
   
   init_pair(1, COLOR_BLACK, COLOR_RED);
   init_pair(2, COLOR_BLACK, COLOR_BLUE );
   init_pair(3, COLOR_BLACK, COLOR_YELLOW );
   init_pair(4, COLOR_BLACK, COLOR_GREEN );
   init_pair(5, COLOR_BLACK, COLOR_WHITE );
   init_pair(6, COLOR_BLACK, COLOR_CYAN);
   init_pair(7, COLOR_WHITE, COLOR_MAGENTA );
   init_pair(8,COLOR_YELLOW, COLOR_RED );   
      
   
   actualizarPantalla();
   leerTeclado();
    
}

Entorno :: ~Entorno ()
{
}


void Entorno :: actualizarPantalla( void )
{
    // ****************  DATOS ESCENARIO ********************* //
    /*
    
        vector < unsigned int > vidas;
        vector < Coordenada > jugadores;
        vector < Coordenada > paredesFijas;
        vector < Coordenada > paredesDestruibles;
        vector < Coordenada > premiosBomba;
        vector < Coordenada > premiosVida;
        vector < Coordenada > premiosExplosion;
        vector < Coordenada > premiosVelocidad;
        vector < vector < Coordenada >  > explosiones;
        
        unsigned int    tiempo;
     */          
     
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
    
    
    if ( pantalla != NULL ) 
                            delwin(pantalla); 
    
    pantalla = newwin(filas,columnas,starty, startx);
   
    box(pantalla,0 ,0);
   
    //  ----------------------------------------LEO EL ESCENARIO Y ACTUALIZO LA PANTALLA--------------------- //
    unsigned int posX, posY;
    
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

    
    // paredes fijas
    unsigned int x,y;
    vector < Coordenada > :: iterator it;
    
    wattron( pantalla,COLOR_PAIR(5) );
    for(it = escenarioActual->paredesFijas.begin() ; it != escenarioActual->paredesFijas.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;
    
        mvwprintw(pantalla, x  ,y, "XX");
        mvwprintw(pantalla, x+1,y, "XX");  
    }
    wattroff(pantalla,COLOR_PAIR(5));   
       
    // paredes destruibles
    
    wattron( pantalla,COLOR_PAIR(5) );
    for(it = escenarioActual->paredesDestruibles.begin() ; it != escenarioActual->paredesDestruibles.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "  ");
        mvwprintw(pantalla, x+1,y, "  ");  
        
    }
    wattroff(pantalla,COLOR_PAIR(5));
    

    //PREMIOS
    
    wattron( pantalla,COLOR_PAIR(7) );
    
    for(it = escenarioActual->premiosVida.begin() ; it != escenarioActual->premiosVida.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "VI");
        mvwprintw(pantalla, x+1,y, "DA");  
        
    }
    
    for(it = escenarioActual->premiosExplosion.begin() ; it != escenarioActual->premiosExplosion.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "++");
        mvwprintw(pantalla, x+1,y, "++");  
        
    }
    
    for(it = escenarioActual->premiosBomba.begin() ; it != escenarioActual->premiosBomba.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, "BO");
        mvwprintw(pantalla, x+1,y, "MB");  
        
    }
    
    for(it = escenarioActual->premiosVelocidad.begin() ; it != escenarioActual->premiosVelocidad.end() ; it++)
    {
        x = 2 * it->get_x() +1;
        y = 2 * it->get_y() +1;

        mvwprintw(pantalla, x  ,y, ">>");
        mvwprintw(pantalla, x+1,y, ">>");  
        
    }
    wattroff(pantalla,COLOR_PAIR(7));
    
    
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
    
    
    // Explosiones
    vector < vector <Coordenada> > :: iterator explosion;
    
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
 

    //actualizo pantalla   
    refresh();
    wrefresh( pantalla ); 
    //wgetch(pantalla);

    //  ---------------------------------------- FIN LEO EL ESCENARIO Y ACTUALIZO LA PANTALLA--------------------- //
   


}


void Entorno :: leerTeclado()
{
     //  ----------------------------------------ESTO DEBE ESTAR EN OTROS METODOS CORRESPONDIENTES A TECLADO --------------------- //
 int ch;
 unsigned int jugador = 0;
    
    while( (ch = getch()) != 'q' )
    {  
        switch(ch)
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
        
}

void Entorno :: finalizarPantalla( void )
{
    endwin();
}

#endif

/*
//////////////////////////////////////////////// Clase coordenada ////////////////////////////////////////////////

class Coordenada
{
    private:
        unsigned int x;
        unsigned int y;
        
    public:
        
        Coordenada( unsigned int x , unsigned int y);
        ~Coordenada();
        
        void set_coordenada( unsigned int x , unsigned int y );
        void set_x ( unsigned int x );
        void set_y ( unsigned int y );
        void get_coordenada( unsigned int &x , unsigned int &y );
        unsigned int  get_x();
        unsigned int  get_y();
};

*/
