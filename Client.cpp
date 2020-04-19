#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/un.h> 
#include <string> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <chrono>
#include "mensaje.pb.h"
using namespace std;
using namespace chat;

void changeStatus(int client){
	
	char buffer1[1024], buffer2[1024];	
	//string status;
	//MANDANDO EL MENU DE STATUS
	recv(client, buffer1, 1024,0);
	printf("Server : %s\n", buffer1);
	//PREGUNTA QUE OPCION
	//scanf("%s",status);
	ChangeStatusRequest * statusChange(new ChangeStatusRequest);
	//ChangeStatusRequest statusChange;
	statusChange->set_status("69");
	ClientMessage clientMessage;
    clientMessage.set_option(5);
	clientMessage.set_userid(12);
    clientMessage.set_allocated_changestatus(statusChange);	

	string newStatus;
	//SE LO MANDAMOS AL SERVER	
	clientMessage.SerializeToString(&newStatus);
	strcpy(buffer2, newStatus.c_str());
	send(client,buffer2, 1024,0);
	printf("Status enviado");
	//printf("Se esta cambiando de status a %s",status);
	
}

void sendInfo(char* user, string ip, int client){

	//MANDAMOS COSAS EN EL BUFFER2 RECIBIMOS EN EL 1
	char buffer1[1024], buffer2[1024];

    MyInfoSynchronize * clientInfo(new MyInfoSynchronize);
    clientInfo->set_username(user);
    clientInfo->set_ip(ip);

    ClientMessage clientMessage;
    clientMessage.set_option(1);
    clientMessage.set_allocated_synchronize(clientInfo);

    string msg;
    clientMessage.SerializeToString(&msg);
	//cout<< clientMessage.option()<<endl;
	//cout<< clientMessage.synchronize().username()<<endl;
	//cout<< clientMessage.synchronize().ip()<<endl;

	printf("MANDANDO PASO 1 DEL 3 WAY \n");
	strcpy(buffer2, msg.c_str());
    send(client, buffer2, msg.size()+1,0);
	
	
	//recv(client,buffer1,1024,0);
	//printf("EL SERVER NOS MANDO %s\n",buffer1);

	recv(client, buffer1, 1024,0);
	MyInfoResponse infoResponse;
	string infoRes;
	infoResponse.ParseFromString(buffer1);
	printf("RECIBIENDO EL PASO 2 DEL 3 WAY\n");
	cout << infoResponse.userid() << endl;

	printf("MANDANDO EL PASO 3 DEL 3 WAY\n");
	MyInfoAcknowledge infoAcknowlege;
	infoAcknowlege.set_userid(infoResponse.userid());
	string infoAck;
	infoAcknowlege.SerializeToString(&infoAck);
	strcpy(buffer2, infoAck.c_str());
	send(client,buffer2, 1024,0);
	
}

int main(int argc, char *argv[]){
	GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    char* username;
    char* ip;
    char* port;
    char buffer1[256], buffer2[256]; 
    struct sockaddr_in my_addr, my_addr1; 
    int client = socket(AF_INET, SOCK_STREAM, 0); 

    if(argc>=3){
        username = argv[1];
        ip = argv[2];
        port = argv[3];
    }else{
        printf("\n No ingreso todos los datos, debe ingresar (username,ip,port)");
    }
    //printf("Username es %s\n",username);
    //printf("Ip es %s\n",ip);
    //printf("Port es %s\n",port);
    //============================================== CONEXION CON EL SERVIDOR ===============================================
    // Se crean buffers para comunicar el cliente y servidor, uno es para mensajes del cliente que envia
	//y el otro es para los mensajes que vienen del servidor
 
    int puerto = atoi(port);
	//printf("Puerto es %d\n",puerto);
	//sendInfo(username, ip, client);


    if (client < 0) 
    printf("Error in client creating\n"); 
    else
        printf("Client Created\n"); 

    my_addr.sin_family = AF_INET; 
    my_addr.sin_addr.s_addr = INADDR_ANY; 
    my_addr.sin_port = htons(puerto); 

    my_addr.sin_addr.s_addr = inet_addr(ip); 

    my_addr1.sin_family = AF_INET; 
    my_addr1.sin_addr.s_addr = INADDR_ANY; 
    my_addr1.sin_port = htons(puerto+1); 

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
    //====================================================================================================================
    char chr[257];
	sendInfo(username, ip, client);

    while(strcmp(buffer1,"Exit")!=0){
		printf("ENTRA ACA?\n");
		recv(client, buffer1, 256, 0); 
		printf("Server : %s\n", buffer1);
		scanf("%s",&chr);
		strcpy(buffer2, chr); 
			send(client, buffer2, 256, 0);
		recv(client, buffer1, 256, 0); 
		//printf("Server : %s\n", buffer1);
		if(strcmp(buffer1,"1")==0){
			changeStatus(client);		
		}		
		scanf("%c",&chr);
    }
	google::protobuf::ShutdownProtobufLibrary();
    return 0; 
}
