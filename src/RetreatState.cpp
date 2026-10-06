// ============================================
// Created by Ben Serkis
// ============================================

#include "RetreatState.h"
#include <memory>
#include "Pacman.h"
#include "WanderingState.h"

// Created by Ben Serkis
void RetreatState::Execute(Pacman& pacman)
{
	if (pacman.DistanceToNearestGhost() > SAFE_RADIUS)
	{
		pacman.ChangeState(std::make_unique<WanderingState>());
		return;
	}
	pacman.PlanEscapeRoute();
}
