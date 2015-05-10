#pragma once

/*
 * DataCache.h
 *
 *  Created on: 17 Aug 2014
 *      Author: richard
 */

#include <ctime>
#include <string>
#include <set>
#include <vector>
#include <thread>
#include <mutex>
//#include "loaderthread.h"
#include "async/messagequeue.h"

//forward declarations
namespace geogl {
	namespace cache {
//		class LoaderThread;
	}
}

// The aim of this class is to provide asynchronous process management for content that has to be loaded dynamically.
// The pattern is to request a resource using GetRemoteFile. The first time this is called, it kicks off a thread to do the
// load and copy in the background while frame rendering continues. A subsequent call in the future to GetRemoteFile will
// return true when it has loaded, then the file is accessible from the local cache using GetLocalCacheFilename.

namespace geogl {
	namespace cache {

	//struct TimedThread {
	//	std::time_t time;
	//	std::thread t;
	//};

		//singleton? (TODO)
	//This is a lazy singleton implementation. DO NOT use multithreaded as you can get two of them
	//Also, the file caching isn't synchronised.
		class DataCache {
		public:
			static const std::string _BaseDir; //location where cache files will be stored
		private:
			//std::mutex MyMutexIndex; //mutex used for fileindex and requestindex changes
			//std::string test;
			std::set<std::string> _FileIndex; //files currently in the cache
			std::set<std::string> _RequestIndex; //files requested and waiting for load to complete

			static DataCache* _Instance;
			std::vector<std::unique_ptr<std::thread>> _ThreadPool;
			geogl::async::MessageQueue _requestQueue;
			geogl::async::MessageQueue _doneQueue;
		protected:
			DataCache();
			void BuildFileIndex();
			virtual void CopiedLocalFile(geogl::async::MessageQueueEventArgs& args);
			bool CopyLocalFile(const std::string& src, const std::string& dst);
		public:
			virtual ~DataCache();
			static DataCache* Instance();

			std::string ExtractFilename(const std::string& Path);
			bool GetRemoteFile(const std::string& URI);
			std::string GetLocalCacheFilename(const std::string& URI);
		};

	} // namespace cache
} // namespace geogl
