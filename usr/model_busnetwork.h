/*
 * model_busnetwork.h
 *
 *  Created on: 4 May 2015
 *      Author: richard
 */

#pragma once

#include "main.h"
#include <string>

#include "abm/Model.h"
#include "abm/agenttime.h"

class ModelBusNetwork : public ABM::Model {
public:
	static const std::string Filename_StopCodes; //bus stop locations
	//todo: these were obviously from the tube network
	static const std::string Filename_BusRoutesNetwork; //bus network from list of stops
	static const std::string Filename_Positions; //bus position data (location csv file)
	static const std::string LogFilename; //name of log file where results get written to
	//static const std::string Filename_AnimationDir; //directory containing animation data as csv files
	//static const float LineSize; //size of track
	//static const int LineTubeSegments; //number of segments making up the tube geometry
	//static const float StationSize; //size of station geometry object
	//static const float TrainSize; //size of train geometry object

	AgentTime AnimationDT; //current animation time
	AgentTime NextTimeDT; //next animation frame time, on 3 min boundary
	AgentTime SensorRecordStartDT; //record when we started the bus count sensor

	ModelBusNetwork(SceneGraphType* SceneGraph);
	virtual ~ModelBusNetwork();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);

protected:
	void LoadBusStops(const std::string& Filename);
	void LoadLinks(const std::string& RoutesFilename);
	void UpdateAgents(const AgentTime& DateTime);
	void WriteLogBusNumbers(const AgentTime& DateTime);
	void WriteLogBusGeoFencedNumbers();

};

