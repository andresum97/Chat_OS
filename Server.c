#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h> 
  
int main() 
{ 
    // Se crean buffers para comunicar el cliente y servidor, uno es para mensajes del servidor que envia
	//y el otro es para los mensajes que vienen del cliente
    char buffer1[256], buffer2[256]; 
	// Se crea el socket
    int server = socket(AF_INET, SOCK_STREAM, 0); 
    if (server < 0) 
        printf("Error in server creating\n"); 
    else
        printf("Server Created\n"); 
    //Se crean las estructuras para la direccion ip y puerto del cliente, y el servidor
    struct sockaddr_in my_addr, peer_addr; 
    my_addr.sin_family = AF_INET; 
	//Esto se pone por si no existe el ip al que se trata de conectar
    my_addr.sin_addr.s_addr = INADDR_ANY; 
      
    // Este es el ip del servidor para que se puedan conectar los clientes
    my_addr.sin_addr.s_addr = inet_addr("192.168.100.4"); 
    //Puerto
    my_addr.sin_port = htons(8080); 
    //Se hace el bind del socket del servidor hacia el puerto e IP del servidor
    if (bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr)) == 0) 
        printf("Binded Correctly\n"); 
    else
        printf("Unable to bind\n"); 
    //El servidor se queda esperando a que alguien se conecte, con un limite de lista de espera de 3
    if (listen(server, 3) == 0) 
        printf("Listening ...\n"); 
    else
        printf("Unable to listen\n"); 
      
    socklen_t addr_size; 
    addr_size = sizeof(struct sockaddr_in); 
      
    // Aqui se guarda el ip del cliente
    char *ip; 
      
    // Se acepta conexiones 1 por 1  para siempre
    while (1) 
    { 
        //Se acepta una coneccion
        int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size); 
        printf("Connection Established\n"); 
        char ip[INET_ADDRSTRLEN]; 
		//Obtiene info del cliente conectado como el ip 
        inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); 
      
        // ntohs sirve para obtener el puerto
        printf("connection established with IP : %s and PORT : %d\n",  
                                            ip, ntohs(peer_addr.sin_port)); 
        while(strcmp(buffer2,"3")!=0){
		strcpy(buffer1, "\n1. Opcion 1 \n2. Opcion 2\n3. Exit\n"); 
        	send(acc, buffer1, 256, 0);
		printf("Se envio el Menu\n");
		recv(acc, buffer2, 256, 0);
		printf("Client: %s\n",buffer2);
		if(strcmp(buffer2,"1")==0){
			printf("Eligio 1\n ");
			strcpy(buffer1, "Elegiste 1"); 
        		send(acc, buffer1, 256, 0);
		} else 
		if(strcmp(buffer2,"2")==0){
			printf("Eligio 2\n ");
			strcpy(buffer1, "Elegiste 2"); 
        		send(acc, buffer1, 256, 0);
		} else 
		if(strcmp(buffer2,"3")==0){
			printf("Eligio 3\n ");
			strcpy(buffer1, "Exit"); 
        		send(acc, buffer1, 256, 0); 
		}
	} 
    }  
    return 0; 
} 
