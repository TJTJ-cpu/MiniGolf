#include "golfball.h"
#include "render/physics.h"
#include <iostream>
#include "render/debugrender.h"

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
	float c = 0.3;
	glm::vec3 Friction = -glm::normalize(Velocity) * c;
	if (glm::length(Velocity) < 0.005) {
		Velocity.x = 0;
		Velocity.z = 0;
	}
	if (glm::length(Velocity) > 0) 
		AddForce(Friction);
	IsWallHit();
	IsGrounded();
	Transform = glm::translate(Position);
}

bool GolfBall::IsGrounded()
{
	Physics::RaycastPayload HitResult;
	float BallRadius = 0.10;

	bool bGrounded = false;

	std::vector<glm::vec3> GroundedDirections = { glm::vec3(0,-1,0), glm::vec3(0.33,-1,0), glm::vec3(-0.33,-1,0), glm::vec3(0,-1,0.33), glm::vec3(0,-1,-0.33) };
	for (auto Direction : GroundedDirections)
	{
		HitResult = Physics::Raycast(this->Position, Direction, BallRadius);
		if (HitResult.hit)
			bGrounded = true;
	}
	std::cout << std::boolalpha << bGrounded << std::endl;
	if (!bGrounded)
		this->AddForce(glm::vec3(0, -3.5, 0));
	else
		Velocity.y = 0;

	return bGrounded;
}

bool GolfBall::IsWallHit()
{
	Physics::RaycastPayload HitResult;
	Physics::RaycastPayload HitResultExtendedCheck;

	std::vector<glm::vec3> WallDirections;

	float BallRadius = 0.07;
	bool bIsWalled = false;
	
	/// Create rays in all directions
	for (float i = 0; i < 2 * 3.141592f; i += (2 * 3.141592f) / 120)
		WallDirections.push_back(glm::vec3(glm::cos(i), 0, glm::sin(i)));
	
	for (glm::vec3 &Direction : WallDirections) {
		HitResult = Physics::Raycast(this->Position, Direction, BallRadius);
		Physics::RaycastPayload TESTDELETETHIS = Physics::Raycast(this->Position, Direction, 5);
		if (TESTDELETETHIS.hit)
			Debug::DrawLine(this->Position, TESTDELETETHIS.hitPoint, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
		else
			Debug::DrawLine(this->Position, this->Position + Direction * 5.0f, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
		if (HitResult.hit) {
			glm::vec3 Dir = Direction + glm::vec3(0.001, 0, 0.001);
			HitResultExtendedCheck = Physics::Raycast(this->Position, Dir, 1);
			if (HitResultExtendedCheck.hit && !bIsWalled) {
				glm::vec3 tangent = HitResult.hitPoint - HitResultExtendedCheck.hitPoint;
				glm::vec3 normal = glm::normalize(glm::cross(tangent, glm::vec3(0, 1, 0)));
				if (glm::dot((this->Position - HitResult.hitPoint), normal) < 0) {
					normal *= -1;
				}
				std::cout << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
				if (glm::dot(this->Velocity, normal) < 0) {
					this->Velocity = Velocity - 2.0f * (glm::dot(Velocity, normal) * normal);
					bIsWalled = true;
				}
			}
		}
	}
	return HitResult.hit;
}

void GolfBall::HandlePhysics(float dt)
{
	Velocity += Acceleration * dt;
	Position += Velocity * dt;
	Acceleration = glm::vec3(0,0,0);
}

void GolfBall::AddForce(glm::vec3 force)
{
	Acceleration += force;
}
