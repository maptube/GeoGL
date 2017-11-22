/*
 * naivebayes.h
 *
 *  Created on: 1 Feb 2017
 *      Author: richard
 */

#pragma once

#include <string>
#include <unordered_map>

namespace GeoGL {
namespace dm {

/// <summary>
/// Contains the relationship between a pair of attributes. The table is the count of when they occur together while the
/// totals is the count for the individual key.
/// The way to read the table is that you look up table(hypothesis), then look up (evidence)
/// i.e. <hypothesis,<evidence,count>>
/// </summary>
struct NBHistogram {
	std::unordered_map<std::string,std::unordered_map<std::string,int>> table;
	std::unordered_map<std::string,int> HypothesisTotals; //separate totals for each hypothesis value
	int HypothesisTotal; //this is the total of everything in HypothesisTotals (above) and used to calculate the hypothesis probability
};

/// <summary>
/// Naive Bayes classifier. This is intended to be used online.
/// TODO: at the moment this is only using a single pair of evidence and hypothesis. Need to include multiple attributes.
/// </summary>
class NaiveBayes {
protected:
	NBHistogram histogram; //you would have one for every pair combination
public:
	NaiveBayes();
	virtual ~NaiveBayes();

	void Add(std::string Hypothesis, std::string Evidence);
	float Test(std::string Hypothesis, std::string Evidence);
};

} // namespace dm
} // namespace GeoGL
