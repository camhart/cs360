/*
 * Client.cpp
 *
 *  Created on: Sep 16, 2013
 *      Author: cam
 */

#include "Client.h"
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <fstream>

Client::Client(string host, int port, bool debug) {
	this->host_ = host;
	this->port_ = port;
	this->debug = debug;

	this->bufferLength = 1024;
	this->buffer = new char[bufferLength+1];

	this->str = Str();
	create();
	getInput();
	close(server_);
}

Client::~Client() {

}

void Client::create() {
	if (debug)
		cout << "connecting to host: " << this->host_ << " on port "
				<< this->port_ << endl;


    struct sockaddr_in server_addr;

    // use DNS to get IP address
    struct hostent *hostEntry;
    hostEntry = gethostbyname(host_.c_str());
    if (!hostEntry) {
        cout << "No such host name: " << host_ << endl;
        exit(-1);
    }

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        perror("socket");
        exit(-1);
    }

    // connect to server
    if (connect(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        exit(-1);
    }
}

bool Client::sendRequest(string request) {
    // prepare to send request
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

void Client::getInput() {
	string line;
	string curMsg = "";
	cout << "% ";
	while(getline(cin, line)) {
		line+="\n";
		curMsg.append(line);
		if(incompleteSend(curMsg)) {
			continue;
		} else if(validSend(curMsg)) {
			string sendStr = str.chopOffFront(curMsg, ' ');
			string userStr = str.chopOffFront(curMsg, ' ');
			string subjectStr = str.chopOffFront(curMsg, '\n');
			string fullmsg = str.chopOffFront(curMsg, "\n\n");
			string mess = "put ";
			mess.append(userStr);
			mess.append(" ");
			mess.append(subjectStr);
			mess.append(" ");
			mess.append(to_string(fullmsg.length()));
			mess.append("\n");
			mess.append(fullmsg);
			bool success = sendRequest(mess);
			if(!success) {
				curMsg = "";
				continue;
			}
			success = getResponse();
			if(!success) {
				curMsg = "";
				continue;
			}
			curMsg = "";
		} else if(validRead(curMsg)) {
			string readStr = str.chopOffFront(curMsg, ' ');
			string userStr = str.chopOffFront(curMsg, ' ');
			string indexStr = str.chopOffFront(curMsg, '\n');
			bool success = sendRequest("get " + userStr + " " + indexStr + "\n" + curMsg);
			if(!success) {
				curMsg = "";
				continue;
			}
			success = getResponse();
			if(!success) {
				curMsg = "";
				continue;
			}
			curMsg = "";
		} else if(validList(curMsg)) {
			string listStr = str.chopOffFront(curMsg, ' ');
			string userStr = str.chopOffFront(curMsg, '\n');
			string listMsg = "list ";
			listMsg.append(userStr);
			listMsg.append("\n");
			bool success = sendRequest(listMsg);
			if(!success) {
				curMsg = "";
				continue;
			}
			success = getResponse();
			if(!success) {
				curMsg = "";
				continue;
			}
		} else if(validQuit(curMsg)) {
			curMsg = "";
			break;
		} else {
			cout << "error invalid cmd (client)" << endl;
			curMsg = "";
		}
		cout << "% ";
	}
}

bool Client::getResponse() {
    string response = "";
    while (response.find("\n") == string::npos) {
        int nread = recv(server_,buffer,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        response.append(buffer,nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    if(response == "OK\n") {
    	//cout << "OK\n";
    	//remove this
    } else if(response.substr(0, 5) == "list ") {
    	str.chopOffFront(response, '\n');
    	cout << response;

    } else if(response.substr(0, 8) == "message ") {
    	string newResponse = response.substr(8, response.size());
    	string subject = str.chopOffFront(newResponse, ' ');
    	str.chopOffFront(newResponse, '\n');

    	string printMsg = subject;
    	printMsg.append("\n");
    	printMsg.append(newResponse);
    	printMsg.append("\n");
    	cout << printMsg;
    } else if(response.substr(0, 6) == "error ") {
    	cout << response;
    }
    return true;
}

bool Client::incompleteSend(string msg) {
	string sendStr = str.chopOffFront(msg, ' ');
	string userStr = str.chopOffFront(msg, ' ');
	string subjectStr = str.chopOffFront(msg, '\n');
	string fullmsg = str.chopOffFront(msg, "\n\n");
	if(sendStr == "send" && !userStr.empty() && !subjectStr.empty() && fullmsg.empty()) {
		return true;
	}
	return false;
}

bool Client::validSend(string msg) {
	string sendStr = str.chopOffFront(msg, ' ');
	string userStr = str.chopOffFront(msg, ' ');
	string subjectStr = str.chopOffFront(msg, '\n');
	string fullmsg = str.chopOffFront(msg, "\n\n");
	if(sendStr != "send" || userStr.empty() || subjectStr.empty() || fullmsg.empty()) {
		return false;
	}
	return true;
}

bool Client::validList(string msg) {
	string listStr = str.chopOffFront(msg, ' ');
	string userStr = str.chopOffFront(msg, '\n');
	if(listStr != "list" || userStr.empty()) {
		return false;
	}
	return true;
}
bool Client::validRead(string msg) {
	string readStr = str.chopOffFront(msg, ' ');
	string userStr = str.chopOffFront(msg, ' ');
	string indexStr = str.chopOffFront(msg, '\n');

	if(readStr != "read" || userStr.empty() || indexStr.empty() || !str.isNumeric(indexStr)) {
		return false;
	}
	return true;
}

bool Client::validQuit(string msg) {
	string quit = str.chopOffFront(msg, '\n');

	if(quit != "quit") {
		return false;
	}
	return true;
}



