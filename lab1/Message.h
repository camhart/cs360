/*
 * Message.h
 *
 *  Created on: Sep 10, 2013
 *      Author: cam
 */

#include <string>
using namespace std;

class Message {
public :
	Message(int index, string subject, string message);
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
