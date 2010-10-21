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

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main()
{   
    
   WINDOW *my_win;
   int startx, starty, width, height;
   
   
   height =24;
   width = 78;
   starty = 1;  
   startx = 1;   
   
   initscr();  
   cbreak();
   noecho();
   curs_set(0);
   keypad(stdscr, TRUE);
   start_color();
  
    mvprintw(0,36,"Time");
    
    // jugador1 : Rojo
    init_pair(1, COLOR_BLACK, COLOR_RED  );
    attron(COLOR_PAIR(1));
    mvprintw(0,1,"Jug 1: ");
    attroff(COLOR_PAIR(1));
    
    // jugador3 : Azul
    init_pair(2, COLOR_BLACK, COLOR_BLUE  );
    attron(COLOR_PAIR(2));
    mvprintw(0,16,"Jug 2: ");
    attroff(COLOR_PAIR(2));

    // jugador3 : Amarillo
    init_pair(3, COLOR_BLACK, COLOR_YELLOW );
    attron(COLOR_PAIR(3));
    mvprintw(0,53,"Jug 3: ");
    attroff(COLOR_PAIR(3));

    // jugador4 : Verde 
    init_pair(4, COLOR_BLACK, COLOR_GREEN );
    attron(COLOR_PAIR(4));
    mvprintw(0,68,"Jug 4: ");
    attroff(COLOR_PAIR(4));

    
    refresh();   
    my_win = create_newwin(height,width,starty, startx);
    
    int posJugX , posJugY;
    
     posJugX = 1;
     posJugY = 1;
     
     // jugador1 : ROJO
    init_pair(1, COLOR_BLACK, COLOR_RED  );
    wattron(my_win,COLOR_PAIR(1));
    mvwprintw(my_win,posJugX    , posJugY  ,"^^");
    mvwprintw(my_win,posJugX +1 , posJugY  ,"\\/");
    wattroff(my_win,COLOR_PAIR(1));
    
    wrefresh(my_win);  
    refresh();
    
    int ch;
    
    while( (ch = getch()) != 'q' )
    {  
        switch(ch)
        {   
            case KEY_LEFT:
                destroy_win(my_win);
                my_win = create_newwin(height, width, starty,startx);
                
                posJugY--;
                
                break;
                
            case KEY_RIGHT:
                destroy_win(my_win);
                my_win = create_newwin(height, width, starty,startx);
                
                posJugY++;
                
                break;
                
            case KEY_UP:
                destroy_win(my_win);
                my_win = create_newwin(height, width, starty,startx);
                  
                posJugX--;
                
                break;
                
            case KEY_DOWN:
                destroy_win(my_win);
                my_win = create_newwin(height, width, starty,startx);
                  
                posJugX++;
                
                break;  
        }
        
        init_pair(1, COLOR_BLACK, COLOR_RED  );
        wattron(my_win,COLOR_PAIR(1));
        mvwprintw(my_win,posJugX    , posJugY  ,"^^");
        mvwprintw(my_win,posJugX +1 , posJugY  ,"\\/");
        wattroff(my_win,COLOR_PAIR(1));
       
        /* carita mas linda pero no bancada por la terminal
        
        mvwprintw(my_win,posJugX    , posJugY  ,"^^");
        mvwprintw(my_win,posJugX +1 , posJugY  ,"└┘");
        
        */
        
        wrefresh(my_win);  

    }
        
    
    endwin();     
    

    return 0;
}


WINDOW *create_newwin(int height, int width, int starty, int startx)
{  
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    
    box(local_win,0 ,0);      
                 
    int posx,posy;
    posx = 3;
    posy = 1;
    
    init_pair(5, COLOR_BLACK, COLOR_WHITE );
    
        while ( posy < 76 )
        {
                while (posx <22)
                {
                    
                    // bloque fijo
                    wattron( local_win,COLOR_PAIR(5) );
                    mvwprintw(local_win,posx    , posy , "XX");
                    mvwprintw(local_win,posx +1 , posy , "XX");
                    wattroff( local_win,COLOR_PAIR(5) );

                    posx = posx +4;
            
                }
                
            posx = 3;    
            posy = posy +4;
        }
    
    wrefresh(local_win);      

    return local_win;
}


void destroy_win(WINDOW *local_win)
{   

    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');

    wrefresh(local_win);
    delwin(local_win);
}
