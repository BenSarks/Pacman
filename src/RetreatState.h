// ============================================
// Created by Ben Serkis
// ============================================

#pragma once
#include "State.h"

// Created by Ben Serkis
class RetreatState : public State
{
public:
	const char* Name() const override { return "Fleeing"; }
	void Execute(Pacman& pacman) override;
};
