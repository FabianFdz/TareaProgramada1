/* Estos son los ficheros de cabecera usuales */
#include "Contacto.h"
#include <stdio.h>          
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//Definicion de MACROS
#define MAX_CONTACTS 200
#define CONTACTOS "contactos.txt" //Nombre del archivo de contactos
#define PORT 3550 /* El puerto que será abierto */
#define BACKLOG 2 /* El número de conexiones permitidas */
#define CONTACTO_ACTUAL "%s\t%s\t%s",actual.nombre,actual.ip,actual.puerto //para imprimir el contacto

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

#define CONTACTO_ACTUAL "%s\t%s\t%s",actual.nombre,actual.ip,actual.puerto //para imprimir el contacto
contacto contactos[MAX_CONTACTS]; //Arreglo de contactos de cantidad MAX_CONTACTS
int total_contactos=0; 	//Variable que guarda el numero total de contactos

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
			printf(PERROR "Archivo de contactos no encontrado\n");
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

void AgregarContactos(char usuario,char ip,char puerto){
	if (total_contactos==MAX_CONTACTS){
		printf(PERROR "Maximo numero de contactos alcanzado\n");
	}else{
		printf("Contacto: %s - %s - %s\n",usuario,puerto,ip);
		strcpy(contactos[total_contactos].nombre,usuario);
		strcpy(contactos[total_contactos].puerto,puerto);
		strcpy(contactos[total_contactos].ip,ip);
		
		//Las siguientes 2 lineas agregan el contacto al archivo txt
		FILE *archivo = fopen(CONTACTOS, "a"); //creo un puntero del tipo File y cargo el archivo hola.txt, si el archivo no existe, lo crea y si existe escribe al final
		fprintf(archivo, "%s,%s,%s\n", usuario, ip, puerto);//escribo la variable a en el archivo archivo.txt
		fclose(archivo);
		printf(PINFO "Contacto agregado exitosamente!\n");
	}
	return;
}

contacto BuscaContacto(char *nombre){
	contacto actual;
	while(cont<total_contactos){//Ciclo que imprime cada contacto con su info
		actual=contactos[cont];		
		if(strcmp(actual.nombre,nombre)==0){
			printf("Contacto encontrado\n");
			break;}
		cont++;
	}
	return actual;
}

char servidor_ordenes(){
    int sock;
    char datosrecibidos[2048];    //arreglo para recibir los datos  
    char datosenviados[2048];
    char datoColor[2048]="\x1b[34m";
    struct sockaddr_in servidor; //struct obligatorio   para puertos y almacenar direcciones
    int var1,misock;

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
		while(1){
			recv(misock,datosrecibidos,sizeof(datosrecibidos),0);
			if (strcmp(datosrecibidos,"R")){ //Cuando recibe R quiere decir que es para registrar un usuario
				char usuario[256];
				char ip[256];
				char puerto[256];
			    recv(misock,datosrecibidos,sizeof(datosrecibidos),0);
			    usuario = datosrecibidos;
			    recv(misock,datosrecibidos,sizeof(datosrecibidos),0);
			    ip = datosrecibidos;
			    recv(misock,datosrecibidos,sizeof(datosrecibidos),0);
			    puerto = datosrecibidos;
			    AgregarContactos(usuario,ip,puerto);
			}else if (strcmp(datosrecibidos,"I")){ //Cuando recibe I quiere decir que es para imprimir todos los contactos disponibles
				/* code */
			}       
			printf("%s\n",datosrecibidos);
    	}
	} 
}

int cliente(char *direccion){
	char direccion[16];
	int sock,bytesrecibidos;
 	struct sockaddr_in cliente;
 	char datosenviados[2048];
 	char datosrecibidos[2048];
 	char nombrerecibido[100];
 	char nombreenviado[100];
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
 	cliente.sin_addr.s_addr = inet_addr(direccion); //Conexión compu-compu
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
		int i = 0;
		while(i < total_contactos){
			datosenviados = ImprimeContactos(i);
			if(send(sock,datosenviados,sizeof(datosenviados),0)<0){
				perror("envio fallido\n");
				close(sock);
	    		exit(1);
			}
			i++;
		}
	}
	close(sock);
}

char * ImprimeContactos(int i){
	contacto actual=contactos[cont];		
	return CONTACTO_ACTUAL;
}

main(){
	servidor();
}