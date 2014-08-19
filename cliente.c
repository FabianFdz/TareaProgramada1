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
#define MAXDATASIZE 100 /* El número máximo de datos en bytes */
#define IP system("hostname -I");
#define CONTACTO_ACTUAL "%s\t%s\t%s",actual.nombre,actual.ip,actual.puerto //para imprimir el contacto

servidor(){
    int sock;
    char datosrecibidos[2048];    //arreglo para recibir los datos  
    struct sockaddr_in servidor; //struct obligatorio   para puertos y almacenar direcciones
    int misock;

    sock = socket(AF_INET, SOCK_STREAM, 0);  
    int id;     
    if(sock ==-1){
       	perror("no se creo el socket");
        exit(1);
    }

 	servidor.sin_family = AF_INET;         
 	servidor.sin_port = htons(9005);     
 	servidor.sin_addr.s_addr = INADDR_ANY;
 	bzero(&(servidor.sin_zero),8);

 	if (bind(sock,(struct sockaddr *)&servidor,sizeof(servidor))){
  		perror("Fallo el bind");
  		exit(1);
 	}

 	listen(sock,5);
    misock = accept(sock, (struct sockaddr *)0,0);
    if (misock == -1){
        perror("Error al aceptar");
    }
    else{
    	int estado;
		while(estado = recv(misock,nombrerecibido,sizeof(nombrerecibido),0)>0){
			recv(misock,datosrecibidos,sizeof(datosrecibidos),0);               
			printf("%s\n",datosrecibidos);
    	}
    	if (estado == 0){
    		printf("Desconectado...\n");
    	}else{
    		printf("Error\n")
    	}
	} 
}

int cliente_chat(char *direccion){
	char direccion[16];
	int sock,bytesrecibidos;
 	struct sockaddr_in cliente;
 	char datosenviados[2048];
 	char datoColor[2048]="\x1b[36m";
 	struct hostent *hp;

 	printf("Digite su username: ");
	scanf("%s",nombreenviado);

 	sock = socket(AF_INET,SOCK_STREAM,0);
 	if(sock<0){
 		perror("No se creó el socket\n");
   		exit(1);
  	}
 	else{
  		printf("\t \t \x1b[36mIngresa 'Exit' en cualquier momento para salir\x1b[0m\n");
  		printf("\n");
    }

 	cliente.sin_family = AF_INET;         
 	cliente.sin_port = htons(9005);
 	cliente.sin_addr.s_addr = inet_addr("127.0.0.1"); //Conexión compu-compu
 	bzero(&(cliente.sin_zero),8); 
 
 	if(connect(sock,(struct sockaddr *)&cliente,sizeof(cliente))<0){
  		perror("conexion fallida\n");
  		close(sock);
  		exit(1);
 	}

	else{
    	while(1){
	    	char datoColor[2048]="\x1b[36m";
	    	printf("%s", nombreenviado);
	    	char ch;
	    	int i;
	    	for(i=0; i<2048||datosenviados[i]!='\0'; i++){
	    		datosenviados[i] = '\0';
	    	}
	    	i = 0;
	    	while( (ch=getchar())!='\n' ){
           		datosenviados[i]=ch;
           		i++;
     		}
			if(strcmp(datosenviados,"Exit")==0){
				printf("Salida");
				close(sock);
				return;
			}
    		strcat(datoColor,datosenviados);
    		strcat(datoColor,"\x1b[0m");
			if(send(sock,datoColor,sizeof(datoColor),0)<0){
				perror("envio fallido\n");
				close(sock);
	    		exit(1);
			}
		}
	}
	return 0;
}

int cliente_registro(char *ip,char *nombre,char *puerto){
	int sock,bytesrecibidos;
 	struct sockaddr_in cliente;
 	char datosenviados[2048];
 	char datoColor[2048]="\x1b[36m";
 	struct hostent *hp;

 	printf("Digite su username: ");
	scanf("%s",nombreenviado);

 	sock = socket(AF_INET,SOCK_STREAM,0);
 	if(sock<0){
 		perror("No se creó el socket\n");
   		exit(1);
  	}
 	else{
  		printf("\t \t \x1b[36mIngresa 'Exit' en cualquier momento para salir\x1b[0m\n");
  		printf("\n");
    }

 	cliente.sin_family = AF_INET;         
 	cliente.sin_port = htons(ip);
 	cliente.sin_addr.s_addr = inet_addr("127.0.0.1"); //Conexión compu-compu
 	bzero(&(cliente.sin_zero),8); 
 
 	if(connect(sock,(struct sockaddr *)&cliente,sizeof(cliente))<0){
  		perror("conexion fallida\n");
  		close(sock);
  		exit(1);
 	}

	else{
		if(send(sock,nombre,sizeof(nombre),0)<0 && send(sock,ip,sizeof(ip),0)<0 && send(sock,puerto,sizeof(puerto),0)<0){
			perror("envio fallido\n");
			close(sock);
	    	exit(1);
		}
	}
	close(sock);
}

void inicio(){
	int estado;
 	//Inicio de fork()
	switch(fork()){
		case -1: // Si es -1 quiere decir que ha habido un error
			perror(ANSI_COLOR_RED"Error de bifurcacion\n");
			break;
		case 0: // Cuando es cero quiere decir que es el proceso hijo
			
			break;			
		case 1: // Cuando es 1 es el padre
			
			wait(estado); //?
			close(socket2); //Se cierra el socket para no tener dificultades a la hora de correrlo nuevamente
			close(bind2); //Se cierra el bind para no tener dificultades a la hora de correrlo nuevamente
			break;
	}
}

void registro(){
	char usuario[256];
	char ip[256];
	char puerto[256];
	
	ip = IP;

	printf("Escribe tu nombre de usuario: ");
	scanf("%s",usuario); //Se hace un scan y se almacena almacena el valor en la variable usuario.

	printf("Desea cambiar el puerto(8080) por defecto? (S/N)\n");
	scanf("%s",puerto); //Se hace un scan y se almacena almacena el valor en la variable usuario.
	if (strcmp(puerto,"S")==0||strcmp(puerto,"s")==0){
		printf("Digite el puerto:\n");
		scanf("%s",puerto);
	}else{
		puerto = "8080";
	}
	cliente_registro(ip,nombre,puerto);
}

RecibeContactos(){
	int sock;
    char datosrecibidos[2048];    //arreglo para recibir los datos  
    struct sockaddr_in servidor; //struct obligatorio   para puertos y almacenar direcciones
    int misock;

    sock = socket(AF_INET, SOCK_STREAM, 0);  
    int id;     
    if(sock ==-1){
       	perror("no se creo el socket");
        exit(1);
    }

 	servidor.sin_family = AF_INET;         
 	servidor.sin_port = htons(9005);     
 	servidor.sin_addr.s_addr = INADDR_ANY;
 	bzero(&(servidor.sin_zero),8);

 	if (bind(sock,(struct sockaddr *)&servidor,sizeof(servidor))){
  		perror("Fallo el bind");
  		exit(1);
 	}

 	listen(sock,5);
    misock = accept(sock, (struct sockaddr *)0,0);
    if (misock == -1){
        perror("Error al aceptar");
    }
    else{
    	int estado;
    	printf(ANSI_COLOR_CYAN "NOMBRE\t    IP    \tPUERTO\n" ANSI_COLOR_RESET);//Cabecera de la impresion
		while(estado = recv(misock,nombrerecibido,sizeof(nombrerecibido),0)>0){
			recv(misock,datosrecibidos,sizeof(datosrecibidos),0);               
			printf("%s\n",datosrecibidos);
    	}
    	printf(ANSI_COLOR_YELLOW "\n==========Fin de contactos==========\n" ANSI_COLOR_RESET);
	} 
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
					registro();
					break;
				case 2:
					RecibeContactos();
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