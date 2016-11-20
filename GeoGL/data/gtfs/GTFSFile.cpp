///GTFSFile.cpp

#include "GTFSFile.h"

#include "text/TextUtilities.h"

#include <string>

#include "ZipLib/ZipFile.h"

namespace data {
	namespace gtfs {

		/// <summary>
		/// Load a GTFS file. Filename must be a zip file.
		/// </summary>
		/// <param name="Filename"></param>
		GTFSFile::GTFSFile(std::string Filename)
		{
			CurrentFilename = Filename;
			/*using (ZipArchive zip = ZipFile.OpenRead(Filename))
			{
				//load routes from routes.txt inside zip file
				ZipArchiveEntry routestxt = zip.GetEntry("routes.txt");
				using (StreamReader reader = new StreamReader(routestxt.Open()))
				{
					ParseRoutes(reader);
				}

				//load stop points from stops.txt inside zip file
				ZipArchiveEntry stopstxt = zip.GetEntry("stops.txt");
				using (StreamReader reader = new StreamReader(stopstxt.Open()))
				{
					ParseStops(reader);
				}

				//trips
				ZipArchiveEntry tripstxt = zip.GetEntry("trips.txt");
				using (StreamReader reader = new StreamReader(tripstxt.Open()))
				{
					ParseTrips(reader);
				}

				//stop_times
				ZipArchiveEntry stoptimestxt = zip.GetEntry("stop_times.txt");
				using (StreamReader reader = new StreamReader(stoptimestxt.Open()))
				{
					ParseStopTimes(reader);
				}
			}*/

			ZipArchive::Ptr archive = ZipFile::Open(Filename);
			ZipArchiveEntry::Ptr entry = archive->GetEntry("routes.txt");
			std::istream* decompressStream = entry->GetDecompressionStream();
			ParseRoutes(decompressStream);

		}

		/// <summary>
		/// Splits a string containing comma separated data and returns the elements as an array of strings.
		/// Quotes can be used around items containing a comma.
		/// These quotes are removed from the string array that is returned.
		/// </summary>
		/// <param name="line">The CSV line string to be split</param>
		/// <returns>Array of strings containing the comma separated elements in the CSV file line</returns>
		/*std::vector<std::string> GTFSFile::ParseCSVLine(std::string line)
		{
			List<string> Items = new List<string>();
			string Current = "";
			bool Quote = false;
			foreach(char ch in line)
			{
				switch (ch)
				{
				case ',':
					if (!Quote)
					{
						Items.Add(Current.Trim());
						Current = "";
					}
					else Current += ","; //comma inside a quote
					break;
				case '"':
					Quote = !Quote;
					break;
				default:
					Current += ch;
					break;
				}
			}
			Items.Add(Current.Trim()); //add trailing item - even if last char was a comma, we still want a null on the end

			return Items.ToArray();
		}*/

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

		/// <summary>
		/// routes.txt
		/// TODO: looks like the gbrail doesn't have all 5 fields in it
		/// Default to rail if nothing found.
		/// </summary>
		/// <param name="reader"></param>
		void GTFSFile::ParseRoutes(std::istream* reader)
		{
			//route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color
			//1-BAK-_-y05-400122,OId_LUL,Bakerloo,,Elephant & Castle - Queen's Park - Harrow & Wealdstone,1,,,

			Routes.clear();
			std::string Line;
			std::getline(*reader,Line); //skip header
			while (!reader->eof())
			{
				std::getline(*reader, Line);
				std::vector<std::string> Fields = SplitCSV(Line);
				std::string RouteId = Fields[0];
				std::string AgencyId = Fields[1];
				std::string RouteShortName = Fields[2];
				std::string RouteLongName = Fields[3];
				std::string RouteDesc = Fields[4];
				int RouteType = 0;

				//The GB rail data doesn't contain a route type.
				//TODO: this needs to be a preset - the route type always defaults to rail if none is present in the GTFS file.
				bool Success = false;
				char* end;
				long result = std::strtol(Fields[5].c_str(),&end,10);
				if (*end!='\0')  RouteType = GTFSRouteType::Rail; //failed to read a number back, so default to rail
				Routes.insert(std::pair<std::string,GTFSRoute>(RouteId, GTFSRoute(RouteId, AgencyId, RouteShortName, RouteLongName, RouteDesc, (GTFSRouteType)RouteType)));
			}
		}

		/// <summary>
		/// reader points to a stops.txt file, which is read to extract stop locations and populate this.StopPoints.
		/// </summary>
		/// <param name="reader"></param>
		void GTFSFile::ParseStops(std::istream* reader)
		{
			//stops.txt
			//stop_id,stop_name,stop_desc,stop_lat,stop_lon,zone_id,stop_url
			//9400ZZLUHAW1,"Wealdstone, Harrow & Wealdstone Station",,51.5923316813,-0.3354040827,,
			//9400ZZLUKEN2,"Kenton (London), Kenton",,51.5822158642,-0.3171684662,,

			Stops.clear();
			std::string Line;
			std::getline(*reader, Line); //skip header
			while (!reader->eof())
			{
				std::getline(*reader, Line);
				std::vector<std::string> Fields = SplitCSV(Line);
				std::string Code = Fields[0];
				std::string Name = Fields[1];
				
				float Lat = std::stof(Fields[3]);
				float Lon = std::stof(Fields[4]);
				Stops[Code] = GTFSStop(Code, Name, Lat, Lon);
			}
		}

		/// <summary>
		/// Reader points to a trips.txt file.
		/// </summary>
		/// <param name="reader"></param>
		void GTFSFile::ParseTrips(std::istream* reader)
		{
			//route_id,service_id,trip_id,trip_headsign,direction_id,block_id,shape_id
			//3-E10-_-y05-41620,3-E10-_-y05-41620,VJ_3-E10-_-y05-41620-1-MF@05:20:00,Islip Manor Road - Ealing Broadway Station / Haven Green,0,,
			//3-E10-_-y05-41620,3-E10-_-y05-41620,VJ_3-E10-_-y05-41620-2-MF@05:22:00,Haven Green / Ealing Broadway - Islip Manor Road,1,,
			
			Trips.clear();
			std::string Line;
			std::getline(*reader, Line); //skip header
			while (!reader->eof())
			{
				std::getline(*reader, Line);
				std::vector<std::string> Fields = SplitCSV(Line);
				std::string RouteId = Fields[0];
				std::string ServiceId = Fields[1];
				std::string TripId = Fields[2];
				std::string TripHeadsign = Fields[3];
				
				Trips[TripId] = GTFSTrip(RouteId, ServiceId, TripId, TripHeadsign);
			}
		}

		/// <summary>
		/// Parses a GTFS time code string, taking into account the fact that it might be of the form 24:49:00.
		/// Arbitrarily sets all times to have a base date of 1 Jan 1970, so any hours>=24 will be set to the 2nd Jan.
		/// </summary>
		/// <param name="strTime"></param>
		/// <returns></returns>
		time_t GTFSFile::ParseGTFSTime(std::string strTime)
		{
			std::vector<std::string> Fields = geogl::text::TextUtilities::split(strTime,std::string(":"));
			int HH = std::stoi(Fields[0]);
			int MM = std::stoi(Fields[1]);
			int SS = std::stoi(Fields[2]);
			bool Over24 = false;
			if (HH >= 24) {
				Over24 = true;
				HH -= 24;
			}
			//DateTime DT = new DateTime(1970, 1, 1, HH, MM, SS);
			struct tm tm_struct = { 0 };
			tm_struct.tm_year = 1970;
			tm_struct.tm_mon = 0;
			tm_struct.tm_mday = 1;
			tm_struct.tm_hour = HH;
			tm_struct.tm_min = MM;
			tm_struct.tm_sec = SS;
			time_t DT = mktime(&tm_struct);
			if (Over24) DT = DT + 24*60*60; //add 24 hours
			return DT;
		}

		void GTFSFile::ParseStopTimes(std::istream* reader)
		{
			//trip_id,arrival_time,departure_time,stop_id,stop_sequence,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled
			//VJ_3-E10-_-y05-41620-1-MF@05:20:00,05:20:00,05:20:00,490008519N,1,,0,0,
			//VJ_3-E10-_-y05-41620-1-MF@05:20:00,05:20:00,05:20:00,490009557N,2,,0,0,
			
			//StopTimes = new Dictionary<string, List<GTFSStopTime>>();
			StopTimes.clear();
			std::string Line;
			std::getline(*reader,Line); //skip header
			while (!reader->eof())
			{
				std::vector<std::string> Fields = SplitCSV(Line);
				//try
				//{
					std::string TripId = Fields[0];
					//Annoyingly, you get some times recorded as 24:49:00, which is obviously over 24 hours. This is on services that run across midnight so you can tell it's not going backwards in time.
					//Need to filter this out though.
					//DateTime ArrivalTime = DateTime.Parse(Fields[1]); //DateTime - only the time is used, Date defaults to today
					//DateTime DepartureTime = DateTime.Parse(Fields[2]); //DateTime
					time_t ArrivalTime = ParseGTFSTime(Fields[1]);
					time_t DepartureTime = ParseGTFSTime(Fields[2]);
					std::string StopId = Fields[3];
					int StopSequence = std::stoi(Fields[4]);

					std::map<std::string,std::list<data::gtfs::GTFSStopTime>>::iterator it = StopTimes.find(TripId);
					if (it==StopTimes.end()) //create a new TripId sequence
					{
						StopTimes.insert(std::pair<std::string,std::list<GTFSStopTime>>(TripId, std::list<GTFSStopTime>()));
					}
					//push new stop onto end of TripId sequence
					StopTimes[TripId].push_back(GTFSStopTime(TripId, ArrivalTime, DepartureTime, StopId, StopSequence));
				//}
				//catch (Exception ex)
				//{
				//	System.Diagnostics.Debug.WriteLine("Error: stop_times.txt " + CurrentFilename + ", " + Line);
				//}
			}
		}

	} //namespace gtfs
} //namespace data
