#pragma once
#include "golfball.h"
#include "golfclub.h"
#include "gamepad.h"

class PlayerCamera
{
public:
	PlayerCamera();
	PlayerCamera(GolfInput::Gamepad* gm, glm::vec3 pos, glm::vec3 offset, glm::vec3 rot);

	glm::mat4 Transform;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 OffSet;

	GolfInput::Gamepad* gamepad;
	GolfBall Ball;
	GolfClub Club;

	glm::vec3 BallHitDirection;

	/// THIRD PERSON CAMERA VARIABLES
	bool ThirdPersonCam = false;
	bool FirstPress = true;
	bool IsGameWon = false;
	bool bIsPress = false;
	float CurrentTime = 0;
	float OrbitPoint = 0;
	float OrbitSpeed = 5;
	float OrbitDistance = 3.0f;
	float MinOrbitDistance = 1.0f;
	float MaxOrbitDistance = 5.0f;
	float ZoomSpeed = 0.1f;
	int Score = 0;

	const float LowGravity = 5.5f;
	const float HighGravity = 50.5f;

	float MovementSpeed = 5.0f;
	float ClubMovementSpeed = 4.2f;
	float VerticleSpeed = 2.0f;
	float DistanceFromClubToTheGolfBall;

	std::string Name = "AAA";
	int currentNameIndex = 0;

	void Update(float DeltaSeconds);
	void Draw();
	void CheckCollisions();
	void EnterHighScoreName();
	void WriteScoreToFile();
	void RenderHighScore(NVGcontext* vg);
	void RenderScore(NVGcontext* vg);
};
