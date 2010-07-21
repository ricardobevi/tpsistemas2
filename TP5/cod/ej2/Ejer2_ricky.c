/************************************/
/* Nombre: Ejer2_ricky.c            */
/* Trabajo: Señales, Pipes y FIFOs */
/* Numero de ejercicio: 2           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D`Aranno Facundo      34.842.320 */
/* Jose Ferreyra         31.144.004 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define LEER            0
#define ESCRIBIR        1


int main () {

   float importe=0, importe_total=0, promedio=0;
   int ventasxdia=0;
   int canalVenta[2], respuestaVenta[2];   /* Descriptores para LEER y ESCRIBIR */

   pipe (canalVenta);
   pipe (respuestaVenta);

   if (fork () == 0){  /* codigo del hijo */

      close (canalVenta[ESCRIBIR]);
      close (respuestaVenta[LEER]);


      while(read(canalVenta[LEER],&importe, sizeof(importe))!=0) {

         ventasxdia ++;

         importe_total += importe;

         promedio = importe_total / ventasxdia;
	
      	 write(respuestaVenta[ESCRIBIR],&importe_total, sizeof(importe_total));


      } /* cierro el while*/

      close(canalVenta[LEER]);

      close(respuestaVenta[ESCRIBIR]);

      printf("Importe total:  %.2f\n",importe_total);

      printf("Promedio de ventas:  %.2f\n",promedio);

      printf("La cantidad de ventas de la sucursal es:  %d\n", ventasxdia);




   } /* fin del codigo del hijo */

   else {   /*codigo del padre */

      close (canalVenta[LEER]);
      close (respuestaVenta[ESCRIBIR]);


      printf("Ingrese el valor de venta <ingrese cero para terminar la carga>");  
	    
      scanf("%f",&importe);


      while (importe!=0){

         write(canalVenta[ESCRIBIR],&importe, sizeof(imp));

	 read(respuestaVenta[LEER],&importe_total, sizeof(importe_total));

         printf("Ingrese el valor de venta <ingrese cero para terminar la carga>");   
   
         scanf("%f",&importe);

      }


      close (canalVenta[ESCRIBIR]);

      close(respuestaVenta[LEER]);

   } /* fin codigo del padre */

   exit(EXIT_SUCCESS);

} /* cierra el main */








/*******/
/* FIN */
/*******/