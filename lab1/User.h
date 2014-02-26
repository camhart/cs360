/*
 * User.h
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */

#include <vector>
#include "Message.h"
using namespace std;

class User {
public:
	User(string);
	~User();
	void addMessage(Message);
	int getNextIndex();
	string getUsername();
	string getMessages();
	string getMessage(int);

private:
	string username;
	vector<Message> messages;
};


