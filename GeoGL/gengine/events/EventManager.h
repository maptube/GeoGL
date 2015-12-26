#pragma once
#include "../gengine.h"
#include "EventListener.h"

#include <vector>

namespace gengine {
	namespace events {

		/**
		* Singleton to allow controllers and other classes to hook themsleves into the event and gestures system to
		* receive notifications events.
		* This implementation is rather ugly and it might be better to have a single event object that wraps the
		* low level GLFW callbacks.
		*/
		class EventManager
		{
		private:
			EventManager() {};
			EventManager(EventManager const&); //don't implement
			void operator=(EventManager const&); //don't implement
			static void windowsize_callback(GLFWwindow *window, int w, int h);
			static void mousebutton_callback(GLFWwindow *window, int button, int action, int mods);
			static void cursorpos_callback(GLFWwindow *window, double mx, double my);
			static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
			static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		public:
			static std::vector<EventListener*> windowsizeListeners;
			static std::vector<EventListener*> mousebuttonListeners;
			static std::vector<EventListener*> cursorposListeners;
			static std::vector<EventListener*> scrollListeners;
			static std::vector<EventListener*> keyListeners;

			static EventManager& getInstance()
			{
				static EventManager instance;

				return instance;
			}
			//virtual ~EventManager(void);
			void SetWindow(GLFWwindow* window);
			static void AddWindowSizeEventListener(EventListener* listener);
			static void AddMouseButtonEventListener(EventListener* listener);
			static void AddCursorPosEventListener(EventListener* listener);
			static void AddScrollEventListener(EventListener* listener);
			static void AddKeyEventListener(EventListener* listener);

			static void RemoveWindowSizeEventListener(EventListener* listener);
			static void RemoveMouseButtonEventListener(EventListener* listener);
			static void RemoveCursorPosEventListener(EventListener* listener);
			static void RemoveScrollEventListener(EventListener* listener);
			static void RemoveKeyEventListener(EventListener* listener);
		};

	} //namespace events
} //namespace gengine
