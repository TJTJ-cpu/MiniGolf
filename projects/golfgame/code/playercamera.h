#pragma once
#include "render/cameramanager.h"
#include "golfball.h"
#include "golfclub.h"
#include "config.h"

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
		Club = GolfClub(pos + glm::vec3(1,0.7,0), "assets/golf/club-blue.glb", 4);
	}

	glm::mat4 Transform;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 OffSet;

	GolfBall Ball;
	GolfClub Club;

	/// TEMP VAR 
	glm::vec3 BallHitDirection;

	float MovementSpeed = 10.0f;
	float ClubMovementSpeed = 4.2f;
	float VerticleSpeed = 2.0f;
	
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
		
		/// GAMEPAD
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		const unsigned char* Hit = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
		if (Hit[0] == GLFW_PRESS && !Club.bIsMovingTowardBall) {
			Club.bIsMovingTowardBall = true;
		}


		/// DECLARE THIS SOMEWHERE ELSE LATER
		glm::vec3 BallPos = Ball.Position;
		glm::vec3 ClubPos = Club.Position;
		BallPos.y = 0;
		ClubPos.y = 0;
		if (!Club.bIsMovingTowardBall)
		{
			float Distance = glm::distance(BallPos, ClubPos);

			/// MOVE THE GOLF CLUB WITH GAMEPAD LEFT STICK
			Club.Position.x += DeltaSeconds * axes[1] * VerticleSpeed;
			Club.Position.z += DeltaSeconds * axes[0] * VerticleSpeed;
			float MaxDistance = 2;
			if (Distance > MaxDistance) {
				glm::vec3 norm = glm::normalize((ClubPos - BallPos));
				norm *= MaxDistance;
				Club.Position = Ball.Position + norm;
				Club.Position.y += 0.7f;
			}
			Club.Rotation = glm::inverse(glm::lookAt(ClubPos, BallPos, glm::vec3(0, 1, 0))) * glm::rotate(3.141592f / 2, glm::vec3(0,1,0));
		}
		else {
			// normalize
			glm::vec3 norm = glm::normalize(BallPos - ClubPos);
			BallHitDirection = norm;
			// go toward until ball -- while
			if (glm::distance(ClubPos, BallPos) > 0.1f){
				Club.Position += norm * ClubMovementSpeed * DeltaSeconds;
				ClubPos = Club.Position;
				ClubPos.y = 0;
			}
			else {
				Club.bIsMovingTowardBall = false;
			}
		}
		
		/// ROTATE THE CLUB TO POINT AT THE BALL

		/// IF THE ACTION BUTTON IS PRESSED GO TOWARDS TO BALL

		Club.Transform = glm::translate(Club.Position) * (glm::mat4)Club.Rotation;
		//OffSet.y += DeltaSeconds * -axes[1] * VerticleSpeed;

		/// MOVE THE CAMERA WITH THE RIGHT STICK
		Position.x += DeltaSeconds * axes[3] * MovementSpeed;
		Position.z += DeltaSeconds * axes[2] * MovementSpeed;

		if (axes[3] == 0 && axes[2] == 0) {
			Position = mix(Position, Ball.Position + OffSet, std::min(0.1f, DeltaSeconds * 30.0f));
		}
		else {
			float CameraMoveableRadius = 2;
			if (glm::length(Position - (Ball.Position + OffSet)) > CameraMoveableRadius) {
				glm::vec3 norm = glm::normalize((Position - (Ball.Position + OffSet)));
				norm.y = 0;
				norm *= CameraMoveableRadius;
				std::cout << "X: " << norm.x << " Y: " << norm.y << " Z: " << norm.z << std::endl;
				Position = Ball.Position + OffSet + norm;
			}
		}
		cam->view = glm::lookAt(Position, Position + glm::vec3(0,-1,0), glm::vec3(1,0,0));
	}
	
	void Draw()
	{
		this->Club.Draw();
		this->Ball.Draw();
	}

};
