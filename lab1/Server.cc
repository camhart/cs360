/*
 * Server.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: cam
 */

#include "Server.h"
#include <string.h>
#include <iostream>


Server::Server(int port, bool debug) {
	this->port = port;
	bufferLength = 1024;
	//allocate buffer
	buffer = new char[bufferLength + 1]; //why plus 1?
	remainingBytes = 0;
	serverSocket = -1;//remove
	data = Data();
	curMsg = "";
	str = Str();
	this->debug = debug;
	this->missingChars = 0;
	this->wasMissing = false;

	create();
	run();
}

Server::~Server() {
	delete buffer;
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

		//handle all requests from client...
		while(1) {
			memset(buffer, 0, bufferLength);
			nread = recv(client, buffer, bufferLength, 0);
			if(nread == 0)
				break;
			handle(client);
		}
		close(client);
		if(debug)
			cout << "closed!" << endl;
	}


	close(serverSocket);
}

void Server::handle(int client) {
	if(missingChars > 0) {
		string temp = string(buffer);
		int amt = missingChars > temp.size() ? temp.size() : missingChars;
		curMsg.append(temp.substr(0, amt));
		missingChars-=amt;
		if(missingChars == 0) {
			string putStr = str.chopOffFront(curMsg, ' ');
			string nameStr = str.chopOffFront(curMsg, ' ');
			string subjectStr = str.chopOffFront(curMsg, ' ');
			string lengthStr = str.chopOffFront(curMsg, '\n');
			data.add(nameStr, subjectStr, curMsg);
			curMsg = "";
			if(debug)
				cout << putStr << " " << nameStr << " " << subjectStr << " " << lengthStr << endl;
			string retmsg = "OK\n";
			send(client, retmsg.c_str(), retmsg.size(), 0);
			return;
		}
	} else {
		curMsg.append(buffer);
		if(debug)
			cout << "curBuffer \"" << curMsg << "\"" << endl;
	}

	if(curMsg == "reset\n") {
		data.reset();
		string retmsg = "OK\n";
		send(client, retmsg.c_str(), retmsg.size(), 0);
		curMsg = "";
	} else if(validPut(curMsg)) {
		string putStr = str.chopOffFront(curMsg, ' ');
		string nameStr = str.chopOffFront(curMsg, ' ');
		string subjectStr = str.chopOffFront(curMsg, ' ');
		string lengthStr = str.chopOffFront(curMsg, '\n');
		if(curMsg.length() < atoi(lengthStr.c_str())) {
			missingChars = atoi(lengthStr.c_str()) - curMsg.length();
			curMsg = putStr + " " + nameStr + " " + subjectStr + " " + lengthStr + "\n" + curMsg;
			//send(client, to_string(missingChars).c_str(), to_string(missingChars).size(), 0);
			send(client,"", 0, 0);
			return;
		}
		data.add(nameStr, subjectStr, curMsg);
		curMsg = "";
		if(debug)
			cout << putStr << " " << nameStr << " " << subjectStr << " " << lengthStr << endl;
		string retmsg = "OK\n";
		send(client, retmsg.c_str(), retmsg.size(), 0);

	} else if(validGet(curMsg)) {
		string getStr = str.chopOffFront(curMsg, ' ');
		string nameStr = str.chopOffFront(curMsg, ' ');
		string indexStr = str.chopOffFront(curMsg, '\n');

		string retStr = data.getMessage(nameStr, atoi(indexStr.c_str()));
		if(retStr.empty()) {
			string emsg = "error invalid username or message index (";
			emsg.append(indexStr);
			emsg.append(")\n");
			send(client, emsg.c_str(), emsg.length(), 0);
			curMsg = "";
		} else {
			send(client, retStr.c_str(), retStr.length(), 0);
			curMsg = "";
		}

		curMsg = "";
	} else if(validList(curMsg)) {

		string listStr = str.chopOffFront(curMsg, ' ');
		string nameStr = str.chopOffFront(curMsg, '\n');

		string num = data.getNumberMessages(nameStr);
		if(num == "-1") {
			string emsg = "error invalid username \"";
			emsg+=nameStr;
			emsg+="\"\n";
			send(client, emsg.c_str(), emsg.length(), 0);
			curMsg = "";
			return;
		}

		if(debug)
			cout << listStr << " " << nameStr << endl;
		string sendMsg = "list ";
		sendMsg+=num;
		sendMsg+="\n";
		sendMsg+=data.getMessages(nameStr);
		send(client, sendMsg.c_str(), sendMsg.length(), 0);
		curMsg = "";
	} else {

//		if(!str.stillValid(curMsg)) {
			string msg = string("error invalid cmd\n");
			send(client, msg.c_str(), msg.length(), 0);
			curMsg = "";
//		} else {
//			send(client, curMsg.c_str(), curMsg.length(), 0);
//		}
	}
}

string Server::processRequest(int client) {
	return "";
}

bool Server::validList(string msg) {
	string listStr = str.chopOffFront(msg, ' ');
	string nameStr = str.chopOffFront(msg, '\n');
	if(listStr != "list") {
		return false;
	}
	else if(nameStr == "") {
		return false;
	}
	return true;
}

bool Server::validPut(string msg) {
	string putStr = str.chopOffFront(msg, ' ');
	string nameStr = str.chopOffFront(msg, ' ');
	string subjectStr = str.chopOffFront(msg, ' ');
	string lengthStr = str.chopOffFront(msg, '\n');
	if(putStr != "put") {
		return false;
	}
	else if(!str.isNumeric(lengthStr)) {
		return false;
	} else if(nameStr == "" || subjectStr == "") {
		return false;
	} else
		return true;
}

bool Server::validGet(string msg) {
	string getStr = str.chopOffFront(msg, ' ');
	string nameStr = str.chopOffFront(msg, ' ');
	string indexStr = str.chopOffFront(msg, '\n');
	if(getStr != "get") {
		return false;
	}
	else if(!str.isNumeric(indexStr)) {
		//string msg = string("error invalid index (must be numeric) - \"" + indexStr + "\"\n");
		return false;
	} else if(nameStr == "") {
		//string msg = string("error invalid name/subject\n");
		return false;
	} else {
		return true;
	}
}
