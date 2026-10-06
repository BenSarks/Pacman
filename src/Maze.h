// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include <cstdlib>
#include "Constants.h"

// row 0 = bottom of the screen
// Created by Ben Serkis
struct GridPos
{
	int row = 0;
	int col = 0;

	bool operator==(const GridPos& other) const { return row == other.row && col == other.col; }
	bool operator!=(const GridPos& other) const { return !(*this == other); }
};

// Created by Ben Serkis
inline int ManhattanDistance(GridPos a, GridPos b)
{
	return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

// Created by Ben Serkis
class Maze
{
public:
	Maze();

	void Reset();

	static bool InBounds(GridPos p);
	bool IsWalkable(GridPos p) const;

	bool HasCoin(GridPos p) const;
	bool EatCoin(GridPos p);
	int CoinsLeft() const { return coinsLeft; }
	int TotalCoins() const { return totalCoins; }

private:
	bool coins[MAZE_ROWS][MAZE_COLS];
	int coinsLeft = 0;
	int totalCoins = 0;
};
