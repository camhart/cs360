/*
 * Server.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: cam
 */
//#include "Q.h"
#include "ServerObj.h"
#include "Server.h"
#include <string.h>
#include <iostream>

Server::Server(int portParam, bool debugParam) {
	port = portParam;
	serverSocket = -1;//remove
	data = new Data(); //new?

	data->setDebug(debugParam);
	threadCount = 10;

	//8 = 1 m 21.98s
	//10 =
	//12 = 1 m 20.64s
	//16 = 1 m 22.04s
	que = new Q(threadCount); //new?
	threadPool = vector<pthread_t*>();

	create();
	createThreadPool();
	run();

}

Server::~Server() {
	cout << "Server destroyed...\n";
	free(data);
	free(que);
}

void Server::create() {
	struct sockaddr_in serverAddress;

	//socket address structure creation
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//socket creation
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(!serverSocket) {
		perror("Failure to create server socket.");
		exit(-1);
	}

	//reuse port when app closes
	int reuse = 1;
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		perror("Failure to setsockopt");
		exit(-1);
	}

	//bind our address and port
	if(bind(serverSocket, (const struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("Failure to bind");
		exit(-1);
	}

	//tell socket to listen
	if(listen(serverSocket, SOMAXCONN) < 0) {
		perror("Error to listen");
		exit(-1);
	}
}

void Server::run() {

	//setup client
	int client;
	struct sockaddr_in clientAddress;
	socklen_t clientLength = sizeof(clientAddress);

	//accept clients
	while((client = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength)) > 0) {

		que->push(client);

	}
	close(serverSocket);
}

void* handle(void* ptr);

void Server::createThreadPool() {
	for(int c = 0; c < threadCount; c++) {
		ServerObj* obj = new ServerObj(c, data, que);
		pthread_t* thread = new pthread_t;
		pthread_create(thread, NULL, &handle, (void *) obj);
		threadPool.push_back(thread);
	}
	if(data->debug) {
		cout << "thread pool created" << endl;
	}
}

void* handle(void* ptr) {
	ServerObj* obj = (ServerObj*) ptr;
	while(1) {

		obj->getClient();//pops client off of Q

		int nread = 0;
		while(1) {
			memset(obj->buffer, 0, obj->bufferLength);
//			if(obj->data->debug)
//				cout << "recieving message (" << to_string(obj->client) << ")\n";
			nread = recv(obj->client, obj->buffer, obj->bufferLength, 0);
			if(nread == 0) {
				break;
			}
			//obj->handle(nread);
			obj->handle();
		}
		close(obj->client);//1015
		obj->reset();
		if(obj->data->debug)
			cout << "closed!" << endl;
	}
	//delete obj;
	//dequeue socket...
}
