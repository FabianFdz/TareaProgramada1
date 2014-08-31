#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void chat(){//char *direccion, char *nombreenviado){
    int sock,sock2,bytesrecibidos;
    struct sockaddr_in servidor; //Servidor para enviar
    struct sockaddr_in servidor2; //Servidor para recibir
    char datosenviados[2048];
    char datosrecibidos[2048];
    char nombrerecibido[100];
    char datoColor[2048]="\x1b[36m";
    struct hostent *he;

    //Para prueba:
    char *direccion = "192.168.146.130";
    char *nombreenviado = "Fabian";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock ==-1){ //Si no se puede crear el socket se intenta crear 5 veces mas
        printf("No se pudo crear el socket\n");
        exit(1);
    }

    if ((he=gethostbyname(direccion))==NULL){       
        /* llamada a gethostbyname() */
        printf("gethostbyname() error\n");
        exit(-1);
    }

    printf("\t \t \x1b[32mBienvenido a Messenger\x1b[0m\n");
    printf("\t \t \x1b[36mIngresa 'Exit' en cualquier momento para salir\x1b[0m\n");
    printf("\n");

    /*Configuraciones para sock1*/
    printf("Configurando...\n");
    servidor.sin_family = AF_INET;         
    servidor.sin_port = htons(3095);//Asigna un puerto disponible de la compu
    servidor.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(servidor.sin_zero),8);

    /*Conexion envia_mensajes con el servidor*/
    printf("Conectando...\n");
    if(connect(sock,(struct sockaddr *)&servidor,sizeof(servidor))<0){
        while((connect(sock,(struct sockaddr *)&servidor,sizeof(servidor))<0)<0){     //Si hay un error con la conexion intenta 
            printf("Conectando... (Envio)\n");                                          //conectar n veces hasta que conecte
        }
    }

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
                printf("Proceso hijo en proceso...\n");
                recv(sock,nombrerecibido,sizeof(nombrerecibido),0);
                recv(sock,datosrecibidos,sizeof(datosrecibidos),0);
                //AQUI VA EL DESENCRIPTADO
                if(strcmp(datosenviados,"Exit")==0){
                    printf("Salida");
                    //close(sock2);
                    close(sock);
                    break;
                }
                printf("%s",nombrerecibido);                 
                printf("%s\n",datosrecibidos);
            }
            break;
        case 1: // Proceso padre (Envio de mensajes)
            /* Envio de mensajes */
            while(1){
                printf("Proceso padre en proceso...\n");
                send(sock,nombreenviado,sizeof(nombreenviado),0); //Envia nombre
                char datoColor[2048]="\x1b[36m"; //Pone color al mensaje que se va a enviar
                printf("%s", nombreenviado); //Imprime en consola el nombre a enviar
                char ch; //Se usa para almacenar caracter que luego se va a adjuntar al mensaje
                int i; //Contador
                for(i=0; i<2048||datosenviados[i]!='\0'; i++){ //For para limpiar la variable datosenviados
                    datosenviados[i] = '\0'; //Se ponen todos en null. Ejm [H][o][l][a] -> [\0][\0][\0][\0]                 
                }
                i = 0; //Contador
                while( (ch=getchar())!='\n' ){//Aqui se construye el mensaje caracter por caracter
                    datosenviados[i]=ch;//Se va adjuntado cada letra a datosenviados
                    i++;//Se suma contador
                }
                if(strcmp(datosenviados,"Exit")==0 || strcmp(datosenviados,"exit")==0){ //En el caso de que el mensaje sea Exit, se cerrara el chat
                    //AQUI VA EL ENCRIPTADO
                    send(sock,datosenviados,sizeof(datosenviados),0);//envia el mensaje "Exit" al otro cliente para que cierre la conexion
                    printf("Salida");
                    close(sock);//Cierra el socket
                    break;//Acaba el while
                }
                strcat(datoColor,datosenviados);//Concatena para darle color
                strcat(datoColor,"\x1b[0m"); //Concatena para darle color
                //AQUI VA EL ENCRIPTADO
                if(send(sock,datoColor,sizeof(datoColor),0)<0){//Envia datos al servidor
                    perror("envio fallido\n");
                    close(sock); //cierra socket
                    //exit(1);
                }
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