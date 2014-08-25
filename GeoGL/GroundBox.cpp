/*
 * GroundBox.cpp
 *
 *  Created on: 19 Aug 2014
 *      Author: richard
 */

#include "GroundBox.h"

#include "ellipsoid.h"
#include "mesh2.h"
#include "object3d.h"
#include "gengine/Camera.h"
#include "gengine/drawobject.h"
#include "gengine/scenedataobject.h"
#include "gengine/graphicscontext.h"
#include "cache/DataCache.h"

GroundBox::GroundBox() {
	// TODO Auto-generated constructor stub
	_Tau=1.0;
	//need min distance before boxes visible
	//need min and max depth that there is data for

}

GroundBox::~GroundBox() {
	// TODO Auto-generated destructor stub
}

//get rid of this...
//void GroundBox::RenderLOD(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo) {
//	//you could set a limit on the maximum number of ground tiles to render, statically allocate them and only
//	//replace one if the new one leads to an improvement in the view
//}

/// <summary>
/// Convert lon,lat into TileX,Y values
/// </summary>
void GroundBox::LonLatToTileXY(glm::dvec3 RadGeodetic3D,int& TileX,int& TileY) {
	//TODO: the zoom level shouldn't be hard coded!
	const float Z = 12; //we've only got tiles for zoom level 12
	float N = glm::pow(2.0f,Z); //number of tiles in X and Y directions
	TileX=(int)glm::floor(2.0f*glm::pi<float>()/N*RadGeodetic3D.x);
	TileY=(int)glm::floor(2.0f*glm::pi<float>()/N*RadGeodetic3D.y);
}

void GroundBox::Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo) {
	//work out closest point to ground and exit early if >min distance
	//we know the earth is covered with patches (and the min zoom)
	//recursively compute patch contribution to scene

	//float K = sdo._camera->_width/(2*glm::tan(sdo._camera->_fov/2.0)); //perspective distance constant based on projection

	//kick off Chunked LOD rendering - use initial delta of 1.0, which halves with each split
	//error factor (delta) for top mesh is worked out as the distance between two straight line points between vertices and the
	//real distance if the line was actually a curve. This is worked out as delta=R(1-cos(Theta/2)) where Theta is the angle
	//calculated as Theta=2PI/WidthSegments (or height?) and R is the radius i.e. semi-major axis
	//double Theta = 2*glm::pi<double>()/(double)LODWidthSegments;
	//double Delta = _ellipsoid.A()*(1-glm::cos(Theta/2));

	//need delta to be 2 x radius (side view of earth)
	//RenderLOD(GC,sdo,_root,K,(float)Delta);

	//Patch method
	//cheat and create an ellipsoid!!
	Ellipsoid e;
	//TODO: Check distance from view to ellipsoid - exit early if too far
	//Calculate patches in the view pyramid
	glm::dvec3 vView = sdo._camera->GetCameraPos();
	//find ground point that camera position is over
	//TODO: could do a quick height check first rather than the full 3D point
	glm::dvec3 geodetic3D = e.ToGeodetic3D(vView);
	//then draw some tiles around this centre point using the geometry clip mapping trick
	if (geodetic3D.z<1000) { //HEIGHT CHECK
		//double lon = glm::degrees(geodetic3D.x);
		//double lat = glm::degrees(geodetic3D.y);
		//you've got 9 boxes to fill, so put the surrounding geometry into them
		DataCache* dc = DataCache::GetInstance();
		//convert lon,lat into tile numbers
		int TileX,TileY;
		LonLatToTileXY(geodetic3D,TileX,TileY);
		if (dc->GetRemoteFile("")) { //kick off async loading, if the file arrives while still in frame then it gets drawn
			std::string text = GetCacheFile(""); //file is available
		}
	}

}

