/*
 * triangulator.cpp
 *
 *  Created on: 11 Jun 2014
 *      Author: richard
 */

#include "triangulator.h"

#include <iostream>

#include "pathshapes.h"
#include "mesh2.h"
#include "poly2tri.h"
#include "clipper/clipper.hpp"

using namespace std;

Triangulator::Triangulator() {
	// TODO Auto-generated constructor stub

}

Triangulator::~Triangulator() {
	// TODO Auto-generated destructor stub
}

void Triangulator::Clear(void) {
	_Shape.outer.clear();
	_Shape.inner.clear();
}

void Triangulator::SetOuterRing(const LinearRing& Outer) {
	_Shape.outer=Outer;
}

void Triangulator::AddInnerRing(const LinearRing& Inner) {
	_Shape.inner.push_back(Inner);
}

void Triangulator::SetShape(const PathShape& Shape) {
	_Shape=Shape;
}

void Triangulator::Triangulate(Mesh2& geom) {
	//use Clipper to do the polygon cleaning: http://www.angusj.com/delphi/clipper.php
	//use poly2tri to do the triangulation see: http://threejsdoc.appspot.com/doc/three.js/src.source/extras/core/Shape.js.html
	ClipperLib::Clipper clipper;
	ClipperLib::Paths linearrings;

	ClipperLib::Path outerring;
	unsigned int count=_Shape.outer.size();
	for (unsigned int p=0; p<count; p++) { //leave off last point as it's a duplicate TODO: need to check for this
		glm::vec3 P = _Shape.outer[p];
		double x = (double)P.x;
		double y = (double)P.y;
		int xi = (int)(x*10000000);
		int yi = (int)(y*10000000);
		ClipperLib::IntPoint pi(xi,yi);
		//cout<< x <<","<< y <<" "<<endl;
		outerring.push_back(pi);
	}
	linearrings.push_back(outerring);

	int ring_i=1;
	for (vector<LinearRing>::iterator ringIT=_Shape.inner.begin(); ringIT!=_Shape.inner.end(); ++ringIT) {
		const LinearRing& ring = *ringIT;
		//cout<<"Ring "<<ring_i<<" size="<<ring.size()<<endl;
		ClipperLib::Path linearring; //it's a vector of clipper int points
		unsigned int count=ring.size();
		for (unsigned int p=0; p<count; p++) { //leave off last point as it's a duplicate TODO: need to check for this
			glm::vec3 P = ring[p];
			double x = (double)P.x;
			double y = (double)P.y;
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
	for (unsigned int i=0; i<linearrings.size(); i++) {
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
		//cout<<"Solutions: size="<<solPath.size()<<endl;
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
	for (unsigned int i=1; i<p2t_linearrings.size(); i++) {
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
			//glm::vec3 P1 = _pellipsoid->toVector(glm::radians(v1->x),glm::radians(v1->y)); //don't forget to convert to radians for the ellipse!
			//glm::vec3 P2 = _pellipsoid->toVector(glm::radians(v2->x),glm::radians(v2->y));
			//glm::vec3 P3 = _pellipsoid->toVector(glm::radians(v3->x),glm::radians(v3->y));
			glm::vec3 P1(v1->x,v1->y,0);
			glm::vec3 P2(v2->x,v2->y,0);
			glm::vec3 P3(v3->x,v3->y,0);
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

