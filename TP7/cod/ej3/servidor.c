/************************************/
/* Nombre: servidor.c               */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 3           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



/* Ejercicio 3: Realizar un programa servidor que atienda hasta 5 clientes concurrentes, pero que no rechace nunca una conexión,
que pueden ser ejecutados en múltiples máquinas. El servidor le asignará a cada uno de los clientes un número único de cliente y
le informará la hora actual del servidor a través del canal de comunicaciones que tienen en común. El cliente deberá comparar la
hora recibida con la hora local e informarle al servidor la diferencia de tiempo existente entre ambos relojes. El servidor,
al momento de recibir una señal para finalizar, deberá mostrar el resumen de los números de clientes, la IP y la diferencia en
los relojes que se calculó en el cliente. Nota: el servidor deberá manejar las comunicaciones a través de procesos livianos */

#include <sys/types.h> /*socket(), bind()*/
#include <sys/socket.h> /*socket(), bind(), inet_addr()*/
#include <netinet/in.h> /*inet_addr()*/
#include <arpa/inet.h>  /*inet_addr()*/
#include <string.h> /*bzero()*/
#include <stdio.h>
#include <unistd.h>
#include <time.h> /*la utilizo para obtener la hora local y para la diferencia de segundos*/
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

#define TAMBUF 1024
#define MAXQ 10  /* por las dudas, escucha hasta 10 conexiones*/
#define PORT 53220
#define FORMAT_DATETIME 21

typedef struct
{
  int       identificador;
  int       cliente;
  int       comm_socket;
} thread_parm_t;

void controlador (int);
void *funcionHilo (void *parametro);


int main()
{
    int                 listen_socket = 0, comm_socket = 0;
    unsigned short int  listen_port = 0;
    unsigned long int   listen_ip_address = 0;
    struct sockaddr_in  listen_address, con_address;
    socklen_t           con_addr_len;
    char                buffer[TAMBUF];
    time_t              current_time;          /* Esta variable contiene el system current date and time */
    struct tm           * date_time;
    char                datetime[FORMAT_DATETIME];
	pthread_t			idHilo;
    thread_parm_t       *parm=NULL;

    /* Alloca memoria para la estructura que se pasara como parametro a la funcion hilo */
    parm = malloc(sizeof(thread_parm_t));

    /* Creación del socket */
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Asignandole una dirección a éste */
    bzero(&listen_address, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    listen_port = htons(PORT); /* Solo un ejemplo, se puede elegir cualquier otro puerto */
    listen_address.sin_port = listen_port;
    listen_ip_address = htonl(INADDR_ANY);
    listen_address.sin_addr.s_addr = listen_ip_address;
    bind(listen_socket, (struct sockaddr *)&listen_address, sizeof(struct sockaddr));

    listen(listen_socket, MAXQ);
    printf("\n");
    printf("Escuchando Conexiones...\n");
    printf("Para Finalizar y Obtener el Informe Presione las Teclas Ctrl+C\n");


    if( signal( SIGINT, controlador ) == SIG_ERR )
	{
		perror( "No se puede cambiar signal" );
	}


    parm->identificador = 0;
    parm->cliente = 0;

    while( 1 )
    {
        if( parm->cliente < 5 )
        {
            bzero(&con_address, sizeof(con_address));
            comm_socket = accept(listen_socket, (struct sockaddr *)(&con_address), &con_addr_len);
            printf("Conexión recibida\n");
            bzero(buffer, TAMBUF);

            parm->comm_socket=comm_socket;
            parm->identificador++;
            parm->cliente++;

    		if( idHilo = pthread_create( &idHilo, NULL, funcionHilo, (void *)parm ) != 0 )
            {
    			printf("Error al crear el Hilo");
    			exit(EXIT_FAILURE);
    		}
        }
    }

    close(listen_socket);
    printf("Se desconectó\n");
    free( parm );
    return 0;
}


/* Funcion que se ejecuta en el thread hijo.*/
void *funcionHilo( void *parm )
{
    char                buffer[TAMBUF];
    time_t              current_time;          /* Esta variable contiene el system current date and time */
    struct tm           * date_time;
    char                datetime[FORMAT_DATETIME];
    thread_parm_t       *p = (thread_parm_t *)parm;

    /* Recibe del cliente el mensaje pasado como parametro */
    recv( p->comm_socket, buffer, TAMBUF, 0 );
    printf("\tMensaje del cliente %d: %s\n", p->identificador, buffer );

    /* Recibe del cliente la ip */
    recv( p->comm_socket, buffer, TAMBUF, 0 );
    printf("\tDireccion ip del cliente %d: %s\n", p->identificador, buffer );

    /* toma el system time */
    current_time = time(&current_time);

    /* convierte system time a local time */
    date_time = localtime(&current_time);

    /* convierte time a un string */
    sprintf(datetime,"%04d-%02d-%02d %02d:%02d:%02d",date_time->tm_year+1900
                                                    ,date_time->tm_mon+1,date_time->tm_mday,date_time->tm_hour
                                                    ,date_time->tm_min,date_time->tm_sec);

    /* envia al cliente fecha y hora */
    send(p->comm_socket, datetime, TAMBUF, 0);

    /* envia al cliente el numero de cliente */
    sprintf(buffer,"%d", p->identificador );
    send( p->comm_socket, buffer, TAMBUF, 0 );

    /* Recibo del cliente la diferencia horaria */
    recv( p->comm_socket, buffer, TAMBUF, 0 );
    printf( "\tDiferencia horaria con el cliente %d: %s\n", p->cliente, buffer );

    close(p->comm_socket);

    p->cliente--;
    return NULL;
}

void controlador( int numeroSenal )
{
	printf("\n=======================================================");
	printf("\n.................Informe Presentado...................\n");
	printf("=======================================================\n");
	/* Se pone controlador por defecto para ctrl-c */
	signal( SIGINT, SIG_DFL );

}




/*******/
/* FIN */
/*******/