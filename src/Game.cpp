// ============================================
// Created by Ben Serkis
// ============================================

#include "Game.h"
#include <cmath>
#include <iostream>
#include <GL/glut.h>

namespace
{
	const GridPos PACMAN_START = { 1, 14 };

	// Created by Ben Serkis
	struct GhostSpawn
	{
		const char* name;
		GridPos start;
		Color color;
		int lookahead;
	};

	const GhostSpawn GHOST_SPAWNS[] = {
		{ "Blinky", { 7, 6 },   { 1.0f, 0.0f, 0.0f },   0 },
		{ "Pinky",  { 16, 14 }, { 1.0f, 0.72f, 0.85f }, AMBUSH_LOOKAHEAD },
		{ "Inky",   { 29, 27 }, { 0.0f, 1.0f, 1.0f },   0 },
	};

	constexpr Color COIN_COLOR = { 0.83f, 0.69f, 0.22f };
	constexpr float COIN_SIZE = 6.0f;
}

Game::Game()
	: rng(std::random_device{}())
{
	Reset();
}

// Created by Ben Serkis
void Game::Reset()
{
	maze.Reset();
	pacman = std::make_unique<Pacman>(PACMAN_START, *this);
	ghosts.clear();
	for (const GhostSpawn& spawn : GHOST_SPAWNS)
		ghosts.push_back(std::make_unique<Ghost>(spawn.start, spawn.color, spawn.name, spawn.lookahead, *this));
	result = GameResult::Running;
	elapsed = 0.0f;
	Log("New game started");
}

// Created by Ben Serkis
void Game::Log(const std::string& message)
{
	lastEvent = message;
	std::cout << "[" << static_cast<int>(elapsed) << "s] " << message << '\n';
}

// Created by Ben Serkis
void Game::Update(float dt)
{
	if (result != GameResult::Running)
		return;

	elapsed += dt;
	const std::string stateBefore = pacman->StateName();

	pacman->Update(dt);
	for (auto& ghost : ghosts)
	{
		if (ghost->IsChasing())
			ghost->Update(dt);
	}

	if (stateBefore != pacman->StateName())
		Log(std::string("Pac-Man: ") + pacman->StateName());

	for (auto& ghost : ghosts)
	{
		if (ghost->Mode() != GhostMode::Fighting)
			continue;
		ghost->UpdateFight(dt);
		if (ghost->Mode() == GhostMode::Defeated)
			Log(ghost->Name() + " lost the fight");
	}

	int fighting = 0;
	for (auto& ghost : ghosts)
	{
		if (ghost->IsChasing())
		{
			float dx = ghost->X() - pacman->X();
			float dy = ghost->Y() - pacman->Y();
			if (std::sqrt(dx * dx + dy * dy) < CATCH_DISTANCE)
			{
				ghost->StartFight();
				Log(ghost->Name() + " caught Pac-Man!");
			}
		}
		if (ghost->Mode() == GhostMode::Fighting)
			fighting++;
	}

	if (fighting >= 2)
	{
		result = GameResult::GhostsWin;
		Log("The ghosts win - two of them caught Pac-Man at once");
	}
	else if (maze.CoinsLeft() == 0)
	{
		result = GameResult::PacmanWins;
		Log("Pac-Man wins - all coins collected");
	}
	else if (GhostsRemaining() == 0)
	{
		result = GameResult::PacmanWins;
		Log("Pac-Man wins - every ghost was defeated");
	}
}

// Created by Ben Serkis
std::vector<GridPos> Game::ChasingGhostCells() const
{
	std::vector<GridPos> cells;
	for (const auto& ghost : ghosts)
	{
		if (ghost->IsChasing())
			cells.push_back(ghost->NearestCell());
	}
	return cells;
}

// Created by Ben Serkis
const Ghost* Game::FightingGhost() const
{
	for (const auto& ghost : ghosts)
	{
		if (ghost->Mode() == GhostMode::Fighting)
			return ghost.get();
	}
	return nullptr;
}

// Created by Ben Serkis
int Game::GhostsRemaining() const
{
	int count = 0;
	for (const auto& ghost : ghosts)
	{
		if (ghost->Mode() != GhostMode::Defeated)
			count++;
	}
	return count;
}

// Created by Ben Serkis
void Game::Draw(bool showRoutes) const
{
	DrawCoins();

	if (showRoutes)
	{
		for (const auto& ghost : ghosts)
		{
			if (ghost->IsChasing())
				DrawRoute(*ghost, 0.55f);
		}
		DrawRoute(*pacman, 0.8f);
	}

	for (const auto& ghost : ghosts)
		ghost->Draw();
	pacman->Draw();
}

// Created by Ben Serkis
void Game::DrawCoins() const
{
	glColor3f(COIN_COLOR.r, COIN_COLOR.g, COIN_COLOR.b);
	glBegin(GL_QUADS);
	for (int r = 0; r < MAZE_ROWS; r++)
	{
		for (int c = 0; c < MAZE_COLS; c++)
		{
			if (!maze.HasCoin({ r, c }))
				continue;
			float cx = (c + 0.5f) * CELL_WIDTH;
			float cy = (r + 0.5f) * CELL_HEIGHT;
			float h = COIN_SIZE / 2.0f;
			glVertex2f(cx - h, cy - h);
			glVertex2f(cx + h, cy - h);
			glVertex2f(cx + h, cy + h);
			glVertex2f(cx - h, cy + h);
		}
	}
	glEnd();
}

// Created by Ben Serkis
void Game::DrawRoute(const NPC& npc, float alpha) const
{
	const Path& route = npc.GetPath();
	if (route.empty())
		return;

	const Color c = npc.GetColor();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(3.0f);
	glColor4f(c.r, c.g, c.b, alpha);

	glBegin(GL_LINE_STRIP);
	glVertex2f((npc.X() + 0.5f) * CELL_WIDTH, (npc.Y() + 0.5f) * CELL_HEIGHT);
	for (const GridPos& p : route)
		glVertex2f((p.col + 0.5f) * CELL_WIDTH, (p.row + 0.5f) * CELL_HEIGHT);
	glEnd();

	const GridPos& goal = route.back();
	const float gx = (goal.col + 0.5f) * CELL_WIDTH;
	const float gy = (goal.row + 0.5f) * CELL_HEIGHT;
	glBegin(GL_LINE_LOOP);
	glVertex2f(gx - 7, gy - 7);
	glVertex2f(gx + 7, gy - 7);
	glVertex2f(gx + 7, gy + 7);
	glVertex2f(gx - 7, gy + 7);
	glEnd();

	glLineWidth(1.0f);
	glDisable(GL_BLEND);
}
