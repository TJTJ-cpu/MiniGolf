#pragma once
#include <vector>
#include "render/model.h"
#include "config.h"
#include <tuple>

struct MyTile
{
	Render::ModelId Model;
	glm::mat4 Transform;
	Physics::ColliderId Collider;
};

class TileManager
{
public:
	TileManager() {}
	std::vector<MyTile> PlatformTiles;

	void SpawnMap(int width, std::string Map, std::vector<int> Rotations)
	{
		Render::ModelId Flag = Render::LoadModel("assets/golf/flag-red.glb");
		Render::ModelId HoleOpen = Render::LoadModel("assets/golf/hole-open.glb");
		Render::ModelId Corner = Render::LoadModel("assets/golf/corner.glb");
		Render::ModelId Start = Render::LoadModel("assets/golf/start.glb");
		Render::ModelId Castle = Render::LoadModel("assets/golf/castle.glb");
		Render::ModelId Open = Render::LoadModel("assets/golf/open.glb");
		Render::ModelId Side = Render::LoadModel("assets/golf/side.glb");
		Render::ModelId Windmill = Render::LoadModel("assets/golf/windmill.glb");
		Render::ModelId BlueGolfBall = Render::LoadModel("assets/golf/ball-blue.glb");
		Render::ModelId Tunnel = Render::LoadModel("assets/golf/straight.glb");
		Render::ModelId HoleRound = Render::LoadModel("assets/golf/hole-round.glb");

		Physics::ColliderMeshId mHoleOpen = Physics::LoadColliderMesh("assets/golf/hole-open.glb");
		Physics::ColliderMeshId mHoleRound = Physics::LoadColliderMesh("assets/golf/hole-round.glb");
		Physics::ColliderMeshId mCastle = Physics::LoadColliderMesh("assets/golf/castle.glb");
		Physics::ColliderMeshId mStart = Physics::LoadColliderMesh("assets/golf/start.glb");
		Physics::ColliderMeshId mOpen = Physics::LoadColliderMesh("assets/golf/open.glb");
		Physics::ColliderMeshId mBlueGolfBall = Physics::LoadColliderMesh("assets/golf/ball-blue.glb");
		Physics::ColliderMeshId mSide = Physics::LoadColliderMesh("assets/golf/side.glb");
		Physics::ColliderMeshId mCorner = Physics::LoadColliderMesh("assets/golf/corner.glb");
		Physics::ColliderMeshId mTunnel = Physics::LoadColliderMesh("assets/golf/straight.glb");

		int height;
		height = Map.length() / width;

		float offSet = 0.95;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				glm::vec3 Location(x * offSet, 0, y * offSet);
				glm::mat4 Transform = glm::translate(Location) * glm::rotate(Rotations[y * width + x] * 3.141592653f / 2, glm::vec3(0, 1, 0));

				MyTile temp;
				std::cout << "x: " << x << " width: " << width << " y: " << y << " = " << y * width + x << std::endl;
				if (Map[y * width + x] == 'c') {
					temp.Collider = Physics::CreateCollider(mCorner, Transform);
					temp.Model = Corner;
					temp.Collider;
				}
				else if (Map[y * width + x] == 'C') {
					temp.Collider = Physics::CreateCollider(mCastle, Transform);
					temp.Model = Castle;
				}
				else if (Map[y * width + x] == 'h') {
					temp.Collider = Physics::CreateCollider(mHoleRound, Transform);
					temp.Model = HoleRound;
				}
				else if (Map[y * width + x] == 't') {
					temp.Collider = Physics::CreateCollider(mTunnel, Transform);
					temp.Model = Tunnel;
				}
				else if (Map[y * width + x] == 'S') {
					temp.Collider = Physics::CreateCollider(mSide, Transform);
					temp.Model = Side;
				}
				else if (Map[y * width + x] == 'w') {
					temp.Collider = Physics::CreateCollider(mOpen, Transform);
					temp.Model = Windmill;
				}
				else if (Map[y * width + x] == 'H') {
					temp.Collider = Physics::CreateCollider(mHoleOpen, Transform);
					temp.Model = HoleOpen;

				}
				else if (Map[y * width + x] == 'o') {
					temp.Collider = Physics::CreateCollider(mOpen, Transform);
					temp.Model = Open;
				}
				else if (Map[y * width + x] == ' ') {
					continue;
				}
				else
					std::cout << "ERROR::TileManager::VoidSpawnMap" << std::endl;
				temp.Transform = Transform;
				PlatformTiles.push_back(temp);
				if (Map[y * width + x] == 'H') {
					temp.Model = Flag;
					temp.Transform = Transform;
					PlatformTiles.push_back(temp);
				}
			}
		}
	}
};
