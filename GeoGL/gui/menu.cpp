/*
 * menu.cpp
 *
 *  Created on: 11 Apr 2015
 *      Author: richard
 */

#include "menu.h"

#include "gengine/gengine.h"
#include "gengine/graphicscontext.h"
#include "gengine/scenedataobject.h"
#include "gengine/shader.h"

#include <string>
#include <vector>
#include <sstream>
//#include <iterator>

namespace geogl {
namespace gui {

Menu::Menu(gengine::GraphicsContext* GC) {
	_GC=GC;
	//initialise font rendering
	_FontFace = _GC->LoadFont(
				"../fonts/FreeSans.ttf",
				/*48*/32);
	//set up events manager - should this be in the GC code?
	//need a mouse move and click listener
	//	EventManager& eventmanager = EventManager::getInstance(); //singleton pattern
	//	eventmanager.SetWindow(GC->window); //initialise event system with OpenGL window handle
	//	eventmanager.AddWindowSizeEventListener(this);

}

Menu::~Menu() {
	//remove event handler
	//	EventManager& em=EventManager::getInstance();
	//	em.RemoveWindowSizeEventListener(this);
}

/// <summary>
/// Add a new item to the menu.
/// </summary>
//MenuItem& Menu::Add(const std::string& item) {
//	//split on |
//	std::stringstream ss(item);
//	MenuGroup G = MenuRoot;
//	while (ss.good()) {
//		std::string elem;
//		std::getline(ss,elem,'|');
//		//find elem in G? or do we need to add a new group
//		bool Found=false;
//		for (std::vector<MenuItem>::iterator it=G.Items.begin(); it!=G.Items.end(); ++it) {
//			if (it->DisplayText==elem) {
//				//found item, so add to existing menu
//				G=it->SubMenu;
//				Found=true;
//				break;
//			}
//		}
//		if (!Found) {
//			//need to add a new one
//			MenuItem m;
//			m.DisplayText=elem;
//			G.Items.push_back(m);
//			G=m;
//		}
//	}
//	return &G;
//}

/// <summary>
/// Overload to add a menu item to a group manually
/// </summary>
//MenuItem& Menu::Add(MenuGroup& G, MenuItem& m) {
//
//
//}

/// <summary>
/// Find and return a reference to the item
/// </summary>
MenuItem& Menu::Find(const std::string& item) {

}

/// <summary>
/// Render the current menu to the screen as a 2D overlay i.e. fixed relative to the view
/// </summary>
void Menu::Render() {
	//draw it

	//_GC->_FontShader->bind();

	//float sx=2.0/512.0; float sy=2.0/512.0; //this is window size
	//GC->RenderText(_FontFace,glm::vec3(1.0,0,0),"text text text text",-1.0+8.0*sx,1.0-50.0*sy,sx,sy);
	//GC->RenderText(_FontFace,glm::vec3(0,1.0,0),"text text text text",0,0,sx,sy);

	//std::string strFPS = std::to_string((double)_debugFPS);
	//char chFPS[256]; //it's never going to be this big?
	//sprintf(chFPS,"%.2f",_debugFPS);

	//_GC->RenderText(_FontFace,glm::vec3(0,1.0,0),"MENU!",-1.0+16.0*sx,1.0-50.0*sy,sx,sy); //why not (-1,-1) in device coordinates?

	//_GC->_FontShader->unbind();
}

} // namespace gui
} // namespace geogl
