#!/bin/bash

# Nombre: Punto2.sh
# Trabajo: Programacion basica de scripts
# Numero de ejercicio: 2
# Entrega: Primer Entrega
#
# Grupo N° 63
# Acha Erica          33.515.479
# D`Aranno Facundo    34.842.320
# Moure Pablo         32.031.459
# Bevilacqua Ricardo  34.304.983

#Chequeo que se reciba solo un parametro.
#Caso contrario muestra el uso y sale.
if test $# -ne 1
then
       echo "Ejemplo 2"
       echo "Uso: `basename $0` directorio"
       exit 1
fi

#Comprueba que el parametro pasado sea
#un directorio.
if test ! -d $1
then
       echo "Ejemplo 2"
       echo "Uso: `basename $0` directorio"
       echo
       echo "ERROR: El parámetro no es un directorio."
fi

#Se comprueba si se tienen permisos de lectura sobre el directorio.
if test ! -r $1 
then
       echo "ERROR: Permiso denegado al directorio $1."
       exit 0
fi


#Lista los nombres y fechas de creacion de los 
#directorios dentro de la ruta recibida por parametro.
for linea in `find $1`
do
       if test -d $linea # PUNTO B
       then
       
          echo $linea: `date -r $linea`
    
       fi
done

exit 0
