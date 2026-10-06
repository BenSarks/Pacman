// ============================================
// Created by Ben Serkis
// ============================================

#include "Pathfinding.h"
#include <algorithm>
#include <climits>
#include <queue>

namespace
{
	const GridPos DIRECTIONS[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };

	constexpr int CELL_COUNT = MAZE_ROWS * MAZE_COLS;

	int ToIndex(GridPos p) { return p.row * MAZE_COLS + p.col; }
	GridPos FromIndex(int i) { return { i / MAZE_COLS, i % MAZE_COLS }; }

	// Created by Ben Serkis
	struct OpenNode
	{
		int f;
		int h;
		int index;

		// min heap by f, tie -> smaller h
		bool operator<(const OpenNode& other) const
		{
			if (f != other.f)
				return f > other.f;
			return h > other.h;
		}
	};
}

// Created by Ben Serkis
std::vector<GridPos> Neighbours(const Maze& maze, GridPos p)
{
	std::vector<GridPos> result;
	result.reserve(4);
	for (const GridPos& d : DIRECTIONS)
	{
		GridPos n{ p.row + d.row, p.col + d.col };
		if (maze.IsWalkable(n))
			result.push_back(n);
	}
	return result;
}

// Created by Ben Serkis
Path FindPathAStar(const Maze& maze, GridPos start, GridPos goal, const StepCostFn& extraCost)
{
	if (start == goal || !maze.IsWalkable(start) || !maze.IsWalkable(goal))
		return {};

	std::vector<int> g(CELL_COUNT, INT_MAX);
	std::vector<int> parent(CELL_COUNT, -1);
	std::vector<bool> closed(CELL_COUNT, false);
	std::priority_queue<OpenNode> open;

	const int startIndex = ToIndex(start);
	const int goalIndex = ToIndex(goal);
	g[startIndex] = 0;
	open.push({ ManhattanDistance(start, goal), ManhattanDistance(start, goal), startIndex });

	while (!open.empty())
	{
		OpenNode current = open.top();
		open.pop();

		// already handled (duplicate in queue)
		if (closed[current.index])
			continue;
		closed[current.index] = true;

		if (current.index == goalIndex)
		{
			Path path;
			for (int i = goalIndex; i != startIndex; i = parent[i])
				path.push_back(FromIndex(i));
			std::reverse(path.begin(), path.end());
			return path;
		}

		const GridPos pos = FromIndex(current.index);
		for (const GridPos& next : Neighbours(maze, pos))
		{
			const int nextIndex = ToIndex(next);
			if (closed[nextIndex])
				continue;

			int stepCost = 1 + (extraCost ? std::max(0, extraCost(next)) : 0);
			int newG = g[current.index] + stepCost;
			if (newG < g[nextIndex])
			{
				g[nextIndex] = newG;
				parent[nextIndex] = current.index;
				int h = ManhattanDistance(next, goal);
				open.push({ newG + h, h, nextIndex });
			}
		}
	}
	return {};
}

DistanceMap::DistanceMap()
{
	distance.fill(UNREACHABLE);
	parent.fill(-1);
}

// Created by Ben Serkis
int DistanceMap::Distance(GridPos p) const
{
	return Maze::InBounds(p) ? distance[Index(p)] : UNREACHABLE;
}

// Created by Ben Serkis
Path DistanceMap::PathTo(GridPos target) const
{
	Path path;
	if (!Reachable(target))
		return path;
	for (int i = Index(target); parent[i] != -1; i = parent[i])
		path.push_back(FromIndex(i));
	std::reverse(path.begin(), path.end());
	return path;
}

// Created by Ben Serkis
DistanceMap BreadthFirstSearch(const Maze& maze, const std::vector<GridPos>& sources,
	int maxDepth, const CanEnterFn& canEnter)
{
	DistanceMap map;
	std::queue<GridPos> frontier;

	for (const GridPos& s : sources)
	{
		if (maze.IsWalkable(s) && map.distance[DistanceMap::Index(s)] == DistanceMap::UNREACHABLE)
		{
			map.distance[DistanceMap::Index(s)] = 0;
			frontier.push(s);
		}
	}

	while (!frontier.empty())
	{
		GridPos current = frontier.front();
		frontier.pop();
		const int depth = map.distance[DistanceMap::Index(current)];
		if (maxDepth >= 0 && depth >= maxDepth)
			continue;

		for (const GridPos& next : Neighbours(maze, current))
		{
			const int nextIndex = DistanceMap::Index(next);
			if (map.distance[nextIndex] != DistanceMap::UNREACHABLE)
				continue;
			if (canEnter && !canEnter(next, depth + 1))
				continue;
			map.distance[nextIndex] = depth + 1;
			map.parent[nextIndex] = DistanceMap::Index(current);
			frontier.push(next);
		}
	}
	return map;
}
