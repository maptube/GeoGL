/*
 * DataCache.cpp
 *
 *  Created on: 17 Aug 2014
 *      Author: richard
 */

#include "DataCache.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

namespace geogl {
	namespace cache {

		DataCache* DataCache::_Instance = 0;

		DataCache::DataCache() {
			//TODO: fix this...
			_BaseDir="../cache/";
			BuildFileIndex();
		}

		DataCache::~DataCache() {
			// TODO Auto-generated destructor stub
		}

		DataCache* DataCache::Instance() {
			if (_Instance == 0) {
				_Instance = new DataCache;
			}
			return _Instance;
		}

		/// <summary>
		/// Build a directory of all the files currently in the cache for fast access
		/// </summary>
		void DataCache::BuildFileIndex() {
			_FileIndex.clear();
			//now go through all the files in _BaseDir and add them to _FileMap
			ifstream fin;
			string filepath;
			DIR *dp;
			struct dirent *dirp;
			struct stat filestat;


			dp = opendir(_BaseDir.c_str() );
			if (dp == NULL)
			{
				cout << "Error(" << errno << ") opening " << _BaseDir << endl;
				return; // errno;
			}

			while ((dirp = readdir(dp)))
			{
				filepath = _BaseDir + "/" + dirp->d_name;

				// If the file is a directory (or is in some way invalid) we'll skip it
				if (stat( filepath.c_str(), &filestat )) continue;
				if (S_ISDIR( filestat.st_mode ))         continue;

				_FileIndex.insert(dirp->d_name);
			}

			closedir(dp);
		}

		/// <summary>
		/// Calling GetRemoteFile signals a desire to load the file in the future. If it is already in the cache, then
		/// the method returns true immediately. The intention is that the rendering loop will continually call this method
		/// until it returns true, then use GetCacheFile to load it.
		/// When a call to this method results in a cache miss, the file is added to the load thread for async loading.
		/// </summary>
		bool DataCache::GetRemoteFile(const std::string& URI) {
			//todo:
			//look in local cache first
			//for a cache miss, load from the URI
			//also, need to handle files that fail to load - blacklist them?
			return (_FileIndex.find(URI)!=_FileIndex.end());
		}

		/// <summary>
		/// Turns a URI into the filename of a file in the local cache that can now be loaded directly
		/// </summary>
		std::string DataCache::GetLocalCacheFilename(const std::string& URI) {
			return URI; //OK, this doesn't do anything yet, but everything is going to be stored under uri names like MapTubeD does
		}

	} // namespace cache
} // namespace geogl
