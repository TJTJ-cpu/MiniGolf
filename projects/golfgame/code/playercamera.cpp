#include "playercamera.h"
#include "config.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/debugrender.h"
#include <iostream>

PlayerCamera::PlayerCamera()
{
}

PlayerCamera::PlayerCamera(glm::vec3 pos, glm::vec3 offset, glm::vec3 rot) : Rotation(rot)
{
	Transform = glm::translate(pos);
	OffSet = offset;
	Position = OffSet;
	Ball = GolfBall(pos, "assets/golf/ball-blue.glb");
	Club = GolfClub(pos + glm::vec3(0, 0.7, -1), "assets/golf/club-blue.glb", 4);
}

void PlayerCamera::Update(float DeltaSeconds)
{
	Ball.Update(DeltaSeconds);
	this->Club.Position.y = this->Ball.Position.y + 0.7f;

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


	/// DECLARE THIS SOMEWHERE ELSE LATER
	glm::vec3 BallPos = Ball.Position;
	glm::vec3 ClubPos = Club.Position;
	BallPos.y = 0;
	ClubPos.y = 0;

	/// IF THE ACTION BUTTON IS PRESSED GO TOWARDS TO BALL
	if (Hit[0] == GLFW_PRESS && !Club.bIsMovingTowardBall && (glm::length(Ball.Velocity) < 0.1)) {
		Club.bIsMovingTowardBall = true;
		DistanceFromClubToTheGolfBall = glm::distance(this->Ball.Position, this->Club.Position);
		BallHitDirection = glm::normalize(BallPos - ClubPos);
	}

	/// SWITCH CAMERA PERSPECTIVE
	if (Hit[5] == GLFW_PRESS && FirstPress) {
		FirstPress = false;
		ThirdPersonCam = !ThirdPersonCam;
		OrbitPoint = 0;
	}
	else if (Hit[5] == GLFW_RELEASE)
	{
		FirstPress = true;
	}

	if (!Club.bIsMovingTowardBall) {
		float Distance = glm::distance(BallPos, ClubPos);

		/// MOVE THE GOLF CLUB WITH GAMEPAD LEFT STICK
		if (!ThirdPersonCam)
		{
			Club.Position.x += DeltaSeconds * axes[1] * VerticleSpeed;
			Club.Position.z += DeltaSeconds * axes[0] * VerticleSpeed;
		}
		float MaxDistance = 2;
		if (Distance > MaxDistance) {
			glm::vec3 norm = glm::normalize((ClubPos - BallPos));
			norm *= MaxDistance - 0.01;
			Club.Position = Ball.Position + norm;
			Club.Position.y += 0.7f;
		}
		/// ROTATE THE CLUB TO POINT AT THE BALL
		Club.Rotation = glm::inverse(glm::lookAt(ClubPos, BallPos, glm::vec3(0, 1, 0))) * glm::rotate(3.141592f / 2, glm::vec3(0, 1, 0));
	}
	else {
		glm::vec3 norm = glm::normalize(BallPos - ClubPos);
		//BallHitDirection = norm;
		if (glm::distance(ClubPos, BallPos) > 0.1f) {
			// Club moving toward the ball
			Club.Position += norm * ClubMovementSpeed * DeltaSeconds;
			ClubPos = Club.Position;
			ClubPos.y = 0;
		}
		else {
			this->Ball.AddForce(BallHitDirection * 75.3f * DistanceFromClubToTheGolfBall);
			Club.bIsMovingTowardBall = false;
		}
	}

	Club.Transform = glm::translate(Club.Position) * (glm::mat4)Club.Rotation;
	//OffSet.y += DeltaSeconds * -axes[1] * VerticleSpeed;

	if (!ThirdPersonCam)
	{
		OffSet = glm::vec3(0, 2, 0);

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
				Position = Ball.Position + OffSet + norm;
			}
		}
		cam->view = glm::lookAt(Position, Position + glm::vec3(0, -1, 0), glm::vec3(1, 0, 0));
	}
	else
	{
		glm::vec3 TargetPosition; 

		OrbitPoint += DeltaSeconds * axes[2] * OrbitSpeed;

		/// GET INPUT IN THE FORM [0, 1]
		float ZoomInAmount = ((axes[5] + 1) / 2);
		float ZoomOutAmount = ((axes[4] + 1) / 2);

		/// COOL WAY TO EXPRESS ZOOMING
		OrbitDistance = std::min(MaxOrbitDistance, std::max(MinOrbitDistance, OrbitDistance + ZoomSpeed * (ZoomOutAmount - ZoomInAmount)));

		/// BORING WAY
		//if (OrbitDistance < 5)
			//OrbitDistance += ZoomSpeed * ZoomOutAmount;
		//if (OrbitDistance > 1)
			//OrbitDistance -= ZoomSpeed * ZoomInAmount;

		OffSet = glm::vec3(0, OrbitDistance/2, 0);

		TargetPosition = Ball.Position + OffSet;
		TargetPosition.x -= glm::cos(OrbitPoint) * OrbitDistance;
		TargetPosition.z -= glm::sin(OrbitPoint) * OrbitDistance;

		Position = mix(Position, TargetPosition, std::min(0.1f, DeltaSeconds * 30.0f));

		cam->view = glm::lookAt(Position, Ball.Position, glm::vec3(0, 1, 0));

		/// PROJECTION EXAMPLE
		// glm::vec3 A;
		// glm::vec3 B;
		// glm::vec3 ProjAOnB = (glm::dot(A, B) / glm::length(B)) * B;

		glm::vec3 ClubMovement = glm::vec3(0, 0, 0);

		ClubMovement.x += DeltaSeconds * axes[0] * VerticleSpeed;
		ClubMovement.z -= DeltaSeconds * axes[1] * VerticleSpeed;


		Club.Position += glm::vec3(glm::inverse(cam->view) * glm::vec4(ClubMovement, 0));
	}

	this->Ball.HandlePhysics(DeltaSeconds);
}

void PlayerCamera::Draw()
{
	if (glm::length(this->Ball.Velocity) < 0.05) {
		this->Club.Draw();
	}
	else {
		Debug::DrawDebugText("HsjflksdajflkasdjflksadjlkfsjlkdfasjfksadjfsadjkfsIT", Ball.Position, glm::vec4(0, 0, 0, 1));
		this->Club.Position = this->Ball.Position + glm::vec3(0, 0, -1);
	}
	this->Ball.Draw();
}

void PlayerCamera::CheckCollisions()
{
	/// CHECK IF BALL IS COLLIDING WITH WALL OR FLOOR

	/// CHECK IF CLUB IS COLLIDING WITH WALL

}
