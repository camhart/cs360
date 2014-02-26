/*
 * MessageData.cpp
 *
 *  Created on: Sep 15, 2013
 *      Author: cam
 */

#include "Data.h"
#include <iostream>

Data::Data(){
	users = vector<User>();
	debug = false;
	sem_init(&lock, 0, 1);
}

Data::~Data() {

}

void Data::setDebug(bool set) {
	sem_wait(&lock);
	debug = set;
	sem_post(&lock);
}

void Data::reset() {
	sem_wait(&lock);
	users = vector<User>();
	sem_post(&lock);
}

void Data::add(string username, string subject, string message) {
	sem_wait(&lock);
	Message msg = Message(subject, message);
	for(int c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			users[c].addMessage(msg);
			sem_post(&lock);
			return;
		}
	}
	User user = User(username);
	user.addMessage(msg);
	users.push_back(user);
	sem_post(&lock);
}

//void Data::addMessage(string username, Message msg) {
//	sem_wait(&lock);
//	for(int c = 0; c < users.size(); c++) {
//		if(users[c].getUsername() == username) {
//			users[c].addMessage(msg);
//			sem_post(&lock);
//			return;
//		}
//	}
//	users.push_back(User(username));
//	users[users.size() - 1].addMessage(msg);
//	sem_post(&lock);
//}

string Data::getMessage(string username, int index) {
	sem_wait(&lock);
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			string ret = users[c].getMessage(index);
			sem_post(&lock);
			return ret;
		}
	}
	sem_post(&lock);
	return "";
}

string Data::getMessagesResponse(string username) {
    sem_wait(&lock);
    string sendMsg = "";
    string num = getNumberMessages(username);
    if (num == "-1") {
        sendMsg.append("error invalid username \"");
        sendMsg.append(username);
        sendMsg.append("\"\n");
        //send(client, emsg.c_str(), emsg.length(), 0);
    }
    string m = getMessages(username);
    sendMsg.append("list ");
    sendMsg.append(num);
    sendMsg.append("\n");
    sendMsg.append(m);
    sem_post(&lock);
    return sendMsg;
}

string Data::getMessages(string username) {
	//sem_wait(&lock);
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			string ret = users[c].getMessages();
			//sem_post(&lock);
			return ret;
		}
	}
	//sem_post(&lock);
	return "";
}

string Data::getNumberMessages(string username) {
	//sem_wait(&lock);
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			string ret = to_string(users[c].getNextIndex());
			//sem_post(&lock);
			return ret;
		}
	}
	//sem_post(&lock);
	return "-1";
}


