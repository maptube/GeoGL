#pragma once

#include "GTFSUtils.h"

#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

namespace data {
	namespace gtfs {

    

			/// <summary>
			/// Splits a string containing comma separated data and returns the elements as an array of strings.
			/// Quotes can be used around items containing a comma.
			/// These quotes are removed from the string array that is returned.
			/// </summary>
			/// <param name="line">The CSV line string to be split</param>
			/// <returns>Array of strings containing the comma separated elements in the CSV file line</returns>
			/*protected static string[] ParseCSVLine(string line)
			{
				List<string> Items = new List<string>();
				string Current = "";
				bool Quote = false;
				foreach (char ch in line)
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

			/// <summary>
			/// Take a directory containing GTFS files and write out a file containing the lat lons of the unique stop point locations.
			/// TODO: you could do this from the GTFSFile class now.
			/// </summary>
			/// <param name="InDirectory"></param>
			/// <param name="OutFilename"></param>
			/*void GTFSUtils::ExtractStopPoints(std::string InDirectory, std::string OutFilename)
			{
				Dictionary<string,float[]> StopPoints = new Dictionary<string,float[]>();

				foreach (string Filename in Directory.EnumerateFiles(InDirectory, "*.zip"))
				{
					System.Diagnostics.Debug.WriteLine("Processing: " + Filename);
					using (ZipArchive zip = ZipFile.OpenRead(Filename))
					{
						//foreach (ZipArchiveEntry entry in zip.Entries)
						//{
						//    entry.
						//}

						//stops.txt
						//stop_id,stop_name,stop_desc,stop_lat,stop_lon,zone_id,stop_url
						//9400ZZLUHAW1,"Wealdstone, Harrow & Wealdstone Station",,51.5923316813,-0.3354040827,,
						//9400ZZLUKEN2,"Kenton (London), Kenton",,51.5822158642,-0.3171684662,,
						ZipArchiveEntry stops = zip.GetEntry("stops.txt");
						using (StreamReader reader = new StreamReader(stops.Open()))
						{
							string Line = reader.ReadLine(); //skip header
							while ((Line = reader.ReadLine()) != null)
							{
								string[] Fields = ParseCSVLine(Line);
								string Name = Fields[0];
								try
								{
									float Lat = Convert.ToSingle(Fields[3]);
									float Lon = Convert.ToSingle(Fields[4]);
									if (!StopPoints.ContainsKey(Name))
									{
										StopPoints.Add(Name, new float[] { Lat, Lon });
									}
								}
								catch (Exception ex)
								{
									System.Diagnostics.Debug.WriteLine("Error: " + Filename + ", " + Line);
								}
							}
						}
					}
				}

				//now write out the file
				using (StreamWriter writer = File.CreateText(OutFilename))
				{
					writer.WriteLine("stop_id,stop_lat,stop_lon");
					foreach (KeyValuePair<string, float[]> KVP in StopPoints)
					{
						writer.WriteLine(KVP.Key + "," + KVP.Value[0] + "," + KVP.Value[1]);
					}
				}

				System.Diagnostics.Debug.WriteLine("Processed " + StopPoints.Count + " stop points from " + InDirectory);
			}*/

			/// <summary>
			/// Returns metres.
			/// </summary>
			/// <param name="lat1"></param>
			/// <param name="lon1"></param>
			/// <param name="lat2"></param>
			/// <param name="lon2"></param>
			/// <returns></returns>
			float GTFSUtils::GreatCircleDistance(float lat1, float lon1, float lat2, float lon2)
			{
				//taken from here: http://www.movable-type.co.uk/scripts/latlong.html
				//double R = 6371e3; // metres
				double R = 6378137.0; //metres
				double Phi1 = lat1 * M_PI / 180.0;
				double Phi2 = lat2 * M_PI / 180.0;
				double DeltaPhi = (lat2 - lat1) * M_PI / 180.0;
				double DeltaLambda = (lon2 - lon1) * M_PI / 180.0;

				double a = sin(DeltaPhi / 2) * sin(DeltaPhi / 2) + cos(Phi1) * cos(Phi2) * sin(DeltaLambda / 2) * sin(DeltaLambda / 2);
				double c = 2 * atan2(sqrt(a), sqrt(1 - a));
				double d = R * c;
				return (float)d;
			}

			/// <summary>
			/// Overload of ExtractRunLinks to enable the links and vertices to be extracted directly for building a graph straight off the gtfs files.
			/// NOTE: this only extracts bus and ferry runlinks by design.
			/// TODO: make the route type configurable so you could do tubes as a separate case?
			/// </summary>
			/// <param name="InDirectory"></param>
			/// <param name="AllowedRouteTypes">Bitset of allowed route types in following order [ Tram=0, Subway=1, Rail=2, Bus=3, Ferry=4, CableCar=5, Gondola=6, Funicular=7 ]</param>
			/// <param name="links"></param>
			/// <param name="vertices"></param>
			/*void GTFSUtils::ExtractRunlinks(string InDirectory, bool [] AllowedRouteTypes, out Dictionary<string, List<GTFS_ODLink>> links, out Dictionary<string, GTFSStop> vertices)
			{
				//key is "origin|destination", value is a list of all the OD links for this segment so we can compute an average time and frequency of service
				links = new Dictionary<string, List<GTFS_ODLink>>();
				vertices = new Dictionary<string, GTFSStop>();

				foreach (string Filename in Directory.EnumerateFiles(InDirectory, "*.zip"))
				{
					System.Diagnostics.Debug.WriteLine("Processing: " + Filename);
					GTFSFile gtfs = new GTFSFile(Filename);
					//The stop times are keyed on the tripid, so each gtfs.StopTimes dictionary value contains a list GTFSStopTimes, which is a list of stops and times for a single TripId
					foreach (List<GTFSStopTime> stops in gtfs.StopTimes.Values)
					{
						//route type detection code - BUS and FERRY ONLY
						//find the trip id for this stop time, which gives the the route id, which give me the route and the route type - horribly inefficient - should walk the routes and trips
						string TripId = stops[0].trip_id;
						string RouteId = gtfs.Trips[TripId].route_id;
						GTFSRoute Route = gtfs.Routes[RouteId];
						//if ((Route.route_type != GTFSRouteType.Bus) && (Route.route_type != GTFSRouteType.Ferry)) continue;
						if (!AllowedRouteTypes[(int)Route.route_type])
						{
							//check that this route type is on the list - if not then exit
							System.Diagnostics.Debug.WriteLine("Skip route type=" + Route.route_type);
							continue;
						}
						//end of route type detection code

						//starting a new timed service, which is a list of stops
						GTFSStopTime LastStop = null;
						foreach (GTFSStopTime Stop in stops)
						{
							//build OD links from this i.e. last stop to this stop
							if (LastStop != null)
							{
								//and OD link has an O and D key, plus a runtime
								//How do you cope with different arrival and departure times? Always take the departure?
								TimeSpan runlink = Stop.departure_time - LastStop.departure_time;
								GTFS_ODLink OD = new GTFS_ODLink(LastStop.stop_id, Stop.stop_id, (float)runlink.TotalSeconds);
								string key = LastStop.stop_id + "|" + Stop.stop_id;
								if (!links.ContainsKey(key)) links.Add(key, new List<GTFS_ODLink>());
								links[key].Add(OD);
							}
							LastStop = Stop;
						}
					}
					//and concatenate the stops data from this file into a master list for later
					foreach (GTFSStop stop in gtfs.Stops.Values)
					{
						if (!vertices.ContainsKey(stop.Code)) vertices.Add(stop.Code, stop); //of course, this doesn't handle stops with the same code and different data...
					}
				}
				System.Diagnostics.Debug.WriteLine("Extracted " + links.Count + " distinct OD segments.");
			}*/

			/// <summary>
			/// Passed in a directory containing gtfs zip files, process out runlinks for origin destination pairs and write them out.
			/// Use the trips.txt file to get the journey paths and the stop_times.txt file to get the runlinks. Stop locations in stops.txt file.
			/// TODO: you could stick the hour of day into the OD link to see how it varied over the course of the day.
			/// </summary>
			/// <param name="InDirectory"></param>
			/// <param name="OutFilename"></param>
			/*void GTFSUtils::ExtractRunlinks(string InDirectory, string OutFilename, bool [] AllowedRouteTypes)
			{
				Dictionary<string, List<GTFS_ODLink>> links = new Dictionary<string, List<GTFS_ODLink>>();
				Dictionary<string, GTFSStop> vertices = new Dictionary<string, GTFSStop>();
				GTFSUtils.ExtractRunlinks(InDirectory, AllowedRouteTypes, out links, out vertices);

				//now you have to write the file out
				using (StreamWriter writer = File.CreateText(OutFilename))
				{
					writer.WriteLine("code_o,lat_o,lon_o,code_d,lat_d,lon_d,average_secs,min_seconds,max_seconds,count,distance_metres,average_speed_ms-1");
					//NOTE: each links.value is a list of the same link, just for different times - we need to average them
					foreach (List<GTFS_ODLink> link in links.Values)
					{
						try
						{
							GTFSStop Stop_O = vertices[link[0].O];
							GTFSStop Stop_D = vertices[link[0].D];
							float total_secs = 0, count = 0;
							float MinSeconds = float.MaxValue, MaxSeconds = 0;
							foreach (GTFS_ODLink timedlink in link)
							{
								total_secs += timedlink.Seconds;
								if (timedlink.Seconds < MinSeconds) MinSeconds = timedlink.Seconds;
								if (timedlink.Seconds > MaxSeconds) MaxSeconds = timedlink.Seconds;
								++count;
							}
							float AverageSecs = total_secs / count;
	//Set hard limit of 30 seconds for minimum link time
	//                        if ((count == 0) || (AverageSecs < 30)) AverageSecs = 30;
							float DistMetres = GreatCircleDistance(Stop_O.Lat, Stop_O.Lon, Stop_D.Lat, Stop_D.Lon);
							//some of the runlinks are zero seconds, so need to be careful about average speed
							float AverageSpeedMS = -1;
							if (AverageSecs>0) AverageSpeedMS = DistMetres / AverageSecs;
							writer.WriteLine(Stop_O.Code + "," + Stop_O.Lat + "," + Stop_O.Lon + "," + Stop_D.Code + "," + Stop_D.Lat + "," + Stop_D.Lon
								+ "," + AverageSecs + "," + MinSeconds + "," + MaxSeconds + "," + count + "," + DistMetres + "," + AverageSpeedMS);
						}
						catch (Exception ex)
						{
							System.Diagnostics.Debug.WriteLine("Error: (missing vertex?) " + link[0].O + " " + link[0].D + " "+ex.Message);
						}
					}
				}

			}*/

			/// <summary>
			/// Convert the data from ExtractRunLinks into a shapefile containing lines linking the origin and destination nodes.
			/// </summary>
			/// <param name="InDirectory">List of CSV files to process</param>
			/// <param name="OutFilename"></param>
			/*void GTFSUtils::ConvertODRunlinksToShapefile(string [] InCSVFiles, string OutFilename)
			{
				GeometryFactory gf = new GeometryFactory();
				List<Feature> fc = new List<Feature>();
				foreach (string file in InCSVFiles)
				{
					using (TextReader reader = File.OpenText(file))
					{
						string Line = reader.ReadLine(); //skip header line
						while ((Line = reader.ReadLine()) != null)
						{
							//code_o,lat_o,lon_o,code_d,lat_d,lon_d,average_secs,min_seconds,max_seconds,count,distance_metres,average_speed_ms-1
							//9400ZZLUEAC1,51.49582,-0.1009803,9400ZZLULBN1,51.49852,-0.1111693,120,120,120,1882,767.7943,6.398285
							string[] Fields = Line.Split(new char[] { ',' });
							string code_o = Fields[0];
							float lat_o = Convert.ToSingle(Fields[1]);
							float lon_o = Convert.ToSingle(Fields[2]);
							string code_d = Fields[3];
							float lat_d = Convert.ToSingle(Fields[4]);
							float lon_d = Convert.ToSingle(Fields[5]);
							float AverageSecs = Convert.ToSingle(Fields[6]);
							float MinSeconds = Convert.ToSingle(Fields[7]);
							float MaxSeconds = Convert.ToSingle(Fields[8]);
							float Count = Convert.ToSingle(Fields[9]);
							float DistanceMetres = Convert.ToSingle(Fields[10]);
							float AverageSpeedMS = Convert.ToSingle(Fields[11]);
							Feature f = new Feature();
							Coordinate OriginPoint = new Coordinate(lon_o,lat_o);
							Coordinate DestPoint = new Coordinate(lon_d,lat_d);
							f.Geometry = gf.CreateLineString(new Coordinate[] { OriginPoint, DestPoint });
							f.Attributes = new AttributesTable();
							f.Attributes.AddAttribute("origin", code_o);
							f.Attributes.AddAttribute("dest", code_d);
							f.Attributes.AddAttribute("AverageSecs", AverageSecs);
							f.Attributes.AddAttribute("MinSecs", MinSeconds);
							f.Attributes.AddAttribute("MaxSecs", MaxSeconds);
							f.Attributes.AddAttribute("Count", Count);
							f.Attributes.AddAttribute("DistM", DistanceMetres);
							f.Attributes.AddAttribute("AvSpeedMS", AverageSpeedMS);
							fc.Add(f);
						}
					}
				}
				ShapeUtils.WriteShapefile(OutFilename, fc);
			}*/



    } //namespace gtfs
} //namespace data
