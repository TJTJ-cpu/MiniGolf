#include "playercamera.h"
#include "config.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/debugrender.h"
#include <iostream>
#include <map>
#include <fstream>
#include <ostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

PlayerCamera::PlayerCamera()
{
}

PlayerCamera::PlayerCamera(GolfInput::Gamepad* gm, glm::vec3 pos, glm::vec3 offset, glm::vec3 rot) : Rotation(rot), gamepad(gm)
{
	Transform = glm::translate(pos);
	OffSet = offset;
	Position = OffSet;
	Ball = GolfBall(pos, "assets/golf/ball-blue.glb");
	Club = GolfClub(pos + glm::vec3(0, 0.7, -1), "assets/golf/club-blue.glb", 4);
}

void PlayerCamera::Update(float DeltaSeconds)
{
	CurrentTime += DeltaSeconds;

	Ball.Update(DeltaSeconds);

	float ClubBallHeightOffset = 0.7f;
	Club.Position.y = Ball.Position.y + ClubBallHeightOffset;

	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	/// DECLARE THIS SOMEWHERE ELSE LATER
	glm::vec3 BallPos = Ball.Position;
	glm::vec3 ClubPos = Club.Position;
	BallPos.y = 0;
	ClubPos.y = 0;

	/// IF THE ACTION BUTTON IS PRESSED GO TOWARDS TO BALL
	if (gamepad->Pressed[GolfInput::Gamepad::Button::A_BUTTON] && !Club.bIsMovingTowardBall && (glm::length(Ball.Velocity) < 0.1)) {
		Club.bIsMovingTowardBall = true;
		DistanceFromClubToTheGolfBall = glm::distance(this->Ball.Position, this->Club.Position);
		BallHitDirection = glm::normalize(BallPos - ClubPos);
	}

	/// SWITCH CAMERA PERSPECTIVE
	if (gamepad->Pressed[GolfInput::Gamepad::Button::RIGHT_SHOULDER_BUTTON]) {
		if (HandleClubAndCameraInput == &PlayerCamera::UpdateCameraThirdPerson)
			HandleClubAndCameraInput = &PlayerCamera::UpdateCameraTopDown;
		else
			HandleClubAndCameraInput = &PlayerCamera::UpdateCameraThirdPerson;
		OrbitPoint = 0;
	}

	if (Club.bIsMovingTowardBall) {
		glm::vec3 norm = glm::normalize(BallPos - ClubPos);
		if (glm::distance(ClubPos, BallPos) > 0.1f) {
			Club.Position += norm * ClubMovementSpeed * DeltaSeconds;
			ClubPos = Club.Position;
			ClubPos.y = 0;
		}
		else {
			this->Ball.AddForce(BallHitDirection * 75.3f * (DistanceFromClubToTheGolfBall * DistanceFromClubToTheGolfBall));
			Club.bIsMovingTowardBall = false;
			Score++;
		}
	}

	/// A COMMAND PATTERN STYLE; HOPEFULLY PREFERABLE TO AN IF-ELSE
	(this->*HandleClubAndCameraInput)(DeltaSeconds);

	/// KEEP CLUB IN RANGE
	if (!Club.bIsMovingTowardBall) {
		float Distance = glm::distance(BallPos, ClubPos);

		float MaxDistance = 2;
		if (Distance > MaxDistance) {
			glm::vec3 norm = glm::normalize((ClubPos - BallPos));
			norm *= MaxDistance - 0.01;
			Club.Position = Ball.Position + norm;
			Club.Position.y += ClubBallHeightOffset;
		}
	}

	Club.Rotation = glm::inverse(glm::lookAt(ClubPos, BallPos, glm::vec3(0, 1, 0))) * glm::rotate(3.141592f / 2, glm::vec3(0, 1, 0));
	Club.Transform = glm::translate(Club.Position) * (glm::mat4)Club.Rotation;

	/// Guided Predictive Line
	glm::vec3 BallVelNotY = Ball.Velocity;
	BallVelNotY.y = 0;
	if (glm::length(BallVelNotY) == 0 && Ball.bGrounded)
		Debug::DrawLine(BallPos + glm::vec3(0,0.1f,0), glm::vec3(0,0.1f,0) + ClubPos + (BallPos - ClubPos) * 1.5f, 2.0f, {1,0,0,1}, {1,0,0,1});

	this->Ball.HandlePhysics(DeltaSeconds);
	//std::cout << "Ball grounded: " << std::boolalpha << Ball.bGrounded << "\n";
	//std::cout << "Ball Velocity: " << glm::length(Ball.Velocity) << "\n";
	//std::cout << "Ball Position: x: " << Ball.Position.x << ", y: " << Ball.Position.y << ", z: " << Ball.Position.z << "\n";
}

void PlayerCamera::Draw()
{
	Debug::DrawLine(Ball.Position, Ball.Position + glm::vec3(0, 0.2, 0), 1.0f, glm::vec4(1,0,1,1),glm::vec4(1,1,1,1));

	if (Ball.bGrounded && glm::length(this->Ball.Velocity) < 0.05) {
		this->Club.Draw();
	}
	else {
		this->Club.Position = this->Ball.Position + glm::vec3(0, 0, -1);
	}

	this->Ball.Draw();
}



void PlayerCamera::CheckCollisions()
{
	/// CHECK IF BALL IS COLLIDING WITH WALL OR FLOOR

	/// CHECK IF CLUB IS COLLIDING WITH WALL

}

void PlayerCamera::UpdateCameraThirdPerson(float dt)
{
	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	glm::vec3 TargetPosition; 

	OrbitPoint += dt * gamepad->RightStick.x * OrbitSpeed;

	/// GET INPUT IN THE FORM [0, 1]
	float ZoomInAmount = ((gamepad->RightTrigger + 1) / 2);
	float ZoomOutAmount = ((gamepad->LeftTrigger + 1) / 2);

	/// COOL WAY TO EXPRESS ZOOMING
	OrbitDistance = std::min(MaxOrbitDistance, std::max(MinOrbitDistance, OrbitDistance + ZoomSpeed * (ZoomOutAmount - ZoomInAmount)));

	/// BORING WAY
	//if (OrbitDistance < 5)
		//OrbitDistance += ZoomSpeed * ZoomOutAmount;
	//if (OrbitDistance > 1)
		//OrbitDistance -= ZoomSpeed * ZoomInAmount;

	OffSet = glm::vec3(0, OrbitDistance/2, 0);

	TargetPosition = Ball.Position + OffSet;
	TargetPosition.x -= glm::cos(OrbitPoint) * OrbitDistance;
	TargetPosition.z -= glm::sin(OrbitPoint) * OrbitDistance;

	Position = mix(Position, TargetPosition, std::min(0.1f, dt * 30.0f));

	cam->view = glm::lookAt(Position, Ball.Position, glm::vec3(0, 1, 0));

	glm::vec3 ClubMovement = glm::vec3(0, 0, 0);

	ClubMovement.x += dt * gamepad->LeftStick.x * VerticleSpeed;
	ClubMovement.z -= dt * gamepad->LeftStick.y * VerticleSpeed;


	Club.Position += glm::vec3(glm::inverse(cam->view) * glm::vec4(ClubMovement, 0));
}

void PlayerCamera::UpdateCameraTopDown(float dt)
{
	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);

	OffSet = glm::vec3(0, 2, 0);

	/// MOVE THE CAMERA WITH THE RIGHT STICK
	Position.x += dt * gamepad->RightStick.y * MovementSpeed;
	Position.z += dt * gamepad->RightStick.x * MovementSpeed;

	/// MOVE THE GOLF CLUB WITH GAMEPAD LEFT STICK
	if (!Club.bIsMovingTowardBall)
	{
		Club.Position.x += dt * gamepad->LeftStick.y * VerticleSpeed;
		Club.Position.z += dt * gamepad->LeftStick.x * VerticleSpeed;
	}

	if (gamepad->RightStick.x == 0 && gamepad->RightStick.y == 0) {
		Position = mix(Position, Ball.Position + OffSet, std::min(0.1f, dt * 30.0f));
	}
	else {
		float CameraMoveableRadius = 2;
		if (glm::length(Position - (Ball.Position + OffSet)) > CameraMoveableRadius) {
			glm::vec3 norm = glm::normalize((Position - (Ball.Position + OffSet)));
			norm.y = 0;
			norm *= CameraMoveableRadius;
			Position = Ball.Position + OffSet + norm;
		}
	}
	cam->view = glm::lookAt(Position, Position + glm::vec3(0, -1, 0), glm::vec3(1, 0, 0));
}

void PlayerCamera::EnterHighScoreName(std::string MapName) {
	if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_DOWN]) {
		Name[currentNameIndex]++;
		if (Name[currentNameIndex] > 'Z')
			Name[currentNameIndex] = 'A';
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_RIGHT]) {
		currentNameIndex++;
		if (currentNameIndex >= Name.length())
			currentNameIndex = 0;
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_UP]) {
		Name[currentNameIndex]--;
		if (Name[currentNameIndex] < 'A')
			Name[currentNameIndex] = 'Z';
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_LEFT]) {
		currentNameIndex--;
		if (currentNameIndex < 0)
			currentNameIndex = Name.length() - 1;
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::A_BUTTON]) {
		WriteScoreToFile(MapName);
		IsGameWon = false;
		Score = 0;
		Name = "AAA";
		Ball.Position = glm::vec3(2, 1, 0);
	}
}

void PlayerCamera::WriteScoreToFile(std::string MapName) {
	std::ofstream ScoreFile(MapName + "_highscores.txt", std::ios::app);
	ScoreFile << Name << " " << Score << "\n";
	ScoreFile.close();
}

void PlayerCamera::RenderHighScore(NVGcontext* vg) {
    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	const char* CName = Name.c_str();
	std::string Hat = "     ";
	Hat[currentNameIndex * 2] = '^';
	const char* cHat = Hat.c_str();
    nvgText(vg, 600, 380, "YOU WIN!!!!", NULL);
    nvgText(vg, 600, 430, CName, NULL);
    nvgText(vg, 600, 480, cHat, NULL);
}

void PlayerCamera::RenderScore(NVGcontext* vg) {
    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string sScore = std::to_string(Score);
	const char* cScore = sScore.c_str();
    nvgText(vg, 60, 60, "Score: ", NULL);
    nvgText(vg, 80, 120, cScore, NULL);
}

void PlayerCamera::RenderOldScore(NVGcontext* vg) {
    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string sScore = std::to_string(Score);
	const char* cScore = sScore.c_str();

    nvgText(vg, 60, 300, "HighScore: ", NULL);
	int i = 350;
	for (auto Score : OldScores) {
		nvgText(vg, 60, i, Score.c_str(), NULL);
		i += 50;
	}

}

std::vector<std::string> PlayerCamera::GetOldScore(std::string MapName) {
	std::vector<std::string> Scores;

	std::string Line;
	std::ifstream MyFile(MapName + "_highscores.txt");

	if (!MyFile.is_open()) {
		return Scores;
	}

	std::vector<std::string> HighscoreName;
	std::vector<int> Highscore;

	while (std::getline(MyFile, Line)) {
		std::string ScoreName = Line.substr(0, Name.size());
		int Score = std::stoi(Line.substr(Name.length() + 1, Line.length() - 1));

		int insertPoint = 0;
		while (insertPoint < Highscore.size() && Highscore[insertPoint] < Score)
			insertPoint++;

		/// INSERT THE SCORES AT THE PROPER POINT IN THE LISTS
		Highscore.insert(Highscore.begin() + insertPoint, Score);
		HighscoreName.insert(HighscoreName.begin() + insertPoint, ScoreName);
	}

	MyFile.close();

	/// TAKES THOSE TWO LISTS WE HAD EARLIER AND MERGE THEM AND PUT THEM IN A NEW LIST AS C STYLE STRINGS
	for (int i = 0; i < HighscoreName.size(); i++)
	{
		Scores.push_back(std::to_string(i + 1) + ". " + HighscoreName[i] + "......" + std::to_string(Highscore[i]));
		if (Scores.size() == 7)
			break;
	}

	for (auto score : Scores)
		std::cout << score << "\n";

	return Scores;
}
