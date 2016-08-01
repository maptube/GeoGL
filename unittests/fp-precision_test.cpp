#include <iostream>
#include <iomanip>

int main(int argc, const char* argv[]) {
	std::cout<<"Floating Point Precision Test"<<std::endl;

	//single precision
	std::cout<<"Single precision"<<std::endl;
	float fearth = 6378137.0f;
	float fdelta=0.1f;
	float flastgood=10.0f;
	for (int i=0; i<1000; i++) {
		float fearth2 = fearth+fdelta;
		if (fearth2>fearth) {
			std::cout<<std::setprecision(26)<<"32 Greater: "<<fearth2<<" "<<fdelta<<std::endl;
			float temp=fdelta;
			fdelta=(flastgood-fdelta)/2;
			flastgood=temp;
		}
		else {
			std::cout<<std::setprecision(26)<<"32 Less: "<<fearth2<<" "<<fdelta<<std::endl;
			fdelta=(flastgood+fdelta)/2;
		}
		//stopping condition?
	}

	//double precision
	std::cout<<"Double precision"<<std::endl;
	double dearth = 6378137.0;
	double ddelta=0.1;
	double dlastgood=10;
	for (int i=0; i<1000; i++) {
		double dearth2 = dearth+ddelta;
		if (dearth2>dearth) {
			std::cout<<std::setprecision(26)<<"64 Greater: "<<dearth2<<" "<<ddelta<<std::endl;
			double temp=ddelta;
			ddelta=(dlastgood-ddelta)/2;
			dlastgood=temp;
		}
		else {
			std::cout<<std::setprecision(26)<<"64 Less: "<<dearth2<<" "<<ddelta<<std::endl;
			ddelta=(dlastgood+ddelta)/2;
		}
		//stopping condition?
	}
}
