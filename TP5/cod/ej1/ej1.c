# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>

void captura(int sig){
	printf("No es posible interrumpir el programa con Ctrl-C");
};

int main() {
        	
	signal(SIGINT, captura);

	int x=1;

	while (x==1){
		printf("\nMENSAJE\n");
		sleep(5);
	}
}
