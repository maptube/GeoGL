#pragma once

#include <string>

namespace data {
	namespace gtfs {

		/// <summary>
		/// Class representing an origin destination link between two points on a GTFS network. The time in seconds between the O and D is used for the runlink time.
		/// </summary>
		class GTFS_ODLink
		{
		public:
			std::string O;
			std::string D;
			float Seconds;

			GTFS_ODLink(std::string Origin, std::string Destination, float LinkSeconds)
			{
				O = Origin;
				D = Destination;
				Seconds = LinkSeconds;
			}
		};

		/// <summary>
		/// Utility class for GTFS, allows higher level things like making network graphs and geojson output
		/// </summary>
		class GTFSUtils {
		public:
			//static void ExtractStopPoints(std::string InDirectory, std::string OutFilename);
			static float GreatCircleDistance(float lat1, float lon1, float lat2, float lon2);
			//static void ExtractRunlinks(string InDirectory, bool[] AllowedRouteTypes, out Dictionary<string, List<GTFS_ODLink>> links, out Dictionary<string, GTFSStop> vertices);
			//static void ExtractRunlinks(string InDirectory, string OutFilename, bool[] AllowedRouteTypes);
			//static void ConvertODRunlinksToShapefile(string[] InCSVFiles, string OutFilename);

		};

	} //namespace gtfs
} //namespace data