#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//Colores para impresion en consola
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//Fin colores
#define PERROR ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET 
#define DIRECCION "192.168.146.130"

char puerto[56] = "8080";

void cliente_(int sock,struct sockaddr_in cliente){
    char datosenviados[2048];
    char datosrecibidos[2048];
    char nombrerecibido[100];
    char datoColor[2048]="\x1b[36m";
    char *nombreenviado = "Fabian";
    int primero = 0;
    while(sock != -1){
        //send(sock,nombreenviado,sizeof(nombreenviado),0);
        //char datoColor[2048]="\x1b[36m";
        if(primero){
            printf("%s: ", nombreenviado);
        }else{
            primero++;
        }
        char ch;
        int i;
        for(i=0; i<2048;i++){//||datosenviados[i]!='\0'; i++){
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
        //strcat(datoColor,datosenviados);
        //strcat(datoColor,"\x1b[0m");
        if(send(sock,datosenviados,sizeof(datosenviados),0)<0){
            //perror("envio fallido\n");
            //close(sock);
            //exit(1);
        }
        printf("Envio: %s\n", datosenviados);
        recv(sock,datosrecibidos,sizeof(datosrecibidos),0);
        printf("Respuesta del servidor%s: %s\n",nombrerecibido,datosrecibidos);
    }
}

void servidor_(int sock){
    int misock;
    while(1){
        misock = accept(sock, (struct sockaddr *)0,0);
        if (misock == -1){
            perror("Error al aceptar");
        }
    }
}

void chat(int sock,struct sockaddr_in cliente,int sock_sv,struct sockaddr_in servidor){//char *direccion, char *nombreenviado){
    int pid;
    int i;
    int estado;

    pid = fork();
 
    switch(pid){
        case -1: // Si pid es -1 quiere decir que ha habido un error
            perror("No se ha podido crear el proceso hijo\n");
            break;
        case 0: // Cuando pid es cero quiere decir que es el proceso hijo
            while(1){
                cliente_(socket,cliente);
            }
            break;
        default: // Cuando es distinto de cero es el padre
            while(1){
                servidor_(sock_sv);
            }
            wait(estado);
            //printf("Mi proceso hijo ya ha terminado.\n");
            break;
    }
}

char *registro(struct sockaddr_in servidor){
    char buffer[512];
    
    char usuario[256];
    int ip;
    char ch;
    
    printf("Escribe tu nombre de usuario: ");
    scanf("%s",usuario); //hago un scan y almaceno el valor en la variable usuario.

    ip = inet_ntoa(servidor.sin_addr);
    printf("%i\n",ip);
    printf("Desea cambiar el puerto predeterminado (8080)? (S/N)\n");
    ch=getchar(); //hago un scan y almaceno el valor en la variable usuario.
    if (ch == "s" || ch == "S"){
        printf("Desea cambiar el puerto predeterminado (8080)? (S/N)\n");
        int i = 0;
        while((ch=getchar())!='\n'){
            puerto[i]=ch;
            i++;
        }
        //scanf("%s",puerto); //hago un scan y almaceno el valor en la variable usuario.
    }
    printf("Antes de sprintf()\n");
    sprintf(buffer,usuario,",",ip,",",puerto);
    printf("%s\n",buffer);
    return buffer;
}

void menu(int sock,struct sockaddr_in cliente,int sock_sv,struct sockaddr_in servidor){
    char *buffer;
    char buffer1[512];
    system("clear");
    while(1){
        int opcion;
        printf(ANSI_COLOR_YELLOW "1." ANSI_COLOR_RESET "Agregar contactos\n");
        printf(ANSI_COLOR_YELLOW "2." ANSI_COLOR_RESET "Imprimir contactos\n");
        printf(ANSI_COLOR_YELLOW "3." ANSI_COLOR_RESET "Iniciar chat\n");
        printf("Opcion: ");
        scanf("%d",&opcion);
        buffer = "\0";
        if(opcion==1){
            send(sock,"1",sizeof("1"),0);
            buffer = registro(servidor);
            send(sock,buffer,sizeof(buffer),0);
        }else if(opcion==2){
            buffer = "2";
            if(send(sock,buffer,sizeof(buffer),0)<0){
                perror("envio fallido\n");
                close(sock);
                exit(1);
            }
            sleep(3);
            while(1){
                int i;
                for(i=0; i<512||buffer1[i]!='\0'; i++){
                    buffer1[i] = '\0';
                }
                recv(sock,buffer1,sizeof(buffer1),0);
                printf("%s\n", buffer1);
            }
        }else if(opcion==3){
            send(sock,"3",sizeof("3"),0);
            chat(sock,cliente,sock_sv,servidor);
        }
    }
}

int main(){//int argc, char const *argv[]){
    int sock,sock1,bytesrecibidos;
    struct sockaddr_in cliente;
    struct sockaddr_in servidor;
    char datosenviados[2048];
    char datosrecibidos[2048];
    char nombrerecibido[100];
    //char nombreenviado[100];
    char datoColor[2048]="\x1b[36m";

    sock1 = socket(AF_INET, SOCK_STREAM, 0);  
    
    if(sock1 ==-1)
        perror("No se creo el socket de escucha");

    servidor.sin_family = AF_INET;         
    servidor.sin_port = htons(puerto);     
    servidor.sin_addr.s_addr = INADDR_ANY;
    bzero(&(servidor.sin_zero),8);

    if (bind(sock1,(struct sockaddr *)&servidor,sizeof(servidor))){
        perror("Fallo el bind");
        close(sock1);
        exit(1);
    }

    listen(sock1,5);

/////////////////////////////////////////////////////////////////////////////////

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("No se creó el socket\n");
        exit(1);
    }
    cliente.sin_family = AF_INET;         
    cliente.sin_port = htons(7001);
    cliente.sin_addr.s_addr = inet_addr(DIRECCION); //Conexión compu-compu
    bzero(&(cliente.sin_zero),8); 
 
    if(connect(sock,(struct sockaddr *)&cliente,sizeof(cliente))<0){
        perror("Error");
        close(sock);
        return 0;
    }
    while(1){
        menu(sock,cliente,sock1,servidor);
    }
    return 0;
}