//------------------------------------------------------------------------------
// spacegameapp.cc
// (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "golfball.h"
#include "spacegameapp.h"
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
#include "spaceship.h"
#include <iostream>
#include "playercamera.h"

using namespace Display;
using namespace Render;

namespace Game
{

//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::SpaceGameApp()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::~SpaceGameApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SpaceGameApp::Open()
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
SpaceGameApp::Run()
{
    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;

    //ModelId Square = LoadModel("asset/golf/square-corner-a.glb");
    ModelId Flag = LoadModel("assets/golf/flag-red.glb");
    ModelId HoleOpen = LoadModel("assets/golf/hole-open.glb");
    ModelId Corner = LoadModel("assets/golf/corner.glb");
    ModelId Start = LoadModel("assets/golf/start.glb");
    ModelId Castle = LoadModel("assets/golf/castle.glb");
    ModelId Open = LoadModel("assets/golf/open.glb");
    ModelId Side = LoadModel("assets/golf/side.glb");
    ModelId Windmill = LoadModel("assets/golf/windmill.glb");
    ModelId BlueGolfBall = LoadModel("assets/golf/ball-blue.glb");

    Physics::ColliderMeshId mHoleOpen = Physics::LoadColliderMesh("assets/golf/hole-open.glb");
    Physics::ColliderMeshId mStart = Physics::LoadColliderMesh("assets/golf/start.glb");
    Physics::ColliderMeshId mOpen = Physics::LoadColliderMesh("assets/golf/open.glb");
    Physics::ColliderMeshId mBlueGolfBall = Physics::LoadColliderMesh("assets/golf/ball-blue.glb");
    Physics::ColliderMeshId mWall = Physics::LoadColliderMesh("assets/golf/wallPhysicCollision.glb");

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
    Physics::ColliderMeshId colliderMeshes[6] = {
        Physics::LoadColliderMesh("assets/space/Asteroid_1_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_2_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_3_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_4_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_5_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_6_physics.glb")
    };

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
        glm::mat4 transform =  glm::rotate(rotation, rotationAxis) * glm::translate(translation) * glm::scale(glm::vec3(size, size, size));
        //std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
        //transform *= glm::scale(glm::vec3(size, size, size));
        std::get<2>(asteroid) = transform;
        asteroids.push_back(asteroid);
    }

    int width = 3;
    std::string Map = "cScSoSSoSSoSSoSSHScSc";
    // THIS SHIT IS BOTTOM UP
    // FROM THE SPACESHIP'S SPAWN PERSPECTIVE X INCREASES LEFT AND Y UP
    std::vector<int> Rotations = {  2,1,1,
                                    2,0,0,
                                    2,0,0,
                                    2,0,0,
                                    2,0,0,
                                    2,0,0,
                                    3,3,0 };
    int height;
    height = Map.length() / width;
    std::vector<std::tuple<ModelId, glm::mat4, Physics::ColliderId>> PlatformTiles;
    float offSet = 0.95;
    for (int x = 0; x < width; x++) 
    {
        for (int y = 0; y < height; y++)
        {
            glm::vec3 Location(x * offSet, 0, y * offSet);
            glm::mat4 Transform =  glm::translate(Location) * glm::rotate(Rotations[y * width + x] * 3.141592653f/2, glm::vec3(0,1,0));

            std::tuple<ModelId, glm::mat4, Physics::ColliderId> Tile;
            std::cout << "x: " << x << " width: " << width << " y: " << y << " = " << y * width + x << std::endl;
            if (Map[y * width + x] == 'c') {
                std::get<0>(Tile) = Corner;
				std::get<2>(Tile) = Physics::CreateCollider(mOpen, Transform);
				std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform * glm::rotate(-3.141592653f / 2 , glm::vec3(0,1,0)));
				std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform );
				//std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform * glm::rotate(-3.141592653f , glm::vec3(0,1,0)));
            }
            else if (Map[y * width + x] == 'C') {
                std::get<0>(Tile) = Castle;
                std::get<2>(Tile) = Physics::CreateCollider(mOpen, Transform);
            }
            else if (Map[y * width + x] == 'S') {
                std::get<0>(Tile) = Side;
                std::get<2>(Tile) = Physics::CreateCollider(mOpen, Transform);
                std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform * glm::translate(glm::vec3(0, 0, 0.1)));
                std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform * glm::translate(glm::vec3(0, 0, -0.1)));
				std::get<2>(Tile) = Physics::CreateCollider(mWall, Transform);
            }
            else if (Map[y * width + x] == 's') {
                std::get<0>(Tile) = Windmill;
                std::get<2>(Tile) = Physics::CreateCollider(mOpen, Transform);
            }
            else if (Map[y * width + x] == 'H') {
                std::get<0>(Tile) = HoleOpen;
                std::get<2>(Tile) = Physics::CreateCollider(mHoleOpen, Transform);
            }
            else if (Map[y * width + x] == 'o') {
                std::get<0>(Tile) = Open;
                std::get<2>(Tile) = Physics::CreateCollider(mOpen, Transform);
            }
            else
                std::cout << "somthing went wrong here" << std::endl;
            std::get<1>(Tile) = Transform;
            PlatformTiles.push_back(Tile);
            if (Map[y * width + x] == 'H')
            {
                std::get<0>(Tile) = Flag;
				std::get<1>(Tile) = Transform;
				PlatformTiles.push_back(Tile);
            }
        }
    }

    // Setup asteroids far
    for (int i = 0; i < 20; i++)
    {
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
        std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
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

    SpaceShip ship;
    ship.model = LoadModel("assets/space/spaceship.glb");
    PlayerCamera GodEye(glm::vec3(1,0.1,1), glm::vec3(0,2,0), glm::vec3(0,0,0));

    std::clock_t c_start = std::clock();
    double dt = 0.01667f;

    /// game loop
    while (this->window->IsOpen())
	{
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

        //ship.Update(dt);
        GodEye.Update(dt);
        GodEye.CheckCollisions();
        //ship.CheckCollisions();

        // Store all drawcalls in the render device
        for (auto const& asteroid : asteroids)
        {
            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
        }
        for (auto const& a : PlatformTiles)
        {
            RenderDevice::Draw(std::get<0>(a), std::get<1>(a));
        }
        GodEye.Draw();

        //RenderDevice::Draw(ship.model, ship.transform);
	
        // Execute the entire rendering pipeline
        RenderDevice::Render(this->window, dt);

		// transfer new frame to window
		this->window->SwapBuffers();

        auto timeEnd = std::chrono::steady_clock::now();
        dt = std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count());

        if (kbd->pressed[Input::Key::Code::Escape])
            this->Exit();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::Exit()
{
    this->window->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::RenderUI()
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
SpaceGameApp::RenderNanoVG(NVGcontext* vg)
{
    nvgSave(vg);

    nvgBeginPath(vg);
    //nvgCircle(vg, 600, 100, 50);
    NVGpaint paint;
    paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
    nvgFillPaint(vg, paint);
    nvgFill(vg);



    // Header
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStroke(vg);

    nvgFontSize(vg, 16.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
    //nvgText(vg, 0, 30, "Testing, testing... Everything seems to be in order.", NULL);

    nvgRestore(vg);
}

} // namespace Game
