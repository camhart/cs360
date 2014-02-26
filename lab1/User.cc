/*
 * User.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */


#include "User.h"
#include <iostream>

User::User(string username) {
	this->username = username;
	this->messages = vector<Message>();
}

User::~User() {
	//delete this->messages;
}

void User::addMessage(Message msg) {
	int size = messages.size();
	msg.setIndex(size + 1);
	this->messages.push_back(msg);
	int nsize = messages.size();
}

int User::getNextIndex() {
	return this->messages.size();
}

string User::getUsername() {
	return this->username;
}

string User::getMessage(int index) {
	index = index - 1;
	if(index > -1 && index < messages.size()) {
		string ret = "message ";
		ret.append(messages[index].getSubject());
		ret.append(" ");
		ret.append(to_string(messages[index].getMessage().size()));
		ret.append("\n");
		ret.append(messages[index].getMessage());
		return ret;
	} else
		return "";
}

string User::getMessages() {
	string ret = "";
	int size = messages.size();
	for(unsigned c = 0; c < messages.size(); c++) {
		ret.append(to_string(messages[c].getIndex()));
		ret.append(" ");
		ret.append(messages[c].getSubject());
		ret.append("\n");
	}
	return ret;
}
