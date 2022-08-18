#ifndef RAY_H_
#define RAY_H_

#include <array>
#include <vector>

#include "pos.h"
#include "bitboard.h"

const char kRayDirN = 0;
const char kRayDirW = 1;
const char kRayDirS = 2;
const char kRayDirE = 3;
const char kRayDirNW = 4;
const char kRayDirSW = 5;
const char kRayDirSE = 6;
const char kRayDirNE = 7;

extern ChessPos kRays[64][8][8];
extern ChessPos kKnightPositionTable[64][8];
extern int kRaySizes[64][8];
extern int kKnightPositionTableSize[64];

void init_ray_table();
void init_knight_position_table();

char opposite_dir(char dir);

#endif