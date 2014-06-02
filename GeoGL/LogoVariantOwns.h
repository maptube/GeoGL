#pragma once

//#include <unordered_map>
#include <string>

namespace ABM {

	//This is a big problem because Agent has a base class of LogoVariantOwns, Link has a base class of LogoVariantOwns, but both require Agent and Link.
	//Everything is just a big circular reference, so used Model.h to hold the correct order of includes and forward declared Agent and Link where required.
	class Agent;
	class Link;

	//need a variant that I can create with a specific type e.g.
	//LogoVariant.Set<string>("mystring"); //and Get<>
	//LogoVariant.Set<int>(2);
	//OK, it's not a variant, it's strongly typed but can be created as any type

//http://www.ojdip.net/2013/10/implementing-a-variant-type-in-cpp/
	//simple method, just union it
	struct LogoVariant {
		//enum {t_int, t_bool, t_float, t_string, t_pAgent, t_pLink} type_id;
		size_t type_id;
		union {
			int as_integer;
			bool as_bool;
			float as_float;
			Agent* as_pAgent;
			Link* as_pLink;
			std::string* as_pString;
		} value;
		//std::string as_string; //this is really nasty!

		////////////////////////////////////////////////////////////////////

		//default constructor
		LogoVariant() {};

		//copy constructor (needed when it's added to a map)
		LogoVariant(const LogoVariant& other) :
		type_id( other.type_id ), value( other.value )
		{
			//this is nasty - we need to create another string pointer if it's assigned, otherwise it's going to get deleted
			if (other.type_id==typeid(std::string).hash_code()) {
				value.as_pString = new std::string(*other.value.as_pString);
			}
		};

		//other copy constructor
		//LogoVariant(LogoVariant& other) :
		//type_id( other.type_id ), value( other.value )
		//{
		//	//this is nasty - we need to create another string pointer if it's assigned, otherwise it's going to get deleted
		//	if (other.type_id==typeid(std::string).hash_code()) {
		//		value.as_pString = new std::string(*other.value.as_pString);
		//	}
		//};

		//assignment operator (needed when it's added to a map)
		LogoVariant& LogoVariant::operator=(const LogoVariant& other)
		{
			type_id=other.type_id;
			value=other.value;
			//this is nasty - we need to create another string pointer if it's assigned, otherwise it's going to get deleted
			if (other.type_id==typeid(std::string).hash_code()) {
				value.as_pString = new std::string(*other.value.as_pString);
			}
			return *this;
		}
		
		//int constructor
		LogoVariant(int val) {
			//type_id=t_int;
			type_id=typeid(int).hash_code();
			value.as_integer=val;
		};
		//bool constructor
		LogoVariant(bool b) {
			//type_id=t_bool;
			type_id=typeid(bool).hash_code();
			value.as_bool=b;
		};
		//float constructor
		LogoVariant(float f) {
			//type_id=t_float;
			type_id=typeid(float).hash_code();
			value.as_float=f;
		};
		//char * constructor - change into a string. For some reason, if you don't include this it tries to use the bool constructor.
		LogoVariant(const char* chars) {
			type_id=typeid(std::string).hash_code();
			value.as_pString=new std::string(chars);
		}
		//string constructor
		LogoVariant(const std::string& str) {
			//type_id=t_string;
			type_id=typeid(std::string).hash_code();
			value.as_pString=new std::string(str);
		};
		//Agent* constructor
		LogoVariant(Agent* pAgent) {
			//type_id=t_pAgent;
			type_id=typeid(Agent*).hash_code();
			value.as_pAgent=pAgent;
		};
		//Link* constructor
		LogoVariant(Link* pLink) {
			//type_id=t_pLink;
			type_id=typeid(Link*).hash_code();
			value.as_pLink=pLink;
		};
		//end of constructors

		//destructor
		~LogoVariant() {
			//for string types we've created a pointer to a string, so we have to delete it
			if (type_id==typeid(std::string).hash_code())
				delete value.as_pString;
		}

		template <typename T>
		T& Get() {
			if (type_id==typeid(std::string).hash_code())
				return **reinterpret_cast<T**>(&value); //nice kludge, we've asked for a string, but have to store it in the union as a pointer, so double de-reference
			else if (type_id == typeid(T).hash_code())
				return *reinterpret_cast<T*>(&value);
			else
				throw std::bad_cast();
		}


		//////////////////////////////////////////////////////////////////

		//accessors
		//int asInt() { return value.as_integer; }
		//bool asBool() { return value.as_bool; }
		//float asFloat() { return value.as_float; }
		//std::string asString() { return *value.as_pString; }
		//Agent* asPAgent() { return value.as_pAgent; }
		//Link* asPLink() { return value.as_pLink; }


		//////////////////////////////////////////////////////////////////

		//equality test
		//bool operator==(LogoVariant& var) {
		//	if (type_id!=var.type_id) return false;
		//	switch (type_id) {
		//	case t_int : return value.as_integer==var.value.as_integer;
		//	case t_bool: return value.as_bool==var.value.as_bool;
		//	case t_float: return value.as_float==var.value.as_float;
		//	case t_string: return *value.as_pString==*var.value.as_pString; //note pointers to strings
		//	case t_pAgent: return value.as_pAgent==var.value.as_pAgent;
		//	case t_pLink: return value.as_pLink==var.value.as_pLink;
		//	}
		//	return false;
		//}

		//equality test
		//bool operator==(LogoVariant& var) {
		//	if (type_id!=var.type_id) return false;
		//You can't do this, you need the type. Unless you write out cases explicitly.
		//}
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