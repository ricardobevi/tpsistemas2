#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


int main () 
{ 
    char  linea[300]= "";
    int i = 0;
    pid_t pidDelEjecutable;

    chdir( getenv("HOME") );
   
    printf("\n Bienvenido al mini interprete F&R \n \n" );

    printf("F&R %s >--> ", (char *)  get_current_dir_name()  );
    
    fgets(linea, 300, stdin);
    linea[strlen(linea) - 1] = '\0';

          while( strcmp(linea,"exit") )
          {
                  if( strcmp(linea,"") && strcmp(linea,"cd") ) 
                  {             
                          if ( ( pidDelEjecutable = fork() ) == -1 ) 
                          {
                                  perror("Error al crear el hijo");
                                  exit(1);
                          }

                          else if( pidDelEjecutable == 0 )
                          {                       
                                  if( execvp( linea, NULL ) < 0 ) {
                                        perror("   Error");
                                        exit(1);
                                  }
                          }
                          else
                          {
                                  wait(NULL);
                          }

                   }     
                   else if ( !strcmp(linea,"cd") )
                   {
                         chdir("/");
                   }
                   else    
                         printf("F&R %s >--> ", (char *)  get_current_dir_name()  );
              
              printf("F&R %s >--> ", (char *)  get_current_dir_name()  );
              fgets(linea, 300, stdin);
              linea[strlen(linea) - 1] = '\0';
          }     
    
    return 0;
}
