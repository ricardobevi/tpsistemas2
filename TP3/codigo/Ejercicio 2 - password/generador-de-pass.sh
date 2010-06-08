#! /bin/bash

# Nombre: generador-de-pass.sh
# Trabajo: Programacion en AWK
# Numero de ejercicio: 2
# Entrega: Primer Entrega
#
# Bevilaqcua Ricardo
# D`Aranno Facundo     34.842.320
# Marcela ....
# Moure Pablo          32.031.459
#
# comienzo del script:


# corroboro que se envie un solo parametro 
if [ ! $1 ]
then  
	echo "ERROR: este script utiliza un solo parametro \n USO: \t  `basename $0 ` password"	
	exit
fi



pass=$1  #asigno valor de entrada a la variable pass para trabajarla desde alli



#corroboro que el pass sea mayor de la longitud minima establecida en el archivo pasado por parametro
# si el awk devuelve un 1, indica caso contrario ( menor a la longitud minima ), entonces cierro script

longmin=`cat pass-configure | cut -d"=" -f2`
awk -v pass=$pass -v longmin=$longmin -f pass-awk pass-constantes
  
  if test $? -eq 1 
    then
    exit 1
  fi  


#paso la primer letra a minuscula en caso de que sea una mayuscula
pass=`echo $pass | sed -r 's/(^[A-Z])/\L&/g'`



#realizo el reemplazo de variables
for reemplazo in `cat pass-constantes `
do
         consonante=`echo $reemplazo| cut -d"=" -f1`
         valor=`echo $reemplazo| cut -d"=" -f2`
      
         # funcionamiento sin considerar regla de dos consonantes ( punto c )
         #                       pass=`echo $pass | sed -e 's/'"$consonante"'/'"$valor"'/g'`
         #
         # Para separar la expresion en sectores:
         #              $ sed -e 's/\(.*\) \(.*\) \(.*\)/Victor \1-\2 Von \3/' miarchivo.txt

         pass=`echo $pass | sed -e 's/'"$consonante"'/'"$valor"'/g'`
         pass=`echo $pass | sed -e 's/\([0123456789]\)\('"$valor"'\)/\1'"$consonante"'/g'`
         echo $pass > password-out

done

exit 1;

                                            ###############
                                            #     END     #
                                            ###############