#!/bin/bash

# Nombre: Punto3.sh
# Trabajo: Programacion basica de scripts
# Numero de ejercicio: 3
# Entrega: Primer Entrega
#
# Grupo N° 63
# D'Aranno, Facundo
# Bevilacqua, Ricardo
#
#

if [ $# -ge 5 ]
then
    echo -e "ERROR: Parametros incorrectos.
         \n`basename $0` Version 1.0
         \nCambia permisos a subdirectorios y archivos dentro de directorio.
         \nUSO: `basename $0` [opciones...] permisos [directorio]
         \nOPCIONES
         \n     -M           Cambia permisos si fue modificado en el mes actual.
         \n     -X <tamaño>  Cambia permisos si el archivo supera el tamaño.
         \n     -C <cadena>  Cambia permisos si el nombre contiene la cadena.
         \n     -G <grupo>   Cambia permisos si el archivo pertenece al grupo.
         \n     -?           Muestra la ayuda."

fi

directorio=`echo $* | cut -d" " -f$#`

if [ ! -d $directorio ]
then

    permisos=$directorio
    directorio="./"

else
    permisos=`echo $* | cut -d" " -f$(($# - 1 ))`

fi

case $1 in
    -M) 
        archivos=`find $directorio -mtime -$( date +%d )`
        ;;
        
    -X) tam=$2
        if test `echo $tam | grep "[^0-9]" > /dev/null; echo $?`
        then 
            archivos=`find $directorio -size +$( echo $tam )k`
        fi
        ;;
        
    -C) cad=$2
        archivos=`ls $directorio | grep $cad`
        ;;
        
    -G) grp=$2
        archivos=`find $directorio -group $grp`
        ;;
        
     -?) echo -e "`basename $0` Version 1.0
         \nCambia permisos a subdirectorios y archivos dentro de directorio.
         \nUSO: `basename $0` [opciones...] permisos [directorio]
         \nOPCIONES
         \n     -M           Cambia permisos si fue modificado en el mes actual.
         \n     -X <tamaño>  Cambia permisos si el archivo supera el tamaño.
         \n     -C <cadena>  Cambia permisos si el nombre contiene la cadena.
         \n     -G <grupo>   Cambia permisos si el archivo pertenece al grupo.
         \n     -?           Muestra la ayuda."
                
         exit 0
        ;;
        
     *) archivos=`ls $directorio`
        ;;
esac

for archivo in $archivos
do
    chmod $permisos $archivo > /dev/null
done

exit 0

#
# FIN SCRIPT
#