#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

/*

176 cuadro punteado fino
177 cuadro punteado medio
178 cuadro punteado grueso
219 cuadro negro

*/

int main()
{   
   
   initscr();   
   cbreak();
   noecho();
   curs_set(0);
   keypad(stdscr, TRUE);
    
    start_color();        
    
    printw("Objetos que apareceran en el juego");
    mvprintw(3,0,"jugadores:");
    
    // jugador1 : ROJO
    init_pair(1, COLOR_BLACK, COLOR_RED  );
    attron(COLOR_PAIR(1));
    mvprintw(5,10,"^^");
    mvprintw(6,10,"\\/");
    attroff(COLOR_PAIR(1));

    // jugador2 : Azul
    init_pair(2, COLOR_BLACK, COLOR_BLUE  );
    attron(COLOR_PAIR(2));
    mvprintw(5,15,"^^");
    mvprintw(6,15,"\\/");
    attroff(COLOR_PAIR(2));
    
    // jugador3 : Amarillo
    init_pair(3, COLOR_BLACK, COLOR_YELLOW );
    attron(COLOR_PAIR(3));
    mvprintw(5,20,"^^");
    mvprintw(6,20,"\\/");
    attroff(COLOR_PAIR(3));

    // jugador4 : Verde 
    init_pair(4, COLOR_BLACK, COLOR_GREEN );
    attron(COLOR_PAIR(4));
    mvprintw(5,25,"^^");
    mvprintw(6,25,"\\/");
    attroff(COLOR_PAIR(4));
    
    mvprintw(9,0,"Paredes:");
    // pared fija
    init_pair(5, COLOR_BLACK, COLOR_WHITE );
    attron(COLOR_PAIR(5));
    mvprintw(11,10,"XX");
    mvprintw(12,10,"XX");
    attroff(COLOR_PAIR(5));
    
    // pared destructible
    init_color(COLOR_CYAN, 00, 00, 00); // esta linea no esta haciendo nada
    init_pair(6, COLOR_CYAN, COLOR_WHITE );
    attron(COLOR_PAIR(6));
    mvprintw(11,15,"  ");
    mvprintw(12,15,"  ");
    attroff(COLOR_PAIR(6));
 
    
    mvprintw(14,0,"Premios:");
    init_pair(7,  COLOR_WHITE , COLOR_MAGENTA );
    
    // premio velocidad
    attron(COLOR_PAIR(7));
    mvprintw(16,10,">>");
    mvprintw(17,10,">>");
    attroff(COLOR_PAIR(7));
    
    // premio vida
    attron(COLOR_PAIR(7));
    mvprintw(16,15,"VI");
    mvprintw(17,15,"DA");
    attroff(COLOR_PAIR(7));
    
    // premio bomba
    attron(COLOR_PAIR(7));
    mvprintw(16,20,"BO");
    mvprintw(17,20,"MB");
    attroff(COLOR_PAIR(7));
    
    
    //Bomba
    mvprintw(19,0,"Bomba:");
    
    init_pair( 8,   COLOR_YELLOW, COLOR_RED );    
    
    attron(COLOR_PAIR(8));
    mvprintw(21,10,"/\\");
    //mvchgat(21, 10, 2, A_BLINK, 1, NULL); 
    mvprintw(22,10,"\\/");
    //mvchgat(22, 10, 2, A_BLINK, 1, NULL);
    attroff(COLOR_PAIR(8));
    
 
    
    
    
    
    
    refresh();          
    getch();      
    endwin();     
    

    return 0;
}

