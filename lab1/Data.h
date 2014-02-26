/*
 * MessageData.h
 *
 *  Created on: Sep 15, 2013
 *      Author: cam
 */

#include "User.h"

class Data {

public:
	Data();
	~Data();
	void reset();
	void add(string, string, string);
	//User getUser(string, bool);
	void addMessage(string, Message);
	string getMessages(string);
	string getNumberMessages(string);
	string getMessage(string, int);

private:
	int curIndex;
	vector<User> users;
};

