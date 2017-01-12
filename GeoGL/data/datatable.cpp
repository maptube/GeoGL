/*
 * datatable.cpp
 *
 *  Created on: 4 Dec 2016
 *      Author: richard
 */

#include "datatable.h"

#include <vector>

namespace data {

DataTable::DataTable() {
	// TODO Auto-generated constructor stub

}

DataTable::~DataTable() {
	// TODO Auto-generated destructor stub
}

//copied from ABM::Model
std::vector<std::string> SplitCSV(const std::string& Line) {
	std::vector<std::string> Fields;
	std::string Text = "";
	bool InQuote = false;
	for (std::string::const_iterator it = Line.begin(); it != Line.end(); ++it) {
		char ch = *it;
		switch (ch) {
		case '\"':
			InQuote = !InQuote; //what happens with this: "abc"d,"xyz" at the moment you get [abcd,xyz]
			break;
		case ',':
			if (InQuote) Text += ch;
			else { Fields.push_back(Text); Text = ""; }
			break;
		case ' ':
			if ((InQuote) || (Text.length()>0)) Text += ch; //don't allow whitespace at start of field
			break;
		default:
			Text += ch;
		}
	}
	if (Text.length()>0) Fields.push_back(Text);
	return Fields;
}

DataTable DataTable::FromCSV(std::vector<char> chars) {
	//it's a vector of chars because that's what curl returns
	DataTable dt;

	bool haveHeaderLine = false;
	std::string line="";
	for (auto it = chars.begin(); it!=chars.end(); ++it) {
		if (*it!='\n') line=line+*it;
		else {
			std::vector<std::string> fields = SplitCSV(line);
			if (!haveHeaderLine) {
				for (auto hIT = fields.begin(); hIT!=fields.end(); ++hIT)
					dt.Headers.push_back(*hIT);
				haveHeaderLine=true;
			}
			else {
				dt.Rows.push_back(fields);
			}
			line="";
		}
	}

	return dt;
}

} //namespace data
