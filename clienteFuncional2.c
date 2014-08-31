#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void envia(){
    int sock,bytesrecibidos;
    struct sockaddr_in cliente;
    char datosenviados[2048];
    char datosrecibidos[2048];
    char nombrerecibido[100];
    char datoColor[2048]="\x1b[36m";
    struct hostent *he;

    char *nombreenviado = "Fabian";
    char *direccion = "192.168.146.130";

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("No se creó el socket\n");
        exit(1);
    }

    if ((he=gethostbyname(direccion))==NULL){       
        /* llamada a gethostbyname() */
        printf("gethostbyname() error\n");
        exit(-1);
    }

    printf("\t \t \x1b[32mBienvenido a Communicator\x1b[0m\n");
    printf("\t \t \x1b[36mIngresa 'Exit' en cualquier momento para salir\x1b[0m\n");
    printf("\n");
    printf("\n");

    cliente.sin_family = AF_INET;         
    cliente.sin_port = htons(9005);
    cliente.sin_addr = *((struct in_addr *)he->h_addr); //Conexión compu-compu
    bzero(&(cliente.sin_zero),8); 
 
    if(connect(sock,(struct sockaddr *)&cliente,sizeof(cliente))<0){
        perror("conexion fallida\n");
        close(sock);
        exit(1);
    }else{
        while(1){
            send(sock,nombreenviado,sizeof(nombreenviado),0);
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
}

void escucha(){
    int sock;
    char datosrecibidos[2048];    //arreglo para recibir los datos  
    char datosenviados[2048];
    //char nombreenviado[15];
    char nombrerecibido[15];
    char datoColor[2048]="\x1b[34m";
    struct sockaddr_in servidor; //struct obligatorio   para puertos y almacenar direcciones
    struct sockaddr_in client;
    int var1,misock;

    sock = socket(AF_INET, SOCK_STREAM, 0);  
    int id;     
    if(sock ==-1){
        perror("no se creo el socket");
        exit(1);
    }
      else{
        printf("Servidor escuchando...\n");
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
    misock = accept(sock,(struct sockaddr *)&client,sizeof(client));
    if (misock == -1){
        perror("Error al aceptar");
    }
    else{
        while(1){
            int d1 = recv(misock,nombrerecibido,sizeof(nombrerecibido),0);
            int d2 =recv(misock,datosrecibidos,sizeof(datosrecibidos),0);
            if (d1 && d2){
                printf("%s",nombrerecibido);
                printf("%s\n",datosrecibidos);
            }
        }
    } 
}

void chat(){//char *direccion, char *nombreenviado){
    /*Fork para envio y recepcion de mensajes*/
    int pid,i,estado;
    printf("Bifurcando...\n");
    pid = fork();
 
    switch(pid){
        case -1: // Si pid es -1 quiere decir que ha habido un error
            perror("No se ha podido crear el proceso hijo\n");
            break;
        case 0: // Proceso hijo (Recepcion de mensajes)
            while(1){
                escucha();
            }
            break;
        case 1: // Proceso padre (Envio de mensajes)
            while(1){
                envia();
            }
            wait(estado); // La función wait detiene el proceso padre y se queda esperando hasta que termine el hijo
            break;
    }
    printf("----------------Fin de chat----------------\n");;
}

int main(){//int argc, char const *argv[]){
    chat();
    return 0;
}