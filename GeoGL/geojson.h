#pragma once
//#include "opengl4.h"
//#include "mesh.h"
#include "object3d.h"

#include "json/json.h"

//forward references
namespace Json {
class Value;
}

class Mesh2;
class Ellipsoid;

/// <summary>
/// Object3D parent containing child meshes for every feature, built from a GeoJSON file
/// 
/// </summary>
class GeoJSON : public Object3D {
public:
	GeoJSON(void);
	~GeoJSON(void);
	GeoJSON(const Ellipsoid& ellipsoid);
	void LoadFile(std::string Filename);
	Mesh2* ParseJSONGeometry(const Json::Value& jsGeometry);
	void ParseJSONPolygon(Mesh2& geom, const Json::Value& jsPolygon);
	void ParseJSONPolygonExtrude(Mesh2& geom, const Json::Value& jsPolygon);
	void SetColour(glm::vec3 new_colour);
private:
	Ellipsoid* _pellipsoid;
};