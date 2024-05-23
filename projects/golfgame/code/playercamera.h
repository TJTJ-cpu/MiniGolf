#pragma once
#include "golfball.h"
#include "golfclub.h"
#include "gamepad.h"
#include <vector>

class PlayerCamera
{
public:
	PlayerCamera();
	PlayerCamera(GolfInput::Gamepad* gm, glm::vec3 pos, glm::vec3 offset, glm::vec3 rot);

	glm::mat4 Transform;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 OffSet;

	/// KINDA GROSS TO HAVE IT EXPLICITLY BE A PART OF THE PLAYER BUT WE CAN LOOK AT IT LATER AND SEE IF WE ARE WISER
	GolfInput::Gamepad* gamepad;

	GolfBall Ball;
	GolfClub Club;

	glm::vec3 BallHitDirection;

	/// CAMERA VARIABLES
	void (PlayerCamera::* HandleClubAndCameraInput)(float) = &PlayerCamera::UpdateCameraTopDown;
	float OrbitPoint = 0;
	float OrbitSpeed = 5;
	float OrbitDistance = 3.0f;
	float MinOrbitDistance = 1.0f;
	float MaxOrbitDistance = 5.0f;
	float ZoomSpeed = 0.1f;

	const float LowGravity = 9.8f;
	const float HighGravity = 50.5f;

	float MovementSpeed = 5.0f;
	float ClubMovementSpeed = 4.2f;
	float VerticleSpeed = 2.0f;
	float DistanceFromClubToTheGolfBall;

	float CurrentTime = 0;
	int Score = 0;
	bool IsGameWon = false;
	bool bStartGame = true;

	std::string Name = "AAA";
	std::vector<std::string> OldScores;
	int currentNameIndex = 0;


	void Update(float DeltaSeconds);
	void Draw();
	void CheckCollisions();
	void UpdateCameraThirdPerson(float dt);
	void UpdateCameraTopDown(float dt);

	void ResetGame(glm::vec3 SpawnPosition);

	//void EnterHighScoreName(std::string MapName);
	//void WriteScoreToFile(std::string MapName);

	//void RenderHighScore(NVGcontext* vg);
	//void RenderHighScoreRanking(NVGcontext* vg);
	//void RenderScore(NVGcontext* vg);
	//void RenderOldScore(NVGcontext* vg);

	//std::vector<std::string> GetOldScore(std::string MapName);
};
