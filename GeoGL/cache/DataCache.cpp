/*
 * DataCache.cpp
 *
 *  Created on: 17 Aug 2014
 *      Author: richard
 */

#include "DataCache.h"
#include "datacacheworker.h"
#include "async/messagequeueeventargs.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>
//#include <memory>
//#include <mutex>
#include <functional>

#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

namespace geogl {
	namespace cache {
		const std::string DataCache::_BaseDir = "../cache/"; //location where cache files will be stored

		DataCache* DataCache::_Instance = 0;

		DataCache::DataCache() {
			BuildFileIndex();
			//todo: add the message queue here
			//DataCacheWorker* dcw = new DataCacheWorker();
			//dcw->_CacheDir = _BaseDir;
			//_requestQueue = new geogl::async::MessageQueue();
			//_doneQueue = new geogl::async::MessageQueue();

			//set up a request queue for getting files asynchronously and a done queue for reporting back when they've loaded into the local cache
			_requestQueue.MessageReceived.push_back(new DataCacheWorker(_doneQueue)); //set up the worker that acts on the messages
			_requestQueue.StartBackgroundThread();
			//set up a done queue handler here to add the loaded files to the index
			_doneQueue.MessageReceived.push_back(new DataCacheIndexWorker(_FileIndex,_RequestIndex));
		}

		DataCache::~DataCache() {
			// TODO Auto-generated destructor stub
			//delete _requestQueue;
			//delete _doneQueue;
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
		/// Copy a local file from src to dst.
		/// TODO: do you need a dst as it's always going to go into the cache?
		/// TODO: how is this for efficiency? Could let the OS do the copy?
		/// </summary>
		bool DataCache::CopyLocalFile(const std::string& src, const std::string& dst) {
			//TODO: should probably copy to a temporary file and rename?
			ifstream in(src.c_str(),ios::in|ios::binary|ios::ate); //use binary flag as we might be handling textures etc.
			if (!in.is_open()) return false; //fail
			ifstream::pos_type pos=in.tellg();

			std::vector<char> data(pos);
			in.seekg(0,ios::beg);
			in.read(&data[0],pos); //read all data into memory - don't like this much, but then all the files are designed to be small

			//now write it out to the new file
			ofstream out(dst, ios::out | ios::binary);
			out.write(&data[0],pos);

			//update file map with the new file we've just added
			//TODO: do you need to synchronise this? The file is copied before it's set so there's no race.
			_FileIndex.insert(src);
			_RequestIndex.erase(src); //remove from waiting request list
			std::cout<<"Move complete "<<src<<std::endl;

			return true;

			//this is quite neat as well
			//ifstream f1 ("C:\\me.txt",fstream::binary);
			//ofstream f2 ("C:\\me2.doc",fstream::trunc|fstream::binary);
			//f2<<f1.rdbuf();
		}

		/// <summary>
		/// callback for after a file has been copied into the cache
		/// </summary>
		void DataCache::CopiedLocalFile(geogl::async::MessageQueueEventArgs& args) {
			std::cout<<"DataCache CopiedLocalFile"<<std::endl;
			if (!args.Success) return; //TODO: what happens here if the request fails? It just stays on the loading list forever
			//update file map with the new file we've just added
			//TODO: do you need to synchronise this? The file is copied before it's set so there's no race.
			geogl::cache::DataCacheWorkerMsg* filemsg = dynamic_cast<geogl::cache::DataCacheWorkerMsg*>(args.umessage.get());
			//std::unique_lock<std::mutex> lck { MyMutexIndex };
			//MyMutexIndex.lock();
			//_FileIndex.insert(filemsg->src);
			//_RequestIndex.erase(filemsg->src); //remove from waiting request list
			std::cout<<"Copied local file complete "<<filemsg->src<<std::endl;
			//lock implicitly released
			//MyMutexIndex.unlock();
		}

		/*static std::vector<char> ReadAllBytes(char const* filename)
		{
			ifstream ifs(filename, ios::binary|ios::ate);
			ifstream::pos_type pos = ifs.tellg();
			
			std::vector<char>  result(pos);
			
			ifs.seekg(0, ios::beg);
			ifs.read(&result[0], pos);
			
			return result;
		}*/

		/// <summary>
		/// Extract just the filename from a full path
		/// </summary>
		std::string DataCache::ExtractFilename(const std::string& Path) {
			std::string::size_type idx;
			idx = Path.rfind('/'); //assumes URI type of separators
			if (idx!=std::string::npos)
				return Path.substr(idx+1,Path.length()-idx+1);
			else
				return Path;
		}

		/// <summary>
		/// Calling GetRemoteFile signals a desire to load the file in the future. If it is already in the cache, then
		/// the method returns true immediately. The intention is that the rendering loop will continually call this method
		/// until it returns true, then use GetCacheFile to load it.
		/// When a call to this method results in a cache miss, the file is added to the load thread for async loading.
		/// TODO: check whether the waiting and thread loading is thread safe
		/// </summary>
		bool DataCache::GetRemoteFile(const std::string& URI) {
			_doneQueue.ProcessQueue(); //handle processing of any messages received since last time
			//todo:
			//look in local cache first
			//for a cache miss, load from the URI
			//also, need to handle files that fail to load - blacklist them?
			
			//old code!!!!
			//return (_FileIndex.find(URI)!=_FileIndex.end());

			//std::unique_lock<std::mutex> lck { _MutexIndex };
			bool hit=(_FileIndex.find(URI)!=_FileIndex.end());
			if (hit) return true; //already there, so nothing to do

			bool waiting=(_RequestIndex.find(URI)!=_RequestIndex.end());
			if (waiting) return false; //file in the process of loading

			//GUARD FOR TOO MANY THREADS
			if (_RequestIndex.size()>0) return false; //was 9

			//kick off a thread to copy the file into the local cache
			//assuming the URI is actually a local file... (and no thread!)
			std::string Filename = ExtractFilename(URI); //strip the filename off and move the file into the cache
			cout<<"Moving "<<URI<<" into "<<(_BaseDir+Filename)<<endl;
			_RequestIndex.insert(URI);
			//old code
			//bool success = CopyLocalFile(URI,_BaseDir+Filename);
			//return success;
			//new code
			//std::unique_ptr<std::thread> t(new std::thread(&DataCache::CopyLocalFile,this,URI,_BaseDir+Filename));
			//_ThreadPool.push_back(std::move(t));
			//even newer thread worker code
			const std::string src = URI;
			const std::string dst=_BaseDir+Filename;
			//geogl::cache::DataCacheWorkerMsg* msg = new geogl::cache::DataCacheWorkerMsg(src,dst);
			geogl::async::MessageQueueEventArgs args(new geogl::cache::DataCacheWorkerMsg(src,dst));

			//args.callback=(void(*)(geogl::async::MessageQueueEventArgs&))&DataCache::CopiedLocalFile; //TODO: you could type the callback

			_requestQueue.Post(args);
			return false;
		}


		/// <summary>
		/// Turns a URI into the filename of a file in the local cache that can now be loaded directly
		/// </summary>
		std::string DataCache::GetLocalCacheFilename(const std::string& URI) {
			//return URI; //OK, this doesn't do anything yet, but everything is going to be stored under uri names like MapTubeD does
			std::string Filename = ExtractFilename(URI);
			return _BaseDir+Filename;
		}

	} // namespace cache
} // namespace geogl
