#!/bin/bash

nomScript=`basename $0`

echo "[$nomScript] Compilando..."

make

echo "[$nomScript] Iniciando servidor Remiseria."
./remiseria

echo "[$nomScript] Iniciando clientes Autos y enviando a segundo plano."
./autos &

echo "[$nomScript] Enviando senales SIGUSR1 al servidor. 20 en total, cada 2 segundos."
for (( i=0 ; i<20 ; i=i+1 ))
do 
    killall -SIGUSR1 remiseria
    sleep 2
done

echo "[$nomScript] Esperando 5 seg. para que todos los autos (clientes) terminen sus viajes..."
for (( i=5 ; i>0 ; i=i-1 ))
do
    echo $i
    sleep 1
done

echo "[$nomScript] Matando proceso servidor y mostrando estadistica."
killall -SIGTERM remiseria

echo "[$nomScript] Matando proceso(s) cliente(s)."
killall -SIGTERM autos


exit 0