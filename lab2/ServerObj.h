/*
 * ServerObj.h
 *
 *  Created on: Sep 23, 2013
 *      Author: cam
 */

#ifndef SERVEROBJ_H
#define SERVEROBJ_H

#include <string>
#include "Str.h"
#include "Data.h"
#include "Q.h"

class ServerObj {
public:
	ServerObj(int, Data*, Q*);
	~ServerObj();
	void handle();
	void getClient();
	int bufferLength;
	string curMsg;
	int missingChars;
	Str str;
	char* buffer;
	//char* sendBuffer;
	int client;
	Data* data;
	Q* que;
	void reset();
	int id;
	bool sendResponse(string);
	//int nread;

private:
	bool validPut(string);
	bool validGet(string);
	bool validList(string);
	string listStrG;
	string nameStrG;
	string putStrG;
	string subjectStrG;
	string lengthStrG;
	string getStrG;
	string indexStrG;
	string nameGStrG;

};

#endif
