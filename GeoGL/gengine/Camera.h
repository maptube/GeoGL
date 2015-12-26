#pragma once
#include "gengine.h"

//#include <iostream>
//#include <memory>

namespace gengine {

	class Camera
	{
	private:
		glm::dmat4 viewMatrix; //view matrix is used to transform scene into view space, camera matrix is camera position+rotation in scene
	public:
		double _fov;
		double _near,_far,_width,_height;
		glm::dmat4 projectionMatrix;

		Camera(void);
		~Camera(void);
		glm::dvec3 GetCameraPos() {
			//get position of camera in scene
			//direct manipulation of GLSL matrix, but can't see any other way of doing this in GLM
			//TODO: not sure this is right?
			//glm::vec3 vCameraPos(-viewMatrix[3][0],-viewMatrix[3][1],-viewMatrix[3][2]);
		
			//I think this is how you do it
			glm::dmat4 mCamera = glm::inverse(viewMatrix);
			//glm::vec3 vCameraPos(-viewMatrix[3][0],-viewMatrix[3][1],-viewMatrix[3][2]);
			glm::dvec3 vCameraPos(mCamera[3][0],mCamera[3][1],mCamera[3][2]);
			return vCameraPos;
		};
		void SetCameraPos(glm::dvec3 Pos) {
			//set position of camera in scene - only holding view matrix which is inverse of camera matrix
			//direct manipulation of GLSL matrix, but can't see any other way of doing this in GLM
			//TODO: not sure this is right?
			//viewMatrix[3][0] = -Pos.x;
			//viewMatrix[3][1] = -Pos.y;
			//viewMatrix[3][2] = -Pos.z;

			//std::cout<<"SetCameraPos: "<<Pos.x<<" "<<Pos.y<<" "<<Pos.z<<" "<<glm::length(Pos)<<std::endl;

			//I think this is how you do it
			glm::dmat4 mCamera = glm::inverse(viewMatrix);
			mCamera[3][0] = Pos.x;
			mCamera[3][1] = Pos.y;
			mCamera[3][2] = Pos.z;
			viewMatrix = glm::inverse(mCamera);
		};
		glm::dmat4 GetCameraMatrix() {
			//return the camera's view and position in the scene - this is where you would place the camera
			//if you were drawing it
			return glm::inverse(viewMatrix); //this gets the translation right as well
		};
		glm::dmat4 GetViewMatrix() {
			return viewMatrix;
		}
		void SetCameraMatrix(glm::dmat4 CameraMatrix) {
			//std::cout<<"SetCameraMatrix: "<<CameraMatrix[3][0]<<" "<<CameraMatrix[3][1]<<" "<<CameraMatrix[3][2]<<std::endl;

			viewMatrix = glm::inverse(CameraMatrix);
		};
		void SetViewMatrix(glm::dmat4 ViewMatrix) {
			//std::cout<<"SetViewMatrix: "<<ViewMatrix[3][0]<<" "<<ViewMatrix[3][1]<<" "<<ViewMatrix[3][2]<<std::endl;

			viewMatrix = ViewMatrix;
		}
		void SetupPerspective(int windowWidth, int windowHeight, double nearClip, double farClip);
		glm::vec4 GetViewport();
		/**
		* LookAt
		* Make the camera look at a new point while staying at its current location. The up vector is forced to (0,1,0).
		* @param vLookPoint The point vector that the camera is to look at.
		*/
		void LookAt(glm::dvec3 vLookPoint) {
			glm::dvec3 eye = GetCameraPos();
			//this is wrong
			//glm::mat4 cameraMatrix = glm::lookAt(eye,vLookPoint,glm::vec3(0.0,1.0,0.0));
			//viewMatrix = glm::inverse(cameraMatrix); //TODO: check translation
		
			//this is right - lookat operates in view space, not camera space
			viewMatrix = glm::lookAt(eye,vLookPoint,glm::dvec3(0.0,1.0,0.0));
		}
	};

} //namespace gengine
