#pragma once

#include "main.h"
#include "gengine/events/EventListener.h"
#include "gengine/shader.h"
#include "graph.h"
#include <map>
#include "object3d.h"
#include "gengine/Camera.h"

//should we be using this?
//#include "SDL/SDL_opengl.h"

typedef std::vector<Object3D*> SceneGraphType;

/**
	OpenGLContext is a class designed to store all of your OpenGL functions and keep them
	out of the way of your application logic. Here we have the ability to create an OpenGL
	context on a given window and then render to that window.
*/
class OpenGLContext : public gengine::events::EventListener {
public:
	static bool hasProgrammableShaders; //set to false for OpenGL1 fallback
	static int VersionMajor; //OpenGL version numbers
	static int VersionMinor;

	OpenGLContext(void); // Default constructor
	OpenGLContext(HWND hwnd); // Constructor for creating our context given a hwnd
	~OpenGLContext(void); // Destructor for cleaning up our application
	virtual void WindowSizeCallback(GLFWwindow* window, int w, int h);

	int GetWindowWidth() { return windowWidth; }
	int GetWindowHeight() { return windowHeight; }

	bool create30Context(HWND hwnd); // Creation of our OpenGL 3.x context
	bool create30ContextSDL(); //Creation of context using SDL
	void destroyContextSDL();
	bool create30ContextGLFW(); //Creation of context using GLFW
	void destroyContextGLFW();
	void destroyScene(void); //free all the objects in the scene graph
	void setupScene(void); // All scene information can be setup here
	void reshapeWindow(int w, int h); // Method to get our window width and height on resize
	void renderScene(gengine::Camera* camera); // Render scene (display method from previous OpenGL tutorials)

	glm::mat4 fitViewMatrix(); //fit a view matrix to enclose all of the current objects in the scene

	GLFWwindow* window; //GLFW window handle

	std::vector<Object3D*> SceneGraph; //list of object to be rendered on "renderScene"
	//SceneGraphType SceneGraph;

private:
	int windowWidth; // Store the width of our window
	int windowHeight; // Store the height of our window

	//SDL_Window *mainwindow; //Our window handle
	//SDL_GLContext maincontext; //Our opengl context handle

	gengine::Shader *shader; // Our GLSL shader

	//glm::mat4 projectionMatrix; // Store the projection matrix
	//glm::mat4 viewMatrix; // Store the view matrix
	glm::mat4 modelMatrix; // Store the model matrix

	//TODO: do these three need to be contiguous in memory?
	unsigned int vaoID[1]; // Our Vertex Array Object
	unsigned int vboID[1]; // Our Vertex Buffer Object
	//unsigned int iboID[1]; //Index buffer object
	GLuint iboID[1]; //Index buffer object

protected:
	HGLRC hrc; // Rendering context
	HDC hdc; // Device context
	HWND hwnd; // Window identifier
};
