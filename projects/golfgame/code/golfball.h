#pragma once
#include "config.h"
#include "render/model.h"

class GolfBall
{
public:
	GolfBall();
	GolfBall(glm::vec3 Pos, const char* md);
	glm::vec3 Position;
	glm::mat4 Transform;
	Render::ModelId Model;

	float Speed;

	void Draw();
	void Update(float dt);
	void CheckCollisions();
};
