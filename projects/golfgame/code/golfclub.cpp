#include "golfclub.h"

GolfClub::GolfClub(glm::vec3 pos, const char* mod, float dis) : Position(pos), DistanceFromGolfBall(dis) {
	Transform = glm::translate(pos);
	Rotation = glm::quat();
	Model = Render::LoadModel(mod);
}


GolfClub::GolfClub(){
	Rotation = glm::quat();
	DistanceFromGolfBall = 4;
	Position = glm::vec3(0,0,0),
	Transform = glm::translate(Position);
	Model = Render::LoadModel("assets/golf/club-blue.glb");
}

void GolfClub::Draw()
{
	Render::RenderDevice::Draw(Model, Transform);
}
