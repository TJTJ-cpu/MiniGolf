#pragma once
#include <vector>
#include "render/model.h"
#include "config.h"
#include <tuple>
#include <string>
#include <map>
#include <iostream>

#include <bitset>
#include <bit>

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
		Render::ModelId SplitT = Render::LoadModel("assets/golf/split-t.glb");
		Render::ModelId End = Render::LoadModel("assets/golf/end.glb");

		Physics::ColliderMeshId mHoleOpen = Physics::LoadColliderMesh("assets/golf/hole-open.glb");
		Physics::ColliderMeshId mHoleRound = Physics::LoadColliderMesh("assets/golf/hole-round.glb");
		Physics::ColliderMeshId mCastle = Physics::LoadColliderMesh("assets/golf/castle.glb");
		Physics::ColliderMeshId mStart = Physics::LoadColliderMesh("assets/golf/start.glb");
		Physics::ColliderMeshId mOpen = Physics::LoadColliderMesh("assets/golf/open.glb");
		Physics::ColliderMeshId mBlueGolfBall = Physics::LoadColliderMesh("assets/golf/ball-blue.glb");
		Physics::ColliderMeshId mSide = Physics::LoadColliderMesh("assets/golf/side.glb");
		Physics::ColliderMeshId mCorner = Physics::LoadColliderMesh("assets/golf/corner.glb");
		Physics::ColliderMeshId mInnerCorner = Physics::LoadColliderMesh("assets/golf/inner-corner.glb");
		Physics::ColliderMeshId mSquareCornerA = Physics::LoadColliderMesh("assets/golf/square-corner-a.glb");
		Physics::ColliderMeshId mTunnel = Physics::LoadColliderMesh("assets/golf/straight.glb");
		Physics::ColliderMeshId mSplitT = Physics::LoadColliderMesh("assets/golf/split-t.glb");
		Physics::ColliderMeshId mEnd = Physics::LoadColliderMesh("assets/golf/end.glb");

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
				else if (Map[y * width + x] == 'i') {
					temp.Collider = Physics::CreateCollider(mInnerCorner, Transform);
					temp.Model = InnerCorner;
				}
				else if (Map[y * width + x] == 'a') {
					temp.Collider = Physics::CreateCollider(mSquareCornerA, Transform);
					temp.Model = SquareCornerA;
				}
				else if (Map[y * width + x] == 'x') {
					temp.Collider = Physics::CreateCollider(mSplitT, Transform);
					temp.Model = SplitT;
				}
				else if (Map[y * width + x] == 'e') {
					temp.Collider = Physics::CreateCollider(mEnd, Transform);
					temp.Model = End;
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
		std::uint8_t b = a;
		std::bitset<8> i(b);
		std::cout << i << "\n";
		return std::rotr(b, 2);

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
			std::cout << "OUR INFO: ";
			std::bitset<8> x(a);
			std::cout << x << "\n";
		}

		std::bitset<8> x(a);
		std::cout << x << "\n";

		return a;
	}

	char CharGetNeighbors(std::string Map, int Width, int X, int Y)
	{
		char c = 0;

		/// THE NEIGHBORS OF THE POINT IN A SPIRAL STARTING FROM THE TOP IN A CLOCKWISE ORDER

		/// 701
		/// 6.2
		/// 543

		std::vector<std::pair<int, int>> Spiral = { { X, Y - 1 }, {X + 1, Y - 1}, {X + 1, Y}, {X + 1, Y + 1}, {X, Y + 1}, {X - 1, Y + 1}, {X - 1, Y}, {X - 1, Y - 1} };
		int i = 7;
		for (auto Point : Spiral)
		{
			c |= (GetChar(Map, Width, Point.first, Point.second) == 'o') << i;
			i--;
		}

		//c |= (GetChar(Map, Width, X, Y - 1) == 'o') << 7;
		//c |= (GetChar(Map, Width, X + 1, Y - 1) == 'o') << 6;
		//c |= (GetChar(Map, Width, X + 1, Y) == 'o') << 5;
		//c |= (GetChar(Map, Width, X + 1, Y + 1) == 'o') << 4;
		//c |= (GetChar(Map, Width, X, Y + 1) == 'o') << 3;
		//c |= (GetChar(Map, Width, X - 1, Y + 1) == 'o') << 2;
		//c |= (GetChar(Map, Width, X - 1, Y) == 'o') << 1;
		//c |= GetChar(Map, Width, X - 1, Y - 1) == 'o';
		std::bitset<8> x(c);
		std::cout << x << "\n";
		return c;
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

		CharGetNeighbors(Map, Width, 2, 2);
		CharGetNeighbors(Map, Width, 0, 0);

		std::map<char, char> Shapes = {
			/// OPEN
			{ 0b11111111, 'o' },

			/// INNER CORNER
			{ 0b10111111, 'i' },

			/// SIDE
			{ 0b11011111, 'S' },
			{ 0b10011111, 'S' },
			{ 0b11001111, 'S' },
			{ 0b10001111, 'S' },

			/// CORNER
			// { 0b10000011, 'c' },
			// { 0b11000011, 'c' },
			// { 0b10010011, 'c' },
			// { 0b11010011, 'c' },
			// { 0b10000111, 'c' },
			// { 0b11000111, 'c' },
			// { 0b10010111, 'c' },
			// { 0b11010111, 'c' },
			{ 0b00001110, 'c' },
			{ 0b00001111, 'c' },
			{ 0b01001110, 'c' },
			{ 0b01001111, 'c' },
			{ 0b00011110, 'c' },
			{ 0b00011111, 'c' },
			{ 0b01011110, 'c' },
			{ 0b01011111, 'c' },

			/// STRAIGHT
			{ 0b00100010, 't' },
			{ 0b00100011, 't' },
			{ 0b01100010, 't' },
			{ 0b01100011, 't' },
			{ 0b00110010, 't' },
			{ 0b00110011, 't' },
			{ 0b01110010, 't' },
			{ 0b01110011, 't' },
			{ 0b00100110, 't' },
			{ 0b00100111, 't' },
			{ 0b01100110, 't' },
			{ 0b01100111, 't' },
			{ 0b00110110, 't' },
			{ 0b00110111, 't' },
			{ 0b01110110, 't' },
			{ 0b01110111, 't' },

			/// END
			{ 0b00100000, 'e' },
			{ 0b00110000, 'e' },
			{ 0b00100100, 'e' },
			{ 0b00110100, 'e' },
			{ 0b00100001, 'e' },
			{ 0b00110001, 'e' },
			{ 0b00100101, 'e' },
			{ 0b00110101, 'e' },
			{ 0b00000110, 'e' },
			{ 0b00000111, 'e' },
			{ 0b01000110, 'e' },
			{ 0b01000111, 'e' },
			{ 0b00010110, 'e' },
			{ 0b00010111, 'e' },
			{ 0b01010110, 'e' },
			{ 0b01010111, 'e' },

			/// SQUARE A
			{ 0b10000010, 'a' },
			{ 0b11000010, 'a' },
			{ 0b10010010, 'a' },
			{ 0b11010010, 'a' },
			{ 0b10000110, 'a' },
			{ 0b11000110, 'a' },
			{ 0b10010110, 'a' },
			{ 0b11010110, 'a' },

			/// SPLIT T
			{ 0b10100010, 'x' },
			{ 0b10110010, 'x' },
			{ 0b10100110, 'x' },
			{ 0b10110110, 'x' },
		};

		for (int x = 0; x < Width; x++)
		{
			for (int y = 0; y < Map.length() / Width; y++)
			{
				if (GetChar(Map, Width, x, y) == 'o')
				{
					bool FoundSomething = false;
					char Neighbors = CharGetNeighbors(Map, Width, x, y);
					for (int i = 0; i < 4; i++)
					{
						if (Shapes.find(Neighbors) != Shapes.end())
						{
							FoundSomething = true;
							Result[y * Width + x] = Shapes.at(Neighbors);
							IntRotations.push_back(3 - i);

							break;
						}
						Neighbors = RotateNeighbors(Neighbors);
					}

					if (!FoundSomething)
					{
						IntRotations.push_back(0);
					}
				}
				else
				{
					Result[y * Width + x] = ' ';
					IntRotations.push_back(0);
				}
			}
		}

		for (int y = 0; y < Map.size()/Width; y++)
		{
			for (int x = 0; x < Width; x++)
			{
				std::cout << IntRotations[y * Width + x] << " ";
			}
			std::cout << "\n";
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
