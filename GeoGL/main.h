#pragma once

/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

//must include this first due to glew, opengl and glm include dependencies
#include "gengine/gengine.h"

#ifdef _DEBUG
#pragma comment(lib, "Leapd.lib")
#else
#pragma comment(lib, "Leap.lib")
#endif

// Parser for JSON data, JSONcpp
//NOTE: although this is labelled mtd, the VS project sln has been modified to MDd to match this project
//Although it builds with errors, the lib file is still generated.
//TODO: find a better solution for this?
#pragma comment(lib, "json_vc71_libmtd.lib")
