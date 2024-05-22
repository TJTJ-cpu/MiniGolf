#pragma once
#include "config.h"
#include "render/model.h"

class GolfBall
{
public:
	GolfBall();
	GolfBall(glm::vec3 Pos, const char* md);

	glm::vec3 Position = glm::vec3(0,0,0);
	glm::vec3 PrevPosition = glm::vec3(0,0,0);
	glm::vec3 Velocity = glm::vec3(0,0,0);
	glm::vec3 Acceleration = glm::vec3(0,0,0);
	glm::mat4 Transform;
	Render::ModelId Model;

	float Speed;
	float HeightOfTheLastFrameOfTheBall;
	float CurrGravity = 5.5f;
	float FrictionForce = 0.9;
	float BallRadius = 0.05;
	bool bGrounded = false;

	void Draw();
	void Update(float dt);
	bool IsGrounded();
	bool IsWallHit();
	void HandlePhysics(float dt);
	void AddForce(glm::vec3 force);
};
