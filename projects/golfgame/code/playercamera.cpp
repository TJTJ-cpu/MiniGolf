#include "playercamera.h"
#include "config.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/debugrender.h"
#include <iostream>
#include <map>
#include <fstream>
#include <ostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

PlayerCamera::PlayerCamera()
{
}

PlayerCamera::PlayerCamera(GolfInput::Gamepad* gm, glm::vec3 pos, glm::vec3 offset, glm::vec3 rot) : Rotation(rot), gamepad(gm)
{
	Transform = glm::translate(pos);
	OffSet = offset;
	Position = OffSet;
	Ball = GolfBall(pos, "assets/golf/ball-blue.glb");
	Club = GolfClub(pos + glm::vec3(0, 0.7, -1), "assets/golf/club-blue.glb", 4);
}

void PlayerCamera::Update(float DeltaSeconds)
{
	CurrentTime += DeltaSeconds;

	Ball.Update(DeltaSeconds);

	float ClubBallHeightOffset = 0.7f;
	Club.Position.y = Ball.Position.y + ClubBallHeightOffset;

	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	/// DECLARE THIS SOMEWHERE ELSE LATER
	glm::vec3 BallPos = Ball.Position;
	glm::vec3 ClubPos = Club.Position;
	BallPos.y = 0;
	ClubPos.y = 0;


	/// SWITCH CAMERA PERSPECTIVE
	if (gamepad->Pressed[GolfInput::Gamepad::Button::RIGHT_SHOULDER_BUTTON]) {
		if (HandleClubAndCameraInput == &PlayerCamera::UpdateCameraThirdPerson)
			HandleClubAndCameraInput = &PlayerCamera::UpdateCameraTopDown;
		else
			HandleClubAndCameraInput = &PlayerCamera::UpdateCameraThirdPerson;
		OrbitPoint = 0;
	}

	if (!IsGameWon)
	{
		/// IF THE ACTION BUTTON IS PRESSED GO TOWARDS TO BALL
		if (gamepad->Pressed[GolfInput::Gamepad::Button::A_BUTTON] && !Club.bIsMovingTowardBall && (glm::length(Ball.Velocity) < 0.1)) {
			Club.bIsMovingTowardBall = true;
			DistanceFromClubToTheGolfBall = glm::distance(this->Ball.Position, this->Club.Position);
			BallHitDirection = glm::normalize(BallPos - ClubPos);
		}
	}

	/// A COMMAND PATTERN STYLE; HOPEFULLY PREFERABLE TO AN IF-ELSE
	(this->*HandleClubAndCameraInput)(DeltaSeconds);

	if (Club.bIsMovingTowardBall) {
		glm::vec3 norm = glm::normalize(BallPos - ClubPos);
		if (glm::distance(ClubPos, BallPos) > 0.1f) {
			Club.Position += norm * ClubMovementSpeed * DeltaSeconds;
			ClubPos = Club.Position;
			ClubPos.y = 0;
		}
		else {
			this->Ball.AddForce(BallHitDirection * 75.3f * (DistanceFromClubToTheGolfBall * DistanceFromClubToTheGolfBall));
			Club.bIsMovingTowardBall = false;
			Score++;
		}
	}

	/// KEEP CLUB IN RANGE
	if (!Club.bIsMovingTowardBall) {
		float Distance = glm::distance(BallPos, ClubPos);

		float MaxDistance = 2;
		if (Distance > MaxDistance) {
			glm::vec3 norm = glm::normalize((ClubPos - BallPos));
			norm *= MaxDistance - 0.01;
			Club.Position = Ball.Position + norm;
			Club.Position.y += ClubBallHeightOffset;
		}
	}

	Club.Rotation = glm::inverse(glm::lookAt(ClubPos, BallPos, glm::vec3(0, 1, 0))) * glm::rotate(3.141592f / 2, glm::vec3(0, 1, 0));
	Club.Transform = glm::translate(Club.Position) * (glm::mat4)Club.Rotation;

	/// Guided Predictive Line
	glm::vec3 BallVelNotY = Ball.Velocity;
	BallVelNotY.y = 0;
	if (glm::length(BallVelNotY) == 0 && Ball.bGrounded)
		Debug::DrawLine(BallPos + glm::vec3(0, 0.1f, 0), glm::vec3(0, 0.1f, 0) + ClubPos + (BallPos - ClubPos) * 1.5f, 2.0f, { 1,0,0,1 }, { 1,0,0,1 });

	this->Ball.HandlePhysics(DeltaSeconds);
	//std::cout << "Ball grounded: " << std::boolalpha << Ball.bGrounded << "\n";
	//std::cout << "Ball Velocity: " << glm::length(Ball.Velocity) << "\n";
	//std::cout << "Ball Position: x: " << Ball.Position.x << ", y: " << Ball.Position.y << ", z: " << Ball.Position.z << "\n";
}

void PlayerCamera::Draw()
{
	Debug::DrawLine(Ball.Position, Ball.Position + glm::vec3(0, 0.2, 0), 1.0f, glm::vec4(1, 0, 1, 1), glm::vec4(1, 1, 1, 1));

	if (!IsGameWon && Ball.bGrounded && glm::length(this->Ball.Velocity) < 0.05) {
		this->Club.Draw();
	}
	else {
		this->Club.Position = this->Ball.Position + glm::vec3(0, 0, -1);
	}

	this->Ball.Draw();
}



void PlayerCamera::CheckCollisions()
{
	/// CHECK IF BALL IS COLLIDING WITH WALL OR FLOOR

	/// CHECK IF CLUB IS COLLIDING WITH WALL

}

void PlayerCamera::UpdateCameraThirdPerson(float dt)
{
	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	glm::vec3 TargetPosition;

	OrbitPoint += dt * gamepad->RightStick.x * OrbitSpeed;

	/// GET INPUT IN THE FORM [0, 1]
	float ZoomInAmount = ((gamepad->RightTrigger + 1) / 2);
	float ZoomOutAmount = ((gamepad->LeftTrigger + 1) / 2);

	/// COOL WAY TO EXPRESS ZOOMING
	OrbitDistance = std::min(MaxOrbitDistance, std::max(MinOrbitDistance, OrbitDistance + ZoomSpeed * (ZoomOutAmount - ZoomInAmount)));

	/// BORING WAY
	//if (OrbitDistance < 5)
		//OrbitDistance += ZoomSpeed * ZoomOutAmount;
	//if (OrbitDistance > 1)
		//OrbitDistance -= ZoomSpeed * ZoomInAmount;

	OffSet = glm::vec3(0, OrbitDistance / 2, 0);

	TargetPosition = Ball.Position + OffSet;
	TargetPosition.x -= glm::cos(OrbitPoint) * OrbitDistance;
	TargetPosition.z -= glm::sin(OrbitPoint) * OrbitDistance;

	Position = mix(Position, TargetPosition, std::min(0.1f, dt * 30.0f));

	cam->view = glm::lookAt(Position, Ball.Position, glm::vec3(0, 1, 0));

	glm::vec3 ClubMovement = glm::vec3(0, 0, 0);

	ClubMovement.x += dt * gamepad->LeftStick.x * VerticleSpeed;
	ClubMovement.z -= dt * gamepad->LeftStick.y * VerticleSpeed;


	Club.Position += glm::vec3(glm::inverse(cam->view) * glm::vec4(ClubMovement, 0));
}

void PlayerCamera::UpdateCameraTopDown(float dt)
{
	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	OffSet = glm::vec3(0, 2, 0);

	/// MOVE THE CAMERA WITH THE RIGHT STICK
	Position.x += dt * gamepad->RightStick.y * MovementSpeed;
	Position.z += dt * gamepad->RightStick.x * MovementSpeed;

	/// MOVE THE GOLF CLUB WITH GAMEPAD LEFT STICK
	if (!Club.bIsMovingTowardBall)
	{
		Club.Position.x += dt * gamepad->LeftStick.y * VerticleSpeed;
		Club.Position.z += dt * gamepad->LeftStick.x * VerticleSpeed;
	}

	if (gamepad->RightStick.x == 0 && gamepad->RightStick.y == 0) {
		Position = mix(Position, Ball.Position + OffSet, std::min(0.1f, dt * 30.0f));
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

void PlayerCamera::ResetGame(glm::vec3 SpawnPosition)
{
	Name = "AAA";
	Score = 0;
	Ball.Position = SpawnPosition;
	Ball.Velocity = { 0,0,0 };
	Ball.Acceleration = { 0,0,0 };
	IsGameWon = false;
}
