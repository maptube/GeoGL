/*
 * quadtree.cpp
 *
 *  Created on: 27 Dec 2015
 *      Author: richard
 */

#include "pointquadtree.h"

#include <iostream>
#include <memory>

#include "gis/envelope2d.h"

namespace algorithms {

namespace index {

PointQuadTree::PointQuadTree() {
	// TODO Auto-generated constructor stub

	//root = std::make_shared<PointQuadTreeNode>();
	//root->isLeaf=true;
	root = makeNode(true);
}

PointQuadTree::~PointQuadTree() {
	// TODO Auto-generated destructor stub
	//the tree is destroyed automatically - it's reference counted
}

//void traverse(std::shared_ptr<QuadTreeNode> node) {
//
//}

/// <summary>
/// Builds a quadtree node
/// </summary>
std::shared_ptr<PointQuadTreeNode> PointQuadTree::makeNode(bool isLeaf) {
	std::shared_ptr<PointQuadTreeNode> node = std::make_shared<PointQuadTreeNode>();
	node->isLeaf=isLeaf;
	node->ne=nullptr;
	node->se=nullptr;
	node->sw=nullptr;
	node->nw=nullptr;
	//node.data?
	return node;
}

/// <summary>
/// Return a count of the number of points stored in this structure
/// </summary>
int PointQuadTree::size() {
	if (isEmpty) return 0;
	return size(root);
}

int PointQuadTree::size(std::shared_ptr<PointQuadTreeNode> node) {
	if (node==nullptr) return 0; //guard case

	std::cout<<"size: isLeaf="<<node->isLeaf<<" x="<<node->point.x<<" y="<<node->point.y<<std::endl;
	//if (node->isLeaf) return 1; //node->data.size();
	int count=1;
	count+=size(node->ne);
	count+=size(node->se);
	count+=size(node->sw);
	count+=size(node->nw);
	return count;
}

/// <summary>
/// Print the contents of the quadtree structure to the console for debugging purposes.
/// <summary>
void PointQuadTree::debug_printtree() {
	std::cout<<"root"<<std::endl;
	debug_printtree(0,root);
}

void PointQuadTree::debug_printtree(int level,std::shared_ptr<PointQuadTreeNode> node) {
	if (node==nullptr) return; //guard case
	for (int i=0; i<level; i++) std::cout<<".."; //spacing
	std::cout<<"Node ("<<node->point.x<<","<<node->point.y<<")"<<std::endl;
	//now the children
	for (int i=0; i<level+1; i++) std::cout<<"  "; //spacing
	std::cout<<"ne: "<<std::endl;
	debug_printtree(level+1,node->ne);

	for (int i=0; i<level+1; i++) std::cout<<"  "; //spacing
	std::cout<<"se: "<<std::endl;
	debug_printtree(level+1,node->se);

	for (int i=0; i<level+1; i++) std::cout<<"  "; //spacing
	std::cout<<"sw: "<<std::endl;
	debug_printtree(level+1,node->sw);

	for (int i=0; i<level+1; i++) std::cout<<"  "; //spacing
	std::cout<<"nw: "<<std::endl;
	debug_printtree(level+1,node->nw);

}

/// <summary>
/// Insert data with the given bounding box into the quadtree index
/// TODO: need to add the duplicate point case
/// </summary>
/// <param name="P">Position of point to insert</param>
/// <param name="data">Payload containing the actual data to be stored, which is retrieved with a geographic query</param>
void PointQuadTree::insert(glm::dvec2 P,std::shared_ptr<void> data) {
	//std::cout<<"PointQuadTree::insert begin "<<P.x<<","<<P.y<<std::endl;

	//trivial case where this is the first point to be added
	if (isEmpty) {
		root->isLeaf=true;
		root->point=P;
		root->data=data;
		isEmpty=false;
		return;
	}

	std::shared_ptr<PointQuadTreeNode> node = root;
	/*std::shared_ptr<PointQuadTreeNode>* dir = nullptr;
	do { //so a leaf node is also where ne==nw==se==sw==null
		//traversal logic, P is the point we're adding
		//std::cout<<"top"<<std::endl;
		std::cout<<"PointQuadTree::insert test node: isLeaf="<<node->isLeaf<<" x="<<node->point.x<<" y="<<node->point.y<<std::endl;
		if (P.x>=node->point.x) {
			//east
			if (P.y>=node->point.y) { //ne
				dir = &(node->ne);
				std::cout<<"PointQuadTree::insert traverse ne"<<std::endl;
			}
			else { //se
				dir = &(node->se);
				std::cout<<"PointQuadTree::insert traverse se"<<std::endl;
			}
		}
		else {
			//west
			if (P.y>=node->point.y) { //nw
				dir = &(node->nw);
				std::cout<<"PointQuadTree::insert traverse nw"<<std::endl;
			}
			else { //sw
				dir = &(node->sw);
				std::cout<<"PointQuadTree::insert traverse sw"<<std::endl;
			}
		}
		if ((*dir)!=nullptr) node=*dir;
		//std::cout<<"after block"<<std::endl;
		//std::cout<<"node="<<node->point.x<<" "<<node->point.y<<std::endl;
		//std::cout<<"*dir="<<*dir<<std::endl;
	} while ((*dir)!=nullptr);*/
	std::shared_ptr<PointQuadTreeNode>* dir = traverse(root,P);

	//std::cout<<"PointQuadTree::insert Node found"<<std::endl;

	//OK, found the right leaf node, so we can add the new data
	node->isLeaf=false;
	*dir = makeNode(true);
	(*dir)->point = P;
	(*dir)->data = data;
	//std::cout<<"PointQuadTree::insert end"<<std::endl;
}

/// <summary>
/// Traverse from the starting node to where point P is, or will be located.
/// Used by insert and delete for finding parent nodes.
/// </summary>
/// <returns>A pointer to the ne|se|sw|nw link pointing to the child node containing point P.</returns>
std::shared_ptr<PointQuadTreeNode>* PointQuadTree::traverse(std::shared_ptr<PointQuadTreeNode> node, glm::dvec2 P) {
	std::shared_ptr<PointQuadTreeNode>* dir = nullptr;
	do { //so a leaf node is also where ne==nw==se==sw==null
		//traversal logic, P is the point we're adding, or looking for
		if (P==node->point) break; //duplicate point (insert), or found the point we're looking for (remove)
		//std::cout<<"top"<<std::endl;
		//std::cout<<"PointQuadTree::traverse test node: isLeaf="<<node->isLeaf<<" x="<<node->point.x<<" y="<<node->point.y<<std::endl;
		if (P.x>=node->point.x) {
			//east
			if (P.y>=node->point.y) { //ne
				dir = &(node->ne);
				//std::cout<<"PointQuadTree::traverse traverse ne"<<std::endl;
			}
			else { //se
				dir = &(node->se);
				//std::cout<<"PointQuadTree::traverse traverse se"<<std::endl;
			}
		}
		else {
			//west
			if (P.y>=node->point.y) { //nw
				dir = &(node->nw);
				//std::cout<<"PointQuadTree::traverse traverse nw"<<std::endl;
			}
			else { //sw
				dir = &(node->sw);
				//std::cout<<"PointQuadTree::traverse traverse sw"<<std::endl;
			}
		}
		if ((*dir)!=nullptr) node=*dir;
	} while ((*dir)!=nullptr);

	return dir;
}

/// <summary>
/// Insert a tree of quadtree nodes into the existing tree by walking the "node" tree and calling insert on every item. In other words, a merge.
/// Used after delete node to reconnect the child nodes of the deleted one. Not computationally efficient, but there's no other way of doing it.
/// </summary>
void PointQuadTree::insert_quadtree(std::shared_ptr<PointQuadTreeNode> node) {
	if (node==nullptr) return; //guard case
	insert(node->point, node->data);
	//and now the children
	insert_quadtree(node->ne);
	insert_quadtree(node->se);
	insert_quadtree(node->sw);
	insert_quadtree(node->nw);
}

/// <summary>
/// Remove the specified point from the quadtree. The only way of doing this is to find the point and remove all the child nodes, which is computationally expensive.
/// </summary>
void PointQuadTree::remove(glm::dvec2 P) {
	//Find point, disconnect tree, go through child tree and add the disconnected nodes back on again
	std::shared_ptr<PointQuadTreeNode>* dir = traverse(root,P);
	//std::cout<<"PointQuadTree::remove found point "<<(*dir)->point.x<<" "<<(*dir)->point.y<<std::endl;
	//get a local copy of the node that we're going to delete as we need it's out links to reconnect
	std::shared_ptr<PointQuadTreeNode> node = *dir;
	//disconnect the tree at the removal node
	*dir = nullptr;
	//and reconnect all the disconnected nodes - NOT node itself, which is the one we want to delete!
	insert_quadtree(node->ne);
	insert_quadtree(node->se);
	insert_quadtree(node->sw);
	insert_quadtree(node->nw);
}

/// <summary>
/// Remove point by data value.
/// TODO: NOT IMPLEMENTED
/// </summary>
void PointQuadTree::remove(std::shared_ptr<void> data) {

}

/// <summary>
/// Region query to return all points within the bounds
/// </summary>
/// <param name="bounds">Find all the points within the bounds of this Envelope</param>
void PointQuadTree::query(gis::Envelope2D bounds) {
	//todo: is this query or search? It's region search in the Finkel/Bentley paper or Knuth "simple query" and "region query" (also "boolean query") from sorting and searching
	if (isEmpty) return;
	query(bounds,root);
}

void PointQuadTree::query(gis::Envelope2D bounds, std::shared_ptr<PointQuadTreeNode> node) {
	if (node==nullptr) return; //guard case
	if ((bounds.width()<=0)||(bounds.height()<=0)) return; //second guard case
	if (bounds.Contains(node->point)) std::cout<<"Point: "<<node->point.x<<","<<node->point.y<<std::endl;
	//TODO: you could preempt the query here by checking the point against the bounds first
	if (node->ne!=nullptr) query(gis::Envelope2D( node->point.x, bounds.max.x,  node->point.y, bounds.max.y  ), node->ne);
	if (node->se!=nullptr) query(gis::Envelope2D( node->point.x, bounds.max.x,  bounds.min.y,  node->point.y ), node->se);
	if (node->sw!=nullptr) query(gis::Envelope2D( bounds.min.x,  node->point.x, bounds.min.y,  node->point.y ), node->sw);
	if (node->nw!=nullptr) query(gis::Envelope2D( bounds.min.x,  node->point.x, node->point.y, bounds.max.y  ), node->nw);
}

} //namespace index
} //namespace algorithms

