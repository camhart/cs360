/*
 * Client.h
 *
 *  Created on: Sep 16, 2013
 *      Author: cam
 */

#include "Str.h"
#include <string>
using namespace std;

class Client {
public:
	Client(string, int, bool);
	~Client();

private:
	bool sendRequest(string);
	bool getResponse();
	void create();
	void getInput();
	bool validSend(string);
	bool validRead(string);
	bool validList(string);
	bool validQuit(string);
	bool incompleteSend(string);
	string host_;
	int port_;
	int server_;
	int bufferLength;
	char* buffer;
	bool debug;
	Str str;


};
