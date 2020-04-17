#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h> 
#include<pthread.h>

void * serverThread(void *arg){
	int acc = *((int *)arg);
	char buffer1[256], buffer2[256];
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
	//close(acc);
	pthread_exit(NULL);
}

int main() 
{ 
    char buffer1[256], buffer2[256]; 
    int server = socket(AF_INET, SOCK_STREAM, 0); 
    if (server < 0) 
        printf("Error in server creating\n"); 
    else
        printf("Server Created\n"); 
          
    struct sockaddr_in my_addr, peer_addr; 
    my_addr.sin_family = AF_INET; 
    my_addr.sin_addr.s_addr = INADDR_ANY; 
      
    my_addr.sin_addr.s_addr = inet_addr("192.168.100.4"); 
      
    my_addr.sin_port = htons(8080); 
  
    if (bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr)) == 0) 
        printf("Binded Correctly\n"); 
    else
        printf("Unable to bind\n"); 
          
    if (listen(server, 3) == 0) 
        printf("Listening ...\n"); 
    else
        printf("Unable to listen\n"); 
      
    socklen_t addr_size; 
    addr_size = sizeof(struct sockaddr_in); 
      
    char *ip; 
    pthread_t tid[2];
    int i = 0;
    while (1) 
    { 
	strcpy(buffer2, "0");
        int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size); 
        printf("Connection Established\n"); 
        char ip[INET_ADDRSTRLEN]; 
        inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); 
      
        printf("connection established with IP : %s and PORT : %d\n",  
                                            ip, ntohs(peer_addr.sin_port)); 
	if (pthread_create(&tid[i], NULL, serverThread, &acc) != 0)
		printf("Fallo\n");
	
        
    }  
    return 0; 
}  
