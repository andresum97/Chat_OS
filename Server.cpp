#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <chrono>
#include "mensaje.pb.h"
#include<pthread.h>
using namespace std;
using namespace chat;


void changeStatus(void *arg){
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];

	strcpy(buffer1, "\n1. Activo 1 \n2. Inactivo 2\n3. Ocupado\n"); 
	send(acc, buffer1, 256, 0);
	printf("Se envio las opciones\n");

	recv(acc, buffer2, 1024,0);
	ClientMessage responseStatus;
	//responseStatus.set_userid(acc);
	//responseStatus.set_status(buffer2);
	responseStatus.ParseFromString(buffer2);

	printf("EL STATUS QUE MANDO ES\n");
	cout << responseStatus.changestatus().status() << endl;
		
}

void * serverThread(void *arg){

	//MANDAMOS COSAS EN EL BUFFER1 RECIBIMOS EN EL 2
	int acc = *((int *)arg);
	int id = acc;
	char buffer1[1024], buffer2[1024];

	//strcpy(buffer1, "\n1. Opcion 1 \n2. Opcion 2\n3. Exit\n"); 
	//send(acc, buffer1, 256, 0);
	//printf("Se envio el Menu\n");

	recv(acc, buffer2, 256, 0);
	ClientMessage client;
	string msg;
	client.ParseFromString(buffer2);
	printf("RECIBIENDO EL PASO 1 DEL 3 WAY\n");	
	printf("-----------------------------------------------\n");
	cout<< client.option()<<endl;
	cout<< client.synchronize().username()<<endl;
	cout<< client.synchronize().ip()<<endl;
	printf("-----------------------------------------------\n");
	
	printf("MANDANDO EL PASO 2 DEL 3 WAY\n");
	MyInfoResponse * responseInfo(new MyInfoResponse);
	responseInfo-> set_userid(acc);
	
	ServerMessage serverMessage;
	serverMessage.set_option(4);
	serverMessage.set_allocated_myinforesponse(responseInfo);
	
	string infoRes;
	serverMessage.SerializeToString(&infoRes);
	strcpy(buffer1, infoRes.c_str());
	send(acc,buffer1, 1024,0);

	printf("-----------------------------------------------\n");
	cout<< serverMessage.option()<<endl;
	cout<< serverMessage.myinforesponse().userid()<<endl;
	printf("-----------------------------------------------\n");


	//MyInfoResponse infoResponse;
	//infoResponse.set_userid(acc);
	//string infoRes;
	//infoResponse.SerializeToString(&infoRes);
	//strcpy(buffer1, infoRes.c_str());
	//send(acc,buffer1, 1024,0);
	recv(acc, buffer2, 256, 0);
	ClientMessage client2;
	string msg2;
	client2.ParseFromString(buffer2);
	printf("RECIBIENDO EL PASO 3 DEL 3 WAY\n");	
	printf("-----------------------------------------------\n");
	cout << client2.option() << endl;
	cout << client2.acknowledge().userid() << endl;
	printf("-----------------------------------------------\n");
	//recv(acc, buffer2, 1024,0);
	//MyInfoAcknowledge infoAcknowlege;
	//infoAcknowlege.ParseFromString(buffer2);
	//printf("RECIBIENDO EL PASO 3 DEL 3 WAY\n");
	//cout << infoAcknowlege.userid() << endl;

	
	while(strcmp(buffer2,"3")!=0){
		strcpy(buffer1, "\n1. Cambiar Status \n2. Opcion 2\n3. Exit\n"); 
		send(acc, buffer1, 256, 0);
		printf("Se envio el Menu\n");
		
		recv(acc, buffer2, 1024,0);
		printf("Client: %s\n",buffer2);
		if(strcmp(buffer2,"1")==0){
			printf("Eligio 1\n ");
			strcpy(buffer1, "1"); 
        	send(acc, buffer1, 256, 0);
			changeStatus(&acc);
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
		}else{
			printf("%s",buffer2);
			printf("El acc es %d",acc);
			strcpy(buffer1, "Opcion no valida"); 
        		send(acc, buffer1, 256, 0);
		}
	}
	//close(acc);
	pthread_exit(NULL);
}


int main() 
{ 
    GOOGLE_PROTOBUF_VERIFY_VERSION;
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
	google::protobuf::ShutdownProtobufLibrary();
    return 0; 
} 
