#! /bin/bash
#
#ejercicio numero 2 TP Nª3
#
#Facus la mueve :p  ( jejeje )
#

if [ ! $1 ]
then  
	echo "ERROR: falta el parametro"	
	exit
fi

longmin=`cat pass-configure | cut -d"=" -f2`

#who | awk '{print $5, $1}' | sort
#awk -v pass=$1 -v longmin=$longmin -f pass-awk pass-constantes > password.out
awk -v pass=$1 -v longmin=$longmin -f pass-awk pass-constantes 

exit 1 

