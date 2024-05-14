#pragma once

#include "config.h"
#include "render/model.h"

class GolfClub
{
public:
	GolfClub();
	GolfClub(glm::vec3 pos, const char* mod, float dis);

	glm::vec3 Position;
	glm::vec3 Velocity = glm::vec3(0,0,0);
	glm::vec3 Acceleration= glm::vec3(0,0,0);
	glm::mat4 Transform;
	glm::quat Rotation;

	Render::ModelId Model;

	bool bIsMovingTowardBall = false;
	float DistanceFromGolfBall;
	float MaxDistance;

	void Draw();
	void CheckCollisions();
	void AddForce(glm::vec3 f);
	void HandlePhysics(float dt);
};
