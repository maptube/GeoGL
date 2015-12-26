/*
 * model_networkrail.h
 *
 *  Created on: 6 Jun 2015
 *      Author: richard
 */

#pragma once

#include <string>

#include "scenegraph.h"
#include "abm/Model.h"
#include "abm/agenttime.h"


class ModelNetworkRail : public ABM::Model {
public:
	static const std::string Filename_TrainPositions; //train position data (location csv file)

	AgentTime AnimationDT; //current animation time (shouldn't this be on the model?)

	ModelNetworkRail(SceneGraphType* SceneGraph);
	virtual ~ModelNetworkRail();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
protected:
	void LoadTrainPositions(const std::string& Filename);
};

