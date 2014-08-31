#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

void servidor_chat(){
  	int sock1,sock2,intento_de_conexion,misock1,misock2;
  	char datosrecibidos[2048];    //arreglo para recibir los datos  
  	char datosenviados[2048];
  	char nombrerecibido[15];
  	char datoColor[2048]="\x1b[34m";
    struct sockaddr_in servidor;
  	struct sockaddr_in cliente1; //struct obligatorio para puertos y almacenar direcciones de conexion 1
    struct sockaddr_in cliente2; //struct obligatorio para puertos y almacenar direcciones de conexion 2

    /*Creacion de socket 1*/
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
  	if(sock1 ==-1){ //Si no se puede crear el socket se intenta crear 5 veces mas
        intento_de_conexion = 1;
    	while((sock1 == -1) && (intento_de_conexion < 6)){
            close(sock1);
            sock1 = socket(AF_INET, SOCK_STREAM, 0);
            printf("Intento de crear socket %i\n",intento_de_conexion++);
        }
        if (intento_de_conexion == 6){//Si es 6 quiere decir que no se pudo crear
            printf("No se pudo crear el socket\n");
            exit(1);
        }
  	}
    /*Creacion de socket 2*
    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if(sock2 == -1){//Si no se puede crear el socket se intenta crear 5 veces mas
        intento_de_conexion = 1;
        while((sock2 == -1) && (intento_de_conexion < 6)){
            close(sock2);
            sock2 = socket(AF_INET, SOCK_STREAM, 0);
            printf("Intento de crear socket %i\n",intento_de_conexion++);
        }
        if (intento_de_conexion == 6){//Si es 6 quiere decir que no se pudo crear
            printf("No se pudo crear el socket\n");
            exit(1);
        }
    }//*/

    /*Exito en creacion de sockets 1 & 2*/
  	printf("Servidor escuchando...\n");

    /*Configuracion de sockets 1 & 2*/
    //int puerto = htons(0);
    //printf("El puerto abierto es: %i\n", puerto);
    /*Configuraciones para sock1*/
 	servidor.sin_family = AF_INET;         
 	servidor.sin_port = puerto;//Asigna un puerto disponible de la compu
 	servidor.sin_addr.s_addr = INADDR_ANY;
 	bzero(&(servidor.sin_zero),8);
    printf("Configuracion lista en cliente1\n");

    /*Bind*/
 	if(bind(sock1,(struct sockaddr *)&servidor,sizeof(servidor))){
  		intento_de_conexion = 1;
        while((bind(sock1,(struct sockaddr *)&servidor,sizeof(servidor))) && (intento_de_conexion < 6)){
            printf("Intento de crear bind %i\n",intento_de_conexion++);
        }
        if (intento_de_conexion == 6){
            printf("No se pudo crear bind\n");
            exit(1);
        }
 	}
    printf("Bind listo\n");

    /*Listen para 5 conexiones*/
    listen(sock1,5);
    printf("Esperando cliente 1...\n");
    misock1 = accept(sock1,(struct sockaddr *)&cliente1,sizeof(cliente1));
    if (misock1 == -1){
        intento_de_conexion = 1;
        while(misock1 == -1){
            misock1 = accept(sock1,(struct sockaddr *)0,0);
            printf("Intento de conectar... (%i)\n",intento_de_conexion++);
        }
    }
    printf("Esperando cliente 2...\n");
    misock2 = accept(sock1,(struct sockaddr *)&cliente2,sizeof(cliente2));
    if (misock2 == -1){
        intento_de_conexion = 1;
        while(misock2 == -1){
            misock2 = accept(sock1,(struct sockaddr *)0,0);
            printf("Intento de conectar... (%i)\n",intento_de_conexion++);
        }
    }

	while(1){
        /*Recepcion y reenvio de nombres*/
		if(recv(misock1,nombrerecibido,sizeof(nombrerecibido),0) != -1){ //Se recibe el nombre del usuario 1
            printf("%s\n",nombrerecibido);
            send(misock2,nombrerecibido,sizeof(nombrerecibido),0); //Se reenvia al usuario 2
        }
        if(recv(misock2,nombrerecibido,sizeof(nombrerecibido),0) != -1){ //Se recibe el nombre del usuario 2
            printf("%s\n",nombrerecibido);
            send(misock1,nombrerecibido,sizeof(nombrerecibido),0); //Se reenvia al usuario 1
        }
        /*Fin de recepcion y reenvio de nombres*/

        /*Recepcion y reenvio de mensajes*/
		if(recv(misock1,datosrecibidos,sizeof(datosrecibidos),0) != -1){ //Se recibe el mensaje del usuario 1
            printf("%s\n",datosrecibidos);
            send(misock2,datosrecibidos,sizeof(datosrecibidos),0); //Se reenvia al usuario 2
        }
        if(recv(misock2,datosrecibidos,sizeof(datosrecibidos),0) != -1){ //Se recibe el mensaje del usuario 2
            printf("%s\n",datosrecibidos);
            send(misock1,datosrecibidos,sizeof(datosrecibidos),0); //Se reenvia al usuario 1
        }
        /*Fin de recepcion y reenvio de mensajes*/
    }
    close(misock1);
    close(misock2);
    printf("Se perdio la conexion...\n");
    return;
}

main(){
    servidor_chat();
}