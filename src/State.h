// ============================================
// Created by Ben Serkis
// ============================================

#pragma once

class Pacman;

// Created by Ben Serkis
class State
{
public:
	virtual ~State() = default;

	virtual const char* Name() const = 0;
	virtual void OnEnter(Pacman&) {}
	virtual void Execute(Pacman& pacman) = 0;
	virtual void OnExit(Pacman&) {}
};
