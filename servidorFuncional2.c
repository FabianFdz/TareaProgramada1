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
    /*Creacion de socket 2*/
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
    }

    /*Exito en creacion de sockets 1 & 2*/
  	printf("Servidor escuchando...\n");

    /*Configuracion de sockets 1 & 2*/
    int puerto = htons(0);
    printf("El puerto abierto es: %i\n", puerto);
    /*Configuraciones para sock1*/
 	cliente1.sin_family = AF_INET;         
 	cliente1.sin_port = puerto;//Asigna un puerto disponible de la compu
 	cliente1.sin_addr.s_addr = INADDR_ANY;
 	bzero(&(cliente1.sin_zero),8);
    printf("Configuracion lista en cliente1\n");
    /*Configuraciones para sock2*/
    puerto = htons(0);
    printf("El puerto abierto es: %i\n", puerto);
    cliente2.sin_family = AF_INET;         
    cliente2.sin_port = htons(9090);//Asigna un puerto disponible de la compu
    cliente2.sin_addr.s_addr = INADDR_ANY;
    bzero(&(cliente2.sin_zero),8);//*/
    printf("Configuracion lista en cliente2\n");
    /* Creacion de BIND para sock1 y sock2*/

    /*Bind con sock1*/
 	if(bind(sock1,(struct sockaddr *)&cliente1,sizeof(cliente1))){
  		intento_de_conexion = 1;
        while((bind(sock1,(struct sockaddr *)&cliente1,sizeof(cliente1))) && (intento_de_conexion < 6)){
            printf("Intento de crear bind %i\n",intento_de_conexion++);
        }
        if (intento_de_conexion == 6){
            printf("No se pudo crear bind\n");
            exit(1);
        }
 	}
    printf("Bind listo en cliente1\n");
    /*Bind con sock2*/
    if(bind(sock2,(struct sockaddr *)&cliente2,sizeof(cliente2))){
        intento_de_conexion = 1;
        while((bind(sock2,(struct sockaddr *)&cliente2,sizeof(cliente2))) && (intento_de_conexion < 6)){
            printf("Intento de crear bind %i\n",intento_de_conexion++);
        }
        if (intento_de_conexion == 6){
            printf("No se pudo crear bind\n");
            exit(1);
        }
    }
    printf("Bind listo en cliente2\n");
    /*Listen para sock1*/
    listen(sock1,5);
    printf("Esperando cliente1...\n");
    misock1 = accept(sock1,(struct sockaddr *)0,0);
    if (misock1 == -1){
        intento_de_conexion = 1;
        while(misock1 == -1){
            misock1 = accept(sock1,(struct sockaddr *)0,0);
            printf("Intento de conectar... (%i)\n",intento_de_conexion++);
        }
    }
    
    /*Conexion envia_mensajes con clientes*/
    printf("Conectando...\n");
    if(connect(sock1,(struct sockaddr *)&cliente1,sizeof(cliente1))<0){
        while((connect(sock1,(struct sockaddr *)&cliente1,sizeof(cliente1))<0)<0){     //Si hay un error con la conexion intenta 
            printf("Conectando... (Envio)\n");                                          //conectar n veces hasta que conecte
        }
    }

    printf("Esperando conexion # 2\n");

    /*Listen para sock2*/
    listen(sock2,5);
    misock2 = accept(sock2,(struct sockaddr *)0,0);
    if (misock2 == -1){
        intento_de_conexion = 1;
        while(misock2 == -1){
            misock2 = accept(sock2,(struct sockaddr *)0,0);
            printf("Intento de conectar... (%i)\n",intento_de_conexion++);
        }
    }
    /*Conexion envia_mensajes con clientes*/
    printf("Conectando...\n");
    if(connect(sock2,(struct sockaddr *)&cliente2,sizeof(cliente2))<0){
        while((connect(sock2,(struct sockaddr *)&cliente2,sizeof(cliente2))<0)<0){     //Si hay un error con la conexion intenta 
            printf("Conectando... (Envio)\n");                                          //conectar n veces hasta que conecte
        }
    }

	while(misock1 != -1 && misock2 != -1){
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