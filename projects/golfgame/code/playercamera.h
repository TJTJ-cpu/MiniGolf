#pragma once
#include "golfball.h"
#include "golfclub.h"

class PlayerCamera
{
public:
	PlayerCamera();
	PlayerCamera(glm::vec3 pos, glm::vec3 offset, glm::vec3 rot);

	glm::mat4 Transform;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 OffSet;

	GolfBall Ball;
	GolfClub Club;

	glm::vec3 BallHitDirection;

	float MovementSpeed = 5.0f;
	float ClubMovementSpeed = 4.2f;
	float VerticleSpeed = 2.0f;
	float DistanceFromClubToTheGolfBall;
	
	void Update(float DeltaSeconds);
	void Draw();
	void CheckCollisions();
};
