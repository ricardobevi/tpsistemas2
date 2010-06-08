#!/bin/bash

# Nombre: Punto1.sh
# Trabajo: Programacion basica de scripts
# Numero de ejercicio: 1
# Entrega: Primer Entrega
#
# Grupo N° 63
# Acha Erica          33.515.479
# D`Aranno Facundo    34.842.320
# Moure Pablo         32.031.459
# Bevilacqua Ricardo  34.304.983

#Chequea que no se le pasen parametros al script,
#en caso de tenerlos terminamos el mismo.
if test $# -ge 1
then
       echo "Ejemplo 1"
       echo "Uso: este script no necesita parametros."
       exit 1
fi

#Se toman los datos de los grupos del sistema y se lo guarda en
#el archivo /tmp/grupos
getent group > /tmp/grupos

#Se listan del archivo /tmp/grupos los usuarios 
#que pertenecen al grupo 'users'.
IFS=","
for dato in `cat /tmp/grupos | grep ^users | cut -d":" -f4`
do
       echo $dato
done

#Por ultimo eliminamos el archivo /tmp/grupos
rm -f /tmp/grupos

exit 0
