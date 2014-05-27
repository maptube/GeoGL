#pragma once

#include "../gengine.h"

namespace gengine {
	namespace events {
		/**
		* EventListener
		* Base class for controllers which need mouse, key or gesture event notification.
		*/
		class EventListener
		{
		public:
			EventListener(void);
			~EventListener(void);
			virtual void WindowSizeCallback(GLFWwindow *window, int w, int h);
			virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
			virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
			virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
			virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		};

	} // namespace events
} //namespace gengine
