#pragma once
#include <vector>
#include "render/model.h"
#include "config.h"
#include <tuple>
#include <string>

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
	std::string tMap;
	int tWidth;
	glm::vec3 BallSpawn;
	std::string Name;

	void SpawnMap(std::string MapName, int width, std::string Map, std::vector<int> Rotations, glm::vec3 Spawn)
	{
		Name = MapName;
		tMap = Map;
		tWidth = width;
		BallSpawn = Spawn;

		Render::ModelId Flag = Render::LoadModel("assets/golf/flag-red.glb");
		Render::ModelId HoleOpen = Render::LoadModel("assets/golf/hole-open.glb");
		Render::ModelId Corner = Render::LoadModel("assets/golf/corner.glb");
		Render::ModelId SquareCornerA = Render::LoadModel("assets/golf/square-corner-a.glb");
		Render::ModelId InnerCorner = Render::LoadModel("assets/golf/inner-corner.glb");
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

		float offSet = 1.0f;
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
				//if (Map[y * width + x] == 'H') {
				//	temp.Model = Flag;
				//	temp.Transform = Transform;
				//	PlatformTiles.push_back(temp);
				//}
			}
		}
	}

	char GetChar(std::string Map, int Width, int X, int Y)
	{
		if (X < 0 || X >= Width || Y < 0 || Y >= (Map.length() / Width))
			return ' ';
		char t = Map[Y * Width + X];
		return t;
	}

	std::string RotateCounterClockwise(std::string Map, int Width)
	{
		std::string Result = Map;

		Result[0] = Map[6];
		Result[1] = Map[3];
		Result[2] = Map[0];

		Result[3] = Map[7];
		Result[4] = Map[4];
		Result[5] = Map[1];

		Result[6] = Map[8];
		Result[7] = Map[5];
		Result[8] = Map[2];

		return Result;
	}

	char RotateNeighbors(char a)
	{
		/// Do Twice
		for (float i = 1.0; i > 0.4; i /= 2.0)
		{
			if (a % 2 != 0)
			{
				a = a >> 1;
				a |= 128;
			}
			else
				a = a >> 1;
		}

		return a;
	}

	std::string GetNeighbors(std::string Map, int Width, int X, int Y)
	{
		std::string Result = "";

		for (int x = X - 1; x <= X + 1; x++)
		{
			for (int y = Y - 1; y <= Y + 1; y++)
			{
				Result += GetChar(Map, Width, x, y);
			}
		}

		return Result;
	}

	std::pair<std::string, std::vector<int>> GenerateTileMap(std::string Map, int Width)
	{
		std::string Result = Map;
		std::string Rotations = Map;
		std::vector<int> IntRotations;
		for (int i = 0; i < Map.length(); i++)
			if (Result[i] != ' ')
				Result[i] = '_';
			else if (Rotations[i] != ' ')
				Rotations[i] = '_';

		std::map<std::string, char> Shape = {
						{"ooooooooo", 'o'},
						{" o ooo o ", 'o'},
						{"oo ooo o ", 'o'},
						{" ooooo o ", 'o'},
						{"oooooo o ", 'o'},
						{"oooooo oo", 'o'},

						{"    oo oo", 'c'},
						{"ooo oo   ", 'c'},
						{"ooo oo   ", 'c'},
						{"o  oo oo ", 'c'},

						{"    o  o ", 'e'},
						{"ooo o    ", 'e'},
						{"oo  o    ", 'e'},
						{" oo o    ", 'e'},

						{"ooooo oo ", 'S'},
						{" o oo oo ", 'S'},
						{"   oooooo", 'S'},
						{"o ooooooo", 'S'},
						{"  ooooooo", 'S'},
						{" ooooo   ", 'S'},
						{"ooooooo  ", 'S'},
						{"   ooooo ", 'S'} };


		for (int x = 0; x < Width; x++)
		{
			for (int y = 0; y < Map.length() / Width; y++)
			{
				std::string Neighbors = GetNeighbors(Map, Width, x, y);
				for (int i = 0; i < 4; i++)
				{
					if (Shape.find(Neighbors) != Shape.end())
					{
						Result[y * Width + x] = Shape.at(Neighbors);
						IntRotations.push_back(i);

						if (i == 0)
							Rotations[y * Width + x] = '0';
						if (i == 1)
							Rotations[y * Width + x] = '1';
						if (i == 2)
							Rotations[y * Width + x] = '2';
						if (i == 3)
							Rotations[y * Width + x] = '3';

						break;
					}
					Neighbors = RotateCounterClockwise(Neighbors, Width);
				}
			}
		}

		return { Result, IntRotations };
	}

	char SearchWhereAmI(float xCoor, float zCoor) {
		char result;
		int x = xCoor + 0.5;
		int z = zCoor + 0.5;
		int Index = z * tWidth + x;
		if (Index >= tMap.length())
			return ' ';
		result = tMap[Index];
		return result;
	}
};

class MapManager
{
public:
	std::vector<TileManager> Maps;
	int CurrentMap = 0;

	void RegisterMap(TileManager Map)
	{
		Maps.push_back(Map);
	}

	void ChangeMap()
	{
		for (auto& tile : Maps[CurrentMap].PlatformTiles)
			Physics::SetTransform(tile.Collider, glm::translate(glm::vec3(99, 99, 99)));

		CurrentMap = ++CurrentMap % Maps.size();

		for (auto& tile : Maps[CurrentMap].PlatformTiles)
			Physics::SetTransform(tile.Collider, tile.Transform);
	}
};
