#pragma once

#include "Scoring.h"
#include <fstream>

using std::vector;
using std::pair;
using std::string;
using namespace GolfInput;

vector<pair<string, int>> Scoring::LoadHighscores(string MapName, int NameLength)
{
	vector<pair<string, int>> Scores;

	string Line;
	std::ifstream MyFile(MapName + "_highscores.txt");

	if (!MyFile.is_open()) {
		return Scores;
	}

	vector<string> HighscoreName;
	vector<int> Highscore;

	while (std::getline(MyFile, Line)) {
		string ScoreName = Line.substr(0, NameLength);
		int Score = std::stoi(Line.substr(NameLength + 1, Line.length() - 1));

		int insertPoint = 0;
		while (insertPoint < Highscore.size() && Highscore[insertPoint] < Score)
			insertPoint++;

		/// INSERT THE SCORES AT THE PROPER POINT IN THE LISTS
		Highscore.insert(Highscore.begin() + insertPoint, Score);
		HighscoreName.insert(HighscoreName.begin() + insertPoint, ScoreName);
	}

	MyFile.close();

	/// TAKES THOSE TWO LISTS WE HAD EARLIER AND MERGE THEM
	for (int i = 0; i < HighscoreName.size(); i++)
	{
		Scores.push_back({ HighscoreName[i],Highscore[i] });
	}

	return Scores;
}

vector<string> Scoring::GetOldScore(string MapName, int NameLength) {
	vector<string> Scores;

	int i = 0;
	for (auto score : LoadHighscores(MapName, NameLength))
	{
		Scores.push_back(std::to_string(i + 1) + ". " + score.first + "......" + std::to_string(score.second));
		i++;
	}

	return Scores;
}


vector<int> Scoring::GetScores(string MapName, int NameLength)
{
	vector<int> Scores;

	for (auto score : LoadHighscores(MapName, NameLength))
		Scores.push_back(score.second);

	return Scores;
}

void Scoring::WriteScoreToFile(string MapName, string Name, int Score) {
	std::ofstream ScoreFile(MapName + "_highscores.txt", std::ios::app);
	ScoreFile << Name << " " << Score << "\n";
	ScoreFile.close();
}

bool Scoring::EnterHighScoreName(string MapName, GolfInput::Gamepad* gamepad, PlayerCamera* pl) {
	if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_DOWN]) {
		pl->Name[pl->currentNameIndex]++;
		if (pl->Name[pl->currentNameIndex] > 'Z')
			pl->Name[pl->currentNameIndex] = 'A';
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_RIGHT]) {
		pl->currentNameIndex++;
		if (pl->currentNameIndex >= pl->Name.length())
			pl->currentNameIndex = 0;
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_UP]) {
		pl->Name[pl->currentNameIndex]--;
		if (pl->Name[pl->currentNameIndex] < 'A')
			pl->Name[pl->currentNameIndex] = 'Z';
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::DPAD_LEFT]) {
		pl->currentNameIndex--;
		if (pl->currentNameIndex < 0)
			pl->currentNameIndex = pl->Name.length() - 1;
	}
	else if (gamepad->Pressed[GolfInput::Gamepad::Button::A_BUTTON]) {
		WriteScoreToFile(MapName, pl->Name, pl->Score);
		return true;
	}
	return false;
}

