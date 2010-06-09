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
# COMIENZO DEL SCRIPT:


# corroboro que se envie un solo parametro 
if [ ! $1 ]
then  
        echo -e "\nERROR:\n\t este script utiliza un solo parametro\n"
        echo -e "USO:\t `basename $0 ` password \n"        
        exit 0
fi


# Asigno valor de entrada a la variable pass para trabajarla desde alli,
# y la longitud dada en archivo de configuracion para la longitud minima del pass a longmin

pass=$1  
longmin=`cat pass-configure | cut -d"=" -f2`

# Corroboro que la cantidad de caracteres de pass sea mayor de la longitud minima establecida 
# si el awk devuelve un 1, indica caso contrario ( menor a la longitud minima )
# entonces Informo y cierro script

  if test `echo $pass $longmin | awk  '{ if ( length($1) < $2) print 1  }'` 
    then
        echo -e "\nERROR:\n\t El pass es menor al tamaño minimo establecido para una contrasenia\n"
        echo -e "\t Tamaño minimo:\t $longmin \n"   
        exit 0
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
         # Para separar la expresion en sectores y poder realizar el punto c:
         #              $ sed -e 's/\(.*\) \(.*\) \(.*\)/Victor \1-\2 Von \3/' miarchivo.txt

         pass=`echo $pass | sed -e 's/'"$consonante"'/'"$valor"'/g'`
         pass=`echo $pass | sed -e 's/\([0123456789]\)\('"$valor"'\)/\1'"$consonante"'/g'`
        

done

#guardo pass en archivo ,lo informo y termino script
echo -e "\nEl password codificado es:  "$pass
echo $pass > password-out

exit 0;

                                            ###############
                                            #     END     #
                                            ###############