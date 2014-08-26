#pragma once

/*
 * DataCache.h
 *
 *  Created on: 17 Aug 2014
 *      Author: richard
 */

#include <string>
#include <set>

namespace geogl {
	namespace cache {

		//singleton? (TODO)
	//This is a lazy singleton implementation. DO NOT use multithreaded as you can get two of them
	//Also, the file caching isn't synchronised.
		class DataCache {
		private:
			std::string _BaseDir;
			std::set<std::string> _FileIndex;
			static DataCache* _Instance;
		protected:
			DataCache();
			void BuildFileIndex();
		public:
			virtual ~DataCache();
			static DataCache* Instance();


			bool GetRemoteFile(const std::string& URI);
			std::string GetLocalCacheFilename(const std::string& URI);
		};

	} // namespace cache
} // namespace geogl
