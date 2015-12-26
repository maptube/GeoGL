/*
 * model_wxairq.h
 *
 *  Created on: 21 Jul 2015
 *      Author: richard
 */

#pragma once

#include "main.h"
#include <string>

#include "abm/Model.h"
#include "abm/agenttime.h"

class ModelWXAirQ : public ABM::Model {
public:
	static const std::string Filename_WXData; //location of weather data
	static const std::string Filename_AirQData; //location of air quality data
	static const std::string Filename_AirQStationList; //location of file containing lat lon locations for airquality stations
	static const std::string LogFilename; //name of log file where results get written to

	AgentTime AnimationDT; //current animation time
	//AgentTime NextTimeDT; //next animation frame time, on 3 min boundary

	ModelWXAirQ(SceneGraphType* SceneGraph);
	virtual ~ModelWXAirQ();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);

protected:
	//std::string MatchFilePattern(std::string BaseDir, std::string);
private:
	void StartLogWXHRLHIL();
	void WriteLogWXHRLHIL(std::string TimeCode);
	void WriteLogLondonBoxNO2Hourly(std::string TimeCode);
	void WriteLogLondonBoxNO2(std::string TimeCode);
};


