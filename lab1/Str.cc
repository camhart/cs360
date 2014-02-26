/*
 * Str.cc
 *
 *  Created on: Sep 17, 2013
 *      Author: cam
 */

#include "Str.h"

Str::Str() {
}

Str::~Str() {

}

string Str::toLower(string str) {
	string ret = str;
	for (unsigned c = 0; c < str.size(); ++c) {
		if (!islower(str[c])) {
			ret[c] = tolower(ret[c]);
		}
	}
	return ret;
}

string Str::chopOffFront(string& buffer, char split) {
	string ret = "";
	for (unsigned c = 0; c < buffer.size(); c++) {
		if (buffer[c] == split) {
			ret = buffer.substr(0, c);
			buffer = buffer.substr(c + 1, buffer.length());
			return ret;
		}
	}
	return ret;
}

string Str::chopOffFront(string& buffer, string split) {
	string ret = "";
	for (unsigned c = 0; c < buffer.size(); c++) {
		if (c + split.size() <= buffer.size()
				&& buffer.substr(c, c + split.size()) == split) {
			ret = buffer.substr(0, c);
			buffer = buffer.substr(c + split.size(), buffer.length());
			return ret;
		}
	}
	return ret;
}

bool Str::contains(const string& tokstr, char c) {
	for (unsigned d = 0; d < tokstr.size(); c++) {
		if (tokstr[c] == c) {
			return true;
		}
	}
	return false;
}

bool Str::isNumeric(const string& numstr) {
	string::const_iterator iter = numstr.begin();
	while (iter != numstr.end() && isdigit(*iter))
		++iter;
	return !numstr.empty() && iter == numstr.end();
}
