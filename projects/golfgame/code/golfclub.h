#pragma once

#include "config.h"
#include "render/model.h"

class GolfClub
{
public:
	GolfClub();
	GolfClub(glm::vec3 pos, const char* mod, float dis);

	glm::vec3 Position;
	glm::mat4 Transform;
	glm::quat Rotation;

	Render::ModelId Model;

	bool bIsMovingTowardBall = false;
	float DistanceFromGolfBall;
	float MaxDistance;

	void Draw();
};
