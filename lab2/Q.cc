/*
 * Q.cc
 *
 *  Created on: Sep 23, 2013
 *      Author: cam
 */

#include "Q.h"

using namespace std;

Q::Q(int threadCount) {
	theQueue = queue<int>();
	sem_init(&lock_s, 0, 1);
	sem_init(&numberInBuffer_n, 0, 0);
	sem_init(&bufferSize_e, 0, threadCount);
}

Q::Q() {

}

Q::~Q() {
}

int Q::pop() {
	int ret = -1;
	sem_wait(&numberInBuffer_n);
	sem_wait(&lock_s);
	ret = theQueue.front();
	theQueue.pop();
	sem_post(&lock_s);
	sem_post(&bufferSize_e);
	return ret;
}

void Q::push(int val) {
	sem_wait(&bufferSize_e);
	sem_wait(&lock_s);
	theQueue.push(val);
	sem_post(&lock_s);
	sem_post(&numberInBuffer_n);
}

int Q::size() {
	sem_wait(&lock_s);
	int ret = theQueue.size();
	sem_post(&lock_s);
	return ret;
}

//string Q::printValues() {
//	sem_wait(&lock_s);
//	string ret = "";
//	for(int c = 0; c < theQueue.size(); c++) {
//		ret.append(to_string(theQueue.front()));
//		ret.append(", ");
//	}
//	sem_post(&lock_s);
//	return ret;
//}
