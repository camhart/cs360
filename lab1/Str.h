/*
 * Str.h
 *
 *  Created on: Sep 15, 2013
 *      Author: cam
 */

#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

class Str {
public:
	Str();
	~Str();
	string toLower(string);
	string chopOffFront(string&, char);
	string chopOffFront(string&, string);
	bool contains(const string&, char);
	bool isNumeric(const string&);

};

