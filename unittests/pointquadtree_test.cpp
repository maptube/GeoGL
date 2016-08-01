/*
 * pointquadtree_test.cpp
 *
 *  Created on: 29 Dec 2015
 *      Author: richard
 */

#include "pointquadtree_test.h"

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <random>

#include "gengine/gengine.h"
#include "algorithms/index/pointquadtree.h"
#include "gis/envelope2d.h"


int main(int argc, const char* argv[]) {
	std::cout<<"PointQuadTreeTest"<<std::endl;

	algorithms::index::PointQuadTree* pquad = new algorithms::index::PointQuadTree();
	//std::shared_ptr<std::string> test = std::make_shared<std::string>("test text");
	//std::shared_ptr<void> test = std::make_shared<void>();

	//todo: obviously, you need to add some points here...
	pquad->insert(glm::dvec2(0,0),std::make_shared<std::string>("first point"));
	pquad->insert(glm::dvec2(1,1),std::make_shared<std::string>("second point"));
	pquad->insert(glm::dvec2(0.5,0.5),std::make_shared<std::string>("third point"));
	pquad->insert(glm::dvec2(-2,-2),std::make_shared<std::string>("fourth point"));
	pquad->insert(glm::dvec2(-3,-3),std::make_shared<std::string>("fifth point"));
	std::cout<<"after insert"<<std::endl;

	std::cout<<"Point Count="<<pquad->size()<<std::endl;

	std::cout<<"Query region (0,0) (2,2)"<<std::endl;
	pquad->query(gis::Envelope2D(0,2,0,2));

	std::cout<<"Query region (-2,-2) (0,0)"<<std::endl;
	pquad->query(gis::Envelope2D(-2,0,-2,0));

	std::cout<<"Remove (-2,-2)"<<std::endl;
	pquad->remove(glm::dvec2(-2,-2));
	std::cout<<"Point Count="<<pquad->size()<<std::endl;

	//pquad->debug_printtree();

	delete pquad;

	//real tests
	unittests::PointQuadTreeTest::InsertTest();
}

namespace unittests {

PointQuadTreeTest::PointQuadTreeTest() {
	// TODO Auto-generated constructor stub

}

PointQuadTreeTest::~PointQuadTreeTest() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// Insert test which runs insert on n=0 up to n=100000 random points and times it. A new pointquadtree is constructed for each test.
/// X and Y points are picked from uniform random distributions between -1 and 1.
/// </summary>
void PointQuadTreeTest::InsertTest() {
	//create two uniform double distributions to pick the x and y insertion points from
	std::uniform_real_distribution<double> uniform_x {-1.0,1.0}; //uniform distribution of doubles in [-1..1]
	std::uniform_real_distribution<double> uniform_y {-1.0,1.0}; //uniform distribution of doubles in [-1..1]
	std::default_random_engine e;

	for (int n=0; n<=100000; n+=100)
	{
		//create the quadtree index structure
		algorithms::index::PointQuadTree* pquad = new algorithms::index::PointQuadTree();
		std::shared_ptr<std::string> data = std::make_shared<std::string>("point data"); //data to put into the index so we're not timing the creation of this

		auto t1=std::chrono::high_resolution_clock::now();

		//this is the bit we're timing
		for (int i=0; i<=n; i++)
		{
			double x = uniform_x(e);
			double y = uniform_y(e);
			pquad->insert(glm::dvec2(x,y),data);
		}
		//end of timed section

		auto t2=std::chrono::high_resolution_clock::now();
		auto dms=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
		std::cout<<"Insert,n=,"<<n<<","<<" took, "<<dms.count()<<", milliseconds"<<std::endl;

		delete pquad;
	}

}

} // namespace unittests
