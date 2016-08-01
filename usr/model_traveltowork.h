#pragma once

/*
 * model_traveltowork.h
 *
 *  Created on: 23 Jan 2016
 *      Author: richard
 */

#include "main.h"

#include <string>
#include <vector>
#include <random>

#include "abm/Model.h"
#include "abm/agenttime.h"

class ODData {
public:
	std::string Origin;
	std::string Destination;
	int Count;
	ODData(std::string Origin, std::string Destination, int Count) : Origin(Origin), Destination(Destination), Count(Count) {};
};

class ModelTravelToWork : public ABM::Model {
public:
	static const std::string Filename_MSOANodes; //csv file containing msoa node names and their lat lon locations
	static const std::string Filename_TravelToWorkOD; //travel to work origin destination file

	int NumberOfAgents = 100;
	int TotalTripCount=0;

	ModelTravelToWork(SceneGraphType* SceneGraph);
	virtual ~ModelTravelToWork();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
private:
	//setup random number generators
	//std::uniform_int_distribution<int> uniform_trips;
	std::default_random_engine e;

	std::vector<ODData> trips;
	void CreateRandomJourney();
};
