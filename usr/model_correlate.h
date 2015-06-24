/*
 * model_correlate.h
 *
 *  Created on: 19 May 2015
 *      Author: richard
 *  This is a non-visual model to load and correlate bus data with tube data for a whole year.
 */

#pragma once

#include "main.h"
#include <string>
#include <unordered_map>

#include "abm/Model.h"
#include "abm/agenttime.h"

class ModelCorrelate : public ABM::Model {
public:
	static const std::string Filename_BusPositions; //bus position data (location csv file)
	static const std::string Filename_TubePositions; //bus position data (location csv file)

	//char TubeLines[] = { 'B', 'C', 'D', 'H', 'J', 'M', 'N', 'P', 'V' }; //skipped Waterloo and City
	//enum TubeLines { B, C, D, H, J, M, N, P, V };

	AgentTime AnimationDT; //current animation time (shouldn't this be on the model?)
	int TubeNumbers[480];
	std::unordered_map<char,int[480]> TubeNumbersLine;
	int BusNumbers[480];

	ModelCorrelate(SceneGraphType* SceneGraph);
	virtual ~ModelCorrelate();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
protected:
	void LoadTubePositions(const std::string& Filename);
	void LoadBusPositions(const std::string& Filename);
};

