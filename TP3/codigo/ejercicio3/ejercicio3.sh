#!/bin/bash

# Nombre: ejercicio3.sh
# Trabajo: Programacion de Scripts con AWK
# Numero de ejercicio: 3
# Entrega: Primer Entrega
#
# Grupo N° 63
# D`Aranno Facundo     34.842.320
# Moure Pablo          32.031.459
# Marcela A. Uslenghi  26.920.315
# Bevilacqua Ricardo   34.304.983

FILE_TMP=info.tmp

#------------------------------------------------------------------------
#1) Cantidad de procesos por usuario indicando en rojo el que mas tiene
#------------------------------------------------------------------------
        for i in `ps h -ely | awk '{ print $2 }' | sort | uniq -c | sort -k1 -g -r | awk '{ print $2 }'`; 
        do
                USUARIO=`grep -e "x:${i}:" /etc/passwd | cut -d: -f1`
                PROCUSER=`ps h -u${USUARIO} | wc -l`
                echo "$USUARIO;$PROCUSER" >> $FILE_TMP
        done
        awk 'BEGIN { FS=";" 
                        print "\033[0m"
                        print "1-Cantidad de procesos por usuario"
                        print "Usuario                      Cantidad"
                        print "====================================="} 
                    { 
                        if ( NR == 1 )
                                printf "%s%-29s%8s\n", "\033[1;31m", $1, $2 
                        else
                                printf "%s%-29s%8s\n", "\033[0m", $1, $2 
                        }' $FILE_TMP
        rm $FILE_TMP 

#------------------------------------------------------------------------
#2) Proceso mas nuevo y mas viejo del usuario que mas procesos tiene
#------------------------------------------------------------------------
        USUARIOID=`ps h -ely | awk '{ print $2 }' | sort | uniq -c | sort -k1 -g -r | awk '{ print $2 }' | head -n1`
        echo "Mas viejo; $(ps h -efaU${USUARIOID} O T | head -n1 | awk '{ print $5 }')" >> $FILE_TMP
        echo "Mas nuevo; $(ps h -efaU${USUARIOID} O T | tail -n1 | awk '{ print $5 }')" >> $FILE_TMP

        awk 'BEGIN { FS=";" 
                        print ""
                        print "2-Proceso mas nuevo y mas viejo del usuario que mas procesos tiene"
                        print "Proceso"
                        print "===================================="} 
                    { 
                        printf "%-30s%5s\n", $1, $2 
                        }' $FILE_TMP
        rm $FILE_TMP 

#------------------------------------------------------------------------
#3) Proceso con mas ejecuciones concurrentes
#------------------------------------------------------------------------
        ps auxww | awk '{ print $11 }' | sort | uniq -c | sort -k1 -g | tail -n1 | awk '{print $2"\t" $1}' |
        awk 'BEGIN { FS=" " 
                        print ""
                        print "3-Proceso con mas Ejecuciones Concurrentes"
                        print "Proceso                            Cantidad"
                        print "==========================================="} 
                    { 
                        printf "%-34s%9s\n", $1, $2
                        }'

#------------------------------------------------------------------------
#4) Proceso con mas carga de trabajo
#------------------------------------------------------------------------
        ps h auxfww O C | sort -k3 | tail -n1 | awk '{print $11 "\t" $2}' | 
        awk 'BEGIN { FS=" " 
                        print ""
                        print "4-Proceso con mas carga de trabajo"
                        print "Proceso                             PID"
                        print "==========================================="} 
                    { 
                        printf "%-35s%8s\n", $1, $2
                        }' 
#------------------------------------------------------------------------
#5) Proceso con mas memoria asignada
#------------------------------------------------------------------------
        ps -e -orss=,args= | sort -b -k1,1n | tac | awk 'NR==1'| awk '{print $2"\t"$1}' | 
        awk 'BEGIN { FS=" " 
                        print ""
                        print "5-Proceso con mas memoria asignada"
                        print "Proceso                              Memoria (en Kb)"
                        print "===================================================="} 
                    { 
                        printf "%-37s%15s\n", $1, $2
                        }'

#------------------------------------------------------------------------
#6) Carga de trabajo total del equipo identificando en:
#          verde si es menor al 40%, 
#          amarillo si esta entre el 40% y el 80%,
#          rojo si es superior al 80%
#------------------------------------------------------------------------
for i in `vmstat 1 11 | awk '{ print $15 }' | tail` ; do SUM=$[ $SUM + $i ] ; done
CARGA_TOTAL=`echo $(( 100 - ( $SUM / 10 ) ))`

        echo $CARGA_TOTAL | awk 'BEGIN { 
                print ""
                print "6-Carga de trabajo total del equipo"
                print "==========================================="} 
                { 
                        if( $1 < 40 )   
                                printf "%s%s%s\n", "\033[1;32m", $1, "%"
                        else if( $1 > 40 && $1 < 80 )
                                printf "%s%s%s\n", "\033[1;33m", $1, "%"
                        else
                                printf "%s%s%s\n", "\033[1;31m", $1, "%"
                        print "\033[0m"
                        }'