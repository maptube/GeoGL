/*
 * mainwindow.h
 *
 *  Created on: 3 May 2015
 *      Author: richard
 */

#pragma once

#include <gtkmm/button.h>
#include <gtkmm/window.h>

namespace geogl {
namespace gui {

class MainWindow : public Gtk::Window {
public:
	MainWindow();
	virtual ~MainWindow();
};

} // namespace gui
} // namespace geogl


