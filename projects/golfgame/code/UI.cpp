#include "UI.h"

void UI::DrawBackgroundBox(NVGcontext* vg, int X, int Y, int Width, int Height, int Opacity)
{
    nvgBeginPath(vg);
	nvgRect(vg, X, Y, Width, Height);
    NVGpaint paint;
    paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(0, 0, 0, Opacity), nvgRGBA(0, 0, 0, Opacity));
    nvgFillPaint(vg, paint);
    nvgFill(vg);
}

void UI::RenderEnterName(NVGcontext* vg, PlayerCamera pl)
{
	/// BOX BEHIND TEXT
    DrawBackgroundBox(vg, 550, 300, 400, 200, 200);

    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");

	nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
    nvgText(vg, 600, 380, "Enter Name: ", NULL);

	for (int i = 0; i < pl.Name.length(); i++)
	{
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
		if (pl.currentNameIndex == i)
			nvgFillColor(vg, nvgRGBA(255, 0, 0, 128));
		nvgText(vg, 675 + 50 * i, 450, pl.Name.substr(i,1).c_str(), NULL);
	}
}

void UI::RenderHighScoreRanking(NVGcontext* vg, PlayerCamera pl, std::vector<std::string> Scores, int PlayerScoreIndex)
{
    int XPOS = 650;
    int YPOS = 200;
    DrawBackgroundBox(vg, XPOS, YPOS, 650, (13 + 1) * 50 + 10, 200);

    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string sScore = std::to_string(pl.Score);
	const char* cScore = sScore.c_str();

    nvgText(vg, XPOS + 50, 300, "Highscores: ", NULL);
	int i = 350;
    for (int j = 0; j < Scores.size() && j < 10; j++) {
        if (j == PlayerScoreIndex)
			nvgFillColor(vg, nvgRGBA(255, 0, 0, 128));
		nvgText(vg, XPOS + 50, i, Scores[j].c_str(), NULL);
		i += 50;
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	}

	nvgFillColor(vg, nvgRGBA(255, 0, 0, 128));
    if (PlayerScoreIndex >= 10)
		nvgText(vg, XPOS + 50, 350 + 50 * 10, Scores[PlayerScoreIndex].c_str(), NULL);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	nvgText(vg, XPOS + 50, 350 + 50 * 11, "PRESS ANY BUTTON", NULL);


}

void UI::RenderCurrentScore(NVGcontext* vg, PlayerCamera pl)
{
	/// BOX BEHIND TEXT
    DrawBackgroundBox(vg, 40, 10, 300, 150, 100);

    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string sScore = std::to_string(pl.Score);
	const char* cScore = sScore.c_str();
    nvgText(vg, 60, 60, "Score: ", NULL);
    nvgText(vg, 80, 120, cScore, NULL);
}

void UI::RenderOldScore(NVGcontext* vg, PlayerCamera pl)
{
    const int MaxScoresShown = 7;
    DrawBackgroundBox(vg, 50, 250, 400, (MaxScoresShown + 1) * 50 + 10, 100);

    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string sScore = std::to_string(pl.Score);
	const char* cScore = sScore.c_str();

    nvgText(vg, 60, 300, "HighScore: ", NULL);
	int i = 350;
    int ScoresShown = 0;
	for (auto Score : pl.OldScores) {
		nvgText(vg, 60, i, Score.c_str(), NULL);
		i += 50;
        ScoresShown++;
        if (ScoresShown == MaxScoresShown)
            break;
	}
}

void UI::DrawFPS(NVGcontext* vg, float DeltaSeconds)
{
	/// BOX BEHIND TEXT
    DrawBackgroundBox(vg, 900, 10, 600, 150, 200);

    nvgFontSize(vg, 56.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	std::string Framerate = "FPS: " + std::to_string(1 / DeltaSeconds);
    nvgText(vg, 980, 120, Framerate.c_str(), NULL);
}

