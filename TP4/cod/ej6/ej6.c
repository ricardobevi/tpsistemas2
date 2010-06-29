#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

int main () 
{ 
    char linea[300] = "";
    char dirAct[300] = "";
    char host[50];
    char user[50];
    int i = 0;
    pid_t pidDelEjecutable;
    int cantBkground = 0;
    struct arg argumentos;

    chdir( getenv("HOME") );
   
    printf("\n Bienvenido al mini interprete F&R \n \n" );
    
    gethostname(host, 50);
    strcpy(user, getenv("USER"));
    getcwd(dirAct, 300);
    
    printf("%s(%s):%s=> ",user ,host, dirAct );
    
    fgets(linea, 300, stdin);
    linea[strlen(linea) - 1] = '\0';

          while( strcmp(linea,"exit") )
          {
                  if( strcmp(linea,"")  && strcmp(linea,"echo") ) 
                  {         
                              split_args(linea, &argumentos);
                              printf("\n\n%d\n\n",argumentos.bkground);

                              if ( !strcmp( argumentos.argum[0] ,"cd") )
                              {
                                    chdir(argumentos.argum[1]);
                                    getcwd(dirAct, 300);
                              }
                              else if ( ( pidDelEjecutable = fork() ) == -1 ) 
                              {
                                      perror("Error al crear el hijo");
                                      exit(1);
                              }

                              else if( pidDelEjecutable == 0 )
                              {        
                                      if(argumentos.crear)
                                                  freopen(argumentos.arch, "w", stdout);
                                      else if (argumentos.agregar)
                                                  freopen(argumentos.arch, "a", stdout);

                                      if( execvp( argumentos.argum[0], argumentos.argum ) < 0 ) {
                                            perror("   Error");
                                            exit(1);
                                      }
                              }
                              else
                              {

                                      if ( ! argumentos.bkground )

                                                   waitpid(pidDelEjecutable);
                                      else
                                                    wait(NULL);

                                   
                              }

                   }     
                   else if ( !strcmp(linea,"echo") ) 
                   {
                        printf("%s",linea);
                   }
               
              printf("%s(%s):%s=> ",user ,host, dirAct);
              fgets(linea, 300, stdin);
              linea[strlen(linea) - 1] = '\0';
          }    
    
/* 
//  aparentemente con el waitpid no hay necesidad de este for   

    for(i=0; i< cantBkground ; i++)
    {
        wait();
    }
*/
    
    return 0;
}
