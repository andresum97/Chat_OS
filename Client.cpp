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

void * listenThread(void *arg){

	//MANDAMOS COSAS EN EL BUFFER1 RECIBIMOS EN EL 2
	int acc = *((int *)arg);
	char buffer1[1024], buffer2[1024];
	ServerMessage serverMessage;
	string changeStat;
	while(1){
		recv(acc, buffer1, 1024,0);

		serverMessage.ParseFromString(buffer1);
		if(serverMessage.option() == 6){
			printf("-----------------------------------------------\n");
			cout<< "La opcion del change es " <<serverMessage.option()<<endl;
			cout<< "El Id del user " << serverMessage.changestatusresponse().userid()<<endl;
			cout<< "El nuevo Status "<< serverMessage.changestatusresponse().status()<<endl;
			printf("-----------------------------------------------\n");
		} else if(serverMessage.option() == 5){
			printf("RECIBIENDO LISTA DE USUARIOS\n");
			printf("-----------------------------------------------\n");
			int max = serverMessage.connecteduserresponse().connectedusers().size();
			cout<<"cuandtos usuarios hay " <<max<<endl;
			for(int i = 0; i<max; i++){
				cout<< "El username del user " << serverMessage.connecteduserresponse().connectedusers(i).username()<< endl;
				cout<< "El estado del user " << serverMessage.connecteduserresponse().connectedusers(i).status()<< endl;
		}
			cout<< "La opcion del change es " <<serverMessage.option()<<endl;
	
			printf("-----------------------------------------------\n");
		} 
	}
}


void connectedUsers(int client, char* username){
	
	char buffer1[1024], buffer2[1024];
	char enter[10];
	//recv(client, buffer1, 1024,0);
	//printf("Server : %s\n", enter);
	//PREGUNTA QUE OPCION
	//scanf("%s",enter);

	connectedUserRequest * connectedUsers(new connectedUserRequest);
	connectedUsers->set_username(username);
	ClientMessage clientMessage;
    	clientMessage.set_option(2);
    	clientMessage.set_allocated_connectedusers(connectedUsers);
	string connectUser;
	//SE LO MANDAMOS AL SERVER	
	clientMessage.SerializeToString(&connectUser);
	strcpy(buffer2, connectUser.c_str());
	send(client,buffer2, 1024,0);
	
	printf("ConnectUsers enviado\n");
	
}

void changeStatus(int client){
	
	char buffer1[1024], buffer2[1024];	
	char status[10];
	//MANDANDO EL MENU DE STATUS
	printf("\n1. Activo 1 \n2. Inactivo 2\n3. Ocupado\n");
	//PREGUNTA QUE OPCION
	scanf("%s",status);
	printf("El status seleccionado es : %s \n",status);
	cout << "El status seleccionado es " << status << endl;
	ChangeStatusRequest * statusChange(new ChangeStatusRequest);
	//ChangeStatusRequest statusChange;
	statusChange->set_status(status);

	ClientMessage clientMessage;
    	clientMessage.set_option(3);
    	clientMessage.set_allocated_changestatus(statusChange);	

	string newStatus;
	//SE LO MANDAMOS AL SERVER	
	clientMessage.SerializeToString(&newStatus);
	//printf("alo\n");
	//cout<< clientMessage<<endl
	//cout<< clientMessage.option()<<endl;
	//cout<< clientMessage.changestatus().status()<<endl;


	strcpy(buffer2, newStatus.c_str());
	send(client,buffer2, 1024,0);
	printf("Status enviado\n");
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
	printf("MANDANDO PASO 1 DEL 3 WAY \n");
	printf("-----------------------------------------------\n");
	cout<< clientMessage.option()<<endl;
	cout<< clientMessage.synchronize().username()<<endl;
	cout<< clientMessage.synchronize().ip()<<endl;
	printf("-----------------------------------------------\n");

	strcpy(buffer2, msg.c_str());
    send(client, buffer2, msg.size()+1,0);
	
	
	//recv(client,buffer1,1024,0);
	//printf("EL SERVER NOS MANDO %s\n",buffer1);

	recv(client, buffer1, 1024,0);
	ServerMessage serverMessage;
	string infoRes;
	serverMessage.ParseFromString(buffer1);	

	//MyInfoResponse infoResponse;
	//string infoRes;
	//infoResponse.ParseFromString(buffer1);
	printf("RECIBIENDO EL PASO 2 DEL 3 WAY\n");
	printf("-----------------------------------------------\n");
	cout << serverMessage.option() << endl;
	cout << serverMessage.myinforesponse().userid() << endl;
	printf("-----------------------------------------------\n");

	printf("MANDANDO EL PASO 3 DEL 3 WAY\n");
	
	MyInfoAcknowledge * infoAck(new MyInfoAcknowledge);
	infoAck->set_userid(serverMessage.myinforesponse().userid());

	ClientMessage clientMessage2;
    clientMessage2.set_option(6);
    clientMessage2.set_allocated_acknowledge(infoAck);

	string msg2;
    clientMessage2.SerializeToString(&msg2);
	printf("MANDANDO PASO 3 DEL 3 WAY \n");
	printf("-----------------------------------------------\n");
	cout<< clientMessage2.option()<<endl;
	cout<< clientMessage2.acknowledge().userid()<<endl;
	printf("-----------------------------------------------\n");
	strcpy(buffer2, msg2.c_str());
    send(client, buffer2, msg2.size()+1,0);
	
}

int main(int argc, char *argv[]){
	GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    char* username;
    char* ip;
    char* port;
    char buffer1[256], buffer2[256], bufferU[256]; 
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
    pthread_t tid;
	strcpy(bufferU,username);
	//cout <<"el buffer2 es "<< buffer2<<endl;
	send(client, bufferU, 256, 0);
	sendInfo(username, ip, client);
	if (pthread_create(&tid, NULL, listenThread, &client) != 0)
			printf("Fallo\n");
	
    while(strcmp(buffer1,"Exit")!=0){
		
		printf("\n1. Cambiar Status \n2. Chatear con Todos \n3. Mensaje Directo\n4. Lista de Usuarios Conectados\n5. Informacion de Usuario\n6. Ayuda\n7. Exit\n");
		scanf("%s",&chr);
		strcpy(buffer1, chr); 
		//send(client, buffer2, 256, 0);
		//recv(client, buffer1, 256, 0); 
		//printf("Server : %s\n", buffer1);
		if(strcmp(buffer1,"1")==0){
			send(client, buffer1, 256, 0);
			changeStatus(client);		
		}else
		if(strcmp(buffer1,"4")==0){
			send(client, buffer1, 256, 0);
			connectedUsers(client,username);		
		} else 
		if(strcmp(buffer1,"7")==0){
			send(client, buffer1, 256, 0);
			strcpy(buffer1, "Exit"); 	
		}
		scanf("%c",&chr);
    }
	google::protobuf::ShutdownProtobufLibrary();
    return 0; 
}
