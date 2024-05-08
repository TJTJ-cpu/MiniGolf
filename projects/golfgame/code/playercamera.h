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

class PlayerCamera
{
public:
	PlayerCamera() {}
	PlayerCamera(glm::vec3 pos, glm::vec3 rot) : Position(pos), Rotation(rot) { Transform = glm::translate(pos); }



	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::mat4 Transform;

	float MovementSpeed = 5.0f;
	float VerticleSpeed = 2.0f;
	//func
	
	void Update(float DeltaSeconds)
	{
		Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);
		Input::Keyboard* kbd = Input::GetDefaultKeyboard();

		cam->view = glm::lookAt(Position, Position + glm::vec3(0,-1,0), glm::vec3(1,0,0));


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
		Position.x += DeltaSeconds * axes[3] * MovementSpeed;
		Position.y += DeltaSeconds * -axes[1] * VerticleSpeed;
		Position.z += DeltaSeconds * axes[2] * MovementSpeed;
	}
};
