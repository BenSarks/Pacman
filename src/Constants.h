// ============================================
// Created by Ben Serkis
// ============================================

#pragma once

// maze & window
constexpr int MAZE_ROWS = 31;
constexpr int MAZE_COLS = 29;

constexpr int MAZE_WIDTH_PX = 696; // size of the maze image
constexpr int MAZE_HEIGHT_PX = 744;
constexpr int HUD_HEIGHT_PX = 44;

constexpr int WINDOW_WIDTH = MAZE_WIDTH_PX;
constexpr int WINDOW_HEIGHT = MAZE_HEIGHT_PX + HUD_HEIGHT_PX;

constexpr float CELL_WIDTH = static_cast<float>(MAZE_WIDTH_PX) / MAZE_COLS;
constexpr float CELL_HEIGHT = static_cast<float>(MAZE_HEIGHT_PX) / MAZE_ROWS;

// timing
constexpr int TICK_MS = 16;
constexpr float TICK_SECONDS = TICK_MS / 1000.0f;

constexpr float PACMAN_SPEED = 5.0f; // cells/sec
constexpr float GHOST_SPEED = 4.6f; // cells/sec

// rules
// 2 ghosts on pacman at the same time = game over
constexpr float FIGHT_DURATION = 3.0f;
constexpr float CATCH_DISTANCE = 0.6f;

// AI
constexpr int DANGER_RADIUS = 5; // start running away
constexpr int SAFE_RADIUS = 8; // stop running away
constexpr int FLEE_HORIZON = 10;
constexpr int GHOST_AVOID_COST = 6;
constexpr int SHARED_ROUTE_COST = 4;
constexpr int AMBUSH_LOOKAHEAD = 4;
