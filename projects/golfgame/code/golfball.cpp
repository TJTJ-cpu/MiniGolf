#include "golfball.h"
#include "render/physics.h"
#include <iostream>


GolfBall::GolfBall() 
{
	Position = glm::vec3(1, 1, 1);
	Transform = glm::translate(Position);
	Model = Render::LoadModel("assets/golf/ball-green.glb");
	Speed = 5.0f;
}

GolfBall::GolfBall(glm::vec3 Pos, const char* md) : Position(Pos)  
{ 
	Transform = glm::translate(Pos);
	Model = Render::LoadModel(md);
	Speed = 5.0f;
}

void GolfBall::Draw()
{
	Render::RenderDevice::Draw(Model, Transform);
}


void GolfBall::Update(float dt)
{
	Velocity *= FrictionForce;
	this->Position += Velocity;
	if (!CheckCollisions()) {
		//this->Position.y -= dt * Gravity;
	}
	Transform = glm::translate(Position);
}

bool GolfBall::CheckCollisions()
{
	Physics::RaycastPayload HitResult;
	HitResult = Physics::Raycast(this->Position, glm::vec3(0, -1, 0), 0.07);
	std::cout << "Hit: " << HitResult.hit << std::endl;
	return HitResult.hit;
}
