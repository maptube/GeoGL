
#include "tiledearth.h"

#include <sstream>
#include <string>

#include "mesh2.h"
#include "object3d.h"
#include "gengine/vertexdata.h"
#include "gengine/vertexformat.h"
#include "gengine/Camera.h"
#include "gengine/drawobject.h"
#include "gengine/scenedataobject.h"
#include "gengine/graphicscontext.h"
#include "gengine/indexbuffer.h"
#include "gengine/ogldevice.h"
#include "gengine/texture2d.h"
#include "gengine/shader.h"
#include "gengine/shaderuniformcollection.h"
#include "gengine/shaderuniform.h"

using namespace std;
using namespace gengine;

//these constants define the resolution of the data at each LOD level and the number of levels created
const int TiledEarth::LODDepth=2; //4;
const int TiledEarth::LODWidthSegments=40; //40
const int TiledEarth::LODHeightSegments=40; //40

TiledEarth::TiledEarth(void)
{
	_ellipsoid = Ellipsoid();
	_Tau=1.0; //~1.0-2.0? This is the screen error tolerance which has to be acceptable to draw an LOD chunk, otherwise the child chunks are considered instead
	_MeshCount=0; //count how many meshes we create
	//_MinDelta=std::numeric_limits<float>::max();
	//calculate delta at the maximum LOD based on the initial delta from spheroid radius and width segments, then recursively divide by two for LODDepth levels
	double Theta = 2*glm::pi<double>()/(double)LODWidthSegments; //angle between segments approximating sphere at top level
	double InitialDelta = _ellipsoid.A()*(1-glm::cos(Theta/2)); //delta at top level
	_MinDelta=InitialDelta/glm::pow<double>(2.0,(double)LODDepth); //delta at maximum LOD based on dividing mesh by two each step

	//this is the part where we build the Earth's chunked LODs
	//step 1: create an index buffer which will be shared by all the meshes
	_ib = CreateIndexBuffer(LODWidthSegments,LODHeightSegments);
	//the returned mesh is the root node, width and height segments define how many points are on each LOD Chunk (they're all the same). Depth is the depth to create the oct tree to.
	//_root = BuildChunkedLOD(LODDepth, LODWidthSegments, LODHeightSegments, -glm::pi<double>(), 0, glm::pi<double>(), 2.0*glm::pi<double>()); //(lat=-pi..+pi, lon=0..2pi)
	_root = BuildChunkedLOD(LODDepth, LODWidthSegments, LODHeightSegments, 0, 0, 0, -glm::half_pi<double>(), 0, glm::half_pi<double>(), 2*glm::pi<double>()); //(lat=-pi/2..+pi/2, lon=0..2pi)
	//_root is the top of the progressive mesh tree. Can't use Children of this object as the standard scene render would
	//try to draw all of them!
	//bounds=_root->bounds;
	cout<<"TiledEarth Mesh Count="<<_MeshCount<<endl;
	cout<<"TiledEarth R="<<_root->bounds.Radius()<<endl;
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
/// All the patches in the tiled Earth can use the same index buffer as they are all grid objects of the same size and topology.
/// This function creates the single index buffer which is used on every mesh and child mesh for rendering.
/// <summary>
gengine::IndexBuffer* TiledEarth::CreateIndexBuffer(const int WidthSegments,const int HeightSegments)
{
	int NumFaces = HeightSegments*WidthSegments*6; //this is number of face indices, so number of faces*3
	_NumElements = NumFaces; //used for setting correct number of index elements when manipulating the mesh vertex data directly
	IndexBuffer* ib=OGLDevice::CreateIndexBuffer(ElementArrayBuffer,StaticDraw,NumFaces*sizeof(unsigned int)); //note that NumFaces is already *3 (see def above)
	unsigned int* buf_indices = new unsigned int[NumFaces*3]; //GLuint

	//now create indices, which is easy as it's just a grid and every object is the same
	//Note the < on height and width which is correct
	int ptr=0;
	for (int h=0; h<HeightSegments; ++h) {
		for (int w=0; w<WidthSegments; ++w) {
			int i=h*(WidthSegments+1)+w; //base index (bottom left)
			int Pa=i;
			int Pb=i+1;
			int Pc=i+WidthSegments+2;
			int Pd=i+WidthSegments+1;
			//mesh->AddFace(Pc,Pd,Pa); //(CCW)
			//mesh->AddFace(Pa,Pd,Pc); //(CW)
			//next face
			//mesh->AddFace(Pb,Pc,Pa); //(CCW)
			//mesh->AddFace(Pa,Pc,Pb); //(CW)
			
			//wireframe
			//mesh->AddFace(Pa,Pb,Pc);
			//mesh->AddFace(Pb,Pc,Pd);

			//TODO: you could use rectangular faces instead of triangles?

			//CCW
			buf_indices[ptr++]=Pc; buf_indices[ptr++]=Pd; buf_indices[ptr++]=Pa; //first triangle
			buf_indices[ptr++]=Pb; buf_indices[ptr++]=Pc; buf_indices[ptr++]=Pa; //second triangle

			//wireframe
			//buf_indices[ptr++]=Pa; buf_indices[ptr++]=Pb; buf_indices[ptr++]=Pc; //first triangle
			//buf_indices[ptr++]=Pb; buf_indices[ptr++]=Pc; buf_indices[ptr++]=Pd; //second triangle
		}
	}

	//don't forget to actually copy the data into the buffer!
	ib->CopyFromMemory(buf_indices);

	delete [] buf_indices;
	return ib;
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
	cout<<"MakePatch "<<MinLat<<" "<<MinLon<<" "<<MaxLat<<" "<<MaxLon<<endl;
	double A = _ellipsoid.A();
	double B = _ellipsoid.B();
	double C = _ellipsoid.C();

	Mesh2* mesh = new Mesh2();
	++_MeshCount;
	mesh->_VertexFormat=gengine::Position;

	//work out cartesian coordinates on a patch based on WidthSegments and HeightSegments for the lat/lon bounds
	double dlat=(MaxLat-MinLat)/(double)HeightSegments;
	double dlon=(MaxLon-MinLon)/(double)WidthSegments;
	//old code - too inaccurate
	/*int lastPointI=-1;
	for (double lat=MinLat; lat<=MaxLat; lat+=dlat) {
		for (double lon=MinLon; lon<=MaxLon; lon+=dlon) {
			//original formula - lat lon the wrong way around
			//double x = A * glm::sin(lon);
			//double coslon = glm::cos(lon);
			//double y = B * coslon * glm::sin(lat);
			//double z = C * coslon * glm::cos(lat);
			double x = A * glm::sin(lat);
			double coslon = glm::cos(lat);
			double y = B * coslon * glm::sin(lon);
			double z = C * coslon * glm::cos(lon);
			//push xyz
			lastPointI=mesh->AddVertexRaw(glm::vec3(x,y,z)); //could do with add unique vertex here?
		}
	}*/
	//new code
	int lastPointI=-1;
	//Note the <= on height and width which is correct
	for (int h=0; h<=HeightSegments; ++h) {
		for (int w=0; w<=WidthSegments; ++w) {
			double lat=MinLat+(double)h*dlat;
			double lon=MinLon+(double)w*dlon;
			//original formula
			//double x = A * glm::sin(lon);
			//double coslon = glm::cos(lon);
			//double y = B * coslon * glm::sin(lat);
			//double z = C * coslon * glm::cos(lat);
			
			//new one - this gets the poles correct
			//double y = B * glm::sin(lat);
			//double coslat = glm::cos(lat);
			//double x = A * coslat * glm::sin(lon);
			//double z = C * coslat * glm::cos(lon);
			
			//new 2 - this works, creates the pole singularity at the poles and maintains the correct coords for vector data put onto the globe (matches ellipsoid tovector)
			//double z = A * glm::sin(lat);
			//double coslon = glm::cos(lat);
			//double y = B * coslon * glm::sin(lon);
			//double x = C * coslon * glm::cos(lon);
			
			//new 3 - why not use the ellipsoid toVector? This works!
			glm::vec3 p = _ellipsoid.toVector(lon,lat,0);
			
			//push xyz
			lastPointI=mesh->AddVertexRaw(p /*glm::vec3(x,y,z)*/); //add unique vertex here
		}
	}

	//now create indices, which is easy as it's just a grid and every object is the same
	//Note the < on height and width which is correct
	//for (int h=0; h<HeightSegments; ++h) {
	//	for (int w=0; w<WidthSegments; ++w) {
	//		int i=h*(WidthSegments+1)+w; //base index (bottom left)
	//		int Pa=i;
	//		int Pb=i+1;
	//		int Pc=i+WidthSegments+2;
	//		int Pd=i+WidthSegments+1;
	//		//mesh->AddFace(Pc,Pd,Pa); //(CCW)
	//		//mesh->AddFace(Pa,Pd,Pc); //(CW)
	//		//next face
	//		//mesh->AddFace(Pb,Pc,Pa); //(CCW)
	//		//mesh->AddFace(Pa,Pc,Pb); //(CW)
	//		
	//		//wireframe
	//		mesh->AddFace(Pa,Pb,Pc);
	//		mesh->AddFace(Pb,Pc,Pd);
	//	}
	//}
	cout<<"MakePatch vertex count ="<<(lastPointI+1)<<endl;

	mesh->_HasIndexBuffer=false;
	mesh->CreateBuffers();
	//HACK!IndexBuffer* ib = CreateIndexBuffer(WidthSegments,HeightSegments);
	//wire up the index buffer here...
	VertexData* vtxdata = mesh->GetDrawObject()._vertexData;
	vtxdata->_ib=_ib; //HACK! ib would be the local version of _ib (which is right)
	vtxdata->_NumElements=_NumElements;
	mesh->_HasIndexBuffer=true;

	return mesh;
}

//replace XYZ in the base string
std::string MakeTextureTileString(int Z,int X,int Y,const std::string& base)
{
	std::stringstream ss;
	ss<<"../data/BlueMarble/land_ocean_ice_QUAD_"<<Z<<"_"<<X<<"_"<<Y<<".jpg";
	return ss.str();
}

// recursive build all the mesh objects that we need
/// <summary>
/// Build a mesh object for the box passed in the min/max lat/lon coords and recurse down to create an oct tree of more detailed child mesh objects based on the depth
/// </summary>
/// <param name="Depth">Depth to create child LOD chunks down to. When this gets to zero, recursion stops.</param>
/// <param name="WidthSegments">Number of segments around globe</param>
/// <param name="HeightSegments">Number of segments up globe</param>
/// <param name="TileZ">Tile Z (zoom level) number i.e. it goes in the opposite direction to Depth</param>
/// <param name="TileX">Tile X coordinate (origin top left)</param>
/// <param name="TileY">Tile Y coordinate (origin top left)</param>
/// <param name="MinLat">minimum latitude of box in radians</param>
/// <param name="MinLon">minimum longitude of box in radians</param>
/// <param name="MaxLat">maximum latitude of box in radians</param>
/// <param name="MaxLon">maximum longitude of box in radians</param>
/// <returns>The root node of the chunked LOD hierarchy</returns>
Mesh2* TiledEarth::BuildChunkedLOD(int Depth, int WidthSegments, int HeightSegments, int TileZ, int TileX, int TileY, double MinLat, double MinLon, double MaxLat, double MaxLon)
{
	//cout<<"TiledEarth BuildChunkedLOD Depth="<<Depth<<endl;
	//create a mesh at this level 
	Mesh2* mesh = MakePatch(WidthSegments,HeightSegments,MinLat,MinLon,MaxLat,MaxLon);
	//Texture2D* texture = OGLDevice::CreateTexture2DFromFile("../textures/test-blue.jpg");
	Texture2D* texture = OGLDevice::CreateTexture2DFromFile(MakeTextureTileString(TileZ,TileX,TileY,"")); //bit of a hack, but it gets the texture right!!
	texture->SetWrapS(TexClampToEdge);
	texture->SetWrapT(TexClampToEdge);
	texture->SetMinFilter(TexMinFilterLinear);
	texture->SetMagFilter(TexMagFilterLinear);
	mesh->AttachTexture(0,texture);

	if (Depth==0) return mesh; //guard case, we're at the limit of recursion, so don't create any children
	//quadtree recurse
	double LatSplit = (MinLat+MaxLat)/2;
	double LonSplit = (MinLon+MaxLon)/2;
	int NewTileZ=TileZ+1, NewTileX=TileX<<1, NewTileY=TileY<<1;
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, NewTileZ, NewTileX,   NewTileY,   MinLat,   MinLon,   LatSplit, LonSplit) ); //sw
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, NewTileZ, NewTileX,   NewTileY+1, LatSplit, MinLon,   MaxLat,   LonSplit) ); //nw
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, NewTileZ, NewTileX+1, NewTileY+1, LatSplit, LonSplit, MaxLat,   MaxLon) ); //ne
	mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, NewTileZ, NewTileX+1, NewTileY,   MinLat,   LonSplit, LatSplit, MaxLon) ); //se

	//octtree recurse - this splits the longitude into four chunks along the axes
	//double LatSplit = (MinLat+MaxLat)/2;
	//double LonSplit1 = (MinLon+MaxLon)/2; //centre
	//double LonSplit0 = (MinLon+LonSplit1)/2; //left quarter (west)
	//double LonSplit2 = (LonSplit1+MaxLon)/2; //right quarter (east), so lon split goes 012 west to east
	//8 BuildChunkedLOD - it doesn't matter what order you do this in
	//top top (north)
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, MinLon,    MaxLat,   LonSplit0 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit0, MaxLat,   LonSplit1 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit1, MaxLat,   LonSplit2 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, LatSplit, LonSplit2, MaxLat,   MaxLon    ) );
	//bottom row (south)
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   MinLon,    LatSplit, LonSplit0 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit0, LatSplit, LonSplit1 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit1, LatSplit, LonSplit2 ) );
	//mesh->AddChild( BuildChunkedLOD(Depth-1, WidthSegments, HeightSegments, MinLat,   LonSplit2, LatSplit, MaxLon    ) );

	return mesh;
}

//delta halves each recursion
/// <summary>
/// Render using chunked LOD
/// </summary>
/// <param name="GC">Graphics context to render to</param>
/// <param name="sdo">The scene data object containing the camera</param>
/// <param name="mesh">The mesh root to render from now, or recurse if we need a finer mesh</param>
/// <param name="K">The projection constant used to calculate the screen space error from the mesh density</param>
/// <param name="Delta">The mesh density at this level of detail</param>
void TiledEarth::RenderLOD(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo,Mesh2* mesh,float K,float Delta)
{
	//calculate distance from camera to mesh
	//Real formula from globe book is: d=(c-viewer).v - r  (v=view dir vector, c=centre of object, r=radius of object bounds)
	//float D = glm::distance(sdo._camera->GetCameraPos(),mesh->bounds.Centre()); //OK, this is distance to object centre, not nearest point to camera
	//glm::dvec3 c = mesh->bounds.Centre(); //object centre
	//float r = mesh->bounds.Radius(); //object bounding radius
	//glm::dvec4 v = -(sdo._camera->GetCameraMatrix())[2]; //view direction
	//glm::dvec3 vp = sdo._camera->GetCameraPos(); //viewpoint
	//float D = glm::dot((c-vp),glm::dvec3(v))-r;
	//if (D<0) cout<<"D="<<D<<endl;
	//method 3
	//float r = mesh->bounds.Radius(); //object bounding radius
	////cout<<"R="<<r<<endl;
	//float D = glm::distance(sdo._camera->GetCameraPos(),mesh->bounds.Centre()) - r; //distance between viewpoint and object centre minus object radius
	////if (D<0) cout<<"D="<<D<<" R="<<r<<endl;
	//if (D<0) D=0; //we're inside the bounding radius of the object
	//method 4 - use the distance to the centre of the object (same as method one)
	float D = glm::distance(sdo._camera->GetCameraPos(),mesh->bounds.Centre()); //OK, this is distance to object centre, not nearest point to camera
	float Rho=Delta/D*K;
	if ((Delta<=_MinDelta)||(Rho<=_Tau)) { //Delta<MinDelta at limit of recursion, or current error is<Tau
		const DrawObject& dobj = mesh->GetDrawObject();
		ShaderUniformCollection* uniforms=dobj._ShaderProgram->_shaderUniforms;
//TODO: you need to set these uniforms to scale the texture
		(*uniforms)["u_texOffset"]=glm::vec2(0,0);
		(*uniforms)["u_texScale"]=glm::vec2(1,1);
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

void TiledEarth::AttachTexture(unsigned int TextureUnitNum, gengine::Texture2D* Texture)
{
	_root->AttachTexture(TextureUnitNum,Texture);
}

