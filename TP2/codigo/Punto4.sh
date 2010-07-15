#! /bin/bash

# Nombre: Punto4.sh
# Trabajo: Programacion basica de scripts
# Numero de ejercicio: 4
# Entrega: Primer Entrega
#
# Grupo N° 63
# Acha Erica          33.515.479
# D`Aranno Facundo    34.842.320
# Moure Pablo         32.031.459
# Bevilacqua Ricardo  34.304.983

# | ($# = 2 & $1 = "-R" & ! -d $2)`

# la siguiente funcion muestra el uso del script en pantalla y termina el proceso
function error(){

          echo -e "ERROR: Parametros Incorrectos. 
                    \nUSO: `basename $0 ` [-R] directorio
                  \n     -R    Indica si el script debe ejecutarce en forma recursviva
                  \n           cambiando los nombres del los subdirectorios del directorio indicado\n"

          exit 1; 
}

#corroboro que los parametros enviados sean correctos, en caso correcto llamo a la funcion error
case $# in
      
      0)
            directorio="./"
            recursividad=0
         ;;

      1)  
            if [ ! -d $1 ]
            then
                 error                 

            else
                  # establesco los valores de las variables desde los argumentos 
                  # (caso de un solo parametro)
                  directorio=$1
                  recursividad=0

            fi
        ;;

      2)   
            if [  $1 != -R ]
            then
                 error

            else
                    
                    if [ ! -d $2 ]
                    then
                         error

                    else
                           # establesco los valores de las variables desde los argumentos
                           #  (caso de dos parametro)
                           directorio=$2
                           recursividad=1

                    fi
            fi  
        ;;
     
      *)
              #cualquier otro parametro indicara Error
              error

        ;;

esac


echo -e "\nEl directorio a renombrar es: " $directorio "y la recursividad esta en estado : " $recursividad "\n" 

#recorro el directorio enviado como parametro renombrando sus archivos en forma normalizada  

IFS=$'\n'
cd $directorio 

if test $recursividad -ne 1
then


            for lista in `find -maxdepth 1`
            do
                  archivo=$( basename $lista )

                  archivonuevo=`echo $archivo | sed -r 's/([A-Z])/\L&/g'| sed -r 's/(^.|[.;\)\(* _-,].)/\U&/g'`                               

                        if test $archivo !=  $archivonuevo
                        then
                                                mv $archivo $archivonuevo 2> /dev/null
                        fi

            done


else

            for lista in `find -depth`
            do
                  archivo=$( basename $lista )
                  ruta=$( dirname $lista )

                  archivonuevo=`echo $archivo | sed -r 's/([A-Z])/\L&/g'| sed -r 's/(^.|[.;\)\(* _-,].)/\U&/g'`                               

                        if test $archivo !=  $archivonuevo
                        then
                                                mv  `echo  $ruta/$archivo` `echo $ruta/$archivonuevo ` 2> /dev/null
                        fi

            done



fi
IFS=' '

exit 0