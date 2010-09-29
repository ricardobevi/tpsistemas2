/************************************/
/* Nombre: cliente.c                */
/* Trabajo: Threads y Sockets       */
/* Numero de ejercicio: 3           */
/* Entrega: Primer Entrega          */
/*                                  */
/* Grupo N 63                       */
/* D'Aranno Facundo      34.842.320 */
/* Marcela A. Uslenghi   26.920.315 */
/* Bevilacqua Ricardo    34.304.983 */
/************************************/



# include <sys/types.h> /* socket(), bind() */
# include <sys/socket.h> /* socket(), bind(), inet_addr() */
# include <netinet/in.h> // inet_addr() */
# include <arpa/inet.h> /* inet_addr() */
# include <string.h> /* bzero(), strerror() */
# include <stdio.h>
# include <errno.h> /* variable global errno */
# include <time.h> /*la utilizo para obtener la hora local y para la diferencia de segundos*/
# include <stdlib.h>

# define TAMBUF 1024
# define PORT 53220
# define FORMAT_DATETIME 21

int main( int argc, const char *argv[] )
{
    int                 caller_socket = 0;
    unsigned short int  listen_port = 0;
    unsigned long int   listen_ip_address = 0;
    struct sockaddr_in  listen_address;
    char                buffer[TAMBUF];
    char                datetime[FORMAT_DATETIME];
    struct tm           * date_time;
    struct tm           server_tm;
    time_t              current_time;
    time_t              server_time;

    /* Creación del socket */
    caller_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Asignando una dirección al socket */
    listen_address.sin_family = AF_INET;
    listen_port = htons(PORT);
    listen_address.sin_port = listen_port;
    listen_ip_address = inet_addr( argv[1] );
    listen_address.sin_addr.s_addr = listen_ip_address;
    bzero(&(listen_address.sin_zero), 8);

    /* Se conecta con el servidor */
    connect(caller_socket, (struct sockaddr *)&listen_address, sizeof(struct sockaddr));


	if( argc!=3)  /* Valido la cantidad de argumentos */
    {
        printf("ERROR en la entrada de argumentos.\n");
        printf("Forma de ejecución: \n");
        printf("./programa\t IP cadena\n");
		printf("./cliente\t 127.0.0.5 prueba_1\n");
		printf("ATENCION: para terminar ingrese la cadena fin\n");
        exit(EXIT_FAILURE);
    }

    /* Le envia al servidor el mensaje pasado como parametro */
    strncpy(buffer, argv[2], TAMBUF);
    send(caller_socket, buffer, TAMBUF, 0);

    /* Le envia al servidor la direccion ip */
    strncpy(buffer, inet_ntoa(listen_address.sin_addr), TAMBUF);
    send(caller_socket, buffer, TAMBUF, 0);

    /* Recibe del servidor la fecha y hora */
    recv(caller_socket, buffer, TAMBUF, 0);
    printf("\tFecha y hora del Servidor: %s\n", buffer );

    strptime( buffer, "%Y-%m-%d %H:%M:%S", &server_tm );
    server_time = mktime( &server_tm );

    /* Recibe del servidor el numero de cliente */
    recv(caller_socket, buffer, TAMBUF, 0);
    printf("\tCliente: %s\n", buffer );

    /* dormimos a la terminal un minuto */
	
    sleep(60);

    /* toma el system time */
    current_time = time( &current_time );

    /* convierte system time a local time */
    date_time = localtime(&current_time);

    /* convierte time a un string */
    sprintf(datetime,"%04d-%02d-%02d %02d:%02d:%02d",date_time->tm_year+1900
                    ,date_time->tm_mon+1,date_time->tm_mday,date_time->tm_hour
                    ,date_time->tm_min,date_time->tm_sec);

    printf("\tFecha y hora del Cliente: %s\n", datetime );

	/* La funcion difftime recibe dos variables de tipo time t, calcula su diferencia y */
	/* devuelve el resultado (double) expresado en segundos.*/
    /* Le envia al servidor la diferencia horaria */
    sprintf(buffer, "%.2lf", difftime( current_time, server_time )-3600 );
    send( caller_socket, buffer, TAMBUF, 0);


    /* Cerramos el socket como corresponde. */
    close(caller_socket);
}



/*******/
/* FIN */
/*******/