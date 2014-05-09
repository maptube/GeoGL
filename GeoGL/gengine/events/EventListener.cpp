
#include "EventListener.h"

namespace gengine {
	namespace events {

		EventListener::EventListener(void)
		{
		}


		EventListener::~EventListener(void)
		{
		}

		void EventListener::WindowSizeCallback(GLFWwindow *window, int w, int h)
		{
		}

		void EventListener::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
		{
		}

		void EventListener::CursorPosCallback(GLFWwindow *window, double mx, double my)
		{
		}

		void EventListener::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
		{
		}

		void EventListener::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
		}

	} //namespace events
} //namespace gengine