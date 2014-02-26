/*
 * User.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */


#include "User.h"
#include <iostream>

User::User(string usernameParam) {
	username = usernameParam;
	messages = vector<Message>();
}

User::~User() {
	//delete messages;
}

void User::addMessage(Message msgParam) {
	int size = messages.size();
	msgParam.setIndex(size + 1);
	messages.push_back(msgParam);
	int nsize = messages.size();
}

int User::getNextIndex() {
	return messages.size();
}

string User::getUsername() {
	return username;
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
