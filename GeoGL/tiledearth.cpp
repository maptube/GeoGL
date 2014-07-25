
#include "tiledearth.h"

#include "mesh2.h"
#include "object3d.h"
#include "gengine/vertexformat.h"
#include "gengine/Camera.h"

using namespace std;

//these constants define the resolution of the data at each LOD level and the number of levels created
const int TiledEarth::LODDepth=4;
const int TiledEarth::LODWidthSegments=20;
const int TiledEarth::LODHeightSegments=20;

TiledEarth::TiledEarth(void)
{
	_ellipsoid = Ellipsoid();

	//this is the part where we build the Earth's chunked LODs
	//the returned mesh is the root node, width and height segments define how many points are on each LOD Chunk (they're all the same). Depth is the depth to create the oct tree to.
	_root = TiledEarth::BuildChunkedLOD(LODDepth, LODWidthSegments, LODHeightSegments, -glm::pi<double>(), 0, glm::pi<double>(), 2.0*glm::pi<double>()); //(lat=-pi..+pi, lon=0..2pi)
}


TiledEarth::~TiledEarth(void)
{
	delete _root;
}

/// <summary>
/// Make a section of the Earth mesh which fits inside the min/max lat/lon coords box
/// </summary>
/// <param name="WidthSegments">Number of segments around globe</param>
/// <param name="HeightSegments">Number of segments up globe</param>
/// <param name="MinLat">minimum latitude of box in radians</param>
/// <param name="MinLon">minimum longitude of box in radians</param>
/// <param name="MaxLat">maximum latitude of box in radians</param>
/// <param name="MaxLon">maximum longitude of box in radians</param>
/// <returns>the mesh object</returns>
Mesh2* TiledEarth::MakePatch(int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon) {
	double A = _ellipsoid.A();
	double B = _ellipsoid.B();
	double C = _ellipsoid.C();

	Mesh2* mesh = new Mesh2();
	mesh->_VertexFormat=gengine::Position;

	//work out cartesian coordinates on a patch based on WidthSegments and HeightSegments for the lat/lon bounds
	double dlat=(MaxLat-MinLat)/HeightSegments;
	double dlon=(MaxLon-MaxLat)/WidthSegments;
	//int i=0;
	for (double lat=MinLat; lat<=MaxLat; lat+=dlat) {
		for (double lon=MinLon; lon<=MaxLon; lon+=dlon) {
			double x = A * glm::sin(lon);
			double coslon = glm::cos(lon);
			double y = B * coslon * glm::sin(lat);
			double z = C * coslon * glm::cos(lat);
			//push xyz
			mesh->AddVertex(glm::vec3(x,y,z)); //could do with add unique vertex here?
		}
	}

	//now create indices, which is easy as it's just a grid and every object is the same
	for (int h=0; h<HeightSegments; ++h) {
		for (int w=0; w<WidthSegments; ++w) {
			int i=h*(WidthSegments+1)+w; //base index (bottom left)
			int Pa=i;
			int Pb=i+1;
			int Pc=i+WidthSegments+2;
			int Pd=i+WidthSegments+1;
			mesh->AddFace(Pa,Pd,Pc);
			//next face
			mesh->AddFace(Pa,Pc,Pb);
		}
	}

	return mesh;

	//this is the code it was based on from sphere init2
	//work out and store the sin and cos of the positions around one of the horizontal slices as it gets used going up the sphere
	//and also the heights of the slices
	//float* sinlon = new float[WidthSegments];
	//float* coslon = new float[WidthSegments];
	//float* sinlat = new float[HeightSegments];
	//float* coslat = new float[HeightSegments];

	//float anglon=2*glm::pi<float>()/(float)WidthSegments; //segment angle step around lon
	//float anglat=2*glm::pi<float>()/(float)HeightSegments; //segment angle step around lat
	//int i=0;
	//for (float lon=0; lon<2*glm::pi<float>(); lon+=anglon) {
	//	sinlon[i]=glm::sin(lon); //sin(lon);
	//	coslon[i]=glm::cos(lon); //cos(lon);
	//	++i;
	//}
	////now store the height slices
	//i=0;
	//for (float lat=-glm::pi<float>(); lat<glm::pi<float>(); lat+=anglat) {
	//	sinlat[i]=glm::sin(lat); //sin(lat);
	//	coslat[i]=glm::cos(lat); //cos(lat);
	//	++i;
	//}

	////OK, that's enough pre-calculation, wire up the points into faces
	//for (int ilat=0; ilat<HeightSegments; ++ilat) {
	//	for (int ilon=0; ilon<WidthSegments; ++ilon) {
	//		int ilat2=(ilat+1)%HeightSegments; int ilon2=(ilon+1)%WidthSegments;
	//		//work out four patch coordinates based on current lat/lon angles +- each combination of next lat/lon coord
	//		glm::vec3 Pa,Pb,Pc,Pd;
	//		//TODO: you might want to check the orientation of the point
	//		Pa.x=sinlon[ilon];		Pa.y=coslon[ilon]*sinlat[ilat];			Pa.z=coslon[ilon]*coslat[ilat];
	//		Pb.x=sinlon[ilon2];		Pb.y=coslon[ilon2]*sinlat[ilat];		Pb.z=coslon[ilon2]*coslat[ilat];
	//		Pc.x=sinlon[ilon2];		Pc.y=coslon[ilon2]*sinlat[ilat2];		Pc.z=coslon[ilon2]*coslat[ilat2];
	//		Pd.x=sinlon[ilon];		Pd.y=coslon[ilon]*sinlat[ilat2];		Pd.z=coslon[ilon]*coslat[ilat2];
	//		//TODO: could do with texture and normals being calculated here
	//		//NOTE: the mesh AddVertex code takes care of the VertexFormat if we're not using all the vertex data (i.e. colour/texture/normal absent)
	//		
	//		//Pa,Pb,Pc,Pd are clockwise, so add in reverse
	//		AddFace(Pa,Pd,Pc,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
	//		AddFace(Pa,Pc,Pb,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
	//	}
	//}

	////now scale up (or down) to the correct radius
	//ScaleVertices(A,B,C);

	////and finally build the graphics
	//CreateBuffers();

	//delete [] sinlon;
	//delete [] coslon;
	//delete [] sinlat;
	//delete [] coslat;
}


// recursive build all the mesh objects that we need
/// <summary>
/// Build a mesh object for the box passed in the min/max lat/lon coords and recurse down to create an oct tree of more detailed child mesh objects based on the depth
/// </summary>
/// <param name="Depth">Depth to create child LOD chunks down to. When this gets to zero, recursion stops.</param>
/// <param name="WidthSegments">Number of segments around globe</param>
/// <param name="HeightSegments">Number of segments up globe</param>
/// <param name="MinLat">minimum latitude of box in radians</param>
/// <param name="MinLon">minimum longitude of box in radians</param>
/// <param name="MaxLat">maximum latitude of box in radians</param>
/// <param name="MaxLon">maximum longitude of box in radians</param>
/// <returns>The root node of the chunked LOD hierarchy</returns>
Mesh2* TiledEarth::BuildChunkedLOD(int Depth, int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon)
{
	//create a mesh at this level 
	Mesh2* mesh = MakePatch(WidthSegments,HeightSegments,MinLat,MinLon,MaxLat,MaxLon);

	if (Depth==0) return mesh; //guard case, we're at the limit of recursion, so don't create any children
	//quadtree recurse
	//double LatSplit = (MinLat+MaxLat)/2;
	//double LonSplit = (MinLon+MaxLon)/2;
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   MinLon,   LatSplit, LonSplit) ); //sw
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, MinLon,   MaxLat,   LonSplit) ); //nw
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit, MaxLat,   MaxLon) ); //ne
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit, LatSplit, MaxLon) ); //se

	//octtree recurse - this splits the longitude into four chunks along the axes
	double LatSplit = (MinLat+MaxLat)/2;
	double LonSplit1 = (MinLon+MaxLon)/2; //centre
	double LonSplit0 = (MinLon+LonSplit1)/2; //left quarter (west)
	double LonSplit2 = (LonSplit1+MaxLon)/2; //right quarter (east), so lon split goes 012 west to east
	//8 BuildChunkedLOD - it doesn't matter what order you do this in
	//top top (north)
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, MinLon,    MaxLat,   LonSplit0 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit0, MaxLat,   LonSplit1 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit1, MaxLat,   LonSplit2 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit2, MaxLat,   MaxLon    ) );
	//bottom row (south)
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   MinLon,    LatSplit, LonSplit0 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit0, LatSplit, LonSplit1 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit1, LatSplit, LonSplit2 ) );
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit2, LatSplit, MaxLon    ) );

	return mesh;
}

//delta halves each recursion
void RenderLOD(Mesh2* mesh,gengine::Camera* camera,float K,float Tau,float Delta)
{
	//calculate distance from camera to mesh
	float D = glm::distance(camera->GetCameraPos(),mesh->bounds.Centre()); //OK, this is distance to object centre, not nearest point to camera
	float Rho=Delta/D*K;
	if (Rho<=Tau) {
		//draw - this is borrowed from the globe render
		const DrawObject& dobj = mesh->GetDrawObject();
		glm::dvec3 P = glm::dvec3(dobj._ModelMatrix[3])-vCam;
		//OK, this is a box test, which is better, this, sqrt or a d^2 comparison with VERY large numbers? AND we might draw twice.
		if (((abs(P.x)>=nearClip)&&(abs(P.x)<=farClip))||((abs(P.y)>=nearClip)&&(abs(P.y)<=farClip))||((abs(P.z)>=nearClip)&&(abs(P.z)<=farClip)))
			GC->Render(dobj,*_sdo);
	}
	else {
		//recursion
		for (vector<Object3D*>::iterator childIT=mesh->BeginChild(); childIT!=mesh->EndChild(); ++childIT) {
			RenderLOD((Mesh2*)*childIT,camera,K,Tau,Delta/2.0);
		}
	}
}

/// <summary>
/// </summary>
/// <param name="Tau">This is the screen error tolerance which has to be acceptable to draw an LOD chunk, otherwise the child chunks are considered instead</param>
/// <param name="camera">Camera object which contains the viewpoint to calculate screen error for candidate chunks</param>
void TiledEarth::Render(float Tau,gengine::Camera* camera)
{
	float K = camera->_width/(2*glm::tan(camera->_fov/2.0)); //perspective distance constant based on projection

	//kick off Chunked LOD rendering - use initial delta of 1.0, which halves with each split
	RenderLOD(_root,camera,K,Tau,1.0f);
}

//go through current objects and test pixel projection size if too big then split, if too small then drop and go back a level
//void TiledEarth::UpdateView(gengine::Camera* cam)
//{
	//TODO: could keep the last view and only update meshes if it has changed significantly?

	//here's the method:
	//for every child mesh
	//  project bounding box and check size
	//  if too big for screen then split into smaller
	//  if too detailed for screen then dump and replace with higher level (how? there are multiple)
//}