/*
 * menu.h
 *
 *  Created on: 11 Apr 2015
 *      Author: richard
 */

#pragma once

//# yum install gtk3 gtk2 (should already be there - it's gnome)
//# yum install gtk3-devel gtk2-devel
//# yum install gtkmm30-devel

#include <gtk/gtk.h>
#include <gtkmm.h>

#include <string>
#include <vector>
#include <memory>

#include "gengine/gengine.h"

//forward references
namespace gengine {
	class GraphicsContext;
	//class SceneDataObject;
}

namespace geogl {
namespace gui {

//forward references
struct MenuGroup;
struct MenuItem;

struct MenuGroup {
	std::vector<MenuItem> Items;
};

struct MenuItem {
	std::string DisplayText;
	//icon?
	//void* Callback;
	std::vector<MenuGroup> SubMenu;
	bool IsChildOpen;
};


class Menu {
public:
	//need to save the current state here
	MenuGroup MenuRoot;
private:
	FT_Face _FontFace;
	gengine::GraphicsContext* _GC;
public:
	Menu(gengine::GraphicsContext* GC);
	virtual ~Menu();
	//MenuItem& Add(const std::string& item);
	MenuItem& Find(const std::string& item);
	void Render();
};

} // namespace gui
} // namespace geogl


