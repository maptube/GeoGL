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
		public:
			static const std::string _BaseDir; //location where cache files will be stored
		private:
			std::set<std::string> _FileIndex;
			static DataCache* _Instance;
		protected:
			DataCache();
			void BuildFileIndex();
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
