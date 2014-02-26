/*
 * MessageData.h
 *
 *  Created on: Sep 15, 2013
 *      Author: cam
 */

//#ifndef DATA_H
//#define DATA_H

#pragma once

#include "User.h"
#include <semaphore.h>

class Data {

public:
	Data();
	~Data();
	void reset();
	void add(string, string, string);
	string getMessages(string);
	string getNumberMessages(string);
	string getMessagesResponse(string);
	string getMessage(string, int);
	void setDebug(bool);
	bool debug = false;

private:
	sem_t lock;
	vector<User> users;
};
//#endif
