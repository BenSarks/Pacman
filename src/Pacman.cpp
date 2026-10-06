// ============================================
// Created by Ben Serkis
// ============================================

#include "Pacman.h"
#include <climits>
#include <cmath>
#include <random>
#include "Game.h"
#include "Ghost.h"
#include "WanderingState.h"
#include <GL/glut.h>

namespace
{
	constexpr Color PACMAN_YELLOW = { 1.0f, 1.0f, 0.0f };
	constexpr int COIN_POINTS = 10;
	constexpr float TWO_PI = 6.2831853f;
}

Pacman::Pacman(GridPos start, Game& game)
	: NPC(start, PACMAN_YELLOW, PACMAN_SPEED), game(game), state(std::make_unique<WanderingState>())
{
	state->OnEnter(*this);
}

// Created by Ben Serkis
void Pacman::ChangeState(std::unique_ptr<State> next)
{
	pendingState = std::move(next);
}

// Created by Ben Serkis
void Pacman::Think()
{
	state->Execute(*this);
	if (pendingState)
	{
		state->OnExit(*this);
		state = std::move(pendingState);
		state->OnEnter(*this);
		state->Execute(*this);
	}
}

// Created by Ben Serkis
void Pacman::OnCellReached()
{
	if (game.GetMaze().EatCoin(cell))
		score += COIN_POINTS;
}

// Created by Ben Serkis
int Pacman::DistanceToNearestGhost() const
{
	std::vector<GridPos> ghostCells = game.ChasingGhostCells();
	if (ghostCells.empty())
		return INT_MAX;
	DistanceMap fromGhosts = BreadthFirstSearch(game.GetMaze(), ghostCells);
	int distance = fromGhosts.Distance(cell);
	return distance == DistanceMap::UNREACHABLE ? INT_MAX : distance;
}

// Created by Ben Serkis
int Pacman::GhostAvoidanceCost(GridPos p) const
{
	int cost = 0;
	for (const GridPos& ghostCell : game.ChasingGhostCells())
	{
		int distance = ManhattanDistance(p, ghostCell);
		if (distance < DANGER_RADIUS)
			cost += GHOST_AVOID_COST * (DANGER_RADIUS - distance);
	}
	return cost;
}

// BFS to find closest coin, then A* to it (with penalty near ghosts)
// Created by Ben Serkis
void Pacman::PlanRouteToNearestCoin()
{
	const Maze& maze = game.GetMaze();
	DistanceMap fromPacman = BreadthFirstSearch(maze, { cell });

	std::vector<GridPos> nearestCoins;
	int bestDistance = INT_MAX;
	for (int r = 0; r < MAZE_ROWS; r++)
	{
		for (int c = 0; c < MAZE_COLS; c++)
		{
			GridPos p{ r, c };
			int d = fromPacman.Distance(p);
			if (!maze.HasCoin(p) || d == DistanceMap::UNREACHABLE)
				continue;
			if (d < bestDistance)
			{
				bestDistance = d;
				nearestCoins.clear();
			}
			if (d == bestDistance)
				nearestCoins.push_back(p);
		}
	}

	if (nearestCoins.empty())
	{
		SetPath({});
		return;
	}

	std::uniform_int_distribution<size_t> pick(0, nearestCoins.size() - 1);
	GridPos target = nearestCoins[pick(game.Random())];
	SetPath(FindPathAStar(maze, cell, target, [this](GridPos p) { return GhostAvoidanceCost(p); }));
}

// BFS from ghosts + BFS from pacman that only goes to cells he reaches first
// Created by Ben Serkis
void Pacman::PlanEscapeRoute()
{
	const Maze& maze = game.GetMaze();
	DistanceMap fromGhosts = BreadthFirstSearch(maze, game.ChasingGhostCells());

	auto ghostDistance = [&fromGhosts](GridPos p) {
		int d = fromGhosts.Distance(p);
		return d == DistanceMap::UNREACHABLE ? MAZE_ROWS * MAZE_COLS : d;
	};

	DistanceMap safeArea = BreadthFirstSearch(maze, { cell }, FLEE_HORIZON,
		[&ghostDistance](GridPos p, int depth) { return depth < ghostDistance(p); });

	GridPos best = cell;
	int bestScore = INT_MIN;
	for (int r = 0; r < MAZE_ROWS; r++)
	{
		for (int c = 0; c < MAZE_COLS; c++)
		{
			GridPos p{ r, c };
			if (safeArea.Distance(p) <= 0)
				continue;
			int candidateScore = ghostDistance(p) * 10;
			if (maze.HasCoin(p))
				candidateScore += 3;
			if (Neighbours(maze, p).size() == 1)
				candidateScore -= 40; // dead end
			if (candidateScore > bestScore)
			{
				bestScore = candidateScore;
				best = p;
			}
		}
	}

	if (best != cell)
	{
		SetPath(safeArea.PathTo(best));
		return;
	}

	// no safe cell - just move away
	Path step;
	int bestGhostDistance = -1;
	for (const GridPos& n : Neighbours(maze, cell))
	{
		if (ghostDistance(n) > bestGhostDistance)
		{
			bestGhostDistance = ghostDistance(n);
			step = { n };
		}
	}
	SetPath(step);
}

// Created by Ben Serkis
void Pacman::Draw() const
{
	Color c = GetColor();
	// flash when fighting
	if (const Ghost* opponent = game.FightingGhost())
	{
		if (static_cast<int>(animationTime * 8.0f) % 2 == 0)
			c = opponent->GetColor();
	}

	const float radius = CELL_WIDTH * 0.45f;
	const float mouth = 0.08f + 0.6f * std::fabs(std::sin(animationTime * 10.0f));
	const float facing = std::atan2(static_cast<float>(facingRow), static_cast<float>(facingCol));
	const int segments = 32;

	glColor3f(c.r, c.g, c.b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(CenterX(), CenterY());
	for (int i = 0; i <= segments; i++)
	{
		float a = facing + mouth + (TWO_PI - 2.0f * mouth) * i / segments;
		glVertex2f(CenterX() + radius * std::cos(a), CenterY() + radius * std::sin(a));
	}
	glEnd();
}
