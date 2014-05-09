
#include "LogoVariantOwns.h"

namespace ABM {

/*LogoVariantOwns::LogoVariantOwns(void)
{
}


LogoVariantOwns::~LogoVariantOwns(void)
{
	//TODO: do you need to free all the data?
	//probably not as the only pointers are agent and link which are only references
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// <summary>
/// template Get for LogoVariant of type int, bool, float, string, *Agent, *Link
/// </summary>
/// <param name="Name"></param>
/// <returns>The value of the parameter requested. Returns NULL for value does not exist, or exists as a different type.</returns>
/*template <typename T>
T LogoVariantOwns::Get(std::string Name)
{
	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(Name);
	if (it!=_owns.end()) {
		if (typeof(T)==int) {
			if (it->second.type_id==LogoVariant::t_int)
				return it->second.as_int;
		}
		else if (typeof(T)==bool) {
			if (it->second.type_id==LogoVariant::t_bool)
				return it->second.as_bool;
		}
		else if (typeof(T)==float) {
			if (it->second.type_id==LogoVariant::t_float)
				return it->second.as_float;
		}
		else if (typeof(T)==std::string) {
			if (it->second.type_id==LogoVariant::t_string)
				return it->second.as_string;
		}
		else if (typeof(T)==*Agent) {
			if (it->second.type_id==LogoVariant::t_pAgent)
				return it->second.as_pAgent;
		}
		else if (typeof(T)==*Link) {
			if (it->second.type_id==LogoVariant::t_pLink)
				return it->second.as_pLink;
		}
	}
	return NULL;
}
*/

/// <summary>
/// template Set for LogoVariant of type int, bool, float, string
/// </summary>
/// <param name="Name"></param>
/// <param name="Value"></param>
/*template <typename T>
void LogoVariantOwns::Set(std::string Name, T Value)
{
	//TODO: should also keep a global map of what this agent "owns"
	LogoVariant var = new LogoVariant();
	if (typeof(T)==int) {
		var.type_id=LogoVariant::t_int;
		var.as_integer=T;
	}
	else if (typeof(T)==bool) {
		var.type_id=LogoVariant::t_bool;
		var.as_bool=T;
	}
	else if (typeof(T)==float) {
		var.type_id=LogoVariant::t_float;
		var.as_float=T;
	}
	else if (typeof(T)==std::string) {
		var.type_id=LogoVariant::t_string;
		var.as_string=T;
	}
	else if (typeof(T)==*Agent) {
		var.type_id=LogoVariant::t_pAgent;
		var.as_pAgent=T;
	}
	else if (typeof(T)==*Link) {
		var.type_id=LogoVariant::as_pLink;
		var.as_pLink=T;
	}
	_owns.insert(std::pair<std::string,LogoVariant>(Name,var));
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////



}