// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include <memory>
#include "NPC.h"
#include "State.h"

class Game;

// Created by Ben Serkis
class Pacman : public NPC
{
public:
	Pacman(GridPos start, Game& game);

	void Draw() const override;

	// state is switched at the end of Think()
	void ChangeState(std::unique_ptr<State> next);
	const char* StateName() const { return state->Name(); }
	int Score() const { return score; }

	int DistanceToNearestGhost() const;
	void PlanRouteToNearestCoin();
	void PlanEscapeRoute();

protected:
	void Think() override;
	void OnCellReached() override;

private:
	int GhostAvoidanceCost(GridPos p) const;

	Game& game;
	std::unique_ptr<State> state;
	std::unique_ptr<State> pendingState;
	int score = 0;
};
