#!/bin/bash

# Nombre: Punto3.sh
# Trabajo: Programacion de Scripts con AWK
# Numero de ejercicio: 3
# Entrega: Primer Entrega
#
# Grupo N° 63
# D`Aranno Facundo    34.842.320
# Moure Pablo         32.031.459
# Bevilacqua Ricardo  34.304.983

function ayuda(){
    echo  "`basename $0` Version 1.0"
    echo  "Cambia permisos a subdirectorios y archivos dentro de directorio."
    echo  "USO: `basename $0` [opciones...] permisos [directorio]"
    echo  "OPCIONES"
    echo  "-M           Cambia permisos si fue modificado en el mes actual."
    echo  "-X <tamano>  Cambia permisos si el archivo supera el tamano."
    echo  "-C <cadena>  Cambia permisos si el nombre contiene la cadena."
    echo  "-G <grupo>   Cambia permisos si el archivo pertenece al grupo."
    echo  "-?           Muestra la ayuda."
}

if [ $# -ge 5 ] ; then
    echo  "ERROR: Parametros incorrectos."
    ayuda
fi

archTempPermisos="/tmp/.perm"
archTempErr="/tmp/.err"

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
        archivos=`find $directorio* -maxdepth 1 -mtime -$( date +%d )`
        
        cond="Creado o modif. en el mes actual."
        
        ;;
        
    -X) tam=$2
        if test `echo $tam | grep "[^0-9]" > /dev/null; echo $?`
        then 
            archivos=`find $directorio* -maxdepth 1 -size +$( echo $tam )k`
        fi
        
        cond="Tamano mayor que $tam KB"
        
        ;;
        
    -C) cad=$2
        archivos=`ls $directorio* | grep $cad`
        
        cond="Cadena \"$cad\""
        
        ;;
             
    -G) grp=$2
        archivos=`find $directorio* -maxdepth 1 -group $grp`
        
        cond="Pertenece al grupo \"$grp\""
        
        ;;
        
        
     -?) ayuda  
         exit 0
        ;;
        
     *) archivos=`ls $directorio`
        ;;
esac

for archivo in $archivos
do
    archBase=`basename $archivo`
    
    permAnt=`ls -l $directorio | grep $archBase | cut -d" " -f1 | sed "s/^.//g"`
    
    #Aca cambia los permisos.
    chmod $permisos $archivo 2> /dev/null
    
    
    if [ $? -eq 0 ] ; then
        
        permActual=`ls -l $directorio | grep $archBase | cut -d" " -f1 | sed "s/^.//g"` 
        
        if [ $permAnt != $permActual ]; then
            echo "$archBase $permAnt $permActual" >> $archTempPermisos
        fi
        
    else
        echo $archBase >> $archTempErr
    fi
    
done

if [ -e $archTempPermisos ]; then
    awk -v "cond=$cond" -v "fecha=`date +"%d/%m/%Y %H:%M"`" -v "permisos=$permisos" -f p1Formato.awk $archTempPermisos
else
    echo "No hubo modificaciones."
fi

if [ -e $archTempErr ]; then
    awk 'BEGIN{ print "Archivos no cambiados:" }{ print $1 }' $archTempErr
else
    echo "No hubo errores."
fi

rm -f $archTempPermisos $archTempErr
exit 0

#
# FIN SCRIPT
#