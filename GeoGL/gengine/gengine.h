#pragma once

//#define NO_STDIO_REDIRECT
//#define SDL_MAIN_AVAILABLE
//#define NO_SDL_GLEXT

//SDL_HINT_RENDER_OPENGL_SHADERS

//////////////////////////////////////////////////////////////
//Building instructions and external dependencies
//
//poly2tri
//========
//poly2tri builds to its intermediate directory of GeoGL\poly2tri\x64\Debug, but copies the lib file to the solution output dir GeoGL\x64\Debug. When GeoGL compiles, it uses
//its intermediate dir of GeoGL\GeoGL\x64\Debug, but can't find poly2tri.lib unless the solution output dir is on the library path. Then it copies its output to the solution output
//dir of GeoGL\x64\Debug where poly2tri.lib is and everything executes smoothly when you run the project.
//So, basically, put GeoGL\x64\Debug onto GeoGL's library path for the static link.
//
//GLEW
//====
//Downloaded from sourceforge: http://glew.sourceforge.net/ (from windows binary link)
//The windows binaries contain 32 bit and 64 bit builds. OpenGL DLLs are always labelled with 32, regardless of whether they are 32 bit or 64 bit and GLEW is no exception.
//For an x64 build, copy glew-1.10.0\bin\Release\x64\glew32.dll into windows\system32. For a win32 build it's supposed to go into syswow64.
//
//OpenGL
//======
//OK, so this is completely weird - OpenGL 64 bit is labelled opengl32.dll and it's in system32 - the real 32 bit version is in wow64 and is also labelled with opengl32.dll
//Nothing needs to be installed for this as it's part of the display driver. The only way you can update it on Windows or Linux is to install another Nvidia or Radeon (ATi) driver.
//
//GLFW
//====
//Downloaded from: http://www.glfw.org/
//You can build from source, or Windows binaries are available for 32 bit and 64 bit: http://www.glfw.org/download.html
//The only problem with pre-build binaries is MSVCRT, so building from the vs2010 project supplied with the source might be better. Unfortunately, there is no x64 target, so
//you have to add one. Also, you need to edit CMakeLists.txt and change the CMakeLists.txt "LIB_SIFFIX" variable from "" to "64" so it knows to build a x64 version.
//Not sure if this is strictly necessary though. Added x64 target and changed WIN32 define to WIN64 define on GLFW.
//Important: The glfw project is the only one you actually need to build. All the others are test cases and they don't appear to used the same x64 defines as the main glfw.
//Be careful where the lib file builds to and copy into /externals/glfw-mine directory.
//Also, copy vc100.pdb which is in glfw.dir directory under source. This is the debug info, but after copying the file it's built, I still get vc100.pdb not found messages.
//Linux: use the package manager, although you can build from source.

//glm
//===
//Header only library downloaded from: http://glm.g-truc.net/0.9.5/index.html

//jsoncpp
//built from source, windows uses an old VS project file and converts to vs2010 to build

//freetype
//Linux: use the package manager or build from source.

//Leap
//Get the SDK from the Leap Motion site.

//dirent-1.20.1
//=============
//Used dirent header only library which provides Unix style directory listing functions for Windows by wrapping FindFirst, FindNext

//Libjpeg 9a
//==========
//There are instructions included for windows which tells you to run NMAKE /f makefile.vc  setup-v10
//in order to configure the project for vs2010. You then have to add an x64 project to the jpeg.sln yourself by copying from the win32 one

//GTK3.0, GTKMM, GTKGLEXTMM

//libcurl
//yum install libcurl libcurl-devel

//////////////////////////////////////////////////////////////
//Windows include block - probably need WIN32 and WIN64!
#ifdef WIN32

// Include Windows functions
#include <Windows.h>

// Include input and output operations
#include <iostream>

//OpenGL and GLEW Header Files and Libraries
//This is quite a complicated set of includes where the ordering is critical. GLEW and WGLEW must be included before SDL_opengl.h as this will
//include gl.h which must happen after the glew includes. The glew open GL extension wrangler is used to determine open GL capabilities and
//contains the code to initialise the shader functions. Otherwise, the gl shader functions are all set to null pointers and unusable. The
//NO_SDL_GEXT #define is to defer all GL extension processing to GLEW i.e. I want to control it myself and not let SDL do anything.
//Then come the implementation dependent versions of GLEW, WGLEW etc which need to be included based on operating system.
//So why are we using SDL? To make the application Windows/Linux portable (maybe Android?).
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <glext.h>????
//#include <GL/glxew.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
//#pragma comment(lib, "SDL2.lib")
//#pragma comment(lib, "SDL2main.lib")

#pragma comment(lib, "poly2tri.lib")

#endif
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//Windows x64 include block
#ifdef WIN64

// Include Windows functions
#include <Windows.h>

// Include input and output operations
#include <iostream>

//OpenGL and GLEW Header Files and Libraries
//This is quite a complicated set of includes where the ordering is critical. GLEW and WGLEW must be included before SDL_opengl.h as this will
//include gl.h which must happen after the glew includes. The glew open GL extension wrangler is used to determine open GL capabilities and
//contains the code to initialise the shader functions. Otherwise, the gl shader functions are all set to null pointers and unusable. The
//NO_SDL_GEXT #define is to defer all GL extension processing to GLEW i.e. I want to control it myself and not let SDL do anything.
//Then come the implementation dependent versions of GLEW, WGLEW etc which need to be included based on operating system.
//So why are we using SDL? To make the application Windows/Linux portable (maybe Android?).
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
//#include <glext.h>????
//#include <GL/glxew.h>

//OK, so this is completely weird - OpenGL 64 bit is labelled opengl32.dll and it's in system32 - the real 32 bit version is in wow64 and is also labelled with opengl32.dll
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")

#pragma comment(lib, "jpeg.lib")

#pragma comment(lib, "poly2tri.lib")

#endif

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//LINUX include block
//#ifdef LINUX
//#define __cplusplus
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#endif
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//Freetype includes for OpenGL fonts
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma comment(lib, "freetype253_D.lib")
///////////////////////////////////////////////////////////////////

// GLM include files
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_projection.hpp> //NOT THERE!
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

//IMPORTANT:
//Everything inside gengine is related to either the low level OpenGL functions, or event handling with GLFW
//NOTHING OUTSIDE gengine SHOULD EVER MAKE AN OPENGL CALL DIRECTLY
//In other words, gengine is a lightweight wrapper around OpenGL
//Outside gengine, we deal with meshes, globes and data at a higher level
namespace gengine {
}
