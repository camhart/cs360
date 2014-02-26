/*
 * Server.h
 *
 *  Created on: Sep 11, 2013
 *      Author: cam
 */


#include <sys/types.h> //not necessary if on linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "Data.h"
#include "Str.h"
using namespace std;

class Server{
public:
	Server(int, bool);
	~Server();


private:
	int remainingBytes;
	int port;
	int serverSocket;
	int bufferLength;
	int nread;
	char* buffer;
	int missingChars;
	Data data;
	string curMsg;
	Str str;
	bool debug;
	bool wasMissing;

	void create();
	void run();
	void handle(int);
	string processRequest(int);
	bool validPut(string);
	bool validGet(string);
	bool validList(string);
};


