#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#define LLAVE (key_t)234
#define MAX 10
int main()
{
        int shmid, i;//variable i para el for y el identificar en el sistema
        char *vector;//para asignar la memoria.

        if((shmid=shmget(LLAVE, MAX*sizeof(char), IPC_CREAT | 0600)) == -1) // shmget asigna un fragmento de MC al entero smhid.
        {
                printf("No se ha podido crear el segmento");
           
                exit (-1);//sale del programa (no se pudo crear un segmento de memoria)
        }
        else
        {
                if((int)(vector=shmat(shmid, 0, 0)) == -1)
                {
                  // El segmento de MC apunta a vector. Usando la funcion shmat se logra que todos puedan acceder a la memoria en común.       
                        printf("Error en la conexion"); // No se puede conectar. Sale del programa.
                        exit(-1);
                }

                for (i=0; i<MAX; i++) //Asigna los valores a el vector (MC)


                for (i=0; i<MAX; i++)
                        printf("Valor almacenado %c\n", vector[i]); 	//Muestro elementos del vector 
	 
		 sleep(10); //Espera 10 segundos

                if(shmdt(vector) == -1)//Libera MC,se usa la funcion shmdt con argumento Shmdt
                        
                {
	                        printf("Error en la desconexion"); 
                        exit(-1);
                }

                sleep(10);//se esperan 10 segundos

                if(shmctl(shmid, IPC_RMID, 0) == -1)//shmctl (borrado del segmento)
      

                {
                        printf("Error en el borrado del segmento."); // Si hay error.
                        exit(-1);
                }
        }
}
