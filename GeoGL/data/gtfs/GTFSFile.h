#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <ctime>


namespace data {
	namespace gtfs {
		enum GTFSRouteType
		{
			Tram = 0, Subway = 1, Rail = 2, Bus = 3, Ferry = 4, CableCar = 5, Gondola = 6, Funicular = 7
		};

		/// <summary>
		/// From routes.txt
		/// </summary>
		class GTFSRoute
		{
		public:
			std::string route_id;
			std::string agency_id;
			std::string route_short_name;
			std::string route_long_name;
			std::string route_desc;
			GTFSRouteType route_type;

			GTFSRoute(std::string RouteId, std::string AgencyId, std::string RouteShortName, std::string RouteLongName, std::string RouteDesc, GTFSRouteType RouteType)
			{
				route_id = RouteId;
				agency_id = AgencyId;
				route_short_name = RouteShortName;
				route_long_name = RouteLongName;
				route_desc = RouteDesc;
				route_type = RouteType;
			}
		};

		/// <summary>
		/// From stops.txt
		/// </summary>
		class GTFSStop
		{
		public:
			float Lat, Lon;
			std::string Code;
			std::string Name;

			GTFSStop() {}

			GTFSStop(std::string StopCode, std::string StopName, float StopLat, float StopLon)
			{
				Code = StopCode;
				Name = StopName;
				Lat = StopLat;
				Lon = StopLon;
			}
		};

		/// <summary>
		/// From trips.txt
		/// </summary>
		class GTFSTrip
		{
		public:
			std::string route_id;
			std::string service_id;
			std::string trip_id;
			std::string trip_headsign;

			GTFSTrip() {}

			GTFSTrip(std::string RouteId, std::string ServiceId, std::string TripId, std::string TripHeadsign)
			{
				route_id = RouteId;
				service_id = ServiceId;
				trip_id = TripId;
				trip_headsign = TripHeadsign;
			}
		};

		/// <summary>
		/// From stop_times.txt
		/// </summary>
		class GTFSStopTime
		{
		public:
			std::string trip_id;
			time_t arrival_time;
			time_t departure_time;
			std::string stop_id;
			int stop_sequence;

			GTFSStopTime(std::string TripId, time_t ArrivalTime, time_t DepartureTime, std::string StopId, int StopSequence)
			{
				trip_id = TripId;
				arrival_time = ArrivalTime;
				departure_time = DepartureTime;
				stop_id = StopId;
				stop_sequence = StopSequence;
			}
		};

		/// <summary>
		/// Wrapper for the data in a GTFS file. Automatically loads the stop points and routes into internal structures ready for use by another class (maybe GTFSUtils?).
		/// This is a wrapper for a single file, not a whole directory of GTFS zip files like GTFSUtils handles.
		/// </summary>
		class GTFSFile
		{
		public:
			std::string CurrentFilename = "";
			std::map<std::string, GTFSRoute> Routes; //from routes.txt
			std::map<std::string, GTFSStop> Stops; //from stops.txt
			std::map<std::string, GTFSTrip> Trips; //from trips.txt
			std::map<std::string, std::list<GTFSStopTime>> StopTimes; //from stop_times.txt
		public:
			GTFSFile(std::string Filename);
			void ParseRoutes(std::istream* reader);
			void ParseStops(std::istream* reader);
			void ParseTrips(std::istream* reader);
			time_t ParseGTFSTime(std::string strTime);
			void ParseStopTimes(std::istream* reader);
		protected:
			//static std::vector<std::string> ParseCSVLine(std::string line);
		};

	} //namespace gtfs
} //namespace data
