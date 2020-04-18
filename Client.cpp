#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<stdlib.h> 
  
int main() 
{ 
    char buffer1[256], buffer2[256]; 
    struct sockaddr_in my_addr, my_addr1; 
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
