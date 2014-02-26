/*
 * MessageData.cpp
 *
 *  Created on: Sep 15, 2013
 *      Author: cam
 */

#include "Data.h"

Data::Data(){
	this->curIndex = 0;
	this->users = vector<User>();
	users.push_back(User("!@#$NULL"));
}

Data::~Data() {

}

void Data::reset() {
	this->curIndex = 0;
	this->users = vector<User>();
	users.push_back(User("!@#$NULL"));
}

void Data::add(string username, string subject, string message) {
	//Message(int index, string subject, string message);
	//User user = getUser(username, true);
	Message msg = Message(-1, subject, message);
	addMessage(username, msg);
}

//User Data::getUser(string username, bool add) {
//	for(int c = 0; c < users.size(); c++) {
//		if(users[c].getUsername() == username) {
//			return users[c];
//		}
//	}
//	if(add) {
//		users.push_back(User(username));
//		return users[users.size() - 1];
//	} else {
//		return users[0]; //null user -  w/ username "!@#$NULL"
//	}
//}

void Data::addMessage(string username, Message msg) {
	for(int c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			users[c].addMessage(msg);
			return;
		}
	}
	users.push_back(User(username));
	users[users.size() - 1].addMessage(msg);
}

string Data::getMessage(string username, int index) {
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			return users[c].getMessage(index);
		}
	}
	return "";
}

string Data::getMessages(string username) {
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			return users[c].getMessages();
		}
	}
	return "";
}

string Data::getNumberMessages(string username) {
	//User user1 = getUser(username, false);
	for(unsigned c = 0; c < users.size(); c++) {
		if(users[c].getUsername() == username) {
			return to_string(users[c].getNextIndex());
		}
	}
	return "-1";
}


