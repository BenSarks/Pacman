// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include "State.h"

// Created by Ben Serkis
class WanderingState : public State
{
public:
	const char* Name() const override { return "Collecting coins"; }
	void Execute(Pacman& pacman) override;
};
