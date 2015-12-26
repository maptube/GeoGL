/*
 * utils.cpp
 *
 *  Created on: 14 Jun 2015
 *      Author: richard
 */

#include "utils.h"

#include "../gengine/gengine.h"
#include "../abm/Agent.h"

namespace ABM {

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// Linearly interpolate between the From agent and the To agent, using Amount [0..1].
/// If Amount =0, then Agent A is placed at From. If Amount=1, then Agent A is placed at To.
/// If Amount=0.75, then A is placed 75% of the distance along the line From->To.
/// <summary>
/// <param name="A">The agent whose position is about to be altered.</param>
/// <param name="Amount">The Amount along the line From->To to place agent A. Amount=[0..1], although you can exceed this in either direction to go beyond From and To.</param>
/// <param name="From">The starting point of the line to place A on.</param>
/// <param name="To">The end point of the line to place A on.</param>
void Utils::LinearInterpolate(Agent* A, float Amount, /*const*/ Agent* From, /*const*/ Agent* To) {
	glm::dvec3 P0=From->GetXYZ();
	glm::dvec3 P1=To->GetXYZ();
	glm::dvec3 delta = P1-P0;
	double Lambda = Amount/glm::length(delta);
	glm::dvec3 Pa=P0+Lambda*delta;
	A->SetXYZ(Pa.x,Pa.y,Pa.z);
}

} // namespace ABM
