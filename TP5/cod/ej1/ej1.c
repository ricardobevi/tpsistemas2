/************************************/
/* Nombre: ej1.c                    */
/* Trabajo: Senales, Pipes y FIFOs  */
/* Numero de ejercicio: 1           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D`Aranno Facundo      34.842.320 */
/* Jose Ferreyra         31.144.004 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>

void captura(int sig){
	printf("No es posible interrumpir el programa con Ctrl-C");
};

int main() {
        	
	signal(SIGINT, captura);

	int x=1;

	while (x==1){
		printf("\nMENSAJE\n");
		sleep(5);
	}
}



/*******/
/* FIN */
/*******/
