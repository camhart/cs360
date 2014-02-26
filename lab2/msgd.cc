/*
 * Run.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: cam
 */

#include "Server.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char **argv) {
	bool debug = false;
	int option, port;
	while((option = getopt(argc, argv, "p:d")) != -1) {
		switch(option) {
			case 'p':
				port = atoi(optarg);
				break;
			case 'd':
				debug = true;
				//cout << "debug" << endl;
				break;
			default:
				//cout << "server [-p port] [-d debug]" << endl;
				exit(-1);
		}
	}

	Server myServer = Server(port, debug);

	return 0;
}
