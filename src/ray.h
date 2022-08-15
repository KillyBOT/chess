#ifndef RAY_H_
#define RAY_H_

#include <array>
#include <vector>

#include "pos.h"

const char kRayDirN = 0;
const char kRayDirW = 1;
const char kRayDirS = 2;
const char kRayDirE = 3;
const char kRayDirNW = 4;
const char kRayDirSW = 5;
const char kRayDirSE = 6;
const char kRayDirNE = 7;

extern std::vector<ChessPos> kRays[64][8];
extern std::vector<ChessPos> kKnightPositionTable[64];
extern int kRaysSize[64][8];

void initRayTable();
void initKnightPositionTable();

char opposite_dir(char dir);

#endif