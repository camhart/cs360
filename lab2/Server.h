/*
 * Server.h
 *
 *  Created on: Sep 11, 2013
 *      Author: cam
 */

#include "Q.h" //included in serverobj
#include "ServerObj.h"
#include "Data.h"
#include "Str.h"
#include <sys/types.h> //not necessary if on linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>

using namespace std;

class Server{
public:
	Server(int, bool);
	~Server();


private:
	int port;
	int serverSocket;
	vector<pthread_t*> threadPool;
	int threadCount;

	Data* data;
	Q* que;


	void create();
	void run();
	void createThreadPool();
};


