#!/bin/bash

if [ ! $# -eq 1 ]; then
    
    echo "ERROR parametros"
    exit 1
fi
        
if [ ! -d $1 ]; then
    
    echo "ERROR directorio"
    
    exit 1
fi

directorio=$1
dirName="`basename $1`"

if [ -f ./bkup.conf ]; then
    bkupDir="`grep "backupdir" bkup.conf | cut -d"=" -f2`"

    if [ ! -d $bkupDir ]; then
        echo "ERROR: bkup.conf: El parametro backupdir no existe o es incorrecto."
        exit 1
    fi
    
    if [ $bkupDir = "" ]; then
        echo "ERROR: bkup.conf: El parametro backupdir no existe o es incorrecto."
        exit 1
    fi

else
    echo "ERROR: bkup.conf: No existe archivo de configuracion."
    echo "Desea crearlo? (y/n)"; read -n 1 answer
    
    if [ $answer = "y" ]; then
        echo "Creando archivo bkup.conf y directorio de Back Ups por defecto..."
        echo "backupdir=./bkup" > bkup.conf
        mkdir bkup
        bkupDir="./bkup"
        echo "Listo! Creando Back Up..."
    else
        echo "Saliendo."
        
        exit 1
    fi
fi

# Formato bkup: nombredirectorio%fecha%.tar.gz

bkupName="$dirName%`date +%d-%m-%y-%H:%M`%.tar.gz"

if [ `ls $bkupDir | grep "$dirName" > /dev/null ; echo $?` = "0" ]; then
    
    ultimoBkUp="$bkupDir/`echo $(ls -t $bkupDir) | cut -d" " -f1`"
    
    archMod="`find $directorio -newer $ultimoBkUp -type f`"
    
    if [ ! "$archMod" = "" ]; then
        tar -caf "$bkupDir/$bkupName" $archMod
        
        echo "Creado Back Up $bkupName"
        echo "Archivos:"
        echo $archMod | sed 's/ /\n/g'
    else
        echo "No hay archivos modificados. No se crea Back Up."
    fi
    
else
    
    tar -caf "$bkupDir/$bkupName" $directorio
    echo "Creado Back Up $bkupName"
    echo -e "Archivos: \n" ; find $directorio
    
fi


exit 0








