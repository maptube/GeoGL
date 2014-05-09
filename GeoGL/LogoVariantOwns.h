#pragma once

#include <unordered_map>
#include <string>

namespace ABM {

	//This is a big problem because Agent has a base class of LogoVariantOwns, Link has a base class of LogoVariantOwns, but both require Agent and Link.
	//Everything is just a big circular reference, so used Model.h to hold the correct order of includes and forward declared Agent and Link where required.
	class Agent;
	class Link;

	//TODO: need a variant that I can create with a specific type e.g.
	//LogoVariant<string>("mystring");

//http://www.ojdip.net/2013/10/implementing-a-variant-type-in-cpp/
	//simple method, just union it
	struct LogoVariant {
		enum {t_int, t_bool, t_float, t_string, t_pAgent, t_pLink} type_id;
		union {
			int as_integer;
			bool as_bool;
			float as_float;
			Agent* as_pAgent;
			Link* as_pLink;
		};
		std::string as_string; //this is really nasty!
		
		////////////////////////////////////////////////////////////////////

		//default constructor
		LogoVariant() {};
		
		//int constructor
		LogoVariant(int val) {
			type_id=t_int;
			as_integer=val;
		};
		//bool constructor
		LogoVariant(bool b) {
			type_id=t_bool;
			as_bool=b;
		};
		//float constructor
		LogoVariant(float f) {
			type_id=t_float;
			as_float=f;
		};
		//string constructor
		LogoVariant(std::string str) {
			type_id=t_string;
			as_string=str;
		};
		//Agent* constructor
		LogoVariant(Agent* pAgent) {
			type_id=t_pAgent;
			as_pAgent=pAgent;
		};
		//Link* constructor
		LogoVariant(Link* pLink) {
			type_id=t_pLink;
			as_pLink=pLink;
		};
		//end of constructors

		//////////////////////////////////////////////////////////////////

		//equality test
		bool operator==(LogoVariant& var) {
			if (type_id!=var.type_id) return false;
			switch (type_id) {
			case t_int : return as_integer==var.as_integer;
			case t_bool: return as_bool==var.as_bool;
			case t_float: return as_float==var.as_float;
			case t_string: return as_string==var.as_string;
			case t_pAgent: return as_pAgent==var.as_pAgent;
			case t_pLink: return as_pLink==var.as_pLink;
			}
			return false;
		}
	};

/// <summary>
/// Forms base class for Link and Agent so that they can implement a get and set interface for adding properties to agents and links
/// </summary>
/*class LogoVariantOwns
{
public:
	LogoVariantOwns(void);
	~LogoVariantOwns(void);

	//ownership interface
	std::unordered_map<std::string,LogoVariant> _owns; //key value pairs
	template <typename T>
	void Set(std::string Name, T Value) { };
	template <typename T>
	T Get(std::string Name) { return 42; };
	
	//void SetV(std::string Name, int Value) {};
};*/

}