// ============================================
// Created by Ben Serkis
// ============================================

#include "NPC.h"
#include <cmath>

NPC::NPC(GridPos start, Color color, float speed)
	: cell(start), x(static_cast<float>(start.col)), y(static_cast<float>(start.row)), color(color), speed(speed)
{
}

// Created by Ben Serkis
GridPos NPC::NearestCell() const
{
	return { static_cast<int>(std::lround(y)), static_cast<int>(std::lround(x)) };
}

// Created by Ben Serkis
void NPC::Update(float dt)
{
	animationTime += dt;
	float budget = speed * dt;

	// can pass more than one cell per tick, Think() is called on every cell
	for (int step = 0; step < 8 && budget > 0.0f; step++)
	{
		if (onCell)
		{
			OnCellReached();
			Think();
			if (path.empty())
				return;
		}

		const GridPos next = path.front();
		const float dx = next.col - x;
		const float dy = next.row - y;
		const float remaining = std::fabs(dx) + std::fabs(dy);

		facingCol = (dx > 0) - (dx < 0);
		facingRow = (dy > 0) - (dy < 0);

		if (remaining <= budget)
		{
			x = static_cast<float>(next.col);
			y = static_cast<float>(next.row);
			cell = next;
			path.erase(path.begin());
			onCell = true;
			budget -= remaining;
		}
		else
		{
			x += facingCol * budget;
			y += facingRow * budget;
			onCell = false;
			budget = 0.0f;
		}
	}
}
