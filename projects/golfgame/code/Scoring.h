#pragma once

#include "playercamera.h"
#include "gamepad.h"	

namespace Scoring
{
	using std::vector;
	using std::pair;
	using std::string;
	using namespace GolfInput;

	vector<pair<string, int>> LoadHighscores(string MapName, int NameLength);
	vector<string> GetOldScore(string MapName, int NameLength);
	vector<int> GetScores(string MapName, int NameLength);

	void WriteScoreToFile(string MapName, string Name, int Score);
	bool EnterHighScoreName(string MapName, Gamepad* gamepad, PlayerCamera* pl);

}
