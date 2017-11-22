/*
 * naivebayes.cpp
 *
 *  Created on: 1 Feb 2017
 *      Author: richard
 */

#include "naivebayes.h"

namespace GeoGL {
namespace dm {

NaiveBayes::NaiveBayes() {
	// TODO Auto-generated constructor stub

	histogram();
}

NaiveBayes::~NaiveBayes() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// In this example, Evidence is the TfL Route and Hypothesis is the station code.
/// This dictates how you can look up a probability, so you can test for P(H|E), which in this case
/// is the probability that the next station is OXC based on the route code 123. Then you test the other possible
/// link e.g. ABC with route 123 and see which is higher.
/// </summary>
void NaiveBayes::Add(std::string Hypothesis, std::string Evidence)
{
	//Remember, this is used to lookup P(E|H) later
	auto itH=histogram.table.find(Hypothesis);
	if (itH==histogram.table.end()) {
		//Hypothesis value not seen before, so create new
		std::unordered_map<std::string,int> newE;
		newE.insert(std::pair<std::string,int>(Evidence,1));
		histogram.table.insert(std::pair<std::string,std::unordered_map<std::string,int>>(Hypothesis,newE));
		histogram.HypothesisTotals[Hypothesis]=1; //create a count of one for this new hypothesis value (global count later)
	}
	else {
		//Hypothesis seen before, so updating existing
		++histogram.HypothesisTotals[Hypothesis]; //update the count for this hypothesis value (global count later)
		//Test for Evidence value
		auto itE=itH->second.find(Evidence);
		if (itE==itH->second.end()) {
			//Evidence value not seen before, so create new
			itH->second[Evidence]=1;
		}
		else {
			//Evidence value exists, so update
			++(itH->second[Evidence]);
		}
	}
	++histogram.HypothesisTotal; //this is the global total
}

/// <summary>
/// Test the hypothesis based on the evidence and return a probability.
/// P(H|E)=P(E|H)*P(H)/P(E)
/// </summary>
/// <returns>
/// The Bayesian probability of the event (hypothesis) based on the evidence provided.
/// </returns>
float NaiveBayes::Test(std::string Hypothesis, std::string Evidence)
{
	//P(H|E)=P(E|H)*P(H)/P(E)
	float pEH = 0; //P(E|H)
	float pH = 0; //P(H)
	auto itH = histogram.table.find(Hypothesis); //you have to have an H and an E, otherwise P(E|H)=0
	if (itH!=histogram.table.end()) {
		auto itE = itH->second.find(Evidence);
		if (itE!=itH->second.end()) {
			//if we get to here, then the result is a valid probability, otherwise either the hypothesis or evidence value doesn't exist and the result will always be zero
			int cEH=*itE; //count for E|H
			int cH=histogram.HypothesisTotals[Hypothesis]; //we know it's in here as itH!=end() proved the Hypothesis existed
			pEH=(float)cEH/(float)cH; //this is P(E|H)
			pH=(float)cH/(float)histogram.HypothesisTotal; //and this is P(H)
		}
	}
	return pEH*pH; //NOTE the divided by P(E) value is factored out in the comparisons between Test using a different hypothesis value to see which is the greater
}

} // namespace dm
} // namespace GeoGL
