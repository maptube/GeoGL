
#include "Link.h"
#include "LogoVariantOwns.h"

namespace ABM {

	//static declarations
	Links* Link::_pParentLinks;

	Link::Link(void)
	{
		//ownership = new LogoVariantOwns();
	}


	Link::~Link(void)
	{
		//delete ownership;
	}

}