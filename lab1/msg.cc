/*
 * msg.cpp
 *
 *  Created on: Sep 16, 2013
 *      Author: cam
 */
#include <iostream>
#include <unistd.h>
#include "Client.h"

using namespace std;

int main(int argc, char** argv) {
	int option;
	int port_ = 1234;
	string host_ = "127.0.0.0";
	bool debug = false;

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"h:p:d")) != -1) {
        switch (option) {
            case 'p':
                port_ = atoi(optarg);
                break;
            case 'h':
                host_ = optarg;
                break;
            case 'd':
            	debug = true;
            	break;
            default:
                cout << "client [-h host] [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    if(debug)
	cout << "host: " << host_ << " port: " << port_ << endl;
    Client client_ = Client(host_, port_, debug);
}

