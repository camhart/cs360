/*
 * Message.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */


#include "Message.h"

Message::Message(string subjectParam, string msgParam) {
	index = -1;
	subject = subjectParam;
	message = msgParam;
}

Message::Message() {

}

Message::~Message() {
}

void Message::setIndex(int a) {
	index = a;
}

int Message::getIndex() {
	return index;
}

string Message::getMessage() {
	return message;
}

string Message::getSubject() {
	return subject;
}



