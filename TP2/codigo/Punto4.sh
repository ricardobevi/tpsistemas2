#!/bin/bash

if test -d $1
then
 
    for directorio in `ls $1`
    do    
        
        vectorMod=$directorio
        
        IFS="[;:.,_-@\ \"'\\/~+{}*]"
          
        i=0
    
        for archivo in $directorio
        do
            #agarro la pal y la hago mayus
           
           vectorMod[$i]=$archivo
           i=`expr $i + 1`
           
        done
        
        IFS=" "
        
        echo ${vectorMod[@]}
        
       
    done
    
else
    echo "El parametro no es un directorio."
fi

exit 0


