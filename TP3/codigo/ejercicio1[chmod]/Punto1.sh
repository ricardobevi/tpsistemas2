#!/bin/bash

# Nombre: Punto1.sh
# Trabajo: Programacion de Scripts con AWK
# Numero de ejercicio: 3
# Entrega: Primer Entrega
#
# Grupo N° 63
# D`Aranno Facundo     34.842.320
# Moure Pablo          32.031.459
# Marcela A. Uslenghi  26.920.315
# Bevilacqua Ricardo   34.304.983

# Funcion ayuda()
# Muestra la ayuda del programa.
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

# Corrobora si la cantidad de parametros es correcta.
if [ $# -ge 5 ] ; then
    echo  "ERROR: Parametros incorrectos."
    ayuda
    exit 1
fi

# Constantes que identifican donde se guardan temporalmente
# los archivos de texto con los nombres de los archivos
# modificados y los que generaron error.
# En el archivo temporal de permisos tambien se guardan
# los permisos anteriores y siguientes de los archivos.
archTempPermisos="/tmp/.perm"
archTempErr="/tmp/.err"

# Tomo el ultimo parametro ingresado por linea de comandos.
# Asumiendo que el ultimo parametro sea el directorio.
# ej. $ Punto1.sh 700 /home/lala/docs
# En el ejemplo tomo la ruta ingresada al final.
# ($* imprime todos los parametros separados por el IFS)
directorio=`echo $* | cut -d" " -f$#`

# Puede ocurrir que el parametro pasado no sea un directorio
# en cuyo caso se asume que son los permisos y se reemplaza
# por el directorio actual. El ultimo parametro son los permisos.
# ej. $ Punto1.sh 700
# El ultimo parametro que tome en la sentencia anterior 
# seria '700'.
if [ ! -d $directorio ]
then

    permisos=$directorio
    directorio="./"
    
else
    # Si el ultimo parametro si es un directorio, entonces tomo
    # el anteultimo parametro, que estoy seguro que son los per-
    # misos.
    # ( '$(($# -1 ))' devuelve el numero de parametros menos 1 )
    permisos=`echo $* | cut -d" " -f$(($# - 1 ))`

fi

# Compruebo que los permisos sean un numero y no otra cosa.
# Con el 'grep' le indico con la exprecion regular "[^0-9]" que
# me liste todo lo que NO sea un numero. Si no muestra nada, o sea
# que es un numero lo que se ingreso, el grep devuelve 1, sino 0.
if test `echo $permisos | grep "[^0-9]" > /dev/null; echo $?` -eq 0; then
    echo "Error en los permisos."
    ayuda
    exit 1
fi 

# El primer argumento es la opcion, en el case evaluo este de
# manera de determinar que opcion se paso por parametro.
# Utilizo el comando 'find' para encontrar que archivos dentro
# del directorio cumplen con la condicion.
# El argumento '-maxdepth 1' en cada caso, es para indicarle
# al 'find' que no busque mas alla del directorio pasado por
# parametro.

case $1 in
    -M) # Uso la opcion -mtime para indicarle la condicion.
        # '-mtime n' busca archivos que hayan sido modificados
        # hace n*24 horas, con un valor negativo, toma 
        # los modificados desde hace n*24 horas hasta la fecha
        # actual. 'date +%d' devuelve el dia del mes actual.
        
        archivos=`find $directorio* -maxdepth 1 -mtime -$( date +%d )`
        
        # Guardo la condicion de busqueda para mostrar a la salida.
        cond="Creado o modif. en el mes actual."
        
        ;;
       
    -X) # Tomo el tamano minimo del archivo.
        tam=$2
        
        # Compruebo que sea un numero. Mismo procedimiento anterior.
        if test `echo $tam | grep "[^0-9]" > /dev/null; echo $?` -eq 1
        then
            # Busco los archivos cuyo tamano sea mayor que el parametro.
            archivos=`find $directorio* -maxdepth 1 -size +$( echo $tam )k`
        else
            echo "El tamano del archivo no es valido."
            ayuda
            exit 1
        fi
        
        cond="Tamano mayor que $tam KB"
        
        ;;
        
    -C) cad=$2
        # Listo solo los archivos que contengan la cadena.
        archivos=`ls $directorio* | grep $cad`
        
        cond="Cadena \"$cad\""
        
        ;;
             
    -G) grp=$2
        # Busco los archivos que pertenezcan al grupo usando la
        # opcion -group
        archivos=`find $directorio* -maxdepth 1 -group $grp`
        
        cond="Pertenece al grupo \"$grp\""
        
        ;;
        
        
     -?) ayuda  
         exit 0
        ;;
        
     *) # Si no se pasa parametro se cambia permisos a todo.
        archivos=`find $directorio* -maxdepth 1`
        cond="Ninguna"
        ;;
esac

# Paso por la lista de archivos cambiando los permisos uno por uno.
for archivo in $archivos
do
    # Tomo el nombre base del archivo sin la ruta para obtener los
    # permisos y mostrarlo luego.
    archBase=`basename $archivo`
    
    # Obtengo los permisos actuales del archivo. Primero listo todo el directorio,
    # saco la linea que me interesa (la del archivo actual), tomo la columna que
    # contiene los permisos y por ultimo le saco el primer caracter que es el
    # tipo de archivo.
    permAnt=`ls -l $directorio | grep $archBase | cut -d" " -f1 | sed "s/^.//g"`
    
    #Aca cambia los permisos. Redirijo la salida de errores.
    chmod $permisos $archivo 2> /dev/null
    
    # Chequeo si la salida de chmod fue exitosa o no.
    if [ $? -eq 0 ] ; then
        # Si fue exitosa tomo los permisos actuales.
        permActual=`ls -l $directorio | grep $archBase | cut -d" " -f1 | sed "s/^.//g"` 
        
        # Si los permisos son diferentes, guardo el nombre del archivo y los permisos
        # actuales y anteriores en el archivo temporal.
        if [ $permAnt != $permActual ]; then
            echo "$archBase $permAnt $permActual" >> $archTempPermisos
        fi
        
    else
        # Si la salida del chmod fue erronea, guardo el nombre del archivo en un
        # archivo temporal de errores.
        echo $archBase >> $archTempErr
    fi
    
done

# Salida Formateada

# Chequeo si algun archivo fue modificado y se creo el
# archivo temporal.
if [ -e $archTempPermisos ]; then
    # Llamo al AWK y le paso como variables la condicion, la fecha y los permisos a
    # asignar para formatear la salida.
    awk -v "cond=$cond" -v "fecha=`date +"%d/%m/%Y %H:%M"`" -v "permisos=$permisos" -f p1Formato.awk $archTempPermisos
    
else
    
    echo "No hubo modificaciones."
    
fi

if [ -e $archTempErr ]; then
    # Muestro el archivo de errores, si los hubo.
    awk 'BEGIN{ print "Archivos no cambiados:" }{ print $1 }' $archTempErr
    
else
    
    echo "No hubo errores."
    
fi

# Borro los archivos temporales.
rm -f $archTempPermisos $archTempErr

exit 0

#
# FIN SCRIPT
#