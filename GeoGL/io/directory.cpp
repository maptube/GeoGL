/*
 * directory.cpp
 *
 *  Created on: 27 Jul 2015
 *      Author: richard
 */

#include "directory.h"

#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <string>

#include "wildcardmatch.h"

namespace geogl {
namespace io {

Directory::Directory() {
	// TODO Auto-generated constructor stub

}

Directory::~Directory() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// Static function to match any occurance of a file pattern in the BaseDir directory. Wildcards are *=any length, ?=single char
/// </summay>
std::string Directory::MatchFilePattern(std::string BaseDir, std::string pattern)
{
	//todo: could do with a C# style Directory class
	std::string filepath, Result;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;
	geogl::io::WildcardMatch* match = new geogl::io::WildcardMatch();

	dp = opendir(BaseDir.c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << BaseDir << std::endl;
		return ""; // errno;
	}

	Result = "";
	while ((dirp = readdir(dp)))
	{
		std::string name = dirp->d_name;
		filepath = BaseDir + "/" + name;

		// If the file is a directory (or is in some way invalid) we'll skip it
		if (stat( filepath.c_str(), &filestat )) continue;
		if (S_ISDIR( filestat.st_mode ))         continue;

		//check match on dirp->d_name and pattern
		//std::tr1::smatch matches;
		//bool found = regex_match(name,matches,pattern);
		bool found = match->match(pattern,name);
		if (found) {
			Result=name;
			std::cout<<"Match Found: "<<name<<std::endl;
			break;
		}
		else {
			std::cout<<"Match not found: "<<name<<std::endl;
		}
	}
	closedir(dp);
	delete match;

	return Result;
}

} // namespace io
} // namespace geogl
