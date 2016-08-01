#pragma once

/*
 * quadtree.h
 *
 *  Created on: 27 Dec 2015
 *      Author: richard
 *
 *  QuadTree spatial index
 *  Original paper: Quad Trees A Data Structure for Retrieval on Composite Keys, R.A. Finkel and J. L. Bentley, Acta Informatica Volume 4, 1-9 (1974), Springer-Verlag 1974
 *  see: http://szmoore.net/ipdf/documents/references/finkle1974quad.pdf
 *  Analysis in Knuth, sorting and searching Ch6.5 pp565, retrieval on secondary keys
 *  see: http://www.cs.umd.edu/~hjs/pubs/Samettfcgc88-ocr.pdf
 *  https://www.cs.umd.edu/users/meesh/cmsc420/Notes/PMQuadtree/pm_quadtree_samet.pdf
 *  NOTE: PR quadtree = point region quadtree
 *  A point quadtree splits on the point locations and does not need to know the maximum extents. The point region (PR) quadtree uses fixed maximum limits and splits these limits recursively in four at
 *  each level. The PR quadtree is more like an axis aligned bounding box (AABB) idea as it is fixed in space.
 *  Variations include storing more than one data item in a leaf, but you need to keep each item's bounding box if you want to be able to split the node later
 *
 *  Good walk through of how variants work:
 *  http://web.eecs.utk.edu/~cphillip/cs594_spring2014/quadtree-Allan.pdf
 */

#include <vector>
#include <memory>

//Forward declarations
#include "gis/envelope2d.h"
//namespace gis {
//	class Envelope2D;
//}

//End of forward declarations

namespace algorithms {

namespace index {

//struct PointQuadTreePayload {
//	glm::dvec2 point;
//	std::shared_ptr<void> data;
//};

struct PointQuadTreeNode {
	bool isLeaf;
	std::shared_ptr<PointQuadTreeNode> nw,ne,se,sw;
	//glm::dvec2 splitPoint;
	//std::vector<std::unique_ptr<PointQuadTreePayload>> data;
	glm::dvec2 point;
	std::shared_ptr<void> data;
};

class PointQuadTree {
private:
	//int capacity = 1; //capacity of a leaf node - max number of data items stored on each leaf
	bool isEmpty = true;
	//the data
	std::shared_ptr<PointQuadTreeNode> root;
protected:
	std::shared_ptr<PointQuadTreeNode>* traverse(std::shared_ptr<PointQuadTreeNode> node, glm::dvec2 P);
	std::shared_ptr<PointQuadTreeNode> makeNode(bool isLeaf);
	int size(std::shared_ptr<PointQuadTreeNode> node);
	void query(gis::Envelope2D bounds, std::shared_ptr<PointQuadTreeNode> node);
	void debug_printtree(int level,std::shared_ptr<PointQuadTreeNode> node);
public:
	PointQuadTree();
	virtual ~PointQuadTree();

	//TODO: want this to be an interface - how does stl do it?
	int size();
	void insert(glm::dvec2 P,std::shared_ptr<void> data);
	void insert_quadtree(std::shared_ptr<PointQuadTreeNode> node);
	void remove(glm::dvec2 P);
	void remove(std::shared_ptr<void> data);
	void query(gis::Envelope2D bounds);
	//void all(); //return all points?

	//count points
	//count nodes
	void debug_printtree();
};

//class PRQuadtree ?

} //namespace index
} //namespace algorithms
