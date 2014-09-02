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

char* encriptarFrase (char frase[]){
    char *res;
    int i;
    int largo = strlen(frase);
    for(i=0 ; i<largo ; ++i){
        //convierte las minusculas a mayuscula.
        //*(frase + i) = toupper(*(frase + i));
        //descarta los digitos
        if(isalpha(*(frase + i))){
            //verifica los casos especiales X-Y-Z
            //if(*(frase + i) > 87)
                *(frase + i) = *(frase + i) - 23;
            //else
            //{
                //*(frase + i) = *(frase + i) + 3;
            //}
        }
        if(isdigit(*(frase + i))){
                *(frase + i) = *(frase + i) + 6;
        }else{
                *(frase + i) = *(frase + i) - 4;
        }
    }
    strcpy(res,frase);
    return res;
}

char* desencriptarFrase (char frase[]){
    int i;
    char *res;
    int largo = strlen(frase);
    for(i=0 ; i<largo ; ++i){
        if(isalpha(*(frase + i))){
            //if(*(frase + i) < 68)
                *(frase + i) = *(frase + i) + 23;
            //else
            //{
                //*(frase + i)= *(frase + i) - 3;
            //}
        }
        if(isdigit(*(frase + i))){
                *(frase + i) = *(frase + i) - 6;
        }else{
                *(frase + i) = *(frase + i) + 4;
        }
    }
    strcpy(res,frase);
    return res;
}

void cliente_(int sock){
    char datosenviados[2048];
    char datosrecibidos[2048];
    char nombrerecibido[100];
    char datoColor[2048]="\x1b[36m";
    char *nombreenviado = "Fabian";
    int primero = 0;
    while(sock != -1){
        send(sock,nombreenviado,sizeof(nombreenviado),0);
        char datoColor[2048]="\x1b[36m";
        if(primero){
            printf("%s: ", nombreenviado);
        }else{
            primero++;
        }
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
        strcpy(datosenviados,encriptarFrase(datosenviados));
        if(send(sock,datosenviados,sizeof(datosenviados),0)<0){
            perror("envio fallido\n");
            close(sock);
            exit(1);
        }
        //printf("Envio: %s\n", datosenviados);
    }
}

void servidor_(int sock){
    //int misock;
    char datosrecibidos[2048];
    while(1){
        /*misock = accept(sock, (struct sockaddr *)&servidor,sizeof(servidor));
        if (misock == -1){
            perror("Error al aceptar");
        }//*/
        recv(sock,datosrecibidos,sizeof(datosrecibidos),0);
        strcpy(datosrecibidos,desencriptarFrase(datosrecibidos));
        printf("%s\n",datosrecibidos);
    }
}

void chat(int sock){//,struct sockaddr_in cliente,int sock_sv,struct sockaddr_in servidor){//char *direccion, char *nombreenviado){
    int pid;
    int i;
    int estado;
    char buffer[512];

    pid = fork();
    //printf("Con quien desea iniciar una conversacion: ");
    //scanf("%s",buffer);
    //send(sock,buffer,512,0);
    switch(pid){
        case -1: // Si pid es -1 quiere decir que ha habido un error
            perror("No se ha podido crear el proceso hijo\n");
            break;
        case 0: // Cuando pid es cero quiere decir que es el proceso hijo
            cliente_(socket);
            break;
        default: // Cuando es distinto de cero es el padre
            servidor_(socket);
            wait(estado);
            //printf("Mi proceso hijo ya ha terminado.\n");
            break;
    }
}

registro(int socket){
    char buffer[512],buffer1[512];
    int cont=1;

    recv(socket,buffer1,512,0);//se recibe mensaje del servidor
    printf("%s",buffer1);//Se imprime mensaje del servidor
    scanf("%s",buffer); //se hace un scan y almacena el valor en la variable usuario.
    send(socket,buffer,strlen(buffer),0);//se envia el usuario de vuelta
    recv(socket,buffer1,512,0);
    printf("%s\n",buffer1);
}

void menu(int sock,struct sockaddr_in cliente,int sock_sv,struct sockaddr_in servidor){
    char buffer1[512];
    system("clear");
    while(1){
        int i;
        memset(buffer1, 0, 512);
        int opcion;
        printf(ANSI_COLOR_YELLOW "1." ANSI_COLOR_RESET "Agregar contactos\n");
        printf(ANSI_COLOR_YELLOW "2." ANSI_COLOR_RESET "Imprimir contactos\n");
        printf(ANSI_COLOR_YELLOW "3." ANSI_COLOR_RESET "Iniciar chat\n");
        printf("Opcion: ");
        scanf("%d",&opcion);
        if(opcion==1){////////////Registro///////////////
            send(sock,"1",sizeof("1"),0);
            registro(sock);
        }else if(opcion==2){///////////Impresion de contactos
            if(send(sock,"2",sizeof("2"),0)<0){
                perror("envio fallido\n");
                close(sock);
                exit(1);
            }
            //sleep(3);
            recv(sock,buffer1,sizeof(buffer1),0);
            printf("Total de Contactos: %s\n", buffer1);
            int x = atoi(buffer1);
            while(x>=0){
                for(i=0; i<512&&buffer1[i]!='\0'; i++){
                    buffer1[i] = '\0';
                }
                //sleep(1);
                if(recv(sock,buffer1,sizeof(buffer1),0)==0 || x==20){
                    break;
                }
                printf("\t%s\n", buffer1);
                x--;
            }
            printf("------Fin de Contactos------\n\n");

        }else if(opcion==3){
            send(sock,"3",sizeof("3"),0);
            chat(sock);
        }
    }
}

int main(){//int argc, char const *argv[]){
    int sock,sock1,bytesrecibidos;
    struct sockaddr_in cliente;
    struct sockaddr_in servidor;

    /*sock1 = socket(AF_INET, SOCK_STREAM, 0);  
    
    if(sock1 ==-1)
        perror("No se creo el socket de escucha");
    //char puerto[32];
    //printf("Puerto por el que desea conectarse: ");
    //scanf("%s",puerto);

    servidor.sin_family = AF_INET;         
    servidor.sin_port = htons(8080);//atoi(puerto));
    servidor.sin_addr.s_addr = INADDR_ANY;
    bzero(&(servidor.sin_zero),8);
    printf("El puerto digitado es: %d\n",servidor.sin_port);

    if (bind(sock1,(struct sockaddr *)&servidor,sizeof(servidor))){
        perror("Fallo el bind");
        close(sock1);
        exit(1);
    }

    listen(sock1,5);

///////////////////////////////////////*///////////////////////////////////////////

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("No se creó el socket\n");
        exit(1);
    }
    cliente.sin_family = AF_INET;         
    cliente.sin_port = htons(7000);
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