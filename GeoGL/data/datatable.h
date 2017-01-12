/*
 * datatable.h
 *
 *  Created on: 4 Dec 2016
 *      Author: richard
 */

#pragma once

#include <string>
#include <list>
#include <vector>

namespace data {

	class DataTable {
	public:
		DataTable();
		virtual ~DataTable();

		std::list<std::string> Headers;
		std::list<std::vector<std::string>> Rows;

		static DataTable FromCSV(std::vector<char> chars);
	};

} //namespace data
