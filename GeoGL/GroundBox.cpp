/*
 * GroundBox.cpp
 *
 *  Created on: 19 Aug 2014
 *      Author: richard
 */

#include "GroundBox.h"

#include <istream>
#include <string>
#include <sstream>
#include <set>

#include "ellipsoid.h"
#include "mesh2.h"
#include "object3d.h"
#include "geojson.h"
#include "gengine/vertexformat.h"
#include "gengine/Camera.h"
#include "gengine/drawobject.h"
#include "gengine/scenedataobject.h"
#include "gengine/graphicscontext.h"
//#include "gengine/shader.h"
//#include "gengine/shaderuniformcollection.h"
//#include "gengine/shaderuniform.h"
#include "cache/DataCache.h"
#include "async/meshworker.h"
#include "async/messagequeueeventargs.h"


using namespace std;

using namespace gengine;

using namespace geogl;
using namespace geogl::cache;

const int GroundBox::BoxZoomLevel = 14; // zoom level used for all ground boxes to make it easy to switch

GroundBox::GroundBox(gengine::GraphicsContext* workerGC) : _workerGC(workerGC) {
	// TODO Auto-generated constructor stub
	//_Tau=1.0;
	//need min distance before boxes visible
	//need min and max depth that there is data for

	//set up a request queue for loading meshes asynchronously and a done queue for reporting back when they've been created on the gpu
	_requestQueue.MessageReceived.push_back(new geogl::async::MeshWorker(_doneQueue,_workerGC)); //set up the worker that acts on the messages
	_requestQueue.StartBackgroundThread();
	//set up a done queue handler here to add the loaded mesh objects
//TODO: _Shader isn't actually set at this point!
	_doneQueue.MessageReceived.push_back(new GroundBoxMeshWorker(_gndboxes,_Shader)); //this is the worker to put the meshes into the ground boxes array
}

GroundBox::~GroundBox() {
	// TODO Auto-generated destructor stub
	//TODO: free the meshes in the _gndboxes
}

void GroundBox::SetShader(gengine::Shader* shader) {
	//if you change the shader, then you have to get rid of the mesh worker message queue and create another one with the new shader
	//of course, anything not yet processed is destroyed, but then all the meshes need to be changed as well...
	_Shader=shader;
	_doneQueue.MessageReceived.clear();
	_doneQueue.MessageReceived.push_back(new GroundBoxMeshWorker(_gndboxes,_Shader));
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
	const float Z = BoxZoomLevel; //we've only got tiles for zoom level 12
	float N = glm::pow(2.0f,Z); //number of tiles in X and Y directions
	TileX=(int)glm::floor((RadGeodetic3D.x+glm::pi<float>())/(2.0f*glm::pi<float>())*N);
	TileY=(int)glm::floor((RadGeodetic3D.y+glm::pi<float>())/(2.0f*glm::pi<float>())*N);
	//2046 2635
	//HACK! Always use the same tile for testing
	//TileX=2048; TileY=2629;
	//debug
	//cout<<"lon="<<glm::degrees(RadGeodetic3D.x)<<" lat="<<glm::degrees(RadGeodetic3D.y)<<" TileX="<<TileX<<" TileY="<<TileY<<endl;
}

/// <summary>
/// Build a box that fits the bounds of the tile and return the mesh.
/// Used for debugging.
/// </summary>
Mesh2* GroundBox::DebugMesh(const int TileZ,const int TileX,const int TileY) {
	float fTileZ=(float)TileZ, fTileX=(float)TileX, fTileY=(float)TileY;
	float N = glm::pow(2.0f,fTileZ); //number of tiles in X and Y directions
	double minLon=TileX*2.0f*glm::pi<float>()/N-glm::pi<float>();
	double minLat=TileY*2.0f*glm::pi<float>()/N-glm::pi<float>();
	double maxLon=(TileX+1)*2.0f*glm::pi<float>()/N-glm::pi<float>();
	double maxLat=(TileY+1)*2.0f*glm::pi<float>()/N-glm::pi<float>();

	Ellipsoid e;
	Mesh2* mesh = new Mesh2();
	mesh->_VertexFormat=gengine::PositionColourNormal;
	glm::vec3 P0 = e.toVector(minLon,minLat,100);
	glm::vec3 P1 = e.toVector(maxLon,minLat,100);
	glm::vec3 P2 = e.toVector(maxLon,maxLat,100);
	glm::vec3 P3 = e.toVector(minLon,maxLat,100);
	glm::vec3 vN = glm::cross(P3-P0,P1-P0);
	if ((((int)TileX)%2==0)^(((int)TileY)%2==0)) {
		glm::vec3 red(1.0,0.0,0.0);
		//glm::vec3 blue(0.0,0.0,1.0);
		mesh->AddFace(P0,P1,P2,red,red,red,vN,vN,vN);
		mesh->AddFace(P0,P2,P3,red,red,red,vN,vN,vN);
		//back face (check normal?)
		//mesh->AddFace(P2,P1,P0,red,red,red,vN,vN,vN);
		//mesh->AddFace(P3,P2,P0,red,red,red,vN,vN,vN);
	}
	else {
		glm::vec3 blue(0.0,0.0,1.0);
		mesh->AddFace(P0,P1,P2,blue,blue,blue,vN,vN,vN);
		mesh->AddFace(P0,P2,P3,blue,blue,blue,vN,vN,vN);
		//back face (check normal?)
		//mesh->AddFace(P2,P1,P0,blue,blue,blue,vN,vN,vN);
		//mesh->AddFace(P3,P2,P0,blue,blue,blue,vN,vN,vN);
	}
	mesh->CreateBuffers();

	return mesh;
}

//return the filename of a tile identified by its x and y location - basically a hack for tile filename conversion
//TODO: need to remove the hard coding i.e. dir, filename and zoom level 12 only
std::string GetGeoJSONFilename(const int TileZ, const int TileX,const int TileY) {
	std::stringstream ss;
	//ss<<"../data/vectortiles/"<<TileZ<<"_"<<TileX<<"_"<<TileY<<".geojson";
	ss<<"../data/vectortiles/"<<TileZ<<"_"<<TileX<<"_"<<TileY<<".obj";
	//ss<<"../data/vectortiles/14_8165_10503.obj";
	return ss.str();
}

/// <summary>
/// Given the tile coords of the new centre box, shuffle the existing ground boxes around to make the centre correct. The aim
/// is to reuse any existing meshes as it's likely to be a scroll in one direction. New meshes have their details entered, but
/// are not loaded at this stage. Any meshes no longer being used are freed along with their buffers.
/// 0 1 2
/// 3 4 5
/// 6 7 8
/// </summary>
/// <param name="TileZ">Z coord of centre box tile</param>
/// <param name="TileX">X coord of centre box tile</param>
/// <param name="TileY">Y coord of centre box tile</param>
void GroundBox::ShuffleBoxes(const int TileZ, const int TileX, const int TileY) {
	//If the centre box already has the correct tile xyz coords, then exit early. Although it is possible that the viewpoint
	//could spin, leaving the centre correct and the others changes, we don't bother with direction so it makes no difference.
	//std::cout<<"In ShuffleBoxes"<<std::endl;
	if ((_gndboxes[4].TileX==TileX)&&(_gndboxes[4].TileY==TileY)&&(_gndboxes[4].TileZ==TileZ)) return;

	//std::cout<<"ShuffleBoxes : about to shuffle"<<std::endl;

	//OK, it's moved, so we are going to have to do some work
	int oldx=_gndboxes[0].TileX, oldy=_gndboxes[0].TileY; //store old min box to check for reuse
	BoxContent B[9];
	int i=0;
	for (int y=TileY-1; y<=TileY+1; y++) {
		for (int x=TileX-1; x<=TileX+1; x++) {
			B[i].TileX=x;
			B[i].TileY=y;
			B[i].TileZ=TileZ;
			//look at previous position of this mesh to see whether we can reuse it
			int x2=x-oldx;
			int y2=y-oldy;
			if ((x2>=0)&&(x2<=2)&&(y2>=0)&&(y2<=2)) {
				int i2=y2*3+x2;
				B[i].mesh=std::move(_gndboxes[i2].mesh);
				B[i].IsEmpty=_gndboxes[i2].IsEmpty; B[i].IsLoading=_gndboxes[i2].IsLoading;
			}
			else { B[i].mesh=nullptr; B[i].IsEmpty=true; }
			++i;
		}
	}
	//now assign the local copy to the live copy and free any meshes that aren't being used
	for (i=0; i<9; i++) {
		//now assignment
		_gndboxes[i].TileX=B[i].TileX;
		_gndboxes[i].TileY=B[i].TileY;
		_gndboxes[i].TileZ=B[i].TileZ;
		_gndboxes[i].mesh=std::move(B[i].mesh);
		_gndboxes[i].IsEmpty=B[i].IsEmpty; //(_gndboxes[i].mesh==nullptr); //DEBUG TRAP
		_gndboxes[i].IsLoading=B[i].IsLoading;
	}
	//std::cout<<"Exit ShuffleBoxes"<<std::endl;
}


void GroundBox::UpdateData(const gengine::SceneDataObject& sdo) {
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

	_doneQueue.ProcessQueue(); //update current ground boxes with any loaded meshes, anything not in view is disposed

	//Patch method
	//cheat and create an ellipsoid!!
	Ellipsoid e;
	//TODO: Check distance from view to ellipsoid - exit early if too far - could do a sphere check for speed
	//Calculate patches in the view pyramid
	glm::dvec3 vView = sdo._camera->GetCameraPos();
	//find ground point that camera position is over
	//TODO: could do a quick height check first rather than the full 3D point
	glm::dvec3 geodetic3D = e.ToGeodetic3D(vView);
	//then draw some tiles around this centre point using the geometry clip mapping trick
	if (geodetic3D.z<10000) { //HEIGHT CHECK
		//double lon = glm::degrees(geodetic3D.x);
		//double lat = glm::degrees(geodetic3D.y);
		//you've got 9 boxes to fill, so put the surrounding geometry into them
		DataCache* dc = DataCache::Instance();
		//convert lon,lat into tile numbers
		int TileX,TileY;
		LonLatToTileXY(geodetic3D,TileX,TileY);
		ShuffleBoxes(BoxZoomLevel,TileX,TileY); //shuffle current data around centre box as it's unlikely to have moved much
		//do a mesh done list check for any newly delivered complete meshes
		//now initiate a load and cache check for any ground box meshes set as IsEmpty
		//TODO:
		//for all mesh worker messages... fill empty boxes and discard any meshes not required
		for (int i=0; i<9; i++) {
			if (!(_gndboxes[i].IsLoading)&&(_gndboxes[i].IsEmpty)) {
				std::string TileFilename = GetGeoJSONFilename(_gndboxes[i].TileZ,_gndboxes[i].TileX,_gndboxes[i].TileY); //shouldn't this be a URL?
				if (dc->GetRemoteFile(TileFilename)) { //kick off async loading, if the file arrives while still in frame then it gets drawn
					std::string LocalFilename = dc->GetLocalCacheFilename(TileFilename); //file is available
					//load geojson mesh and extrude - this should be in a thread
					//GeoJSON* geoj = new GeoJSON();
					//geoj->LoadFile(LocalFilename);
					////geoj->ToMesh(e);
					//geoj->ExtrudeMesh(e,0); //hack - 0 is height
					//geoj->AttachShader(_Shader,true); //NOTE: this only attaches to the geoj child objects, NOT the parent which is an O3D
					//geoj->SetColour(glm::vec3(1.0f,0.0f,0.0f));
					//_gndboxes[i].mesh=geoj;

					//load pre-computed obj file - this should also be in a thread
					//this is the bit that needs to push a mesh worker message
					//Mesh2* mesh = new Mesh2();
					//mesh->_VertexFormat=gengine::PositionColourNormal;
					//mesh->FromOBJ(LocalFilename);
					//mesh->CreateBuffers();
					//mesh->AttachShader(_Shader,true);
					//mesh->SetColour(glm::vec3(1.0f,0.0f,0.0f));
					//_gndboxes[i].mesh=mesh;
					//cout<<"Mesh Loaded"<<endl;
					_requestQueue.Post(new geogl::async::MeshWorkerMsg(LocalFilename,_gndboxes[i].TileX,_gndboxes[i].TileY,_gndboxes[i].TileZ));
					//_gndboxes[i].IsEmpty=true; //set flag to false until it's loaded - it's already false (see above)
					_gndboxes[i].IsLoading=true; //so we don't try and load it again while it's still in flight

					//DEBUG - push a coloured ground square to show the grid
					//geoj->AddChild(DebugMesh(BoxZoomLevel,_gndboxes[i].TileX,_gndboxes[i].TileY));
					//geoj->AttachShader(_Shader,true);
					//END DEBUG
					//_gndboxes[i].IsEmpty=false; //don't forget to set the flag
				}
			}
		}
	}
}

void GroundBox::Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo) {
	UpdateData(sdo);
	for (int i=0; i<9; i++) {
		if (!_gndboxes[i].IsEmpty) {
			//RENDER!!!!!
			//cout<<"Render GroundBoxes"<<endl;

			//GeoJSON Render
			//NOTE: the geojson object is an object3d container which is not renderable, so you have to draw the children
			//for (vector<Object3D*>::const_iterator it=_gndboxes[i].mesh->BeginChild(); it!=_gndboxes[i].mesh->EndChild(); ++it) {
			//	const DrawObject& dobj = (*it)->GetDrawObject();
			//	//ShaderUniformCollection* uniforms=dobj._ShaderProgram->_shaderUniforms; //might want to set one of these?
			//	GC->Render(dobj,sdo);
			//}


			//Mesh2 Render
			//Here, we only have the parent mesh object to render
			if (_gndboxes[i].mesh==NULL) {
				std::cerr<<"FAULT: NULL BOX "
					<<i<<": "<<_gndboxes[i].TileX<<" "<<_gndboxes[i].TileY<<" "<<_gndboxes[i].IsLoading<<std::endl;
				//HACK! need to fix this as it shouldn't happen!
			}
			else {
				const DrawObject& dobj = _gndboxes[i].mesh->GetDrawObject();
				GC->Render(dobj,sdo);
			}
		}
	}
}

