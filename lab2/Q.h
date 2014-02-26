/*
 * Q.h
 *
 *  Created on: Sep 23, 2013
 *      Author: cam
 */

//#ifndef Q_H
//#define Q_H

#pragma once

#include <queue>
#include <semaphore.h>
#include <string>

using namespace std;

class Q {
public:
	Q(int);
	Q();
	~Q();
	void push(int);
	int pop();
	int size();

private:
	sem_t numberInBuffer_n, bufferSize_e, lock_s;
	queue<int> theQueue;
};

//#endif

