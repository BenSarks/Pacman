// ============================================
// Created by Ben Serkis
// ============================================

#include "Ghost.h"
#include <algorithm>
#include <array>
#include <cmath>
#include "Game.h"
#include "Pacman.h"
#include <GL/glut.h>

namespace
{
	constexpr float TWO_PI = 6.2831853f;

	// Created by Ben Serkis
	void DrawDisc(float cx, float cy, float radius, float startAngle = 0.0f, float endAngle = TWO_PI)
	{
		const int segments = 24;
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(cx, cy);
		for (int i = 0; i <= segments; i++)
		{
			float a = startAngle + (endAngle - startAngle) * i / segments;
			glVertex2f(cx + radius * std::cos(a), cy + radius * std::sin(a));
		}
		glEnd();
	}
}

Ghost::Ghost(GridPos start, Color color, std::string name, int lookahead, Game& game)
	: NPC(start, color, GHOST_SPEED), game(game), name(std::move(name)), lookahead(lookahead)
{
}

// Created by Ben Serkis
void Ghost::StartFight()
{
	mode = GhostMode::Fighting;
	fightTimeLeft = FIGHT_DURATION;
	SetPath({});
}

// Created by Ben Serkis
void Ghost::UpdateFight(float dt)
{
	if (mode != GhostMode::Fighting)
		return;
	fightTimeLeft -= dt;
	if (fightTimeLeft <= 0.0f)
		mode = GhostMode::Defeated;
}

// Created by Ben Serkis
GridPos Ghost::ChooseTarget() const
{
	const Pacman& pacman = game.GetPacman();
	const GridPos pacmanCell = pacman.NearestCell();

	// pinky aims ahead of pacman
	if (lookahead > 0 && ManhattanDistance(cell, pacmanCell) > lookahead)
	{
		const Path& route = pacman.GetPath();
		if (!route.empty())
			return route[std::min(route.size(), static_cast<size_t>(lookahead)) - 1];
	}
	return pacmanCell;
}

// Created by Ben Serkis
void Ghost::Think()
{
	if (mode != GhostMode::Chasing)
	{
		SetPath({});
		return;
	}

	// cells used by the other ghosts
	std::array<bool, MAZE_ROWS * MAZE_COLS> claimed{};
	for (const auto& other : game.Ghosts())
	{
		if (other.get() == this || !other->IsChasing())
			continue;
		for (const GridPos& p : other->GetPath())
			claimed[p.row * MAZE_COLS + p.col] = true;
	}

	SetPath(FindPathAStar(game.GetMaze(), cell, ChooseTarget(), [&claimed](GridPos p) {
		return claimed[p.row * MAZE_COLS + p.col] ? SHARED_ROUTE_COST : 0;
	}));
}

// Created by Ben Serkis
void Ghost::Draw() const
{
	if (mode != GhostMode::Chasing)
		return;

	const Color c = GetColor();
	const float r = CELL_WIDTH * 0.45f;
	const float cx = CenterX();
	const float cy = CenterY();
	const float headY = cy + r * 0.15f;
	const float skirtY = cy - r * 0.65f;

	glColor3f(c.r, c.g, c.b);
	DrawDisc(cx, headY, r, 0.0f, TWO_PI / 2.0f);
	glRectf(cx - r, skirtY, cx + r, headY);

	const float wiggle = (static_cast<int>(animationTime * 8.0f) % 2) ? r * 0.12f : 0.0f;
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 3; i++)
	{
		float left = cx - r + i * (2.0f * r / 3.0f);
		float right = left + 2.0f * r / 3.0f;
		glVertex2f(left, skirtY);
		glVertex2f(right, skirtY);
		glVertex2f((left + right) / 2.0f + wiggle, cy - r);
	}
	glEnd();

	const float eyeOffsetX = r * 0.38f;
	const float eyeY = cy + r * 0.2f;
	const float lookX = facingCol * r * 0.12f;
	const float lookY = facingRow * r * 0.12f;
	for (float side : { -1.0f, 1.0f })
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		DrawDisc(cx + side * eyeOffsetX, eyeY, r * 0.28f);
		glColor3f(0.1f, 0.2f, 0.9f);
		DrawDisc(cx + side * eyeOffsetX + lookX, eyeY + lookY, r * 0.13f);
	}
}
