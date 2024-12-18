//------------------------------------------------------------------------------
// spacegameapp.cc
// (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include <map>
#include "golfball.h"
#include "golfgameapp.h"
#include <cstring>
#include "imgui.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "render/input/inputserver.h"
#include "core/cvar.h"
#include "render/physics.h"
#include <chrono>
#include <iostream>
#include "playercamera.h"
#include "tilemanager.h"

#include "gamepad.h"
#include "UI.h"
#include "Scoring.h"

using namespace Display;
using namespace Render;

namespace Game
{

//------------------------------------------------------------------------------
/**
*/
GolfGameApp::GolfGameApp()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GolfGameApp::~GolfGameApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
GolfGameApp::Open()
{
	App::Open();
	this->window = new Display::Window;
    //this->window->SetSize(1280, 720);
    this->window->SetSize(1920, 1080);


    if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        RenderDevice::Init();

		// set ui rendering function
		this->window->SetUiRender([this]()
		{
			this->RenderUI();
		});
        this->window->SetNanoFunc([this](NVGcontext* vg)
        {
            this->RenderNanoVG(vg);
        });
        
        return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
GolfGameApp::Run()
{
    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;

    //ModelId Square = LoadModel("asset/golf/square-corner-a.glb");

	int Three = 3;
	//std::string Map = "cScSoSSoSSoSSoSSHScSc";
	std::string Map = "xxxxxxxxx";
    int Four = 4;
    std::string Map1 = "ctc t ccC  tt ccSSS SCc cS   h  ";

    // THIS SHIT IS BOTTOM UP
	// FROM THE SPACESHIP'S SPAWN PERSPECTIVE X INCREASES LEFT AND Y UP
    std::vector<int> Map1Rotations = { 2,1,1,0,
                                    0,0,3,1,
                                    0,0,0,0,
                                    0,0,2,0,
                                    2,1,0,0,
                                    2,2,0,0,
                                    3,0,0,0,
                                    0,0,0,0,
    };
	//std::vector<int> Rotations = { 2,1,1,
	//								2,0,0,
	//								2,0,0,
	//								2,0,0,
	//								2,0,0,
	//								2,0,0,
	//								3,3,0 };
    std::vector<int> Rotations = { 0,1,2,
								3,0,0,
	0,0,0 };

	TileManager Tile;
	TileManager Tile2;
	TileManager Tile3;
	std::pair<std::string, std::vector<int>> MapInfo = Tile3.GenerateTileMap(	std::string("ooooo") +  
																							"o   o" + 
																							"o   o"
																							"o ooo"
																							"o o  "
																							"o o h"
																							"ooooo"
																							"o  oo"
																							"o   o"
																							"ooooo", 5);

	std::pair<std::string, std::vector<int>> MapInfo2 = Tile2.GenerateTileMap(	std::string("ooooo") +  
																							"o  oo" + 
																							"o ooo"
																							"o o o"
																							"ooo o"
																							"o ooo"
																							"o  oo"
																							"oh  o"
																							"o   o"
																							"ooooo", 5);

	Tile.SpawnMap("MAP1", Four, Map1, Map1Rotations, glm::vec3(2, 1, 0));
	Tile2.SpawnMap("MAP2", 5, MapInfo.first, MapInfo.second , glm::vec3(1, 1, 0));
	Tile3.SpawnMap("MAP3", 5, MapInfo2.first, MapInfo2.second, glm::vec3(1, 1, 0));

	MapManager mapManager;
	mapManager.RegisterMap(Tile);
	mapManager.RegisterMap(Tile2);
	mapManager.RegisterMap(Tile3);

	/// MOVES THE UNRELATED SHIT OUTTA THE WAY
	for (auto& tile : Tile2.PlatformTiles)
		Physics::SetTransform(tile.Collider, glm::translate(glm::vec3(99, 99, 99)));

	for (auto& tile : Tile3.PlatformTiles)
		Physics::SetTransform(tile.Collider, glm::translate(glm::vec3(99, 99, 99)));

	std::vector<TileManager> AllMaps = { Tile, Tile2, Tile3 };
	int CurrentMap = 0;

	struct ColliderId {
		std::vector<Physics::ColliderMeshId> CollisionShapes;
		glm::vec2 Coordinate;
	};

	// load all resources
	ModelId models[6] = {
		LoadModel("assets/golf/ball-blue.glb"),
		LoadModel("assets/golf/ball-red.glb"),
		LoadModel("assets/golf/ball-green.glb"),
		LoadModel("assets/golf/ball-blue.glb"),
		LoadModel("assets/golf/ball-red.glb"),
		LoadModel("assets/golf/ball-green.glb")
	};
	Physics::ColliderMeshId tileMeshCollisder[6] = {
		LoadModel("assets/golf/ball-blue.glb"),
		LoadModel("assets/golf/ball-red.glb"),
		LoadModel("assets/golf/ball-green.glb"),
		LoadModel("assets/golf/ball-blue.glb"),
		LoadModel("assets/golf/ball-red.glb"),
		LoadModel("assets/golf/ball-green.glb")
	};
	//Physics::ColliderMeshId colliderMeshes[6] = {
	//    Physics::LoadColliderMesh("assets/space/Asteroid_1_physics.glb"),
	//    Physics::LoadColliderMesh("assets/space/Asteroid_2_physics.glb"),
	//    Physics::LoadColliderMesh("assets/space/Asteroid_3_physics.glb"),
	//    Physics::LoadColliderMesh("assets/space/Asteroid_4_physics.glb"),
	//    Physics::LoadColliderMesh("assets/space/Asteroid_5_physics.glb"),
	//    Physics::LoadColliderMesh("assets/space/Asteroid_6_physics.glb")
	//};

	std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4>> asteroids;

	// Setup asteroids near
	for (int i = 0; i < 25; i++)
	{
		std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
		size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
		std::get<0>(asteroid) = models[resourceIndex];
		float span = 20.0f;
		glm::vec3 translation = glm::vec3(
			Core::RandomFloatNTP() * span,
			Core::RandomFloatNTP() * span,
			Core::RandomFloatNTP() * span
		);
		glm::vec3 rotationAxis = normalize(translation);
		float rotation = translation.x;
		float size = 50;
		glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation) * glm::scale(glm::vec3(size, size, size));
		//std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
		//transform *= glm::scale(glm::vec3(size, size, size));
		std::get<2>(asteroid) = transform;
		asteroids.push_back(asteroid);
	}


	// Setup asteroids far
	for (int i = 0; i < 20; i++) {
		std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
		size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
		std::get<0>(asteroid) = models[resourceIndex];
		float span = 80.0f;
		glm::vec3 translation = glm::vec3(
			Core::RandomFloatNTP() * span,
			Core::RandomFloatNTP() * span,
			Core::RandomFloatNTP() * span
		);
		glm::vec3 rotationAxis = normalize(translation);
		float rotation = translation.x;
		glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
		//std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
		std::get<2>(asteroid) = transform;
		asteroids.push_back(asteroid);
	}

	// Setup skybox
	std::vector<const char*> skybox
	{
		"assets/space/bg.png",
		"assets/space/bg.png",
		"assets/space/bg.png",
		"assets/space/bg.png",
		"assets/space/bg.png",
		"assets/space/bg.png"
	};

	TextureResourceId skyboxId = TextureResource::LoadCubemap("skybox", skybox, true);
	RenderDevice::SetSkybox(skyboxId);

	Input::Keyboard* kbd = Input::GetDefaultKeyboard();

	const int numLights = 4;
	Render::PointLightId lights[numLights];
	// Setup lights
	for (int i = 0; i < numLights; i++)
	{
		glm::vec3 translation = glm::vec3(
			Core::RandomFloatNTP() * 20.0f,
			Core::RandomFloatNTP() * 20.0f,
			Core::RandomFloatNTP() * 20.0f
		);
		glm::vec3 color = glm::vec3(
			Core::RandomFloat(),
			Core::RandomFloat(),
			Core::RandomFloat()
		);
		lights[i] = Render::LightServer::CreatePointLight(translation, color, Core::RandomFloat() * 4.0f, 1.0f + (15 + Core::RandomFloat() * 10.0f));
	}

	GolfInput::Gamepad gamepad;
	PlayerCamera GodEye(&gamepad, AllMaps[CurrentMap].BallSpawn, glm::vec3(0, 2, 0), glm::vec3(0, 0, 0));

	std::clock_t c_start = std::clock();
	double dt = 0.01667f;
	double dtReal = dt;

	bool DEBUGRealTimeUpdate = true;

	/// Collect the old scores from file
	GodEye.OldScores = Scoring::GetOldScore(AllMaps[CurrentMap].Name, GodEye.Name.size());

	bool ShowRanking = false;
	int PlayerRank = 0;

	/// game loop
	while (this->window->IsOpen())
	{
		gamepad.Update();
		UI::DrawFPS(window->vg, dtReal);

		/// THIS SHOULD PROBABLY BE MOVED TO THE PLAYER CAMERA; SOME SORT OF HANDLE INPUT METHOD MAYBE??
		if (gamepad.Pressed[GolfInput::Gamepad::Button::SELECT]) {
			GodEye.CurrentTime = 0;
			for (auto& tile : AllMaps[CurrentMap].PlatformTiles) {
				Physics::SetTransform(tile.Collider, glm::translate(glm::vec3(99, 99, 99)));
			}

			CurrentMap++;
			if (CurrentMap == AllMaps.size())
				CurrentMap = 0;

			GodEye.Ball.Position = AllMaps[CurrentMap].BallSpawn;
			GodEye.Ball.Velocity = glm::vec3(0, 0, 0);

			for (auto& tile : AllMaps[CurrentMap].PlatformTiles) {
				Physics::SetTransform(tile.Collider, tile.Transform);
			}

			GodEye.IsGameWon = false;
			GodEye.Score = 0;
			GodEye.Club.bIsMovingTowardBall = false;
			GodEye.bStartGame = true;
			GodEye.OldScores = Scoring::GetOldScore(AllMaps[CurrentMap].Name, GodEye.Name.size());
		}

		auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		this->window->Update();

		if (kbd->pressed[Input::Key::Code::End])
		{
			ShaderResource::ReloadShaders();
		}

		if (gamepad.Pressed[GolfInput::Gamepad::Button::X_BUTTON])
			DEBUGRealTimeUpdate = !DEBUGRealTimeUpdate;

		if (gamepad.Pressed[GolfInput::Gamepad::Button::Y_BUTTON] || DEBUGRealTimeUpdate)
			GodEye.Update(dt);
		else
		{
			if (gamepad.Pressed[GolfInput::Gamepad::Button::RIGHT_SHOULDER_BUTTON])
			{
				if (GodEye.HandleClubAndCameraInput == &PlayerCamera::UpdateCameraThirdPerson)
					GodEye.HandleClubAndCameraInput = &PlayerCamera::UpdateCameraTopDown;
				else
					GodEye.HandleClubAndCameraInput = &PlayerCamera::UpdateCameraThirdPerson;
			}
			(GodEye.*GodEye.HandleClubAndCameraInput)(dt);
		}

		//Debug::DrawBox(glm::translate(GodEye.Ball.Position + GodEye.Ball.Velocity * float(dt)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), {1,1,1,1});
   //     if (glm::length(GodEye.Ball.Velocity) > 0.1f)
			//Debug::DrawLine(GodEye.Ball.Position, GodEye.Ball.Position + glm::normalize(GodEye.Ball.Velocity) * (0.3f + glm::length(GodEye.Ball.Velocity * float(dt)) + GodEye.Ball.BallRadius), 2.0f, {0,1,0,1}, {0,1,0,1}, Debug::RenderMode::AlwaysOnTop);

		/// LENGTH --> glm::length(Velocity * dt) + BallRadius
		GodEye.CheckCollisions();
		//ship.CheckCollisions();

		// Store all drawcalls in the render device
		for (auto const& asteroid : asteroids) {
			RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
		}

		for (auto const& a : AllMaps[CurrentMap].PlatformTiles) {
			RenderDevice::Draw(a.Model, a.Transform);
		}

		if (GodEye.bStartGame) {
			std::cout << "Once " << std::endl;
			Scoring::GetOldScore(AllMaps[CurrentMap].Name, GodEye.Name.size());
			GodEye.bStartGame = false;
		}

		GodEye.Draw();
		if (GodEye.Ball.Position.y < 0.04) { GodEye.Ball.Position.y = 0.04f; }

		char CurrentTile = AllMaps[CurrentMap].SearchWhereAmI(GodEye.Ball.Position.x, GodEye.Ball.Position.z);
		//std::cout << GodEye.Ball.Position.y << std::endl;
		if (CurrentTile == 'h' || CurrentTile == 'H') {
			GodEye.Ball.CurrGravity = GodEye.LowGravity;

			/// WIN CONDITION
			if (GodEye.Ball.Position.y < 0.06) {
				GodEye.IsGameWon = true;
			}
		}
		else
			GodEye.Ball.CurrGravity = GodEye.LowGravity;

		GodEye.Ball.HeightOfTheLastFrameOfTheBall = GodEye.Ball.Position.y;
		std::vector<std::string> temp;

		if (ShowRanking)
		{
			UI::RenderHighScoreRanking(window->vg, GodEye, GodEye.OldScores, PlayerRank);

			if (gamepad.ANY_BUTTON)
			{
				ShowRanking = false;
				GodEye.ResetGame(AllMaps[CurrentMap].BallSpawn);
			}
		}
		else if (GodEye.IsGameWon) {
			if (Scoring::EnterHighScoreName(AllMaps[CurrentMap].Name, &gamepad, &GodEye))
			{
				ShowRanking = true;
				int Rank = 0;
				for (auto score : Scoring::GetScores(AllMaps[CurrentMap].Name, GodEye.Name.size()))
					if (GodEye.Score > score)
						Rank++;
				//while (Rank < GodEye.OldScores.size() && GodEye.Score < std::stoi(GodEye.OldScores[Rank].substr(11, 100)))
				PlayerRank = Rank;
				GodEye.OldScores = Scoring::GetOldScore(AllMaps[CurrentMap].Name, GodEye.Name.size());
			}
			UI::RenderEnterName(window->vg, GodEye);
		}
		else {
			UI::RenderCurrentScore(window->vg, GodEye);
			UI::RenderOldScore(window->vg, GodEye);
		}

		if (GodEye.Ball.ContinousRaycastPoints.size() != 0)
			for (int i = 0; i < GodEye.Ball.ContinousRaycastPoints.size() - 1; i++)
				Debug::DrawLine(GodEye.Ball.ContinousRaycastPoints[i], GodEye.Ball.ContinousRaycastPoints[i + 1], 2.0f, { 0,1,1,1 }, { 0,1,1,1 }, Debug::RenderMode::AlwaysOnTop);

		//Physics::DebugDrawColliders();

		// Execute the entire rendering pipeline
		RenderDevice::Render(this->window, dt);

		// transfer new frame to window
		this->window->SwapBuffers();

		auto timeEnd = std::chrono::steady_clock::now();
		dt = std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count());
		dtReal = std::chrono::duration<double>(timeEnd - timeStart).count();

		if (kbd->pressed[Input::Key::Code::Escape] || gamepad.Pressed[GolfInput::Gamepad::Button::START])
			this->Exit();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GolfGameApp::Exit()
{
	this->window->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
GolfGameApp::RenderUI()
{
	if (this->window->IsOpen())
	{


		//Debug::DispatchDebugTextDrawing();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GolfGameApp::RenderNanoVG(NVGcontext* vg)
{
	nvgSave(vg);

	nvgBeginPath(vg);
	NVGpaint paint;
	paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
	nvgFillPaint(vg, paint);
	nvgFill(vg);

	// Header
	nvgBeginPath(vg);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
	nvgStroke(vg);

	//nvgFontSize(vg, 16.0f);
	//nvgFontFace(vg, "sans");
	//nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	//nvgText(vg, 0, 30, "Testing, testing... Everything seems to be in order.", NULL);

	nvgRestore(vg);
}

} // namespace Game
