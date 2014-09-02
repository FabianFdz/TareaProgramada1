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

#define MAX_CONTACTS 10
#define CONTACTOS "contactos.txt" //Nombre del archivo de contactos

/** Puerto  */
#define PORT 7001

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
void reloj_registro(int loop);
void Inicia_chat(int socket, struct sockaddr_in addr, char *nombre_destinatario);
unsigned int sleep(unsigned int seconds);
int Busca_socket(char nombre[]);
int CargaContactos();
int Busca_socket(char nombre[]);
char * AgregarContactos(char *datosContacto,int socket,struct sockaddr_in addr);

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
	if(archivo==NULL)	//Imprime un mensaje de error si el archivo de contactos no existe
		{
			printf("Archivo de contactos no encontrado\n");
			return 1; //Retorna 1 al main indicando que el archivo de contactos no fue encontrado
		}
	else
	{
		printf("Archivo de contactos encontrado\n");
		while ((read = getline(&line, &len, archivo)) != -1) //Comienza a leer linea por linea y a dividir cada linea de acuerdo a la estructura definida en el archivo contactos.txt
		{
		  //Separa cada linea del archivo por medio de la coma
          char* nombre= strtok(line,",");
		  char* ipdir = strtok(NULL,",");
          char* puerto = strtok(NULL,",");          
		  strcpy(contactos[total_contactos].nombre,nombre);//ingresa el nombre al struct contacto actual
		  strcpy(contactos[total_contactos].puerto,puerto);//ingresa el puerto al struct contacto actual
		  strcpy(contactos[total_contactos].ip,ipdir);//ingresa la ip al struct contacto actual
		  total_contactos++;
		}
		printf("Contactos importados:" ANSI_COLOR_GREEN " %d\n"  ANSI_COLOR_RESET, total_contactos);
	}
	return 0;
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
            				printf("El total de conexiones es: %i\n", total_conexiones);
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
	printf("Estoy en AtiendeCliente()\n");
    char buffer[BUFFERSIZE];
    char aux[BUFFERSIZE];
    int bytecount;
    int fin=0;
    int code=0;         /* Código de salida por defecto */
    while (!fin){
    	printf("Estoy dentro del while\n");
    	memset(buffer, 0, BUFFERSIZE);
    	if((bytecount = recv(socket, buffer, BUFFERSIZE, 0))== -1)
      		error(5, "No puedo recibir información");
      	//printf("Mensaje de %s: %s\n",addr.sin_addr, buffer);
	    /* Evaluamos los comandos */
    	/* Comando 1 - Agrega Contactos */
    	if (strncmp(buffer, "1", 4)==0){
    		//reloj_registro(i);
    		printf("Estoy dentro del 1\n");
	        memset(buffer, 0, BUFFERSIZE);
	        sleep(10);
	        printf("Pase el sleep()\n");
	        recv(socket, buffer, BUFFERSIZE, 0);
    	    strcpy(buffer,AgregarContactos(buffer,socket,addr));
    	    sprintf(buffer,"Hola estas en el 1");
    	    if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1){
      			error(6, "No puedo enviar información");
    	    }
    	/* Comando 2 - Imprime contactos */
    	}else if (strncmp(buffer, "2", 1)==0){
        	memset(buffer, 0, BUFFERSIZE);
        	int cont=0;
        	sprintf(buffer,"NOMBRE\t    IP    \tPUERTO\n");
        	send(socket, buffer, strlen(buffer), 0);
        	sleep(3);
        	char *msg=(char*)malloc(35);
			while(cont<=total_contactos){//Ciclo que imprime cada contacto con su info
				int i;
	    		for(i=0; i<BUFFERSIZE||buffer[i]!='\0'; i++){
	    			buffer[i] = '\0';
	    		}
				contacto actual=contactos[cont];
				/*strcpy(buffer,actual.nombre);
				strcpy(buffer,"\t");
				strcpy(buffer,actual.ip);
				strcpy(buffer,"\t");
				strcpy(buffer,actual.puerto);*/
  				sprintf(buffer,"%s\t%s\t%s",actual.nombre,actual.ip,actual.puerto);
				if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1){
      				error(6, "No puedo enviar información");
				}
				sleep(3);
				cont++;
			}
      	/* Comando 3 - Inicia chat */
    	}else if (strncmp(buffer, "3", 1)==0){
        	memset(buffer, 0, BUFFERSIZE);
        	sleep(7);
        	recv(socket, buffer, BUFFERSIZE, 0);
        	Inicia_chat(socket,addr,buffer);
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
			return 0;
		}
		cont++;
	}
	return 0; //Si no encuentra contacto
}

char *Nombre(char *datos){
	int i = 0,i2 = 0;
	char *nombre;
	while(datos[i] != ","){
		nombre[i] = datos[i];
		i++;
	}
	return nombre;
}

char *IP(char *datos){
	int i = 0,i2 = 0;
	char *nombre;
	while(i2<2){
		while(datos[i] != "," && i2 == 1){
			nombre[i] = datos[i];
			i++;
		}
		i2++;
	}
	return nombre;
}

char *Puerto(char *datos){
	int i = 0,i2 = 0;
	char *nombre;
	while(i2<2){
		while(datos[i] != "\0" && i2 == 2){
			nombre[i] = datos[i];
			i++;
		}
		i2++;
	}
	return nombre;
}

char * AgregarContactos(char *datosContacto,int socket,struct sockaddr_in addr){
	char *mensaje = "Contacto no pudo ser cargado!";
	if(total_contactos==MAX_CONTACTS)
		return mensaje;
	else{
		char *usuario = Nombre(datosContacto);
		char *puerto = Puerto(datosContacto);
		char *ip = inet_ntoa(addr.sin_addr);
		strcpy(contactos[total_contactos].nombre,usuario);
		strcpy(contactos[total_contactos].puerto,puerto);
		strcpy(contactos[total_contactos].ip,ip);
		
		//Las siguientes 2 lineas agregan el contacto al archivo txt
		FILE *archivo = fopen(CONTACTOS, "a"); //creo un puntero del tipo File y cargo el archivo hola.txt, si el archivo no existe, lo crea y si existe escribe al final
		fprintf(archivo, "%s\n", datosContacto);//escribo la variable a en el archivo archivo.txt
		fclose(archivo);
		mensaje = "Contacto agregado exitosamente!";
	}
	return mensaje;
}

void Inicia_chat(int socket, struct sockaddr_in addr, char *nombre_destinatario){
	int socket_des = Busca_socket(nombre_destinatario);
	if (socket_des == 0){
		return;
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

void reloj_registro(int loop){
  	if (loop==0)
    	printf("[SERVIDOR] Esperando respuesta  ");

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