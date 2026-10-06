// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include <array>
#include <functional>
#include <vector>
#include "Maze.h"

// path without the start cell
using Path = std::vector<GridPos>;

using StepCostFn = std::function<int(GridPos)>;

using CanEnterFn = std::function<bool(GridPos, int depth)>;

// returns empty path if no path found
Path FindPathAStar(const Maze& maze, GridPos start, GridPos goal, const StepCostFn& extraCost = nullptr);

// Created by Ben Serkis
class DistanceMap
{
public:
	static constexpr int UNREACHABLE = -1;

	DistanceMap();

	int Distance(GridPos p) const;
	bool Reachable(GridPos p) const { return Distance(p) != UNREACHABLE; }

	Path PathTo(GridPos target) const;

private:
	friend DistanceMap BreadthFirstSearch(const Maze&, const std::vector<GridPos>&, int, const CanEnterFn&);

	static int Index(GridPos p) { return p.row * MAZE_COLS + p.col; }

	std::array<int, MAZE_ROWS * MAZE_COLS> distance;
	std::array<int, MAZE_ROWS * MAZE_COLS> parent;
};

// maxDepth = -1 means no limit
DistanceMap BreadthFirstSearch(const Maze& maze, const std::vector<GridPos>& sources,
	int maxDepth = -1, const CanEnterFn& canEnter = nullptr);

std::vector<GridPos> Neighbours(const Maze& maze, GridPos p);
