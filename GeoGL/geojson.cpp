
#include "geojson.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "json/json.h"
#include "json/reader.h"

#include "mesh2.h"
#include "ellipsoid.h"

#include "poly2tri.h"
#include "clipper\clipper.hpp"

using namespace std;

GeoJSON::GeoJSON(void)
{
	_pellipsoid = new Ellipsoid(); //hack! I really want to be passing this in for coordinate conversions
}

GeoJSON::~GeoJSON(void)
{
}

/// <summary>
/// Change the colour of all the vertices.
/// </summary>
/// <param name="new_colour"></param>
void GeoJSON::SetColour(glm::vec3 new_colour)
{
	for (vector<Object3D*>::iterator it=Children.begin(); it!=Children.end(); ++it) {
		Object3D* o3d = *it;
		//all the geojson children are meshes, and Object3D doesn't contain a SetColour (no geometry)
		((Mesh2*)o3d)->SetColour(new_colour);
	}
}

/// <summary>
/// Load a geojson file
/// </summary>
/**
* GeoJSON specification is here: http://www.geojson.org/
* GEOMETRY MUST BE IN WGS84 IN 2D
* This is due to the way that the outer ring and inner rings (holes) must be created, extruded
* and then projected onto a spherical Earth (actually spherical to Cartesian).
* The plane needs to be flat in order to load the complex geometry and be able to extrude it.
* An alternative would be to pre-compute the tesselation rather than using path geometry and load
* the object as a true 3D model rather than a 2D one, but this follows the recognised geospatial
* approach.
* This is the root source of the THREE.JS triangulation code (C++)
* http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
*/
void GeoJSON::LoadFile(std::string Filename)
{
	//MUST ensure that polygons have no outer boundaries that are self-touching.
	//Used geotools to clean the geojson file first.

	cout<<"GeoJSON loading "<<Filename<<endl;

	ifstream inputfile(Filename.c_str());
    std::string to_parse;
    int count;

	//NOTE: you seem to get a slight performance benefit if you load the string and parse it like this, rather than
	//using the json parser on the stream directly.
	//Have a look at this: http://blog.thousandeyes.com/efficiency-comparison-c-json-libraries/
    inputfile.seekg(0, std::ios::end);   
    to_parse.reserve(inputfile.tellg());
    inputfile.seekg(0, std::ios::beg);

    to_parse.assign((std::istreambuf_iterator<char>(inputfile)),
                     std::istreambuf_iterator<char>());


	//ifstream in_json(Filename.c_str());

	//NOTE: massive performance boost if you allocate a pointer to a Json::Value - the deallocate is 100x faster - why?
	Json::Value* root=new Json::Value();   // will contain the root value after parsing.
	Json::Reader reader;
	//bool parsingSuccessful = reader.parse(in_json, root );
	bool parsingSuccessful = reader.parse(to_parse, *root );
	//in_json.close();
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		std::cout  << "Failed to parse configuration\n" << reader.getFormatedErrorMessages();
		return;
	}

	int feature_count=0, vertex_count=0,face_count=0;

	//"type" : "FeatureCollection" ???
	//NOTE: always pass back references to json entities as it avoids copying large parts of the tree which takes lots of time
	const Json::Value& jsFeatures = (*root)["features"]; //root["features"];
	for (Json::Value::iterator it = jsFeatures.begin(); it!=jsFeatures.end(); ++it) {
		//cout<<"Feature"<<endl;
		cout<<"Feature "<<feature_count<<endl;
		++feature_count;
		//HACK!!!!
		//if (feature_count>10) break; //ONLY DO THE FIRST 10 FEATURES

		//note the const & types for much better json performance
		std::string strType = (*it)["type"].asString();
		//cout<<strType<<endl;
		//const Json::Value* jsGeometry = &((*it)["geometry"]);
		//jsGeometry->["type"]
		//ParseJSONGeometry(jsGeometry);
		const Json::Value& jsGeometry = (*it)["geometry"];
		//cout<<jsGeometry["type"].asString()<<endl;
		Mesh2* feature_mesh = ParseJSONGeometry(jsGeometry);
		AddChild(feature_mesh);
	}
	//cout<<"GeoJSON loaded "<<feature_count<<" features"<<endl;
	//cout<<"GeoJSON vertices="<<vertex_count<<" faces="<<face_count<<endl;
	//cout<<"GeoJSON loaded "<<Children.size()<<" features"<<endl;
	cout<<"GeoJSON loaded"<<endl;
}

/**
* Takes a GeoJSON geometry object and builds the mesh
* TODO: only handles polygon and multipolygon types at the moment
*/
/// <summary>
/// Takes a GeoJSON geometry object and builds the mesh object from it.
/// TODO: only handles polygon and multipolygon types at the moment
/// </summary>
/// <param name="jsGeometry">The features[i].geometry part of the GeoJSON FeatureCollection</param>
/// <returns>A mesh object containing the geometry</returns>
Mesh2* GeoJSON::ParseJSONGeometry(const Json::Value& jsGeometry) {
	Mesh2* geom = new Mesh2();

	Json::Value jsType = jsGeometry["type"];
	std::string strType = jsType.asString();
	if (strType=="GeometryCollection") {
		//cout<<"GeometryCollection"<<endl;
	}
	else if (strType=="Point") {
		//cout<<"Point"<<endl;
	}
	else if (strType=="LineString") {
		//cout<<"LineString"<<endl;
	}
	else if (strType=="Polygon") {
		//cout<<"Polygon"<<endl;
		const Json::Value& jsCoordinates = jsGeometry["coordinates"];
		ParseJSONPolygon(*geom,jsCoordinates);
	}
	else if (strType=="MultiPoint") {
		//cout<<"MultiPoint"<<endl;
	}
	else if (strType=="MultiLineString") {
		//cout<<"MultiLineString"<<endl;
	}
	else if (strType=="MultiPolygon") {
		//cout<<"MultiPolygon"<<endl;
		//geometry contains an attribute "coordinates" which is an array of polygons
		const Json::Value& jsCoordinates = jsGeometry["coordinates"];
		for (Json::Value::iterator it = jsCoordinates.begin(); it!=jsCoordinates.end(); ++it) {
			//const Json::Value& poly = *it;
			ParseJSONPolygon(*geom,*it);
		}
	}

	geom->CreateBuffers();
	return geom;
}

/// <summary>
/// A jsPolygon is an array of linear rings where the first ring is the exterior ring and all following
/// rings are interior rings (holes). A linear ring is an array of points, where a point is an array as follows:
/// [ [ [x,y],[x,y],...], [ [x,y],[x,y],...], [ [x,y],[x,y],...] ] (list of linear rings making a polygon)
/// In other words, it's an array of arrays.
/// </summary>
/// <param name="geom">The mesh that this polygon is to be added to (could be part of a multi set)</param>
/// <param name="jsPolygon">The javascript geojson data describing the points</param>
void GeoJSON::ParseJSONPolygon(Mesh2& geom, const Json::Value& jsPolygon) {
	
	//use Clipper to do the polygon cleaning: http://www.angusj.com/delphi/clipper.php
	//use poly2tri to do the triangulation see: http://threejsdoc.appspot.com/doc/three.js/src.source/extras/core/Shape.js.html
	
	ClipperLib::Clipper clipper;
	ClipperLib::Paths linearrings;
	int ring_i=0;
	for (Json::Value::iterator ringIT=jsPolygon.begin(); ringIT!=jsPolygon.end(); ++ringIT) {
		const Json::Value& jsRing = *ringIT;
		//cout<<"Ring "<<ring_i<<" size="<<ring.size()<<endl;
		ClipperLib::Path linearring; //it's a vector of clipper int points
		unsigned int count=jsRing.size();
		for (unsigned int p=0; p<count; p++) { //leave off last point as it's a duplicate TODO: need to check for this
			const Json::Value& jsPointTuple = jsRing[p];
			const Json::Value& px = jsPointTuple[(unsigned int)0];
			const Json::Value& py = jsPointTuple[(unsigned int)1];
//HACK! added scaling factor of 0.1 so I can see the mesh
			double x = px.asDouble(); ///10.0;
			double y = py.asDouble(); ///10.0;
			int xi = (int)(x*10000000);
			int yi = (int)(y*10000000);
			ClipperLib::IntPoint pi(xi,yi); 
			//cout<< x <<","<< y <<" "<<endl;
			linearring.push_back(pi);
		}
		++ring_i;
		linearrings.push_back(linearring);
	}
	//Clean outer and inner rings - the distance figure is really critical to the cleaning process as poly2tri fails if it's too low
	//TODO: need to check poly2tri and Clipper epsilons, I suspect the cleaning issue is to do with colinear edges
	ClipperLib::CleanPolygons(linearrings,8.0/*1.415*/); //1.415 is the default of root 2
	//add outer ring
	clipper.AddPath(linearrings[0],ClipperLib::ptSubject,true);
	//add inner holes
	for (int i=1; i<linearrings.size(); i++) {
		clipper.AddPath(linearrings[i],ClipperLib::ptClip,true);
	}
	ClipperLib::Paths solution;
	clipper.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd); //both were pftEvenOdd

	//OK, that's it for clipping, now move the points to the poly2tri structure for triangularisation


	//put clipper points to poly2tri rings
	vector<vector<p2t::Point*>> p2t_linearrings;
	for (ClipperLib::Paths::iterator solIT=solution.begin(); solIT!=solution.end(); ++solIT) {
		vector<p2t::Point*> p2t_linearring;
		ClipperLib::Path solPath = *solIT;
		cout<<"Solutions: size="<<solPath.size()<<endl;
		for (vector<ClipperLib::IntPoint>::iterator solIT=solPath.begin(); solIT!=solPath.end(); ++solIT) { 
			double x = ((double)solIT->X)/10000000;
			double y = ((double)solIT->Y)/10000000;
			p2t_linearring.push_back(new p2t::Point(x,y)); //need to deallocate this - once code is finished...
			//cout<<x<<","<<y<<endl;
		}
		p2t_linearrings.push_back(p2t_linearring);
	}

	//sanity check rings.count>0 ?
	//hack, passed clipper sanitised first outer ring instead
	p2t::CDT* cdt = new p2t::CDT(p2t_linearrings[0]); //start off with the outer boundary
	//add holes, which get added on to the points array used to create the SweepContext
	for (int i=1; i<p2t_linearrings.size(); i++) {
		cdt->AddHole(p2t_linearrings[i]);
	}
	
	//cout<<"Num points"<<linearring.size()<<endl;

	try {
		cdt->Triangulate(); //throws a collinear error for degenerates
		//now get the data out as a mesh
		vector<p2t::Triangle*> triangles = cdt->GetTriangles();
		
		//TODO: can you optimise this?

		//NO! mesh is already nicely triangulated, so don't need to use the unique test for points
		//push all the points onto the mesh points vector
		glm::vec3 Col(1.0,0.0,0.0); //red
		//p2t::Point points[] = swctx.GetPoints();
		//for (size_t i=0; i<swctx.point_count; i++) {
		//	glm::vec3 P(points[i].x,points[i].y,0);
		//	AddVertex(P,Col);
		//}
		//now go through all the triangles and make the faces

		//can't get index from tri, so going to build face using points
		for (vector<p2t::Triangle*>::iterator trIT = triangles.begin(); trIT!=triangles.end(); ++trIT ) {
			p2t::Point* v1 = (*trIT)->GetPoint(0);
			p2t::Point* v2 = (*trIT)->GetPoint(1);
			p2t::Point* v3 = (*trIT)->GetPoint(2);
			glm::vec3 P1 = _pellipsoid->toVector(glm::radians(v1->x),glm::radians(v1->y)); //don't forget to convert to radians for the ellipse!
			glm::vec3 P2 = _pellipsoid->toVector(glm::radians(v2->x),glm::radians(v2->y));
			glm::vec3 P3 = _pellipsoid->toVector(glm::radians(v3->x),glm::radians(v3->y));
			geom.AddFace(P1,P2,P3,Col,Col,Col); //uses x-order uniqueness of point, so not best efficiency
		}
	}
	catch (...) {
		cout<<"poly2tri exception occurred"<<endl; //actually it tends to just crash rather than give throw an exception
	} //probably a collinear error - can't do anything about that

	//deallocate poly2tri linear rings
	for (vector<vector<p2t::Point*>>::iterator ringsIT=p2t_linearrings.begin(); ringsIT!=p2t_linearrings.end(); ++ringsIT) {
		vector<p2t::Point*> linearring = *ringsIT;
		for (vector<p2t::Point*>::iterator ringIT=linearring.begin(); ringIT!=linearring.end(); ++ringIT) {
			delete *ringIT; //free the point created earlier
		}
	}

	delete cdt; //and finally free the triangulator object
}



