#pragma once

//#define NO_STDIO_REDIRECT
//#define SDL_MAIN_AVAILABLE
//#define NO_SDL_GLEXT

//SDL_HINT_RENDER_OPENGL_SHADERS

//////////////////////////////////////////////////////////////
//Windows include block
#ifdef WINDOWS

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
//LINUX include block
#ifdef LINUX
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
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
