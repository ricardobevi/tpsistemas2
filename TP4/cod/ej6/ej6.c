#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

int main () 
{ 
    // declaracion de variables

    char linea[300] = "";   // linea tomada del input
    char dirAct[300] = "";  // cadena que almacena directorio actual
    char host[50];          // cadena que almacena el HOST 
    char user[50];          // cadena que almacena el USER actual
    int i = 0;              // auxiliar de iteracion
    int BkgroundChild = 0;  // cantidad de hijos en background
    pid_t pidDelEjecutable; // variable que almacena los pid 
    
    struct arg argumentos;  // estructura que almacena los parametros 
                            // enviados en el input 


    //************************ COMIENZO DE LA APLICACION************************//

    // genero el prompt y espero primer linea del interprete
    chdir( getenv("HOME") );
   
    printf("\n Bienvenido al mini interprete F&R \n \n" );
    
    gethostname(host, 50);
    strcpy(user, getenv("USER"));
    getcwd(dirAct, 300);
    
    printf("%s(%s):%s=> ",user ,host, dirAct );
    
    fgets(linea, 300, stdin);
    linea[strlen(linea) - 1] = '\0';

          // mientras no escriba exit continuo en el interprete

          while( strcmp(linea,"exit") )
          {
                  if( strcmp(linea,"")  && strcmp(linea,"echo") ) 
                  {         
                              // tomo los argumentos y los almaceno 
                              // en la estructura argumentos
                              split_args(linea, &argumentos); 
                             
                              if ( !strcmp( argumentos.argum[0] ,"cd") )
                              {
                                    // realizo cambio de directorio  
                                    chdir(argumentos.argum[1]);
                                    getcwd(dirAct, 300);
                              }
                              else if ( ( pidDelEjecutable = fork() ) == -1 ) 
                              {
                                      // informo error en caso de que
                                      // no se haya podido crear el hijo 
                                      // y cierro la aplicacion
                                      perror("Error al crear el hijo");
                                      exit(1);
                              }

                              else if( pidDelEjecutable == 0 )
                              {       
                                      // EJECUTA EL HIJO
                                      //  redirecciona la salida en caso de que se le
                                      //  haya enviado el parametro ">" o ">>"
                                      //  luego ejecuta el binario enviandole 
                                      //  los parametros adecuados 
                                      //  ( en caso de error lo informa y mata al hijo )
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
                                      // EJECUTA PADRE (El interprete)
                                      // espera al hijo para continuar
                                      // o continua ejecutando en 
                                      // segundo plano dependiendo del parametro "&"

                                       if(! argumentos.bkground )
                                       {
                                              wait(NULL);
                                       }
                                       else 
                                       {     
                                              BkgroundChild ++;
                                              printf("[%d]  %d\n",BkgroundChild ,pidDelEjecutable);
                                              waitpid(pidDelEjecutable);
                                       }

                                   
                              }

                   }     
                   else if ( !strcmp(linea,"echo") ) 
                   {
                        // ejecuta un echo 
                        // (imprime lo escrito en pantalla)
                        printf("%s",linea);
                   }
               
              printf("%s(%s):%s=> ",user ,host, dirAct);
              fgets(linea, 300, stdin);
              linea[strlen(linea) - 1] = '\0';
          }    
    
    
    return 0;
}
