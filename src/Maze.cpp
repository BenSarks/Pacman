// ============================================
// Created by Ben Serkis
// ============================================

#include "Maze.h"

namespace
{
	constexpr int W = -1; // wall
	constexpr int C = 1; // coin

	// flipped vertically (row 0 is the bottom)
	constexpr int LAYOUT[MAZE_ROWS][MAZE_COLS] = {
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
		{W,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,W},
		{W,C,W,W,W,W,W,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,W,W,W,W,W,C,W},
		{W,C,W,W,W,W,W,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,W,W,W,W,W,C,W},
		{W,C,C,C,C,C,C,W,W,C,C,C,C,W,W,W,C,C,C,C,W,W,C,C,C,C,C,C,W},
		{W,W,W,C,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,C,W,W,W},
		{W,W,W,C,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,C,W,W,W},
		{W,C,C,C,W,W,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,W,W,C,C,C,W},
		{W,C,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,C,W},
		{W,C,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,C,W},
		{W,C,C,C,C,C,C,C,C,C,C,C,C,W,W,W,C,C,C,C,C,C,C,C,C,C,C,C,W},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,C,C,C,C,C,C,C,C,C,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{C,C,C,C,C,C,C,C,C,C,W,W,W,C,C,C,W,W,W,C,C,C,C,C,C,C,C,C,C},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,C,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,W,W,W,W,C,W,W,W,W,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,C,C,C,C,C,C,C,C,C,C,C,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,W,W},
		{W,W,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,W,W},
		{W,C,C,C,C,C,C,W,W,C,C,C,C,W,W,W,C,C,C,C,W,W,C,C,C,C,C,C,W},
		{W,C,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,C,W},
		{W,C,W,W,W,W,C,W,W,C,W,W,W,W,W,W,W,W,W,C,W,W,C,W,W,W,W,C,W},
		{W,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,W},
		{W,C,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,C,W},
		{W,C,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,C,W},
		{W,C,W,W,W,W,C,W,W,W,W,W,C,W,W,W,C,W,W,W,W,W,C,W,W,W,W,C,W},
		{W,C,C,C,C,C,C,C,C,C,C,C,C,W,W,W,C,C,C,C,C,C,C,C,C,C,C,C,W},
		{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
	};
}

Maze::Maze()
{
	Reset();
}

// Created by Ben Serkis
void Maze::Reset()
{
	coinsLeft = 0;
	for (int r = 0; r < MAZE_ROWS; r++)
	{
		for (int c = 0; c < MAZE_COLS; c++)
		{
			coins[r][c] = (LAYOUT[r][c] == C);
			if (coins[r][c])
				coinsLeft++;
		}
	}
	totalCoins = coinsLeft;
}

// Created by Ben Serkis
bool Maze::InBounds(GridPos p)
{
	return p.row >= 0 && p.row < MAZE_ROWS && p.col >= 0 && p.col < MAZE_COLS;
}

// Created by Ben Serkis
bool Maze::IsWalkable(GridPos p) const
{
	return InBounds(p) && LAYOUT[p.row][p.col] != W;
}

// Created by Ben Serkis
bool Maze::HasCoin(GridPos p) const
{
	return InBounds(p) && coins[p.row][p.col];
}

// Created by Ben Serkis
bool Maze::EatCoin(GridPos p)
{
	if (!HasCoin(p))
		return false;
	coins[p.row][p.col] = false;
	coinsLeft--;
	return true;
}
