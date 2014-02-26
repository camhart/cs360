/*
 * ServerObj.cc
 *
 *  Created on: Sep 23, 2013
 *      Author: cam
 */
//nread =
#include "Server.h"

ServerObj::ServerObj(int idParam, Data* dataParam, Q* queueParam) {
	bufferLength = 1024;
	buffer = new char[bufferLength + 1]; //why plus 1?
	//sendBuffer
	que = queueParam;
	data = dataParam;
	missingChars = 0;
	client = -1;
	id = idParam;
	str = Str();
	// = Str()
}

ServerObj::~ServerObj() {
	delete buffer;
}

void ServerObj::reset() {
	this->curMsg = "";
	this->missingChars = 0;
	this->client = -1;

}

void ServerObj::getClient() {
	this->client = que->pop();
}

bool ServerObj::validList(string msg) {
//	string listStr = str.chopOffFront(msg, ' ');
//	string nameStr = str.chopOffFront(msg, '\n');
	listStrG = str.chopOffFront(msg, ' ');
	nameStrG = str.chopOffFront(msg, '\n');
	if (listStrG != "list") {
		return false;
	} else if (nameStrG == "") {
		return false;
	}
	return true;
}

bool ServerObj::validPut(string msg) {
//	string
	putStrG = str.chopOffFront(msg, ' ');
//	string
	nameStrG = str.chopOffFront(msg, ' ');
//	string
	subjectStrG = str.chopOffFront(msg, ' ');
//	string
	lengthStrG = str.chopOffFront(msg, '\n');
	if (putStrG != "put") {
		return false;
	} else if (!str.isNumeric(lengthStrG)) {
		return false;
	} else if (nameStrG == "" || subjectStrG == "") {
		return false;
	} else
		return true;
}

bool ServerObj::validGet(string msg) {
//	string
	getStrG = str.chopOffFront(msg, ' ');
//	string
	nameGStrG = str.chopOffFront(msg, ' ');
//	string
	indexStrG = str.chopOffFront(msg, '\n');
	if (getStrG != "get") {
		return false;
	} else if (!str.isNumeric(indexStrG)) {
		//string msg = string("error invalid index (must be numeric) - \"" + indexStr + "\"\n");
		return false;
	} else if (nameGStrG == "") {
		//string msg = string("error invalid name/subject\n");
		return false;
	} else {
		return true;
	}
}

//void ServerObj::handle(int nread) {
void ServerObj::handle() {
	//string checkMsg = string(curMsg);
	if (missingChars > 0) {
		string temp = string(buffer);
		int amt = missingChars > temp.size() ? temp.size() : missingChars;
		curMsg.append(temp.substr(0, amt));
		missingChars -= amt;
		if (missingChars == 0) {
			string putStr = str.chopOffFront(curMsg, ' ');
			string nameStr = str.chopOffFront(curMsg, ' ');
			string subjectStr = str.chopOffFront(curMsg, ' ');
			string lengthStr = str.chopOffFront(curMsg, '\n');
			data->add(nameStr, subjectStr, curMsg);
			curMsg = "";
			if (data->debug)
				cout << putStr << " " << nameStr << " " << subjectStr << " "
						<< lengthStr << endl;
			string retmsg = "OK\n";
			//send(client, retmsg.c_str(), retmsg.size(), 0);
			sendResponse(retmsg);
			return;
		}
	} else {
		//if(nread > 0)
		//	curMsg.append(buffer, nread);
		curMsg.append(buffer);
	}

	if (curMsg == "reset\n") {

		data->reset();
		string retmsg = "OK\n";
		sendResponse(retmsg);
		curMsg = "";

	} else if (validPut(curMsg)) {
		if (data->debug)
			cout << "valid put message: " << curMsg << endl;
//		string putStr = str.chopOffFront(curMsg, ' ');
//		string nameStr = str.chopOffFront(curMsg, ' ');
//		string subjectStr = str.chopOffFront(curMsg, ' ');
//		string lengthStr = str.chopOffFront(curMsg, '\n');
		int start = putStrG.length() + nameStrG.length() + subjectStrG.length() + lengthStrG.length() + 4;
		curMsg = curMsg.substr(start, curMsg.length() - start);
		if (curMsg.length() < atoi(lengthStrG.c_str())) {
			missingChars = atoi(lengthStrG.c_str()) - curMsg.length();
			string oldMsg = curMsg;
			curMsg = putStrG;
			curMsg.append(" ");
			curMsg.append(nameStrG);
			curMsg.append(" ");
			curMsg.append(subjectStrG);
			curMsg.append(" ");
			curMsg.append(lengthStrG);
			curMsg.append("\n");
			curMsg.append(oldMsg);

			sendResponse("");
			return;
		}
		data->add(nameStrG, subjectStrG, curMsg);

		string retmsg = "OK\n";

		sendResponse(retmsg);
		curMsg = "";

	} else if (validGet(curMsg)) {
//		string getStr = str.chopOffFront(curMsg, ' ');
//		string nameStr = str.chopOffFront(curMsg, ' ');
//		string indexStr = str.chopOffFront(curMsg, '\n');

		string retStr = data->getMessage(nameGStrG, atoi(indexStrG.c_str()));
		if (retStr.empty()) {
			string emsg = "error invalid username or message index (";
			emsg.append(indexStrG);
			emsg.append(")\n");
			//send(client, emsg.c_str(), emsg.length(), 0);
			sendResponse(emsg);
			curMsg = "";
		} else {
			if (data->debug)
				cout << "sent:\"" << retStr << "\"" << endl;

			if (retStr.substr(0, 7) != "message") {
				cout << "problem..." << retStr << endl;
			}

			//send(client, retStr.c_str(), retStr.length(), 0);
			sendResponse(retStr);
			curMsg = "";
		}

		curMsg = "";
	} else if (validList(curMsg)) {

//		string listStr = str.chopOffFront(curMsg, ' ');
//		string nameStr = str.chopOffFront(curMsg, '\n');

		string sendMsg = data->getMessagesResponse(nameStrG);

		sendResponse(sendMsg);
		curMsg = "";
	} else {
		string msg = string("error invalid cmd\n");
		sendResponse(msg);
		curMsg = "";
	}
}

bool ServerObj::sendResponse(string response) {
	// prepare to send response
//	char* ptr = new char[response.length()];
//	strncpy(ptr, response.c_str(), response.length());
	//		response.c_str();
	const char* ptr = response.c_str();
	int nleft = response.length();
	int nwritten;
	// loop to be sure it is all sent
	while (nleft) {
		if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
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

