#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/un.h> 
#include <string> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <stdlib.h>
#include "mensaje.pb.h"

int main(int argc, char *argv[]){
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    string username;
    string ip;
    string port;
    if(argc==3){
        username = argv[0];
        ip = argv[1];
        port = argv[2];
    }else{
        printf("\n No ingreso todos los datos, debe ingresar (username,ip,port)");
    }

    void sendInfo(string user, string ip, string port){
        MyInfoSynchronize * clientInfo(new MyInfoSynchronize);
        clientInfo->set_username(user);
        clientInfo->set_ip(ip);

        ClientMessage clientMessage;
        clientMessage.set_option(1);
        clientMessage.set_allocated_synchronize(clientInfo);
    }
    // Se crean buffers para comunicar el cliente y servidor, uno es para mensajes del cliente que envia
	//y el otro es para los mensajes que vienen del servidor
    char buffer1[256], buffer2[256]; 
	//Se crean las estructuras para la direccion ip y puerto del cliente, y el servidor
    struct sockaddr_in my_addr, my_addr1; 
	//Se crea el socket del cliente
    int client = socket(AF_INET, SOCK_STREAM, 0); 
    if (client < 0) 
    printf("Error in client creating\n"); 
    else
        printf("Client Created\n"); 
          
    my_addr.sin_family = AF_INET; 
    my_addr.sin_addr.s_addr = INADDR_ANY; 
    my_addr.sin_port = htons(8080); 
      
    my_addr.sin_addr.s_addr = inet_addr("192.168.100.4"); 
  
    my_addr1.sin_family = AF_INET; 
    my_addr1.sin_addr.s_addr = INADDR_ANY; 
    my_addr1.sin_port = htons(8081); 
      
    my_addr1.sin_addr.s_addr = inet_addr("192.168.100.5"); 
    if (bind(client, (struct sockaddr*) &my_addr1, sizeof(struct sockaddr_in)) == 0) 
        printf("Binded Correctly\n"); 
    else
        printf("Unable to bind\n"); 
      
    socklen_t addr_size = sizeof my_addr; 
    int con = connect(client, (struct sockaddr*) &my_addr, sizeof my_addr); 
    if (con == 0) 
        printf("Client Connected\n"); 
    else
        printf("Error in Connection\n"); 
    char chr[257];
    while(strcmp(buffer1,"Exit")!=0){
	recv(client, buffer1, 256, 0); 
	printf("Server : %s\n", buffer1);
	scanf("%s",&chr);
	strcpy(buffer2, chr); 
    	send(client, buffer2, 256, 0);
	recv(client, buffer1, 256, 0); 
	printf("Server : %s\n", buffer1);
	scanf("%c",&chr);
    }
    return 0; 
} 
    
    my_addr.sin_family = AF_INET; 
	//Esto se pone por si no existe el ip al que se trata de conectar
    my_addr.sin_addr.s_addr = INADDR_ANY; 
	//Se define el puerto al que se quiere conectar
    my_addr.sin_port = htons(8080); 
      
    // Este es el IP del servidor
    my_addr.sin_addr.s_addr = inet_addr("192.168.100.4"); 
  
    // Se define la direccion del cliente que quiere conectarse
    my_addr1.sin_family = AF_INET; 
	//Esto se pone por si no existe el ip al que se trata de conectar
    my_addr1.sin_addr.s_addr = INADDR_ANY; 
	//Se define el puerto al que se quiere usar
    my_addr1.sin_port = htons(8081); 
      
    // Este es el ip de la maquina del cliente
    my_addr1.sin_addr.s_addr = inet_addr(ip); 
}
