#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

//Colores para impresion en consola
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//Fin colores

#define MAX_CONTACTS 100
#define CONTACTOS "contactos.txt" //Nombre del archivo de contactos

/** Puerto  */
#define PORT 7000

/** Número máximo de hijos */
#define MAX_CHILDS 5

/** Longitud del buffer  */
#define BUFFERSIZE 512

typedef struct{ //struct para almacenar los contactos. 
	char ip[256];
	char nombre[256];
	char puerto[8];
}contacto;

typedef struct{ //struct para unir el socket con la IP 
	int socket;
	struct sockaddr_in datos;
}conexion;

/*La ventaja de typedef es que nos permite hacer "contacto nom;" en lugar de "struct contacto nom;"*/

int AtiendeCliente(int socket, struct sockaddr_in addr);
int DemasiadosClientes(int socket, struct sockaddr_in addr);
void error(int code, char *err);
void reloj(int loop);
void Inicia_chat(int socket, char *nombre_destinatario);
unsigned int sleep(unsigned int seconds);
int Busca_socket(char nombre[]);
int CargaContactos();
int Busca_socket(char nombre[]);
//char * AgregarContactos(char *datosContacto,struct sockaddr_in addr);
void AgregarContactos(char usuario[],char ip[],char puerto[]);

conexion Lista_de_conexiones[4];
contacto contactos[MAX_CONTACTS]; //Arreglo de contactos de cantidad MAX_CONTACTS
int total_contactos=0; //Variable que guarda el numero total de contactos
int total_conexiones=0; //Variable que guarda el numero total de conexiones activas

int CargaContactos(){ 	//funcion que carga los contactos del archivo contactos.txt
	char datos[256];
	char valor;
	char * line = NULL; //variables utilizadas en la lectura y gestion de las lineas de contactos.txt
    size_t len = 0;		//variables utilizadas en la lectura y gestion de las lineas de contactos.txt
    ssize_t read;		//variables utilizadas en la lectura y gestion de las lineas de contactos.txt
	int cont;			//variable contador
	FILE *archivo;		//variable de archivo "archivo"
	archivo = fopen(CONTACTOS, "r");	//apertura del archivo en modo lectura
	printf("Buscando archivo de contactos...\n");
	total_contactos = 0;
	if(archivo==NULL)	//Imprime un mensaje de error si el archivo de contactos no existe
		{
			printf("Archivo de contactos no encontrado\n");
			return 1; //Retorna 1 al main indicando que el archivo de contactos no fue encontrado
		}
	else
	{
		char nombre[512];
		char ipdir[512];
        char puerto[512];
		printf("Archivo de contactos encontrado\n");
		while ((read = getline(&line, &len, archivo)) != -1) //Comienza a leer linea por linea y a dividir cada linea de acuerdo a la estructura definida en el archivo contactos.txt
		{
		  //Separa cada linea del archivo por medio de la coma
          strcpy(nombre,strtok(line,","));
		  strcpy(ipdir,strtok(NULL,","));
          strcpy(puerto,strtok(NULL,","));          
		  strcpy(contactos[total_contactos].nombre,nombre);//ingresa el nombre al struct contacto actual
		  strcpy(contactos[total_contactos].puerto,puerto);//ingresa el puerto al struct contacto actual
		  strcpy(contactos[total_contactos].ip,ipdir);//ingresa la ip al struct contacto actual
		  total_contactos++;
		}
		printf("Contactos importados:" ANSI_COLOR_GREEN " %d\n"  ANSI_COLOR_RESET, total_contactos);
	}
	return 0;
}

void registro(int socket,struct sockaddr_in cliente){
    char buffer[BUFFERSIZE];
    
    char usuario[256];
    char puerto[32];
    char ip[32];
    char ch;

    strcpy(buffer,"Escribe tu nombre de usuario: \n");
    send(socket, buffer, strlen(buffer), 0);
    recv(socket, usuario, 256, 0);
    
    sprintf(ip,"%s",inet_ntoa(cliente.sin_addr));
    sprintf(puerto,"%i",8080);

    printf("Saliendo de registro\n");
    
    AgregarContactos(usuario,ip,puerto);
    printf("Usuario: %s\n",usuario);
    printf("Puerto: %s\n",puerto);
    printf("IP: %s\n",ip);
    printf("------FIN DE REGISTRO------\n");
    send(socket,"------FIN DE REGISTRO------",512,0);
    CargaContactos();
}

int main(int argv, char** argc){
	CargaContactos();
    int socket_host;
    struct sockaddr_in client_addr;
    struct sockaddr_in my_addr;
    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;
    int socket_client;
    fd_set rfds;        /* Conjunto de descriptores a vigilar */
    int childcount=0;
    int exitcode;

    int childpid;
    int pidstatus;

    int activated=1;
    int loop=0;
    socket_host = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_host == -1)
      	error(1, "No puedo inicializar el socket");
   
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

   
    if( bind( socket_host, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
      	error(2, "El puerto está en uso"); /* Error al hacer el bind() */

    if(listen( socket_host, 10) == -1 )
      	error(3, "No puedo escuchar en el puerto especificado");

    size_addr = sizeof(struct sockaddr_in);

    while(activated){
    	reloj(loop);
    	/* select() se carga el valor de rfds */
    	FD_ZERO(&rfds);
    	FD_SET(socket_host, &rfds);

    	/* select() se carga el valor de tv */
    	tv.tv_sec = 0;
    	tv.tv_usec = 500000;    /* Tiempo de espera */
   
    	if (select(socket_host+1, &rfds, NULL, NULL, &tv)){
        	if((socket_client = accept( socket_host, (struct sockaddr*)&client_addr, &size_addr))!= -1){
        		loop=-1;        /* Para reiniciar el mensaje de Esperando conexión... */
        		printf("\nSe ha conectado %s por su puerto %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
        		switch ( childpid=fork() ){
          			case -1:  /* Error */
            			error(4, "No se puede crear el proceso hijo");
            			break;
          			case 0:   /* Somos proceso hijo */
            			if (childcount<MAX_CHILDS){
            				//conexion nueva;
            				//nueva.socket = socket_client;
            				//nueva.datos = client_addr;
            				//Lista_de_conexiones[total_conexiones] = nueva;
            				total_conexiones++;
            				printf("El total de conexiones es: %i\n", childcount+1);
              				exitcode=AtiendeCliente(socket_client, client_addr);
            			}else
              				exitcode=DemasiadosClientes(socket_client, client_addr);

            			exit(exitcode); /* Código de salida */
          			default:  /* Somos proceso padre */
            			childcount++; /* Acabamos de tener un hijo */
            			close(socket_client); /* Nuestro hijo se las apaña con el cliente que
                        entró, para nosotros ya no existe. */
            			break;
          		}
          	}else
          		fprintf(stderr, "ERROR AL ACEPTAR LA CONEXIÓN\n");
      	}

    	/* Miramos si se ha cerrado algún hijo últimamente */
    	childpid=waitpid(0, &pidstatus, WNOHANG);
    	if (childpid>0){
        	childcount--;   /* Se acaba de morir un hijo */
	        /* Muchas veces nos dará 0 si no se ha muerto ningún hijo, o -1 si no tenemos hijos
       		con errorNo=10 (No child process). Así nos quitamos esos mensajes*/

       		if (WIFEXITED(pidstatus)){

       			/* Tal vez querremos mirar algo cuando se ha cerrado un hijo correctamente */
       			if (WEXITSTATUS(pidstatus)==99){
           			printf("\nSe ha pedido el cierre del programa\n");
           			activated=0;
       			}
       		}
   		}
    	loop++;
    }
    close(socket_host);

    return 0;
}

    /* No usamos addr, pero lo dejamos para el futuro */
int DemasiadosClientes(int socket, struct sockaddr_in addr)
{
    char buffer[BUFFERSIZE];
    int bytecount;

    memset(buffer, 0, BUFFERSIZE);
   
    sprintf(buffer, "Demasiados clientes conectados. Por favor, espere unos minutos\n");

    if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1)
      error(6, "No puedo enviar información");
   
    close(socket);

    return 0;
}

int AtiendeCliente(int socket, struct sockaddr_in addr){
    char buffer[BUFFERSIZE];
    char aux[BUFFERSIZE];
    int bytecount;
    int fin=0;
    int code=0;         /* Código de salida por defecto */
    while (!fin){
    	memset(buffer, 0, BUFFERSIZE);
    	if((bytecount = recv(socket, buffer, BUFFERSIZE, 0))== -1){
      		error(5, "No puedo recibir información");
      		fin = 1;
    	}

	    /* Evaluamos los comandos */
    	
    	if (strncmp(buffer, "1", 4)==0){/* Comando 1 - Agrega Contactos */
	        memset(buffer, 0, BUFFERSIZE);
	        registro(socket,addr);
    	}else if (strncmp(buffer, "2", 1)==0){/* Comando 2 - Imprime contactos */
        	memset(buffer, 0, BUFFERSIZE);
        	int i;
        	for(i=0; i<512||buffer[i]!='\0'; i++){
                buffer[i] = '\0';
            }
        	int cont=0;
        	sprintf(buffer,"%i",total_contactos);
        	send(socket, buffer, strlen(buffer), 0);
        	sleep(3);
        	sprintf(buffer,"NOMBRE\n");
        	send(socket, buffer, strlen(buffer), 0);
        	sleep(3);
			while(cont<=total_contactos){//Ciclo que manda cada nombre
	    		for(i=0; i<BUFFERSIZE||buffer[i]!='\0'; i++){
	    			buffer[i] = '\0';
	    		}
				contacto actual=contactos[cont];
  				sprintf(buffer,"%s",actual.nombre);
				if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1){
      				error(6, "No puedo enviar información");
				}
				sleep(1);
				cont++;
			}
      	/* Comando 3 - Inicia chat */
    	}else if (strncmp(buffer, "3", 1)==0){
        	memset(buffer, 0, BUFFERSIZE);
        	recv(socket, buffer, BUFFERSIZE, 0);
        	Inicia_chat(socket,buffer);
        	memset(buffer, 0, BUFFERSIZE);
    	/* Comando EXIT - Cierra la conexión actual */
    	}else if (strncmp(buffer, "4", 1)==0){
        	memset(buffer, 0, BUFFERSIZE);
        	sprintf(buffer, "Hasta luego. Vuelve pronto %s\n", inet_ntoa(addr.sin_addr));
        	if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1){
      			error(6, "No puedo enviar información");
        	}
        	fin=1;
    	/* Comando CERRAR - Cierra el servidor */
    	}else if (strncmp(buffer, "123456", 6)==0){
        	memset(buffer, 0, BUFFERSIZE);
        	sprintf(buffer, "Adiós. Cierro el servidor\n");
        	fin=1;
        	code=99;        /* Salir del programa */
      	}
    }

    close(socket);
    return code;
}

int Busca_socket(char nombre[]){
	int i = 0,socket;
	contacto actual;
	int cont=0;
	while(cont<total_contactos)//Ciclo que imprime cada contacto con su info
	{
		actual=contactos[cont];	
		if(strcmp(actual.nombre,nombre)==0){
			while(i<4){
				if(strcmp(inet_ntoa(Lista_de_conexiones[i].datos.sin_addr),actual.ip)==0){
					socket = Lista_de_conexiones[i].socket;
					return socket;
				}
			}
		}else if(cont==total_contactos){
			printf("No encontrado...\n");
			return 0;
		}
		cont++;
	}
	return 0; //Si no encuentra contacto
}

void AgregarContactos(char usuario[],char ip[],char puerto[]){
	if (total_contactos==MAX_CONTACTS)
		printf("Maximo numero de contactos alcanzado\n");
	else{
		printf("Contacto: %s - %s - %s\n",usuario,puerto,ip);
		strcpy(contactos[total_contactos].nombre,usuario);
		strcpy(contactos[total_contactos].puerto,puerto);
		strcpy(contactos[total_contactos].ip,ip);
		
		//Las siguientes 2 lineas agregan el contacto al archivo txt
		FILE *archivo = fopen(CONTACTOS, "a"); //creo un puntero del tipo File y cargo el archivo hola.txt, si el archivo no existe, lo crea y si existe escribe al final
		fprintf(archivo, "%s,%s,%s\n", usuario, ip, puerto);//escribo la variable a en el archivo archivo.txt
		fclose(archivo);
		printf("Contacto agregado exitosamente!\n");
	}
	return;
}

void Inicia_chat(int socket, char *nombre_destinatario){
	int socket_des = Busca_socket(nombre_destinatario);
	if (socket_des == 0){
		printf("No se encontro...\n");
	}
	char buffer1[BUFFERSIZE];
	char buffer2[BUFFERSIZE];
	int fin = 0;
	while(!fin){
		if((recv(socket, buffer2, BUFFERSIZE, 0))== -1){
      		error(5, "No puedo recibir información");
      		fin = 1;
      	}
      	if((recv(socket_des, buffer1, BUFFERSIZE, 0))== -1){
      		error(5, "No puedo recibir información");
      		fin = 1;
      	}
		if((send(socket, buffer1, strlen(buffer1), 0))== -1){
      		error(6, "No puedo enviar información");
      		fin = 1;
		}
		if((send(socket_des, buffer2, strlen(buffer2), 0))== -1){
      		error(6, "No puedo enviar información");
      		fin = 1;
		}
	}
}

void reloj(int loop){
  	if (loop==0)
    	printf("[SERVIDOR] Esperando conexión  ");

  	printf("\033[1D");        /* Codigo ANSI para retroceder 2 caracteres */
  	switch (loop%4){
    	case 0: printf("|"); break;
    	case 1: printf("/"); break;
    	case 2: printf("-"); break;
    	case 3: printf("\\"); break;
    	default:            /* No debemos estar aqui */
      	break;
    }
	fflush(stdout);       /* Actualiza la pantalla */
}

void error(int code, char *err){
  	char *msg=(char*)malloc(strlen(err)+14);
  	sprintf(msg, "Error %d: %s\n", code, err);
  	fprintf(stderr, msg);
  	exit(1);
}