#include "playercamera.h"
#include "config.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/debugrender.h"
#include <iostream>
#include <fstream>

PlayerCamera::PlayerCamera()
{
}

PlayerCamera::PlayerCamera(glm::vec3 pos, glm::vec3 offset, glm::vec3 rot) : Rotation(rot)
{
	Transform = glm::translate(pos);
	OffSet = offset;
	Position = OffSet;
	Ball = GolfBall(pos, "assets/golf/ball-blue.glb");
	Club = GolfClub(pos + glm::vec3(0, 0.7, -1), "assets/golf/club-blue.glb", 4);

	float minX, minY, maxX, maxY;
}

void PlayerCamera::Update(float DeltaSeconds)
{
	Ball.Update(DeltaSeconds);
	this->Club.Position.y = this->Ball.Position.y + 0.7f;

	Render::Camera* cam = Render::CameraManager::GetCamera(CAMERA_MAIN);
	Input::Keyboard* kbd = Input::GetDefaultKeyboard();

	if (kbd->held[Input::Key::W])
		Position.x += DeltaSeconds * MovementSpeed;

	if (kbd->held[Input::Key::S])
		Position.x -= DeltaSeconds * MovementSpeed;

	if (kbd->held[Input::Key::E])
		Position.y += DeltaSeconds * VerticleSpeed;

	if (kbd->held[Input::Key::Q])
		Position.y -= DeltaSeconds * VerticleSpeed;

	if (kbd->held[Input::Key::D])
		Position.z += DeltaSeconds * MovementSpeed;

	if (kbd->held[Input::Key::A])
		Position.z -= DeltaSeconds * MovementSpeed;

	/// GAMEPAD
	int count;
	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
	const unsigned char* Hit = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);


	/// DECLARE THIS SOMEWHERE ELSE LATER
	glm::vec3 BallPos = Ball.Position;
	glm::vec3 ClubPos = Club.Position;
	BallPos.y = 0;
	ClubPos.y = 0;

	/// IF THE ACTION BUTTON IS PRESSED GO TOWARDS TO BALL
	if (Hit[0] == GLFW_PRESS && !Club.bIsMovingTowardBall && (glm::length(Ball.Velocity) < 0.1)) {
		Club.bIsMovingTowardBall = true;
		DistanceFromClubToTheGolfBall = glm::distance(this->Ball.Position, this->Club.Position);
		BallHitDirection = glm::normalize(BallPos - ClubPos);
	}

	/// SWITCH CAMERA PERSPECTIVE
	if (Hit[5] == GLFW_PRESS && FirstPress) {
		FirstPress = false;
		ThirdPersonCam = !ThirdPersonCam;
		OrbitPoint = 0;
	}
	else if (Hit[5] == GLFW_RELEASE)
	{
		FirstPress = true;
	}

	if (!Club.bIsMovingTowardBall) {
		float Distance = glm::distance(BallPos, ClubPos);

		/// MOVE THE GOLF CLUB WITH GAMEPAD LEFT STICK
		if (!ThirdPersonCam)
		{
			Club.Position.x += DeltaSeconds * axes[1] * VerticleSpeed;
			Club.Position.z += DeltaSeconds * axes[0] * VerticleSpeed;
		}
		float MaxDistance = 2;
		if (Distance > MaxDistance) {
			glm::vec3 norm = glm::normalize((ClubPos - BallPos));
			norm *= MaxDistance - 0.01;
			Club.Position = Ball.Position + norm;
			Club.Position.y += 0.7f;
		}
		/// ROTATE THE CLUB TO POINT AT THE BALL
		Club.Rotation = glm::inverse(glm::lookAt(ClubPos, BallPos, glm::vec3(0, 1, 0))) * glm::rotate(3.141592f / 2, glm::vec3(0, 1, 0));
	}
	else {
		glm::vec3 norm = glm::normalize(BallPos - ClubPos);
		//BallHitDirection = norm;
		if (glm::distance(ClubPos, BallPos) > 0.1f) {
			// Club moving toward the ball
			Club.Position += norm * ClubMovementSpeed * DeltaSeconds;
			ClubPos = Club.Position;
			ClubPos.y = 0;
		}
		else {
			this->Ball.AddForce(BallHitDirection * 75.3f * (DistanceFromClubToTheGolfBall * DistanceFromClubToTheGolfBall));
			Club.bIsMovingTowardBall = false;
			Score++;
			std::cout << Score << std::endl;
		}
	}

	Club.Transform = glm::translate(Club.Position) * (glm::mat4)Club.Rotation;
	//OffSet.y += DeltaSeconds * -axes[1] * VerticleSpeed;

	if (!ThirdPersonCam)
	{
		OffSet = glm::vec3(0, 2, 0);

		/// MOVE THE CAMERA WITH THE RIGHT STICK
		Position.x += DeltaSeconds * axes[3] * MovementSpeed;
		Position.z += DeltaSeconds * axes[2] * MovementSpeed;

		if (axes[3] == 0 && axes[2] == 0) {
			Position = mix(Position, Ball.Position + OffSet, std::min(0.1f, DeltaSeconds * 30.0f));
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
	else
	{
		glm::vec3 TargetPosition; 

		OrbitPoint += DeltaSeconds * axes[2] * OrbitSpeed;

		/// GET INPUT IN THE FORM [0, 1]
		float ZoomInAmount = ((axes[5] + 1) / 2);
		float ZoomOutAmount = ((axes[4] + 1) / 2);

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

		Position = mix(Position, TargetPosition, std::min(0.1f, DeltaSeconds * 30.0f));

		cam->view = glm::lookAt(Position, Ball.Position, glm::vec3(0, 1, 0));

		/// PROJECTION EXAMPLE
		// glm::vec3 A;
		// glm::vec3 B;
		// glm::vec3 ProjAOnB = (glm::dot(A, B) / glm::length(B)) * B;

		glm::vec3 ClubMovement = glm::vec3(0, 0, 0);

		ClubMovement.x += DeltaSeconds * axes[0] * VerticleSpeed;
		ClubMovement.z -= DeltaSeconds * axes[1] * VerticleSpeed;


		Club.Position += glm::vec3(glm::inverse(cam->view) * glm::vec4(ClubMovement, 0));
	}

	this->Ball.HandlePhysics(DeltaSeconds);
}

void PlayerCamera::Draw()
{
	//std::cout << "x: " << Ball.Position.x << ", z: " << Ball.Position.z << std::endl;

	Debug::DrawDebugText("HsjflksdajflkasdjflksadjlkfsjlkdfasjfksadjfsadjkfsIT", Ball.Position + glm::vec3(0,0.2,0), glm::vec4(1, 1, 1, 1));
	Debug::DrawLine(Ball.Position, Ball.Position + glm::vec3(0, 0.2, 0), 1.0f, glm::vec4(1,0,1,1),glm::vec4(1,1,1,1));
	if (glm::length(this->Ball.Velocity) < 0.05) {
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

void PlayerCamera::EnterHighScoreName() {
	int count;
	const unsigned char* Hit = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
	// Select = 6
	bool Click = (Hit[10] == GLFW_PRESS || Hit[11] == GLFW_PRESS || Hit[12] == GLFW_PRESS || Hit[13] == GLFW_PRESS);
	if (!Click)
		bIsPress = false;
	if (!bIsPress) {
		bIsPress = true;
		if (Hit[12] == GLFW_PRESS) {
			Name[currentNameIndex]++;
			if (Name[currentNameIndex] > 'Z')
				Name[currentNameIndex] = 'A';
		}
		else if (Hit[11] == GLFW_PRESS) {
			currentNameIndex++;
			if (currentNameIndex >= Name.length())
				currentNameIndex = 0;
		}
		else if (Hit[10] == GLFW_PRESS) {
			Name[currentNameIndex]--;
			if (Name[currentNameIndex] < 'A')
				Name[currentNameIndex] = 'Z';
		}
		else if (Hit[13] == GLFW_PRESS) {
			currentNameIndex--;
			if (currentNameIndex < 0)
				currentNameIndex = Name.length()-1;
		}
		else if (Hit[0] == GLFW_PRESS) {
			WriteScoreToFile();
			IsGameWon = false;
			Score = 0;
			Name = "AAA";
			Ball.Position = glm::vec3(2, 1, 0);
		}
		else {
			bIsPress = false;
		}
	}
}

void PlayerCamera::WriteScoreToFile() {
	std::ofstream ScoreFile("highscores.txt", std::ios::app);
	ScoreFile << Name << ":" << Score << "\n";
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


