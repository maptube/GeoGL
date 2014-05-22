#pragma once
#include "gengine.h"

namespace gengine {

	class Camera
	{
	public:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix; //view matrix is used to transform scene into view space, camera matrix is camera position+rotation in scene

		Camera(void);
		~Camera(void);
		glm::vec3 GetCameraPos() {
			//get position of camera in scene
			//direct manipulation of GLSL matrix, but can't see any other way of doing this in GLM
			//TODO: not sure this is right?
			//glm::vec3 vCameraPos(-viewMatrix[3][0],-viewMatrix[3][1],-viewMatrix[3][2]);
		
			//I think this is how you do it
			glm::mat4 mCamera = glm::inverse(viewMatrix);
			//glm::vec3 vCameraPos(-viewMatrix[3][0],-viewMatrix[3][1],-viewMatrix[3][2]);
			glm::vec3 vCameraPos(mCamera[3][0],mCamera[3][1],mCamera[3][2]);
			return vCameraPos;
		};
		void SetCameraPos(glm::vec3 Pos) {
			//set position of camera in scene - only holding view matrix which is inverse of camera matrix
			//direct manipulation of GLSL matrix, but can't see any other way of doing this in GLM
			//TODO: not sure this is right?
			//viewMatrix[3][0] = -Pos.x;
			//viewMatrix[3][1] = -Pos.y;
			//viewMatrix[3][2] = -Pos.z;

			//I think this is how you do it
			glm::mat4 mCamera = glm::inverse(viewMatrix);
			mCamera[3][0] = Pos.x;
			mCamera[3][1] = Pos.y;
			mCamera[3][2] = Pos.z;
			viewMatrix = glm::inverse(mCamera);
		};
		glm::mat4 GetCameraMatrix() {
			//return the camera's view and position in the scene - this is where you would place the camera
			//if you were drawing it
			return glm::inverse(viewMatrix); //this gets the translation right as well
		};
		void SetCameraMatrix(glm::mat4 CameraMatrix) {
			viewMatrix = glm::inverse(CameraMatrix);
		};
		void SetupPerspective(int windowWidth, int windowHeight, float nearClip, float farClip);
		/**
		* LookAt
		* Make the camera look at a new point while staying at its current location. The up vector is forced to (0,1,0).
		* @param vLookPoint The point vector that the camera is to look at.
		*/
		void LookAt(glm::vec3 vLookPoint) {
			glm::vec3 eye = GetCameraPos();
			//this is wrong
			//glm::mat4 cameraMatrix = glm::lookAt(eye,vLookPoint,glm::vec3(0.0,1.0,0.0));
			//viewMatrix = glm::inverse(cameraMatrix); //TODO: check translation
		
			//this is right - lookat operates in view space, not camera space
			viewMatrix = glm::lookAt(eye,vLookPoint,glm::vec3(0.0,1.0,0.0));
		}
	};

} //namespace gengine
