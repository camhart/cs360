/*
 * Message.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */


#include "Message.h"

Message::Message(int index, string subject, string msg) {
	this->index = index;
	this->subject = subject;
	this->message = msg;
}

Message::Message() {

}

Message::~Message() {
}

void Message::setIndex(int a) {
	this->index = a;
}

int Message::getIndex() {
	return this->index;
}

string Message::getMessage() {
	return this->message;
}

string Message::getSubject() {
	return this->subject;
}



