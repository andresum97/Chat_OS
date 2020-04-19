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
#include <pthread.h>
#include <unistd.h>
using namespace std;
using namespace chat;

struct User{
	int userid;
	string username;
	char ip_addr[INET_ADDRSTRLEN];
	string status;
};

struct User users[10];
int contUser = 0;

void connectedUsers(void *arg,struct User thisUser){
	
	cout << "hola" << endl;
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];

	/*strcpy(buffer1, "Presione enter para recibir lista\n"); 
	send(acc, buffer1, 256, 0);
	printf("Se envio las opciones\n");
	*/
	
	recv(acc, buffer2, 1024,0);
	cout << buffer2 << "holaaaaaaaaaaaaaaaaa" << endl;
	ClientMessage connectedUsers;
	connectedUsers.ParseFromString(buffer2);
	printf("EL USUARIO QUE PIDIO LA LISTA ES\n");
	cout << connectedUsers.connectedusers().username() << endl;

	//printf("MANDANDO EL RESPONSE\n");
	//ConnectedUserResponse * connectedResponse (new ConnectedUserResponse);
	//connectedResponse-> set_userid(acc);
	//responseStatus2-> set_status(thisUser.status);
	
}

void changeStatus(void *arg,struct User thisUser){
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];

	strcpy(buffer1, "\n1. Activo 1 \n2. Inactivo 2\n3. Ocupado\n"); 
	send(acc, buffer1, 256, 0);
	printf("Se envio las opciones\n");

	recv(acc, buffer2, 1024,0);
	ClientMessage responseStatus;
	responseStatus.ParseFromString(buffer2);

	printf("EL STATUS QUE MANDO ES\n");
	cout << responseStatus.changestatus().status() << endl;
	thisUser.status = responseStatus.changestatus().status();
	cout << "El nuevo estatus " << thisUser.status << endl;
	printf("\n");

	printf("MANDANDO EL RESPONSE\n");
	ChangeStatusResponse * responseStatus2 (new ChangeStatusResponse);
	responseStatus2-> set_userid(acc);
	responseStatus2-> set_status(thisUser.status);
	
	ServerMessage serverMessage2;
	serverMessage2.set_option(6);
	serverMessage2.set_allocated_changestatusresponse(responseStatus2);
	
	string changeStat;
	serverMessage2.SerializeToString(&changeStat);
	strcpy(buffer1, changeStat.c_str());
	send(acc,buffer1, 1024,0);

	printf("-----------------------------------------------\n");
	cout<< serverMessage2.option()<<endl;
	cout<< serverMessage2.changestatusresponse().userid()<<endl;
	cout<< serverMessage2.changestatusresponse().status()<<endl;
	printf("-----------------------------------------------\n");


	
		
}

void * serverThread(void *arg){

	//MANDAMOS COSAS EN EL BUFFER1 RECIBIMOS EN EL 2
	int acc = *((int *)arg);
	int id = acc;
	char buffer1[1024], buffer2[1024];
	struct User thisUser;

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
	if(client2.acknowledge().userid() != acc){
		strcpy(users[contUser].ip_addr,"");
		users[contUser].userid = 0;
		users[contUser].username = "";
		users[contUser].status = "";
		cout << "NO SE REALIZO EL THREE-WAY HANDSHAKE" << endl;
		strcpy(buffer2,"7");
	}else{
		printf("REALIZANDO STRUCT DE CLIENTE\n");
		thisUser.username =  client.synchronize().username();
		thisUser.userid =  serverMessage.myinforesponse().userid();
		strcpy(thisUser.ip_addr,client.synchronize().ip().c_str());
		thisUser.status = "1"; //Activo
	}
	
	//recv(acc, buffer2, 1024,0);
	//MyInfoAcknowledge infoAcknowlege;
	//infoAcknowlege.ParseFromString(buffer2);
	//printf("RECIBIENDO EL PASO 3 DEL 3 WAY\n");
	//cout << infoAcknowlege.userid() << endl;

	
	while(strcmp(buffer2,"7")!=0){
		strcpy(buffer1, "\n1. Cambiar Status \n2. Chatear con Todos \n3. Mensaje Directo\n4. Lista de Usuarios Conectados\n5. Informacion de Usuario\n6. Ayuda\n7. Exit\n"); 
		send(acc, buffer1, 256, 0);
		printf("Se envio el Menu\n");
		
		recv(acc, buffer2, 1024,0);
		printf("Client: %s\n",buffer2);
		if(strcmp(buffer2,"1")==0){
			printf("Eligio 1\n ");
			strcpy(buffer1, "1"); 
        	send(acc, buffer1, 256, 0);
			changeStatus(&acc,thisUser);
		} else 
		if(strcmp(buffer2,"2")==0){
			printf("Eligio 2\n ");
			strcpy(buffer1, "Elegiste 2"); 
        		send(acc, buffer1, 256, 0);
		} else 
		if(strcmp(buffer2,"3")==0){
			printf("Eligio 3\n ");
			strcpy(buffer1, "Elegiste 3"); 
        		send(acc, buffer1, 256, 0); 
		}else 
		if(strcmp(buffer2,"4")==0){
			printf("Eligio 4\n ");
			strcpy(buffer1, "Elegiste 4"); 
        		send(acc, buffer1, 256, 0); 
				connectedUsers(&acc,thisUser);
		}else 
		if(strcmp(buffer2,"5")==0){
			printf("Eligio 5\n ");
			strcpy(buffer1, "Elegiste 5"); 
        		send(acc, buffer1, 256, 0); 
		}else 
		if(strcmp(buffer2,"6")==0){
			printf("Eligio 6\n ");
			strcpy(buffer1, "Elegiste 6"); 
        		send(acc, buffer1, 256, 0); 
		}else 
		if(strcmp(buffer2,"7")==0){
			printf("Eligio 7\n ");
			strcpy(buffer1, "Exit"); 
        		send(acc, buffer1, 256, 0); 
		}else{
			printf("%s",buffer2);
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
    char buffer1[256], buffer2[256], bufferU[256]; 
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
	bool activo = true;
    while (activo) 
    { 
		strcpy(buffer2, "0");
        int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size); 
        printf("Connection Established\n"); 
        char ip[INET_ADDRSTRLEN]; 
        inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); 
      	
        printf("connection established with IP : %s and PORT : %d\n",  
                                            ip, ntohs(peer_addr.sin_port)); 
		recv(acc, bufferU, 256,0); //SE RECIBE USUARIO
		users[contUser].username = bufferU;
		cout << "el bufferU" << bufferU << endl;
		strcpy(users[contUser].ip_addr,ip);
		users[contUser].userid = acc;
		users[contUser].status = "1";
		contUser++;
		strcpy(buffer2, "0");
		if (pthread_create(&tid[i], NULL, serverThread, &acc) != 0)
			printf("Fallo\n");
	
        
    }  
	google::protobuf::ShutdownProtobufLibrary();
    return 0; 
} 
