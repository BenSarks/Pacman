// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include "Maze.h"
#include "Pathfinding.h"

// Created by Ben Serkis
struct Color
{
	float r, g, b;
};

// Created by Ben Serkis
class NPC
{
public:
	NPC(GridPos start, Color color, float speed);
	virtual ~NPC() = default;

	void Update(float dt);
	virtual void Draw() const = 0;

	GridPos Cell() const { return cell; }
	GridPos NearestCell() const;
	float X() const { return x; }
	float Y() const { return y; }
	const Path& GetPath() const { return path; }
	Color GetColor() const { return color; }

protected:
	virtual void Think() = 0;
	virtual void OnCellReached() {}

	void SetPath(Path newPath) { path = std::move(newPath); }

	float CenterX() const { return (x + 0.5f) * CELL_WIDTH; }
	float CenterY() const { return (y + 0.5f) * CELL_HEIGHT; }

	GridPos cell;
	float x, y;
	int facingRow = 0, facingCol = -1;
	float animationTime = 0.0f;

private:
	Color color;
	float speed;
	Path path;
	bool onCell = true;
};
