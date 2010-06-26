#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    
    pid_t hijo;

    if( ( hijo = fork() ) < 0 ) {
        perror("Error!");
        exit(EXIT_FAILURE);
    }
    if( hijo == 0 ) {
        execv("./mipid", NULL);
        perror("Error!");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    printf("Fin!\n");

}


