#pragma once

#include "playercamera.h"

namespace UI
{
	void DrawBackgroundBox(NVGcontext* vg, int X, int Y, int Width, int Height, int Opacity);
	void RenderEnterName(NVGcontext* vg, PlayerCamera pl);
	void RenderHighScoreRanking(NVGcontext* vg, PlayerCamera pl, std::vector<std::string> Scores, int PlayerScoreIndex);
	void RenderCurrentScore(NVGcontext* vg, PlayerCamera pl);
	void RenderOldScore(NVGcontext* vg, PlayerCamera pl);
	void DrawFPS(NVGcontext* vg, float DeltaSeconds);
}
