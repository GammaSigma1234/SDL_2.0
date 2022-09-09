/**
 * @file GlobConst.hpp
 * @brief 
 * 
 * 
 * 
 **/

#ifndef GLOBCONST_H_
#define GLOBCONST_H_

static constexpr int INIT_FIRST_ONE = -1; // Initialise the first rendering driver supporting the requested flags

static constexpr int WINDOW_W_px = 1920; // Screen's width
static constexpr int WINDOW_H_px = 1200; // Screen's heigth

static constexpr double GRAVITY       (0.5);
static constexpr double BOUNCE_FACTOR (0.8);
static constexpr double BRAKING_FACTOR(1.0);

// The dimensions of the level
static constexpr int LEVEL_W_px = 3200;
static constexpr int LEVEL_H_px = 2000;

static constexpr int WALL_W_px = 64;
static constexpr int WALL_H_px = 64;

#endif // GLOBCONST_H_