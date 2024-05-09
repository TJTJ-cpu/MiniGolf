#include "render/cameramanager.h"
#include "glm.hpp"
#include <vec3.hpp> 
#include <vec4.hpp> 
#include <mat4x4.hpp> 
#include <iostream>
#include "gtc/matrix_transform.hpp" 
#include "gtx/transform.hpp"
#include "gtc/quaternion.hpp"
#include "render/input/inputserver.h"
#include "golfball.h"

class PlayerCamera
{
public:
	PlayerCamera() {}
	PlayerCamera(glm::vec3 pos,glm::vec3 offset, glm::vec3 rot) :  Rotation(rot) 
	{
		Transform = glm::translate(pos);  
		OffSet = offset;
		Position = OffSet;
		Ball = GolfBall(pos, "assets/golf/ball-blue.glb");
	}

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 OffSet;
	glm::mat4 Transform;

	GolfBall Ball;

	float MovementSpeed = 3.5f;
	float VerticleSpeed = 2.0f;
	//func
	
	void Update(float DeltaSeconds)
	{
		//Ball.Update(DeltaSeconds);
		Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);
		Input::Keyboard* kbd = Input::GetDefaultKeyboard();



		if (kbd->held[Input::Key::W]) 
			Position.x += DeltaSeconds * MovementSpeed;

		if (kbd->held[Input::Key::S]) 
			Position.x -= DeltaSeconds * MovementSpeed;

		if (kbd->held[Input::Key::E]) 
			Position.y += DeltaSeconds * VerticleSpeed;

		if (kbd->held[Input::Key::Q]) 
			Position.y -= DeltaSeconds * VerticleSpeed;

		if (kbd->held[Input::Key::D]) 
			Position.z += DeltaSeconds * MovementSpeed;

		if (kbd->held[Input::Key::A]) 
			Position.z -= DeltaSeconds * MovementSpeed;
		
		//Gamepad
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		OffSet.y += DeltaSeconds * -axes[1] * VerticleSpeed;
		Position.x += DeltaSeconds * axes[3] * MovementSpeed;
		Position.z += DeltaSeconds * axes[2] * MovementSpeed;

		if (axes[3] == 0 && axes[2] == 0)
		{
			//Position = Ball.Position + OffSet;
			// SMOOTHLY MOVE THE CAMERA BACK TO THE BALL
		}
		else
		{
			//float CameraMoveableRadius = 3;
			//float sth =glm::length(Position - (Ball.Position + OffSet)) < CameraMoveableRadius ;
			//std::cout << sth << std::endl;
			////std::cout << "X: " << OffSet.x << " Y: " << OffSet.y << " Z: " << OffSet.z << std::endl;
			//std::cout << std::endl;
			//if (glm::length(Position - (Ball.Position + OffSet)) < CameraMoveableRadius)
			//{
			//	Position.x += DeltaSeconds * axes[3] * MovementSpeed;
			//	Position.z += DeltaSeconds * axes[2] * MovementSpeed;
			//}

			float CameraMoveableRadius = 2;
			if (glm::length(Position - (Ball.Position + OffSet)) > CameraMoveableRadius)
			{
				glm::vec3 norm = glm::normalize((Position - (Ball.Position + OffSet)));
				norm.y = 0;
				norm *= CameraMoveableRadius;
				std::cout << "X: " << norm.x << " Y: " << norm.y << " Z: " << norm.z << std::endl;
				Position = Ball.Position + OffSet + norm;
			}
			//Ball.Position
			// DONT LET THE CAMERA MOVE TOO FAR
		}

		cam->view = glm::lookAt(Position, Position + glm::vec3(0,-1,0), glm::vec3(1,0,0));
	}
};
