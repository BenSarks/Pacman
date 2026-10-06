// ============================================
// Created by Ben Serkis
// ============================================

#include "WanderingState.h"
#include <memory>
#include "Pacman.h"
#include "RetreatState.h"

// Created by Ben Serkis
void WanderingState::Execute(Pacman& pacman)
{
	if (pacman.DistanceToNearestGhost() <= DANGER_RADIUS)
	{
		pacman.ChangeState(std::make_unique<RetreatState>());
		return;
	}
	pacman.PlanRouteToNearestCoin();
}
