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

void deleteUser(void *arg){
	int acc = *((int *)arg);
	int i,j;
	for(i = 0; i < contUser; i++){
		if(users[i].userid == acc){
			for(j=i; j<(contUser-1);j++){
				users[j] = users[j+1];
			}
			break;
		}
	}
	contUser -= 1;
}
void directMessage(void *arg,struct User users[10]){
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];
	recv(acc, buffer2, 1024,0);
	int idClient = -1;
	string sender = "";
	ClientMessage clientmessage;
	clientmessage.ParseFromString(buffer2);
	string message = clientmessage.directmessage().message();
	string username = clientmessage.directmessage().username();
	printf("Este es el mensaje del usuario\n");
	cout << clientmessage.directmessage().message()<<endl;
	printf("Y va para \n");
	cout << clientmessage.directmessage().username()<<endl;

	for(int i = 0; i < contUser; i++){
		if(users[i].username == username){
			idClient = users[i].userid;
			break;
		}
	}
	for(int i = 0; i< contUser; i++){
		if(acc == users[i].userid){
			sender = users[i].username;
			break;
		}
	}
	if(idClient == -1){
		cout << "No se encontro el username" << endl;
		ErrorResponse * responseE(new ErrorResponse);
		responseE->set_errormessage("Este username no existe amigo.");
		
		ServerMessage * serverMessageE(new ServerMessage);
		serverMessageE->set_option(3);
		serverMessageE->set_allocated_error(responseE);

		string messageE;
		serverMessageE->SerializeToString(&messageE);
		strcpy(buffer1, messageE.c_str());
		send(acc,buffer1,1024,0);
			
	}else{
		DirectMessage * directMessage(new DirectMessage);
		directMessage -> set_message(clientmessage.directmessage().message());
		directMessage -> set_userid(idClient);
		directMessage -> set_username(sender);
		ServerMessage serverMessage;
		serverMessage.set_option(2);
		serverMessage.set_allocated_message(directMessage);
		
		string mensaje;
		
		serverMessage.SerializeToString(&mensaje);
		strcpy(buffer1, mensaje.c_str());
		send(idClient,buffer1, 1024,0);
	}
}

void broadcastMessage(void *arg,struct User users[10],string m){
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];

	recv(acc, buffer2, 1024,0);
	ClientMessage broadcastRequest;
	broadcastRequest.ParseFromString(buffer2);
	printf("EL BROADCAST QUE MANDO ES\n");
	cout << broadcastRequest.broadcast().message() << endl;
	cout << "El usuario es: "<<users[acc-4].username <<endl;

	BroadcastResponse * response(new BroadcastResponse);
	response->set_messagestatus("Send");
	
	ServerMessage * serverMessage(new ServerMessage);
	serverMessage->set_option(7);
	serverMessage->set_allocated_broadcastresponse(response);

	string message;
	serverMessage->SerializeToString(&message);
	strcpy(buffer1, message.c_str());
	send(acc,buffer1,1024,0);
	string con = users[acc-4].username;
	con = con + ": ";
	con = con + broadcastRequest.broadcast().message();
	BroadcastMessage * broadcastMsg(new BroadcastMessage);
	broadcastMsg->set_message(con);
	broadcastMsg->set_userid(acc);

	ServerMessage *msg(new ServerMessage);
	msg->set_option(1);
	msg->set_allocated_broadcast(broadcastMsg);
	msg->SerializeToString(&message);
	for (int i = 0; i<10; i++){
		strcpy(buffer1, message.c_str());
		send(users[i].userid,buffer1,1024,0);
	}

}


void connectedUsers(void *arg,struct User users[10]){
	
	
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];
	int flag = 0;
	recv(acc, buffer2, 1024,0);
	
	ClientMessage connectedUsers;
	connectedUsers.ParseFromString(buffer2);
	printf("EL USUARIO QUE PIDIO LA LISTA ES\n");
	cout << connectedUsers.connectedusers().username() << endl;
	
	printf("MANDANDO EL RESPONSE\n");

	ConnectedUserResponse* connectedResponse (new ConnectedUserResponse);
	ConnectedUser* usuariosOnline(new ConnectedUser);
	char cstr[connectedUsers.connectedusers().username().size()+1];
	strcpy(cstr,connectedUsers.connectedusers().username().c_str());
	//cout<< sizeof users << " aaaaaaaaaaaaa" << endl;
	//cout<< contUser << " asaaaaaaaaaaaaa" << endl;
	if (strcmp(cstr,users[acc-4].username.c_str())==0){
		for(int i = 0; i< contUser;i++){
			usuariosOnline = connectedResponse->add_connectedusers();
			usuariosOnline -> set_username(users[i].username);
			usuariosOnline -> set_status(users[i].status);
			usuariosOnline -> set_userid(users[i].userid);
			usuariosOnline -> set_ip(users[i].ip_addr);
		}
		cout<<"El username de users[0] "<< users[0].username<<endl;
	}else{
		for(int i = 0; i< contUser;i++){
			if (strcmp(cstr,users[i].username.c_str())==0){
				usuariosOnline = connectedResponse->add_connectedusers();
				usuariosOnline -> set_username(users[i].username);
				usuariosOnline -> set_status(users[i].status);
				usuariosOnline -> set_userid(users[i].userid);
				usuariosOnline -> set_ip(users[i].ip_addr);
				flag = 1;
			}
		}
		if(flag < 1){
				usuariosOnline = connectedResponse->add_connectedusers();
				usuariosOnline -> set_username("No existe");
				usuariosOnline -> set_status("");
				usuariosOnline -> set_userid(-1);
				usuariosOnline -> set_ip("");
		}
	}
	
	ServerMessage serverMessage;
	serverMessage.set_option(5);
	serverMessage.set_allocated_connecteduserresponse(connectedResponse);
	
	cout<<"EL username del connectedResponse es: "<< serverMessage.connecteduserresponse().connectedusers(0).username()<< endl;


	string userCon;
	serverMessage.SerializeToString(&userCon);
	strcpy(buffer1, userCon.c_str());
	send(acc,buffer1, 1024,0);
	
	
}


void changeStatus(void *arg,struct User users[10]){
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];

	recv(acc, buffer2, 1024,0);
	ClientMessage responseStatus;
	responseStatus.ParseFromString(buffer2);

	printf("EL STATUS QUE MANDO ES\n");
	cout << responseStatus.changestatus().status() << endl;
	users[acc-4].status = responseStatus.changestatus().status();
	cout << "El nuevo estatus " << users[acc-4].status << endl;
	printf("\n");

	printf("MANDANDO EL RESPONSE\n");
	ChangeStatusResponse * responseStatus2 (new ChangeStatusResponse);
	responseStatus2-> set_userid(acc);
	responseStatus2-> set_status(users[acc-4].status);
	
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

	int acc = *((int *)arg);
	int id = acc;
	char buffer1[1024], buffer2[1024];
	struct User thisUser;

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
		recv(acc, buffer2, 1024,0);
		printf("Client: %s\n",buffer2);
		if(strcmp(buffer2,"1")==0){
			changeStatus(&acc,users);
		} else 
		if(strcmp(buffer2,"2")==0){
			broadcastMessage(&acc,users,buffer2);
		} else 
		if(strcmp(buffer2,"3")==0){
			directMessage(&acc,users); 
		}else 
		if(strcmp(buffer2,"4")==0){
			connectedUsers(&acc,users);
		}else 
		if(strcmp(buffer2,"5")==0){
			connectedUsers(&acc,users); 
		}else 
		if(strcmp(buffer2,"6")==0){
			printf("Eligio 6\n "); 
         
		}else 
		if(strcmp(buffer2,"7")==0){
			printf("Eligio 7\n ");
			strcpy(buffer1, "El usuario salio");
			deleteUser(&acc);
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
    pthread_t tid[10];
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
		cout << "la cantidad de usuarios es"<<contUser<<endl;
		strcpy(buffer2, "0");
		if (pthread_create(&tid[i], NULL, serverThread, &acc) != 0)
			printf("Fallo\n");
		i = i + 1;
        
    }  
	google::protobuf::ShutdownProtobufLibrary();
    return 0; 
} 
