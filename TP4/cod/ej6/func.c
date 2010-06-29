#include<stdio.h>
#include<string.h>
#include "split.h"

int main(){
    char x[1000];
    int i = 0;
    struct arg argumentos;
    
    fgets(x, 1000, stdin);
    x[strlen(x)] = '\0';
    
    split_args(x, &argumentos);
    printf("%d\nbkground = %d\ncrear = %d\nagregar = %d\narch = %s\n",argumentos.size, 
                                                                    argumentos.bkground,
                                                                    argumentos.crear,
                                                                    argumentos.agregar,
                                                                    argumentos.arch);
    
    for(i = 0; i < argumentos.size ; i++ )
        puts(argumentos.argum[i]);
    
    return 0;
}