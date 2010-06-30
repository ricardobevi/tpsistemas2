/********************************************************/
/* Nombre: ej5.c                                        */
/* Trabajo: Fork, Procesos Concurrentes, Zombies y Exec */
/* Numero de ejercicio: 5                               */
/* Entrega: Primer Entrega                              */
/*                                                      */
/* Grupo N 63                                           */
/* D`Aranno Facundo      34.842.320                     */
/* Jose Ferreyra         31.144.004                     */
/* Marcela A. Uslenghi   26.920.315                     */
/* Bevilacqua Ricardo    34.304.983                     */
/********************************************************/



#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int valida(const char*);

int main(int argc, char *argv[]) {

time_t segundosInicio;
int acumulador=0;
int cantidadHijos, tiempoEjecucion;
int i;

	if ( argc!=3)  /* Valido la cantidad de argumentos */
	{
		printf("Error en la entrada de argumentos.\n");
		printf("La forma de invocar al programa debe ser:\n");
		printf("./programa\t Cantidad de Hijos\t Tiempo en segundos\n");
		exit(EXIT_FAILURE);
	}

	if (valida(argv[1])==1) {
		cantidadHijos = atoi(argv[1]);  /*una vez validado el parametro, los convierto en entero*/
	}
	else {
		printf("La cantidad de hijos debe ser un numero entero");
		exit(EXIT_SUCCESS);
	}

	if (valida(argv[2])==1) {
		tiempoEjecucion = atoi(argv[2]);
	}
	else {
		printf("El tiempo de ejecucion debe ser un numero entero");
		exit(EXIT_SUCCESS);
	}
	
	segundosInicio = time(NULL); /*empiezo a contar el tiempo*/

	for ( i=1 ; i<=cantidadHijos ; i++ ) {
		switch ( fork() ) {
		case -1:
			perror("Error al crear un hijo");
			exit(EXIT_FAILURE);
		break;
		case 0:
			/* estoy en el hijo */

			while ( (time(NULL)-segundosInicio)<=tiempoEjecucion ) {
				acumulador++;
			}

			printf("Mi numero logico es %d (%d)\n", i, getpid());
			printf("Mi acumulador es %d\n", acumulador);

			exit(EXIT_SUCCESS);

		break;
		default:
			/* como padre no hago nada */
		break;
		}
	}

exit(EXIT_SUCCESS);
}


/* Funcion */
int valida(const char *cad) {  //funcion valida que la cadena ingresada no tenga caracteres 
size_t i, n=strlen(cad);
    for ( i=0 ; i<n ; i++ ) {
		if ( !isdigit(cad[i]) ) return 0; //tiene caracteres 
	}
return 1; //no tiene caracteres  
} 


/*******/
/* FIN */
/*******/
