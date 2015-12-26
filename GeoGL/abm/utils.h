/*
 * utils.h
 *
 *  Created on: 14 Jun 2015
 *      Author: richard
 */

#pragma once

namespace ABM {

//forward declarations
class Agent;


class Utils {
public:
	Utils();
	virtual ~Utils();

	static void LinearInterpolate(Agent* A, float Amount, /*const*/ Agent* From, /*const*/ Agent* To);
};

} // namespace ABM
