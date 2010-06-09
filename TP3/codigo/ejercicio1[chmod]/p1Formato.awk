#!/bin/awk -e

# Nombre: p1Formato.awk
# Trabajo: Programacion de Scripts con AWK
# Numero de ejercicio: 1
# Entrega: Primer Entrega
#
# Grupo N° 63
# D`Aranno Facundo     34.842.320
# Moure Pablo          32.031.459
# Marcela A. Uslenghi  26.920.315
# Bevilacqua Ricardo   34.304.983

BEGIN{
    printf "Cambio de permisos segun patron\n"
    printf "-------------------------------\n"
    printf "Ejecución del %s\n", fecha
    printf "Permisos a asignar: %s\n", permisos
    printf "Condición de búsqueda: %s\n", cond
    printf "Archivos cambiados:\n"
    printf "-------------------\n"
    printf "|Archivo                |Permisos anteriores|Permisos nuevos|\n"
    printf "-------------------------------------------------------------\n"
}

{
    printf "|%-23s|%19s|%15s|\n", $1, $2, $3
}

#
# FIN
#