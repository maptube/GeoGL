
#include "tiledearth.h"

#include "mesh2.h"
#include "object3d.h"
#include "gengine/vertexformat.h"
#include "gengine/Camera.h"
#include "gengine/drawobject.h"
#include "gengine/scenedataobject.h"
#include "gengine/graphicscontext.h"

using namespace std;
using namespace gengine;

//these constants define the resolution of the data at each LOD level and the number of levels created
const int TiledEarth::LODDepth=1; //4;
const int TiledEarth::LODWidthSegments=40;
const int TiledEarth::LODHeightSegments=40;

TiledEarth::TiledEarth(void)
{
	_ellipsoid = Ellipsoid();
	_Tau=1.0; //This is the screen error tolerance which has to be acceptable to draw an LOD chunk, otherwise the child chunks are considered instead
	_MeshCount=0; //count how many meshes we create

	//this is the part where we build the Earth's chunked LODs
	//the returned mesh is the root node, width and height segments define how many points are on each LOD Chunk (they're all the same). Depth is the depth to create the oct tree to.
	_root = BuildChunkedLOD(LODDepth, LODWidthSegments, LODHeightSegments, -glm::pi<double>(), 0, glm::pi<double>(), 2.0*glm::pi<double>()); //(lat=-pi..+pi, lon=0..2pi)
	//_root is the top of the progressive mesh tree. Can't use Children of this object as the standard scene render would
	//try to draw all of them!
	//bounds=_root->bounds;
	cout<<"TiledEarth Mesh Count="<<_MeshCount<<endl;
}


TiledEarth::~TiledEarth(void)
{
	delete _root;
}

void TiledEarth::AttachShader(gengine::Shader* pShader, bool Recursive)
{
	_root->AttachShader(pShader,Recursive); //apply shader bind to root
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
	//cout<<"MakePatch "<<MinLat<<" "<<MinLon<<" "<<MaxLat<<" "<<MaxLon<<endl;
	double A = _ellipsoid.A();
	double B = _ellipsoid.B();
	double C = _ellipsoid.C();

	Mesh2* mesh = new Mesh2();
	++_MeshCount;
	mesh->_VertexFormat=gengine::Position;

	//work out cartesian coordinates on a patch based on WidthSegments and HeightSegments for the lat/lon bounds
	double dlat=(MaxLat-MinLat)/(double)HeightSegments;
	double dlon=(MaxLon-MinLon)/(double)WidthSegments;
	//int i=0;
	for (double lat=MinLat; lat<=MaxLat; lat+=dlat) {
		for (double lon=MinLon; lon<=MaxLon; lon+=dlon) {
			double x = A * glm::sin(lon);
			double coslon = glm::cos(lon);
			double y = B * coslon * glm::sin(lat);
			double z = C * coslon * glm::cos(lat);
			//push xyz
			mesh->AddVertexRaw(glm::vec3(x,y,z)); //could do with add unique vertex here?
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

	mesh->CreateBuffers();

	return mesh;
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
	//cout<<"TiledEarth BuildChumkedLOD Depth="<<Depth<<endl;
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
void TiledEarth::RenderLOD(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo,Mesh2* mesh,float K,float Delta)
{
	//calculate distance from camera to mesh
	//Real formula from globe book is: d=(c-viewer).v - r  (v=view dir vector, c=centre of object, r=radius of object bounds)
	float D = glm::distance(sdo._camera->GetCameraPos(),mesh->bounds.Centre()); //OK, this is distance to object centre, not nearest point to camera
	float Rho=Delta/D*K;
	if (Rho<=_Tau) {
		const DrawObject& dobj = mesh->GetDrawObject();
		GC->Render(dobj,sdo);
	}
	else {
		//recursion
		for (vector<Object3D*>::const_iterator childIT=mesh->BeginChild(); childIT!=mesh->EndChild(); ++childIT) {
			RenderLOD(GC,sdo,(Mesh2*)*childIT,K,Delta/2.0);
		}
	}
}

/// <summary>
/// </summary>
/// <param name="Tau">This is the screen error tolerance which has to be acceptable to draw an LOD chunk, otherwise the child chunks are considered instead</param>
/// <param name="camera">Camera object which contains the viewpoint to calculate screen error for candidate chunks</param>
void TiledEarth::Render(gengine::GraphicsContext* GC, const gengine::SceneDataObject& sdo)
{
	float K = sdo._camera->_width/(2*glm::tan(sdo._camera->_fov/2.0)); //perspective distance constant based on projection


	//kick off Chunked LOD rendering - use initial delta of 1.0, which halves with each split
	//error factor (delta) for top mesh is worked out as the distance between two straight line points between vertices and the
	//real distance if the line was actually a curve. This is worked out as delta=R(1-cos(Theta/2)) where Theta is the angle
	//calculated as Theta=2PI/WidthSegments (or height?) and R is the radius i.e. semi-major axis
	double Theta = 2*glm::pi<double>()/(double)LODWidthSegments;
	double Delta = _ellipsoid.A()*(1-glm::cos(Theta/2));
	RenderLOD(GC,sdo,_root,K,(float)Delta);
}

