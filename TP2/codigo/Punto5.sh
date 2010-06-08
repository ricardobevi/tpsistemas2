#!/bin/bash

# Nombre: Punto5.sh
# Trabajo: Programacion basica de scripts
# Numero de ejercicio: 5
# Entrega: Primer Entrega
#
# Grupo N° 63
# Acha Erica          33.515.479
# D`Aranno Facundo    34.842.320
# Moure Pablo         32.031.459
# Bevilacqua Ricardo  34.304.983

#Funcion ayuda imprime un texto con la ayuda.
function ayuda(){

    echo -e " `basename $0` 
            \n Crea Back Ups del directorio pasado por parametro.
            \n USO: `basename $0 ` directorio\n"
            
}

#Compruebo que haya solo un parametro.
if [ ! $# -eq 1 ]; then
    
    echo "ERROR: Parametros incorrectos."
    ayuda
    exit 1
fi

#Compruebo si el parametro es la ayuda.
if [ $1 = "-?" ]; then
    ayuda
    exit 0
fi

#Compruebo que el parametro sea un directorio.
if [ ! -d $1 ]; then
    
    echo "ERROR: No es un directorio."
    ayuda
    exit 1
fi

#Extraigo el directorio pasado por parametro y el nombre base sin
#la ruta absoluta o relativa del directorio.
directorio=$1
dirName="`basename $1`"

#Chequeo que exista el archivo bkup.conf en el directorio donde se ejecuta el script.
if [ -f ./bkup.conf ]; then
    #Intento tomar el parametro backupdir del archivo de configuracion.
    bkupDir="`grep "backupdir" bkup.conf | cut -d"=" -f2`"

    #Chequeo que el parametro no sea vacio.
    if [ $bkupDir = "" ]; then
        echo "ERROR: bkup.conf: El parametro backupdir no existe o es incorrecto."
        exit 1
    fi

    #Chequeo que el parametro tomado sea un directorio.
    if [ ! -d $bkupDir ]; then
        echo "ERROR: bkup.conf: El parametro backupdir no existe o es incorrecto."
        exit 1
    fi
    
else #Si no existe el archivo de configuracion, pregunto si desea crearlo.
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

# Formato de archivo de Back Up: nombredirectorio%fecha%.tar.gz
#Obtengo el nombre del archivo del Back Up.
bkupName="$dirName%`date +%d-%m-%y-%H:%M`%.tar.gz"

#Compruebo si hay Back Ups anteriores.
if [ `ls $bkupDir | grep "$dirName" > /dev/null ; echo $?` = "0" ]; then
    #Rescato el nombre del ultimo Back Up.
    ultimoBkUp="$bkupDir/`echo $(ls -t $bkupDir) | cut -d" " -f1`"
    #Busco los archivos modificados desde el ultimo Back Up.
    archMod="`find $directorio -newer $ultimoBkUp -type f`"
    
    #Chequeo si hay algun archivo modificado.
    if [ ! "$archMod" = "" ]; then
        #Creo el nuevo Back Up.
        tar -caf "$bkupDir/$bkupName" $archMod
        
        #Informo al usuario.
        echo "Creado Back Up $bkupName"
        echo "Archivos:"
        echo $archMod | sed 's/ /\n/g'
    else
        #Si no hay archivos modificados, no se hace Back Up.
        echo "No hay archivos modificados. No se crea Back Up."
    fi
    
else
    #Creo el Back Up de todo el directorio.
    tar -caf "$bkupDir/$bkupName" $directorio
    
    #Informo al usuario los archivos agregados al Back Up.
    echo "Creado Back Up $bkupName"
    echo -e "Archivos: \n" ; find $directorio
    
fi


exit 0


#
# FIN
#
