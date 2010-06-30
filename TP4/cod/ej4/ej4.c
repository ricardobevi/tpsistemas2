/********************************************************/
/* Nombre: ej4.c                                        */
/* Trabajo: Fork, Procesos Concurrentes, Zombies y Exec */
/* Numero de ejercicio: 4                               */
/* Entrega: Primer Entrega                              */
/*                                                      */
/* Grupo N 63                                           */
/* D`Aranno Facundo      34.842.320                     */
/* Jose Ferreyra         31.144.004                     */
/* Marcela A. Uslenghi   26.920.315                     */
/* Bevilacqua Ricardo    34.304.983                     */
/********************************************************/



#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main () 
{
  // vector de valores en formato de letras
  char *numeros[]={"uno","dos","tres","cuatro","cinco","seis","siete","ocho","nueve","diez",
                   "once","doce","trece","catorce","quince","dieciseis","diecisiete","dieciocho",
                   "diecinueve","veinte","veintiuno","veintidos","veintitres","veinticuatro",
                   "veinticinco","veintiseis","veintisiete","veintiocho","veintinueve","treinta"};

  pid_t pidHijoUno , pidHijoDos , pidHijoTres ,  pidHijoCuatro;
  int i;
  int tiempo = 1;

      if( ( pidHijoUno = fork() ) == 0)
      {
                if( ( pidHijoDos = fork() ) == 0) 
                {
                          if( ( pidHijoTres = fork() ) == 0) 
                          {
                                      if( ( pidHijoCuatro = fork() ) == 0) 
                                      {
                                            //aqui ejecuta el HIJO 4
                                            for(i = 0; i< 16 ;i++)
                                            {
                                                 printf("%02d) Hijo 4 [PID %d]: %s \n", i , getpid(), numeros[ (2 * i) ] );
                                                 sleep(tiempo);
                                            }                                    
                                      }
                                      else if ( pidHijoCuatro < 0 ) 
                                      {
                                            // informo error en caso de que
                                            // no se haya podido crear el hijo 
                                            // y cierro la aplicacion
                                            perror("Error al crear el hijo");
                                            exit(1);
                                      }
                                      else
                                      {
                                            //aqui ejecuta el HIJO 3
                                            for(i = 0; i< 15 ;i++)
                                            {
                                                 printf("%02d) Hijo 3 [PID %d]: %s \n",i , getpid(), numeros[ (2 * i + 1) ] );
                                                 sleep(tiempo);
                                            }                              
                                      }

                          }
                          else if ( pidHijoTres < 0 ) 
                          {
                                // informo error en caso de que
                                // no se haya podido crear el hijo 
                                // y cierro la aplicacion
                                perror("Error al crear el hijo");
                                exit(1);
                          }
                          else
                          {
                               //aqui ejecuta el HIJO 2
                               for(i = 0; i < 16 ;i++)
                               {
                                   printf("%02d) Hijo 2 [PID %d]: %d \n", i , getpid(), (2 * i) );
                                   sleep(tiempo);
                               }
                        
                          }
                }
                else if ( pidHijoDos < 0 ) 
                {
                      // informo error en caso de que
                      // no se haya podido crear el hijo 
                      // y cierro la aplicacion
                      perror("Error al crear el hijo");
                      exit(1);
                }
                else
                {
                      //aqui ejecuta el HIJO 1
                      for(i = 0; i< 15 ;i++)
                      {
                          printf("%02d) Hijo 1 [PID %d]: %d \n", i , getpid(), (2 * i + 1) );
                          sleep(tiempo);
                      }
                    
                }
      }
      else if ( pidHijoUno < 0 ) 
      {
            // informo error en caso de que
            // no se haya podido crear el hijo 
            // y cierro la aplicacion
            perror("Error al crear el hijo");
            exit(1);
      }
      else
      {
            //aqui ejecuta el padre
            waitpid( pidHijoUno ,NULL,0);
            waitpid( pidHijoDos ,NULL,0);
            waitpid( pidHijoTres ,NULL,0);
            waitpid( pidHijoCuatro ,NULL,0);
            printf("\n \n" );
      }
  


  
  return 0;
}


/*******/
/* FIN */
/*******/