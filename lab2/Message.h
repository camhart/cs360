/*
 * Message.h
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */

//#ifndef MESSAGE_H
//#define MESSAGE_H

#pragma once

#include <string>
using namespace std;

class Message {
public :
	Message(string, string);
	~Message();
	string getSubject();
	string getMessage();
	int getIndex();
	void setIndex(int);

private :
	Message();
	int index;
	string subject;
	string message;
};

//#endif
