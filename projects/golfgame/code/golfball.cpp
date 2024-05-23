#include "golfball.h"
#include "render/physics.h"
#include <iostream>
#include "render/debugrender.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"

GolfBall::GolfBall() 
{
	Position = glm::vec3(1, 1, 1);
	PrevPosition = glm::vec3(1, 1, 1);
	Transform = glm::translate(Position);
	Model = Render::LoadModel("assets/golf/ball-green.glb");
	Speed = 5.0f;
}

GolfBall::GolfBall(glm::vec3 Pos, const char* md) : Position(Pos)  
{ 
	PrevPosition = Pos;
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
	/// FRICTION CONSTANT
	float c = 0.3;
	glm::vec3 Friction = -glm::normalize(Velocity) * c;

	/// IF THE VELOCITY IS CLOSE TO ZERO JUST SET IT TO ZERO
	if (glm::length(Velocity) < 0.005) {
		Velocity.x = 0;
		Velocity.z = 0;
	}

	/// ALWAYS APPLY GRAVITY
	this->AddForce(glm::vec3(0, -CurrGravity, 0));

	/// IF THE BALL IS GROUNDED ADD THE NORMAL FORCE FROM THE GROUND
	if (IsGrounded()) {
		this->AddForce(glm::vec3(0, CurrGravity, 0));
	}

	/// ONLY ADD FRICTION IF THE BALL IS GROUNDED
	if (bGrounded && glm::length(Velocity) > 0) 
		AddForce(Friction);

	Transform = glm::translate(Position);
}

bool GolfBall::IsGrounded()
{
	Physics::RaycastPayload HitResult;

	glm::vec3 GroundedDirection = glm::vec3(0,-1,0);
	HitResult = Physics::Raycast(this->Position, GroundedDirection, BallRadius);
	bGrounded = HitResult.hit;

	/// PUT THE BALL *ON* THE GROUND IF IT IS *IN* THE GROUND 
	/// (PROBABLY A BIT BROKEN AND DUMB RIGHT NOW)
	if (HitResult.hitDistance < BallRadius)
		Position.y += BallRadius - HitResult.hitDistance - 0.01f;

	return bGrounded;
}

bool GolfBall::IsWallHit()
{
	Physics::RaycastPayload HitResult;
	Physics::RaycastPayload HitResultExtendedCheck;

	std::vector<glm::vec3> WallDirections;

	bool bIsWalled = false;

	/// Create rays in all directions
	for (float i = 0; i < 2 * 3.141592f; i += (2 * 3.141592f) / 120)
		WallDirections.push_back(glm::vec3(glm::cos(i), 0, glm::sin(i)));
	
	glm::vec3 rayOrigin = this->Position + glm::vec3(0.0f,-0.03f,0.0f);

	for (int i = 0; i < WallDirections.size(); i++) {
		glm::vec3 &Direction = WallDirections[i];
		HitResult = Physics::Raycast(rayOrigin, Direction, BallRadius);
		Physics::RaycastPayload TESTDELETETHIS = Physics::Raycast(rayOrigin, Direction, 5);
		if (TESTDELETETHIS.hit)
			Debug::DrawLine(rayOrigin, TESTDELETETHIS.hitPoint, 2.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::Normal);
		else
			Debug::DrawLine(rayOrigin, rayOrigin + Direction * 5.0f, 2.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::Normal);
		if (HitResult.hit) {
			glm::vec3 Dir = Direction + glm::vec3(0.001, 0, 0.001);
			HitResultExtendedCheck = Physics::Raycast(rayOrigin, Dir, 1);
			if (HitResultExtendedCheck.hit && !bIsWalled) {
				glm::vec3 tangent = HitResult.hitPoint - HitResultExtendedCheck.hitPoint;
				glm::vec3 normal = glm::normalize(glm::cross(tangent, glm::vec3(0, 1, 0)));
				if (glm::dot((rayOrigin - HitResult.hitPoint), normal) < 0) {
					normal *= -1;
				}
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

	std::vector<glm::vec3> RaycastPoints;

	if (glm::length(Velocity) < 0.01f)
		Velocity = { 0,0,0 };
	//Position += Velocity * dt;

	/// VARIABLES FOR REFLECTING
	glm::vec3 CurrentPosition = Position;
	glm::vec3 TargetPosition = Position + Velocity * dt;


	/// REFLECT VELOCITY IF WE ARE MOVING INTO OR THROUGH A WALL
	Physics::RaycastPayload NextHit;
	NextHit = Physics::Raycast(Position, glm::normalize(Velocity), glm::length(Velocity * dt) + BallRadius);
	if (NextHit.hit) {
		do
		{
			RaycastPoints.push_back(CurrentPosition);

			glm::vec3 Normal = glm::normalize(NextHit.hitNormal);
			float RemainingDistance = glm::length(Velocity * dt) - NextHit.hitDistance;
			Velocity = glm::reflect(Velocity, Normal) * 0.85f;
			///
			Velocity.y *= 0.1f;
			///
			TargetPosition = NextHit.hitPoint + glm::normalize(Velocity) * BallRadius + glm::normalize(Velocity) * RemainingDistance;
			NextHit = Physics::Raycast(CurrentPosition + Normal * BallRadius, glm::normalize(Velocity), glm::length(Velocity * dt) + BallRadius);
			CurrentPosition = TargetPosition;
		} while (NextHit.hit);
		RaycastPoints.push_back(CurrentPosition);

		Position = TargetPosition;
		RaycastPoints.push_back(TargetPosition);
	}
	else 
		Position += Velocity * dt;

	ContinousRaycastPoints = RaycastPoints;

	Transform = glm::translate(Position);

	Acceleration = glm::vec3(0,0,0);
}

void GolfBall::AddForce(glm::vec3 force)
{
	Acceleration += force;
}
