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
		printf("El tiempo de ejecuci�n debe ser un n�mero entero");
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

			printf("Mi n�mero l�gico es %d (%d)\n", i, getpid());
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

/* Ejercicio 5: Codifique un programa que genere N procesos hijos, que deber�n incrementar un acumulador durante un tiempo T, mostrar por pantalla el n�mero l�gico de hijo (no el PID) y el n�mero al que lleg� el acumulador y finalizar.
La cantidad de hijos N y el tiempo T se deben pasar por par�metro en la llamada al proceso. El tiempo T debe estar expresado en segundos. Responda:
a.�Todos los hijos llegan al mismo n�mero en el acumulador? Justifique el comportamiento.
b.Realice un gr�fico de barra mostrando para al menos tres corridas sucesivas, con los mismos par�metros, e indique hasta qu� n�mero lleg� cada hijo. Se debe mostrar un eje con el n�mero de hijo, y otro eje con el n�mero alcanzado, y una barra por cada corrida. Se recomienda realizar varios procesos de an�lisis con distintos par�metros para comprender lo que sucede.
Para la entrega, se debe dejar una imagen en formato JPG o PNG del gr�fico, y un archivo de tipo CSV con los datos con que se arm� el gr�fico.
c.Analizando el gr�fico, �es posible visualizar un patr�n de comportamiento? Explique.
d.Explique a que se deben las diferencias de comportamiento que tienen los procesos ejecutados en el ejercicio 4 y los ejecutados en �ste ejercicio.
Nota: no utilizar una funci�n de tiempos que cuente tics, se debe verificar el tiempo T en segundos del reloj y no utilizar vfork */
