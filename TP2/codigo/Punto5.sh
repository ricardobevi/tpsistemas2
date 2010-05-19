#!/bin/bash


if [ $# -ge 2 ]; then
    
    echo "ERROR parametros"
    exit 1
fi
        
if [ ! -d $1 ]; then
    
    echo "ERROR directorio"
    
    exit 1
fi
        
directorio=$1
#TODO: obtener solo en nombre del directorio sin "/" y sin ruta absoluta.

if [ -f ./bkup.conf ]; then
    bkupDir=`grep "backupdir" bkup.conf | cut -d"=" -f2`
    #TODO: chequear que se tome un parametro y que sea un directorio.
else
    echo "No existe archivo de configuracion."
    #TODO: Dar opcion de crearlo.
fi

# Formato bkup: nombredirectorio%fecha%.tar.gz
#TODO: Chequear que el directorio no tenga '%' en el nombre.

bkupName="$directorio%`date +%d-%m-%y-%H:%M`%.tar.gz"

if [ `ls $bkupDir | grep "$directorio" > /dev/null ; echo $?` = "0" ]; then
    
    ultimoBkUp="$bkupDir/`echo $(ls -t $bkupDir) | cut -d" " -f1`"
    
    archMod="`find $directorio -newer $ultimoBkUp -type f`"
    
    if [ ! "$archMod" = "" ]; then
        tar -caf "$bkupDir/$bkupName" $archMod
        
        echo "Creado Back Up $bkupName"
        echo "Archivos:"
        echo $archMod | sed 's/ /\n/g'
    else
        echo "No hay archivos modificados."
    fi
    
else
    
    tar -caf "$bkupDir/$bkupName" $directorio
    echo "Creado Back Up $bkupName"
    echo -e "Archivos: \n" ; find $directorio
    
fi


exit 0







