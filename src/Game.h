// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "Ghost.h"
#include "Maze.h"
#include "Pacman.h"

// Created by Ben Serkis
enum class GameResult
{
	Running,
	PacmanWins,
	GhostsWin
};

// Created by Ben Serkis
class Game
{
public:
	Game();

	void Reset();
	void Update(float dt);
	void Draw(bool showRoutes) const;

	Maze& GetMaze() { return maze; }
	const Maze& GetMaze() const { return maze; }
	const Pacman& GetPacman() const { return *pacman; }
	const std::vector<std::unique_ptr<Ghost>>& Ghosts() const { return ghosts; }

	std::vector<GridPos> ChasingGhostCells() const;
	const Ghost* FightingGhost() const;
	int GhostsRemaining() const;

	GameResult Result() const { return result; }
	float ElapsedSeconds() const { return elapsed; }
	const std::string& LastEvent() const { return lastEvent; }
	std::mt19937& Random() { return rng; }

private:
	void Log(const std::string& message);
	void DrawCoins() const;
	void DrawRoute(const NPC& npc, float alpha) const;

	Maze maze;
	std::unique_ptr<Pacman> pacman;
	std::vector<std::unique_ptr<Ghost>> ghosts;
	GameResult result = GameResult::Running;
	float elapsed = 0.0f;
	std::string lastEvent;
	std::mt19937 rng;
};
