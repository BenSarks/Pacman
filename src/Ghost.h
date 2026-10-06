// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include <string>
#include "NPC.h"

class Game;

// Created by Ben Serkis
enum class GhostMode
{
	Chasing,
	Fighting,
	Defeated
};

// Created by Ben Serkis
class Ghost : public NPC
{
public:
	// lookahead > 0 -> targets cells ahead of pacman
	Ghost(GridPos start, Color color, std::string name, int lookahead, Game& game);

	void Draw() const override;

	const std::string& Name() const { return name; }
	GhostMode Mode() const { return mode; }
	bool IsChasing() const { return mode == GhostMode::Chasing; }

	void StartFight();
	void UpdateFight(float dt);

protected:
	void Think() override;

private:
	GridPos ChooseTarget() const;

	Game& game;
	std::string name;
	int lookahead;
	GhostMode mode = GhostMode::Chasing;
	float fightTimeLeft = 0.0f;
};
