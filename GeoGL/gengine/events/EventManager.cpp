
#include "EventManager.h"

namespace gengine {
	namespace events {

		//TODO: this could do with some serious restructuring and tidying up as all the code is replicated for each type of listener
		//i.e. make it more general, maybe generic event type?

		//static declarations, dynamic lists of Listeners hooked into specific types of events
		std::vector<EventListener*> EventManager::windowsizeListeners;
		std::vector<EventListener*> EventManager::mousebuttonListeners;
		std::vector<EventListener*> EventManager::cursorposListeners;
		std::vector<EventListener*> EventManager::scrollListeners;
		std::vector<EventListener*> EventManager::keyListeners;

/////////////////////////////////////////////////////////////////////////////////////////////////////

//EventManager::EventManager(/*GLFWwindow* window*/)
//{
//	std::cout<<"EventManager CONSTRUCTOR"<<std::endl;
	//hook in all the static event handlers
	//glfwSetWindowSizeCallback(window, windowsize_callback);
	//glfwSetCursorPosCallback(window, cursorpos_callback);
	//glfwSetMouseButtonCallback(window, mousebutton_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	//glfwSetKeyCallback(window, key_callback);
//}


//EventManager::~EventManager(void)
//{
//}


/////////////////////////////////////////////////////////////////////////////////////////////////////
		void EventManager::SetWindow(GLFWwindow* window)
		{
			//hook in all the static event handlers
			glfwSetWindowSizeCallback(window, windowsize_callback);
			glfwSetCursorPosCallback(window, cursorpos_callback);
			glfwSetMouseButtonCallback(window, mousebutton_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetKeyCallback(window, key_callback);
		}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//Callbacks

		void EventManager::windowsize_callback(GLFWwindow *window, int w, int h)
		{
			//std::cout<<"EventManager::windowsize_callback "<<w<<" "<<h<<std::endl;
			for (std::vector<EventListener*>::iterator it = windowsizeListeners.begin(); it!=windowsizeListeners.end(); ++it) {
				(*it)->WindowSizeCallback(window,w,h);
			}
		}

		void EventManager::mousebutton_callback(GLFWwindow *window, int button, int action, int mods)
		{
			//std::cout<<"EventManager::mousebutton_callback "<<action<<" "<<mods<<" "<<std::endl;
			for (std::vector<EventListener*>::iterator it = mousebuttonListeners.begin(); it!=mousebuttonListeners.end(); ++it) {
				(*it)->MouseButtonCallback(window,button,action,mods);
			}
		}

		void EventManager::cursorpos_callback(GLFWwindow *window, double mx, double my)
		{
			//cursor position means the mouse in GLFW
			//std::cout<<"EventManager::cursorpos_callback "<<mx<<","<<my<<std::endl;
			for (std::vector<EventListener*>::iterator it = cursorposListeners.begin(); it!=cursorposListeners.end(); ++it) {
				(*it)->CursorPosCallback(window,mx,my);
			}
		}

		void EventManager::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
		{
			//scroll means mouse wheel in GLFW
			//std::cout<<"EventManager::scroll_callback "<<xoffset<<" "<<yoffset<<std::endl;
			for (std::vector<EventListener*>::iterator it = scrollListeners.begin(); it!=scrollListeners.end(); ++it) {
				(*it)->ScrollCallback(window,xoffset,yoffset);
			}
		}

		void EventManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
		//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		//		glfwSetWindowShouldClose(window, GL_TRUE);
		//std::cout<<"EventManager::key_callback "<<key<<" "<<scancode<<" "<<action<<" "<<mods<<" "<<std::endl;
			for (std::vector<EventListener*>::iterator it = keyListeners.begin(); it!=keyListeners.end(); ++it) {
				(*it)->KeyCallback(window,key,scancode,action,mods);
			}
		}

//////////////////////////////////////////////////////////////////////////////////////////
		void EventManager::AddWindowSizeEventListener(EventListener* listener)
		{
			windowsizeListeners.push_back(listener);
		}

		void EventManager::AddMouseButtonEventListener(EventListener* listener)
		{
			mousebuttonListeners.push_back(listener);
		}

		void EventManager::AddCursorPosEventListener(EventListener* listener)
		{
			cursorposListeners.push_back(listener);
		}

		void EventManager::AddScrollEventListener(EventListener* listener)
		{
			scrollListeners.push_back(listener);
		}

		void EventManager::AddKeyEventListener(EventListener* listener)
		{
			keyListeners.push_back(listener);
		}
//////////////////////////////////////////////////////////////////////////////////////////
		void EventManager::RemoveWindowSizeEventListener(EventListener* listener)
		{
			for (std::vector<EventListener*>::iterator it=windowsizeListeners.begin(); it!=windowsizeListeners.end(); ++it)
			{
				if (*it==listener) {
					windowsizeListeners.erase(it);
					break;
				}
			}
		}

		void EventManager::RemoveMouseButtonEventListener(EventListener* listener)
		{
			for (std::vector<EventListener*>::iterator it=mousebuttonListeners.begin(); it!=mousebuttonListeners.end(); ++it)
			{
				if (*it==listener) {
					mousebuttonListeners.erase(it);
					break;
				}
			}
		}

		void EventManager::RemoveCursorPosEventListener(EventListener* listener)
		{
			for (std::vector<EventListener*>::iterator it=cursorposListeners.begin(); it!=cursorposListeners.end(); ++it)
			{
				if (*it==listener) {
					cursorposListeners.erase(it);
					break;
				}
			}
		}

		void EventManager::RemoveScrollEventListener(EventListener* listener)
		{
			for (std::vector<EventListener*>::iterator it=scrollListeners.begin(); it!=scrollListeners.end(); ++it)
			{
				if (*it==listener) {
					scrollListeners.erase(it);
					break;
				}
			}
		}

		void EventManager::RemoveKeyEventListener(EventListener* listener)
		{
			for (std::vector<EventListener*>::iterator it=keyListeners.begin(); it!=keyListeners.end(); ++it)
			{
				if (*it==listener) {
					keyListeners.erase(it);
					break;
				}
			}
		}

//////////////////////////////////////////////////////////////////////////////////////////

	} //namespace events
} //namespace gengine
