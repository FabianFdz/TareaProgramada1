#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

//Colores para impresion en consola
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//Fin colores
//Colores configurados para mostrar "error" y "advertencia" en otro color en los mensajes.
#define PERROR ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET 
#define PWARNING ANSI_COLOR_YELLOW "Advertencia: " ANSI_COLOR_RESET
#define PINFO ANSI_COLOR_GREEN "Informacion: " ANSI_COLOR_RESET
//Fin colores.
#define GRACIAS printf("Gracias por utilizar Communicator!\n"); //imprimir un mensaje al final del programa
//Definicion de macros
#define PORT 3550   	/* El Puerto Abierto del nodo remoto */
#define MAXDATASIZE 256 /* El número máximo de datos en bytes */

int recibe(char *argv[]){ //Solo recibe mensajes
   	int fd, numbytes;       
   	/* ficheros descriptores */

   	char buf[MAXDATASIZE];  
   	/* en donde es almacenará el texto recibido */

   	struct hostent *he;         
   	/* estructura que recibirá información sobre el nodo remoto */

   	struct sockaddr_in server;  
   	/* información sobre la dirección del servidor */

   	if ((he=gethostbyname(argv[1]))==NULL){       
    	/* llamada a gethostbyname() */
      	printf("gethostbyname() error\n");
      	exit(-1);
   	}

   	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      	/* llamada a socket() */
      	printf("socket() error\n");
      	exit(-1);
   	}

   	server.sin_family = AF_INET;
   	server.sin_port = htons(PORT); 
   	server.sin_addr = *((struct in_addr *)he->h_addr);  
   	/*he->h_addr pasa la información de ``*he'' a "h_addr" */
   	bzero(&(server.sin_zero),8);

   	if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){ 
      	/* llamada a connect() */
      	printf("connect() error\n");
      	exit(-1);
   	}
   	while(1){
   		if ((numbytes=recv(fd,buf,MAXDATASIZE,0)) == -1){  
      		/* llamada a recv() */
      		printf("Error en recv() \n");
      		exit(-1);
   		}

   		buf[numbytes]='\0';

   		printf("Mensaje del Servidor: %s\n",buf); 
   		/* muestra el mensaje de bienvenida del servidor =) */

   		close(fd);   /* cerramos fd =) */

	}
}

envia(){
   int fd, fd2; /* fd es el socket y fd2 el que acepta */

   struct sockaddr_in server; 
   /* para la información de la dirección del servidor */

   struct sockaddr_in client; 
   /* para la información de la dirección del cliente */

   int sin_size;

   /* A continuación la llamada a socket() */
   if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("Error en socket()\n");
      exit(-1);
   }

   server.sin_family = AF_INET;         

   server.sin_port = htons(PORT); 
   /* ¿Recuerdas a htons() de la sección "Conversiones"? =) */

   server.sin_addr.s_addr = INADDR_ANY; 
   /* INADDR_ANY coloca nuestra dirección IP automáticamente */

   bzero(&(server.sin_zero),8); 
   /* escribimos ceros en el reto de la estructura */


   /* A continuación la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1) {
      printf("Error en bind() \n");
      exit(-1);
   }     

   if(listen(fd,BACKLOG) == -1) {  /* llamada a listen() */
      printf("Error en listen()\n");
      exit(-1);
   }

   while(1) {
      	sin_size = sizeof(struct sockaddr_in);
      	/* A continuación la llamada a accept() */
      	if ((fd2 = accept(fd,(struct sockaddr *)&client,&sin_size))==-1) {
        	printf("Error en accept()\n");
         	exit(-1);
      	}

      	printf("Se obtuvo una conexión desde %s\n",inet_ntoa(client.sin_addr));
      	/* que mostrará la IP del cliente */

      	send(fd2,"Bienvenido a mi servidor.\n",22,0); 
      	/* que enviará el mensaje de bienvenida al cliente */

      	close(fd2); /* cierra fd2 */
   	}
}

void inicio(){
	int estado;
	struct sockaddr_in servidor;
	servidor.sin_family = AF_INET;         
 	servidor.sin_port = htons(9005);     
 	servidor.sin_addr.s_addr = INADDR_ANY;
 	bzero(&(servidor.sin_zero),8);
 	//Inicio de fork()
	switch(fork()){
		case -1: // Si es -1 quiere decir que ha habido un error
			perror(ANSI_COLOR_RED"Error de bifurcacion\n");
			break;
		case 0: // Cuando es cero quiere decir que es el proceso hijo
			socket1 = crea_socket();
 			bind1 = crea_bind(socket1);
 			while(bind1){
 				perror("Fallo bind 1 \n Intentando nuevamente...")
 				bind1 = crea_bind(socket1);
 			}

			break;			
		case 1: // Cuando es 1 es el padre
			socket2 = crea_socket();
 			bind2 = crea_bind(socket2);
 			while(bind2){
 				perror("Fallo bind 2 \n Intentando nuevamente...")
 				bind2 = crea_bind(socket2);
 			}

			wait(estado); //?
			close(socket2); //Se cierra el socket para no tener dificultades a la hora de correrlo nuevamente
			close(bind2); //Se cierra el bind para no tener dificultades a la hora de correrlo nuevamente
			break;
	}
}
void registro(){
	char usuario[256];
	printf("Digite su nombre de usuario:");
	scanf("%s",usuario);
}

void menu(){
	int i=1;
	while(i){
		printf("\t\t"ANSI_COLOR_GREEN" *** "ANSI_COLOR_RESET"MENU PRINCIPAL"ANSI_COLOR_GREEN" ***\n"ANSI_COLOR_RESET);
		int opcion;
		printf(ANSI_COLOR_RED "1." ANSI_COLOR_RESET "Registrarse\n");
		printf(ANSI_COLOR_RED "2." ANSI_COLOR_RESET "Imprimir contactos disponibles\n");
		printf(ANSI_COLOR_RED "3." ANSI_COLOR_RESET "Iniciar chat\n");
		printf(ANSI_COLOR_RED "0." ANSI_COLOR_RESET "Salir\n");
		printf("Opcion: ");
		scanf("%d",&opcion);
		if(opcion<0 || opcion>9){
			printf(PERROR "Valor invalido\n");
		}else{
			switch(opcion){
				case 1:
					//registro();
					break;
				case 2:
					//ImprimeContactos();
					break;
				case 3:
					inicio();
					break;
				default:
					i=0;
					GRACIAS
					break;
			}			
		}
	}
	return;
}

main(){
	system("clear");
	printf("\t\t"ANSI_COLOR_GREEN" *** "ANSI_COLOR_RESET"BIENVENIDO A MESSENGER"ANSI_COLOR_GREEN" ***\n"ANSI_COLOR_RESET);
	menu();
}
