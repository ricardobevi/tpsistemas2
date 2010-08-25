/***********************************************/
/* Nombre: ej4.c                               */
/* Trabajo: TP6 Semaforos y Memoria Compartida */
/* Numero de ejercicio: 4                      */
/* Entrega: Primer Entrega                     */
/*                                             */
/* Grupo N 63                                  */
/* D'Aranno Facundo      34.842.320            */
/* Jose Ferreyra         31.144.004            */
/* Marcela A. Uslenghi   26.920.315            */
/* Bevilacqua Ricardo    34.304.983            */
/***********************************************/



﻿#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#define BUFFER_SIZE 100  /* defino el tamano de la linea */

#include "semaforo.h"

int main(int argc, char *argv[])
{

    key_t clavePadre, claveHijo, claveMemoria, claveMemoriaControl;
    int IdMemoria, IdProceso, IdPadre, IdHijo, i, j, IdMemoriaControl;
    char* pBuffer; /* esta variable la utilizamos para copiar las lineas del archivo */
    char* pBufferControl; /* esta variable la utilizamos para el corte de fin archivo*/

    FILE *fArchivo;

    if( argc!=2)  /* Valido la cantidad de argumentos */
    {
        printf("ERROR en la entrada de argumentos.\n");
        printf("La forma de invocar al programa debe ser:\n");
        printf("./programa\t <nombre_de_archivo.txt>");
		printf("ATENCION: El archivo debe estar en el directorio actual\n");
        exit(EXIT_FAILURE);
    }

    claveMemoria = ftok("/", 5000);
    claveMemoriaControl = ftok("/", 5001);

    IdMemoria = shmget(claveMemoria, BUFFER_SIZE, IPC_CREAT | 0600); /* creo el segmento de memoria */
    IdMemoriaControl = shmget(claveMemoriaControl, BUFFER_SIZE, IPC_CREAT | 0600); /* creamos el segmento de memoria para el corte*/

    clavePadre = ftok("/", 6001);
    claveHijo = ftok("/", 6002);

    IdPadre = obtenerSemaforo( clavePadre, 1 );
    IdHijo = obtenerSemaforo( claveHijo, 1 );

    printf("Iniciando...\n");

    switch ( fork() )
    {
        case -1:
            perror("Error al crear un hijo");
            exit(EXIT_FAILURE);
            break;

        case 0:	/* estoy en el hijo */

            IdMemoria = shmget(claveMemoria, BUFFER_SIZE, IPC_CREAT | 0600); /* Segmento de solo lectura*/
            if ((pBuffer = shmat(IdMemoria, NULL, 0)) == (char *) -1)
            {
			    perror("shmat");
                exit(1);
            }

            IdMemoriaControl = shmget(claveMemoriaControl, BUFFER_SIZE, IPC_CREAT | 0600); /* Segmento de solo lectura*/
            if ((pBufferControl = shmat(IdMemoriaControl, NULL, 0)) == (char *) -1)
            {
			    perror("shmat");
                exit(1);
            }

		    if( strlen( pBuffer ) == 0 )
            {
                exit( EXIT_SUCCESS );
            }
		    if( strlen( pBufferControl ) == 0 )
            {
                exit( EXIT_SUCCESS );
            }


            while( 1 )
            {
 			    pedirSemaforo(IdHijo);

                if( strcmp(pBufferControl,"end") == 0 ) /*pregunto si el padre termino de leer el archivo*/
                {
                    sleep(1);

                    devolverSemaforo(IdPadre);

                    break;
                }

                for( i=strlen(pBuffer); i>=0; i-- ) /*si no termino, imprimo las lineas*/
                {
                    printf( "%c", pBuffer[i] );
                }

                sleep(1);

                devolverSemaforo(IdPadre);
            }

            break;

        default: /* estoy en el padre */

            if((fArchivo = fopen(argv[1], "r"))==NULL)
            {
                printf("ERROR: No se pudo abrir <%s>\n", argv[1]);
				printf("Verifique el nombre del mismo\n");
				printf("ATENCION: El archivo debe estar en el directorio actual\n");
                exit(-1);
            }

            if ((pBuffer = shmat(IdMemoria, NULL, 0)) == (char *) -1)
            {
                perror("shmat");
                exit(1);
            }

            if ((pBufferControl = shmat(IdMemoriaControl, NULL, 0)) == (char *) -1)
            {
                perror("shmat");
                exit(1);
            }

            while( !feof( fArchivo ) )
            {

			    pedirSemaforo( IdPadre );  /* depositar la linea en la memoria */

                fgets( pBuffer, BUFFER_SIZE, fArchivo );

                strcpy( pBufferControl, "read" ); /*esta variable la utilizo para el corte, en este caso, sigo leyendo*/

                sleep(1);

                devolverSemaforo( IdHijo );

            }

    	    pedirSemaforo( IdPadre );

            strcpy(pBufferControl,"end"); /* cuanto termina de leer el archivo, pongo en el buffer de control el corte para que lo lea el hijo*/

            sleep(1);

            devolverSemaforo( IdHijo );

            break;
        }  /* cierro el switch */

        shmctl(IdMemoria, IPC_RMID, 0);  /* elimino los segmentos de memoria */
		shmctl(IdMemoriaControl, IPC_RMID, 0);

        eliminarSemaforo(IdPadre);    /* elimino los semaforos */
        eliminarSemaforo(IdHijo);

        fclose(fArchivo);

   exit(EXIT_SUCCESS);
}





/*******/
/* FIN */
/*******/